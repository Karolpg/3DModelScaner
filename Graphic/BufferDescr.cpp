/*
MIT License

Copyright (c) 2019 Karolpg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "BufferDescr.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

std::map<VkDevice, VkPhysicalDeviceMemoryProperties> BufferDescr::sMemPropMap;

BufferDescr::BufferDescr(QVulkanInstance& vulkanInstance, VkDevice device, VkPhysicalDevice physicalDev)
    : mDevice(device)
{
    assert(mDevice && "Device should be valid!");
    mDevFuncs = vulkanInstance.deviceFunctions(mDevice);
    assert(mDevFuncs && "Device functions should be valid!");

    QVulkanFunctions *vulkanFunc = vulkanInstance.functions();
    assert(vulkanFunc && "Vulkan instance functions should be valid!");
    assert(physicalDev && "Physical device should be valid!");
    VkPhysicalDeviceMemoryProperties& memProperties = sMemPropMap[device];
    vulkanFunc->vkGetPhysicalDeviceMemoryProperties(physicalDev, &memProperties);
}

BufferDescr::~BufferDescr()
{
    release();
}

void BufferDescr::release()
{
    mDevFuncs->vkDestroyBuffer(mDevice, mBuffer, nullptr);
    mDevFuncs->vkFreeMemory(mDevice, mMem, nullptr);
    mBuffer = nullptr;
    mMem = nullptr;
}

BufferDescr::BufferDescr(BufferDescr&& other)
{
    swapAll(std::move(other));
}

BufferDescr& BufferDescr::operator=(BufferDescr&& other)
{
    swapAll(std::move(other));
    return *this;
}

void BufferDescr::swapAll(BufferDescr&& other)
{
    std::swap(mMem, other.mMem);
    std::swap(mBuffer, other.mBuffer);
    std::swap(mDevice, other.mDevice);
    std::swap(mDevFuncs, other.mDevFuncs);
}

bool BufferDescr::createBuffer(const void* data, size_t dataSize, VkBufferUsageFlags usage)
{
    release();
    VkResult result = VK_SUCCESS;

    //
    // Buffer description
    //
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = dataSize;
    //bufferInfo.flags = ; // for sparse buffers, additional protection, capture/replay
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //bufferInfo.pQueueFamilyIndices = ; //only when sharingMode == VK_SHARING_MODE_CONCURRENT
    //bufferInfo.queueFamilyIndexCount = ;

    result = mDevFuncs->vkCreateBuffer(mDevice, &bufferInfo, nullptr, &mBuffer);
    if (result != VK_SUCCESS) {
        qWarning("Can't create buffer\n");
        return false;
    }

    //
    // Memory requirements for this buffer
    //
    VkMemoryRequirements memReqs;
    mDevFuncs->vkGetBufferMemoryRequirements(mDevice, mBuffer, &memReqs);

    //
    // Suitable memory type available on physical device
    //
    uint32_t memoryTypeIndex = ~0u;
    {
        VkPhysicalDeviceMemoryProperties& physDevMemProps = sMemPropMap[mDevice];

        uint32_t memoryPropertyFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT     // allow write by host
                                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                    | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & (1 << i)) && (physDevMemProps.memoryTypes[i].propertyFlags & memoryPropertyFlag) == memoryPropertyFlag) {
                memoryTypeIndex = i;
                break;
            }
        }
        if (memoryTypeIndex == ~0u) {
            qWarning("Can't find memory type for buffer\n");
            return false;
        }
    }

    //
    // Memory allocation
    //
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    result = mDevFuncs->vkAllocateMemory(mDevice, &allocInfo, nullptr, &mMem);
    if (result != VK_SUCCESS) {
        qWarning("Can't allocate memory for buffer\n");
        return false;
    }

    //
    // Copy from host
    //
    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMapped = nullptr;
    mDevFuncs->vkMapMemory(mDevice, mMem, offset, memReqs.size, mappingFlags, &deviceMemMapped);

    memcpy(deviceMemMapped, data, dataSize);

    mDevFuncs->vkUnmapMemory(mDevice, mMem);

    result = mDevFuncs->vkBindBufferMemory(mDevice, mBuffer, mMem, offset);
    if (result != VK_SUCCESS) {
        qWarning("Can't bind memory to buffer\n");
        return false;
    }
    return true;
}

