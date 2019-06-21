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
#include <libshaderc/shaderc.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <array>
#include "Cube.hpp"
#include <Graphic/ResourceManager.hpp>
#include <Graphic/PipelineManager.hpp>
#include <Graphic/DrawManager.hpp>

VulkanRenderer::VulkanRenderer(QVulkanWindow& parent)
    : mDrawMgr(std::unique_ptr<DrawManager>(new DrawManager()))
    , mViewMtx(std::shared_ptr<glm::mat4x4>(new glm::mat4x4))
    , mProjMtx(std::shared_ptr<glm::mat4x4>(new glm::mat4x4))
    , mParent(parent)
{
    mDrawMgr->setProjMatrix(mProjMtx);
    mDrawMgr->setViewMatrix(mViewMtx);
}

void VulkanRenderer::preInitResources()
{

}

void VulkanRenderer::initResources()
{
    assert(mParent.vulkanInstance() && "Vulkan instance should to be valid here!!!");
    mDevFuncs = mParent.vulkanInstance()->deviceFunctions(mParent.device());
    assert(mDevFuncs && "Device functions should to be valid here!!!");

    mResourceMgr = std::unique_ptr<ResourceManager>(new ResourceManager(*mParent.vulkanInstance(),
                                                                        mParent.device(),
                                                                        mParent.physicalDevice()));

    Cube* cube = new Cube(true); // TODO move this allocation somewhere else
    mCube = std::unique_ptr<IRenderable>(cube);
    mCube->initResource(mResourceMgr.get());
}

void VulkanRenderer::initSwapChainResources()
{
    //here swapchain is valid eg. size of surface
    QSize frameSize = mParent.swapChainImageSize();

    mPipelineMgr = std::unique_ptr<PipelineManager>(new PipelineManager(*mParent.vulkanInstance(),
                                                                        mParent.device(),
                                                                        frameSize,
                                                                        mParent.sampleCountFlagBits(),
                                                                        mParent.defaultRenderPass()));
    mCube->initPipeline(mPipelineMgr.get());

    //
    // Vulkan Coordinates System
    //
    //
    //          .------> X
    //         /|
    //        / |
    //       /  |
    //   Z  v   v  Y


    float fov = 60.f;
    preparePerspective(glm::radians(fov), static_cast<float>(frameSize.width()), static_cast<float>(frameSize.height()), 0.001f, 1000.f);

    mEyePosition = glm::vec3(0.f, 2.f, 5.f);
    mEyeLookAtDir = glm::normalize(glm::vec3(0.f, 0.f, 0.f) - mEyePosition);
    *mViewMtx.get() = glm::mat4x4(1);
    lookAt(mEyePosition, mEyePosition + mEyeLookAtDir*mEyeLookAtDistance, mUpDir);
}

void VulkanRenderer::releaseSwapChainResources()
{
    mCube->releasePipeline();
    mPipelineMgr.reset();
}

void VulkanRenderer::releaseResources()
{
    mCube->releaseResource();
    mResourceMgr.reset();
}

void VulkanRenderer::startNextFrame()
{
    QSize frameSize = mParent.swapChainImageSize();
    VkCommandBuffer cmdBuf = mParent.currentCommandBuffer();
    mDrawMgr->setCmdBuffer(cmdBuf);

    mCube->update(mDrawMgr.get());
    mCube->setupBarrier(mDrawMgr.get());

    VkClearColorValue clearColor = { {  0.2f, 0.2f, 0.2f, 1.0f } };
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
    rpBeginInfo.renderArea.extent.width = static_cast<uint32_t>(frameSize.width());
    rpBeginInfo.renderArea.extent.height = static_cast<uint32_t>(frameSize.height());
    rpBeginInfo.clearValueCount = mParent.sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    mDevFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    mCube->draw(mDrawMgr.get());

    mDevFuncs->vkCmdEndRenderPass(cmdBuf);

    mParent.frameReady();

    mDrawMgr->setCmdBuffer(nullptr);
}

void VulkanRenderer::physicalDeviceLost()
{
    qInfo("Physical device lost\n");

}

void VulkanRenderer::logicalDeviceLost()
{
    qInfo("Logical device lost\n");
}

void VulkanRenderer::rotateCamera(float pitch, float yaw, float roll)
{
    glm::mat4 mat(1);
    mat = glm::rotate(mat, glm::radians(pitch), glm::vec3(-1.f, 0.f, 0.f));
    mat = glm::rotate(mat, glm::radians(yaw),   glm::vec3(0.f, -1.f, 0.f));
    mat = glm::rotate(mat, glm::radians(roll),  glm::vec3(0.f, 0.f, 1.f));
    mEyeLookAtDir = glm::mat3(mat) * mEyeLookAtDir;

    lookAt(mEyePosition, mEyePosition + mEyeLookAtDir*mEyeLookAtDistance, mUpDir);
}

void VulkanRenderer::moveCamera(float right, float up, float forward)
{
    glm::vec3 sideDir = glm::normalize(glm::cross(mEyeLookAtDir, mUpDir));
    glm::vec3 upDir = glm::cross(mEyeLookAtDir, sideDir);

    mEyePosition += sideDir * right;
    mEyePosition += upDir * -up;
    mEyePosition += mEyeLookAtDir * forward;
    lookAt(mEyePosition, mEyePosition + mEyeLookAtDir*mEyeLookAtDistance, mUpDir);
}

void VulkanRenderer::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    glm::vec3 const f(glm::normalize(center - eye));
    glm::vec3 const s(glm::normalize(glm::cross(f, up)));
    glm::vec3 const u(glm::cross(s, f));

    glm::mat4x4& viewMtx = *mViewMtx.get();

    viewMtx[0][0] = s.x;
    viewMtx[1][0] = s.y;
    viewMtx[2][0] = s.z;
    viewMtx[0][1] = u.x;
    viewMtx[1][1] = u.y;
    viewMtx[2][1] = u.z;
    viewMtx[0][2] =-f.x;
    viewMtx[1][2] =-f.y;
    viewMtx[2][2] =-f.z;
    viewMtx[3][0] =-dot(s, eye);
    viewMtx[3][1] =-dot(u, eye);
    viewMtx[3][2] = dot(f, eye);
}

void VulkanRenderer::preparePerspective(float fovRadians, float width, float height, float minDepth, float maxDepth)
{
    float halfFov = 0.5f * fovRadians;
    float f = glm::cos(halfFov) / glm::sin(halfFov);

    glm::mat4x4& projMtx = *mProjMtx.get();

    projMtx = glm::mat4x4(0);
    projMtx[0][0] = f * height / width;
    projMtx[1][1] = -f;
    projMtx[2][2] = maxDepth / (minDepth - maxDepth);
    projMtx[2][3] = -1;
    projMtx[3][2] = (minDepth * maxDepth) / (minDepth - maxDepth);
}

