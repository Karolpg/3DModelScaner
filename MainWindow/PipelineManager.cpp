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

#include "PipelineManager.hpp"
#include <QVulkanInstance>
#include <QVulkanDeviceFunctions>
#include <fstream>
#include <spirv_cross.hpp>

PipelineManager::PipelineManager(QVulkanInstance &vulkanInstance,
                                 VkDevice device,
                                 const QSize& frameSize,
                                 uint32_t rasterizationSamples,
                                 VkRenderPass defaultRenderPass)
    : mDevice(device)
    , mFrameSize(frameSize)
    , mRasterizationSamples(rasterizationSamples)
    , mDefaultRenderPass(defaultRenderPass)
{
    assert(mDevice && "Device should be valid!");
    mDevFuncs = vulkanInstance.deviceFunctions(mDevice);
    assert(mDevFuncs && "Device functions should be valid!");

    qInfo("Creating pipeline manager: %p", this);
}

PipelineManager::~PipelineManager()
{
    qInfo("Destroying pipeline manager: %p", this);

    cleanUpShaders();

    std::for_each(mPipelines.begin(), mPipelines.end(),
                  [this](const decltype(mPipelines)::value_type& pair) { const PipelineInfo& pi = pair.second;
                                                                         mDevFuncs->vkDestroyPipeline(mDevice, pi.pipeline, nullptr);
                                                                         mDevFuncs->vkDestroyPipelineLayout(mDevice, pi.pipelineLayout, nullptr);
                                                                         //mDevFuncs->vkDestroyPipelineCache(mDevice, pi.pipelineCache, nullptr);
                                                                         for (const DescriptorSetInfo& dsi : pi.descriptorSetInfo) {
                                                                             mDevFuncs->vkDestroyDescriptorSetLayout(mDevice, dsi.layout, nullptr);
                                                                             mDevFuncs->vkFreeDescriptorSets(mDevice, pi.descriptorPool, 1, &dsi.descriptor);
                                                                         }
                                                                         mDevFuncs->vkDestroyDescriptorPool(mDevice, pi.descriptorPool, nullptr);
                                                                       });
    mPipelines.clear();
}

void PipelineManager::cleanUpShaders()
{
    std::for_each(mShaders.begin(), mShaders.end(),
                  [this](const decltype(mShaders)::value_type& pair) { mDevFuncs->vkDestroyShaderModule(mDevice, pair.second.shader, nullptr); });
    mShaders.clear();
}

/*
#include <libshaderc/shaderc.hpp>

VkShaderModule PipelineManager::createShader(const char* shaderStr, uint32_t shaderLen, int shadercShaderKindEnumVal)
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

    VkShaderModuleCreateInfo shaderModuleCi = {};
    shaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCi.codeSize = sizeof(uint32_t)*static_cast<uint32_t>(compilationResult.end() - compilationResult.begin());
    shaderModuleCi.pCode = compilationResult.begin();
    //shaderModuleCi.flags = ; //future use

    VkShaderModule shaderModule;
    result = mDevFuncs->vkCreateShaderModule(mDevice, &shaderModuleCi, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        qInfo("Failed to create shader module. Result: %i", result);
    }
    return shaderModulmShaderse;
}
*/

VkFormat PipelineManager::chooseFloatFormat(uint32_t fieldBitWidth, uint32_t fieldCount)
{
    assert(fieldBitWidth && fieldCount);
    switch(fieldBitWidth) {
    case 16:
        switch (fieldCount) {
        case 1: return VK_FORMAT_R16_SFLOAT;
        case 2: return VK_FORMAT_R16G16_SFLOAT;
        case 3: return VK_FORMAT_R16G16B16_SFLOAT;
        case 4: return VK_FORMAT_R16G16B16A16_SFLOAT;
        default:break;
        }
        break;
    case 32:
        switch (fieldCount) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:break;
        }
        break;
    case 64:
        switch (fieldCount) {
        case 1: return VK_FORMAT_R64_SFLOAT;
        case 2: return VK_FORMAT_R64G64_SFLOAT;
        case 3: return VK_FORMAT_R64G64B64_SFLOAT;
        case 4: return VK_FORMAT_R64G64B64A64_SFLOAT;
        default:break;
        }
        break;
    default: break;
    }
    qWarning("Unsupported Float format. fieldBitWidth=%d, fieldCount=%d\n", fieldBitWidth, fieldCount);
    return VK_FORMAT_UNDEFINED;
}

