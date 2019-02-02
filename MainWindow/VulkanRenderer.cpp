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

#include "VulkanRenderer.hpp"
#include <QVulkanDeviceFunctions>

VulkanRenderer::VulkanRenderer(QVulkanWindow& parent)
    : mParent(parent)
{

}

//void VulkanRenderer::preInitResources() {}

void VulkanRenderer::initResources()
{
    assert(mParent.vulkanInstance() && "Vulkan instance should to be valid here!!!");
    mDevFuncs = mParent.vulkanInstance()->deviceFunctions(mParent.device());
}

//void VulkanRenderer::initSwapChainResources() {}
//void VulkanRenderer::releaseSwapChainResources() {}
//void VulkanRenderer::releaseResources() {}

void VulkanRenderer::startNextFrame()
{
    QSize frameSize = mParent.swapChainImageSize();

    VkClearColorValue clearColor = { { 1.0f, 0.0f, 0.0f, 1.0f } };
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[3];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = mParent.defaultRenderPass();
    rpBeginInfo.framebuffer = mParent.currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = frameSize.width();
    rpBeginInfo.renderArea.extent.height = frameSize.height();
    rpBeginInfo.clearValueCount = mParent.sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    mDevFuncs->vkCmdBeginRenderPass(mParent.currentCommandBuffer(), &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    mDevFuncs->vkCmdEndRenderPass(mParent.currentCommandBuffer());


    mParent.frameReady();
}

//void VulkanRenderer::physicalDeviceLost() {}
//void VulkanRenderer::logicalDeviceLost() {}
