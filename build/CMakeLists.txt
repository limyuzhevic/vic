cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Simplified core library with only the necessary files
add_library(nlm_core STATIC
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
)

target_include_directories(nlm_core PUBLIC ${NLM_SOURCE_DIR})

# Agent components
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World components
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core)

pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
