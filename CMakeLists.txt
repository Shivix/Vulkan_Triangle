cmake_minimum_required(VERSION 3.17)
project(vulkanTriangle)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE
        include/vulkanTriangle.hpp
        include/graphicsPipeline.hpp
        include/vulkanInstance.hpp
        include/swapChain.hpp)

set(SRC
        src/main.cpp
        src/vulkanTriangle.cpp
        src/graphicsPipeline.cpp
        src/vulkanInstance.cpp
        src/swapChain.cpp)

find_package(Boost REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})

add_executable(vulkanTriangle ${SRC} ${INCLUDE})

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

target_link_libraries(vulkanTriangle "C:/MinGW/lib/libglfw3.a")
target_link_libraries(vulkanTriangle ${Vulkan_LIBRARY})