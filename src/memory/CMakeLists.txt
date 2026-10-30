cmake_minimum_required(VERSION 3.16)

# Memory module CMakeLists.txt
# Contains: WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory, AssociativeMemory
# Contains: NeuralWorkingMemory, NeuralEpisodicMemory

add_library(nlm_memory STATIC
    memory/WorkingMemory.cpp
    memory/EpisodicMemory.cpp
    memory/SemanticMemory.cpp
    memory/ProceduralMemory.cpp
    memory/AssociativeMemory.cpp
    memory/NeuralWorkingMemory.cpp
    memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_memory PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_memory PUBLIC nlm_core)

install(TARGETS nlm_memory
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY memory/
    DESTINATION include/memory
    FILES_MATCHING PATTERN "*.hpp"
)
