cmake_minimum_required(VERSION 3.5.1)

message("Start cmake MainWindow dir...")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# QT config variables
include(${PROJECT_SOURCE_DIR}/CMakeShare/Qt_CMakeLists.txt)

include_directories(${VK_INCLUDE_DIRS})

#
# SOURCE
#
add_executable(3DModelScaner main.cpp MainWindow.cpp VulkanWindow.cpp VulkanRenderer.cpp)
target_link_libraries(3DModelScaner ${QT_LIBS} ${VULKAN_LIB})

message("End cmake MainWindow dir...")

