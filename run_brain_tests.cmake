// run_brain_tests.cmake
// This script runs the brain tests to verify our changes

# Find the test executable
test_path("../tests/test_brain")

if(test_found)
    message(STATUS "Running brain tests...")
    run_tests()
else()
    message(WARNING "Brain test executable not found")
endif()