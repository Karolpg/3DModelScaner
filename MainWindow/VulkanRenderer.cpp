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

VulkanRenderer::VulkanRenderer(QVulkanWindow& parent)
    : mParent(parent)
{

}

void VulkanRenderer::preInitResources()
{

}

void VulkanRenderer::initResources()
{
    assert(mParent.vulkanInstance() && "Vulkan instance should to be valid here!!!");
    mDevFuncs = mParent.vulkanInstance()->deviceFunctions(mParent.device());
    assert(mDevFuncs && "Device functions should to be valid here!!!");

    createCube();
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

    mCube->pipelineInfo = mPipelineMgr->getPipeline("../shaders/calc_position.vert.bin", "", "", "", "../shaders/gradient.frag.bin");

    createUniformSet();

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
    mViewMtx = glm::mat4x4(1);
    lookAt(mEyePosition, mEyePosition + mEyeLookAtDir*mEyeLookAtDistance, mUpDir);
}

void VulkanRenderer::releaseSwapChainResources()
{
    mPipelineMgr.release();
}

void VulkanRenderer::releaseResources()
{
    releaseCube();
}

void VulkanRenderer::startNextFrame()
{
    updateUniformBuffer();

    QSize frameSize = mParent.swapChainImageSize();
    VkCommandBuffer cmdBuf = mParent.currentCommandBuffer();

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

    drawCube();

    mDevFuncs->vkCmdEndRenderPass(cmdBuf);

    mParent.frameReady();
}

void VulkanRenderer::physicalDeviceLost()
{
    qInfo("Physical device lost\n");

}

void VulkanRenderer::logicalDeviceLost()
{
    qInfo("Logical device lost\n");
}


void VulkanRenderer::createCube()
{
    static const float cubeVertices[] = {
        -1.0f,-1.0f,-1.0f, //0               2--------- 3
        -1.0f,-1.0f, 1.0f, //1            /  .      /   |
        -1.0f, 1.0f, 1.0f, //2          7-------- 4     |
         1.0f, 1.0f, 1.0f, //3          |    .    |     |
         1.0f, 1.0f,-1.0f, //4          |    1....|.... 6
         1.0f,-1.0f,-1.0f, //5          |  /      |  /
         1.0f,-1.0f, 1.0f, //6          0-------- 5
        -1.0f, 1.0f,-1.0f, //7
    };

    static const float cubeUV[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };

    //static const int16_t indices[] = {
       // 3vert 2vert 2 vert ....
       // 0, 5, 7, 4, 2, 3, 1, 6, 0, 5,
     // front | up     | back |
    //};

    static const uint16_t indices[] = {
        7,0,5,    4,7,5, //front
        2,3,1,    1,3,6, //back
        4,5,6,    3,4,6, //right
        1,7,2,    1,0,7, //left
        3,2,7,    3,7,4, //up
        5,0,1,    5,1,6, //down
    };

    mCube = std::unique_ptr<GraphicObject>(new GraphicObject);
    mCube->vertices = std::unique_ptr<BufferDescr>(new BufferDescr(*mParent.vulkanInstance(), mParent.device(), mParent.physicalDevice()));
    mCube->vertices->createBuffer(cubeVertices, sizeof(cubeVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    mCube->indices = std::unique_ptr<BufferDescr>(new BufferDescr(*mParent.vulkanInstance(), mParent.device(), mParent.physicalDevice()));
    mCube->indices->createBuffer(indices, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mCube->indexType = VK_INDEX_TYPE_UINT16;
    mCube->indicesCount = sizeof(indices)/sizeof(indices[0]);

    Uniform uniformDefinition = {};
    mCube->uniforms = std::unique_ptr<BufferDescr>(new BufferDescr(*mParent.vulkanInstance(), mParent.device(), mParent.physicalDevice()));
    mCube->uniforms->createBuffer(&uniformDefinition, sizeof(uniformDefinition), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    mCube->uniformMapping.resize(1); // one descriptor set
    mCube->uniformMapping.back().resize(1); // one binding
    VkDescriptorBufferInfo& uniformBufferInfo = mCube->uniformMapping.back().back();
    uniformBufferInfo.buffer = mCube->uniforms->getBuffer();
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(Uniform);

    mCube->modelMtx = glm::identity<glm::mat4>();
}

void VulkanRenderer::releaseCube()
{
    mCube.release();
}

void VulkanRenderer::drawCube()
{
    if (!mCube->pipelineInfo) {
        qWarning("Object does not contain pipelineInfo object!");
        return;
    }

    VkCommandBuffer cmdBuf = mParent.currentCommandBuffer();

    mDevFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mCube->pipelineInfo->pipeline);

    std::vector<VkDescriptorSet> descriptorSets(mCube->pipelineInfo->descriptorSetInfo.size());
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < descriptorSets.size(); ++descriptorSetIdx) {
        descriptorSets[descriptorSetIdx] = mCube->pipelineInfo->descriptorSetInfo[descriptorSetIdx].descriptor;
    }

    mDevFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mCube->pipelineInfo->pipelineLayout,
                                       0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), //descriptor set info
                                       0, nullptr); //dynamic offset

    uint32_t firstBinding = 0;
    std::array<VkBuffer, 1> vertexBuffers = {mCube->vertices->getBuffer()};
    std::array<VkDeviceSize, 1>  offsets = {0};
    static_assert(vertexBuffers.size() == offsets.size(), "This arrays have to be the same size!\n");
    mDevFuncs->vkCmdBindVertexBuffers(cmdBuf, firstBinding, vertexBuffers.size(), vertexBuffers.data(), offsets.data());

    VkDeviceSize indexOffset = 0;
    mDevFuncs->vkCmdBindIndexBuffer(cmdBuf, mCube->indices->getBuffer(), indexOffset,  mCube->indexType);

    //vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    mDevFuncs->vkCmdDrawIndexed(cmdBuf, mCube->indicesCount, 1, 0, 0, 0);
}

