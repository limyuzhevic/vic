cmake_minimum_required(VERSION 3.16)

# Visualization module CMakeLists.txt
# Contains: VisualizationInterface

add_library(nlm_visualization STATIC
    visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_visualization PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_visualization PUBLIC nlm_core)

install(TARGETS nlm_visualization
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY visualization/
    DESTINATION include/visualization
    FILES_MATCHING PATTERN "*.hpp"
)
