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

    createUniformSet();
    createPipeline();

    //
    // Vulkan Coordinates System
    //
    //
    //          .------> X
    //         /|
    //        / |
    //       /  |
    //   Z  v   v  Y

    QSize frameSize = mParent.swapChainImageSize();
    float fov = 60.f;
    preparePerspective(glm::radians(fov), static_cast<float>(frameSize.width()), static_cast<float>(frameSize.height()), 0.001f, 1000.f);

    mEyePosition = glm::vec3(0.f, 2.f, 5.f);
    mEyeLookAtDir = glm::normalize(glm::vec3(0.f, 0.f, 0.f) - mEyePosition);
    mViewMtx = glm::mat4x4(1);
    lookAt(mEyePosition, mEyePosition + mEyeLookAtDir*mEyeLookAtDistance, mUpDir);
}

void VulkanRenderer::releaseSwapChainResources()
{
    releasePipeline();
    releaseUniformSet();
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

    mDevFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    mDevFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mUniformDescriptorSet, 0, nullptr);

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

    mCube.vertices = createBuffer(cubeVertices, sizeof(cubeVertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    mCube.indices = createBuffer(indices, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    mCube.indexType = VK_INDEX_TYPE_UINT16;
    mCube.indicesCount = sizeof(indices)/sizeof(indices[0]);

    Uniform uniformDefinition = {};
    mCube.uniforms = createBuffer(&uniformDefinition, sizeof(uniformDefinition), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    const char simpleVertexShaderTxt[] =
        "#version 450\n"
        "layout(location = 0) in vec3 pos;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 1.0);\n"
        "}\n";

    const char vertexShaderTxt[] =
        "#version 450\n"
        "layout(location = 0) in vec3 pos;\n"
        "layout(location = 0) out vec3 posOut;\n"
        "layout(binding = 0) uniform buf {\n"
        "            mat4 mvp;            \n"
        "            mat4 view;           \n"
        "            mat4 proj;           \n"
        "            mat4 model;          \n"
        "    } uniformBuf;                \n"
        "                                 \n"

        "void main() {                    \n"
        "    posOut = pos;                           \n"
        "    vec4 posLocal = vec4(pos, 1.0);         \n"
        "    gl_Position = uniformBuf.mvp * posLocal;\n"
        "}                                           \n";

    const char dirShowfragmentShaderTxt[] =
        "#version 450\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"

        "layout(location = 0) in vec3 pixelPos;\n"
        "layout(location = 0) out vec4 outColor;"

        "void main() {"
            "      if (pixelPos.x < 0 && pixelPos.y < 0 && pixelPos.z < 0)  outColor = vec4(0.3,0.3,0.3,1);"

            " else if (pixelPos.x >= 0 && pixelPos.y < 0 && pixelPos.z < 0) outColor = vec4(1.0,0.3,0.3,1);"
            " else if (pixelPos.x < 0 && pixelPos.y >= 0 && pixelPos.z < 0) outColor = vec4(0.3,1.0,0.3,1);"
            " else if (pixelPos.x < 0 && pixelPos.y < 0 && pixelPos.z >= 0) outColor = vec4(0.3,0.3,1.0,1);"

            " else if (pixelPos.x < 0 && pixelPos.y >= 0 && pixelPos.z >= 0) outColor = vec4(0.3, 1.0, 1.0,1);"
            " else if (pixelPos.x >= 0 && pixelPos.y < 0 && pixelPos.z >= 0) outColor = vec4(1.0, 0.3, 1.0,1);"

            " else if (pixelPos.x >= 0 && pixelPos.y >= 0 && pixelPos.z < 0) outColor = vec4(1.0, 1.0, 0.3,1);"

            " else if (pixelPos.x >= 0 && pixelPos.y >= 0 && pixelPos.z >= 0) outColor = vec4(1.0, 1.0, 1.0,1);"
            " else outColor = vec4(0.5, 1.0, 1.0, 1);"

        //"    outColor = vec4((pixelPos + 1.0)/2.0, 1.0);"
        "}";

    const char fragmentShaderTxt[] =
        "#version 450\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"

        "layout(location = 0) in vec3 pixelPos;\n"
        "layout(location = 0) out vec4 outColor;"

        "void main() {"
        "    outColor = vec4((pixelPos + 1.0)/2.0, 1.0);"
        "}";

    mCube.material.vertexShader = createShader(vertexShaderTxt, sizeof(vertexShaderTxt)-1, shaderc_vertex_shader);
    mCube.material.fragmentShader = createShader(fragmentShaderTxt, sizeof(fragmentShaderTxt)-1, shaderc_fragment_shader);

    mCube.modelMtx = glm::identity<glm::mat4>();
}

void VulkanRenderer::releaseCube()
{
    VkDevice device = mParent.device();

    mDevFuncs->vkDestroyBuffer(device, mCube.vertices.buffer, nullptr);
    mDevFuncs->vkFreeMemory(device, mCube.vertices.mem, nullptr);

    mDevFuncs->vkDestroyBuffer(device, mCube.indices.buffer, nullptr);
    mDevFuncs->vkFreeMemory(device, mCube.indices.mem, nullptr);

    mDevFuncs->vkDestroyShaderModule(device, mCube.material.vertexShader, nullptr);
    mDevFuncs->vkDestroyShaderModule(device, mCube.material.fragmentShader, nullptr);
}

VkShaderModule VulkanRenderer::createShader(const char* shaderStr, uint32_t shaderLen, int shadercShaderKindEnumVal)
{
    VkResult result = VK_SUCCESS;

    shaderc::Compiler compiler;
    if (!compiler.IsValid()) {
        qFatal("SpirV compiler is invalid!");
    }
    shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(shaderStr, shaderLen, shaderc_shader_kind(shadercShaderKindEnumVal), "");
    shaderc_compilation_status compilationStatus = compilationResult.GetCompilationStatus();
    if (compilationStatus != shaderc_compilation_status_success) {
        qInfo("Compilation fail. Error msg: %s, Error count: %d warning count: %d \n", compilationResult.GetErrorMessage().c_str()
                                                                                     , static_cast<uint32_t>(compilationResult.GetNumErrors())
                                                                                     , static_cast<uint32_t>(compilationResult.GetNumWarnings()));
    }

    VkDevice device = mParent.device();

    VkShaderModuleCreateInfo shaderModuleCi = {};
    shaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCi.codeSize = sizeof(uint32_t)*static_cast<uint32_t>(compilationResult.end() - compilationResult.begin());
    shaderModuleCi.pCode = compilationResult.begin();
    //shaderModuleCi.flags = ; //future use

    VkShaderModule shaderModule;
    result = mDevFuncs->vkCreateShaderModule(device, &shaderModuleCi, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        qInfo("Failed to create shader module. Result: %i", result);
    }
    return shaderModule;
}

void VulkanRenderer::createPipeline()
{
    VkResult result = VK_SUCCESS;
    VkDevice device = mParent.device();
    QSize frameSize = mParent.swapChainImageSize();

    //
    // Shaders to stages
    //
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = mCube.material.vertexShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = mCube.material.fragmentShader;
    fragShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    //
    // Vertex description
    //
    struct VertexInputData {
        float position[3];
    };

    bool isSeparate = false;
    std::vector<VkVertexInputBindingDescription> vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAtrDesc;
    if (isSeparate) {
        vertexBindings.push_back({0, sizeof(VertexInputData::position), VK_VERTEX_INPUT_RATE_VERTEX});
    }
    else {
        vertexBindings.push_back({0, sizeof(VertexInputData), VK_VERTEX_INPUT_RATE_VERTEX});
    }

    uint32_t location = 0;
    uint32_t bindCtr = 0;
    uint32_t offset = 0;

#define UPDATE_VARS(sizeofAdded)  location += isSeparate ? 0 : 1; \
                                  bindCtr += isSeparate ? 1 : 0; \
                                  offset += isSeparate ? 0 : sizeofAdded;

    vertexAtrDesc.push_back({location, bindCtr, VK_FORMAT_R32G32B32_SFLOAT, offset});
    UPDATE_VARS(sizeof(VertexInputData::position))
    //...
#undef UPDATE_VARS

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindings.data();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAtrDesc.size());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAtrDesc.data();

    //
    // Topology
    //
    static const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
    {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,// sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                        // topology
        VK_FALSE                                                    // primitiveRestartEnable
    };


    VkViewport viewport = {};
    viewport.width = frameSize.width();
    viewport.height = frameSize.height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.f;

    //static const VkRect2D scissor = { { 0, 0 }, { 1, 1 } }; // offset, extent
    VkRect2D scissor = {};
    scissor.extent.width = static_cast<uint32_t>(frameSize.width());
    scissor.extent.height = static_cast<uint32_t>(frameSize.height());

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    //
    // Rasterizer
    //
    static const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
    {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // sType
        nullptr,                         // pNext
        0,                               // flags
        VK_FALSE,                        // depthClampEnable
        VK_FALSE,                        // rasterizerDiscardEnable
        VK_POLYGON_MODE_FILL,            // polygonMode
        VK_CULL_MODE_FRONT_BIT,          // cullMode
        VK_FRONT_FACE_COUNTER_CLOCKWISE , //VK_FRONT_FACE_CLOCKWISE,         // frontFace
        VK_FALSE,                        // depthBiasEnable
        0.0f,                            // depthBiasConstantFactor
        0.0f,                            // depthBiasClamp
        0.0f,                            // depthBiasSlopeFactor
        1.0f                             // lineWidth
    };

    //
    // Multisample
    //
    static const VkPipelineMultisampleStateCreateInfo multisampleState = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // sType;
        nullptr,                       // pNext;
        0,                             // flags;
        mParent.sampleCountFlagBits(), // rasterizationSamples;
        VK_FALSE,                      // sampleShadingEnable;
        0.f,//1.0f,                          // minSampleShading;
        nullptr,                       // pSampleMask;
        VK_FALSE,                      // alphaToCoverageEnable;
        VK_FALSE,                      // alphaToOneEnable;
    };

    //
    // Depth stencil
    //
    static const VkStencilOpState frontOp = {
        VK_STENCIL_OP_KEEP, // failOp;
        VK_STENCIL_OP_KEEP, // passOp;
        VK_STENCIL_OP_KEEP, // depthFailOp;
        VK_COMPARE_OP_LESS, // compareOp;
        0,                  // compareMask;
        0,                  // writeMask;
        0                   // reference;
    };

    static const VkStencilOpState backOp = {
        VK_STENCIL_OP_KEEP, // failOp;
        VK_STENCIL_OP_KEEP, // passOp;
        VK_STENCIL_OP_KEEP, // depthFailOp;
        VK_COMPARE_OP_LESS, // compareOp;
        0,                  // compareMask;
        0,                  // writeMask;
        0                   // reference;
    };

    static const VkPipelineDepthStencilStateCreateInfo depthStencilState = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // sType;
        nullptr,            // pNext;
        0,                  // flags;
        VK_TRUE,            // depthTestEnable;
        VK_TRUE,            // depthWriteEnable;
        VK_COMPARE_OP_LESS_OR_EQUAL, // depthCompareOp;
        VK_FALSE,           // depthBoundsTestEnable;
        VK_FALSE,           // stencilTestEnable;
        frontOp,            // front;
        backOp,             // back;
        0.0f,               // minDepthBounds;
        0.1f                // maxDepthBounds;
    };

    //
    // Color blend
    //
    static const VkPipelineColorBlendAttachmentState takeSrcColorAttachment = {
        VK_FALSE,                  // blendEnable;
        VK_BLEND_FACTOR_ONE,       // srcColorBlendFactor;
        VK_BLEND_FACTOR_ZERO,      // dstColorBlendFactor;
        VK_BLEND_OP_ADD,           // colorBlendOp;
        VK_BLEND_FACTOR_ONE,       // srcAlphaBlendFactor;
        VK_BLEND_FACTOR_ZERO,      // dstAlphaBlendFactor;
        VK_BLEND_OP_ADD,           // alphaBlendOp;
          VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT // colorWriteMask;
    };

    static const VkPipelineColorBlendAttachmentState mixColorAttachment = {
        VK_TRUE,                             // blendEnable;
        VK_BLEND_FACTOR_SRC_ALPHA,           // srcColorBlendFactor;
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, // dstColorBlendFactor;
        VK_BLEND_OP_ADD,                     // colorBlendOp;
        VK_BLEND_FACTOR_ONE,                 // srcAlphaBlendFactor;
        VK_BLEND_FACTOR_ZERO,                // dstAlphaBlendFactor;
        VK_BLEND_OP_ADD,                     // alphaBlendOp;
          VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT           // colorWriteMask;
    };

    static const VkPipelineColorBlendStateCreateInfo colorBlendState = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, // sType;
        nullptr,                    // pNext;
        0,                          // flags;
        VK_FALSE,                   // logicOpEnable;
        VK_LOGIC_OP_COPY,           // logicOp;
        1,                          // attachmentCount;
        &takeSrcColorAttachment,    // pAttachments;
        {0.0f, 0.0f, 0.0f, 0.0f}    // blendConstants[4];
    };

    //
    // Pipeline layout - Uniforms, Samplers
    //
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;            // Optional
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    result = mDevFuncs->vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    if (result != VK_SUCCESS) {
        qFatal("Failed to create pipeline layout. Result: %i", result);
    }

    //
    // Dynamic state e.g. dynamic viewport
    //
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = 0;
    dynamicStateInfo.pDynamicStates = nullptr;

    //
    // Graphic pipeline
    //
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = shaderStages.size();
    graphicsPipelineCreateInfo.pStages = shaderStages.data();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilState;   // Optional
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
    graphicsPipelineCreateInfo.pDynamicState = nullptr;//&dynamicStateInfo;         // Optional

    graphicsPipelineCreateInfo.layout = mPipelineLayout;
    graphicsPipelineCreateInfo.renderPass = mParent.defaultRenderPass();
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = nullptr;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    //
    // Cache info - if I correctly remember it is used to save pipeline on disk to faster recreate TODO check it
    //
    //VkPipelineCacheCreateInfo pipelineCacheInfo;
    //VkPipelineCache pipelineCache;
    //result = mDevFuncs->vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache);
    //if (result != VK_SUCCESS) {
    //    qFatal("Can't create pipeline cache. Result: %i", result);
    //}

    result = mDevFuncs->vkCreateGraphicsPipelines(device, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline);
    if (result != VK_SUCCESS) {
        qFatal("Can't create pipeline. Result: %i", result);
    }
}

