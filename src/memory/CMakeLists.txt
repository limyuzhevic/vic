cmake_minimum_required(VERSION 3.16)

project(nlm_memory VERSION 1.0.0 LANGUAGES CXX)

# Memory library sources

set(MEMORY_SOURCES
    Memory.cpp
    NeuralWorkingMemory.cpp
    NeuralEpisodicMemory.cpp
)

# Add memory library
add_library(nlm_memory STATIC ${MEMORY_SOURCES})

target_include_directories(nlm_memory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../memory
)

target_link_libraries(nlm_memory PUBLIC nlm_core nlm_brain)

# Install the memory library
install(TARGETS nlm_memory
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../memory/ DESTINATION include/nlm/memory
    FILES_MATCHING PATTERN "*.hpp"
)