VkFormat PipelineManager::chooseIntFormat(uint32_t fieldBitWidth, uint32_t fieldCount, bool sign)
{
    assert(fieldBitWidth && fieldCount);

#define CHOOSE_INT(SIGN_TYPE)\
    switch(fieldBitWidth) {\
    case 16:\
        switch (fieldCount) {\
        case 1: return VK_FORMAT_R16_## SIGN_TYPE ## INT;\
        case 2: return VK_FORMAT_R16G16_## SIGN_TYPE ## INT;\
        case 3: return VK_FORMAT_R16G16B16_## SIGN_TYPE ## INT;\
        case 4: return VK_FORMAT_R16G16B16A16_## SIGN_TYPE ## INT;\
        default:break;\
        }\
        break;\
    case 32:\
        switch (fieldCount) {\
        case 1: return VK_FORMAT_R32_## SIGN_TYPE ## INT;\
        case 2: return VK_FORMAT_R32G32_## SIGN_TYPE ## INT;\
        case 3: return VK_FORMAT_R32G32B32_## SIGN_TYPE ## INT;\
        case 4: return VK_FORMAT_R32G32B32A32_## SIGN_TYPE ## INT;\
        default:break;\
        }\
        break;\
    case 64:\
        switch (fieldCount) {\
        case 1: return VK_FORMAT_R64_## SIGN_TYPE ## INT;\
        case 2: return VK_FORMAT_R64G64_## SIGN_TYPE ## INT;\
        case 3: return VK_FORMAT_R64G64B64_## SIGN_TYPE ## INT;\
        case 4: return VK_FORMAT_R64G64B64A64_## SIGN_TYPE ## INT;\
        default:break;\
        }\
        break;\
    default: break;\
    }

    if (sign) {
        CHOOSE_INT(S)
    }
    else {
        CHOOSE_INT(U)
    }
#undef CHOOSE_INT

    qWarning("Unsupported Float format. fieldBitWidth=%d, fieldCount=%d\n", fieldBitWidth, fieldCount);
    return VK_FORMAT_UNDEFINED;
}

// Reason why it is not part of PipelineManager is becaouse of no need to infect spirv header in PipelineManager header
static VkFormat SPIRTypeToVulkanFormat(const spirv_cross::SPIRType& type)
{
    VkFormat variableFormat = VK_FORMAT_UNDEFINED;

    switch (type.basetype) {
    case spirv_cross::SPIRType::Char         :
    case spirv_cross::SPIRType::SByte        :
    case spirv_cross::SPIRType::Short        :
    case spirv_cross::SPIRType::Int          :
    case spirv_cross::SPIRType::Int64        : variableFormat = PipelineManager::chooseIntFormat(type.width, type.vecsize, true); break;

    case spirv_cross::SPIRType::Boolean      :
    case spirv_cross::SPIRType::UByte        :
    case spirv_cross::SPIRType::UShort       :
    case spirv_cross::SPIRType::UInt         :
    case spirv_cross::SPIRType::UInt64       : variableFormat = PipelineManager::chooseIntFormat(type.width, type.vecsize, false); break;

    case spirv_cross::SPIRType::Half         :
    case spirv_cross::SPIRType::Float        :
    case spirv_cross::SPIRType::Double       : variableFormat = PipelineManager::chooseFloatFormat(type.width, type.vecsize); break;

    //case spirv_cross::SPIRType::AtomicCounter: break;
    //case spirv_cross::SPIRType::Struct       : break;
    //case spirv_cross::SPIRType::Image        : break;
    //case spirv_cross::SPIRType::SampledImage : break;
    //case spirv_cross::SPIRType::Sampler      : break;
    default:
        qWarning("Unsupported base type!\n");
        break;
    }

    return variableFormat;
}

