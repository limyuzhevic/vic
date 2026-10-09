cmake_minimum_required(VERSION 3.20)

# Build tests with modern CMake
enable_testing()

# Use file globbing for test files
file(GLOB TEST_SOURCES
    "tests/test_*.cpp"
)

# Create test executable
add_executable(nlm_test ${TEST_SOURCES})

target_link_libraries(nlm_test nlm_core)

# Test-specific compiler flags
target_compile_features(nlm_test PRIVATE cxx_std_20)

target_compile_options(nlm_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    -O0 -g -DNDEBUG  # Debug builds for tests
)

# Add tests to CTest
add_test(NAME NLMTests
    COMMAND nlm_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Install tests (optional)
install(TARGETS nlm_test
    RUNTIME DESTINATION bin
)
