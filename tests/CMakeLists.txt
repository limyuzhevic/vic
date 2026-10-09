# Integration Tests CMake Configuration
# 
# This CMakeLists.txt builds the integration test executable (nlm_integration_test)
# which includes comprehensive system-wide integration tests for the NLM neural network simulation framework.

# Integration Test Configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create integration test executable
add_executable(nlm_integration_test
    IntegrationTestMain.cpp
    IntegrationTests.cpp
    IntegrationTestRunner.cpp
    
    # Integration test framework
    IntegrationTestFramework.hpp
    EndToEndTests.hpp
    CrossComponentTests.hpp
    PerformanceTests.hpp
    StressTests.hpp
)

target_link_libraries(nlm_integration_test PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)