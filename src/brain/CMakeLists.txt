cmake_minimum_required(VERSION 3.16)

# Brain module CMakeLists.txt
# Contains: Neuron, Synapse, NeuralPopulation, NeuralRegion, Brain

# Brain library
add_library(nlm_brain STATIC
    brain/Neuron.cpp
    brain/Synapse.cpp
    brain/NeuralPopulation.cpp
    brain/NeuralRegion.cpp
    brain/Brain.cpp
)

target_include_directories(nlm_brain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_brain PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Add dependencies
find_package(nlm_core REQUIRED)
target_link_libraries(nlm_brain PUBLIC nlm_core)

# Install targets
install(TARGETS nlm_brain
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY brain/
    DESTINATION include/brain
    FILES_MATCHING PATTERN "*.hpp"
)

# Configuration options
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
