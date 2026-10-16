cmake_minimum_required(VERSION 3.16)

project(nlm_development VERSION 1.0.0 LANGUAGES CXX)

# Development library sources

set(DEVELOPMENT_SOURCES
    Synaptogenesis.cpp
    Pruning.cpp
    Maturation.cpp
    DevelopmentSystem.cpp
)

# Add development library
add_library(nlm_development STATIC ${DEVELOPMENT_SOURCES})

target_include_directories(nlm_development PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../development
)

target_link_libraries(nlm_development PUBLIC nlm_core nlm_brain nlm_neuromodulation)

# Install the development library
install(TARGETS nlm_development
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../development/ DESTINATION include/nlm/development
    FILES_MATCHING PATTERN "*.hpp"
)
