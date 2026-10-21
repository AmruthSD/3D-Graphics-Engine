cmake_minimum_required(VERSION 3.16)
project(3d_graphics_engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if no build type specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

# Packages
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
endif()

find_package(glm QUIET)

# Source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Shader compilation
find_program(GLSLC_EXECUTABLE glslc HINTS /usr/bin /usr/local/bin ${Vulkan_GLSLC_EXECUTABLE})

if(GLSLC_EXECUTABLE)
    set(SHADER_OUTPUTS
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytrace.comp.spv"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv"
        COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert" -o "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.spv"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert"
        COMMENT "Compiling shaders/shader.vert -> vert.spv"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv"
        COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag" -o "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.spv"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag"
        COMMENT "Compiling shaders/shader.frag -> frag.spv"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytrace.comp.spv"
        COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytracer.comp" -o "${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytrace.comp.spv"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/raytracer.comp"
        COMMENT "Compiling shaders/raytracer.comp -> raytrace.comp.spv"
    )

    add_custom_target(compile_shaders DEPENDS ${SHADER_OUTPUTS})
else()
    message(WARNING "glslc not found. Precompiled SPIR-V shaders will be used if present.")
endif()

add_executable(3d_graphics_engine
    ${SRC_FILES}
)

if(TARGET compile_shaders)
    add_dependencies(3d_graphics_engine compile_shaders)
endif()

target_include_directories(3d_graphics_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Suppress deprecated enum warnings from third-party stb headers in C++20
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(3d_graphics_engine PRIVATE -Wno-deprecated-enum-enum-conversion)
endif()

if(DEFINED GLFW_INCLUDE_DIRS)
    target_include_directories(3d_graphics_engine PRIVATE ${GLFW_INCLUDE_DIRS})
endif()

if(DEFINED Vulkan_INCLUDE_DIRS)
    target_include_directories(3d_graphics_engine PRIVATE ${Vulkan_INCLUDE_DIRS})
endif()

if(DEFINED GLM_INCLUDE_DIRS)
    target_include_directories(3d_graphics_engine PRIVATE ${GLM_INCLUDE_DIRS})
endif()

target_compile_definitions(3d_graphics_engine PRIVATE
    GLFW_INCLUDE_NONE
)

target_link_libraries(3d_graphics_engine PRIVATE
    Vulkan::Vulkan
    Threads::Threads
)

if(TARGET glfw)
    target_link_libraries(3d_graphics_engine PRIVATE glfw)
elseif(TARGET GLFW3::GLFW3)
    target_link_libraries(3d_graphics_engine PRIVATE GLFW3::GLFW3)
elseif(DEFINED GLFW_LIBRARIES)
    target_link_libraries(3d_graphics_engine PRIVATE ${GLFW_LIBRARIES})
endif()

if(TARGET glm::glm)
    target_link_libraries(3d_graphics_engine PRIVATE glm::glm)
elseif(DEFINED GLM_LIBRARIES)
    target_link_libraries(3d_graphics_engine PRIVATE ${GLM_LIBRARIES})
endif()
