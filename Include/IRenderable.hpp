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

class PipelineManager;
class DrawManager;
class ResourceManager;

class IRenderable
{
public:
    virtual ~IRenderable() {}
    virtual const char* id() const = 0;                 /// sth unique to identify instance
    virtual const char* description() const = 0;        /// some full description of instance

    virtual void initResource(ResourceManager* resourceMgr) = 0;
    virtual void initPipeline(PipelineManager* pipelineMgr) = 0;
    virtual void update(DrawManager* drawMgr) = 0;
    virtual void setupBarrier(DrawManager* drawMgr) = 0; // TODO probably to change
    virtual void draw(DrawManager* drawMgr) = 0;
    virtual void releasePipeline() = 0;
    virtual void releaseResource() = 0;
};
