# Include test framework headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/test)

# Add test executable
add_executable(nlm_test_framework
    src/main.cpp
    src/test/TestFramework.cpp
    src/test/TestRunner.cpp
    src/test/TestImplementations.cpp
)

# Link with NLM library
target_link_libraries(nlm_test_framework nlm)

# Set C++ standard
target_compile_features(nlm_test_framework PRIVATE cxx_std_17)

# Install test framework
install(TARGETS nlm_test_framework
    RUNTIME DESTINATION bin
    OPTIONAL
)
