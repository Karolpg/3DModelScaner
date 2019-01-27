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


#include <iostream>

#include <QApplication>
#include "MainWindow.hpp"

#include <assert.h>

//require QT version 5.10 minimum
#include <QVulkanInstance>

int main(int argc, char **argv) {
    qInfo("3D Model Scaner start\n");

    QApplication app(argc, argv); //Have to be first e.g. before QVulcanInstance

    QVulkanInstance vkInstance;

#ifndef Q_OS_ANDROID
    vkInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#else
    vkInstance.setLayers(QByteArrayList()
                       << "VK_LAYER_GOOGLE_threading"
                       << "VK_LAYER_LUNARG_parameter_validation"
                       << "VK_LAYER_LUNARG_object_tracker"
                       << "VK_LAYER_LUNARG_core_validation"
                       << "VK_LAYER_LUNARG_image"
                       << "VK_LAYER_LUNARG_swapchain"
                       << "VK_LAYER_GOOGLE_unique_objects");
#endif

    qInfo("Creating Vulkan instance...");
    if (!vkInstance.create()) {
        qFatal("Failed to create Vulkan instance: %d", vkInstance.errorCode());
    }
    qInfo("Vulkan instance created.");

    MainWindow mainWindow;
    mainWindow.initGui();
    mainWindow.setVulkanInstance(&vkInstance);
    mainWindow.resize(800, 600);
    mainWindow.show();

    int retVal = app.exec();
    qInfo("3D Model Scaner end. Return value: %d\n", retVal);

    vkInstance.destroy();

    return retVal;
}







//void init_vk() {
//    uint32_t instance_extension_count = 0;
//    uint32_t instance_layer_count = 0;
//    uint32_t validation_layer_count = 0;
//    char const *const *instance_validation_layers = nullptr;
//    int enabled_extension_count = 0;
//    int enabled_layer_count = 0;
//    vk::Instance inst;
//    vk::PhysicalDevice gpu;
//    vk::PhysicalDeviceProperties gpu_props;

//    char const *const instance_validation_layers_alt1[] = {"VK_LAYER_LUNARG_standard_validation"};

//    char const *const instance_validation_layers_alt2[] = {"VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation",
//                                                           "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_core_validation",
//                                                           "VK_LAYER_GOOGLE_unique_objects"};

//    // Look for validation layers
////    vk::Bool32 validation_found = VK_FALSE;
////    if (m_validate) {
////        auto result = vk::enumerateInstanceLayerProperties(&instance_layer_count, nullptr);
////        VERIFY(result == vk::Result::eSuccess);
////        qInfo("Number of instance layer properties: %d\n", instance_layer_count);

////        instance_validation_layers = instance_validation_layers_alt1;
////        if (instance_layer_count > 0) {
////            std::unique_ptr<vk::LayerProperties[]> instance_layers(new vk::LayerProperties[instance_layer_count]);
////            result = vk::enumerateInstanceLayerProperties(&instance_layer_count, instance_layers.get());
////            VERIFY(result == vk::Result::eSuccess);

////            validation_found = check_layers(ARRAY_SIZE(instance_validation_layers_alt1), instance_validation_layers,
////                                            instance_layer_count, instance_layers.get());
////            if (validation_found) {
////                enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt1);
////                enabled_layers[0] = "VK_LAYER_LUNARG_standard_validation";
////                validation_layer_count = 1;
////            } else {
////                // use alternative set of validation layers
////                instance_validation_layers = instance_validation_layers_alt2;
////                enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
////                validation_found = check_layers(ARRAY_SIZE(instance_validation_layers_alt2), instance_validation_layers,
////                                                instance_layer_count, instance_layers.get());
////                validation_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
////                for (uint32_t i = 0; i < validation_layer_count; i++) {
////                    enabled_layers[i] = instance_validation_layers[i];
////                }
////            }
////        }

////        if (!validation_found) {
////            ERR_EXIT(
////                "vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
////                "Please look at the Getting Started guide for additional information.\n",
////                "vkCreateInstance Failure");
////        }
////    }

//    /* Look for instance extensions */
//    vk::Bool32 surfaceExtFound = VK_FALSE;
//    vk::Bool32 platformSurfaceExtFound = VK_FALSE;
//    char* extension_names[128];
//    memset(extension_names, 0, sizeof(extension_names));

