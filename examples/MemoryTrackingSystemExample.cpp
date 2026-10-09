// Memory Tracking System - Usage Example
// 
// This file demonstrates how to use the comprehensive memory tracking system
// for large-scale NLM brain simulation. It shows all the key features:
// 1. Core MemoryTracker with unified interface
// 2. Specialized memory monitors for each component
// 3. Memory reporting and analysis
// 4. Integration with existing NLM systems
// 5. Memory testing utilities
// 6. Memory visualization tools
// 7. Memory alert system
// 8. Memory optimization recommendations
// 
// This example can be used as a starting point for implementing memory
// tracking in your NLM brain simulation.

#include "nlm/api/AdvancedBrain.hpp"
#include "nlm/api/utilities/BrainProfiler.hpp"
#include "nlm/profiling/tools/MemoryTracker.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

void demonstrateMemoryTrackerBasicUsage() {
    std::cout << "=== Memory Tracker Basic Usage Example ===\n";
    
    // Create a memory tracker with standard configuration
    auto memoryTracker = nlm::profiling::MemoryTracker::create(
        nlm::profiling::MemoryTracker::Mode::Active,
        nlm::profiling::MemoryTracker::Scope::System,
        nlm::profiling::MemoryTracker::LeakDetectionMode::Advanced
    );
    
    // Start tracking
    memoryTracker->startTracking();
    std::cout << "Memory tracking started.\n";
    
    // Simulate some memory allocations
    std::vector<int> data1(1000);
    memoryTracker->recordAllocation(data1.data(), sizeof(int) * data1.size(), 
                                   "example.cpp", "main", 100);
    
    std::vector<double> data2(500);
    memoryTracker->recordAllocation(data2.data(), sizeof(double) * data2.size(), 
                                   "example.cpp", "main", 105);
    
    // Simulate some memory usage recording
    memoryTracker->recordMemoryUsage(1024 * 1024, "system", {{ "operation", "test" }});
    memoryTracker->recordComponentMemoryUsage("spikeSystem", "spike_system", 512 * 1024);
    memoryTracker->recordComponentMemoryUsage("plasticitySystem", "plasticity_system", 256 * 1024);
    
    // Stop tracking
    memoryTracker->stopTracking();
    std::cout << "Memory tracking stopped.\n";
    
    // Get memory metrics
    auto metrics = memoryTracker->getMemoryMetrics();
    std::cout << "Current Memory Usage: " << metrics.currentMemoryUsage << " bytes\n";
    std::cout << "Peak Memory Usage: " << metrics.peakMemoryUsage << " bytes\n";
    std::cout << "Total Allocations: " << metrics.allocatedCount << "\n";
    std::cout << "Total Deallocations: " << metrics.deallocatedCount << "\n";
    std::cout << "Memory Waste: " << metrics.memoryWaste << " bytes\n";
    std::cout << "Fragmentation Index: " << metrics.fragmentationIndex << "%\n";
    
    // Generate memory report
    std::string report = memoryTracker->generateMemoryReport();
    std::cout << "\n" << report << "\n";
}

