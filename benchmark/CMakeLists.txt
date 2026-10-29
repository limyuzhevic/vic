# Benchmark directory
add_library(nlm_benchmark STATIC
    src/benchmark/BenchmarkSystem.cpp
    src/benchmark/ExampleBenchmark.cpp
)
target_include_directories(nlm_benchmark PUBLIC src)
target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_agent nlm_world nlm_visualization nlm_advanced)

# Add benchmark executable
add_executable(nlm_benchmark_example
    src/benchmark/ExampleBenchmark.cpp
)
target_link_libraries(nlm_benchmark_example PUBLIC nlm_benchmark)

# Add benchmark shell script to installation
install(FILES benchmark_example.sh
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# Install benchmark documentation
install(FILES BENCHMARK.md
    DESTINATION share/doc/nlm
)
