// Performance Test Runner Header for NLM
// Comprehensive performance testing framework for NLM simulations
// Includes timing, memory profiling, and performance benchmarking

#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace nlm {
    class Config;
    class Brain;
    class NeuronId;
    enum class SimulationStep : uint64_t;
    enum class Timestamp : double;
    struct MembranePotential;
}

namespace test_performance {

// Performance test result structure
struct PerformanceTestResult {
    std::string test_name;
    bool passed;
    std::string error_message;
    
    // Timing metrics
    double avg_step_time_us;
    double steps_per_second;
    uint64_t duration_us;
    
    // Memory metrics
    uint64_t avg_memory_usage;
    uint64_t peak_memory_usage;
    double memory_growth_rate;
    
    // Scale metrics
    double neurons_per_ms;
    
    // Brain statistics
    uint64_t total_neurons;
    uint64_t total_synapses;
    uint64_t active_neurons;
    uint64_t firing_neurons;
    double average_firing_rate;
    uint64_t total_spikes;
    double excitation_inhibition_ratio;
    
    // Test parameters
    int steps;
    int neuron_count;
    float stress_factor;
};

// Performance benchmark report structure
struct PerformanceBenchmarkReport {
    std::string test_name;
    int steps;
    int neuron_count;
    
    std::vector<PerformanceTestResult> timing_tests;
    std::vector<PerformanceTestResult> memory_tests;
    std::vector<PerformanceTestResult> scale_tests;
    std::vector<PerformanceTestResult> stress_tests;
    
    double average_step_time() const;
    uint64_t total_spikes() const;
    uint64_t peak_memory() const;
};

class PerformanceTestRunner {
public:
    static PerformanceTestResult runTimingTest(int steps, int neuronCount);
    static PerformanceTestResult runMemoryTest(int steps, int neuronCount);
    static PerformanceTestResult runScaleTest(int neuronCount);
    static PerformanceTestResult runStressTest(int steps, int neuronCount, float stressFactor);
    static PerformanceBenchmarkReport runBenchmarkSuite(int steps, int neuronCount);
    
    static void generateReport(PerformanceBenchmarkReport& report);
    static void printHeader(const std::string& testName);
    static void printFooter(const std::string& testName, bool passed, const std::string& details);
};

} // namespace test_performance
