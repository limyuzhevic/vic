// Performance Test Runner for NLM
// Comprehensive performance testing framework for NLM simulations
// Includes timing, memory profiling, and performance benchmarking

#include "tests/PerformanceTestRunner.h"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <cmath>
#include <iomanip>

namespace test_performance {

PerformanceTestResult PerformanceTestRunner::runTimingTest(int steps, int neuronCount) {
    std::cout << "    Running timing test with " << steps << " steps, " << neuronCount << " neurons" << std::endl;
    
    PerformanceTestResult result;
    result.test_name = "Timing Test";
    result.steps = steps;
    
    try {
        // Create brain configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        // Create and initialize brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            result.passed = false;
            result.error_message = "Brain initialization failed";
            return result;
        }
        
        // Measure timing
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        result.duration_us = duration.count();
        result.avg_step_time_us = static_cast<double>(result.duration_us) / steps;
        
        // Calculate performance metrics
        double steps_per_second = (steps * 1000000.0) / result.duration_us;
        result.steps_per_second = steps_per_second;
        
        // Get brain statistics
        result.total_neurons = brain.getTotalNeuronCount();
        result.total_synapses = brain.getTotalSynapseCount();
        result.active_neurons = brain.getActiveNeuronCount();
        result.firing_neurons = brain.getFiringNeuronCount();
        result.average_firing_rate = brain.getAverageFiringRate();
        
        // Determine if test passed (should complete without errors)
        result.passed = result.total_neurons > 0 && result.duration_us > 0;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Exception: ") + e.what();
    }
    
    return result;
}

PerformanceTestResult PerformanceTestRunner::runMemoryTest(int steps, int neuronCount) {
    std::cout << "    Running memory test with " << steps << " steps, " << neuronCount << " neurons" << std::endl;
    
    PerformanceTestResult result;
    result.test_name = "Memory Test";
    result.steps = steps;
    
    try {
        // Create brain configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
        
        // Create and initialize brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            result.passed = false;
            result.error_message = "Brain initialization failed";
            return result;
        }
        
        // Measure memory usage over time
        std::vector<size_t> memory_samples;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
            
            // Take memory sample every 100 steps
            if (step % 100 == 0) {
                // Simulate memory measurement (would need OS-specific code)
                // For now, we'll use brain statistics as proxy
                memory_samples.push_back(brain.getTotalNeuronCount() + brain.getTotalSynapseCount());
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        result.duration_us = duration.count();
        result.avg_step_time_us = static_cast<double>(result.duration_us) / steps;
        
        // Calculate memory metrics
        if (!memory_samples.empty()) {
            result.peak_memory_usage = *std::max_element(memory_samples.begin(), memory_samples.end());
            result.avg_memory_usage = std::accumulate(memory_samples.begin(), memory_samples.end(), 0.0) / memory_samples.size();
            
            // Memory growth rate
            if (memory_samples.size() >= 2) {
                double initial_mem = memory_samples.front();
                double final_mem = memory_samples.back();
                result.memory_growth_rate = (final_mem - initial_mem) / initial_mem;
            }
        }
        
        // Get brain statistics
        result.total_neurons = brain.getTotalNeuronCount();
        result.total_synapses = brain.getTotalSynapseCount();
        
        // Determine if test passed
        result.passed = result.total_neurons > 0 && !memory_samples.empty();
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Exception: ") + e.what();
    }
    
    return result;
}

PerformanceTestResult PerformanceTestRunner::runScaleTest(int neuronCount) {
    std::cout << "    Running scale test with " << neuronCount << " neurons" << std::endl;
    
    PerformanceTestResult result;
    result.test_name = "Scale Test";
    result.steps = 100; // Fixed steps for scaling
    
    try {
        // Create brain configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(std::max(1, neuronCount / 100)), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
        
        // Measure initialization time
        auto start_time = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            result.passed = false;
            result.error_message = "Brain initialization failed";
            return result;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        result.duration_us = duration.count();
        
        // Calculate scale metrics
        double init_time_ms = result.duration_us / 1000.0;
        result.steps_per_second = 100.0 * 1000000.0 / result.duration_us; // Assuming 100 steps per second
        result.avg_step_time_us = init_time_ms * 1000.0; // Rough estimate
        
        // Get brain statistics
        result.total_neurons = brain.getTotalNeuronCount();
        result.total_synapses = brain.getTotalSynapseCount();
        
        // Calculate neurons per millisecond
        if (init_time_ms > 0) {
            result.neurons_per_ms = neuronCount / init_time_ms;
        }
        
        // Determine if test passed
        result.passed = result.total_neurons == neuronCount;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Exception: ") + e.what();
    }
    
    return result;
}

