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

#include <Graphic/ImageDescr.hpp>
#include <Graphic/SamplerDescr.hpp>


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

const char* qImgFormatToStr(QImage::Format format) {
    switch (format) {
        case QImage::Format_Invalid                : return "Invalid";
        case QImage::Format_Mono                   : return "Mono";
        case QImage::Format_MonoLSB                : return "MonoLSB";
        case QImage::Format_Indexed8               : return "Indexed8";
        case QImage::Format_RGB32                  : return "RGB32";
        case QImage::Format_ARGB32                 : return "ARGB32";
        case QImage::Format_ARGB32_Premultiplied   : return "ARGB32_Premultiplied";
        case QImage::Format_RGB16                  : return "RGB16";
        case QImage::Format_ARGB8565_Premultiplied : return "ARGB8565_Premultiplied";
        case QImage::Format_RGB666                 : return "RGB666";
        case QImage::Format_ARGB6666_Premultiplied : return "ARGB6666_Premultiplied";
        case QImage::Format_RGB555                 : return "RGB555";
        case QImage::Format_ARGB8555_Premultiplied : return "ARGB8555_Premultiplied";
        case QImage::Format_RGB888                 : return "RGB888";
        case QImage::Format_RGB444                 : return "RGB444";
        case QImage::Format_ARGB4444_Premultiplied : return "ARGB4444_Premultiplied";
        case QImage::Format_RGBX8888               : return "RGBX8888";
        case QImage::Format_RGBA8888               : return "RGBA8888";
        case QImage::Format_RGBA8888_Premultiplied : return "RGBA8888_Premultiplied";
        case QImage::Format_BGR30                  : return "BGR30";
        case QImage::Format_A2BGR30_Premultiplied  : return "A2BGR30_Premultiplied";
        case QImage::Format_RGB30                  : return "RGB30";
        case QImage::Format_A2RGB30_Premultiplied  : return "A2RGB30_Premultiplied";
        case QImage::Format_Alpha8                 : return "Alpha8";
        case QImage::Format_Grayscale8             : return "Grayscale8";
        case QImage::Format_RGBX64                 : return "RGBX64";
        case QImage::Format_RGBA64                 : return "RGBA64";
        case QImage::Format_RGBA64_Premultiplied   : return "RGBA64_Premultiplied";
        default : break;
    }
    return "Unknown";
}

}

