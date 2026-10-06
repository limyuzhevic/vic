cmake_minimum_required(VERSION 3.16)

# Test configuration - CONSISTENT with root and python CMakeLists.txt
# Use the NLM_SOURCE_DIR from parent or define it relative to this file
if(DEFINED NLM_SOURCE_DIR)
    # NLM_SOURCE_DIR is already defined from parent CMakeLists.txt
    message(STATUS "Using NLM_SOURCE_DIR from parent: ${NLM_SOURCE_DIR}")
else()
    # Define relative to tests directory
    set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" CACHE STRING "Path to NLM source directory")
    set_property(GLOBAL PROPERTY NLM_SOURCE_DIR "${NLM_SOURCE_DIR}")
endif()

# Helper function to add an NLM library (consistent with root and python)
function(add_nlm_library target_name target_sources)
    add_library(${target_name} STATIC ${target_sources})
    
    # Common include directories
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    
    # Common compile options
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    
    # Set C++ standard properties
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endfunction()

function(link_nlm_target target_name)
    foreach(lib ${ARGN})
        target_link_libraries(${target_name} PUBLIC ${lib})
    endforeach()
endfunction()

# Include directories
include_directories(${NLM_SOURCE_DIR})

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

message(STATUS "Test suite configuration complete")