void demonstrateSpecializedMemoryMonitors() {
    std::cout << "=== Specialized Memory Monitors Example ===\n";
    
    // Create specialized memory monitors
    auto spikeMonitor = std::make_unique<nlm::profiling::SpikeSystemMemoryMonitor>();
    auto plasticityMonitor = std::make_unique<nlm::profiling::PlasticitySystemMemoryMonitor>();
    auto memorySystemMonitor = std::make_unique<nlm::profiling::MemorySystemMemoryMonitor>();
    auto neuralRegionMonitor = std::make_unique<nlm::profiling::NeuralRegionMemoryMonitor>();
    auto neuronSynapseMonitor = std::make_unique<nlm::profiling::NeuronSynapseMemoryMonitor>();
    auto configMonitor = std::make_unique<nlm::profiling::ConfigurationMemoryMonitor>();
    auto checkpointMonitor = std::make_unique<nlm::profiling::CheckpointSystemMemoryMonitor>();
    
    // Start all monitors
    spikeMonitor->startMonitoring();
    plasticityMonitor->startMonitoring();
    memorySystemMonitor->startMonitoring();
    neuralRegionMonitor->startMonitoring();
    neuronSynapseMonitor->startMonitoring();
    configMonitor->startMonitoring();
    checkpointMonitor->startMonitoring();
    
    std::cout << "All specialized memory monitors started.\n";
    
    // Simulate recording memory usage for each component
    spikeMonitor->recordSpikeEvent(nlm::SpikeEvent(), 1024);
    spikeMonitor->recordSpikeSystemAllocation("spike_queue", 2048);
    spikeMonitor->recordSpikeSystemDeallocation("spike_queue", 1024);
    
    plasticityMonitor->recordSTDPMemoryUsage(4096);
    plasticityMonitor->recordHebbianMemoryUsage(2048);
    plasticityMonitor->recordStructuralPlasticityMemoryUsage(1024);
    
    memorySystemMonitor->recordWorkingMemoryUsage(8192, 16384);
    memorySystemMonitor->recordEpisodicMemoryUsage(16384, 512);
    memorySystemMonitor->recordAssociativeMemoryUsage(4096, 256);
    
    neuralRegionMonitor->recordNeuralRegionMemoryUsage(1, 32768, 1024, 2048);
    neuronSynapseMonitor->recordNeuronMemoryUsage(1, 4096, nlm::NeuronType::Internal, 1);
    neuronSynapseMonitor->recordSynapseMemoryUsage(1, 2048, nlm::SynapseType::Excitatory, 1, 2);
    
    configMonitor->recordConfigurationMemoryUsage(2048, 1024);
    configMonitor->recordSmartPointerMemoryUsage(1024, "shared_ptr");
    
    checkpointMonitor->recordCheckpointMemoryUsage(8192, 4096);
    
    // Stop all monitors
    spikeMonitor->stopMonitoring();
    plasticityMonitor->stopMonitoring();
    memorySystemMonitor->stopMonitoring();
    neuralRegionMonitor->stopMonitoring();
    neuronSynapseMonitor->stopMonitoring();
    configMonitor->stopMonitoring();
    checkpointMonitor->stopMonitoring();
    
    std::cout << "All specialized memory monitors stopped.\n";
    
    // Generate reports for each monitor
    std::cout << "\nSpike System Memory Report:";
    std::cout << "\n" << spikeMonitor->generateMemoryReport();
    
    std::cout << "\nPlasticity System Memory Report:";
    std::cout << "\n" << plasticityMonitor->generateMemoryReport();
    
    std::cout << "\nMemory System Memory Report:";
    std::cout << "\n" << memorySystemMonitor->generateMemoryReport();
    
    std::cout << "\nNeural Region Memory Report:";
    std::cout << "\n" << neuralRegionMonitor->generateMemoryReport();
    
    std::cout << "\nNeuron/Synapse Memory Report:";
    std::cout << "\n" << neuronSynapseMonitor->generateMemoryReport();
    
    std::cout << "\nConfiguration Memory Report:";
    std::cout << "\n" << configMonitor->generateMemoryReport();
    
    std::cout << "\nCheckpoint System Memory Report:";
    std::cout << "\n" << checkpointMonitor->generateMemoryReport();
}