Q_DECLARE_METATYPE(VkDescriptorBufferInfo);
Q_DECLARE_METATYPE(VkDescriptorImageInfo);

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
        0.0f  , 0.0f,   //0
        0.999f, 0.0f,   //1
        0.999f, 0.999f, //2
        0.0f  , 0.999f, //3
        0.999f, 0.999f, //4
        0.999f, 0.0f,   //5
        0.0f  , 0.0f,   //6
        0.0f  , 0.999f, //7
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

        prepareTexture();
    }

    mGo.indices = std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev));
    mGo.indices->createBuffer(indices, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mGo.indexType = VK_INDEX_TYPE_UINT16;
    mGo.indicesCount = sizeof(indices)/sizeof(indices[0]);

    ::Uniform uniformDefinition = {};
    mGo.uniforms = std::unique_ptr<BufferDescr>(new BufferDescr(*mVulkanInstance, mDevice, mPhysicalDev));
    mGo.uniforms->createBuffer(&uniformDefinition, sizeof(uniformDefinition), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    mGo.uniformMapping.resize(1); // descriptor sets
    mGo.uniformMapping.back().resize(1 + (mUseTexture ? 1 : 0)); // bindings
    VkDescriptorBufferInfo uniformBufferInfo;
    uniformBufferInfo.buffer = mGo.uniforms->getBuffer();
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(::Uniform);
    mGo.uniformMapping[0][0] = QVariant::fromValue(uniformBufferInfo);

    if (mUseTexture) {
        VkDescriptorImageInfo uniformSamplerInfo;
        uniformSamplerInfo.sampler = mGo.textures.back().sampler->getSampler();
        uniformSamplerInfo.imageView = mGo.textures.back().view->getImageView();
        uniformSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        mGo.uniformMapping[0][1] = QVariant::fromValue(uniformSamplerInfo);
        mSetTextureLayout = true;
    }

    mGo.modelMtx = glm::identity<glm::mat4>();
}

void Cube::setImageLayout(VkCommandBuffer cmdBuf)
{
    // TODO
    // Similar found in some Vulkan example but it works without this also. Is this bug? Does't matter which layout I choose, for my device.
    // I have to check if it is realy faster, read more!!!
    if (!mSetTextureLayout) {
        return;
    }
    mSetTextureLayout = false;

    auto imgLayoutToDstAccessMask = [](const VkImageLayout &layout) -> uint32_t {
        switch (layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL            : return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL            : return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        : return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            // Make sure any Copy or CPU writes to image are flushed
                                                             : return (VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR                 : return VK_ACCESS_MEMORY_READ_BIT;
        default:
            break;
        }
        return 0;
    };

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = imgLayoutToDstAccessMask(barrier.newLayout);
    barrier.srcQueueFamilyIndex = 0;
    barrier.dstQueueFamilyIndex = 0;
    barrier.image = mGo.textures.back().image->getImage();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.baseArrayLayer = 0;

    mDevFuncs->vkCmdPipelineBarrier(cmdBuf,
                                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                    VkDependencyFlagBits(),
                                    0, nullptr,
                                    0, nullptr,
                                    1, &barrier);
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

void Cube::setupBarrier(VkCommandBuffer cmdBuf)
{
    assert(mDevFuncs);
    if (!mGo.pipelineInfo) {
        qWarning("%s does not contain pipelineInfo object!", mId.c_str());
        return;
    }

    if (!cmdBuf) {
        qWarning("Invalid command buffer provided! While setupBarrier: %s", mId.c_str());
        return;
    }

    setImageLayout(cmdBuf);
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
    mGo.textures.clear();
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

void Cube::prepareTexture()
{
    //QString imageFilePath = "../../resources/textures/wood_001.jpg";
    QString imageFilePath = "../../resources/textures/test.png";
    if (!mImage.load(imageFilePath)) {
        qWarning("Can't load image: %s", imageFilePath.toLatin1().data());
    }
    else {
        qInfo("Loaded image: %s size(%d, %d), format(%s-%d), hasAlpha(%d)"
              , imageFilePath.toLatin1().data()
              , mImage.width(), mImage.height()
              , qImgFormatToStr(mImage.format()), static_cast<int>(mImage.format())
              , static_cast<int>(mImage.hasAlphaChannel()));
        if (mImage.format() != QImage::Format_RGBA8888) {
            mImage = mImage.convertToFormat(QImage::Format_RGBA8888);
        }
        //mImage.data_ptr();
        const uchar * rawData = mImage.bits();

        VkExtent3D imageSize = {static_cast<uint32_t>(mImage.width()), static_cast<uint32_t>(mImage.height()), 1};

        mGo.textures.push_back(Texture());
        Texture& t = mGo.textures.back();
        VkFormat imgFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
        t.image = std::unique_ptr<ImageDescr>(new ImageDescr(*mVulkanInstance, mDevice, mPhysicalDev));
        t.image->createImage(imgFormat, imageSize, 1, rawData, false, VK_IMAGE_USAGE_SAMPLED_BIT);

        VkSamplerCreateInfo sci = {};
        sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        //sci.flags = ;
        sci.magFilter = VK_FILTER_NEAREST;
        sci.minFilter = VK_FILTER_NEAREST;
        sci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        sci.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        sci.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        //sci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        //sci.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sci.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sci.mipLodBias = 0.0f;
        sci.anisotropyEnable = VK_FALSE;
        sci.maxAnisotropy = 1.0f;
        sci.compareEnable = VK_FALSE;
        sci.compareOp = VK_COMPARE_OP_NEVER;
        sci.minLod = 0.0f;
        sci.maxLod = 0.0f;
        sci.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        sci.unnormalizedCoordinates = VK_FALSE;
        t.sampler = std::unique_ptr<SamplerDescr>(new SamplerDescr(*mVulkanInstance, mDevice));
        t.sampler->createSampler(sci);

        VkImageViewCreateInfo ivci = {};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        //imageViewInfo.flags = ;
        ivci.image = t.image->getImage();
        ivci.viewType = ImageViewDescr::imgFormatToViewFormat(VK_IMAGE_TYPE_2D);
        ivci.format = imgFormat;
        //imageViewInfo.components = ; // swizzling
        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivci.subresourceRange.baseMipLevel = 0;
        ivci.subresourceRange.levelCount = 1;
        ivci.subresourceRange.baseArrayLayer = 0;
        ivci.subresourceRange.layerCount = 1;
        t.view = std::unique_ptr<ImageViewDescr>(new ImageViewDescr(*mVulkanInstance, mDevice));
        t.view->createImageView(ivci);
    }
}
