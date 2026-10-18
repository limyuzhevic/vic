cmake_minimum_required(VERSION 3.16)

# Test framework - now inline in header
target_link_libraries(nlm_tests PUBLIC nlm_core)

# Test executable
add_executable(run_tests
    main_test.cpp
)

target_link_libraries(run_tests PUBLIC nlm_core nlm_tests)

# Add test targets
enable_testing()
add_test(NAME NLM_Tests COMMAND run_tests)

# Installation for tests (optional)
install(TARGETS nlm_tests run_tests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)