static const char* executionModelToStr(spv::ExecutionModel execModel)
{
#define caseConvertToStr(x) case spv::x : return #x;
    switch(execModel) {
    caseConvertToStr(ExecutionModelVertex);
    caseConvertToStr(ExecutionModelTessellationControl);
    caseConvertToStr(ExecutionModelTessellationEvaluation);
    caseConvertToStr(ExecutionModelGeometry);
    caseConvertToStr(ExecutionModelFragment);
    caseConvertToStr(ExecutionModelGLCompute);
    caseConvertToStr(ExecutionModelKernel);
    caseConvertToStr(ExecutionModelTaskNV);
    caseConvertToStr(ExecutionModelMeshNV);
    caseConvertToStr(ExecutionModelRayGenerationNV);
    caseConvertToStr(ExecutionModelIntersectionNV);
    caseConvertToStr(ExecutionModelAnyHitNV);
    caseConvertToStr(ExecutionModelClosestHitNV);
    caseConvertToStr(ExecutionModelMissNV);
    caseConvertToStr(ExecutionModelCallableNV);
#undef caseConvertToStr
    default: break;
    }
    return "Unknown";
}

static void fillShaderInput(const std::string& shaderPath,
                            const spirv_cross::Compiler& resourcesCtx,
                            const spirv_cross::ShaderResources& resources,
                            std::vector<VkVertexInputBindingDescription>& vertexBindings,
                            std::vector<VkVertexInputAttributeDescription>& vertexAtrDesc,
                            const std::map<PipelineManager::AdditionalParameters, QVariant> &parameters)
{
    auto separatedParam = parameters.find(PipelineManager::ApSeparatedAttributes);
    bool isSeparate = separatedParam == parameters.end() ? false : separatedParam->second.toBool();
    uint32_t inputStructureSize = 0;
    for (uint32_t i = 0; i < resources.stage_inputs.size(); ++i) {
        const spirv_cross::Resource& inputRes = resources.stage_inputs[i];
        spirv_cross::SPIRType variableType = resourcesCtx.get_type(inputRes.type_id);

        uint32_t binding = resourcesCtx.get_decoration(inputRes.id, spv::DecorationBinding);
        uint32_t location = resourcesCtx.get_decoration(inputRes.id, spv::DecorationLocation);
        uint32_t variableSize = (variableType.width / 8 + (variableType.width % 8 ? 1 : 0)) * variableType.vecsize * variableType.columns; // in bytes
        VkFormat variableFormat = SPIRTypeToVulkanFormat(variableType);

        qInfo("Input: %s id:%d type:%d base:%d size:%d binding:%d location:%d"
             , inputRes.name.c_str(), inputRes.id, inputRes.type_id, inputRes.base_type_id
             , variableSize
             , binding
             , location);

        if (variableFormat == VK_FORMAT_UNDEFINED) {
            qWarning("Undefined format during shader (%s) input processing.", shaderPath.c_str());
            continue;
        }

        if (isSeparate) {
            vertexBindings.push_back({i, variableSize, VK_VERTEX_INPUT_RATE_VERTEX});
        }
        else {
            inputStructureSize += variableSize;
        }

        //uint32_t location = 0;
        uint32_t bindCtr = 0;
        uint32_t offset = 0;

    #define UPDATE_VARS(sizeofAdded)  /*location += isSeparate ? 0 : 1; */\
                                      bindCtr += isSeparate ? 1 : 0; \
                                      offset += isSeparate ? 0 : sizeofAdded;

        vertexAtrDesc.push_back({location, bindCtr, variableFormat, offset});
        UPDATE_VARS(variableSize)
        //...
    #undef UPDATE_VARS

    }
    if (!isSeparate && inputStructureSize) {
        vertexBindings.push_back({0, inputStructureSize, VK_VERTEX_INPUT_RATE_VERTEX});
    }
}