//    auto result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
//    assert(result == vk::Result::eSuccess);
//    qInfo("Number of instance extension: %d\n", instance_extension_count);

//    if (instance_extension_count > 0) {
//        std::unique_ptr<vk::ExtensionProperties[]> instance_extensions(new vk::ExtensionProperties[instance_extension_count]);
//        result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, instance_extensions.get());
//        assert(result == vk::Result::eSuccess);

//        for (uint32_t i = 0; i < instance_extension_count; i++) {
//            qInfo("Extension: %d - %s - 0x%X\n", i, instance_extensions[i].extensionName, instance_extensions[i].specVersion);
//            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                surfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
//            }
//#if defined(VK_USE_PLATFORM_WIN32_KHR)
//            if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
//            }
//#elif defined(VK_USE_PLATFORM_XLIB_KHR)
//            if (!strcmp(VK_KHR_XLIB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
//            }
//#elif defined(VK_USE_PLATFORM_XCB_KHR)
//            if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
//            }
//#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
//            if (!strcmp(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
//            }
//#elif defined(VK_USE_PLATFORM_MIR_KHR)
//#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
//            if (!strcmp(VK_KHR_DISPLAY_EXTENSION_NAME, instance_extensions[i].extensionName)) {
//                platformSurfaceExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_DISPLAY_EXTENSION_NAME;
//            }

//#endif
//            assert(enabled_extension_count < 128);
//        }
//    }

//    if (!surfaceExtFound) {
//        qFatal("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME " extension.\n\n"
//                "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                "Please look at the Getting Started guide for additional information.\n"
//                "vkCreateInstance Failure");
//    }

//    if (!platformSurfaceExtFound) {
//#if defined(VK_USE_PLATFORM_WIN32_KHR)
//        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n",
//                 "vkCreateInstance Failure");
//#elif defined(VK_USE_PLATFORM_XCB_KHR)
//        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n",
//                 "vkCreateInstance Failure");
//#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
//        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n",
//                 "vkCreateInstance Failure");
//#elif defined(VK_USE_PLATFORM_MIR_KHR)
//#elif defined(VK_USE_PLATFORM_XLIB_KHR)
//        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_XLIB_SURFACE_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n",
//                 "vkCreateInstance Failure");
//#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
//        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_DISPLAY_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n",
//                 "vkCreateInstance Failure");
//#endif
//    }
//    static const char* APP_SHORT_NAME = "3DModelScaner";
//    auto const app = vk::ApplicationInfo()
//                         .setPApplicationName(APP_SHORT_NAME)
//                         .setApplicationVersion(0)
//                         .setPEngineName(APP_SHORT_NAME)
//                         .setEngineVersion(0)
//                         .setApiVersion(VK_API_VERSION_1_0);
//    auto const inst_info = vk::InstanceCreateInfo()
//                               .setPApplicationInfo(&app)
//                               .setEnabledLayerCount(enabled_layer_count)
//                               .setPpEnabledLayerNames(instance_validation_layers)
//                               .setEnabledExtensionCount(enabled_extension_count)
//                               .setPpEnabledExtensionNames(extension_names);

//    result = vk::createInstance(&inst_info, nullptr, &inst);
//    if (result == vk::Result::eErrorIncompatibleDriver) {
//        qFatal(
//            "Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
//            "Please look at the Getting Started guide for additional information.\n"
//            "vkCreateInstance Failure");
//    } else if (result == vk::Result::eErrorExtensionNotPresent) {
//        qFatal(
//            "Cannot find a specified extension library.\n"
//            "Make sure your layers path is set appropriately.\n"
//            "vkCreateInstance Failure");
//    } else if (result != vk::Result::eSuccess) {
//        qFatal(
//            "vkCreateInstance failed.\n\n"
//            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//            "Please look at the Getting Started guide for additional information.\n"
//            "vkCreateInstance Failure");
//    }

//    /* Make initial call to query gpu_count, then second call for gpu info*/
//    uint32_t gpu_count;
//    result = inst.enumeratePhysicalDevices(&gpu_count, nullptr);
//    assert(result == vk::Result::eSuccess);
//    assert(gpu_count > 0);

