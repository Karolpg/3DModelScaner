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

#include "GraphicObject.hpp"
#include <QVulkanDeviceFunctions>

void GraphicObject::connectResourceWithUniformSets(QVulkanDeviceFunctions &devFuncs, VkDevice device)
{
    //
    // Validation
    //
    if (uniformMapping.size() != pipelineInfo->descriptorSetInfo.size()) {
        qWarning("Inconsistent data. uniformMapping.size() = %d, descriptorSetInfo.size() = %d"
                 , static_cast<uint32_t>(uniformMapping.size())
                 , static_cast<uint32_t>(pipelineInfo->descriptorSetInfo.size()));
        return;
    }
    if (uniformMapping.empty()) {
        //nothing to do here - both vectors are empty
        return;
    }

    //
    // Calculate memory and process additional validation
    //
    size_t allBindings = 0;
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < pipelineInfo->descriptorSetInfo.size(); ++descriptorSetIdx) {
        const PipelineManager::DescriptorSetInfo& dsi = pipelineInfo->descriptorSetInfo[descriptorSetIdx];
        size_t shaderBindings = dsi.bindingInfo.size();
        size_t objectBindings = uniformMapping[descriptorSetIdx].size();

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
            if (dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                const VkDescriptorBufferInfo* dbi = reinterpret_cast<const VkDescriptorBufferInfo*>(uniformMapping[descriptorSetIdx][bindingIdx].data());
                if (dbi->range != dsi.bindingInfo[bindingIdx].byteSize) { // check if it was correctly provided/created in app and shader
                    qWarning("Inconsistent data. Descriptor = %d binding = %d objectDataSize = %d, shaderDataSize = %d"
                             , static_cast<uint32_t>(descriptorSetIdx)
                             , static_cast<uint32_t>(bindingIdx)
                             , static_cast<uint32_t>(dbi->range)
                             , static_cast<uint32_t>(dsi.bindingInfo[bindingIdx].byteSize));
                }
            }
            else if (dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                //const VkDescriptorImageInfo* dii = reinterpret_cast<const VkDescriptorImageInfo*>(uniformMapping[descriptorSetIdx][bindingIdx].data());
            }
        }
    }

    //
    // Make connection between Buffor and DescriptorSet
    //
    std::vector<VkWriteDescriptorSet> uniformsWrite(allBindings);
    VkWriteDescriptorSet* writeDs = uniformsWrite.data();
    for (size_t descriptorSetIdx = 0; descriptorSetIdx < pipelineInfo->descriptorSetInfo.size(); ++descriptorSetIdx) {
        const PipelineManager::DescriptorSetInfo& dsi = pipelineInfo->descriptorSetInfo[descriptorSetIdx];
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
            if (dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                const VkDescriptorImageInfo* dii = reinterpret_cast<const VkDescriptorImageInfo*>(uniformMapping[descriptorSetIdx][bindingIdx].data());
                writeDs->pImageInfo = dii;
            }
            writeDs->pBufferInfo = nullptr;
            if (dsi.bindingInfo[bindingIdx].vdslbInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                const VkDescriptorBufferInfo* dbi = reinterpret_cast<const VkDescriptorBufferInfo*>(uniformMapping[descriptorSetIdx][bindingIdx].data());
                writeDs->pBufferInfo = dbi;
            }
            writeDs->pTexelBufferView = nullptr;
            ++writeDs;
        }
    }

    devFuncs.vkUpdateDescriptorSets(device, static_cast<uint32_t>(uniformsWrite.size()), uniformsWrite.data(), 0, nullptr);
}

