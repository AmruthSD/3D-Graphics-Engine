cmake_minimum_required(VERSION 3.16)
project(3d_graphics_engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_SOURCE_DIR}/src/*.c"
     "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
)


add_executable(3d_graphics_engine
    ${SRC_FILES}
)

target_include_directories(3d_graphics_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_definitions(3d_graphics_engine PRIVATE
    GLFW_INCLUDE_NONE
)

target_link_libraries(3d_graphics_engine PRIVATE
    ${GLFW_LIBRARIES}
    Vulkan::Vulkan
)
