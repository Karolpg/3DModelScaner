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
#include "ResourceManager.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

BufferDescr::BufferDescr(ResourceManager* resourceMgr)
    : mResourceMgr(resourceMgr)
{
    assert(mResourceMgr && "Resource Manager should be valid!");
}

BufferDescr::~BufferDescr()
{
    release();
}

void BufferDescr::release()
{
    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();
    devFuncs->vkDestroyBuffer(device, mBuffer, nullptr);
    devFuncs->vkFreeMemory(device, mMem, nullptr);
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
    std::swap(mResourceMgr, other.mResourceMgr);
}

bool BufferDescr::createBuffer(const void* data, size_t dataSize, VkBufferUsageFlags usage)
{
    release();
    VkResult result = VK_SUCCESS;

    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();

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

    result = devFuncs->vkCreateBuffer(device, &bufferInfo, nullptr, &mBuffer);
    if (result != VK_SUCCESS) {
        qWarning("Can't create buffer\n");
        return false;
    }

    //
    // Memory requirements for this buffer
    //
    VkMemoryRequirements memReqs;
    devFuncs->vkGetBufferMemoryRequirements(device, mBuffer, &memReqs);

    //
    // Suitable memory type available on physical device
    //
    uint32_t memoryTypeIndex = ~0u;
    {
        const VkPhysicalDeviceMemoryProperties& physDevMemProps = mResourceMgr->phyDevMemProps();

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
    result = devFuncs->vkAllocateMemory(device, &allocInfo, nullptr, &mMem);
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
    devFuncs->vkMapMemory(device, mMem, offset, memReqs.size, mappingFlags, &deviceMemMapped);

    memcpy(deviceMemMapped, data, dataSize);

    devFuncs->vkUnmapMemory(device, mMem);

    result = devFuncs->vkBindBufferMemory(device, mBuffer, mMem, offset);
    if (result != VK_SUCCESS) {
        qWarning("Can't bind memory to buffer\n");
        return false;
    }
    return true;
}