static void fillUniformInfo(const std::string& shaderPath,
                            VkShaderStageFlagBits stage,
                            const spirv_cross::Compiler& resourcesCtx,
                            const spirv_cross::ShaderResources& resources,
                            std::vector<std::vector<PipelineManager::BindingInfo>>& descriptorSetsSpecifications)
{
    for (uint32_t i = 0; i < resources.uniform_buffers.size(); ++i) {
        const spirv_cross::Resource& uniformRes = resources.uniform_buffers[i];
        spirv_cross::SPIRType variableType = resourcesCtx.get_type(uniformRes.type_id);

        uint32_t binding = resourcesCtx.get_decoration(uniformRes.id, spv::DecorationBinding); //if not parameter provided then 0
        uint32_t descriptorSet = resourcesCtx.get_decoration(uniformRes.id, spv::DecorationDescriptorSet);

        uint32_t arraySize = 1;
        for (uint32_t j = 0; j < variableType.array.size(); ++j) {
            arraySize += arraySize * variableType.array[j];
        }

        if (descriptorSetsSpecifications.size() <= descriptorSet) {
            descriptorSetsSpecifications.resize(descriptorSet + 1); // assume that there will be no empty descriptor set, if not then change it on the map
        }
        descriptorSetsSpecifications[descriptorSet].push_back(PipelineManager::BindingInfo());
        PipelineManager::BindingInfo& bindingInfo = descriptorSetsSpecifications[descriptorSet].back();

        //
        // Layout of the descriptor set - how it will be used in shader
        //
        bindingInfo.vdslbInfo.binding = binding; //start from binding
        bindingInfo.vdslbInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindingInfo.vdslbInfo.descriptorCount = arraySize; // for arrays
        bindingInfo.vdslbInfo.stageFlags = stage;
        bindingInfo.vdslbInfo.pImmutableSamplers = nullptr;

        //std::string variableName = uniformRes.name;
        //variableName = variableName.empty() ? resourcesCtx.get_name(uniformRes.id) : variableName;
        //variableName = variableName.empty() ? resourcesCtx.get_fallback_name(uniformRes.id) : variableName;

        if (variableType.basetype == spirv_cross::SPIRType::Struct) {
            bindingInfo.byteSize = resourcesCtx.get_declared_struct_size(variableType);
            /*
            for(uint32_t j = 0; j < variableType.member_types.size(); ++j) {
                spirv_cross::SPIRType subVarType = resourcesCtx.get_type(variableType.member_types[j]);

                uint32_t subVarSize = (subVarType.width / 8 + (subVarType.width % 8 ? 1 : 0)) * subVarType.vecsize * subVarType.columns; // in bytes
                VkFormat subVarFormat = SPIRTypeToVulkanFormat(subVarType);

                std::string memberName = resourcesCtx.get_member_name(uniformRes.type_id, j);

                qInfo("Uniform: (%s %s %s).(%s %s %s) id:%d(%d) type:%d(%d) base:%d size:%d(%d-%d) offset:%d arrayStride:_d matrixStride:_d bitset:0x%llx binding:%d descriptorSet:%d"
                      , uniformRes.name.c_str(), resourcesCtx.get_name(uniformRes.id).c_str(), resourcesCtx.get_fallback_name(uniformRes.id).c_str()
                      , resourcesCtx.get_member_name(uniformRes.id, j).c_str(), resourcesCtx.get_member_qualified_name(uniformRes.type_id, j).c_str(), resourcesCtx.get_fallback_member_name(j).c_str()
                      , uniformRes.id, j
                      , uniformRes.type_id, variableType.member_types[j]
                      , uniformRes.base_type_id
                      , static_cast<uint32_t>(resourcesCtx.get_declared_struct_size(variableType))
                      , subVarSize
                      , static_cast<uint32_t>(resourcesCtx.get_declared_struct_member_size(variableType, j))
                      , resourcesCtx.type_struct_member_offset(variableType, j)
                      //, resourcesCtx.type_struct_member_array_stride(variableType, j)
                      //, resourcesCtx.type_struct_member_matrix_stride(variableType, j)
                      , resourcesCtx.get_member_decoration_bitset(uniformRes.id, j).get_lower()
                      , binding
                      , descriptorSet
                );

                // TODO
                // Do I realy want to support nested structures?
                // Is it possible in glsl?
                if (subVarFormat == VK_FORMAT_UNDEFINED) {
                    qWarning("Undefined format during shader (%s) uniform processing.", shaderPath.c_str());
                    continue;
                }
            }
            */
        }
        else {
            uint32_t variableSize = (variableType.width / 8 + (variableType.width % 8 ? 1 : 0)) * variableType.vecsize * variableType.columns; // in bytes
            //VkFormat variableFormat = SPIRTypeToVulkanFormat(variableType);

            bindingInfo.byteSize = variableSize; //TODO SPIRV-Cross calculate it differently look at get_declared_struct_size inside !

            //qInfo("Uniform: %s id:%d type:%d base:%d size:%d binding:%d descriptorSet:%d"
            //      , uniformRes.name.c_str(), uniformRes.id, uniformRes.type_id, uniformRes.base_type_id
            //      , variableSize
            //      , binding
            //      , descriptorSet);
        }
    }
}

