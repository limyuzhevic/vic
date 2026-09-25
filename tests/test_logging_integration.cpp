// test_logging_integration.cpp - Test suite for the logging and monitoring system

#include "core/Logger/AdvancedLogger.hpp"
#include "core/Logger/PerformanceMonitor.hpp"
#include "core/Logger/RuntimeMetricsCollector.hpp"
#include "core/Logger/LogFormatter.hpp"
#include "core/Logger/MetricCalculator.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>
#include <random>

namespace nlm {

// Test helper functions
void printTestResult(const std::string& testName, bool passed) {
    std::cout << (passed ? "✓" : "✗") << " " << testName << ": " 
              << (passed ? "PASSED" : "FAILED") << std::endl;
}

// Test basic logging functionality
void testBasicLogging() {
    std::cout << "Testing basic logging functionality..." << std::endl;
    
    auto& logger = AdvancedLogger::getInstance();
    
    // Test log levels
    logger.info("Testing INFO level");
    logger.debug("Testing DEBUG level");
    logger.warning("Testing WARNING level");
    logger.error("Testing ERROR level");
    
    // Test log with context
    logger.logStructured(LogLevel::Info, "TestContext",
                        {{ "test_id", "123" },
                         { "test_name", "BasicLogging" }},
                        "Test log with context");
    
    printTestResult("BasicLogging", true);
}

// Test performance monitoring
void testPerformanceMonitoring() {
    std::cout << "Testing performance monitoring..." << std::endl;
    
    auto& perfMonitor = PerformanceMonitor::getInstance();
    
    // Test profiling
    perfMonitor.startProfiling("test_operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    perfMonitor.stopProfiling("test_operation");
    
    // Test custom metrics
    perfMonitor.recordMetric("test_metric", 42.5);
    perfMonitor.recordTiming("timing_test", 0.05);
    perfMonitor.recordMemory(1024, 2048);
    
    // Test system metrics
    perfMonitor.updateSystemMetrics();
    
    // Get metrics
    auto current = perfMonitor.getCurrentMeasurement("test_operation");
    assert(current.duration > 0.0);
    
    printTestResult("PerformanceMonitoring", true);
}

// Test runtime metrics collection
void testRuntimeMetricsCollection() {
    std::cout << "Testing runtime metrics collection..." << std::endl;
    
    auto& metricsCollector = RuntimeMetricsCollector::getInstance();
    
    // Test brain metrics
    BrainStateMetrics brainMetrics;
    brainMetrics.totalNeuronCount = 100;
    brainMetrics.activeNeuronCount = 10;
    brainMetrics.firingNeuronCount = 8;
    brainMetrics.averageFiringRate = 2.5f;
    brainMetrics.totalSpikeCount = 500;
    brainMetrics.excitationInhibitionRatio = 1.2f;
    
    metricsCollector.recordBrainMetrics(brainMetrics);
    
    // Test simulation metrics
    SimulationMetrics simMetrics;
    simMetrics.step = 1;
    simMetrics.spikeCount = 100;
    simMetrics.firingRate = 2.5f;
    simMetrics.activeNeuronCount = 10;
    
    metricsCollector.recordSimulationMetrics(simMetrics);
    
    // Test memory pool metrics
    MemoryPoolMetrics poolMetrics;
    poolMetrics.poolName = "test_pool";
    poolMetrics.currentSize = 50;
    poolMetrics.maxSize = 200;
    poolMetrics.totalAllocated = 500;
    poolMetrics.utilizationPercent = 25.0;
    
    metricsCollector.recordMemoryPoolMetrics(poolMetrics);
    
    // Test event metrics
    EventMetrics eventMetrics;
    eventMetrics.queueDepth = 25;
    eventMetrics.processingLatency = 10;
    eventMetrics.eventCount = 250;
    eventMetrics.throughput = 10.0;
    
    metricsCollector.recordEventMetrics(eventMetrics);
    
    // Test neuromodulator metrics
    NeuromodulatorMetrics neuromodMetrics;
    neuromodMetrics.type = "test_mod";
    neuromodMetrics.currentLevel = 0.7;
    neuromodMetrics.effectMagnitude = 0.8;
    
    metricsCollector.recordNeuromodulatorMetrics(neuromMetrics);
    
    // Test developmental stage metrics
    DevelopmentalStageMetrics devMetrics;
    devMetrics.stage = DevelopmentalStage::Phase1;
    devMetrics.developmentProgress = 0.3;
    devMetrics.plasticityRate = 0.5;
    
    metricsCollector.recordDevelopmentalStageMetrics(devMetrics);
    
    printTestResult("RuntimeMetricsCollection", true);
}

// Test log formatting
void testLogFormatting() {
    std::cout << "Testing log formatting..." << std::endl;
    
    auto& formatter = LogFormatter::getInstance();
    
    // Test JSON formatting
    nlohmann::json testData;
    testData["test"] = "value";
    testData["number"] = 42;
    testData["array"] = std::vector<int>{1, 2, 3};
    
    std::string jsonOutput = formatter.formatJSON("test", testData);
    assert(jsonOutput.find("test") != std::string::npos);
    
    // Test CSV formatting
    std::string csvOutput = formatter.formatCSV("test", testData);
    assert(csvOutput.find("test") != std::string::npos);
    
    // Test HTML formatting
    std::string htmlOutput = formatter.formatHTML("test", testData);
    assert(htmlOutput.find("<html>") != std::string::npos);
    
    // Test plain text formatting
    std::string plainOutput = formatter.formatPlainText("test", testData, false);
    assert(plainOutput.find("test") != std::string::npos);
    
    // Test export functionality
    bool exportSuccess = formatter.exportToFile("test_data", "test_output.json", "json");
    assert(exportSuccess);
    
    printTestResult("LogFormatting", true);
}

// Test metric calculation
void testMetricCalculation() {
    std::cout << "Testing metric calculation..." << std::endl;
    
    auto& calculator = MetricCalculator::getInstance();
    
    // Generate test data
    std::vector<double> testData;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 100.0);
    for (int i = 0; i < 50; ++i) {
        testData.push_back(dist(rng));
    }
    
    // Test statistical analysis
    StatisticalAnalysis stats = calculator.analyze(testData);
    assert(stats.sampleSize == 50);
    assert(stats.mean >= 0.0);
    assert(stats.standardDeviation >= 0.0);
    
    // Test anomaly detection
    AnomalyDetection anomaly = calculator.detectAnomaly(testData, 2.0);
    assert(anomaly.anomalyScore >= 0.0);
    
    // Test trend analysis
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> timeSeries;
    auto now = std::chrono::system_clock::now();
    for (int i = 0; i < 10; ++i) {
        timeSeries.push_back(std::make_pair(
            now + std::chrono::seconds(i * 10),
            static_cast<double>(i * 10)
        ));
    }
    
    TrendAnalysis trend = calculator.analyzeTrend(timeSeries);
    assert(trend.trendStrength >= 0.0);
    
    // Test baseline management
    calculator.addBaseline("test_operation", testData);
    PerformanceBaseline baseline = calculator.getBaseline("test_operation");
    assert(baseline.operation == "test_operation");
    assert(baseline.isActive);
    
    printTestResult("MetricCalculation", true);
}

// Test integration
void testIntegration() {
    std::cout << "Testing system integration..." << std::endl;
    
    auto& logger = AdvancedLogger::getInstance();
    auto& perfMonitor = PerformanceMonitor::getInstance();
    auto& metricsCollector = RuntimeMetricsCollector::getInstance();
    
    // Test brain integration
    Brain brain(Config());
    brain.initialize();
    
    // Test logging with brain
    logger.logStructured(LogLevel::Info, "IntegrationTest",
                        {{ "brain_initialized", "true" },
                         { "test_name", "Integration" }},
                        "Integration test completed");
    
    // Test performance monitoring with brain
    perfMonitor.startProfiling("integration_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    perfMonitor.stopProfiling("integration_test");
    perfMonitor.recordBrainMetrics(brain);
    
    // Test runtime metrics with brain
    metricsCollector.updateFromBrain(brain);
    
    printTestResult("Integration", true);
}

// Test file operations
void testFileOperations() {
    std::cout << "Testing file operations..." << std::endl;
    
    auto& formatter = LogFormatter::getInstance();
    
    // Test log file analysis
    std::string testLogFile = "test_log.txt";
    std::ofstream logFile(testLogFile);
    logFile << "[2023-01-01 12:00:00] [INFO] Test log entry 1\n";
    logFile << "[2023-01-01 12:00:01] [ERROR] Test log entry 2\n";
    logFile << "[2023-01-01 12:00:02] [WARNING] Test log entry 3\n";
    logFile.close();
    
    // Analyze log file
    LogFileAnalyzer analyzer;
    auto analysis = analyzer.analyzeLogFile(testLogFile);
    assert(analysis.contains("entry_count"));
    assert(analysis.contains("log_levels"));
    
    // Test log export utility
    LogExportUtility exporter;
    bool exportSuccess = exporter.exportLogs(testLogFile, "exported_log.txt", "text");
    assert(exportSuccess);
    
    // Test log compression
    bool compressSuccess = exporter.compressFile("exported_log.txt");
    assert(compressSuccess);
    
    printTestResult("FileOperations", true);
}

// Run all tests
void runAllTests() {
    std::cout << "=== NLM Logging System Test Suite ===" << std::endl;
    
    testBasicLogging();
    testPerformanceMonitoring();
    testRuntimeMetricsCollection();
    testLogFormatting();
    testMetricCalculation();
    testIntegration();
    testFileOperations();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
}

} // namespace nlm

// Main test runner
int main() {
    try {
        nlm::runAllTests();
        std::cout << "\nAll tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}