cmake_minimum_required(VERSION 3.16)
project(pynlm_viz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Include main NLM core sources
add_library(nlm_core_viz STATIC
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
    ${NLM_SOURCE_DIR}/visualization/SpikeTrainVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/MembranePotentialVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/SynapticConnectionVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/RegionActivityVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/DevelopmentVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/PlasticityVisualizer.cpp
    ${NLM_SOURCE_DIR}/visualization/MemorySystemVisualizer.cpp
)

target_include_directories(nlm_core_viz PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core_viz PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Visualization module
pybind11_add_module(pynlm_viz bindings_viz.cpp)
target_link_libraries(pynlm_viz PRIVATE nlm_core_viz)
target_include_directories(pynlm_viz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm_viz PRIVATE /permissive-)
endif()

install(TARGETS pynlm_viz
    LIBRARY DESTINATION pynlm_viz
    ARCHIVE DESTINATION pynlm_viz
    RUNTIME DESTINATION pynlm_viz
)
