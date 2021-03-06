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

#include <vulkan/vulkan_core.h>

inline const char* VkResultToStr(VkResult result) {
    switch(result) {
        case VK_SUCCESS                           : return "VK_SUCCESS";
        case VK_NOT_READY                         : return "VK_NOT_READY";
        case VK_TIMEOUT                           : return "VK_TIMEOUT";
        case VK_EVENT_SET                         : return "VK_EVENT_SET";
        case VK_EVENT_RESET                       : return "VK_EVENT_RESET";
        case VK_INCOMPLETE                        : return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY          : return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY        : return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED       : return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST                 : return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED           : return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT           : return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT       : return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT         : return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER         : return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS            : return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED        : return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL             : return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_OUT_OF_POOL_MEMORY          : return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE     : return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_SURFACE_LOST_KHR            : return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR    : return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR                    : return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR             : return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR    : return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT       : return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV           : return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_NOT_PERMITTED_EXT           : return "VK_ERROR_NOT_PERMITTED_EXT";
        //case VK_ERROR_OUT_OF_POOL_MEMORY_KHR      : return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
        //case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR : return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
        //case VK_RESULT_BEGIN_RANGE                : return "VK_RESULT_BEGIN_RANGE";
        //case VK_RESULT_END_RANGE                  : return "VK_RESULT_END_RANGE";
        case VK_RESULT_RANGE_SIZE                 : return "VK_RESULT_RANGE_SIZE";
        case VK_RESULT_MAX_ENUM                   : return "VK_RESULT_MAX_ENUM";
        default: break;
    }
    return "Unknown";
}

inline const char* VkPhysicalDeviceTypeToStr(VkPhysicalDeviceType type) {
    switch(type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER          : return "OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU : return "INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU   : return "DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU    : return "VIRTUAL_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU            : return "CPU";
        //case VK_PHYSICAL_DEVICE_TYPE_BEGIN_RANGE = VK_PHYSICAL_DEVICE_TYPE_OTHER,
        //case VK_PHYSICAL_DEVICE_TYPE_END_RANGE = VK_PHYSICAL_DEVICE_TYPE_CPU,
        case VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE     : return "RANGE_SIZE";
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM       : return "MAX_ENUM";
        default: break;
    }
    return "Unknown";
}

