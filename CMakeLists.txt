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
        src/misc/stb_image_impl.cpp
        src/misc/tinyobjloader_impl.cpp src/vulkan/buffer.cpp
        src/vulkan/buffer.h
        src/vulkan/commandbuffer.cpp
        src/vulkan/commandbuffer.h
        src/vulkan/commandpool.cpp
        src/vulkan/commandpool.h
        src/vulkan/depthstencilimage.cpp
        src/vulkan/depthstencilimage.h
        src/vulkan/descriptorpool.cpp
        src/vulkan/descriptorpool.h
        src/vulkan/descriptorset.cpp
        src/vulkan/descriptorset.h
        src/vulkan/deviceallocator.cpp
        src/vulkan/deviceallocator.h
        src/vulkan/framebuffer.cpp
        src/vulkan/framebuffer.h
        src/vulkan/image.cpp
        src/vulkan/image.h
        src/vulkan/imageview.cpp
        src/vulkan/imageview.h
        src/vulkan/instance.cpp
        src/vulkan/instance.h
        src/vulkan/logicaldevice.cpp
        src/vulkan/logicaldevice.h
        src/vulkan/model.cpp
        src/vulkan/model.h
        src/vulkan/physicaldevice.cpp
        src/vulkan/physicaldevice.h
        src/vulkan/pipeline.cpp
        src/vulkan/pipeline.h
        src/vulkan/pipelinebuilder.cpp
        src/vulkan/pipelinebuilder.h
        src/vulkan/renderpass.cpp
        src/vulkan/renderpass.h
        src/vulkan/sampler.cpp
        src/vulkan/sampler.h
        src/vulkan/shadermodule.cpp
        src/vulkan/shadermodule.h
        src/vulkan/surfacekhr.cpp
        src/vulkan/surfacekhr.h
        src/vulkan/swapchainkhr.cpp
        src/vulkan/swapchainkhr.h
        src/vulkan/uniformbuffer.cpp
        src/vulkan/uniformbuffer.h
        src/vulkan/vkhandle.h
        src/vulkan/vulkan.cpp
        src/vulkan/vulkan.h
        src/vulkan_engine.h
        )

add_executable(vulkan_engine)
target_sources(vulkan_engine PUBLIC ${SOURCES})

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_engine PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(vulkan_engine PUBLIC ${Vulkan_INCLUDE_DIRS})

# GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(vulkan_engine PUBLIC glfw)
target_include_directories(vulkan_engine PUBLIC glfw)

# GLM
find_package(glm REQUIRED)
target_include_directories(vulkan_engine PUBLIC glm)

# Microsoft GSL
find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(vulkan_engine PUBLIC Microsoft.GSL::GSL)

# spdlog
add_subdirectory(deps/spdlog)
target_link_libraries(vulkan_engine PUBLIC spdlog)

# tinyobjloader
target_include_directories(vulkan_engine PUBLIC deps/tinyobjloader)

# stb single-files librairies
target_include_directories(vulkan_engine PUBLIC deps/stb)

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

# Copy resources
add_custom_command(TARGET vulkan_engine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:vulkan_engine>/resources
        )