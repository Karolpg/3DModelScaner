cmake_minimum_required(VERSION 3.5.1)

#
# GLOBAL COMPILER FLAGS
#
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra")
    #set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    #set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11 -fno-rtti")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11")
    
    #if (UNIX)
    #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    #endif()

    message("--------------------------------")
    message("CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
    message("CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
    message("--------------------------------")
endif()

include_directories("Include")

#
# EXTERNALS
# - glslang validator
# - vulkan path
#  
if ("$ENV{VK_PATH}" EQUAL "" OR NOT IS_DIRECTORY "$ENV{VK_PATH}")
    message(FATAL_ERROR "Vulkan path is not valid, VK_PATH = $ENV{VK_PATH}")
endif()

message("VK_PATH = $ENV{VK_PATH}")

get_filename_component(VK_INCLUDE_DIRS "$ENV{VK_PATH}/include" ABSOLUTE)
get_filename_component(GLM_INCLUDE_DIRS "Submodule/glm" ABSOLUTE)
get_filename_component(SPIRV_CROSS_PATH "Submodule/SPIRV-Cross" ABSOLUTE)

message("VK_INCLUDE_DIRS  = ${VK_INCLUDE_DIRS}")
message("GLM_INCLUDE_DIRS = ${GLM_INCLUDE_DIRS}")
message("SPIRV_CROSS_PATH = ${SPIRV_CROSS_PATH}")

find_program(GLSLANG_VALIDATOR glslangValidator PATHS $ENV{VK_PATH}/bin)
find_library(VULKAN_LIB vulkan $ENV{VK_PATH}/lib)
find_library(SHADERC_LIB shaderc_combined $ENV{VK_PATH}/lib/libshaderc)
find_library(SPIRV_CROSS_LIB spirv-cross ${SPIRV_CROSS_PATH})

# Qt configuration to work with Vulkan
message("#################################################################################")
message("#- By default Qt is not configured to work with Vulkan. In this case please invoke:")
message("#-    configure -I $ENV{VK_PATH}/include -L $ENV{VK_PATH}/lib -v -feature-vulkan -opensource -confirm-license")
message("#-    make")
message("#-    sudo make install")
message("#- in your QT Src directory.")
message("#- QT will be installed in default path: /usr/local/Qt-5.xx.yy")
message("#- If you have troubles with pcre2 just invoke make another time. Look https://bugreports.qt.io/browse/QTBUG-71420 .")
message("#- Do not try to invoke make with -jN parameters.")
message("#################################################################################")

#
# SET OUTPUT PATHS
#
set(MAIN_PROJECT_DIR ${PROJECT_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_PROJECT_DIR}/bin/${CMAKE_BUILD_TYPE})
message("MAIN_PROJECT_DIR = ${MAIN_PROJECT_DIR}")
message("CMAKE_ARCHIVE_OUTPUT_DIRECTORY = ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#
# SOURCE
#
#include_directories("Include")
add_subdirectory(MainWindow)

