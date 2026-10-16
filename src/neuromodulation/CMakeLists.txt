cmake_minimum_required(VERSION 3.16)

project(nlm_neuromodulation VERSION 1.0.0 LANGUAGES CXX)

# Neuromodulation library sources

set(NEUROMODULATION_SOURCES
    Reward.cpp
    PredictionError.cpp
    Novelty.cpp
    Neuromodulator.cpp
    Curiosity.cpp
)

# Add neuromodulation library
add_library(nlm_neuromodulation STATIC ${NEUROMODULATION_SOURCES})

target_include_directories(nlm_neuromodulation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../neuromodulation
)

target_link_libraries(nlm_neuromodulation PUBLIC nlm_core nlm_brain)

# Install the neuromodulation library
install(TARGETS nlm_neuromodulation
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../neuromodulation/ DESTINATION include/nlm/neuromodulation
    FILES_MATCHING PATTERN "*.hpp"
)
