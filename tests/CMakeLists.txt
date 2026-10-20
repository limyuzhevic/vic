# This CMakeLists.txt is deprecated. Tests are now built as part of the main CMake configuration.
# The main CMakeLists.txt creates the nlm_test target when NLM_ENABLE_TESTS is ON.

message(FATAL_ERROR "This CMakeLists.txt is deprecated. Use the main CMakeLists.txt in the project root.")

# For backward compatibility:
# The main CMakeLists.txt now builds the nlm_test target using modern testing infrastructure
# with support for:
# 1. Basic unit tests
# 2. Code coverage (when NLM_ENABLE_COVERAGE=ON)
# 3. Integration with CTest
# 4. Better test organization

# To build tests:
# 1. Configure with: cmake -DNLM_ENABLE_TESTS=ON ..
# 2. Build with: cmake --build . --target nlm_test
# 3. Run tests with: ctest

# The modern testing framework includes:
# - Enhanced test infrastructure
# - Performance benchmarks
# - Code coverage reporting
# - Better error reporting
# - Integration with CI/CD pipelines
