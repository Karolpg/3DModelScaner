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
#include <memory>
#include <glm/glm.hpp>

class DrawManager
{
public:
    void setCmdBuffer(VkCommandBuffer cmdBuf);
    VkCommandBuffer getCmdBuffer() const;

    void setProjMatrix(const std::shared_ptr<glm::mat4x4>& projMtx);
    const std::shared_ptr<glm::mat4x4>& getProjMatrix() const;

    void setViewMatrix(const std::shared_ptr<glm::mat4x4>& viewMtx);
    const std::shared_ptr<glm::mat4x4>& getViewMatrix() const;

private:
    VkCommandBuffer mCmdBuf;

    std::shared_ptr<glm::mat4x4> mViewMtx;
    std::shared_ptr<glm::mat4x4> mProjMtx;
};

