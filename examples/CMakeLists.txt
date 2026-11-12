# Examples CMakeLists.txt
# Configuration for example applications and utilities

# Create examples executable for testing refactored components
add_executable(test_examples
    src/examples/MainRunner.cpp
    src/examples/ExperimentRunner.cpp
    src/examples/ResultReporter.cpp
    src/examples/Benchmarker.cpp
)

target_include_directories(test_examples PRIVATE src)
target_link_libraries(test_examples PUBLIC nlm_core nlm_agent nlm_world)