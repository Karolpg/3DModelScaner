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

#include "Cube.hpp"
#include <atomic>
#include <glm/ext.hpp>
#include <QVulkanDeviceFunctions>


Cube::Cube(bool useTexture)
    : mVulkanInstance(nullptr)
    , mDevFuncs(nullptr)
    , mDevice(nullptr)
    , mPhysicalDev(nullptr)
    , mViewMtx(nullptr)
    , mProjMtx(nullptr)
    , mUseTexture(useTexture)
{
    static std::atomic_uint counter;
    uint32_t localId = ++counter;
    mId = std::string("Cube ") + std::to_string(localId);
    mDescr = "Base simple graphic object";

    qInfo("Creating: %s - %s", mId.c_str(), mDescr.c_str());
}

Cube::~Cube()
{
    qInfo("Destroying: %s - %s", mId.c_str(), mDescr.c_str());
}

const char* Cube::id() const
{
    return mId.c_str();
}

const char* Cube::description() const
{
    return mDescr.c_str();
}

namespace {
struct Uniform {
    glm::mat4x4    mvpMtx;
    glm::mat4x4    viewMtx;
    glm::mat4x4    projMtx;
    glm::mat4x4    modelMtx;
};
}

void Cube::initResource(QVulkanInstance *vulkanInstance,
                        QVulkanDeviceFunctions *devFuncs,
                        VkDevice device,
                        VkPhysicalDevice physicalDev)
{
    mVulkanInstance = vulkanInstance;
    assert(mVulkanInstance);

    mDevFuncs = devFuncs;
    assert(mDevFuncs);

    mDevice = device;
    assert(mDevice);

    mPhysicalDev = physicalDev;
    assert(mPhysicalDev);

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

    mGo.vertices.push_back(std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev)));
    mGo.vertices[0]->createBuffer(cubeVertices, sizeof(cubeVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    if (mUseTexture) {
        mGo.vertices.push_back(std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev)));
        mGo.vertices[1]->createBuffer(cubeUV, sizeof(cubeUV), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }

    mGo.indices = std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev));
    mGo.indices->createBuffer(indices, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mGo.indexType = VK_INDEX_TYPE_UINT16;
    mGo.indicesCount = sizeof(indices)/sizeof(indices[0]);

    ::Uniform uniformDefinition = {};
    mGo.uniforms = std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev));
    mGo.uniforms->createBuffer(&uniformDefinition, sizeof(uniformDefinition), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    mGo.uniformMapping.resize(1); // one descriptor set
    mGo.uniformMapping.back().resize(1); // one binding
    VkDescriptorBufferInfo& uniformBufferInfo = mGo.uniformMapping.back().back();
    uniformBufferInfo.buffer = mGo.uniforms->getBuffer();
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(::Uniform);

    mGo.modelMtx = glm::identity<glm::mat4>();
}

void Cube::initPipeline(PipelineManager *pipelineMgr)
{
    if (mUseTexture) {
        std::map<PipelineManager::AdditionalParameters, QVariant> parameter;
        parameter[PipelineManager::ApSeparatedAttributes] = true;
        mGo.pipelineInfo = pipelineMgr->getPipeline("../shaders/calc_position_uv.vert.bin", "", "", "", "../shaders/texture.frag.bin", parameter);
    }
    else {
        mGo.pipelineInfo = pipelineMgr->getPipeline("../shaders/calc_position.vert.bin", "", "", "", "../shaders/gradient.frag.bin");
    }

    mGo.connectResourceWithUniformSets(*mDevFuncs, mDevice);
}

void Cube::update()
{
    updateUniformBuffer();
}

void Cube::draw(VkCommandBuffer cmdBuf)
{
    assert(mDevFuncs);
    if (!mGo.pipelineInfo) {
        qWarning("%s does not contain pipelineInfo object!", mId.c_str());
        return;
    }

    if (!cmdBuf) {
        qWarning("Invalid command buffer provided! While drawing: %s", mId.c_str());
        return;
    }

    mDevFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mGo.pipelineInfo->pipeline);

    std::vector<VkDescriptorSet> descriptorSets(mGo.pipelineInfo->descriptorSetInfo.size());
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < descriptorSets.size(); ++descriptorSetIdx) {
        descriptorSets[descriptorSetIdx] = mGo.pipelineInfo->descriptorSetInfo[descriptorSetIdx].descriptor;
    }

    mDevFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mGo.pipelineInfo->pipelineLayout,
                                       0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), //descriptor set info
                                       0, nullptr); //dynamic offset

    uint32_t firstBinding = 0;

    std::vector<VkBuffer> vertexBuffers(mGo.vertices.size(), nullptr);
    std::vector<VkDeviceSize> offsets(mGo.vertices.size(), 0);

    for (size_t i = 0; i < mGo.vertices.size(); ++i) {
        vertexBuffers[i] = mGo.vertices[i]->getBuffer();
    }

    mDevFuncs->vkCmdBindVertexBuffers(cmdBuf, firstBinding, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(), offsets.data());

    VkDeviceSize indexOffset = 0;
    mDevFuncs->vkCmdBindIndexBuffer(cmdBuf, mGo.indices->getBuffer(), indexOffset,  mGo.indexType);

    //vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    mDevFuncs->vkCmdDrawIndexed(cmdBuf, mGo.indicesCount, 1, 0, 0, 0);
}

void Cube::releasePipeline()
{
    mGo.pipelineInfo = nullptr;
}

void Cube::releaseResource()
{
    mGo.vertices.clear();
    mGo.indices.release();
    mGo.indicesCount = 0;
    mGo.uniforms.release();
    mGo.uniformMapping.clear();
}

void Cube::updateUniformBuffer()
{
    assert(mDevFuncs);
    if (!mViewMtx || !mProjMtx) {
        qWarning("%s does not have valid view or projection matrix!", mId.c_str());
        return;
    }
    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMappedPtr = nullptr;
    mDevFuncs->vkMapMemory(mDevice, mGo.uniforms->getMem(), offset, sizeof(Uniform), mappingFlags, &deviceMemMappedPtr);
    char* deviceMemMapped = static_cast<char*>(deviceMemMappedPtr);

    memcpy(deviceMemMapped + offsetof(Uniform, viewMtx), &((*mViewMtx)[0]), sizeof(decltype(*mViewMtx)));
    memcpy(deviceMemMapped + offsetof(Uniform, projMtx), &((*mProjMtx)[0]), sizeof(decltype(*mProjMtx)));
    memcpy(deviceMemMapped + offsetof(Uniform, modelMtx), &mGo.modelMtx[0], sizeof(mGo.modelMtx));

    glm::mat4x4 mvpMtx = (*mProjMtx) * (*mViewMtx) * mGo.modelMtx;
    memcpy(deviceMemMapped + offsetof(Uniform, mvpMtx), &mvpMtx[0], sizeof(mvpMtx));

    mDevFuncs->vkUnmapMemory(mDevice, mGo.uniforms->getMem());
}