void VulkanRenderer::createUniformSet()
{
    VkDevice device = mParent.device();

    //
    // Validation
    //
    if (mCube->uniformMapping.size() != mCube->pipelineInfo->descriptorSetInfo.size()) {
        qWarning("Inconsistent data. uniformMapping.size() = %d, descriptorSetInfo.size() = %d"
                 , static_cast<uint32_t>(mCube->uniformMapping.size())
                 , static_cast<uint32_t>(mCube->pipelineInfo->descriptorSetInfo.size()));
        return;
    }
    if (mCube->uniformMapping.empty()) {
        //nothing to do here - both vectors are empty
        return;
    }

    //
    // Calculate memory and process additional validation
    //
    size_t allBindings = 0;
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < mCube->pipelineInfo->descriptorSetInfo.size(); ++descriptorSetIdx) {
        const PipelineManager::DescriptorSetInfo& dsi = mCube->pipelineInfo->descriptorSetInfo[descriptorSetIdx];
        size_t shaderBindings = dsi.bindingInfo.size();
        size_t objectBindings = mCube->uniformMapping[descriptorSetIdx].size();

        if (objectBindings != shaderBindings) {
            qWarning("Inconsistent data. Descriptor = %d objectBindings = %d, shaderBindings = %d"
                     , static_cast<uint32_t>(descriptorSetIdx)
                     , static_cast<uint32_t>(objectBindings)
                     , static_cast<uint32_t>(shaderBindings));
            return;
        }
        allBindings += shaderBindings;

        for (size_t bindingIdx = 0; bindingIdx < dsi.bindingInfo.size(); ++bindingIdx) {
            assert(bindingIdx == dsi.bindingInfo[bindingIdx].vdslbInfo.binding);
            if (mCube->uniformMapping[descriptorSetIdx][bindingIdx].range != dsi.bindingInfo[bindingIdx].byteSize) { // check if it was correctly provided/created in app and shader
                qWarning("Inconsistent data. Descriptor = %d binding = %d objectDataSize = %d, shaderDataSize = %d"
                         , static_cast<uint32_t>(descriptorSetIdx)
                         , static_cast<uint32_t>(bindingIdx)
                         , static_cast<uint32_t>(mCube->uniformMapping[descriptorSetIdx][bindingIdx].range)
                         , static_cast<uint32_t>(dsi.bindingInfo[bindingIdx].byteSize));
            }
        }
    }

    //
    // Make connection between Buffor and DescriptorSet
    //
    std::vector<VkWriteDescriptorSet> uniformsWrite(allBindings);
    VkWriteDescriptorSet* writeDs = uniformsWrite.data();
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < mCube->pipelineInfo->descriptorSetInfo.size(); ++descriptorSetIdx) {
        const PipelineManager::DescriptorSetInfo& dsi = mCube->pipelineInfo->descriptorSetInfo[descriptorSetIdx];
        for (size_t bindingIdx = 0; bindingIdx < dsi.bindingInfo.size(); ++bindingIdx) {
            writeDs->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDs->pNext = nullptr;
            writeDs->dstSet = dsi.descriptor;
            writeDs->dstBinding = dsi.bindingInfo[bindingIdx].vdslbInfo.binding;
            writeDs->dstArrayElement = 0; // is the starting element in that array. If the descriptor binding identified by ... then dstArrayElement specifies the starting byte ...
            writeDs->descriptorType = dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorType;

            writeDs->descriptorCount = 1; // hope that binding proper uniform to cover possible array in shader will be OK // TODO check it !!!
            //writeDs->descriptorCount = dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorCount; //is the number of descriptors to update (the number of elements in pImageInfo, pBufferInfo, or pTexelBufferView
            //assert(writeDs->descriptorCount == 1 && "Currently only one elemnt array available!");)

            writeDs->pImageInfo = nullptr;
            writeDs->pBufferInfo = &mCube->uniformMapping[descriptorSetIdx][bindingIdx];
            writeDs->pTexelBufferView = nullptr;
            ++writeDs;
        }
    }

    mDevFuncs->vkUpdateDescriptorSets(device, static_cast<uint32_t>(uniformsWrite.size()), uniformsWrite.data(), 0, nullptr);
}

