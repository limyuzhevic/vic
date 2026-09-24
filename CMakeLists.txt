# NLM Phase 6 Testing Infrastructure - Simplified CMake Configuration
# Modern GoogleTest-based testing for Phase 6 systems

cmake_minimum_required(VERSION 3.16)
project(NLM_Testing VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find GoogleTest
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Source directories
set(NLM_CORE_SOURCE_DIR "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_25bf7a96-3252-4bf0-bc91-18c524c2a454/src")
set(TESTS_SOURCE_DIR "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_25bf7a96-3252-4bf0-bc91-18c524c2a454/tests")

# Legacy test executable (backward compatibility)
add_executable(nlm_test_legacy
    ${TESTS_SOURCE_DIR}/test_main.cpp
    ${TESTS_SOURCE_DIR}/test_types.cpp
    ${TESTS_SOURCE_DIR}/test_config.cpp
    ${TESTS_SOURCE_DIR}/test_random.cpp
    ${TESTS_SOURCE_DIR}/test_clock.cpp
    ${TESTS_SOURCE_DIR}/test_neuron.cpp
    ${TESTS_SOURCE_DIR}/test_synapse.cpp
    ${TESTS_SOURCE_DIR}/test_stdp.cpp
    ${TESTS_SOURCE_DIR}/test_brain.cpp
)

target_include_directories(nlm_test_legacy PRIVATE
    ${NLM_CORE_SOURCE_DIR}
)
target_link_libraries(nlm_test_legacy PRIVATE ${GTEST_LIBRARIES} Threads::Threads)
target_compile_features(nlm_test_legacy PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_legacy COMMAND nlm_test_legacy)

# Modern Phase 6 testing with GoogleTest
add_executable(nlm_test_phase6
    tests/test_main_phase6.cpp
    tests/test_phase6/TestPhase6Utils.cpp
    tests/test_phase6/Phase6TestFramework.hpp
    tests/test_phase6/TestPhase6Utils.hpp
    tests/test_phase6/TestPhase6Memory.cpp
    tests/test_phase6/TestPhase6Neuromodulation.cpp
    tests/test_phase6/TestPhase6Prediction.cpp
    tests/test_phase6/TestPhase6Cognition.cpp
    tests/test_phase6/TestPhase6Development.cpp
    tests/test_phase6/TestPhase6Checkpointing.cpp
    tests/test_phase6/TestPhase6Replay.cpp
    tests/test_phase6/TestPhase6Performance.cpp
    tests/test_phase6/TestPhase6Integration.cpp
    tests/test_phase6/TestPhase6Demo.cpp
    tests/test_phase6/TestPhase6Documentation.hpp
)

target_include_directories(nlm_test_phase6 PRIVATE
    ${NLM_CORE_SOURCE_DIR}
    ${TESTS_SOURCE_DIR}/test_phase6
)
target_link_libraries(nlm_test_phase6 PRIVATE ${GTEST_LIBRARIES} Threads::Threads)
target_compile_features(nlm_test_phase6 PRIVATE cxx_std_20)

# GoogleTest discovery
include(GoogleTest)
gtest_discover_tests(nlm_test_legacy
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    TEST_PREFIX "legacy_"
    TEST_SUFFIX "_legacy"
)

gtest_discover_tests(nlm_test_phase6
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    TEST_PREFIX "phase6_"
    TEST_SUFFIX "_phase6"
)

# Installation
install(TARGETS nlm_test_legacy nlm_test_phase6
    RUNTIME DESTINATION bin
)
