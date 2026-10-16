cmake_minimum_required(VERSION 3.16)

project(nlm_cognition VERSION 1.0.0 LANGUAGES CXX)

# Cognition library sources

set(COGNITION_SOURCES
    NeuralPlanner.cpp
    ConceptFormation.cpp
)

# Add cognition library
add_library(nlm_cognition STATIC ${COGNITION_SOURCES})

target_include_directories(nlm_cognition PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../cognition
)

target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_brain nlm_memory)

# Install the cognition library
install(TARGETS nlm_cognition
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../cognition/ DESTINATION include/nlm/cognition
    FILES_MATCHING PATTERN "*.hpp"
)
