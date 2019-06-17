cmake_minimum_required(VERSION 3.5.1)

message("Start cmake MainWindow dir...")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# QT config variables
include(${PROJECT_SOURCE_DIR}/CMakeShare/Qt_CMakeLists.txt)

# Shaders build
include(${PROJECT_SOURCE_DIR}/CMakeShare/CompileShader_CMakeLists.txt)

include_directories(${VK_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${SPIRV_CROSS_PATH})

set(commandOutList)
compileShaders(${PROJECT_SOURCE_DIR}/resources/shaders  ${PROJECT_SOURCE_DIR}/bin/shaders commandOutList)

#
# SOURCE
#
add_executable(3DModelScaner    main.cpp 
                                Cube.cpp
                                MainWindow.cpp 
                                VulkanWindow.cpp 
                                VulkanRenderer.cpp
                                ${commandOutList})
target_link_libraries(3DModelScaner ${QT_LIBS} ${VULKAN_LIB} pthread graphic)

message("End cmake MainWindow dir...")

