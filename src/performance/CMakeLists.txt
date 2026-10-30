cmake_minimum_required(VERSION 3.16)

# Performance module CMakeLists.txt
# Contains: CheckpointSystem, PerformanceMonitor, MemoryPool

add_library(nlm_performance STATIC
    performance/CheckpointSystem.cpp
    performance/PerformanceMonitor.cpp
    performance/MemoryPool.cpp
)

target_include_directories(nlm_performance PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_performance PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_performance PUBLIC nlm_core)

install(TARGETS nlm_performance
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY performance/
    DESTINATION include/performance
    FILES_MATCHING PATTERN "*.hpp"
)
