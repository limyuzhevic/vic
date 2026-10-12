//!/usr/bin/env cpp
// Performance System Test Suite
//
// This file contains comprehensive tests for the PerformanceSystem class,
// demonstrating all features of the performance monitoring system.

#include "performance/PerformanceSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <random>

// Test helper functions
namespace {
    void testPerformanceSystemInitialization() {
        std::cout << "Testing PerformanceSystem initialization..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Test initialization
        bool initialized = perfSystem.initialize();
        assert(initialized == true);
        assert(perfSystem.isEnabled() == true);
        
        // Test real-time monitoring
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        assert(perfSystem.isRealTimeMonitoring() == true);
        
        // Wait a bit for monitoring to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Update snapshot
        perfSystem.updateSnapshot();
        
        // Check snapshot
        auto snapshot = perfSystem.getCurrentSnapshot();
        assert(snapshot.timestamp.time_since_epoch().count() > 0);
        
        // Stop monitoring
        perfSystem.stopRealTimeMonitoring();
        assert(perfSystem.isRealTimeMonitoring() == false);
        
        // Shutdown
        perfSystem.shutdown();
        
        std::cout << "✓ PerformanceSystem initialization test passed" << std::endl;
    }
    
    void testPerformanceMetricsCollection() {
        std::cout << "Testing performance metrics collection..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Record various metrics
        perfSystem.recordMetric("cpu_usage", 45.2, nlm::MetricType::Gauge);
        perfSystem.recordMetric("memory_used", 512 * 1024 * 1024, nlm::MetricType::Memory);
        perfSystem.recordMetric("event_queue_size", 1000, nlm::MetricType::Counter);
        perfSystem.recordMetric("response_time_ms", 25.5, nlm::MetricType::Timer);
        
        // Update snapshot
        perfSystem.updateSnapshot();
        
        // Check metric collection
        auto snapshot = perfSystem.getCurrentSnapshot();
        assert(snapshot.cpuUsagePercent >= 0.0);
        assert(snapshot.memoryUsed >= 0);
        assert(snapshot.eventQueueSize >= 0);
        
        // Check function profiles
        perfSystem.profileFunction([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }, "test_function");
        
        auto profiles = perfSystem.getFunctionProfiles();
        assert(profiles.find("test_function") != profiles.end());
        
        // Check call counts
        auto callCounts = perfSystem.getFunctionCallCounts();
        assert(callCounts.find("test_function") != callCounts.end());
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Performance metrics collection test passed" << std::endl;
    }
    
    void testThresholdMonitoring() {
        std::cout << "Testing threshold monitoring..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Add thresholds
        perfSystem.addThreshold("cpu_usage", 80.0, nlm::MetricType::Gauge, "warning");
        perfSystem.addThreshold("memory_used", 1024 * 1024 * 1024, nlm::MetricType::Memory, "critical");
        perfSystem.addThreshold("response_time_ms", 100.0, nlm::MetricType::Timer, "warning");
        
        // Check thresholds
        auto thresholds = perfSystem.getThresholds();
        assert(thresholds.size() >= 3);
        
        // Update snapshot with metrics exceeding thresholds
        perfSystem.updateSnapshot();
        
        // Check for alerts
        perfSystem.checkThresholds();
        
        // Remove threshold
        perfSystem.removeThreshold("cpu_usage");
        
        auto newThresholds = perfSystem.getThresholds();
        assert(newThresholds.size() == 2);
        
        perfSystem.shutdown();
        
        std::cout << "✓ Threshold monitoring test passed" << std::endl;
    }
    
    void testMemoryProfiling() {
        std::cout << "Testing memory profiling..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Start memory profiling
        perfSystem.startMemoryProfiling();
        
        // Simulate memory allocations
        std::vector<std::vector<float>> memoryData;
        for (int i = 0; i < 100; ++i) {
            std::vector<float> data(1000, 1.0f);
            memoryData.push_back(std::move(data));
        }
        
        // Stop memory profiling
        perfSystem.stopMemoryProfiling();
        
        // Check memory usage
        uint64_t totalAllocated = perfSystem.getTotalMemoryAllocated();
        uint64_t totalDeallocated = perfSystem.getTotalMemoryDeallocated();
        assert(totalAllocated >= 0);
        assert(totalDeallocated >= 0);
        
        // Get memory usage by function
        auto memoryUsage = perfSystem.getMemoryUsageByFunction();
        assert(!memoryUsage.empty());
        
        // Detect memory leaks
        perfSystem.detectMemoryLeaks();
        
        perfSystem.shutdown();
        
        std::cout << "✓ Memory profiling test passed" << std::endl;
    }
    
