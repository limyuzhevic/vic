# ==============================================================================
# Visualization Build System
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(NLM_Visualization LANGUAGES CXX)

# Find required packages
find_package(Threads REQUIRED)
find_package(GRAPHVIZ REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Visualization source files
set(VISUALIZATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetworkVisualizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpikeTrainAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlasticityVisualizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryVisualizer.cpp
)

# Visualization header files
set(VISUALIZATION_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetworkVisualizer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SpikeTrainAnalyzer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PlasticityVisualizer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryVisualizer.hpp
)

# Create visualization library
add_library(nlm_visualization ${VISUALIZATION_SOURCES} ${VISUALIZATION_HEADERS})

# Target properties
target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_visualization PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(nlm_visualization PUBLIC
    Threads::Threads
    GRAPHVIZ::graphviz
)

# Install target
install(TARGETS nlm_visualization
    EXPORT NLM_Visualization_Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${VISUALIZATION_HEADERS}
    DESTINATION include/nlm/debug
)

# Export targets
install(EXPORT NLM_Visualization_Targets
    FILE NLM_Visualization_Targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm_visualization
)

# Create neural network visualizer target
add_executable(neural_network_visualizer ${CMAKE_CURRENT_SOURCE_DIR}/NeuralNetworkVisualizerCLI.cpp)
target_link_libraries(neural_network_visualizer PRIVATE nlm_visualization nlm_debug_cli)
install(TARGETS neural_network_visualizer DESTINATION bin)

# Create spike train analyzer target
add_executable(spike_train_analyzer ${CMAKE_CURRENT_SOURCE_DIR}/SpikeTrainAnalyzerCLI.cpp)
target_link_libraries(spike_train_analyzer PRIVATE nlm_visualization nlm_debug_cli)
install(TARGETS spike_train_analyzer DESTINATION bin)

# Create plasticity visualizer target
add_executable(plasticity_visualizer ${CMAKE_CURRENT_SOURCE_DIR}/PlasticityVisualizerCLI.cpp)
target_link_libraries(plasticity_visualizer PRIVATE nlm_visualization nlm_debug_cli)
install(TARGETS plasticity_visualizer DESTINATION bin)

# Create memory visualizer target
add_executable(memory_visualizer ${CMAKE_CURRENT_SOURCE_DIR}/MemoryVisualizerCLI.cpp)
target_link_libraries(memory_visualizer PRIVATE nlm_visualization nlm_debug_cli)
install(TARGETS memory_visualizer DESTINATION bin)

# Package generation
include(GNUInstallDirs)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_VisualizationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_VisualizationConfig.cmake
    @ONLY
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_VisualizationPkgConfig.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_VisualizationPkgConfig.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_VisualizationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_VisualizationPkgConfig.pc
    DESTINATION lib/cmake/nlm_visualization
)

# Add dependencies
add_dependencies(nlm_visualization nlm_brain nlm_memory nlm_plasticity)

# Build options
option(BUILD_VISUALIZATION "Build NLM Visualization" ON)
option(INSTALL_VISUALIZATION "Install NLM Visualization" ON)
option(INSTALL_VISUALIZATION_COMPONENTS "Install Visualization components" ON)

if(NOT BUILD_VISUALIZATION)
    add_custom_target(visualization_off)
    add_dependencies(nlm_visualization_off visualization_off)
endif()
