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

#include "ResourceManager.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>
#include <assert.h>

std::map<VkDevice, VkPhysicalDeviceMemoryProperties> ResourceManager::sMemPropMap;

ResourceManager::ResourceManager(QVulkanInstance &vulkanInstance,
                                 VkDevice device,
                                 VkPhysicalDevice physicalDev)
    : mVulkanInstance(vulkanInstance)
    , mDevice(device)
    , mPhysicalDev(physicalDev)
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

ImageDescr* ResourceManager::createImage()
{
    mImages.emplace_back(new ImageDescr(this));
    return mImages.back().get();
}

ImageViewDescr* ResourceManager::createImageView()
{
    mImageViews.emplace_back(new ImageViewDescr(this));
    return mImageViews.back().get();
}

SamplerDescr* ResourceManager::createSampler()
{
    mSamplers.emplace_back(new SamplerDescr(this));
    return mSamplers.back().get();
}

BufferDescr* ResourceManager::createBuffer()
{
    mBuffers.emplace_back(new BufferDescr(this));
    return mBuffers.back().get();
}

const QVulkanInstance& ResourceManager::vulkanInstance() const
{
    return mVulkanInstance;
}

QVulkanDeviceFunctions* ResourceManager::deviceFunctions() const
{
    return mDevFuncs;
}

VkDevice ResourceManager::device() const
{
    return mDevice;
}

VkPhysicalDevice ResourceManager::physicalDevice() const
{
    return mPhysicalDev;
}

const VkPhysicalDeviceMemoryProperties& ResourceManager::phyDevMemProps() const
{
    return sMemPropMap[mDevice];
}
