cmake_minimum_required(VERSION 3.16)

# Experiments module CMakeLists.txt
# Contains: Experiment, ExperimentRunner, Metrics

add_library(nlm_experiments STATIC
    experiments/Experiment.cpp
    experiments/ExperimentRunner.cpp
    experiments/Metrics.cpp
)

target_include_directories(nlm_experiments PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_experiments PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_experiments PUBLIC nlm_core)

install(TARGETS nlm_experiments
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY experiments/
    DESTINATION include/experiments
    FILES_MATCHING PATTERN "*.hpp"
)
