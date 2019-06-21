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

#include "ImageViewDescr.hpp"
#include "ResourceManager.hpp"
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

ImageViewDescr::ImageViewDescr(ResourceManager* resourceMgr)
    : mResourceMgr(resourceMgr)
{
    assert(mResourceMgr && "Resource Manager should be valid!");
}

ImageViewDescr::~ImageViewDescr()
{
    release();
}

void ImageViewDescr::release()
{
    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();
    devFuncs->vkDestroyImageView(device, mImageView, nullptr);
    mImageView = nullptr;
}

ImageViewDescr::ImageViewDescr(ImageViewDescr&& other)
{
    swapAll(std::move(other));
}

ImageViewDescr& ImageViewDescr::operator=(ImageViewDescr&& other)
{
    swapAll(std::move(other));
    return *this;
}

void ImageViewDescr::swapAll(ImageViewDescr&& other)
{
    std::swap(mImageView, other.mImageView);
    std::swap(mResourceMgr, other.mResourceMgr);
}

bool ImageViewDescr::createImageView(const VkImageViewCreateInfo& imageViewInfo)
{
    release();
    VkResult result = VK_SUCCESS;

    QVulkanDeviceFunctions* devFuncs = mResourceMgr->deviceFunctions();
    VkDevice device = mResourceMgr->device();

    result = devFuncs->vkCreateImageView(device, &imageViewInfo, nullptr, &mImageView);
    if (result != VK_SUCCESS) {
        qWarning("Can't create image view\n");
        return false;
    }

    return true;
}

VkImageViewType ImageViewDescr::imgFormatToViewFormat(VkImageType imgType) {
    switch (imgType) {
    case VK_IMAGE_TYPE_1D : return VK_IMAGE_VIEW_TYPE_1D;
    case VK_IMAGE_TYPE_2D : return VK_IMAGE_VIEW_TYPE_2D;
    case VK_IMAGE_TYPE_3D : return VK_IMAGE_VIEW_TYPE_3D;
    // return VK_IMAGE_VIEW_TYPE_CUBE;
    // return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    // return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    // return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    default: break;
    }
    assert(!"Can't convert!!!");
    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

