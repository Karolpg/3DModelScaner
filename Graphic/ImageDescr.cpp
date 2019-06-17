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

#include "ImageDescr.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

std::map<VkDevice, VkPhysicalDeviceMemoryProperties> ImageDescr::sMemPropMap;

ImageDescr::ImageDescr(QVulkanInstance& vulkanInstance, VkDevice device, VkPhysicalDevice physicalDev)
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

ImageDescr::~ImageDescr()
{
    release();
}

void ImageDescr::release()
{
    mDevFuncs->vkDestroyImage(mDevice, mImage, nullptr);
    mDevFuncs->vkFreeMemory(mDevice, mMem, nullptr);
    mImage = nullptr;
    mMem = nullptr;
}

ImageDescr::ImageDescr(ImageDescr&& other)
{
    swapAll(std::move(other));
}

ImageDescr& ImageDescr::operator=(ImageDescr&& other)
{
    swapAll(std::move(other));
    return *this;
}

void ImageDescr::swapAll(ImageDescr&& other)
{
    std::swap(mMem, other.mMem);
    std::swap(mImage, other.mImage);
    std::swap(mDevice, other.mDevice);
    std::swap(mDevFuncs, other.mDevFuncs);
}

bool ImageDescr::createImage(VkFormat pixelFormat, VkExtent3D imageSize, uint32_t mipLevels, const uint8_t* data,
                             bool generateMipMaps, VkImageUsageFlags usage)
{
    release();
    VkResult result = VK_SUCCESS;

    //
    // Image description
    //
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    //imageInfo.flags = ; // sparse bit, mutable, additional protection, compatible
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = pixelFormat;
    imageInfo.extent = imageSize;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; //TODO can be provided from VulkanWindow
    imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //imageInfo.queueFamilyIndexCount; //only when sharingMode == VK_SHARING_MODE_CONCURRENT
    //imageInfo.pQueueFamilyIndices;
    //imageInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

    result = mDevFuncs->vkCreateImage(mDevice, &imageInfo, nullptr, &mImage);
    if (result != VK_SUCCESS) {
        qWarning("Can't create image\n");
        return false;
    }

    //
    // Memory requirements for this image
    //
    VkMemoryRequirements memReqs;
    mDevFuncs->vkGetImageMemoryRequirements(mDevice, mImage, &memReqs);

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
            qWarning("Can't find memory type for image\n");
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
        qWarning("Can't allocate memory for image\n");
        return false;
    }

    //
    // Generate mip maps
    //
    uint32_t pixelSize = 4; // TODO should be taken from format
    std::vector<uint8_t> mipMaps;
    if (generateMipMaps && mipLevels > 1) {
        uint32_t mipMapsSize = 0;
        VkExtent3D mipSize = imageSize;
        for (uint32_t i = 1; i < mipLevels; ++i) {
            mipSize.width  /= 2;
            mipSize.height /= 2;
            if (mipSize.depth > 1) {
                mipSize.depth  /= 2;
            }
            if (mipSize.width < 2 || mipSize.height < 2) { //TODO check this condition !!!
                if (i + 1 < mipLevels) {
                    mipLevels = i + 1;
                    qWarning("Mip map levels is to big. There is no possibility to devide it more.");
                }
                break;
            }
            mipMapsSize += mipSize.width * mipSize.height * mipSize.depth * pixelSize;
        }
        mipMaps.resize(mipMapsSize);

        uint32_t mipMapsOffset = 0;
        const uint8_t* prevMipMapData = data;
        mipSize = imageSize;
        for (uint32_t i = 1; i < mipLevels; ++i) {
            mipSize.width  /= 2;
            mipSize.height /= 2;
            if (mipSize.depth > 1) {
                mipSize.depth  /= 2;
            }
            if (mipSize.width < 2 || mipSize.height < 2) {
                break;
            }

            uint8_t* mipMapData = &mipMaps[mipMapsOffset];

            //TODO finish generating mipmaps maybe interpolation
            //mipMapPtr[] = prevMipMapData[]
            assert(!"Generating mip maps not finished");

            prevMipMapData = mipMapData;
            mipMapsOffset += mipSize.width * mipSize.height * mipSize.depth * pixelSize;
        }
    }
    //TODO check below how to use sbresource layout during mipmap generation
//    VkImageSubresource subresource = {};
//    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    for (uint32_t al = 0; al < imageInfo.arrayLayers; ++al) {
//        for (uint32_t ml = 0; ml < mipLevels; ++ml) {
//            subresource.mipLevel = ml;
//            subresource.arrayLayer = al;

//            VkSubresourceLayout srlayout;
//            mDevFuncs->vkGetImageSubresourceLayout(mDevice, mImage, &subresource, &srlayout);
//        }
//    }



    //
    // Copy from host
    //
    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMapped = nullptr;
    mDevFuncs->vkMapMemory(mDevice, mMem, offset, memReqs.size, mappingFlags, &deviceMemMapped);

#if 0
    uint32_t imgSize = imageSize.width * imageSize.height * imageSize.depth * pixelSize;
    memcpy(deviceMemMapped, data, imgSize);
    if (!mipMaps.empty()) {
        uint8_t* deviceMemMipMaps = static_cast<uint8_t*>(deviceMemMapped) + imgSize;
        memcpy(deviceMemMipMaps, data, mipMaps.size());
    }
#else
    VkImageSubresource subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    for (uint32_t al = 0; al < imageInfo.arrayLayers; ++al) {
        for (uint32_t ml = 0; ml < mipLevels; ++ml) {
            subresource.mipLevel = ml;
            subresource.arrayLayer = al;

            VkSubresourceLayout srlayout;
            mDevFuncs->vkGetImageSubresourceLayout(mDevice, mImage, &subresource, &srlayout);

            qInfo("Coping image [al=%d][ml=%d]. Offset: %d, Size: %lld, rowPitch: %lld,  arrayPitch: %lld,  depthPitch: %lld\n"
                  , al, ml
                  , srlayout.offset, srlayout.size
                  , srlayout.rowPitch, srlayout.arrayPitch, srlayout.depthPitch);

            //memcpy(static_cast<uint8_t*>(deviceMemMapped) + srlayout.offset, data + srlayout.offset, srlayout.size);

            uint32_t dataOffset = 0; // src mipmap offset
            uint32_t dataRowPitch = imageSize.width * pixelSize; // mipmap row pitch
            for (uint32_t h = 0; h < imageSize.height; ++h) {
                memcpy(static_cast<uint8_t*>(deviceMemMapped) + srlayout.offset + h * srlayout.rowPitch, // dst
                       data + dataOffset + h * dataRowPitch, // src
                       dataRowPitch); // size
            }
        }
    }
#endif

    mDevFuncs->vkUnmapMemory(mDevice, mMem);

    result = mDevFuncs->vkBindImageMemory(mDevice, mImage, mMem, offset);
    if (result != VK_SUCCESS) {
        qWarning("Can't bind memory to image\n");
        return false;
    }

    return true;
}

