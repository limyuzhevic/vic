# Test Framework
add_library(nlm_test_framework
    TestFramework.cpp
)

target_include_directories(nlm_test_framework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_test_framework PUBLIC nlm_core)

# Add examples to tests
add_executable(nlm_examples
    ../test_examples/TestExamples.cpp
)

target_link_libraries(nlm_examples PRIVATE nlm_test_framework)
target_compile_features(nlm_examples PRIVATE cxx_std_20)