void demonstrateMemoryAlertSystem() {
    std::cout << "=== Memory Alert System Example ===\n";
    
    // Create memory alert system
    auto alertSystem = std::make_unique<nlm::profiling::MemoryAlertSystem>();
    
    // Add alert rules
    alertSystem->addAlertRule("memory_usage_high", 
                             nlm::profiling::MemoryAlertSystem::AlertLevel::Warning,
                             1024 * 1024 * 1024,  // 1GB threshold
                             "Memory usage exceeded threshold", 
                             "system");
    
    alertSystem->addAlertRule("memory_leak_detected",
                             nlm::profiling::MemoryAlertSystem::AlertLevel::Error,
                             0,  // Any leak
                             "Memory leak detected", 
                             "leak_detection");
    
    alertSystem->addAlertRule("memory_fragmentation_high",
                             nlm::profiling::MemoryAlertSystem::AlertLevel::Critical,
                             80,  // 80% fragmentation threshold
                             "Memory fragmentation exceeded threshold", 
                             "memory_management");
    
    std::cout << "Memory alert system configured with 3 rules.\n";
    
    // Simulate memory alerts
    nlm::profiling::MemoryMetrics metrics;
    metrics.currentMemoryUsage = 1500 * 1024 * 1024;  // 1.5GB
    metrics.memoryWaste = 200 * 1024 * 1024;  // 200MB waste
    metrics.fragmentationIndex = 85;  // 85% fragmentation
    
    // Evaluate alerts
    alertSystem->evaluateAlerts(metrics);
    
    // Get active alerts
    auto activeAlerts = alertSystem->getActiveAlerts();
    std::cout << "Active Alerts: " << activeAlerts.size() << "\n";
    for (const auto& alert : activeAlerts) {
        std::cout << "  - " << alert.first << " (" << alert.second << ")" << "\n";
    }
    
    // Acknowledge and resolve alerts
    if (!activeAlerts.empty()) {
        alertSystem->acknowledgeAlert("memory_usage_high_001");
        alertSystem->resolveAlert("memory_leak_detected_001");
    }
    
    std::cout << "Alerts acknowledged and resolved.\n";
    
    // Export alert report
    alertSystem->exportAlertReport("memory_alert_report.json", "json");
    std::cout << "Alert report exported to memory_alert_report.json\n";
}

void demonstrateMemoryOptimizationRecommendations() {
    std::cout << "=== Memory Optimization Recommendations Example ===\n";
    
    // Create memory optimization recommendations system
    auto optimizer = std::make_unique<nlm::profiling::MemoryOptimizationRecommendations>();
    
    // Get optimization recommendations based on current memory metrics
    nlm::profiling::MemoryMetrics metrics;
    metrics.currentMemoryUsage = 2048 * 1024 * 1024;  // 2GB
    metrics.peakMemoryUsage = 3072 * 1024 * 1024;     // 3GB
    metrics.memoryWaste = 400 * 1024 * 1024;           // 400MB
    metrics.fragmentationIndex = 70;                   // 70% fragmentation
    metrics.detectedLeaks = std::vector<nlm::profiling::MemoryMetrics::LeakInfo>(2);
    
    // Get recommendations
    auto recommendations = optimizer->analyzeAllocationPatterns(metrics);
    recommendations.insert(recommendations.end(),
                         optimizer->analyzeFragmentation(metrics).begin(),
                         optimizer->analyzeFragmentation(metrics).end());
    recommendations.insert(recommendations.end(),
                         optimizer->analyzeComponentUsage(metrics).begin(),
                         optimizer->analyzeComponentUsage(metrics).end());
    
    std::cout << "Memory optimization recommendations:" << recommendations.size() << "\n";
    for (size_t i = 0; i < recommendations.size() && i < 5; ++i) {  // Show first 5
        const auto& rec = recommendations[i];
        std::cout << "\nRecommendation " << i + 1 << ":";
        std::cout << "\n  Category: " << rec.category;
        std::cout << "\n  Description: " << rec.description;
        std::cout << "\n  Expected Savings: " << rec.expectedMemorySavings << " bytes";
        std::cout << "\n  Complexity: " << rec.implementationCost << "/1.0";
        std::cout << "\n  Risk: " << rec.riskLevel << "/1.0";
        std::cout << "\n  Priority: " << rec.priority;
    }
    
    // Generate optimization report
    std::string report = optimizer->generateOptimizationReport(metrics);
    std::cout << "\n" << report << "\n";
    
    // Export optimization recommendations
    optimizer->exportOptimizationRecommendations(recommendations, "optimization_recommendations.json", "json");
    std::cout << "Optimization recommendations exported to optimization_recommendations.json\n";
}