//    auto pdt2str = [](vk::PhysicalDeviceType t) {
//            switch (t) {
//                case vk::PhysicalDeviceType::eOther:            return "Other";
//                case vk::PhysicalDeviceType::eIntegratedGpu:    return "Integrated GPU";
//                case vk::PhysicalDeviceType::eDiscreteGpu:      return "Discrete GPU";
//                case vk::PhysicalDeviceType::eVirtualGpu:       return "Virtual GPU";
//                case vk::PhysicalDeviceType::eCpu:              return "CPU";
//                default: break;
//            }
//            return "Unknown";
//    };

//    if (gpu_count > 0) {
//        std::unique_ptr<vk::PhysicalDevice[]> physical_devices(new vk::PhysicalDevice[gpu_count]);
//        result = inst.enumeratePhysicalDevices(&gpu_count, physical_devices.get());
//        assert(result == vk::Result::eSuccess);

//        qInfo ("Number of available physical devices: %d\n", gpu_count);
//        for (int i = 0; i < gpu_count; ++i) {
//            vk::PhysicalDevice& device = physical_devices[i];
//            vk::PhysicalDeviceProperties props;
//            device.getProperties(&props);
//            qInfo("Physical device idx: %d, api version: %d, driver ver.: %d, vendor id: %d, device id: %d, "
//                   "device type: %s, pipeline UUID: %lX%lX, device name: %s"
//                   "\n",
//                   i, props.apiVersion, props.driverVersion, props.vendorID, props.deviceID,
//                   pdt2str(props.deviceType), ((uint64_t*)props.pipelineCacheUUID)[0], ((uint64_t*)props.pipelineCacheUUID)[1],
//                   props.deviceName);
//        }

//        /* For cube demo we just grab the first physical device */
//        gpu = physical_devices[0];
//    } else {
//        qFatal(
//            "vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
//            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//            "Please look at the Getting Started guide for additional information.\n"
//            "vkEnumeratePhysicalDevices Failure");
//    }

//    /* Look for device extensions */
//    uint32_t device_extension_count = 0;
//    vk::Bool32 swapchainExtFound = VK_FALSE;
//    enabled_extension_count = 0;
//    memset(extension_names, 0, sizeof(extension_names));

//    result = gpu.enumerateDeviceExtensionProperties(nullptr, &device_extension_count, nullptr);
//    assert(result == vk::Result::eSuccess);
//    qInfo("Gpu device extension properties count: %d\n", device_extension_count);

//    if (device_extension_count > 0) {
//        std::unique_ptr<vk::ExtensionProperties[]> device_extensions(new vk::ExtensionProperties[device_extension_count]);
//        result = gpu.enumerateDeviceExtensionProperties(nullptr, &device_extension_count, device_extensions.get());
//        assert(result == vk::Result::eSuccess);

//        for (uint32_t i = 0; i < device_extension_count; i++) {
//            qInfo("Gpu device extension %d name: %s\n", i, device_extensions[i].extensionName);
//            if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) {
//                swapchainExtFound = 1;
//                extension_names[enabled_extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
//            }
//            assert(enabled_extension_count < 128);
//        }
//    }

//    if (!swapchainExtFound) {
//        qFatal("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
//                 " extension.\n\n"
//                 "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
//                 "Please look at the Getting Started guide for additional information.\n"
//                 "vkCreateInstance Failure");
//    }

//    gpu.getProperties(&gpu_props);

//    int queue_family_count = 0;
//    std::unique_ptr<vk::QueueFamilyProperties[]> queue_props;
//    /* Call with nullptr data to get count */
//    /*gpu.getQueueFamilyProperties(&queue_family_count, nullptr);
//    assert(queue_family_count >= 1);

//    queue_props.reset(new vk::QueueFamilyProperties[queue_family_count]);
//    gpu.getQueueFamilyProperties(&queue_family_count, queue_props.get());
//*/
//    // Query fine-grained feature support for this device.
//    //  If app has specific feature requirements it should check supported
//    //  features based on this query
//    vk::PhysicalDeviceFeatures physDevFeatures;
//    gpu.getFeatures(&physDevFeatures);
//}
