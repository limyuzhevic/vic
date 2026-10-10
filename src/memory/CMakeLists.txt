cmake_minimum_required(VERSION 3.16)

# Memory system module (nlm_memory)
get_filename_component(NLM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(NLM_ROOT_DIR ${NLM_SRC_DIR} DIRECTORY)

# Create memory system libraries
add_library(nlm_memory
    memory/Memory.cpp
    memory/NeuralWorkingMemory.cpp
    memory/NeuralEpisodicMemory.cpp
    memory/SemanticMemory.cpp
    memory/ProceduralMemory.cpp
    memory/AssociativeMemory.cpp
)

target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Memory-specific test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_memory_test
        memory/tests/test_memory.cpp
        memory/tests/test_working_memory.cpp
        memory/tests/test_episodic_memory.cpp
    )
    
    target_link_libraries(nlm_memory_test PRIVATE nlm_memory)
    target_compile_features(nlm_memory_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_memory_test COMMAND nlm_memory_test)
endif()