cmake_minimum_required(VERSION 3.16)

project(nlm_phase VERSION 1.0.0 LANGUAGES CXX)

# Phase/experiment library sources

set(PHASE_SOURCES
    Experiment.cpp
    ExperimentRunner.cpp
    Metrics.cpp
    Phase3Demo.cpp
    Phase3Experiment.cpp
    Phase4Demo.cpp
    Phase4Experiment.cpp
    Phase5Experiment.cpp
    Phase6Demo.cpp
    Phase6IntegratedExperiment.cpp
)

# Add phase library
add_library(nlm_phase STATIC ${PHASE_SOURCES})

target_include_directories(nlm_phase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../experiments
    ${CMAKE_CURRENT_SOURCE_DIR}/../agent
    ${CMAKE_CURRENT_SOURCE_DIR}/../world
    ${CMAKE_CURRENT_SOURCE_DIR}/../brain
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

target_link_libraries(nlm_phase PUBLIC nlm_core nlm_brain nlm_memory nlm_neuromodulation nlm_cognition)

# Install the phase library
install(TARGETS nlm_phase
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../experiments/ DESTINATION include/nlm/experiments
    FILES_MATCHING PATTERN "*.hpp"
)
