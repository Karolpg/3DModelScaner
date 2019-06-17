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

#include <QVulkanWindow>
#include <glm/glm.hpp>
#include <memory>

#include <Graphic/PipelineManager.hpp>
#include <IRenderable.hpp>

class VulkanRenderer : public QVulkanWindowRenderer
{

public:
    VulkanRenderer(QVulkanWindow& parent);

    // Give a last chance to do decisions based on the physical device and the surface.
    void preInitResources() override;
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;
    void physicalDeviceLost() override;
    void logicalDeviceLost() override;

    void rotateCamera(float pitch, float yaw, float roll); //relative rotation x-pitch, y-yaw, z-roll [degree]
    void moveCamera(float right, float up, float forward); //relative move [meter]
protected:

    void updateUniformBuffer();

    void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
    void preparePerspective(float fovRadians, float width, float height, float minDepth, float maxDepth);

    std::unique_ptr<IRenderable> mCube;
    std::unique_ptr<PipelineManager> mPipelineMgr;

    glm::vec3 mEyePosition;
    glm::vec3 mEyeLookAtDir;
    const float mEyeLookAtDistance = 0.5f;
    const glm::vec3 mUpDir = glm::vec3(0.f, 1.f, 0.f); //-1 because of Vulkan Coordinates

    glm::mat4x4 mViewMtx;
    glm::mat4x4 mProjMtx;

    QVulkanWindow &mParent;
    QVulkanDeviceFunctions *mDevFuncs;
};
