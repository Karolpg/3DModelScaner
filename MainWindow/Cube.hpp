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

#include <IRenderable.hpp>
#include <string>
#include <vulkan/vulkan.h>
#include <Graphic/GraphicObject.hpp>
#include <QImage>

class QVulkanInstance;
class QVulkanDeviceFunctions;

class Cube : public IRenderable
{
public:

    Cube(bool useTexture = false);
    ~Cube() override;

    const char* id() const override;
    const char* description() const override;

    void initResource(QVulkanInstance *vulkanInstance,
                      QVulkanDeviceFunctions *devFuncs,
                      VkDevice device,
                      VkPhysicalDevice physicalDev) override;
    void initPipeline(PipelineManager* pipelineMgr) override;
    void update() override;
    void setupBarrier(VkCommandBuffer cmdBuf) override;
    void draw(VkCommandBuffer cmdBuf) override;
    void releasePipeline() override;
    void releaseResource() override;

    void setViewMtx(const glm::mat4x4 *viewMtx) { mViewMtx = viewMtx;}
    void setProjMtx(const glm::mat4x4 *projMtx) { mProjMtx = projMtx;}
protected:
    void updateUniformBuffer();
    void setImageLayout(VkCommandBuffer cmdBuf);
    void prepareTexture();

protected:
    std::string mId;
    std::string mDescr;
    GraphicObject mGo;

    QVulkanInstance *mVulkanInstance;
    QVulkanDeviceFunctions *mDevFuncs;
    VkDevice mDevice;
    VkPhysicalDevice mPhysicalDev;

    const glm::mat4x4 *mViewMtx = nullptr;
    const glm::mat4x4 *mProjMtx = nullptr;

    bool mUseTexture = false;
    bool mSetTextureLayout = false;
    QImage mImage;
};
