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

#include "ImageDescr.hpp"
#include "ImageViewDescr.hpp"
#include "SamplerDescr.hpp"
#include "BufferDescr.hpp"
#include <memory>
#include <vector>
#include <map>
#include <vulkan/vulkan.h>

class QVulkanInstance;
class QVulkanDeviceFunctions;

class ResourceManager 
{
public:
    ///
    /// Device should be created from provided physicalDevice
    ///
    ResourceManager(QVulkanInstance &vulkanInstance,
                    VkDevice device,
                    VkPhysicalDevice physicalDev);

    ///
    /// Below create... functions return object which ownership is ResourceManager
    ///

    ImageDescr* createImage();
    ImageViewDescr* createImageView();
    SamplerDescr* createSampler();
    BufferDescr* createBuffer();

    const QVulkanInstance& vulkanInstance() const;
    QVulkanDeviceFunctions* deviceFunctions() const;
    VkDevice device() const;
    VkPhysicalDevice physicalDevice() const;
    const VkPhysicalDeviceMemoryProperties& phyDevMemProps() const;

private:
    std::vector<std::unique_ptr<BufferDescr>> mBuffers;
    std::vector<std::unique_ptr<ImageDescr>> mImages;
    std::vector<std::unique_ptr<ImageViewDescr>> mImageViews;
    std::vector<std::unique_ptr<SamplerDescr>> mSamplers;

    QVulkanInstance &mVulkanInstance;
    QVulkanDeviceFunctions *mDevFuncs;
    VkDevice mDevice;
    VkPhysicalDevice mPhysicalDev;
    static std::map<VkDevice, VkPhysicalDeviceMemoryProperties> sMemPropMap; // TODO make it thread safe
};