void VulkanRenderer::releasePipeline()
{
    VkDevice device = mParent.device();
    mDevFuncs->vkDestroyPipeline(device, mPipeline, nullptr);
    mDevFuncs->vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
    mDevFuncs->vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
}

void VulkanRenderer::drawCube()
{
    VkCommandBuffer cmdBuf = mParent.currentCommandBuffer();

    uint32_t firstBinding = 0;
    std::array<VkBuffer, 1> vertexBuffers = {mCube.vertices.buffer};
    std::array<VkDeviceSize, 1>  offsets = {0};
    static_assert(vertexBuffers.size() == offsets.size(), "This arrays have to be the same size!\n");
    mDevFuncs->vkCmdBindVertexBuffers(cmdBuf, firstBinding, vertexBuffers.size(), vertexBuffers.data(), offsets.data());

    VkDeviceSize indexOffset = 0;
    mDevFuncs->vkCmdBindIndexBuffer(cmdBuf, mCube.indices.buffer, indexOffset,  mCube.indexType);

    //vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    mDevFuncs->vkCmdDrawIndexed(cmdBuf, mCube.indicesCount, 1, 0, 0, 0);

    //void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    //mDevFuncs->vkCmdDraw(cmdBuf, static_cast<uint32_t>(vertices.size()), 1, 0, 0);


//    mDevFuncs->vkCmdDraw(cmdBuf, 6, 1, 0, 0);
}

