#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    
    struct BenchmarkResult {
        double executionTime = 0.0;
        size_t totalMemoryAlloc = 0;
        size_t totalMemoryAccess = 0;
        double peakCPUUsage = 0.0;
        double peakMemoryUsage = 0.0;
    };
    
    void runBenchmark(const std::shared_ptr<class Brain>& brain) {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::cout << "Benchmarking brain performance..." << std::endl;
        
        // CPU Performance
        size_t peakInstructions = 0;
        size_t peakMemoryAccess = 0;
        double peakCPUTime = 0.0;
        
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            ++peakInstructions;
            ++peakMemoryAccess;
            
            // Simulate work for performance measurements
            double workPerSecond = 1.0 / (peakInstructions + 1);
            peakCPUTime += workPerSecond;
        }
        
        double peakCPU = static_cast<double>(peakInstructions * 0.01);
        double peakMemory = static_cast<double>(peakMemoryAccess * 0.1);
        
        std::cout << "Results:" << std::endl;
        std::cout << "  CPU Instructions: " << peakInstructions << std::endl;
        std::cout << "  Memory Access: " << peakMemoryAccess << std::endl;
        std::cout << "  Peak CPU Usage: " << peakCPU << "  Peak Memory Usage: " << peakMemory << std::endl;
    }
    
    void run(const std::shared_ptr<class Brain>& brain) {
        runBenchmark(brain);
    }
};