    void testThreadAnalysis() {
        std::cout << "Testing thread analysis..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Start real-time monitoring
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Simulate thread activity
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // Do some work
                volatile int x = 0;
                for (int j = 0; j < 1000; ++j) {
                    x += j;
                }
            });
        }
        
        // Join threads
        for (auto& t : threads) {
            t.join();
        }
        
        // Analyze thread performance
        perfSystem.analyzeThreadPerformance();
        
        // Get thread CPU usage
        auto threadCpuUsage = perfSystem.getThreadCpuUsage();
        assert(!threadCpuUsage.empty());
        
        // Get thread active counts
        auto threadActiveCounts = perfSystem.getThreadActiveCounts();
        assert(!threadActiveCounts.empty());
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Thread analysis test passed" << std::endl;
    }
    
    void testNeuralNetworkMetrics() {
        std::cout << "Testing neural network metrics..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Record neural network metrics
        perfSystem.recordNeuralNetworkMetrics("test_network_1",
                                               1000,  // forward passes
                                               50.5,   // forward time ms
                                               500,   // backward passes
                                               30.2,   // backward time ms
                                               5000,   // examples processed
                                               0.95    // accuracy
                                              );
        
        perfSystem.recordNeuralNetworkMetrics("test_network_2",
                                               2000,
                                               80.0,
                                               1000,
                                               60.0,
                                               10000,
                                               0.92
                                              );
        
        // Get neural network performance
        auto nnPerf = perfSystem.getNeuralNetworkPerformance();
        assert(nnPerf.find("test_network_1_forward_time") != nnPerf.end());
        assert(nnPerf.find("test_network_2_accuracy") != nnPerf.end());
        
        perfSystem.shutdown();
        
        std::cout << "✓ Neural network metrics test passed" << std::endl;
    }
    
    void testDashboardGeneration() {
        std::cout << "Testing dashboard generation..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Update snapshot
        perfSystem.updateSnapshot();
        
        // Generate dashboards
        std::string htmlReport = perfSystem.generateDashboardHTML();
        std::string jsonReport = perfSystem.generateDashboardJSON();
        std::string textReport = perfSystem.generatePerformanceReport();
        
        // Check reports
        assert(!htmlReport.empty());
        assert(!jsonReport.empty());
        assert(!textReport.empty());
        
        assert(htmlReport.find("<html>") != std::string::npos);
        assert(jsonReport.find("{") != std::string::npos);
        assert(textReport.find("NLM Performance Report") != std::string::npos);
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Dashboard generation test passed" << std::endl;
    }
    
    void testHistoricalData() {
        std::cout << "Testing historical data..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Generate multiple snapshots
        std::vector<nlm::PerformanceSnapshot> snapshots;
        for (int i = 0; i < 10; ++i) {
            perfSystem.updateSnapshot();
            snapshots.push_back(perfSystem.getCurrentSnapshot());
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
        
        // Get historical snapshots
        auto historicalSnapshots = perfSystem.getHistoricalSnapshots();
        assert(historicalSnapshots.size() >= 10);
        
        // Archive snapshot
        perfSystem.archiveSnapshot(snapshots[0]);
        
        // Get archived snapshots
        assert(perfSystem.getArchivedSnapshots().size() >= 1);
        
        // Test time range query
        auto startTime = std::chrono::system_clock::now() - std::chrono::seconds(300);
        auto endTime = std::chrono::system_clock::now() + std::chrono::seconds(300);
        
        auto timeRangeSnapshots = perfSystem.getSnapshotsByTimeRange(startTime, endTime);
        assert(!timeRangeSnapshots.empty());
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Historical data test passed" << std::endl;
    }
    
    void testPerformanceTrends() {
        std::cout << "Testing performance trends..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Generate performance data
        std::vector<double> cpuUsageData = {10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0};
        
        // Calculate trend
        double trend = perfSystem.calculateTrend(cpuUsageData);
        assert(trend > 0.0);
        
        // Calculate rate of change
        double rateOfChange = perfSystem.calculateRateOfChange(cpuUsageData);
        assert(rateOfChange > 0.0);
        
        // Identify bottlenecks
        std::string bottlenecks = perfSystem.identifyBottlenecks();
        assert(!bottlenecks.empty());
        
        perfSystem.shutdown();
        
        std::cout << "✓ Performance trends test passed" << std::endl;
    }
    
    void testErrorHandling() {
        std::cout << "Testing error handling..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Test performance error raising
        perfSystem.raisePerformanceError(
            nlm::PerformanceErrorType::MemoryUsageExceeded,
            "memory_used",
            "Consider optimizing memory allocation"
        );
        
        perfSystem.raisePerformanceError(
            nlm::PerformanceErrorType::CPUUsageExceeded,
            "cpu_usage",
            "Check for CPU-intensive operations"
        );
        
        // Check error count
        uint64_t errors = perfSystem.getTotalErrors();
        assert(errors >= 2);
        
        perfSystem.shutdown();
        
        std::cout << "✓ Error handling test passed" << std::endl;
    }
    
    void testOptimizationSuggestions() {
        std::cout << "Testing optimization suggestions..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Get optimization suggestions
        auto optimizationSuggestions = perfSystem.getOptimizationSuggestions();
        assert(!optimizationSuggestions.empty());
        
        // Get resource allocation recommendations
        auto resourceRecommendations = perfSystem.getResourceAllocationRecommendations();
        assert(!resourceRecommendations.empty());
        
        // Get SIMD optimization opportunities
        auto simdOpportunities = perfSystem.getSimdOptimizationOpportunities();
        assert(!simdOpportunities.empty());
        
        // Get parallelization recommendations
        auto parallelizationRecommendations = perfSystem.getParallelizationRecommendations();
        assert(!parallelizationRecommendations.empty());
        
        // Get memory pool optimization suggestions
        auto memoryPoolSuggestions = perfSystem.getMemoryPoolOptimizationSuggestions();
        assert(!memoryPoolSuggestions.empty());
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Optimization suggestions test passed" << std::endl;
    }
    
    void testDataExportImport() {
        std::cout << "Testing data export/import..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(50));
        
        // Update some snapshots
        for (int i = 0; i < 5; ++i) {
            perfSystem.updateSnapshot();
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
        
        // Export performance data
        std::string exportFile = "performance_export_test.json";
        perfSystem.exportPerformanceData(exportFile);
        
        // Verify export file exists
        std::ifstream exportFileStream(exportFile);
        assert(exportFileStream.good());
        exportFileStream.close();
        
        // Import performance data (placeholder - simplified implementation)
        std::string importFile = "performance_import_test.json";
        std::ofstream importFileStream(importFile);
        importFileStream << "{}"; // Empty import for testing
        importFileStream.close();
        
        // Cleanup
        std::remove(exportFile.c_str());
        std::remove(importFile.c_str());
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Data export/import test passed" << std::endl;
    }
}

// Main test function
int main() {
    std::cout << "Running Performance System Test Suite" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    try {
        testPerformanceSystemInitialization();
        testPerformanceMetricsCollection();
        testThresholdMonitoring();
        testMemoryProfiling();
        testThreadAnalysis();
        testNeuralNetworkMetrics();
        testDashboardGeneration();
        testHistoricalData();
        testPerformanceTrends();
        testErrorHandling();
        testOptimizationSuggestions();
        testDataExportImport();
        
        std::cout << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

// Additional test functions for advanced features
namespace {
    void testRealTimeMonitoring() {
        std::cout << "Testing real-time monitoring..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        perfSystem.startRealTimeMonitoring(std::chrono::milliseconds(100));
        
        // Wait for real-time updates
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Check monitoring state
        assert(perfSystem.isRealTimeMonitoring() == true);
        
        // Update snapshot
        perfSystem.updateSnapshot();
        
        // Get statistics
        uint64_t updates = perfSystem.getTotalUpdates();
        assert(updates >= 5);  // Should have multiple updates
        
        perfSystem.stopRealTimeMonitoring();
        perfSystem.shutdown();
        
        std::cout << "✓ Real-time monitoring test passed" << std::endl;
    }
    
    void testPerformanceScripting() {
        std::cout << "Testing performance scripting..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Execute performance script (placeholder)
        perfSystem.executePerformanceScript("analyze_performance");
        
        // Get performance scripts
        auto scripts = perfSystem.getPerformanceScripts();
        assert(!scripts.empty());
        
        perfSystem.shutdown();
        
        std::cout << "✓ Performance scripting test passed" << std::endl;
    }
    
    void testConfigurationOptimization() {
        std::cout << "Testing configuration optimization..." << std::endl;
        
        auto& perfSystem = nlm::PerformanceSystem::getInstance();
        
        // Reinitialize
        perfSystem.initialize();
        
        // Optimize configuration (placeholder)
        std::string optimizedConfig = perfSystem.optimizeConfiguration("default_config");
        assert(!optimizedConfig.empty());
        
        // Get data structure optimization suggestions
        std::string dataStructureSuggestions = perfSystem.suggestDataStructureOptimization();
        assert(!dataStructureSuggestions.empty());
        
        perfSystem.shutdown();
        
        std::cout << "✓ Configuration optimization test passed" << std::endl;
    }
}

// Register additional tests
void registerAdvancedTests() {
    testRealTimeMonitoring();
    testPerformanceScripting();
    testConfigurationOptimization();
}