PerformanceTestResult PerformanceTestRunner::runStressTest(int steps, int neuronCount, float stressFactor) {
    std::cout << "    Running stress test with " << steps << " steps, " << neuronCount << " neurons, stress factor " << stressFactor << std::endl;
    
    PerformanceTestResult result;
    result.test_name = "Stress Test";
    result.steps = steps;
    
    try {
        // Create brain configuration with higher stress
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", stressFactor, nlm::ConfigSource::Default); // High connectivity
        
        // Create and initialize brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            result.passed = false;
            result.error_message = "Brain initialization failed";
            return result;
        }
        
        // Measure stress performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain.step(step);
            
            // Simulate stress with high activity
            if (step % 10 == 0) {
                // Inject high current to simulate stress
                for (size_t i = 0; i < 10 && i < brain.getTotalNeuronCount(); ++i) {
                    nlm::NeuronId stressNeuron(i + 1);
                    brain.injectCurrent(stressNeuron, 10.0f); // High current injection
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        result.duration_us = duration.count();
        result.avg_step_time_us = static_cast<double>(result.duration_us) / steps;
        
        // Calculate stress metrics
        double stress_steps_per_second = (steps * 1000000.0) / result.duration_us;
        result.steps_per_second = stress_steps_per_second;
        
        // Get brain statistics under stress
        result.total_neurons = brain.getTotalNeuronCount();
        result.total_synapses = brain.getTotalSynapseCount();
        result.active_neurons = brain.getActiveNeuronCount();
        result.firing_neurons = brain.getFiringNeuronCount();
        result.average_firing_rate = brain.getAverageFiringRate();
        
        // Calculate stress indicators
        if (result.total_synapses > 0) {
            result.excitation_inhibition_ratio = brain.getExcitationInhibitionRatio();
        }
        
        result.total_spikes = brain.getTotalSpikeCount();
        
        // Determine if test passed (should complete without crashing)
        result.passed = result.total_neurons > 0 && result.duration_us > 0 && result.total_spikes > 0;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Exception: ") + e.what();
    }
    
    return result;
}

PerformanceBenchmarkReport PerformanceTestRunner::runBenchmarkSuite(int steps, int neuronCount) {
    std::cout << "=== NLM Performance Benchmark Suite ===" << std::endl;
    
    PerformanceBenchmarkReport report;
    report.test_name = "Benchmark Suite";
    report.steps = steps;
    report.neuron_count = neuronCount;
    
    // Run all benchmark tests
    PerformanceTestResult timingResult = runTimingTest(steps, neuronCount);
    report.timing_tests.push_back(timingResult);
    
    PerformanceTestResult memoryResult = runMemoryTest(steps, neuronCount);
    report.memory_tests.push_back(memoryResult);
    
    PerformanceTestResult scaleResult = runScaleTest(neuronCount);
    report.scale_tests.push_back(scaleResult);
    
    PerformanceTestResult stressResult = runStressTest(steps, neuronCount, 0.3f);
    report.stress_tests.push_back(stressResult);
    
    // Generate summary report
    generateReport(report);
    
    return report;
}

void PerformanceTestRunner::generateReport(PerformanceBenchmarkReport& report) {
    std::cout << "\n=== Performance Benchmark Report ===" << std::endl;
    std::cout << "Test Name: " << report.test_name << std::endl;
    std::cout << "Steps: " << report.steps << std::endl;
    std::cout << "Neuron Count: " << report.neuron_count << std::endl;
    std::cout << std::endl;
    
    // Timing Tests
    std::cout << "Timing Tests:" << std::endl;
    for (const auto& test : report.timing_tests) {
        std::cout << "  - " << test.test_name << ": " << std::fixed << std::setprecision(2)
                  << test.avg_step_time_us << " µs/step, "
                  << test.steps_per_second << " steps/sec" << std::endl;
    }
    
    // Memory Tests
    std::cout << "\nMemory Tests:" << std::endl;
    for (const auto& test : report.memory_tests) {
        std::cout << "  - " << test.test_name << ": " << std::fixed << std::setprecision(2)
                  << test.avg_memory_usage << " avg usage, "
                  << test.peak_memory_usage << " peak usage" << std::endl;
    }
    
    // Scale Tests
    std::cout << "\nScale Tests:" << std::endl;
    for (const auto& test : report.scale_tests) {
        std::cout << "  - " << test.test_name << ": " << std::fixed << std::setprecision(2)
                  << test.duration_us << " µs init, "
                  << test.neurons_per_ms << " neurons/ms" << std::endl;
    }
    
    // Stress Tests
    std::cout << "\nStress Tests:" << std::endl;
    for (const auto& test : report.stress_tests) {
        std::cout << "  - " << test.test_name << ": " << std::fixed << std::setprecision(2)
                  << test.avg_step_time_us << " µs/step, "
                  << test.total_spikes << " total spikes" << std::endl;
    }
    
    // Overall Results
    std::cout << "\n=== Overall Results ===" << std::endl;
    int passed = 0;
    int total = report.timing_tests.size() + report.memory_tests.size() + 
                report.scale_tests.size() + report.stress_tests.size();
    
    for (const auto& test : report.timing_tests) {
        if (test.passed) passed++;
    }
    for (const auto& test : report.memory_tests) {
        if (test.passed) passed++;
    }
    for (const auto& test : report.scale_tests) {
        if (test.passed) passed++;
    }
    for (const auto& test : report.stress_tests) {
        if (test.passed) passed++;
    }
    
    std::cout << "Tests Passed: " << passed << "/" << total << " (" 
              << (total > 0 ? (passed * 100.0 / total) : 0.0) << "%)" << std::endl;
    
    // Recommendations
    std::cout << "\n=== Recommendations ===" << std::endl;
    if (report.timing_tests[0].avg_step_time_us > 10000) { // 10ms per step
        std::cout << "  - Consider optimizing step execution time for large networks" << std::endl;
    }
    if (report.memory_tests[0].peak_memory_usage > 1000000) { // 1M units
        std::cout << "  - Memory usage is high; consider memory optimization techniques" << std::endl;
    }
    if (report.stress_tests[0].excitation_inhibition_ratio > 10.0) {
        std::cout << "  - Excitation/Inhibition ratio is high; consider adjusting connection weights" << std::endl;
    }
}

void PerformanceTestRunner::printHeader(const std::string& testName) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
}

void PerformanceTestRunner::printFooter(const std::string& testName, bool passed, const std::string& details) {
    std::cout << testName << " - " << (passed ? "PASSED" : "FAILED") << std::endl;
    if (!details.empty()) {
        std::cout << "  Details: " << details << std::endl;
    }
}

} // namespace test_performance
