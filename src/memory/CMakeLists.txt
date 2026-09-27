# NLM Memory Layer - Memory systems

# Working memory (from memory directory)
add_library(nlm_working_memory STATIC
    src/memory/Memory.cpp
)

target_include_directories(nlm_working_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/memory>
    $<INSTALL_INTERFACE:include/memory>
)

# Episodic memory
add_library(nlm_episodic_memory STATIC
    src/memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_episodic_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/memory>
    $<INSTALL_INTERFACE:include/memory>
)

# Semantic memory
add_library(nlm_semantic_memory STATIC
    src/memory/NeuralWorkingMemory.cpp
)

target_include_directories(nlm_semantic_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/memory>
    $<INSTALL_INTERFACE:include/memory>
)

# Memory layer interface library
add_library(nlm_memory INTERFACE)
target_link_libraries(nlm_memory INTERFACE
    nlm_working_memory
    nlm_episodic_memory
    nlm_semantic_memory
)

# Memory layer installation
install(TARGETS nlm_working_memory nlm_episodic_memory nlm_semantic_memory
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory
    DESTINATION include/memory
    FILES_MATCHING PATTERN "*.hpp"
)

# Memory interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/include/nlm_memory.hpp
    DESTINATION include/memory
)