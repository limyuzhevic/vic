# benchmarks/CMakeLists.txt

# Create benchmark helper library
add_library(nlm_benchmark_helper STATIC
    benchmark_helper.cpp
)

target_include_directories(nlm_benchmark_helper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nlm_benchmark_helper PUBLIC nlm_core)

# Create benchmark executable
add_executable(nlm_benchmarks
    benchmarks.cpp
)

target_link_libraries(nlm_benchmarks PUBLIC nlm_core nlm_benchmark_helper)

# Set target properties
set_target_properties(nlm_benchmarks PROPERTIES
    OUTPUT_NAME "nlm_benchmarks"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add benchmark target to install
install(TARGETS nlm_benchmarks
    RUNTIME DESTINATION bin
)

# Add benchmark tests
enable_testing()
add_test(NAME nlm_benchmarks COMMAND nlm_benchmarks)

# Create custom benchmarks target for easier execution
add_custom_target(benchmarks
    COMMAND nlm_benchmarks
    DEPENDS nlm_benchmarks
    COMMENT "Running NLM performance benchmarks"
    VERBATIM
)