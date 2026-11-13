// core/PerformanceProfiler.hpp
// Performance Profiling Tool for NLM
// Provides detailed performance analysis and profiling capabilities

#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace profiling {

// Simple profiler for measuring code execution time
class Profiler {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::string name;
    double duration;
    
public:
    Profiler(const std::string& name) : name(name), duration(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Profiler() {
        auto endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(endTime - startTime).count();
        std::cout << "[PROFILER] " << name << ": " << std::fixed << std::setprecision(6) 
                  << duration << "s" << std::endl;
    }
    
    double getDuration() const { return duration; }
    const std::string& getName() const { return name; }
};

// Performance metrics collector
struct PerformanceMetrics {
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double avgSpikeFrequency;
    size_t peakMemoryUsage;
    size_t totalMemoryAllocated;
    std::vector<double> neuronActivityOverTime;
    std::vector<double> firingRateOverTime;
    
    PerformanceMetrics() 
        : totalSteps(0), totalTime(0.0), stepsPerSecond(0.0), 
          avgSpikeFrequency(0.0), peakMemoryUsage(0), 
          totalMemoryAllocated(0) {}
    
    void reset() {
        totalSteps = 0;
        totalTime = 0.0;
        stepsPerSecond = 0.0;
        avgSpikeFrequency = 0.0;
        peakMemoryUsage = 0;
        totalMemoryAllocated = 0;
        neuronActivityOverTime.clear();
        firingRateOverTime.clear();
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "=== Performance Metrics ===\n";
        ss << "Total Steps: " << totalSteps << "\n";
        ss << "Total Time: " << std::fixed << std::setprecision(3) << totalTime << "s\n";
        ss << "Steps/Second: " << std::fixed << std::setprecision(2) << stepsPerSecond << "\n";
        ss << "Avg Spike Frequency: " << std::fixed << std::setprecision(2) << avgSpikeFrequency << " Hz\n";
        ss << "Peak Memory: " << peakMemoryUsage << " bytes\n";
        ss << "Total Memory Allocated: " << totalMemoryAllocated << " bytes\n";
        return ss.str();
    }
};

// Performance analyzer for complex metrics
class PerformanceAnalyzer {
public:
    static double calculateEfficiency(const PerformanceMetrics& metrics) {
        if (metrics.totalSteps == 0) return 0.0;
        return metrics.stepsPerSecond / (1.0 + metrics.peakMemoryUsage / 1024.0);  // Normalize by memory
    }
    
    static double calculateStability(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        
        double mean = 0.0;
        for (double val : data) mean += val;
        mean /= data.size();
        
        double variance = 0.0;
        for (double val : data) {
            double diff = val - mean;
            variance += diff * diff;
        }
        variance /= data.size();
        
        return 1.0 / (1.0 + std::sqrt(variance));  // Lower variance = higher stability
    }
    
    static std::string generatePerformanceReport(const PerformanceMetrics& metrics) {
        std::stringstream ss;
        ss << "=== Performance Analysis ===\n";
        ss << "Efficiency Score: " << std::fixed << std::setprecision(4) 
           << calculateEfficiency(metrics) << "\n";
        ss << "Stability Score: " << std::fixed << std::setprecision(4) 
           << calculateStability(metrics.firingRateOverTime) << "\n";
        ss << "Performance Profile: ";
        
        if (metrics.stepsPerSecond > 1000.0) ss << "High Performance\n";
        else if (metrics.stepsPerSecond > 500.0) ss << "Good Performance\n";
        else if (metrics.stepsPerSecond > 100.0) ss << "Moderate Performance\n";
        else ss << "Low Performance\n";
        
        return ss.str();
    }
};

// Main performance profiling function
inline PerformanceMetrics profilePerformance(
    size_t neuronCount,
    size_t steps,
    float connectionProb = 0.1f,
    bool enableProfiling = true) {
    
    PerformanceMetrics metrics;
    
    if (!enableProfiling) {
        std::cout << "Performance profiling disabled" << std::endl;
        return metrics;
    }
    
    std::cout << "=== Performance Profiling Started ===" << std::endl;
    std::cout << "Configuration: " << neuronCount << " neurons, " << steps << " steps" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", connectionProb, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Simulate some workload
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect metrics
    metrics.totalSteps = steps;
    metrics.totalTime = totalTime;
    metrics.stepsPerSecond = steps / totalTime;
    metrics.avgSpikeFrequency = brain->getAverageFiringRate();
    
    std::cout << "=== Performance Profiling Complete ===" << std::endl;
    std::cout << metrics.toString() << std::endl;
    std::cout << PerformanceAnalyzer::generatePerformanceReport(metrics) << std::endl;
    
    return metrics;
}

} // namespace profiling
} // namespace nlm

// Convenience macro for profiling
#define PROFILE(name) nlm::profiling::Profiler profiler(name)

// Example usage:
// int main() {
//     PROFILE("Main Simulation");
//     
//     // Run simulation code here
//     auto metrics = nlm::profiling::profilePerformance(500, 1000);
//     
//     // Analyze results
//     std::cout << "Performance efficiency: " << nlm::profiling::PerformanceAnalyzer::calculateEfficiency(metrics) << std::endl;
//     return 0;
// }