BufferDescr VulkanRenderer::createBuffer(const void* data, size_t dataSize, VkBufferUsageFlags usage)
{
    BufferDescr retVal = {};

    VkResult result = VK_SUCCESS;
    VkDevice device = mParent.device();

    //
    // Vertex buffer description
    //
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = dataSize;
    //bufferInfo.flags = ; // for sparse buffers, additional protection, capture/replay
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //bufferInfo.pQueueFamilyIndices = ; //only when sharingMode == VK_SHARING_MODE_CONCURRENT
    //bufferInfo.queueFamilyIndexCount = ;

    VkBuffer buffer;
    result = mDevFuncs->vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        qWarning("Can't create buffer\n");
        return retVal;
    }

    //
    // Memory requirements for this buffer
    //
    VkMemoryRequirements memReqs;
    mDevFuncs->vkGetBufferMemoryRequirements(device, buffer, &memReqs);

    //
    // Suitable memory type available on physical device
    //
    uint32_t memoryTypeIndex = ~0u;
    {
        VkPhysicalDeviceMemoryProperties physDevMemProps;
        mParent.vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mParent.physicalDevice(), &physDevMemProps);

        uint32_t memoryPropertyFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT     // allow write by host
                                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                    | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & (1 << i)) && (physDevMemProps.memoryTypes[i].propertyFlags & memoryPropertyFlag) == memoryPropertyFlag) {
                memoryTypeIndex = i;
                break;
            }
        }
        if (memoryTypeIndex == ~0u) {
            qWarning("Can't find memory type for buffer\n");
            return retVal;
        }
    }

    //
    // Memory allocation
    //
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    VkDeviceMemory mem;
    result = mDevFuncs->vkAllocateMemory(device, &allocInfo, nullptr, &mem);
    if (result != VK_SUCCESS) {
        qWarning("Can't allocate memory for buffer\n");
        return retVal;
    }

    //
    // Copy from host
    //
    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMapped = nullptr;
    mDevFuncs->vkMapMemory(device, mem, offset, memReqs.size, mappingFlags, &deviceMemMapped);

    memcpy(deviceMemMapped, data, dataSize);

    mDevFuncs->vkUnmapMemory(device, mem);

    result = mDevFuncs->vkBindBufferMemory(device, buffer, mem, offset);
    if (result != VK_SUCCESS) {
        qWarning("Can't bind memory to buffer\n");
        return retVal;
    }
    retVal.buffer = buffer;
    retVal.mem = mem;
    return retVal;
}

