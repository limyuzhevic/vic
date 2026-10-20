// NLM Performance Monitoring and Optimization Demonstration
// 
// This file demonstrates the enhanced performance monitoring capabilities
// added to the NLM logging system.

#include "core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace nlm;

void simulateMemoryIntensiveOperation(const std::string& operation, size_t dataSize) {
    Logger::getGlobal().startPerformanceMeasurement(operation);
    
    // Simulate memory-intensive computation
    std::vector<double> data(dataSize);
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    double sum = 0.0;
    for (size_t i = 0; i < dataSize; ++i) {
        data[i] = dist(rng);
        sum += data[i];
        
        // Record memory usage periodically
        if (i % 1000 == 0) {
            Logger::getGlobal().recordMemoryUsage(sizeof(double) * data.size());
        }
    }
    
    // Clean up data (simulate memory release)
    data.clear();
    data.shrink_to_fit();
    
    Logger::getGlobal().recordMetric("data_points_processed", static_cast<double>(dataSize));
    Logger::getGlobal().recordMetric("sum_result", sum);
    
    Logger::getGlobal().endPerformanceMeasurement(operation);
}

void demonstrateMemoryManagement() {
    NLM_LOG_INFO("=== Memory Management Demonstration ===");
    
    // Simulate various memory-intensive operations
    simulateMemoryIntensiveOperation("neural_network_training", 100000);
    simulateMemoryIntensiveOperation("memory_allocation", 50000);
    simulateMemoryIntensiveOperation("data_processing", 75000);
    
    // Memory usage analysis
    NLM_LOG_INFO("Memory management analysis complete");
    NLM_LOG_INFO("Current memory usage: " + std::to_string(Logger::getGlobal().getCurrentMemoryUsage()) + " bytes");
    NLM_LOG_INFO("Peak memory usage: " + std::to_string(Logger::getGlobal().getPeakMemoryUsage()) + " bytes");
    
    // Memory optimization suggestions
    std::vector<std::string> suggestions = Logger::getGlobal().getOptimizationSuggestions();
    if (!suggestions.empty()) {
        NLM_LOG_INFO("Memory optimization suggestions:");
        for (const auto& suggestion : suggestions) {
            NLM_LOG_INFO("  - " + suggestion);
        }
    }
}

void demonstrateErrorHandling() {
    NLM_LOG_INFO("=== Error Handling Demonstration ===");
    
    // Demonstrate different error categories
    NLM_LOG_ERROR("General system error", ErrorCategory::General);
    NLM_LOG_ERROR("Memory allocation failed", ErrorCategory::Memory);
    NLM_LOG_ERROR("Plasticity rule violated", ErrorCategory::Plasticity);
    NLM_LOG_ERROR("Network connectivity issue", ErrorCategory::Network);
    NLM_LOG_ERROR("Configuration error", ErrorCategory::Configuration);
    NLM_LOG_ERROR("File I/O error", ErrorCategory::FileIO);
    NLM_LOG_ERROR("Performance degradation detected", ErrorCategory::Performance);
    NLM_LOG_ERROR("Synchronization failed", ErrorCategory::Synchronization);
    NLM_LOG_ERROR("Validation error", ErrorCategory::Validation);
    NLM_LOG_ERROR("Neural dynamics instability", ErrorCategory::NeuralDynamics);
}

void demonstratePerformanceThresholds() {
    NLM_LOG_INFO("=== Performance Threshold Demonstration ===");
    
    // Set up performance alerts
    Logger::getGlobal().setPerformanceAlert("brain_initialization_time", 5.0, AlertLevel::Warning);
    Logger::getGlobal().setPerformanceAlert("plasticity_computation", 10.0, AlertLevel::Error);
    
    // Simulate performance-critical operations
    double initializationTime = 3.2; // seconds
    double plasticityTime = 8.5; // seconds
    
    if (Logger::getGlobal().checkPerformanceThreshold("brain_initialization_time", 5.0, initializationTime)) {
        NLM_LOG_WARNING("Brain initialization approaching performance threshold");
    }
    
    if (Logger::getGlobal().checkPerformanceThreshold("plasticity_computation", 10.0, plasticityTime)) {
        NLM_LOG_ERROR("Plasticity computation exceeded performance threshold");
    }
}

void demonstrateProfiling() {
    NLM_LOG_INFO("=== Performance Profiling Demonstration ===");
    
    // Start profiling different operations
    Logger::getGlobal().startProfiling("brain_setup");
    simulateIntensiveComputation("memory_allocation", 5000);
    Logger::getGlobal().endProfiling("brain_setup");
    
    Logger::getGlobal().startProfiling("plasticity_simulation");
    simulateIntensiveComputation("plasticity_updates", 10000);
    Logger::getGlobal().endProfiling("plasticity_simulation");
    
    Logger::getGlobal().startProfiling("network_simulation");
    simulateIntensiveComputation("spike_propagation", 8000);
    Logger::getGlobal().endProfiling("network_simulation");
    
    // Generate performance report
    std::string report = Logger::getGlobal().generatePerformanceReport();
    NLM_LOG_INFO("=== Performance Profile Report ===");
    NLM_LOG_INFO(report);
}

void demonstrateMemoryMonitoring() {
    NLM_LOG_INFO("=== Memory Monitoring Demonstration ===");
    
    // Simulate memory usage tracking
    std::vector<double> memorySamples;
    
    for (int i = 0; i < 20; ++i) {
        size_t memoryUsage = 50 + i * 10; // Simulate increasing memory usage
        Logger::getGlobal().recordMemoryUsage(memoryUsage);
        memorySamples.push_back(static_cast<double>(memoryUsage));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    NLM_LOG_INFO("Current memory usage: " + std::to_string(Logger::getGlobal().getCurrentMemoryUsage()) + " bytes");
    NLM_LOG_INFO("Peak memory usage: " + std::to_string(Logger::getGlobal().getPeakMemoryUsage()) + " bytes");
    
    // Memory usage analysis
    double avgMemory = std::accumulate(memorySamples.begin(), memorySamples.end(), 0.0) / memorySamples.size();
    double maxMemory = *std::max_element(memorySamples.begin(), memorySamples.end());
    
    NLM_LOG_INFO("Memory analysis: Average usage = " + std::to_string(avgMemory) + " bytes");
    NLM_LOG_INFO("Memory analysis: Peak usage = " + std::to_string(maxMemory) + " bytes");
}

void demonstrateMemoryManagement();

int main() {
    std::cout << "=== NLM Enhanced Performance and Error Handling Demo ===" << std::endl;
    std::cout << "Demonstrating advanced logging capabilities..." << std::endl << std::endl;
    
    // Initialize logger with enhanced features
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Run demonstrations
    demonstrateErrorHandling();
    std::cout << std::endl;
    
    demonstratePerformanceThresholds();
    std::cout << std::endl;
    
    demonstrateProfiling();
    std::cout << std::endl;
    
    demonstrateMemoryManagement();
    std::cout << std::endl;
    
    NLM_LOG_INFO("=== Performance and Error Handling Demo Complete ===");
    NLM_LOG_INFO("Enhanced logging features demonstrated successfully!");
    
    return 0;
}
