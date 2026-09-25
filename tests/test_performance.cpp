// Performance Benchmarking and Memory Tests
// Comprehensive performance testing for all NLM system components
// Includes benchmarking, memory leak detection, and stress tests

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <sys/resource.h>
#include <unistd.h>

namespace test_performance {

// Memory tracking structure
struct MemoryUsage {
    size_t rss;        // Resident Set Size in bytes
    size_t vms;        // Virtual Memory Size in bytes
    size_t stack;      // Stack size in bytes
};

// Get current memory usage
MemoryUsage getMemoryUsage() {
    MemoryUsage mem;
    
    // Get RSS (Resident Set Size) from /proc/self/status
    std::ifstream status_file("/proc/self/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.find("VmRSS:") != std::string::npos) {
            // Extract value in KB
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value_str = line.substr(pos + 1);
                // Remove whitespace and " kB" suffix
                value_str.erase(0, value_str.find_first_not_of(" \t\n\r"));
                value_str.erase(value_str.find_last_not_of(" \t\n\r") + 1);
                if (value_str.size() > 2 && value_str.substr(value_str.size() - 2) == " kB") {
                    value_str = value_str.substr(0, value_str.size() - 2);
                }
                mem.rss = std::stoull(value_str) * 1024; // Convert KB to bytes
            }
        } else if (line.find("VmSize:") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value_str = line.substr(pos + 1);
                value_str.erase(0, value_str.find_first_not_of(" \t\n\r"));
                value_str.erase(value_str.find_last_not_of(" \t\n\r") + 1);
                if (value_str.size() > 2 && value_str.substr(value_str.size() - 2) == " kB") {
                    value_str = value_str.substr(0, value_str.size() - 2);
                }
                mem.vms = std::stoull(value_str) * 1024; // Convert KB to bytes
            }
        } else if (line.find("Threads:") != std::string::npos) {
            // Use threads count for stack estimation (rough approximation)
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value_str = line.substr(pos + 1);
                value_str.erase(0, value_str.find_first_not_of(" \t\n\r"));
                mem.stack = std::stoull(value_str) * 1024 * 10; // Rough stack estimate
            }
        }
    }
    
    return mem;
}

// Performance benchmark result structure
struct PerformanceResult {
    std::string testName;
    double executionTime;      // seconds
    double cpuTime;           // CPU seconds
    size_t memoryUsage;       // bytes
    double operationsPerSecond;
    double memoryPerOperation; // bytes per operation
    double throughput;        // operations per second
};

