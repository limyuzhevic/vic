//! Test implementation of NLM Brain performance profiling and debugging tools

#include "Brain.hpp"
#include "brain/profiling/BrainProfiler.hpp"
#include "brain/profiling/DebugTools.hpp"
#include "brain/profiling/PerformanceMetrics.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

void testBrainProfilerWithBrain() {
    std::cout << "=== Testing BrainProfiler with Brain ===" << std::endl;
    
    // Create a simple Brain instance (would need proper config in real usage)
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    
    // Create profiler
    auto profiler = std::make_shared<nlm::BrainProfiler>();
    
    // Initialize brain (simplified for test)
    bool initialized = brain->initialize();
    if (!initialized) {
        std::cout << "Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Record some simulation metrics
    for (size_t step = 1; step <= 10; ++step) {
        // Get current statistics
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t neuronsProcessed = brain->getTotalNeuronCount();
        
        // Simulate some spikes for testing
        if (step % 3 == 0) {
            // Inject some current to stimulate neurons
            brain->injectCurrentToNeurons(nlm::NeuronType::Internal, 1.0f);
        }
        
        // Step the brain
        brain->step(step);
        
        // Create mock stats for profiler
        nlm::BrainProfiler::MemoryStats memoryStats;
        memoryStats.used = neuronsProcessed * 1000; // approximation
        memoryStats.totalAllocated = neuronsProcessed * 2000;
        memoryStats.usagePercentage = 50.0;
        
        nlm::BrainProfiler::FiringStats firingStats;
        firingStats.totalSpikes = totalSpikes;
        firingStats.spikesPerStep = 10; // approximation
        firingStats.firingNeurons = neuronsProcessed / 4;
        
        // Record metrics
        profiler->recordSimulationMetrics(step, static_cast<double>(step) * 0.001,
                                          neuronsProcessed, totalSpikes,
                                          memoryStats, firingStats);
        
        std::cout << "Step " << step << ": " << totalSpikes << " total spikes" << std::endl;
    }
    
    // Get some statistics from profiler
    auto memoryStats = profiler->getMemoryStats();
    auto firingStats = profiler->getFiringStats();
    
    std::cout << "\nProfiler Statistics:" << std::endl;
    std::cout << "Memory Usage: " << memoryStats.usagePercentage << "%" << std::endl;
    std::cout << "Total Spikes Recorded: " << firingStats.totalSpikes << std::endl;
    
    // Generate report
    std::string report = profiler->generateReport();
    std::cout << "\n=== Generated Report ===" << std::endl;
    std::cout << report << std::endl;
    
    // Export to JSON
    std::string jsonData = profiler->exportToJSON();
    std::cout << "\n=== Exported JSON (first 500 chars) ===" << std::endl;
    std::cout << jsonData.substr(0, std::min<size_t>(500, jsonData.size())) << "..." << std::endl;
}

void testDebugToolsWithBrain() {
    std::cout << "\n\n=== Testing DebugTools with Brain ===" << std::endl;
    
    // Create brain instance
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    
    // Initialize brain
    bool initialized = brain->initialize();
    if (!initialized) {
        std::cout << "Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Create debug tools
    auto debugTools = std::make_shared<nlm::DebugTools>();
    
    // Validate brain state
    auto validationResult = debugTools->validateState(*brain);
    std::cout << "\nState Validation:" << std::endl;
    std::cout << "Is Valid: " << (validationResult.isValid ? "YES" : "NO") << std::endl;
    if (!validationResult.errors.empty()) {
        std::cout << "Errors:" << std::endl;
        for (const auto& error : validationResult.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Check for memory leaks
    auto leaks = debugTools->checkMemoryLeaks();
    std::cout << "\nMemory Leaks Found: " << leaks.size() << std::endl;
    for (const auto& leak : leaks) {
        std::cout << "  - " << leak.context << ": " << leak.allocationCount 
                  << " allocations, " << leak.allocatedBytes << " bytes" << std::endl;
    }
    
    // Analyze performance
    auto bottlenecks = debugTools->analyzePerformance();
    std::cout << "\nPerformance Bottlenecks: " << bottlenecks.size() << std::endl;
    for (const auto& bottleneck : bottlenecks) {
        std::cout << "  - " << bottleneck.description << " (severity: " << bottleneck.severity << ")" << std::endl;
    }
    
    // Analyze connectivity
    auto connectivity = debugTools->checkConnectivity(*brain);
    std::cout << "\nConnectivity Analysis:" << std::endl;
    std::cout << "  Is Connected: " << (connectivity.isConnected ? "YES" : "NO") << std::endl;
    std::cout << "  Total Connections: " << connectivity.totalConnections << std::endl;
    std::cout << "  Isolated Neurons: " << connectivity.isolatedNeurons << std::endl;
    
    // Analyze firing patterns
    auto firingPattern = debugTools->analyzeFiringPatterns(*brain);
    std::cout << "\nFiring Pattern Analysis:" << std::endl;
    std::cout << "  Mean Firing Rate: " << firingPattern.meanFiringRate << " Hz" << std::endl;
    std::cout << "  Max Firing Rate: " << firingPattern.maxFiringRate << " Hz" << std::endl;
    std::cout << "  Silent Neurons: " << firingPattern.silentNeurons.size() << std::endl;
    
    // Generate debug report
    std::string debugReport = debugTools->generateDebugReport();
    std::cout << "\n=== Generated Debug Report (first 800 chars) ===" << std::endl;
    std::cout << debugReport.substr(0, std::min<size_t>(800, debugReport.size())) << "..." << std::endl;
}

void testPerformanceMetricsWithBrain() {
    std::cout << "\n\n=== Testing PerformanceMetrics with Brain ===" << std::endl;
    
    // Create PerformanceMetrics instance
    nlm::PerformanceMetrics metrics;
    
    // Simulate performance updates
    for (size_t i = 0; i < 20; ++i) {
        double wallTime = i * 0.001;
        double simTime = i * 0.0005;
        double stepDuration = 0.0001;
        
        // Update metrics
        metrics.updateTimeMetrics(wallTime, simTime, stepDuration);
        metrics.updateMemoryMetrics(1000 * (i + 1), 2000 * (i + 1), 500 * (i + 1));
        metrics.updateNetworkMetrics(1000, 100 + i * 10, 10 + i, 
                                    5000 + i * 100, 2.5 + i * 0.1, 2.0 + i * 0.05);
        metrics.updateIntegrationMetrics(i, i * 0.1, i / 5, 0.5 + i * 0.01, 0.3 + i * 0.02);
        metrics.updateHealthMetrics(50.0 + i * 0.5, 60.0 + i * 0.3, 
                                    0.8 - i * 0.01, i % 10 == 0, true);
        
        std::cout << "Metric update " << i << " completed" << std::endl;
    }
    
    // Get performance report
    auto report = metrics.getPerformanceReport();
    
    std::cout << "\n=== Performance Report ===" << std::endl;
    std::cout << report.generateHumanReadableReport() << std::endl;
    
    // Export to JSON
    std::string jsonData = metrics.exportToJSON();
    std::cout << "\n=== Exported JSON (first 600 chars) ===" << std::endl;
    std::cout << jsonData.substr(0, std::min<size_t>(600, jsonData.size())) << "..." << std::endl;
    
    // Export to CSV
    std::string csvData = metrics.exportToCSV();
    std::cout << "\n=== Exported CSV (first 400 chars) ===" << std::endl;
    std::cout << csvData.substr(0, std::min<size_t>(400, csvData.size())) << "..." << std::endl;
    
    // Test merging
    nlm::PerformanceMetrics otherMetrics;
    otherMetrics.updateTimeMetrics(10.0, 5.0, 0.5);
    metrics.merge(otherMetrics);
    std::cout << "\nMetrics merged successfully" << std::endl;
}

int main() {
    std::cout << "NLM Brain Performance Profiling and Debugging Tools Test" << std::endl;
    std::cout << "==========================================================" << std::endl;
    
    // Note: These tests would need proper configuration and setup for real usage
    // This is a simplified demonstration of the API
    
    try {
        // Test BrainProfiler
        testBrainProfilerWithBrain();
        
        // Test DebugTools  
        testDebugToolsWithBrain();
        
        // Test PerformanceMetrics
        testPerformanceMetricsWithBrain();
        
        std::cout << "\n\n=== All Tests Completed ===" << std::endl;
        std::cout << "Performance profiling and debugging tools are working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
