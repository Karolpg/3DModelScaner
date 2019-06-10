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

#include "SamplerDescr.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

SamplerDescr::SamplerDescr(QVulkanInstance &vulkanInstance, VkDevice device)
    : mDevice(device)
{
    assert(mDevice && "Device should be valid!");
    mDevFuncs = vulkanInstance.deviceFunctions(mDevice);
    assert(mDevFuncs && "Device functions should be valid!");

    QVulkanFunctions *vulkanFunc = vulkanInstance.functions();
    assert(vulkanFunc && "Vulkan instance functions should be valid!");
}

SamplerDescr::~SamplerDescr()
{
    release();
}

bool SamplerDescr::createSampler(const VkSamplerCreateInfo& samplerInfo)
{
    VkResult result = mDevFuncs->vkCreateSampler(mDevice, &samplerInfo, nullptr, &mSampler);
    if (result != VK_SUCCESS) {
        qWarning("Can't create image\n");
        return false;
    }
    return true;
}

void SamplerDescr::release()
{
    mDevFuncs->vkDestroySampler(mDevice, mSampler, nullptr);
    mSampler = nullptr;
}

SamplerDescr::SamplerDescr(SamplerDescr&& other)
{
    swapAll(std::move(other));
}

SamplerDescr& SamplerDescr::operator=(SamplerDescr&& other)
{
    swapAll(std::move(other));
    return *this;
}

void SamplerDescr::swapAll(SamplerDescr&& other)
{
    std::swap(mSampler, other.mSampler);
    std::swap(mDevice, other.mDevice);
    std::swap(mDevFuncs, other.mDevFuncs);
}
