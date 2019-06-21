cmake_minimum_required(VERSION 3.5.1)

message("Start cmake Graphic dir...")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# QT config variables
include(${PROJECT_SOURCE_DIR}/CMakeShare/Qt_CMakeLists.txt)

include_directories(${VK_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${SPIRV_CROSS_PATH})


#
# SOURCE
#
add_library(graphic  STATIC  BufferDescr.cpp
                             DrawManager.cpp
                             GraphicObject.cpp
                             ImageDescr.cpp
                             ImageViewDescr.cpp
                             PipelineManager.cpp
                             ResourceManager.cpp
                             SamplerDescr.cpp)

target_link_libraries(graphic ${QT_LIBS} ${SPIRV_CROSS_LIB})

message("End cmake Graphic dir...")

