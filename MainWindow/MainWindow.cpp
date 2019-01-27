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

#include "MainWindow.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QPlainTextEdit>

#include <QVulkanInstance>
//#include <vulkan/vulkan.hpp>
#include "VulkanWindow.hpp"

#include <VulkanTools.hpp>

void MainWindow::initGui()
{
    if (mMainWidget) {
        return;
    }

    setWindowTitle(QApplication::translate("main", "3D Model Scaner"));

    mMainWidget = new QWidget(this);
    this->setCentralWidget(mMainWidget);

    QLayout* mainLayout = new QVBoxLayout(mMainWidget);
    mMainWidget->setLayout(mainLayout);

    mButtonTurnOnOffVulkanWin = new QPushButton(QApplication::translate("mainWin", "Show Vulkan"), this);
    connect(mButtonTurnOnOffVulkanWin, &QPushButton::clicked, this, &MainWindow::toogleVulkanWindow);
    mainLayout->addWidget(mButtonTurnOnOffVulkanWin);

    QAbstractButton* buttonDisplayVulkanInfo = new QPushButton(QApplication::translate("mainWin", "Display Vulkan Info"), this);
    connect(buttonDisplayVulkanInfo, &QPushButton::clicked, this, &MainWindow::displayVulkanInfo);
    mainLayout->addWidget(buttonDisplayVulkanInfo);

    initVulkanWindow();
}

void MainWindow::initVulkanWindow()
{
    if (mVulkanWindow) {
        delete mVulkanWindow;
    }
    mVulkanWindow = new VulkanWindow();
    mVulkanWindow->setTitle(QApplication::translate("main", "Vulkan display"));
    mVulkanWindow->setVulkanInstance(mVulkanInstance);
    mVulkanWindow->setWidth(800);
    mVulkanWindow->setHeight(600);
    mVulkanWindow->create();
    QVulkanWindowRenderer* vulkanRenderer = mVulkanWindow->createRenderer();
    qInfo("vulkanRenderer = %p\n", vulkanRenderer);
}

void MainWindow::setVulkanInstance(QVulkanInstance* vkInstance)
{
    mVulkanInstance = vkInstance;
    if (mVulkanWindow) {
        if (!mVulkanWindow->isValid()) {
            mVulkanWindow->setVulkanInstance(mVulkanInstance);
        }
    }
}

void MainWindow::toogleVulkanWindow()
{
    if (!mVulkanWindow) {
        return;
    }
    mVulkanWindow->setVisible(!mVulkanWindow->isVisible());
    mVulkanWindow->setX(x() + 50);
    mVulkanWindow->setY(y() + 50);

    if (mButtonTurnOnOffVulkanWin) {
        if (mVulkanWindow->isVisible())
            mButtonTurnOnOffVulkanWin->setText(QApplication::translate("mainWin", "Hide Vulkan"));
        else
            mButtonTurnOnOffVulkanWin->setText(QApplication::translate("mainWin", "Show Vulkan"));
    }
}

