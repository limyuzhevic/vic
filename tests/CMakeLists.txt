# CMakeLists.txt for tests directory
# Defines all test targets for NLM Phase 6 comprehensive integration testing

# Test directories
add_subdirectory(framework)

# Unit test executables

# Core system tests
add_executable(test_config
    test_config.cpp
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_config nlm_core)

add_executable(test_config_enhanced
    test_config_enhanced.cpp
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_config_enhanced nlm_core)

# Brain and integration tests
add_executable(test_brain
    test_brain.cpp
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_brain nlm_core)

add_executable(test_clock
    test_clock.cpp
)

target_link_libraries(test_clock nlm_core)

# Memory system tests (will be implemented)
add_executable(test_memory
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_memory nlm_core)

# Neuromodulation system tests (will be implemented)
add_executable(test_neuromodulation
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_neuromodulation nlm_core)

# Prediction system tests (will be implemented)
add_executable(test_prediction
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_prediction nlm_core)

# Cognition system tests (will be implemented)
add_executable(test_cognition
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_cognition nlm_core)

# Development system tests (will be implemented)
add_executable(test_development
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_development nlm_core)

# Checkpoint system tests (will be implemented)
add_executable(test_checkpoint
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_checkpoint nlm_core)

# Sleep/rest cycle tests (will be implemented)
add_executable(test_sleep
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_sleep nlm_core)

# Replay system tests (will be implemented)
add_executable(test_replay
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(test_replay nlm_core)

# Comprehensive integration test runner
add_executable(nlm_test_runner
    test_runner.cpp
)

target_link_libraries(nlm_test_runner nlm_core)

# Performance benchmark executable
add_executable(nlm_benchmark
    ${CMAKE_SOURCE_DIR}/tests/framework/TestFramework.cpp
)

target_link_libraries(nlm_benchmark nlm_core)

# Installation targets for tests
install(TARGETS test_config test_config_enhanced test_brain test_clock
    test_memory test_neuromodulation test_prediction test_cognition
    test_development test_checkpoint test_sleep test_replay
    nlm_test_runner nlm_benchmark
    RUNTIME DESTINATION bin
)

# Test discovery
enable_test()

# Add tests to CTest (simplified approach)
add_test(NAME test_config
    COMMAND test_config
)

add_test(NAME test_config_enhanced
    COMMAND test_config_enhanced
)

add_test(NAME test_brain
    COMMAND test_brain
)

add_test(NAME test_clock
    COMMAND test_clock
)

add_test(NAME nlm_test_runner
    COMMAND nlm_test_runner
)

# Add integration tests (conditional)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable Phase 6 integration tests" ON)

if(NLM_ENABLE_INTEGRATION_TESTS)
    # Integration tests use the main experiment runner
    add_test(NAME nlm_phase6_demo
        COMMAND nlm_phase6_demo
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# Performance benchmarks (optional)
option(NLM_ENABLE_BENCHMARKS "Enable performance benchmarks" ON)

if(NLM_ENABLE_BENCHMARKS)
    add_test(NAME nlm_benchmark
        COMMAND nlm_benchmark
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()