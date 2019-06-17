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
#include <map>

class QVulkanInstance;
class QVulkanDeviceFunctions;

class BufferDescr
{
public:

    ///
    ///  Device should be created from provided physicalDevice
    ///
    BufferDescr(QVulkanInstance &vulkanInstance, VkDevice device, VkPhysicalDevice physicalDev);
    ~BufferDescr();

    //TODO this is creating direct buffer for both host and device access - add other posibilities - some staging buffer
    bool createBuffer(const void* data, size_t dataSize, VkBufferUsageFlags usage);
    VkBuffer getBuffer() const { return mBuffer; }
    VkDeviceMemory getMem() const { return mMem; }

    BufferDescr(const BufferDescr&) = delete;
    BufferDescr& operator=(const BufferDescr&) = delete;

    BufferDescr(BufferDescr&&);
    BufferDescr& operator=(BufferDescr&&);

protected:
    void release();
    void swapAll(BufferDescr&&);

    VkBuffer mBuffer = nullptr;
    VkDeviceMemory mMem = nullptr;
    VkDevice mDevice = nullptr;
    QVulkanDeviceFunctions *mDevFuncs = nullptr;

    static std::map<VkDevice, VkPhysicalDeviceMemoryProperties> sMemPropMap;
};
