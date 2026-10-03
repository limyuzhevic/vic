# Test Framework CMake Configuration

# Create test framework library
add_library(nlm_test_framework STATIC
    src/test/TestFramework.cpp
    src/test/TestSuite.cpp
    src/test/TestRunner.cpp
    src/test/TestConfiguration.cpp
    src/test/TestFactory.cpp
    src/test/TestMemory.cpp
    src/test/TestPlasticity.cpp
    src/test/TestNeuromodulation.cpp
    src/test/TestDevelopment.cpp
    src/test/TestIntegration.cpp
)

target_include_directories(nlm_test_framework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/test>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_test_framework PRIVATE cxx_std_20)

# Create test runner executable
add_executable(nlm_test_runner
    tools/nlm_test_runner.py
)

# Create test analyzer executable
add_executable(nlm_test_analyzer
    tools/test_analyzer.py
)

# Create test coverage executable
add_executable(nlm_test_coverage
    tools/test_coverage.py
)

# Make Python scripts executable
set_target_properties(nlm_test_runner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add test dependencies
add_dependencies(nlm_test_runner nlm_test_framework)
add_dependencies(nlm_test_analyzer nlm_test_framework)
add_dependencies(nlm_test_coverage nlm_test_framework)

# Install test framework
install(TARGETS nlm_test_framework
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install test tools
install(PROGRAMS tools/nlm_test_runner.py
    DESTINATION bin
    RUNTIME DESTINATION bin
)

install(PROGRAMS tools/test_analyzer.py
    DESTINATION bin
    RUNTIME DESTINATION bin
)

install(PROGRAMS tools/test_coverage.py
    DESTINATION bin
    RUNTIME DESTINATION bin
)