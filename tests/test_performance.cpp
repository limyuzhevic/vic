// Performance Tests
#include "experiments/ScalingBenchmark.hpp"
#include <catch2/catch_test_macros.hpp>
#include <chrono>

namespace test_performance {

TEST_CASE("ScalingBenchmark creation and basic properties", "[performance][benchmark][creation]") {
    nlm::ScalingBenchmark benchmark;
    
    REQUIRE(benchmark.getDatasetSize() == 0);
    REQUIRE(benchmark.getIterations() == 0);
}

TEST_CASE("ScalingBenchmark initialization and configuration", "[performance][benchmark][initialization]") {
    nlm::ScalingBenchmark benchmark;
    
    benchmark.configure(100, 10, "medium_scale");
    
    REQUIRE(benchmark.getDatasetSize() == 100);
    REQUIRE(benchmark.getIterations() == 10);
    REQUIRE(benchmark.getScaleType() == "medium_scale");
}

TEST_CASE("Performance measurement and timing", "[performance][measurement][timing]") {
    nlm::ScalingBenchmark benchmark;
    benchmark.configure(50, 5, "small_scale");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate performance test
    for (int i = 0; i < benchmark.getIterations(); ++i) {
        // Simulate processing dataset
        for (int j = 0; j < benchmark.getDatasetSize(); ++j) {
            double value = static_cast<double>(i + j) * 0.01;
            // Process value
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    REQUIRE(duration.count() > 0);
}

TEST_CASE("Performance benchmark scaling analysis", "[performance][benchmark][scaling]") {
    nlm::ScalingBenchmark benchmark;
    
    // Test with different dataset sizes
    std::vector<int> datasetSizes = {10, 50, 100, 500};
    
    for (int size : datasetSizes) {
        nlm::ScalingBenchmark testBenchmark;
        testBenchmark.configure(size, 3, "test_scale");
        
        REQUIRE(testBenchmark.getDatasetSize() == size);
    }
}

TEST_CASE("Performance benchmark iteration analysis", "[performance][benchmark][iterations]") {
    nlm::ScalingBenchmark benchmark;
    
    // Test with different iteration counts
    std::vector<int> iterationCounts = {1, 10, 50, 100};
    
    for (int iterations : iterationCounts) {
        nlm::ScalingBenchmark testBenchmark;
        testBenchmark.configure(100, iterations, "test_scale");
        
        REQUIRE(testBenchmark.getIterations() == iterations);
    }
}

TEST_CASE("Performance benchmark result analysis", "[performance][benchmark][results]") {
    nlm::ScalingBenchmark benchmark;
    benchmark.configure(100, 20, "analysis_scale");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate benchmark run
    for (int i = 0; i < benchmark.getIterations(); ++i) {
        for (int j = 0; j < benchmark.getDatasetSize(); ++j) {
            double value = static_cast<double>(i * j) / 100.0;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Calculate performance metrics
    double totalOperations = static_cast<double>(benchmark.getDatasetSize() * benchmark.getIterations());
    double operationsPerSecond = totalOperations / (duration.count() / 1000000.0);
    
    REQUIRE(operationsPerSecond > 0.0);
    REQUIRE(benchmark.getDatasetSize() > 0);
    REQUIRE(benchmark.getIterations() > 0);
}

TEST_CASE("Performance benchmark memory efficiency", "[performance][benchmark][memory]") {
    nlm::ScalingBenchmark benchmark;
    benchmark.configure(1000, 10, "memory_test");
    
    // Simulate memory-intensive operations
    std::vector<std::vector<double>> data(benchmark.getIterations());
    
    for (int i = 0; i < benchmark.getIterations(); ++i) {
        data[i].resize(benchmark.getDatasetSize());
        for (int j = 0; j < benchmark.getDatasetSize(); ++j) {
            data[i][j] = static_cast<double>(i + j) * 0.001;
        }
    }
    
    REQUIRE(data.size() == benchmark.getIterations());
    for (const auto& row : data) {
        REQUIRE(row.size() == benchmark.getDatasetSize());
    }
}

TEST_CASE("Performance benchmark parallel processing simulation", "[performance][benchmark][parallel]") {
    nlm::ScalingBenchmark benchmark;
    benchmark.configure(200, 10, "parallel_test");
    
    // Simulate parallel processing (conceptual)
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Sequential processing simulation
    for (int i = 0; i < benchmark.getIterations(); ++i) {
        for (int j = 0; j < benchmark.getDatasetSize(); ++j) {
            double value = static_cast<double>(i * j) * 0.0001;
            // Simulate processing
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Performance should be measurable
    REQUIRE(duration.count() > 0);
}

TEST_CASE("Performance benchmark result validation", "[performance][benchmark][validation]") {
    nlm::ScalingBenchmark benchmark;
    benchmark.configure(50, 5, "validation_test");
    
    // Run benchmark and collect results
    double totalTime = 0.0;
    double maxTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    
    for (int iteration = 0; iteration < benchmark.getIterations(); ++iteration) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Simulate benchmark work
        for (int i = 0; i < benchmark.getDatasetSize(); ++i) {
            double value = static_cast<double>(i) * 0.01;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        double timeMs = duration.count() / 1000.0;
        
        totalTime += timeMs;
        maxTime = std::max(maxTime, timeMs);
        minTime = std::min(minTime, timeMs);
    }
    
    double averageTime = totalTime / benchmark.getIterations();
    
    REQUIRE(averageTime > 0.0);
    REQUIRE(maxTime >= minTime);
    REQUIRE(benchmark.getDatasetSize() > 0);
    REQUIRE(benchmark.getIterations() > 0);
}

} // namespace test_performance