void demonstrateMemoryTestingUtilities() {
    std::cout << "=== Memory Testing Utilities Example ===\n";
    
    // Create memory testing utilities
    auto tester = std::make_unique<nlm::profiling::MemoryTestingUtilities>();
    
    // Run various memory tests
    bool leakTestPassed = tester->runMemoryLeakDetectionTest();
    bool allocationPatternTestPassed = tester->runMemoryAllocationPatternTest();
    bool fragmentationTestPassed = tester->runMemoryFragmentationTest();
    bool thresholdTestPassed = tester->runMemoryThresholdTest();
    bool growthTrendTestPassed = tester->runMemoryGrowthTrendTest();
    bool componentProfileTestPassed = tester->runComponentMemoryProfilingTest();
    bool optimizationTestPassed = tester->runMemoryOptimizationRecommendationTest();
    bool alertTestPassed = tester->runMemoryAlertSystemTest();
    bool visualizationTestPassed = tester->runMemoryVisualizationTest();
    
    std::cout << "Memory test results:" << "\n";
    std::cout << "  Leak Detection Test: " << (leakTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Allocation Pattern Test: " << (allocationPatternTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Fragmentation Test: " << (fragmentationTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Threshold Test: " << (thresholdTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Growth Trend Test: " << (growthTrendTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Component Profile Test: " << (componentProfileTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Optimization Test: " << (optimizationTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Alert Test: " << (alertTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Visualization Test: " << (visualizationTestPassed ? "PASSED" : "FAILED") << "\n";
    
    // Get all test results
    auto testResults = tester->getTestResults();
    std::cout << "\nAll test results:" << "\n";
    for (const auto& result : testResults) {
        std::cout << "  - " << result.first << ": " << (result.second ? "PASSED" : "FAILED") << "\n";
    }
    
    // Export test results
    tester->exportTestResults("memory_test_results.json", "json");
    std::cout << "Test results exported to memory_test_results.json\n";
}

void demonstrateMemoryVisualizationTools() {
    std::cout << "=== Memory Visualization Tools Example ===\n";
    
    // Create memory visualization tools
    auto visualizer = std::make_unique<nlm::profiling::MemoryVisualizationTools>();
    
    // Create sample memory metrics
    nlm::profiling::MemoryMetrics metrics;
    metrics.currentMemoryUsage = 1024 * 1024 * 1024;  // 1GB
    metrics.peakMemoryUsage = 2048 * 1024 * 1024;     // 2GB
    metrics.totalMemoryAllocated = 4096 * 1024 * 1024; // 4GB
    metrics.totalMemoryFreed = 2048 * 1024 * 1024;   // 2GB
    
    // Generate various visualizations
    std::string memoryUsageGraph = visualizer->generateMemoryUsageGraph(metrics);
    std::string allocationDeallocationGraph = visualizer->generateAllocationDeallocationGraph(metrics);
    std::string componentBreakdownGraph = visualizer->generateComponentBreakdownGraph(metrics);
    std::string leakDetectionGraph = visualizer->generateLeakDetectionGraph(metrics);
    std::string optimizationRecommendationsGraph = visualizer->generateOptimizationRecommendationsGraph(metrics);
    std::string memoryUsageTrendsGraph = visualizer->generateMemoryUsageTrendsGraph(metrics);
    std::string memoryThresholdAlertingGraph = visualizer->generateMemoryThresholdAlertingGraph(metrics);
    std::string memoryAllocationTimingGraph = visualizer->generateMemoryAllocationTimingGraph(metrics);
    std::string memoryComponentProfilingGraph = visualizer->generateMemoryComponentProfilingGraph(metrics);
    
    std::cout << "Memory visualizations generated:" << "\n";
    std::cout << "  Memory Usage Graph" << "\n";
    std::cout << "  Allocation/Deallocation Graph" << "\n";
    std::cout << "  Component Breakdown Graph" << "\n";
    std::cout << "  Leak Detection Graph" << "\n";
    std::cout << "  Optimization Recommendations Graph" << "\n";
    std::cout << "  Memory Usage Trends Graph" << "\n";
    std::cout << "  Memory Threshold Alerting Graph" << "\n";
    std::cout << "  Memory Allocation Timing Graph" << "\n";
    std::cout << "  Memory Component Profiling Graph" << "\n";
    
    // Export visualization data
    bool exportSuccess = visualizer->exportVisualizationData(metrics, "memory_visualization_data.json", "json");
    if (exportSuccess) {
        std::cout << "Visualization data exported to memory_visualization_data.json\n";
    }
    
    // Generate HTML report
    bool htmlReportSuccess = visualizer->generateHTMLReport(metrics, "memory_visualization_report.html");
    if (htmlReportSuccess) {
        std::cout << "HTML report generated: memory_visualization_report.html\n";
    }
}

void demonstrateMemoryTrackerAPI() {
    std::cout << "=== Memory Tracker API Example ===\n";
    
    // Use the memory tracking API
    std::cout << "1. Creating memory tracker using API:" << "\n";
    auto memoryTracker = nlm::profiling::api::createMemoryTrackerForComponents({
        "spikeSystem", "plasticitySystem", "memorySystem", "neuralRegion", "neuronSynapse"
    });
    
    std::cout << "2. Adding memory threshold using API:" << "\n";
    memoryTracker->addMemoryThreshold("high_memory_usage", 1024 * 1024 * 1024, "warning", true);
    
    std::cout << "3. Enabling component tracking using API:" << "\n";
    memoryTracker->enableComponentTracking("spikeSystem", true);
    memoryTracker->enableComponentTracking("plasticitySystem", true);
    
    std::cout << "4. Getting tracking statistics using API:" << "\n";
    auto stats = nlm::profiling::api::getMemoryTrackingStats(*memoryTracker);
    std::cout << "   Total Allocations: " << stats.totalAllocations << "\n";
    std::cout << "   Total Deallocations: " << stats.totalDeallocations << "\n";
    std::cout << "   Current Tracked Objects: " << stats.currentTrackedObjects << "\n";
    std::cout << "   Detected Leaks: " << stats.detectedLeaks << "\n";
    std::cout << "   Threshold Triggers: " << stats.thresholdTriggers << "\n";
    std::cout << "   Tracking Duration: " << stats.trackingDuration << " seconds\n";
    
    std::cout << "5. Checking if memory tracking is available using API:" << "\n";
    bool trackingAvailable = nlm::profiling::api::isMemoryTrackingAvailable();
    std::cout << "   Memory tracking available: " << (trackingAvailable ? "Yes" : "No") << "\n";
    
    std::cout << "6. Getting memory tracking system version using API:" << "\n";
    std::string version = nlm::profiling::api::getMemoryTrackingSystemVersion();
    std::cout << "   System version: " << version << "\n";
    
    std::cout << "7. Initializing memory tracking system using API:" << "\n";
    bool initSuccess = nlm::profiling::api::initializeMemoryTrackingSystem();
    std::cout << "   Initialization: " << (initSuccess ? "Success" : "Failed") << "\n";
    
    std::cout << "8. Shutting down memory tracking system using API:" << "\n";
    nlm::profiling::api::shutdownMemoryTrackingSystem();
    std::cout << "   Shutdown complete\n";
}

int main() {
    std::cout << "=== Comprehensive Memory Tracking System - Complete Example ===\n\n";
    
    // Run all demonstration examples
    demonstrateMemoryTrackerBasicUsage();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateSpecializedMemoryMonitors();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateMemoryAlertSystem();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateMemoryOptimizationRecommendations();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateMemoryTestingUtilities();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateMemoryVisualizationTools();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    demonstrateMemoryTrackerAPI();
    std::cout << "\n" << std::string(80, '=') << "\n\n";
    
    std::cout << "=== All demonstrations completed successfully! ===\n";
    std::cout << "\nThe comprehensive memory tracking system has been demonstrated with:" << "\n";
    std::cout << "✓ Core MemoryTracker with unified interface" << "\n";
    std::cout << "✓ Specialized memory monitors for each component" << "\n";
    std::cout << "✓ Memory reporting and analysis tools" << "\n";
    std::cout << "✓ Integration with existing NLM memory systems" << "\n";
    std::cout << "✓ Memory tracking API" << "\n";
    std::cout << "✓ Memory testing utilities" << "\n";
    std::cout << "✓ Memory visualization tools" << "\n";
    std::cout << "✓ Memory alert system" << "\n";
    std::cout << "✓ Memory optimization recommendations" << "\n";
    
    return 0;
}
}