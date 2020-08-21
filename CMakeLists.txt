cmake_minimum_required(VERSION 3.17) # Should be 3.18
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES_SHADERS
        src/shaders/basic.frag
        src/shaders/basic.vert
        )

set(SOURCES
        src/frontend/glfw3.cpp
        src/frontend/glfw3.h
        src/frontend/window.cpp
        src/frontend/window.h
        src/main.cpp
        src/vulkan/image.cpp
        src/vulkan/image.h
        src/vulkan/imageview.cpp
        src/vulkan/imageview.h
        src/vulkan/instance.cpp
        src/vulkan/instance.h
        src/vulkan/logicaldevice.cpp
        src/vulkan/logicaldevice.h
        src/vulkan/physicaldevice.cpp
        src/vulkan/physicaldevice.h
        src/vulkan/shadermodule.cpp
        src/vulkan/shadermodule.h
        src/vulkan/surfacekhr.cpp
        src/vulkan/surfacekhr.h
        src/vulkan/swapchainkhr.cpp
        src/vulkan/swapchainkhr.h
        src/vulkan/vkhandle.cpp
        src/vulkan/vkhandle.h
        src/vulkan/vulkan.cpp
        src/vulkan/vulkan.h
        src/vulkan_engine.h
        )

add_executable(vulkan_engine)
target_sources(vulkan_engine PRIVATE ${SOURCES})

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

# GLSL to SPIR-V compiler
find_program(SPIRV_COMPILER glslc DOC "GLSL to SPIR-V compiler" REQUIRED)

foreach(SHADER ${SOURCES_SHADERS})
    set(SHADER ${PROJECT_SOURCE_DIR}/${SHADER})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(OUTPUT "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
            COMMAND ${SPIRV_COMPILER} ${SHADER} -o ${OUTPUT}
            DEPENDS ${SHADER}
    )
    list(APPEND SPIRV_BINARY_FILES ${OUTPUT})
endforeach(SHADER)

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(vulkan_engine shaders)