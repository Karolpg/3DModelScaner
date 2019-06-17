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

#pragma once

#include <vulkan/vulkan.h>
//#include <glm/glm.hpp>
#include <map>
#include <string>
#include <QSize>
#include <QVariant> //TODO exchange with c++17 std::variant

class QVulkanInstance;
class QVulkanDeviceFunctions;

class PipelineManager
{
public:
    enum AdditionalParameters {
        ApSeparatedAttributes, // [bool] 0 - interleaved (default); 1 - separated
    };

    struct BindingInfo {
        VkDescriptorSetLayoutBinding vdslbInfo;
        size_t byteSize;
    };

    struct DescriptorSetInfo {
        VkDescriptorSetLayout layout;
        VkDescriptorSet       descriptor;
        std::vector<BindingInfo> bindingInfo;
    };

    struct PipelineInfo {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        //VkPipelineCache pipelineCache;
        std::vector<DescriptorSetInfo> descriptorSetInfo;
        VkDescriptorPool               descriptorPool;
    };

    typedef std::vector<std::vector<BindingInfo>> DescriptorSetsSpecifications; // DescriptorSetsSpecifications[ descriptorSet ][ bindingIdx ]

    PipelineManager(QVulkanInstance &vulkanInstance,
                    VkDevice device,
                    const QSize& frameSize,
                    uint32_t rasterizationSamples,
                    VkRenderPass defaultRenderPass);
    ~PipelineManager();

    ///
    /// Get or Create pipeline
    /// Owner of the returned object is PipelineManager
    ///
    const PipelineInfo* getPipeline(const std::string& vertexShaderPath,
                                    const std::string& tesselationControlShaderPath,
                                    const std::string& tesselationEvaluationShaderPath,
                                    const std::string& geometryShaderPath,
                                    const std::string& fragmentShaderPath,
                                    const std::map<AdditionalParameters, QVariant> &parameters = std::map<AdditionalParameters, QVariant>());

    void cleanUpShaders();


    static VkFormat chooseFloatFormat(uint32_t fieldBitWidth, uint32_t fieldCount);
    static VkFormat chooseIntFormat(uint32_t fieldBitWidth, uint32_t fieldCount, bool sign);
protected:

    struct VertexInfo{
        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAtrDesc;
    };

    struct UniformInfo{
        DescriptorSetsSpecifications descriptorSetsSpecifications; // descriptorSetSpecifications[ descriptorSet ][ bindingIdx ]
    };

    struct SamplerInfo{
        DescriptorSetsSpecifications descriptorSetsSpecifications; // descriptorSetSpecifications[ descriptorSet ][ bindingIdx ]
    };

    struct ShaderInfo {
        VkShaderModule shader;
        VertexInfo vertexInfo;
        UniformInfo uniformInfo;
        SamplerInfo samplerInfo;
    };

    //VkShaderModule createShader(const char* shaderStr, uint32_t shaderLen, int shadercShaderKindEnumVal);
    const ShaderInfo* getShader(const std::string& shaderPath, VkShaderStageFlagBits stage, const std::map<AdditionalParameters, QVariant> &parameters);
    bool createLayoutAndPoolForDescriptorSets(const std::vector<const ShaderInfo*>& shaderInfos,
                                              PipelineInfo& pipelineInfo);

    template <VkDescriptorType descrType>
    const DescriptorSetsSpecifications& getDescriptorSetSpec(const PipelineManager::ShaderInfo &shaderInfos);

    template <VkDescriptorType descrType>
    size_t getMaxDescriptorSet(const std::vector<const PipelineManager::ShaderInfo *> &shaderInfos);

    template <VkDescriptorType descrType>
    void fillMaxDescriptorSetBindings(const std::vector<const PipelineManager::ShaderInfo *> &shaderInfos, std::vector<size_t>& maxBindings);

    template <VkDescriptorType descrType>
    void fillDescriptorSetBindingsInfo(const std::vector<const PipelineManager::ShaderInfo *> &shaderInfos, DescriptorSetsSpecifications& infos);

    std::map<std::string, ShaderInfo> mShaders;
    std::map<std::string, PipelineInfo> mPipelines;

    VkDevice mDevice = nullptr;
    QVulkanDeviceFunctions *mDevFuncs = nullptr;

    QSize mFrameSize;
    uint32_t mRasterizationSamples;
    VkRenderPass mDefaultRenderPass;
};