void VulkanRenderer::updateUniformBuffer()
{
    VkDevice device = mParent.device();

    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMappedPtr = nullptr;
    mDevFuncs->vkMapMemory(device, mCube->uniforms->getMem(), offset, sizeof(Uniform), mappingFlags, &deviceMemMappedPtr);
    char* deviceMemMapped = static_cast<char*>(deviceMemMappedPtr);

    memcpy(deviceMemMapped + offsetof(Uniform, viewMtx), &mViewMtx[0], sizeof(mViewMtx));
    memcpy(deviceMemMapped + offsetof(Uniform, projMtx), &mProjMtx[0], sizeof(mProjMtx));
    memcpy(deviceMemMapped + offsetof(Uniform, modelMtx), &mCube->modelMtx[0], sizeof(mCube->modelMtx));

    glm::mat4x4 mvpMtx = mProjMtx * mViewMtx * mCube->modelMtx;
    memcpy(deviceMemMapped + offsetof(Uniform, mvpMtx), &mvpMtx[0], sizeof(mvpMtx));

    mDevFuncs->vkUnmapMemory(device, mCube->uniforms->getMem());
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

    mViewMtx[0][0] = s.x;
    mViewMtx[1][0] = s.y;
    mViewMtx[2][0] = s.z;
    mViewMtx[0][1] = u.x;
    mViewMtx[1][1] = u.y;
    mViewMtx[2][1] = u.z;
    mViewMtx[0][2] =-f.x;
    mViewMtx[1][2] =-f.y;
    mViewMtx[2][2] =-f.z;
    mViewMtx[3][0] =-dot(s, eye);
    mViewMtx[3][1] =-dot(u, eye);
    mViewMtx[3][2] = dot(f, eye);
}

void VulkanRenderer::preparePerspective(float fovRadians, float width, float height, float minDepth, float maxDepth)
{
    float halfFov = 0.5f * fovRadians;
    float f = glm::cos(halfFov) / glm::sin(halfFov);

    mProjMtx = glm::mat4x4(0);
    mProjMtx[0][0] = f * height / width;
    mProjMtx[1][1] = -f;
    mProjMtx[2][2] = maxDepth / (minDepth - maxDepth);
    mProjMtx[2][3] = -1;
    mProjMtx[3][2] = (minDepth * maxDepth) / (minDepth - maxDepth);
}