void VulkanRenderer::createUniformSet()
{
    VkResult result = VK_SUCCESS;
    VkDevice device = mParent.device();

    //
    // Layout of the descriptor set - how it will be used in shader
    //
    VkDescriptorSetLayoutBinding descriptorSetSpecification = {};
    descriptorSetSpecification.binding = 0; //start from binding
    descriptorSetSpecification.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetSpecification.descriptorCount = 1;
    descriptorSetSpecification.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetSpecification.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //descriptorSetLayoutInfo.flags = 0;
    descriptorSetLayoutInfo.bindingCount = 1;
    descriptorSetLayoutInfo.pBindings = &descriptorSetSpecification;

    result = mDevFuncs->vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &mDescriptorSetLayout);
    if (result != VK_SUCCESS) {
        qFatal("Failed to create pipeline layout. Result: %i", result);
        return;
    }

    //
    // Create descriptor pool - container for batch of descriptorSets
    //
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = 1; // sum of descriptors created below

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.maxSets = 1; // if we would like to allow for all descrSets at one time then: maxSets = 0; for(poolSize : pPoolSizes) maxSets += poolSize.descriptorCount;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;

    result = mDevFuncs->vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &mDescriptorPool);
    if (result != VK_SUCCESS) {
        qWarning("Failed to create descriptor pool. Result: %i", result);
        return;
    }

    //
    // Create descriptor set
    //
    VkDescriptorSetAllocateInfo uniformAllocateInfo = {};
    uniformAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    uniformAllocateInfo.descriptorPool = mDescriptorPool;
    uniformAllocateInfo.descriptorSetCount = 1;
    uniformAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

    result = mDevFuncs->vkAllocateDescriptorSets(device, &uniformAllocateInfo, &mUniformDescriptorSet);
    if (result != VK_SUCCESS) {
        qWarning("Can't allocate descriptor set\n");
        return;
    }

    //
    // Make connection between Buffor and DescriptorSet
    //
    VkDescriptorBufferInfo uniformBufferInfo;
    uniformBufferInfo.buffer = mCube.uniforms.buffer;
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(Uniform);

    VkWriteDescriptorSet uniformWrite = {};
    uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniformWrite.dstSet = mUniformDescriptorSet;
    uniformWrite.dstBinding = 0;
    uniformWrite.dstArrayElement = 0;
    uniformWrite.descriptorCount = 1;
    uniformWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformWrite.pImageInfo = nullptr;
    uniformWrite.pBufferInfo = &uniformBufferInfo;
    uniformWrite.pTexelBufferView = nullptr;

    mDevFuncs->vkUpdateDescriptorSets(device, 1, &uniformWrite, 0, nullptr);
}