void MainWindow::displayVulkanInfo()
{
    QDialog infoDialog(this);
    infoDialog.setLayout(new QVBoxLayout(&infoDialog));

    static const QString o("\n   "); //text offset

    QString text;
    if (!mVulkanInstance) {
        text += QApplication::translate("displayVulkanInfo", "Vulkan instance was't provided.\n");
    }
    else {
        QVulkanInfoVector<QVulkanLayer> supportedLayers = mVulkanInstance->supportedLayers();
        text += QApplication::translate("displayVulkanInfo", "Number of supported layers: %n.", "", supportedLayers.size());
        for (int i = 0; i < supportedLayers.size(); ++i) {
            text += QString("\n%1. %2 (%3 - %4) - %5")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(supportedLayers[i].name.data())
                            .arg(supportedLayers[i].specVersion.toString())
                            .arg(supportedLayers[i].version)
                            .arg(supportedLayers[i].description.data());
        }

        text += "\n\n";

        QVulkanInfoVector<QVulkanExtension> supportedExt = mVulkanInstance->supportedExtensions();
        text += QApplication::translate("displayVulkanInfo", "Number of supported extensions: %n.", "", supportedExt.size());
        for (int i = 0; i < supportedExt.size(); ++i) {
            text += o + QString("%1. %2 version: %3")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(supportedExt[i].name.data())
                            .arg(supportedExt[i].version);
        }

        text += "\n\n";

        text += QApplication::translate("displayVulkanInfo", "Current instance values:");
        text += o + QApplication::translate("displayVulkanInfo", "NoDebugOutputRedirect: ") + (mVulkanInstance->flags().testFlag(QVulkanInstance::NoDebugOutputRedirect) ? '1' : '0');
        text += o + QApplication::translate("displayVulkanInfo", "Api ver: ") + mVulkanInstance->apiVersion().toString();

        QByteArrayList layers = mVulkanInstance->layers();
        for (int i = 0; i < layers.size(); ++i) {
            text += o + QString("Layer-%1: %2")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(layers[i].data());
        }

        QByteArrayList extensions = mVulkanInstance->extensions();
        for (int i = 0; i < extensions.size(); ++i) {
            text += o + QString("Extension-%1: %2")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(extensions[i].data());
        }
    }

    text += "\n\n";

    if (!mVulkanWindow) {
        text += QApplication::translate("displayVulkanInfo", "Vulkan window was't provided.\n");
    }
    else {
        text += QApplication::translate("displayVulkanInfo", "Current window values:");
        text += o + QApplication::translate("displayVulkanInfo", "Is valid: ") + (mVulkanWindow->isValid() ? '1' : '0');
        text += o + QApplication::translate("displayVulkanInfo", "Support grab: ") + (mVulkanWindow->supportsGrab() ? '1' : '0');
        text += o + QApplication::translate("displayVulkanInfo", "PersistentResources: ") + (mVulkanWindow->flags().testFlag(QVulkanWindow::PersistentResources) ? '1' : '0');

        text += "\n\n";

        QVector<VkPhysicalDeviceProperties> physicalDevicesProps = mVulkanWindow->availablePhysicalDevices();
        text += QApplication::translate("displayVulkanInfo", "Available physical devices: %n", "", physicalDevicesProps.size());

        text += "\n\n";

        for (int i = 0; i < physicalDevicesProps.size(); ++i) {
            text += QApplication::translate("displayVulkanInfo", "Physical device idx: %n", "", i);
            VkPhysicalDeviceProperties& pdp = physicalDevicesProps[i];
            text += o + QApplication::translate("displayVulkanInfo", "Api version   : %1").arg(pdp.apiVersion);
            text += o + QApplication::translate("displayVulkanInfo", "Driver version: %1").arg(pdp.driverVersion);
            text += o + QApplication::translate("displayVulkanInfo", "Vendor ID     : %1").arg(pdp.vendorID);
            text += o + QApplication::translate("displayVulkanInfo", "Device ID     : %1").arg(pdp.deviceID);
            text += o + QApplication::translate("displayVulkanInfo", "Device type   : %1").arg(VkPhysicalDeviceTypeToStr(pdp.deviceType));
            text += o + QApplication::translate("displayVulkanInfo", "Device name   : %1").arg(pdp.deviceName);
            text += o + QApplication::translate("displayVulkanInfo", "Pipeline cache UUID: %1%2").arg(*reinterpret_cast<uint64_t*>(pdp.pipelineCacheUUID), 0, 16)
                                                                                                 .arg(*reinterpret_cast<uint64_t*>(pdp.pipelineCacheUUID+8), 0, 16);

            text += "\n";

            text += o + QApplication::translate("displayVulkanInfo", "Limits:");
            text += o + QApplication::translate("displayVulkanInfo", "maxImageDimension1D       : %1").arg(pdp.limits.maxImageDimension1D       );
            text += o + QApplication::translate("displayVulkanInfo", "maxImageDimension2D       : %1").arg(pdp.limits.maxImageDimension2D       );
            text += o + QApplication::translate("displayVulkanInfo", "maxImageDimension3D       : %1").arg(pdp.limits.maxImageDimension3D       );
            text += o + QApplication::translate("displayVulkanInfo", "maxImageDimensionCube     : %1").arg(pdp.limits.maxImageDimensionCube     );
            text += o + QApplication::translate("displayVulkanInfo", "maxImageArrayLayers       : %1").arg(pdp.limits.maxImageArrayLayers       );
            text += o + QApplication::translate("displayVulkanInfo", "maxTexelBufferElements    : %1").arg(pdp.limits.maxTexelBufferElements    );
            text += o + QApplication::translate("displayVulkanInfo", "maxUniformBufferRange     : %1").arg(pdp.limits.maxUniformBufferRange     );
            text += o + QApplication::translate("displayVulkanInfo", "maxStorageBufferRange     : %1").arg(pdp.limits.maxStorageBufferRange     );
            text += o + QApplication::translate("displayVulkanInfo", "maxPushConstantsSize      : %1").arg(pdp.limits.maxPushConstantsSize      );
            text += o + QApplication::translate("displayVulkanInfo", "maxMemoryAllocationCount  : %1").arg(pdp.limits.maxMemoryAllocationCount  );
            text += o + QApplication::translate("displayVulkanInfo", "maxSamplerAllocationCount : %1").arg(pdp.limits.maxSamplerAllocationCount );
            text += o + QApplication::translate("displayVulkanInfo", "bufferImageGranularity    : %1").arg(pdp.limits.bufferImageGranularity    );
            text += o + QApplication::translate("displayVulkanInfo", "sparseAddressSpaceSize    : %1").arg(pdp.limits.sparseAddressSpaceSize    );
            text += o + QApplication::translate("displayVulkanInfo", "maxBoundDescriptorSets                : %1").arg(pdp.limits.maxBoundDescriptorSets               );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorSamplers         : %1").arg(pdp.limits.maxPerStageDescriptorSamplers        );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorUniformBuffers   : %1").arg(pdp.limits.maxPerStageDescriptorUniformBuffers  );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorStorageBuffers   : %1").arg(pdp.limits.maxPerStageDescriptorStorageBuffers  );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorSampledImages    : %1").arg(pdp.limits.maxPerStageDescriptorSampledImages   );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorStorageImages    : %1").arg(pdp.limits.maxPerStageDescriptorStorageImages   );
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageDescriptorInputAttachments : %1").arg(pdp.limits.maxPerStageDescriptorInputAttachments);
            text += o + QApplication::translate("displayVulkanInfo", "maxPerStageResources                  : %1").arg(pdp.limits.maxPerStageResources                 );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetSamplers              : %1").arg(pdp.limits.maxDescriptorSetSamplers             );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetUniformBuffers        : %1").arg(pdp.limits.maxDescriptorSetUniformBuffers       );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetUniformBuffersDynamic : %1").arg(pdp.limits.maxDescriptorSetUniformBuffersDynamic);
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetStorageBuffers        : %1").arg(pdp.limits.maxDescriptorSetStorageBuffers       );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetStorageBuffersDynamic : %1").arg(pdp.limits.maxDescriptorSetStorageBuffersDynamic);
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetSampledImages         : %1").arg(pdp.limits.maxDescriptorSetSampledImages        );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetStorageImages         : %1").arg(pdp.limits.maxDescriptorSetStorageImages        );
            text += o + QApplication::translate("displayVulkanInfo", "maxDescriptorSetInputAttachments      : %1").arg(pdp.limits.maxDescriptorSetInputAttachments     );
            text += o + QApplication::translate("displayVulkanInfo", "maxVertexInputAttributes              : %1").arg(pdp.limits.maxVertexInputAttributes             );
            text += o + QApplication::translate("displayVulkanInfo", "maxVertexInputBindings                : %1").arg(pdp.limits.maxVertexInputBindings               );
            text += o + QApplication::translate("displayVulkanInfo", "maxVertexInputAttributeOffset         : %1").arg(pdp.limits.maxVertexInputAttributeOffset        );
            text += o + QApplication::translate("displayVulkanInfo", "maxVertexInputBindingStride           : %1").arg(pdp.limits.maxVertexInputBindingStride          );
            text += o + QApplication::translate("displayVulkanInfo", "maxVertexOutputComponents             : %1").arg(pdp.limits.maxVertexOutputComponents            );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationGenerationLevel        : %1").arg(pdp.limits.maxTessellationGenerationLevel       );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationPatchSize              : %1").arg(pdp.limits.maxTessellationPatchSize             );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationControlPerVertexInputComponents : %1").arg(pdp.limits.maxTessellationControlPerVertexInputComponents );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationControlPerVertexOutputComponents: %1").arg(pdp.limits.maxTessellationControlPerVertexOutputComponents);
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationControlPerPatchOutputComponents : %1").arg(pdp.limits.maxTessellationControlPerPatchOutputComponents );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationControlTotalOutputComponents    : %1").arg(pdp.limits.maxTessellationControlTotalOutputComponents    );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationEvaluationInputComponents       : %1").arg(pdp.limits.maxTessellationEvaluationInputComponents       );
            text += o + QApplication::translate("displayVulkanInfo", "maxTessellationEvaluationOutputComponents      : %1").arg(pdp.limits.maxTessellationEvaluationOutputComponents      );
            text += o + QApplication::translate("displayVulkanInfo", "maxGeometryShaderInvocations        : %1").arg(pdp.limits.maxGeometryShaderInvocations       );
            text += o + QApplication::translate("displayVulkanInfo", "maxGeometryInputComponents          : %1").arg(pdp.limits.maxGeometryInputComponents         );
            text += o + QApplication::translate("displayVulkanInfo", "maxGeometryOutputComponents         : %1").arg(pdp.limits.maxGeometryOutputComponents        );
            text += o + QApplication::translate("displayVulkanInfo", "maxGeometryOutputVertices           : %1").arg(pdp.limits.maxGeometryOutputVertices          );
            text += o + QApplication::translate("displayVulkanInfo", "maxGeometryTotalOutputComponents    : %1").arg(pdp.limits.maxGeometryTotalOutputComponents   );
            text += o + QApplication::translate("displayVulkanInfo", "maxFragmentInputComponents          : %1").arg(pdp.limits.maxFragmentInputComponents         );
            text += o + QApplication::translate("displayVulkanInfo", "maxFragmentOutputAttachments        : %1").arg(pdp.limits.maxFragmentOutputAttachments       );
            text += o + QApplication::translate("displayVulkanInfo", "maxFragmentDualSrcAttachments       : %1").arg(pdp.limits.maxFragmentDualSrcAttachments      );
            text += o + QApplication::translate("displayVulkanInfo", "maxFragmentCombinedOutputResources  : %1").arg(pdp.limits.maxFragmentCombinedOutputResources );
            text += o + QApplication::translate("displayVulkanInfo", "maxComputeSharedMemorySize          : %1").arg(pdp.limits.maxComputeSharedMemorySize         );
            text += o + QApplication::translate("displayVulkanInfo", "maxComputeWorkGroupCount[3]         : [%1][%2][%3]").arg(pdp.limits.maxComputeWorkGroupCount[0])
                                                                                                                          .arg(pdp.limits.maxComputeWorkGroupCount[1])
                                                                                                                          .arg(pdp.limits.maxComputeWorkGroupCount[2]);
            text += o + QApplication::translate("displayVulkanInfo", "maxComputeWorkGroupInvocations      : %1").arg(pdp.limits.maxComputeWorkGroupInvocations    );
            text += o + QApplication::translate("displayVulkanInfo", "maxComputeWorkGroupSize[3]          : [%1][%2][%3]").arg(pdp.limits.maxComputeWorkGroupSize[0])
                                                                                                                          .arg(pdp.limits.maxComputeWorkGroupSize[1])
                                                                                                                          .arg(pdp.limits.maxComputeWorkGroupSize[2]);
            text += o + QApplication::translate("displayVulkanInfo", "subPixelPrecisionBits               : %1").arg(pdp.limits.subPixelPrecisionBits             );
            text += o + QApplication::translate("displayVulkanInfo", "subTexelPrecisionBits               : %1").arg(pdp.limits.subTexelPrecisionBits             );
            text += o + QApplication::translate("displayVulkanInfo", "mipmapPrecisionBits                 : %1").arg(pdp.limits.mipmapPrecisionBits               );
            text += o + QApplication::translate("displayVulkanInfo", "maxDrawIndexedIndexValue            : %1").arg(pdp.limits.maxDrawIndexedIndexValue          );
            text += o + QApplication::translate("displayVulkanInfo", "maxDrawIndirectCount                : %1").arg(pdp.limits.maxDrawIndirectCount              );
            text += o + QApplication::translate("displayVulkanInfo", "maxSamplerLodBias                   : %1").arg(static_cast<double>(pdp.limits.maxSamplerLodBias));
            text += o + QApplication::translate("displayVulkanInfo", "maxSamplerAnisotropy                : %1").arg(static_cast<double>(pdp.limits.maxSamplerAnisotropy));
            text += o + QApplication::translate("displayVulkanInfo", "maxViewports                        : %1").arg(pdp.limits.maxViewports                      );
            text += o + QApplication::translate("displayVulkanInfo", "maxViewportDimensions[2]            : [%1][%2]").arg(pdp.limits.maxViewportDimensions[0])
                                                                                                                      .arg(pdp.limits.maxViewportDimensions[1]);
            text += o + QApplication::translate("displayVulkanInfo", "viewportBoundsRange[2]              : [%1][%2]").arg(static_cast<double>(pdp.limits.viewportBoundsRange[0]))
                                                                                                                      .arg(static_cast<double>(pdp.limits.viewportBoundsRange[1]));
            text += o + QApplication::translate("displayVulkanInfo", "viewportSubPixelBits                : %1").arg(pdp.limits.viewportSubPixelBits              );
            text += o + QApplication::translate("displayVulkanInfo", "minMemoryMapAlignment               : %1").arg(pdp.limits.minMemoryMapAlignment             );
            text += o + QApplication::translate("displayVulkanInfo", "minTexelBufferOffsetAlignment       : %1").arg(pdp.limits.minTexelBufferOffsetAlignment     );
            text += o + QApplication::translate("displayVulkanInfo", "minUniformBufferOffsetAlignment     : %1").arg(pdp.limits.minUniformBufferOffsetAlignment   );
            text += o + QApplication::translate("displayVulkanInfo", "minStorageBufferOffsetAlignment     : %1").arg(pdp.limits.minStorageBufferOffsetAlignment   );
            text += o + QApplication::translate("displayVulkanInfo", "minTexelOffset                      : %1").arg(pdp.limits.minTexelOffset                    );
            text += o + QApplication::translate("displayVulkanInfo", "maxTexelOffset                      : %1").arg(pdp.limits.maxTexelOffset                    );
            text += o + QApplication::translate("displayVulkanInfo", "minTexelGatherOffset                : %1").arg(pdp.limits.minTexelGatherOffset              );
            text += o + QApplication::translate("displayVulkanInfo", "maxTexelGatherOffset                : %1").arg(pdp.limits.maxTexelGatherOffset              );
            text += o + QApplication::translate("displayVulkanInfo", "minInterpolationOffset              : %1").arg(static_cast<double>(pdp.limits.minInterpolationOffset));
            text += o + QApplication::translate("displayVulkanInfo", "maxInterpolationOffset              : %1").arg(static_cast<double>(pdp.limits.maxInterpolationOffset));
            text += o + QApplication::translate("displayVulkanInfo", "subPixelInterpolationOffsetBits     : %1").arg(pdp.limits.subPixelInterpolationOffsetBits   );
            text += o + QApplication::translate("displayVulkanInfo", "maxFramebufferWidth                 : %1").arg(pdp.limits.maxFramebufferWidth               );
            text += o + QApplication::translate("displayVulkanInfo", "maxFramebufferHeight                : %1").arg(pdp.limits.maxFramebufferHeight              );
            text += o + QApplication::translate("displayVulkanInfo", "maxFramebufferLayers                : %1").arg(pdp.limits.maxFramebufferLayers              );
            text += o + QApplication::translate("displayVulkanInfo", "framebufferColorSampleCounts        : %1").arg(pdp.limits.framebufferColorSampleCounts      );
            text += o + QApplication::translate("displayVulkanInfo", "framebufferDepthSampleCounts        : %1").arg(pdp.limits.framebufferDepthSampleCounts      );
            text += o + QApplication::translate("displayVulkanInfo", "framebufferStencilSampleCounts      : %1").arg(pdp.limits.framebufferStencilSampleCounts    );
            text += o + QApplication::translate("displayVulkanInfo", "framebufferNoAttachmentsSampleCounts: %1").arg(pdp.limits.framebufferNoAttachmentsSampleCounts);
            text += o + QApplication::translate("displayVulkanInfo", "maxColorAttachments                 : %1").arg(pdp.limits.maxColorAttachments               );
            text += o + QApplication::translate("displayVulkanInfo", "sampledImageColorSampleCounts       : %1").arg(pdp.limits.sampledImageColorSampleCounts     );
            text += o + QApplication::translate("displayVulkanInfo", "sampledImageIntegerSampleCounts     : %1").arg(pdp.limits.sampledImageIntegerSampleCounts   );
            text += o + QApplication::translate("displayVulkanInfo", "sampledImageDepthSampleCounts       : %1").arg(pdp.limits.sampledImageDepthSampleCounts     );
            text += o + QApplication::translate("displayVulkanInfo", "sampledImageStencilSampleCounts     : %1").arg(pdp.limits.sampledImageStencilSampleCounts   );
            text += o + QApplication::translate("displayVulkanInfo", "storageImageSampleCounts            : %1").arg(pdp.limits.storageImageSampleCounts          );
            text += o + QApplication::translate("displayVulkanInfo", "maxSampleMaskWords                  : %1").arg(pdp.limits.maxSampleMaskWords                );
            text += o + QApplication::translate("displayVulkanInfo", "timestampComputeAndGraphics         : %1").arg(pdp.limits.timestampComputeAndGraphics       );
            text += o + QApplication::translate("displayVulkanInfo", "timestampPeriod                     : %1").arg(static_cast<double>(pdp.limits.timestampPeriod));
            text += o + QApplication::translate("displayVulkanInfo", "maxClipDistances                    : %1").arg(pdp.limits.maxClipDistances                  );
            text += o + QApplication::translate("displayVulkanInfo", "maxCullDistances                    : %1").arg(pdp.limits.maxCullDistances                  );
            text += o + QApplication::translate("displayVulkanInfo", "maxCombinedClipAndCullDistances     : %1").arg(pdp.limits.maxCombinedClipAndCullDistances   );
            text += o + QApplication::translate("displayVulkanInfo", "discreteQueuePriorities             : %1").arg(pdp.limits.discreteQueuePriorities           );
            text += o + QApplication::translate("displayVulkanInfo", "pointSizeRange[2]                   : [%1][%2]").arg(static_cast<double>(pdp.limits.pointSizeRange[0]))
                                                                                                                      .arg(static_cast<double>(pdp.limits.pointSizeRange[1]));
            text += o + QApplication::translate("displayVulkanInfo", "lineWidthRange[2]                   : [%1][%2]").arg(static_cast<double>(pdp.limits.lineWidthRange[0]))
                                                                                                                      .arg(static_cast<double>(pdp.limits.lineWidthRange[1]));
            text += o + QApplication::translate("displayVulkanInfo", "pointSizeGranularity                : %1").arg(static_cast<double>(pdp.limits.pointSizeGranularity));
            text += o + QApplication::translate("displayVulkanInfo", "lineWidthGranularity                : %1").arg(static_cast<double>(pdp.limits.lineWidthGranularity));
            text += o + QApplication::translate("displayVulkanInfo", "strictLines                         : %1").arg(pdp.limits.strictLines                       );
            text += o + QApplication::translate("displayVulkanInfo", "standardSampleLocations             : %1").arg(pdp.limits.standardSampleLocations           );
            text += o + QApplication::translate("displayVulkanInfo", "optimalBufferCopyOffsetAlignment    : %1").arg(pdp.limits.optimalBufferCopyOffsetAlignment  );
            text += o + QApplication::translate("displayVulkanInfo", "optimalBufferCopyRowPitchAlignment  : %1").arg(pdp.limits.optimalBufferCopyRowPitchAlignment);
            text += o + QApplication::translate("displayVulkanInfo", "nonCoherentAtomSize                 : %1").arg(pdp.limits.nonCoherentAtomSize               );

            text += "\n";

            text += o + QApplication::translate("displayVulkanInfo", "Sparse properties:");
            text += o + QApplication::translate("displayVulkanInfo", "residencyStandard2DBlockShape            : %1").arg(pdp.sparseProperties.residencyStandard2DBlockShape);
            text += o + QApplication::translate("displayVulkanInfo", "residencyStandard2DMultisampleBlockShape : %1").arg(pdp.sparseProperties.residencyStandard2DMultisampleBlockShape);
            text += o + QApplication::translate("displayVulkanInfo", "residencyStandard3DBlockShape            : %1").arg(pdp.sparseProperties.residencyStandard3DBlockShape);
            text += o + QApplication::translate("displayVulkanInfo", "residencyAlignedMipSize                  : %1").arg(pdp.sparseProperties.residencyAlignedMipSize      );
            text += o + QApplication::translate("displayVulkanInfo", "residencyNonResidentStrict               : %1").arg(pdp.sparseProperties.residencyNonResidentStrict   );
        }

        text += "\n\n";

        QVulkanInfoVector<QVulkanExtension> currentDeviceExt = mVulkanWindow->supportedDeviceExtensions();
        text += QApplication::translate("displayVulkanInfo", "Number of supported current device extensions: %n.", "", currentDeviceExt.size());
        for (int i = 0; i < currentDeviceExt.size(); ++i) {
            text += o + QString("%1. %2 version: %3")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(currentDeviceExt[i].name.data())
                            .arg(currentDeviceExt[i].version);
        }

        text += "\n\n";

        QVector<int> supportedSampleCounts = mVulkanWindow->supportedSampleCounts();
        text += QApplication::translate("displayVulkanInfo", "Number of supported sample counts: %n.", "", supportedSampleCounts.size());
        for (int i = 0; i < supportedSampleCounts.size(); ++i) {
            text += o + QString("%1 - %2")
                            .arg(i+1, 2, 10, QChar('0'))
                            .arg(supportedSampleCounts[i]);
        }

        text += "\n";

        text += QApplication::translate("displayVulkanInfo", "\nPhysical device: ") + QString().asprintf("%p", mVulkanWindow->physicalDevice());
        text += QApplication::translate("displayVulkanInfo", "\nPhysical device props: ") + QString().asprintf("%p", mVulkanWindow->physicalDeviceProperties());
        text += QApplication::translate("displayVulkanInfo", "\nDevice: ") + QString().asprintf("%p", mVulkanWindow->device());
        text += QApplication::translate("displayVulkanInfo", "\nQueue: ") + QString().asprintf("%p", mVulkanWindow->graphicsQueue());
        text += QApplication::translate("displayVulkanInfo", "\nCmd pool: ") + QString().asprintf("%p", mVulkanWindow->graphicsCommandPool());
        text += QApplication::translate("displayVulkanInfo", "\nHost mem Idx: ") + QString().asprintf("%d", mVulkanWindow->hostVisibleMemoryIndex());
        text += QApplication::translate("displayVulkanInfo", "\nDev  mem Idx: ") + QString().asprintf("%d", mVulkanWindow->deviceLocalMemoryIndex());
        text += QApplication::translate("displayVulkanInfo", "\nRender pass: ") + QString().asprintf("%p", mVulkanWindow->defaultRenderPass());


        text += QApplication::translate("displayVulkanInfo", "\nColor format: %1").arg(VkFormatToStr(mVulkanWindow->colorFormat()));
        text += QApplication::translate("displayVulkanInfo", "\nDepth stencil format: %1").arg(VkFormatToStr(mVulkanWindow->depthStencilFormat()));
        QSize sciSize = mVulkanWindow->swapChainImageSize();
        text += QApplication::translate("displayVulkanInfo", "\nSwapchain image size: %1x%2").arg(sciSize.width()).arg(sciSize.height());
        text += QApplication::translate("displayVulkanInfo", "\nCurrent cmd buf: ") + QString().asprintf("%p", mVulkanWindow->currentCommandBuffer());
        text += QApplication::translate("displayVulkanInfo", "\nCurrent frame buf: ") + QString().asprintf("%p", mVulkanWindow->currentFramebuffer());
        text += QApplication::translate("displayVulkanInfo", "\nConcurrnet frame count: %1").arg(mVulkanWindow->concurrentFrameCount());
        text += QApplication::translate("displayVulkanInfo", "\nSwapChain image count: %1").arg(mVulkanWindow->swapChainImageCount());
        text += QApplication::translate("displayVulkanInfo", "\nCurrent swapchain image idx: %1").arg(mVulkanWindow->currentSwapChainImageIndex());

        for (int i = 0; i < mVulkanWindow->swapChainImageCount(); ++i) {
            text += o + QApplication::translate("displayVulkanInfo", "Swapchain img %1 - ").arg(i) + QString().asprintf("%p", mVulkanWindow->swapChainImage(i));
            text += o + QApplication::translate("displayVulkanInfo", "Swapchain img view %1 - ").arg(i) + QString().asprintf("%p", mVulkanWindow->swapChainImageView(i));
            //Below: Multisample color image, or VK_NULL_HANDLE if multisampling is not in use.
            text += o + QApplication::translate("displayVulkanInfo", "Msaa Color Image %1 - ").arg(i) + QString().asprintf("%p", mVulkanWindow->msaaColorImage(i));
            text += o + QApplication::translate("displayVulkanInfo", "Msaa Color Image view %1 - ").arg(i) + QString().asprintf("%p", mVulkanWindow->msaaColorImageView(i));
        }

        text += QApplication::translate("displayVulkanInfo", "\nSwapchain depth/stencil - ") + QString().asprintf("%p", mVulkanWindow->depthStencilImage());
        text += QApplication::translate("displayVulkanInfo", "\nSwapchain depth/stencil view - ") + QString().asprintf("%p", mVulkanWindow->depthStencilImageView());

        text += QApplication::translate("displayVulkanInfo", "\nSample Count Flag Bits: %1").arg(mVulkanWindow->sampleCountFlagBits());
    }

    QPlainTextEdit* textField = new QPlainTextEdit(&infoDialog);
    textField->setReadOnly(true);

    QFont font = textField->font();
    font.setStyleHint(QFont::Monospace);
    textField->setFont(font);

    textField->setPlainText(text);
    infoDialog.layout()->addWidget(textField);

    int lines = text.count('\n') + 1;
    int linesHeight = lines * textField->fontMetrics().height();
    double aspect = 800.0 / 600.0;
    if (linesHeight > 1080) { // add display dimensions
        linesHeight = 1080;
    }
    else if (linesHeight < 320) {
        linesHeight = 320;
    }
    int newWidth = aspect * linesHeight;
    if (newWidth > 1920) {
        newWidth = 1920;
    }

    infoDialog.setModal(true);
    infoDialog.resize(newWidth, linesHeight);
    infoDialog.show();
    infoDialog.exec();
}
