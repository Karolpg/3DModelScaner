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

#include "VulkanWindow.hpp"
#include "VulkanRenderer.hpp"
#include <QInputEvent>
#include <set>

VulkanWindow::VulkanWindow()
{
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    assert(mVulkanRenderer == nullptr && "Should be called once!!!");
    mVulkanRenderer = new VulkanRenderer(*this);
    return mVulkanRenderer;
}

void VulkanWindow::keyPressEvent(QKeyEvent *event)
{
    static const std::set<int> movementSet = {Qt::Key_W, Qt::Key_S, Qt::Key_A, Qt::Key_D, Qt::Key_Q, Qt::Key_E};
    if (mVulkanRenderer
     && event
     && movementSet.find(event->key()) != movementSet.end()) {

        float moveRight = 0;
        float moveUp = 0;
        float moveForward = 0;
        float speed = 0.3f; //[m / pressEvent]
        moveRight += event->key() == Qt::Key_D ?  speed : 0.f;
        moveRight += event->key() == Qt::Key_A ? -speed : 0.f;
        moveForward += event->key() == Qt::Key_W ?  speed : 0.f;
        moveForward += event->key() == Qt::Key_S ? -speed : 0.f;
        moveUp += event->key() == Qt::Key_E ?  speed : 0.f;
        moveUp += event->key() == Qt::Key_Q ? -speed : 0.f;

        mVulkanRenderer->moveCamera(moveRight, moveUp, moveForward);
        requestUpdate();
    }
    QVulkanWindow::keyPressEvent(event);
}

void VulkanWindow::mousePressEvent(QMouseEvent *event)
{
    if (event
     && event->button() & Qt::LeftButton) {
        mPrevCursorPosition = event->localPos();
    }
    QVulkanWindow::mousePressEvent(event);
}
void VulkanWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mVulkanRenderer
     && event
     && (event->buttons() & Qt::LeftButton)) {
        QPointF mouseMove = event->localPos() - mPrevCursorPosition;
        mPrevCursorPosition = event->localPos();

        float speed = 0.5f; // [degree / pixel]
        mVulkanRenderer->rotateCamera(speed * static_cast<float>(mouseMove.y()), speed * static_cast<float>(mouseMove.x()), 0.f);
        requestUpdate();
    }
    QVulkanWindow::mouseMoveEvent(event);
}