inline const char* VkFormatToStr(VkFormat format) {
    switch(format) {
        case VK_FORMAT_UNDEFINED                                   : return "VK_FORMAT_UNDEFINED";
        case VK_FORMAT_R4G4_UNORM_PACK8                            : return "VK_FORMAT_R4G4_UNORM_PACK8";
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16                       : return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16                       : return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
        case VK_FORMAT_R5G6B5_UNORM_PACK16                         : return "VK_FORMAT_R5G6B5_UNORM_PACK16";
        case VK_FORMAT_B5G6R5_UNORM_PACK16                         : return "VK_FORMAT_B5G6R5_UNORM_PACK16";
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16                       : return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16                       : return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16                       : return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
        case VK_FORMAT_R8_UNORM                                    : return "VK_FORMAT_R8_UNORM";
        case VK_FORMAT_R8_SNORM                                    : return "VK_FORMAT_R8_SNORM";
        case VK_FORMAT_R8_USCALED                                  : return "VK_FORMAT_R8_USCALED";
        case VK_FORMAT_R8_SSCALED                                  : return "VK_FORMAT_R8_SSCALED";
        case VK_FORMAT_R8_UINT                                     : return "VK_FORMAT_R8_UINT";
        case VK_FORMAT_R8_SINT                                     : return "VK_FORMAT_R8_SINT";
        case VK_FORMAT_R8_SRGB                                     : return "VK_FORMAT_R8_SRGB";
        case VK_FORMAT_R8G8_UNORM                                  : return "VK_FORMAT_R8G8_UNORM";
        case VK_FORMAT_R8G8_SNORM                                  : return "VK_FORMAT_R8G8_SNORM";
        case VK_FORMAT_R8G8_USCALED                                : return "VK_FORMAT_R8G8_USCALED";
        case VK_FORMAT_R8G8_SSCALED                                : return "VK_FORMAT_R8G8_SSCALED";
        case VK_FORMAT_R8G8_UINT                                   : return "VK_FORMAT_R8G8_UINT";
        case VK_FORMAT_R8G8_SINT                                   : return "VK_FORMAT_R8G8_SINT";
        case VK_FORMAT_R8G8_SRGB                                   : return "VK_FORMAT_R8G8_SRGB";
        case VK_FORMAT_R8G8B8_UNORM                                : return "VK_FORMAT_R8G8B8_UNORM";
        case VK_FORMAT_R8G8B8_SNORM                                : return "VK_FORMAT_R8G8B8_SNORM";
        case VK_FORMAT_R8G8B8_USCALED                              : return "VK_FORMAT_R8G8B8_USCALED";
        case VK_FORMAT_R8G8B8_SSCALED                              : return "VK_FORMAT_R8G8B8_SSCALED";
        case VK_FORMAT_R8G8B8_UINT                                 : return "VK_FORMAT_R8G8B8_UINT";
        case VK_FORMAT_R8G8B8_SINT                                 : return "VK_FORMAT_R8G8B8_SINT";
        case VK_FORMAT_R8G8B8_SRGB                                 : return "VK_FORMAT_R8G8B8_SRGB";
        case VK_FORMAT_B8G8R8_UNORM                                : return "VK_FORMAT_B8G8R8_UNORM";
        case VK_FORMAT_B8G8R8_SNORM                                : return "VK_FORMAT_B8G8R8_SNORM";
        case VK_FORMAT_B8G8R8_USCALED                              : return "VK_FORMAT_B8G8R8_USCALED";
        case VK_FORMAT_B8G8R8_SSCALED                              : return "VK_FORMAT_B8G8R8_SSCALED";
        case VK_FORMAT_B8G8R8_UINT                                 : return "VK_FORMAT_B8G8R8_UINT";
        case VK_FORMAT_B8G8R8_SINT                                 : return "VK_FORMAT_B8G8R8_SINT";
        case VK_FORMAT_B8G8R8_SRGB                                 : return "VK_FORMAT_B8G8R8_SRGB";
        case VK_FORMAT_R8G8B8A8_UNORM                              : return "VK_FORMAT_R8G8B8A8_UNORM";
        case VK_FORMAT_R8G8B8A8_SNORM                              : return "VK_FORMAT_R8G8B8A8_SNORM";
        case VK_FORMAT_R8G8B8A8_USCALED                            : return "VK_FORMAT_R8G8B8A8_USCALED";
        case VK_FORMAT_R8G8B8A8_SSCALED                            : return "VK_FORMAT_R8G8B8A8_SSCALED";
        case VK_FORMAT_R8G8B8A8_UINT                               : return "VK_FORMAT_R8G8B8A8_UINT";
        case VK_FORMAT_R8G8B8A8_SINT                               : return "VK_FORMAT_R8G8B8A8_SINT";
        case VK_FORMAT_R8G8B8A8_SRGB                               : return "VK_FORMAT_R8G8B8A8_SRGB";
        case VK_FORMAT_B8G8R8A8_UNORM                              : return "VK_FORMAT_B8G8R8A8_UNORM";
        case VK_FORMAT_B8G8R8A8_SNORM                              : return "VK_FORMAT_B8G8R8A8_SNORM";
        case VK_FORMAT_B8G8R8A8_USCALED                            : return "VK_FORMAT_B8G8R8A8_USCALED";
        case VK_FORMAT_B8G8R8A8_SSCALED                            : return "VK_FORMAT_B8G8R8A8_SSCALED";
        case VK_FORMAT_B8G8R8A8_UINT                               : return "VK_FORMAT_B8G8R8A8_UINT";
        case VK_FORMAT_B8G8R8A8_SINT                               : return "VK_FORMAT_B8G8R8A8_SINT";
        case VK_FORMAT_B8G8R8A8_SRGB                               : return "VK_FORMAT_B8G8R8A8_SRGB";
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32                       : return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32                       : return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32                     : return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32                     : return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_UINT_PACK32                        : return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SINT_PACK32                        : return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32                        : return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32                    : return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32                    : return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32                  : return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32                  : return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_UINT_PACK32                     : return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
        case VK_FORMAT_A2R10G10B10_SINT_PACK32                     : return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32                    : return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32                    : return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32                  : return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32                  : return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_UINT_PACK32                     : return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
        case VK_FORMAT_A2B10G10R10_SINT_PACK32                     : return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
        case VK_FORMAT_R16_UNORM                                   : return "VK_FORMAT_R16_UNORM";
        case VK_FORMAT_R16_SNORM                                   : return "VK_FORMAT_R16_SNORM";
        case VK_FORMAT_R16_USCALED                                 : return "VK_FORMAT_R16_USCALED";
        case VK_FORMAT_R16_SSCALED                                 : return "VK_FORMAT_R16_SSCALED";
        case VK_FORMAT_R16_UINT                                    : return "VK_FORMAT_R16_UINT";
        case VK_FORMAT_R16_SINT                                    : return "VK_FORMAT_R16_SINT";
        case VK_FORMAT_R16_SFLOAT                                  : return "VK_FORMAT_R16_SFLOAT";
        case VK_FORMAT_R16G16_UNORM                                : return "VK_FORMAT_R16G16_UNORM";
        case VK_FORMAT_R16G16_SNORM                                : return "VK_FORMAT_R16G16_SNORM";
        case VK_FORMAT_R16G16_USCALED                              : return "VK_FORMAT_R16G16_USCALED";
        case VK_FORMAT_R16G16_SSCALED                              : return "VK_FORMAT_R16G16_SSCALED";
        case VK_FORMAT_R16G16_UINT                                 : return "VK_FORMAT_R16G16_UINT";
        case VK_FORMAT_R16G16_SINT                                 : return "VK_FORMAT_R16G16_SINT";
        case VK_FORMAT_R16G16_SFLOAT                               : return "VK_FORMAT_R16G16_SFLOAT";
        case VK_FORMAT_R16G16B16_UNORM                             : return "VK_FORMAT_R16G16B16_UNORM";
        case VK_FORMAT_R16G16B16_SNORM                             : return "VK_FORMAT_R16G16B16_SNORM";
        case VK_FORMAT_R16G16B16_USCALED                           : return "VK_FORMAT_R16G16B16_USCALED";
        case VK_FORMAT_R16G16B16_SSCALED                           : return "VK_FORMAT_R16G16B16_SSCALED";
        case VK_FORMAT_R16G16B16_UINT                              : return "VK_FORMAT_R16G16B16_UINT";
        case VK_FORMAT_R16G16B16_SINT                              : return "VK_FORMAT_R16G16B16_SINT";
        case VK_FORMAT_R16G16B16_SFLOAT                            : return "VK_FORMAT_R16G16B16_SFLOAT";
        case VK_FORMAT_R16G16B16A16_UNORM                          : return "VK_FORMAT_R16G16B16A16_UNORM";
        case VK_FORMAT_R16G16B16A16_SNORM                          : return "VK_FORMAT_R16G16B16A16_SNORM";
        case VK_FORMAT_R16G16B16A16_USCALED                        : return "VK_FORMAT_R16G16B16A16_USCALED";
        case VK_FORMAT_R16G16B16A16_SSCALED                        : return "VK_FORMAT_R16G16B16A16_SSCALED";
        case VK_FORMAT_R16G16B16A16_UINT                           : return "VK_FORMAT_R16G16B16A16_UINT";
        case VK_FORMAT_R16G16B16A16_SINT                           : return "VK_FORMAT_R16G16B16A16_SINT";
        case VK_FORMAT_R16G16B16A16_SFLOAT                         : return "VK_FORMAT_R16G16B16A16_SFLOAT";
        case VK_FORMAT_R32_UINT                                    : return "VK_FORMAT_R32_UINT";
        case VK_FORMAT_R32_SINT                                    : return "VK_FORMAT_R32_SINT";
        case VK_FORMAT_R32_SFLOAT                                  : return "VK_FORMAT_R32_SFLOAT";
        case VK_FORMAT_R32G32_UINT                                 : return "VK_FORMAT_R32G32_UINT";
        case VK_FORMAT_R32G32_SINT                                 : return "VK_FORMAT_R32G32_SINT";
        case VK_FORMAT_R32G32_SFLOAT                               : return "VK_FORMAT_R32G32_SFLOAT";
        case VK_FORMAT_R32G32B32_UINT                              : return "VK_FORMAT_R32G32B32_UINT";
        case VK_FORMAT_R32G32B32_SINT                              : return "VK_FORMAT_R32G32B32_SINT";
        case VK_FORMAT_R32G32B32_SFLOAT                            : return "VK_FORMAT_R32G32B32_SFLOAT";
        case VK_FORMAT_R32G32B32A32_UINT                           : return "VK_FORMAT_R32G32B32A32_UINT";
        case VK_FORMAT_R32G32B32A32_SINT                           : return "VK_FORMAT_R32G32B32A32_SINT";
        case VK_FORMAT_R32G32B32A32_SFLOAT                         : return "VK_FORMAT_R32G32B32A32_SFLOAT";
        case VK_FORMAT_R64_UINT                                    : return "VK_FORMAT_R64_UINT";
        case VK_FORMAT_R64_SINT                                    : return "VK_FORMAT_R64_SINT";
        case VK_FORMAT_R64_SFLOAT                                  : return "VK_FORMAT_R64_SFLOAT";
        case VK_FORMAT_R64G64_UINT                                 : return "VK_FORMAT_R64G64_UINT";
        case VK_FORMAT_R64G64_SINT                                 : return "VK_FORMAT_R64G64_SINT";
        case VK_FORMAT_R64G64_SFLOAT                               : return "VK_FORMAT_R64G64_SFLOAT";
        case VK_FORMAT_R64G64B64_UINT                              : return "VK_FORMAT_R64G64B64_UINT";
        case VK_FORMAT_R64G64B64_SINT                              : return "VK_FORMAT_R64G64B64_SINT";
        case VK_FORMAT_R64G64B64_SFLOAT                            : return "VK_FORMAT_R64G64B64_SFLOAT";
        case VK_FORMAT_R64G64B64A64_UINT                           : return "VK_FORMAT_R64G64B64A64_UINT";
        case VK_FORMAT_R64G64B64A64_SINT                           : return "VK_FORMAT_R64G64B64A64_SINT";
        case VK_FORMAT_R64G64B64A64_SFLOAT                         : return "VK_FORMAT_R64G64B64A64_SFLOAT";
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32                     : return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32                      : return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
        case VK_FORMAT_D16_UNORM                                   : return "VK_FORMAT_D16_UNORM";
        case VK_FORMAT_X8_D24_UNORM_PACK32                         : return "VK_FORMAT_X8_D24_UNORM_PACK32";
        case VK_FORMAT_D32_SFLOAT                                  : return "VK_FORMAT_D32_SFLOAT";
        case VK_FORMAT_S8_UINT                                     : return "VK_FORMAT_S8_UINT";
        case VK_FORMAT_D16_UNORM_S8_UINT                           : return "VK_FORMAT_D16_UNORM_S8_UINT";
        case VK_FORMAT_D24_UNORM_S8_UINT                           : return "VK_FORMAT_D24_UNORM_S8_UINT";
        case VK_FORMAT_D32_SFLOAT_S8_UINT                          : return "VK_FORMAT_D32_SFLOAT_S8_UINT";
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK                         : return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK                          : return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK                        : return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK                         : return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
        case VK_FORMAT_BC2_UNORM_BLOCK                             : return "VK_FORMAT_BC2_UNORM_BLOCK";
        case VK_FORMAT_BC2_SRGB_BLOCK                              : return "VK_FORMAT_BC2_SRGB_BLOCK";
        case VK_FORMAT_BC3_UNORM_BLOCK                             : return "VK_FORMAT_BC3_UNORM_BLOCK";
        case VK_FORMAT_BC3_SRGB_BLOCK                              : return "VK_FORMAT_BC3_SRGB_BLOCK";
        case VK_FORMAT_BC4_UNORM_BLOCK                             : return "VK_FORMAT_BC4_UNORM_BLOCK";
        case VK_FORMAT_BC4_SNORM_BLOCK                             : return "VK_FORMAT_BC4_SNORM_BLOCK";
        case VK_FORMAT_BC5_UNORM_BLOCK                             : return "VK_FORMAT_BC5_UNORM_BLOCK";
        case VK_FORMAT_BC5_SNORM_BLOCK                             : return "VK_FORMAT_BC5_SNORM_BLOCK";
        case VK_FORMAT_BC6H_UFLOAT_BLOCK                           : return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
        case VK_FORMAT_BC6H_SFLOAT_BLOCK                           : return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
        case VK_FORMAT_BC7_UNORM_BLOCK                             : return "VK_FORMAT_BC7_UNORM_BLOCK";
        case VK_FORMAT_BC7_SRGB_BLOCK                              : return "VK_FORMAT_BC7_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK                     : return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK                      : return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK                   : return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK                    : return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK                   : return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK                    : return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
        case VK_FORMAT_EAC_R11_UNORM_BLOCK                         : return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11_SNORM_BLOCK                         : return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK                      : return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK                      : return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK                        : return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK                         : return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK                       : return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK                        : return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK                       : return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK                        : return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK                       : return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK                        : return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK                      : return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK                       : return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK                      : return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK                       : return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK                      : return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK                       : return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
        case VK_FORMAT_G8B8G8R8_422_UNORM                          : return "VK_FORMAT_G8B8G8R8_422_UNORM";
        case VK_FORMAT_B8G8R8G8_422_UNORM                          : return "VK_FORMAT_B8G8R8G8_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM                   : return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM                    : return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM                   : return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM                    : return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM                   : return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
        case VK_FORMAT_R10X6_UNORM_PACK16                          : return "VK_FORMAT_R10X6_UNORM_PACK16";
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16                    : return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16          : return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16      : return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16      : return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16  : return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16   : return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 ";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16  : return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16   : return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 ";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16  : return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_R12X4_UNORM_PACK16                          : return "VK_FORMAT_R12X4_UNORM_PACK16";
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16                    : return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16          : return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16      : return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16      : return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16  : return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16   : return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 ";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16  : return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16   : return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 ";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16  : return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_G16B16G16R16_422_UNORM                      : return "VK_FORMAT_G16B16G16R16_422_UNORM";
        case VK_FORMAT_B16G16R16G16_422_UNORM                      : return "VK_FORMAT_B16G16R16G16_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM                : return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM                 : return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM                : return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM                 : return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM                : return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG                 : return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG                 : return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG                 : return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG                 : return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG                  : return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG                  : return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG                  : return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG                  : return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
        //case VK_FORMAT_G8B8G8R8_422_UNORM_KHR = VK_FORMAT_G8B8G8R8_422_UNORM,
        //case VK_FORMAT_B8G8R8G8_422_UNORM_KHR = VK_FORMAT_B8G8R8G8_422_UNORM,
        //case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
        //case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
        //case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
        //case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
        //case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
        //case VK_FORMAT_R10X6_UNORM_PACK16_KHR = VK_FORMAT_R10X6_UNORM_PACK16,
        //case VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR = VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
        //case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        //case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        //case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        //case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        //case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        //case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        //case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        //case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        //case VK_FORMAT_R12X4_UNORM_PACK16_KHR = VK_FORMAT_R12X4_UNORM_PACK16,
        //case VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR = VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
        //case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        //case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        //case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        //case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        //case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        //case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        //case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        //case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        //case VK_FORMAT_G16B16G16R16_422_UNORM_KHR = VK_FORMAT_G16B16G16R16_422_UNORM,
        //case VK_FORMAT_B16G16R16G16_422_UNORM_KHR = VK_FORMAT_B16G16R16G16_422_UNORM,
        //case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
        //case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
        //case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
        //case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
        //case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
        //case VK_FORMAT_BEGIN_RANGE = VK_FORMAT_UNDEFINED,
        //case VK_FORMAT_END_RANGE = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
        //case VK_FORMAT_RANGE_SIZE = (VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1),
        case VK_FORMAT_MAX_ENUM : return "VK_FORMAT_MAX_ENUM";
        default: break;
    }
    return "Unknown";
}


#define VERIFY_VK_RESULT(result, whatIfWrong) { if (result != vk::Result::eSuccess) { whatIfWrong; } }