bool PipelineManager::createLayoutAndPoolForDescriptorSets(const std::vector<const ShaderInfo *> &shaderInfos,
                                                           PipelineInfo &pipelineInfo)
{
    VkResult result = VK_SUCCESS;

    //
    // Pick max size to not doing unnecessary reallocations for descriptor set level
    //
    size_t maxDescriptorSets = 0;
    for (const ShaderInfo *shaderInfo : shaderInfos) {
        if (!shaderInfo) {
            continue;
        }
        const auto& descrSetsSpecs = shaderInfo->uniformInfo.descriptorSetsSpecifications;
        maxDescriptorSets = std::max(maxDescriptorSets, descrSetsSpecs.size());
    }

    //
    // Pick max size to not doing unnecessary reallocations for binding level
    //
    std::vector<size_t> maxBindings(maxDescriptorSets);
    for (const ShaderInfo *shaderInfo : shaderInfos) {
        if (!shaderInfo) {
            continue;
        }
        const auto& descrSetsSpecs = shaderInfo->uniformInfo.descriptorSetsSpecifications;
        for (size_t descriptorSetIdx = 0; descriptorSetIdx < descrSetsSpecs.size(); ++descriptorSetIdx) {
            for (size_t bindingIdx = 0; bindingIdx < descrSetsSpecs[descriptorSetIdx].size(); ++bindingIdx) {
                size_t bindingNo = descrSetsSpecs[descriptorSetIdx][bindingIdx].vdslbInfo.binding;
                maxBindings[descriptorSetIdx] = std::max(maxBindings[descriptorSetIdx], bindingNo + 1);
            }
        }
    }

    //
    // Allocate memory
    //
    decltype (UniformInfo::descriptorSetsSpecifications) allShadersDescrSetsSpecs(maxDescriptorSets);
    for (size_t i = 0; i < allShadersDescrSetsSpecs.size(); ++i) {
        allShadersDescrSetsSpecs[i].resize(maxBindings[i]);
    }

    //
    // Fill info from all shaders
    //
    for (const ShaderInfo *shaderInfo : shaderInfos) {
        if (!shaderInfo) {
            continue;
        }
        const auto& srcDescrSetsSpecs = shaderInfo->uniformInfo.descriptorSetsSpecifications;
        for (size_t descriptorSetIdx = 0; descriptorSetIdx < srcDescrSetsSpecs.size(); ++descriptorSetIdx) {
            const auto& srcDescrSetSpecs = srcDescrSetsSpecs[descriptorSetIdx];
            auto& dstDescrSetSpecs = allShadersDescrSetsSpecs[descriptorSetIdx];
            for (size_t bindingIdx = 0; bindingIdx < srcDescrSetSpecs.size(); ++bindingIdx) {
                const BindingInfo& srcBinding = srcDescrSetSpecs[bindingIdx];
                BindingInfo& dstBinding = dstDescrSetSpecs[srcBinding.vdslbInfo.binding];

                assert(srcBinding.vdslbInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER && "Invalid descriptor type!"); // TODO allow for different type

                dstBinding.vdslbInfo.binding = srcBinding.vdslbInfo.binding;
                dstBinding.vdslbInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                if (dstBinding.vdslbInfo.descriptorCount == 0) {
                    dstBinding.vdslbInfo.descriptorCount = srcBinding.vdslbInfo.descriptorCount;
                }
                else {
                    assert(srcBinding.vdslbInfo.descriptorCount == dstBinding.vdslbInfo.descriptorCount && "Two different array size of the same binding on different stage!");
                }
                dstBinding.vdslbInfo.stageFlags |= srcBinding.vdslbInfo.stageFlags;
                dstBinding.vdslbInfo.pImmutableSamplers = nullptr;
                dstBinding.byteSize = srcBinding.byteSize;
            }
        }
    }

    pipelineInfo.descriptorSetInfo.resize(allShadersDescrSetsSpecs.size());
    for (size_t i = 0; i < allShadersDescrSetsSpecs.size(); ++i) {
        pipelineInfo.descriptorSetInfo[i].bindingInfo = allShadersDescrSetsSpecs[i];
    }

    //
    // Create descriptor set LAYOUTs
    //
    std::vector<VkDescriptorSetLayout> layouts(allShadersDescrSetsSpecs.size());
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < allShadersDescrSetsSpecs.size(); ++descriptorSetIdx) {
        std::vector<VkDescriptorSetLayoutBinding> bindings(allShadersDescrSetsSpecs[descriptorSetIdx].size());
        for (size_t bindingIdx = 0; bindingIdx < bindings.size(); ++bindingIdx) {
            bindings[bindingIdx] = allShadersDescrSetsSpecs[descriptorSetIdx][bindingIdx].vdslbInfo;
        }
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        //descriptorSetLayoutInfo.flags = 0;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(allShadersDescrSetsSpecs[descriptorSetIdx].size());
        descriptorSetLayoutInfo.pBindings = bindings.data();

        result = mDevFuncs->vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutInfo, nullptr, &pipelineInfo.descriptorSetInfo[descriptorSetIdx].layout);
        if (result != VK_SUCCESS) {
            qFatal("Failed to create pipeline layout. Result: %i", result);
            return false;
        }
        layouts[descriptorSetIdx] = pipelineInfo.descriptorSetInfo[descriptorSetIdx].layout;
    }

    //
    // Create descriptor POOL - container for batch of descriptorSets
    //
    VkDescriptorPoolSize descriptorPoolSize;
    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = static_cast<uint32_t>(pipelineInfo.descriptorSetInfo.size()); // descriptorCount is the number of descriptors of that type to allocate.

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //maxSets is the maximum number of descriptor sets that can be allocated from the pool.
    descriptorPoolInfo.maxSets = static_cast<uint32_t>(pipelineInfo.descriptorSetInfo.size());
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;

    result = mDevFuncs->vkCreateDescriptorPool(mDevice, &descriptorPoolInfo, nullptr, &pipelineInfo.descriptorPool);
    if (result != VK_SUCCESS) {
        qWarning("Failed to create descriptor pool. Result: %i", result);
        return false;
    }

    //
    // Create descriptor SETs
    //
    std::vector<VkDescriptorSet> descriptors(layouts.size());
    VkDescriptorSetAllocateInfo uniformAllocateInfo = {};
    uniformAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    uniformAllocateInfo.descriptorPool =pipelineInfo.descriptorPool;
    uniformAllocateInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    uniformAllocateInfo.pSetLayouts = layouts.data();

    result = mDevFuncs->vkAllocateDescriptorSets(mDevice, &uniformAllocateInfo, descriptors.data());
    if (result != VK_SUCCESS) {
        qWarning("Can't allocate descriptor set\n");
        return false;
    }

    for (size_t i = 0; i < descriptors.size(); ++i) {
        pipelineInfo.descriptorSetInfo[i].descriptor = descriptors[i];
    }

    return true;
}