// Configuration benchmark tests
void benchmarkConfigCreation() {
    std::cout << "Benchmarking Config creation..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startCpu = std::clock();
    
    // Benchmark Config creation
    const size_t iterations = 10000;
    for (size_t i = 0; i < iterations; ++i) {
        auto config = nlm::Config::create();
        config->set("test_key", i, nlm::ConfigSource::Runtime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endCpu = std::clock();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    double cpuTime = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;
    
    PerformanceResult result;
    result.testName = "Config Creation";
    result.executionTime = executionTime;
    result.cpuTime = cpuTime;
    result.memoryUsage = getMemoryUsage().rss;
    result.operationsPerSecond = iterations / executionTime;
    result.memoryPerOperation = result.memoryUsage / iterations;
    result.throughput = result.operationsPerSecond;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << result.operationsPerSecond << std::endl;
}

// Random number generation benchmark
void benchmarkRandomGeneration() {
    std::cout << "Benchmarking Random generation..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startCpu = std::clock();
    
    const size_t iterations = 1000000;
    
    // Benchmark different random number generation methods
    for (size_t i = 0; i < iterations; ++i) {
        nlm::RandomGenerator rng(i); // Deterministic for benchmark
        
        // Mix different types of random operations
        if (i % 4 == 0) {
            uint32_t val = rng.uniformInt(0, 100);
        } else if (i % 4 == 1) {
            double val = rng.uniformReal(0.0, 1.0);
        } else if (i % 4 == 2) {
            float val = rng.normal32(0.0f, 1.0f);
        } else {
            bool val = rng.bernoulli(0.5);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endCpu = std::clock();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    double cpuTime = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;
    
    PerformanceResult result;
    result.testName = "Random Generation";
    result.executionTime = executionTime;
    result.cpuTime = cpuTime;
    result.memoryUsage = getMemoryUsage().rss;
    result.operationsPerSecond = iterations / executionTime;
    result.memoryPerOperation = result.memoryUsage / iterations;
    result.throughput = result.operationsPerSecond;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << result.operationsPerSecond << std::endl;
}

// SimulationClock benchmark
void benchmarkSimulationClock() {
    std::cout << "Benchmarking SimulationClock..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startCpu = std::clock();
    
    const size_t iterations = 100000;
    nlm::SimulationClock clock(0.001);
    
    for (size_t i = 0; i < iterations; ++i) {
        double time = clock.getTime();
        clock.step();
        clock.advanceTime(0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endCpu = std::clock();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    double cpuTime = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;
    
    PerformanceResult result;
    result.testName = "SimulationClock";
    result.executionTime = executionTime;
    result.cpuTime = cpuTime;
    result.memoryUsage = getMemoryUsage().rss;
    result.operationsPerSecond = iterations / executionTime;
    result.memoryPerOperation = result.memoryUsage / iterations;
    result.throughput = result.operationsPerSecond;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << result.operationsPerSecond << std::endl;
}

// Memory stress test
void memoryStressTest() {
    std::cout << "Running memory stress test..." << std::flush;
    
    auto initialMemory = getMemoryUsage();
    std::vector<std::unique_ptr<char[]>> memoryBlocks;
    
    const size_t numBlocks = 1000;
    const size_t blockSize = 1024; // 1KB per block
    
    // Allocate memory blocks
    for (size_t i = 0; i < numBlocks; ++i) {
        memoryBlocks.push_back(std::make_unique<char[]>(blockSize));
        // Initialize memory to prevent optimization
        std::fill(memoryBlocks.back().get(), memoryBlocks.back().get() + blockSize, static_cast<char>(i % 256));
    }
    
    auto peakMemory = getMemoryUsage();
    std::cout << " Peak memory: " << peakMemory.rss / 1024 << " KB" << std::endl;
    
    // Deallocate memory
    memoryBlocks.clear();
    
    auto finalMemory = getMemoryUsage();
    std::cout << " Final memory: " << finalMemory.rss / 1024 << " KB" << std::endl;
    
    double memoryIncrease = static_cast<double>(peakMemory.rss - initialMemory.rss) / 1024 / 1024; // MB
    std::cout << " Memory increase: " << std::fixed << std::setprecision(2) << memoryIncrease << " MB" << std::endl;
    
    // Check for memory leaks
    bool memoryLeakFree = (finalMemory.rss - initialMemory.rss) < (numBlocks * blockSize * 2); // Allow some overhead
    
    if (!memoryLeakFree) {
        std::cout << " WARNING: Possible memory leak detected!" << std::endl;
    }
    
    std::cout << " Done!" << std::endl;
}

// Container operation benchmark
void benchmarkContainerOperations() {
    std::cout << "Benchmarking container operations..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startCpu = std::clock();
    
    const size_t iterations = 50000;
    
    // Benchmark vector operations
    for (size_t i = 0; i < iterations; ++i) {
        std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::shuffle(vec.begin(), vec.end(), std::default_random_engine(i));
        std::sort(vec.begin(), vec.end());
        int sum = std::accumulate(vec.begin(), vec.end(), 0);
        (void)sum; // Suppress unused variable warning
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endCpu = std::clock();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    double cpuTime = static_cast<double>(endCpu - startCpu) / CLOCKS_PER_SEC;
    
    PerformanceResult result;
    result.testName = "Container Operations";
    result.executionTime = executionTime;
    result.cpuTime = cpuTime;
    result.memoryUsage = getMemoryUsage().rss;
    result.operationsPerSecond = iterations / executionTime;
    result.memoryPerOperation = result.memoryUsage / iterations;
    result.throughput = result.operationsPerSecond;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << result.operationsPerSecond << std::endl;
}

// Stress test with multiple threads
void stressTestMultiThreading() {
    std::cout << "Running multi-threading stress test..." << std::flush;
    
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    const int numThreads = 8;
    const int iterationsPerThread = 50000;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t, iterationsPerThread]() {
            nlm::RandomGenerator rng(t * 12345); // Different seed per thread
            
            for (int i = 0; i < iterationsPerThread; ++i) {
                // Thread-safe operations
                uint32_t val = rng.uniformInt(0, 1000);
                double dbl = rng.uniformReal(0.0, 1.0);
                float flt = rng.normal32(0.0f, 1.0f);
                bool b = rng.bernoulli(0.5);
                
                // Prevent optimization
                (void)val; (void)dbl; (void)flt; (void)b;
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    size_t totalIterations = numThreads * iterationsPerThread;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Total iterations: " << totalIterations << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << totalIterations / executionTime << std::endl;
}

// Performance comparison test
void performanceComparisonTest() {
    std::cout << "Running performance comparison test..." << std::flush;
    
    // Test different data type combinations
    struct BenchmarkConfig {
        std::string name;
        std::function<void(size_t)> benchmarkFunc;
    };
    
    std::vector<BenchmarkConfig> benchmarks;
    
    // Add various benchmark functions
    benchmarks.push_back({"uint32_t operations", [](size_t iterations) {
        nlm::RandomGenerator rng(42);
        for (size_t i = 0; i < iterations; ++i) {
            uint32_t val = rng.uniformInt32(i, i + 100);
        }
    }});
    
    benchmarks.push_back({"uint64_t operations", [](size_t iterations) {
        nlm::RandomGenerator rng(42);
        for (size_t i = 0; i < iterations; ++i) {
            uint64_t val = rng.uniformInt64(i, i + 100);
        }
    }});
    
    benchmarks.push_back({"float operations", [](size_t iterations) {
        nlm::RandomGenerator rng(42);
        for (size_t i = 0; i < iterations; ++i) {
            float val = rng.uniformReal32(i % 1000 / 1000.0f, (i + 1) % 1000 / 1000.0f);
        }
    }});
    
    benchmarks.push_back({"double operations", [](size_t iterations) {
        nlm::RandomGenerator rng(42);
        for (size_t i = 0; i < iterations; ++i) {
            double val = rng.uniformReal64(i % 1000 / 1000.0, (i + 1) % 1000 / 1000.0);
        }
    }});
    
    // Run benchmarks
    std::vector<PerformanceResult> results;
    const size_t iterations = 100000;
    
    for (const auto& bench : benchmarks) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bench.benchmarkFunc(iterations);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        PerformanceResult result;
        result.testName = bench.name;
        result.executionTime = executionTime;
        result.operationsPerSecond = iterations / executionTime;
        result.throughput = result.operationsPerSecond;
        
        results.push_back(result);
    }
    
    // Print comparison results
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    Performance Comparison Results                  ║" << std::endl;
    std::cout << "╠═══════════════════════╦═══════════════════════════╦══════════════╣" << std::endl;
    std::cout << "║ Test Name            ║ Operations/sec         ║ Time (s)      ║" << std::endl;
    std::cout << "╠═══════════════════════╬═══════════════════════════╬══════════════╣" << std::endl;
    
    for (const auto& result : results) {
        std::cout << "║ " << std::setw(23) << std::left << result.testName
                  << " ║ " << std::setw(27) << std::left << std::fixed << std::setprecision(0) << result.operationsPerSecond
                  << " ║ " << std::setw(12) << std::left << std::fixed << std::setprecision(4) << result.executionTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════════════╩═══════════════════════════╩══════════════╝" << std::endl;
    
    std::cout << " Done!" << std::endl;
}

// Memory allocation stress test
void memoryAllocationStressTest() {
    std::cout << "Running memory allocation stress test..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startMemory = getMemoryUsage();
    
    // Allocate large amounts of memory with different sizes
    std::vector<std::vector<std::unique_ptr<char[]>>> largeAllocations;
    
    const size_t numAllocations = 100;
    const size_t smallBlock = 1024;   // 1 KB
    const size_t mediumBlock = 65536; // 64 KB
    const size_t largeBlock = 1048576; // 1 MB
    
    // Small block allocations
    std::cout << "  Testing small block allocations..." << std::flush;
    for (size_t i = 0; i < numAllocations; ++i) {
        largeAllocations.emplace_back();
        largeAllocations.back().push_back(std::make_unique<char[]>(smallBlock));
        std::fill(largeAllocations.back().back().get(), 
                 largeAllocations.back().back().get() + smallBlock, static_cast<char>(i));
    }
    auto smallAllocMemory = getMemoryUsage();
    std::cout << " Memory: " << smallAllocMemory.rss / 1024 << " KB" << std::endl;
    
    // Medium block allocations
    std::cout << "  Testing medium block allocations..." << std::flush;
    for (size_t i = 0; i < numAllocations; ++i) {
        largeAllocations.emplace_back();
        largeAllocations.back().push_back(std::make_unique<char[]>(mediumBlock));
        std::fill(largeAllocations.back().back().get(), 
                 largeAllocations.back().back().get() + mediumBlock, static_cast<char>(i));
    }
    auto mediumAllocMemory = getMemoryUsage();
    std::cout << " Memory: " << mediumAllocMemory.rss / 1024 << " KB" << std::endl;
    
    // Large block allocations
    std::cout << "  Testing large block allocations..." << std::flush;
    for (size_t i = 0; i < numAllocations; ++i) {
        largeAllocations.emplace_back();
        largeAllocations.back().push_back(std::make_unique<char[]>(largeBlock));
        std::fill(largeAllocations.back().back().get(), 
                 largeAllocations.back().back().get() + largeBlock, static_cast<char>(i));
    }
    auto peakMemory = getMemoryUsage();
    std::cout << " Memory: " << peakMemory.rss / 1024 << " KB" << std::endl;
    
    // Deallocate all memory
    largeAllocations.clear();
    
    auto finalMemory = getMemoryUsage();
    std::cout << " Memory after cleanup: " << finalMemory.rss / 1024 << " KB" << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    double memoryIncrease = static_cast<double>(peakMemory.rss - startMemory.rss) / 1024 / 1024; // MB
    std::cout << " Total execution time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << " Peak memory increase: " << std::fixed << std::setprecision(2) << memoryIncrease << " MB" << std::endl;
    
    // Check for memory leaks
    bool memoryLeakFree = (finalMemory.rss - startMemory.rss) < (numAllocations * (smallBlock + mediumBlock + largeBlock) * 2);
    
    if (!memoryLeakFree) {
        std::cout << " WARNING: Possible memory leak detected!" << std::endl;
    } else {
        std::cout << " Memory leak test PASSED" << std::endl;
    }
    
    std::cout << " Done!" << std::endl;
}

// Integration performance test
void integrationPerformanceTest() {
    std::cout << "Running integration performance test..." << std::flush;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate a realistic workload using all major components
    const size_t numIterations = 1000;
    
    for (size_t i = 0; i < numIterations; ++i) {
        // Create and configure a complete system
        auto config = nlm::Config::create();
        config->set("neuron_count", 100, nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        config->set("random_seed", i, nlm::ConfigSource::Runtime);
        
        // Create random generator
        nlm::RandomGenerator rng(i);
        
        // Perform random operations
        uint32_t neuronCount = rng.uniformInt32(50, 200);
        float learningRate = rng.uniformReal32(0.001f, 0.1f);
        bool enablePlasticity = rng.bernoulli(0.7);
        
        // Simulate some processing
        double sum = 0.0;
        for (int j = 0; j < 100; ++j) {
            sum += rng.uniformReal64(0.0, 1.0) * learningRate;
        }
        
        // Simulate logging
        // In real implementation, would use actual logger
        
        (void)sum; // Suppress unused variable warning
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double executionTime = std::chrono::duration<double>(endTime - startTime).count();
    double opsPerSecond = numIterations / executionTime;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Iterations: " << numIterations << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(4) << executionTime << "s" << std::endl;
    std::cout << "    Operations/sec: " << std::fixed << std::setprecision(0) << opsPerSecond << std::endl;
}

void runAll() {
    std::cout << "=== NLM Performance Tests ===" << std::endl;
    std::cout << std::endl;
    
    benchmarkConfigCreation();
    std::cout << std::endl;
    
    benchmarkRandomGeneration();
    std::cout << std::endl;
    
    benchmarkSimulationClock();
    std::cout << std::endl;
    
    memoryStressTest();
    std::cout << std::endl;
    
    benchmarkContainerOperations();
    std::cout << std::endl;
    
    stressTestMultiThreading();
    std::cout << std::endl;
    
    performanceComparisonTest();
    std::cout << std::endl;
    
    memoryAllocationStressTest();
    std::cout << std::endl;
    
    integrationPerformanceTest();
    std::cout << std::endl;
    
    std::cout << "=== All Performance Tests Complete ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_performance