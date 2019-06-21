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
#include "ResourceManager.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

SamplerDescr::SamplerDescr(ResourceManager* resourceMgr)
    : mResourceMgr(resourceMgr)
{
    assert(mResourceMgr && "Resource Manager should be valid!");
}

SamplerDescr::~SamplerDescr()
{
    release();
}

bool SamplerDescr::createSampler(const VkSamplerCreateInfo& samplerInfo)
{
    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();

    VkResult result = devFuncs->vkCreateSampler(device, &samplerInfo, nullptr, &mSampler);
    if (result != VK_SUCCESS) {
        qWarning("Can't create image\n");
        return false;
    }
    return true;
}

void SamplerDescr::release()
{
    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();

    devFuncs->vkDestroySampler(device, mSampler, nullptr);
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
    std::swap(mResourceMgr, other.mResourceMgr);
}