const PipelineManager::ShaderInfo* PipelineManager::getShader(const std::string& shaderPath, VkShaderStageFlagBits stage, const std::map<AdditionalParameters, QVariant> &parameters)
{
    auto foundIt = mShaders.find(shaderPath);
    if (foundIt != mShaders.end()) {
        return &foundIt->second;
    }

    std::ifstream ifs;
    ifs.open(shaderPath, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        qWarning("Can't find shader: \"%s\"", shaderPath.c_str());
        return nullptr;
    }

    ifs.seekg (0, ifs.end);
    uint32_t length = static_cast<uint32_t>(ifs.tellg());
    ifs.seekg (0, ifs.beg);

    if (length % sizeof(uint32_t)) {
        qWarning("Shader don't have correct size. Shader: \"%s\" size: %d %% 4 != 0", shaderPath.c_str(), length);
        return nullptr;
    }

    std::vector<uint32_t> buf(length / sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(buf.data()), length);

    VkShaderModuleCreateInfo shaderModuleCi = {};
    shaderModuleCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCi.codeSize = length;
    shaderModuleCi.pCode = buf.data();
    //shaderModuleCi.flags = ; //future use

    //
    // Create Vulkan shader module
    //
    VkShaderModule shaderModule;
    VkResult result = VK_SUCCESS;
    result = mDevFuncs->vkCreateShaderModule(mDevice, &shaderModuleCi, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        qInfo("Failed to create shader module. Result: %i", result);
        return nullptr;
    }
    auto inserted = mShaders.insert(std::make_pair(shaderPath, ShaderInfo()));
    if (!inserted.second) {
        return nullptr;
    }
    ShaderInfo* shaderInfo = &inserted.first->second;
    shaderInfo->shader = shaderModule;

    //
    // Decompile shader from spir-v
    //
    spirv_cross::Compiler glsl(std::move(buf));
    spirv_cross::ShaderResources resources = glsl.get_shader_resources();    

    qInfo("Shader path: %s", shaderPath.c_str());
    std::vector<spirv_cross::EntryPoint> entryPoints = glsl.get_entry_points_and_stages();
    for (const spirv_cross::EntryPoint& entryPoint : entryPoints) {
        qInfo("%s(%d)-%s", executionModelToStr(entryPoint.execution_model), entryPoint.execution_model, entryPoint.name.c_str());
    }

    //
    // https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
    //

    //
    // Input
    //
    fillShaderInput(shaderPath, glsl, resources,
                    shaderInfo->vertexInfo.vertexBindings, shaderInfo->vertexInfo.vertexAtrDesc,
                    parameters);

    //
    // Uniform info
    //
    fillUniformInfo(shaderPath, stage, glsl, resources, shaderInfo->uniformInfo.descriptorSetsSpecifications);
    //
    // Samplers
    //
    //TODO

    return shaderInfo;
}

