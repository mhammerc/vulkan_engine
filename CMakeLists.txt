cmake_minimum_required(VERSION 3.17)
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(vulkan_engine src/main.cpp src/vulkan_engine.h src/vulkan/instance.cpp src/vulkan/instance.h src/vulkan/vulkan.h src/vulkan/vulkan.cpp)

find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_engine PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(vulkan_engine PUBLIC ${Vulkan_INCLUDE_DIRS})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(vulkan_engine PUBLIC glfw)
target_include_directories(vulkan_engine PUBLIC glfw)

find_package(glm REQUIRED)
target_include_directories(vulkan_engine PUBLIC glm)

add_subdirectory(deps/spdlog)
target_link_libraries(vulkan_engine PUBLIC spdlog)
