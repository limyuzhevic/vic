cmake_minimum_required(VERSION 3.16)

# Dynamics module CMakeLists.txt
# Contains: NeuralDynamics, SpikeSystem

add_library(nlm_dynamics STATIC
    dynamics/NeuralDynamics.cpp
    dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_dynamics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_dynamics PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_dynamics PUBLIC nlm_core)

install(TARGETS nlm_dynamics
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY dynamics/
    DESTINATION include/dynamics
    FILES_MATCHING PATTERN "*.hpp"
)
