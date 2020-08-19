cmake_minimum_required(VERSION 3.17)
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(vulkan_engine src/main.cpp src/vulkan_engine.h src/vulkan/instance.cpp src/vulkan/instance.h src/vulkan/vulkan.h src/vulkan/vulkan.cpp src/frontend/glfw3.h src/frontend/glfw3.cpp src/frontend/window.cpp src/frontend/window.h src/vulkan/surfacekhr.cpp src/vulkan/surfacekhr.h src/vulkan/physicaldevice.cpp src/vulkan/physicaldevice.h src/vulkan/logicaldevice.cpp src/vulkan/logicaldevice.h src/vulkan/swapchainkhr.cpp src/vulkan/swapchainkhr.h src/vulkan/image.cpp src/vulkan/image.h src/vulkan/imageview.cpp src/vulkan/imageview.h src/vulkan/vkhandle.cpp src/vulkan/vkhandle.h)

find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_engine PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(vulkan_engine PUBLIC ${Vulkan_INCLUDE_DIRS})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(vulkan_engine PUBLIC glfw)
target_include_directories(vulkan_engine PUBLIC glfw)

find_package(glm REQUIRED)
target_include_directories(vulkan_engine PUBLIC glm)

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(vulkan_engine PUBLIC Microsoft.GSL::GSL)

add_subdirectory(deps/spdlog)
target_link_libraries(vulkan_engine PUBLIC spdlog)
