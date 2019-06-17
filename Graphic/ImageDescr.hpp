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

#pragma once

#include <vulkan/vulkan.h>
#include <map>

class QVulkanInstance;
class QVulkanDeviceFunctions;

class ImageDescr
{
public:
    ///
    ///  Device should be created from provided physicalDevice
    ///
    ImageDescr(QVulkanInstance &vulkanInstance, VkDevice device, VkPhysicalDevice physicalDev);
    ~ImageDescr();

    bool createImage(VkFormat pixelFormat, VkExtent3D imageSize, uint32_t mipLevels, const uint8_t* data,
                     bool generateMipMaps, VkImageUsageFlags usage);
    VkImage getImage() const { return mImage; }
    VkDeviceMemory getMem() const { return mMem; }

    ImageDescr(const ImageDescr&) = delete;
    ImageDescr& operator=(const ImageDescr&) = delete;

    ImageDescr(ImageDescr&&);
    ImageDescr& operator=(ImageDescr&&);

protected:
    void release();
    void swapAll(ImageDescr&&);

    VkImage mImage = nullptr;
    VkDeviceMemory mMem = nullptr;
    VkDevice mDevice = nullptr;
    QVulkanDeviceFunctions *mDevFuncs = nullptr;

    static std::map<VkDevice, VkPhysicalDeviceMemoryProperties> sMemPropMap;

};