void VulkanRenderer::releaseUniformSet()
{
    VkDevice device = mParent.device();
    mDevFuncs->vkFreeDescriptorSets(device, mDescriptorPool,1, &mUniformDescriptorSet);

    mDevFuncs->vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
}

void VulkanRenderer::updateUniformBuffer()
{
    VkDevice device = mParent.device();

    VkDeviceSize offset = 0;
    VkMemoryMapFlags mappingFlags = 0; // reserved for future use
    void* deviceMemMappedPtr = nullptr;
    mDevFuncs->vkMapMemory(device, mCube.uniforms.mem, offset, sizeof(Uniform), mappingFlags, &deviceMemMappedPtr);
    char* deviceMemMapped = static_cast<char*>(deviceMemMappedPtr);

    memcpy(deviceMemMapped + offsetof(Uniform, viewMtx), &mViewMtx[0], sizeof(mViewMtx));
    memcpy(deviceMemMapped + offsetof(Uniform, projMtx), &mProjMtx[0], sizeof(mProjMtx));
    memcpy(deviceMemMapped + offsetof(Uniform, modelMtx), &mCube.modelMtx[0], sizeof(mCube.modelMtx));

    glm::mat4x4 mvpMtx = mProjMtx * mViewMtx * mCube.modelMtx;
    memcpy(deviceMemMapped + offsetof(Uniform, mvpMtx), &mvpMtx[0], sizeof(mvpMtx));

    mDevFuncs->vkUnmapMemory(device, mCube.uniforms.mem);
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