const PipelineManager::PipelineInfo* PipelineManager::getPipeline(const std::string& vertexShaderPath,
                                                                  const std::string& tesselationControlShaderPath,
                                                                  const std::string& tesselationEvaluationShaderPath,
                                                                  const std::string& geometryShaderPath,
                                                                  const std::string& fragmentShaderPath,
                                                                  const std::map<AdditionalParameters, QVariant> &parameters)
{
    std::string key = vertexShaderPath + tesselationControlShaderPath + tesselationEvaluationShaderPath + geometryShaderPath + fragmentShaderPath;

    auto foundIt = mPipelines.find(key);
    if (foundIt != mPipelines.end()) {
        return &foundIt->second;
    }

    VkResult result = VK_SUCCESS;


    const ShaderInfo* vertexShader = getShader(vertexShaderPath, VK_SHADER_STAGE_VERTEX_BIT, parameters);
    const ShaderInfo* tesselationControlShader = getShader(tesselationControlShaderPath, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, parameters);
    const ShaderInfo* tesselationEvaluationShader = getShader(tesselationEvaluationShaderPath, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, parameters);
    const ShaderInfo* geometryShader = getShader(geometryShaderPath, VK_SHADER_STAGE_GEOMETRY_BIT, parameters);
    const ShaderInfo* fragmentShader = getShader(fragmentShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT, parameters);

    std::vector<const ShaderInfo *> shaderInfos;
    shaderInfos.push_back(vertexShader);
    shaderInfos.push_back(tesselationControlShader);
    shaderInfos.push_back(tesselationEvaluationShader);
    shaderInfos.push_back(geometryShader);
    shaderInfos.push_back(fragmentShader);

    PipelineInfo pipelineInfo;

    createLayoutAndPoolForDescriptorSets(shaderInfos, pipelineInfo);

    //
    // Shaders to stages
    //
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexShader ? vertexShader->shader : nullptr;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentShader ? fragmentShader->shader : nullptr;
    fragShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    //
    // Vertex description input
    //
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexShader->vertexInfo.vertexBindings.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexShader->vertexInfo.vertexBindings.data();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexShader->vertexInfo.vertexAtrDesc.size());
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexShader->vertexInfo.vertexAtrDesc.data();

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


    //
    // Viewport Scissor
    //
    VkViewport viewport = {};
    viewport.width = mFrameSize.width();
    viewport.height = mFrameSize.height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.extent.width = static_cast<uint32_t>(mFrameSize.width());
    scissor.extent.height = static_cast<uint32_t>(mFrameSize.height());

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
        VkSampleCountFlagBits(mRasterizationSamples),  // rasterizationSamples;
        VK_FALSE,                      // sampleShadingEnable;
        0.f,//1.0f,                    // minSampleShading;
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
    std::vector<VkDescriptorSetLayout> descriptorSetlayouts(pipelineInfo.descriptorSetInfo.size());
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < descriptorSetlayouts.size(); ++descriptorSetIdx) {
        descriptorSetlayouts[descriptorSetIdx] = pipelineInfo.descriptorSetInfo[descriptorSetIdx].layout;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetlayouts.size());  // Optional
    pipelineLayoutInfo.pSetLayouts = descriptorSetlayouts.data();                            // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    result = mDevFuncs->vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &pipelineInfo.pipelineLayout);
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

    graphicsPipelineCreateInfo.layout = pipelineInfo.pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = mDefaultRenderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = nullptr;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    //
    // Cache info - if I correctly remember it is used to save pipeline on disk to faster recreate TODO check it
    //
    //VkPipelineCacheCreateInfo pipelineCacheInfo;
    //result = mDevFuncs->vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineInfo.pipelineCache);
    //if (result != VK_SUCCESS) {
    //    qFatal("Can't create pipeline cache. Result: %i", result);
    //}

    result = mDevFuncs->vkCreateGraphicsPipelines(mDevice, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &pipelineInfo.pipeline);
    if (result != VK_SUCCESS) {
        qFatal("Can't create pipeline. Result: %i", result);
        mDevFuncs->vkDestroyPipelineLayout(mDevice, pipelineInfo.pipelineLayout, nullptr);
        return nullptr;
    }
    auto inserted = mPipelines.insert(std::make_pair(key, pipelineInfo));
    return (inserted.second ? &inserted.first->second : nullptr);
}

