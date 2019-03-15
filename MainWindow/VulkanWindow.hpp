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

class VulkanRenderer;

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    VulkanWindow();

    ///From QT documentation:
    /// This virtual function is called once during the lifetime of the window, at some point after making it visible for the first time.
    /// The window takes ownership of the returned renderer object.
    ///Karolpg:
    /// This is not clear from documentation how this object is handled.
    /// QVulkanWindowPrivate invoke this function once in init().
    /// And then in destructor ~QVulkanWindowPrivate it delete allocated object.
    /// So there is no need to call this function by our site.
    QVulkanWindowRenderer *createRenderer() override;

protected:
    void keyPressEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

//s_ignals:
//    void vulkanInfoReceived(const QString &text);
//    void frameQueued(int colorValue);

protected:
    VulkanRenderer* mVulkanRenderer = nullptr; // Should not be released here!!!
    QPointF mPrevCursorPosition;
};
