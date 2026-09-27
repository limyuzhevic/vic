# Phase 6 Integration Test Suite Build Configuration

# This CMake file adds the Phase 6 Integration Test Suite to the build

# Add Phase 6 Integration Test Suite executable
add_executable(nlm_phase6_integration_test
    src/experiments/Phase6IntegrationTestSuite.cpp
)

target_link_libraries(nlm_phase6_integration_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(nlm_phase6_integration_test PRIVATE cxx_std_20)

# Add test for Phase 6 Integration Test Suite
add_test(NAME nlm_phase6_integration_test COMMAND nlm_phase6_integration_test)

# Optional: Add demo executable for Phase 6 Integration Test Suite
add_executable(nlm_phase6_integration_demo
    src/experiments/Phase6IntegrationTestSuite.cpp
)

target_link_libraries(nlm_phase6_integration_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(nlm_phase6_integration_demo PRIVATE cxx_std_20)

# Add custom target for running the integration test suite
add_custom_target(run_phase6_integration_tests
    COMMAND nlm_phase6_integration_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Phase 6 Integration Test Suite"
)

# Add custom target for running the Phase 6 integration demo
add_custom_target(run_phase6_integration_demo
    COMMAND nlm_phase6_integration_demo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Phase 6 Integration Demo"
)