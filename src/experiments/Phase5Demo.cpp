// Phase 5 Demo - Performance Benchmark
//
// This demo runs a simple performance benchmark to verify
// that performance optimizations are working correctly.

#include "experiments/Phase5Experiment.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Performance Demo ===" << std::endl;
    std::cout << "Testing performance optimizations..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create simple scaling benchmark
    ScalingBenchmark benchmark;
    
    // Define scale levels for testing
    std::vector<ScaleLevel> scales;
    scales.push_back(ScaleLevel(1000, "Small"));
    scales.push_back(ScaleLevel(10000, "Medium"));
    scales.push_back(ScaleLevel(50000, "Large"));
    
    // Run scaling experiment
    std::cout << "--- Performance Scaling Test ---" << std::endl;
    ScalingAnalysis result = benchmark.runScalingExperiment(scales, 1000);
    
    std::cout << std::endl;
    std::cout << "=== PERFORMANCE SCALING RESULTS ===" << std::endl;
    std::cout << "Performance scaling curve:" << std::endl;
    for (const auto& pair : result.scaleVsPerformance) {
        std::cout << "  " << pair.first << " neurons: " << pair.second << " spikes/sec" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== PERFORMANCE ANALYSIS ===" << std::endl;
    std::cout << "Neural efficiency: " << result.neuralEfficiency << std::endl;
    std::cout << "Parallel efficiency: " << result.parallelEfficiency << std::endl;
    std::cout << "Memory efficiency: " << result.memoryEfficiency << std::endl;
    
    // Check if scaling shows expected behavior
    bool scalingWorks = true;
    if (scales.size() >= 2) {
        // Check if performance increases with scale (but not linearly)
        for (size_t i = 1; i < result.scaleVsPerformance.size(); ++i) {
            float prevEfficiency = result.scaleVsPerformance[i-1].second / scales[i-1].neuronCount;
            float currEfficiency = result.scaleVsPerformance[i].second / scales[i].neuronCount;
            if (currEfficiency < prevEfficiency * 0.1f) {  // Efficiency shouldn't drop by more than 10x
                scalingWorks = false;
                break;
            }
        }
    }
    
    std::cout << "=== PERFORMANCE STATUS ===" << std::endl;
    std::cout << "Neural scaling: " << (scalingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Parallel processing: " << (result.parallelEfficiency > 0.5f ? "EFFECTIVE" : "INEFFICIENT") << std::endl;
    std::cout << "Memory management: " << (result.memoryEfficiency > 0.5f ? "EFFICIENT" : "INEFFICIENT") << std::endl;
    
    return 0;
}
