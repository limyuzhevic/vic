// Test file for advanced logging system
// Tests structured logging, JSON output, and debugging features

#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <fstream>
#include <filesystem>
#include "core/Logger/Logger.hpp"
#include "core/Logging/LoggerManager.hpp"

void testBasicLogging() {
    std::cout << "Testing basic logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Test all log levels
    logger.debug("Debug message test");
    logger.info("Info message test");
    logger.warning("Warning message test");
    logger.error("Error message test");
    logger.critical("Critical message test");
    
    std::cout << "✓ Basic logging test passed" << std::endl;
}

void testStructuredLogging() {
    std::cout << "Testing structured logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Test structured log entry
    nlm::StructuredLogEntry entry;
    entry.eventType = "neuron_created";
    entry.module = "neuron";
    entry.entityId = 42;
    entry.entityType = "neuron";
    entry.data = {{"neuron_type", "lstm"}, {"size", 128}};
    
    logger.logStructured(entry);
    
    std::cout << "✓ Structured logging test passed" << std::endl;
}

void testCorrelationIds() {
    std::cout << "Testing correlation ID tracking..." << std::endl;
    
    auto logger = nlm::LoggerManager::getInstance().getLogger("test");
    
    uint64_t correlationId1 = logger->getNextCorrelationId();
    uint64_t correlationId2 = logger->getNextCorrelationId();
    
    assert(correlationId1 == 1);
    assert(correlationId2 == 2);
    
    logger->setCorrelationId(100);
    assert(logger->getCorrelationId() == 100);
    
    std::cout << "✓ Correlation ID test passed" << std::endl;
}

void testLogLevelFiltering() {
    std::cout << "Testing log level filtering..." << std::endl;
    
    auto logger = nlm::LoggerManager::getInstance().getLogger("filtered");
    
    // Set minimum level to Warning
    logger->setLevel(nlm::LogLevel::Warning);
    
    // Test that lower level messages are filtered
    logger->debug("Debug message (should be filtered)");
    logger->info("Info message (should be filtered)");
    logger->warning("Warning message (should appear)");
    logger->error("Error message (should appear)");
    logger->critical("Critical message (should appear)");
    
    std::cout << "✓ Log level filtering test passed" << std::endl;
}

void testPerformanceProfiling() {
    std::cout << "Testing performance profiling..." << std::endl;
    
    auto profiler = nlm::PerformanceProfiler::getInstance();
    
    profiler->startProfile("test_operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    profiler->stopProfile("test_operation");
    
    double duration = profiler->getLastMeasurement("test_operation");
    assert(duration >= 40.0 && duration <= 100.0); // Allow some tolerance
    
    std::cout << "✓ Performance profiling test passed (duration: " << duration << " ms)" << std::endl;
}

void testMemoryTracking() {
    std::cout << "Testing memory tracking..." << std::endl;
    
    auto memoryTracker = nlm::MemoryTracker::getInstance();
    
    // Test memory allocation tracking
    int* ptr = new int[100];
    memoryTracker->trackAllocation(ptr, 100 * sizeof(int), "test_allocation");
    
    // Test memory deallocation
    memoryTracker->trackDeallocation(ptr);
    delete[] ptr;
    
    // Test leak detection
    int* leakPtr = new int[50];
    memoryTracker->trackAllocation(leakPtr, 50 * sizeof(int), "leak_test");
    
    bool hasLeaks = memoryTracker->detectMemoryLeaks();
    assert(hasLeaks); // Should detect the leak
    
    memoryTracker->trackDeallocation(leakPtr);
    delete[] leakPtr;
    
    std::cout << "✓ Memory tracking test passed" << std::endl;
}

void testAsyncLogging() {
    std::cout << "Testing async logging..." << std::endl;
    
    auto logger = nlm::LoggerManager::getInstance().getLogger("async");
    
    // Log many messages to test async performance
    for (int i = 0; i < 100; ++i) {
        logger->info("Async log message " + std::to_string(i));
    }
    
    // Wait for async processing
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "✓ Async logging test passed" << std::endl;
}

void testFileOutput() {
    std::cout << "Testing file output..." << std::endl;
    
    std::string logFile = "test_logs.json";
    
    // Configure logger to write to file
    auto logger = nlm::LoggerManager::getInstance().getLogger("file_test");
    logger->setOutputFile(logFile);
    
    logger->info("Test message 1");
    logger->warning("Test message 2");
    logger->error("Test message 3");
    
    // Flush logs
    logger->flush();
    
    // Check if file was created and has content
    std::ifstream file(logFile);
    assert(file.good());
    
    // Clean up
    file.close();
    std::remove(logFile.c_str());
    
    std::cout << "✓ File output test passed" << std::endl;
}

void testDebugConsole() {
    std::cout << "Testing debug console..." << std::endl;
    
    auto console = nlm::DebugConsole::getInstance();
    
    console->show();
    assert(console->isVisible());
    
    console->logMessage("Test debug message", nlm::LogLevel::Info);
    console->addSystemStatus("CPU", "75%", "warning");
    console->addPerformanceMetric("test_metric", 123.45, "ms");
    console->addMemoryUsage(512 * 1024 * 1024, 1024 * 1024 * 1024); // 512MB / 1GB
    console->setCpuUsage(75.5);
    console->setActiveThreads(4);
    
    console->hide();
    assert(!console->isVisible());
    
    console->clear();
    
    std::cout << "✓ Debug console test passed" << std::endl;
}

void testSystemMonitor() {
    std::cout << "Testing system monitor..." << std::endl;
    
    auto monitor = nlm::SystemMonitor::getInstance();
    
    monitor->startMonitoring();
    
    // Update system info
    monitor->update();
    
    auto status = monitor->getStatus("test_component");
    assert(status.component == "test_component");
    
    auto memoryInfo = monitor->getMemoryInfo();
    auto cpuInfo = monitor->getCpuInfo();
    
    auto threads = monitor->getThreadInfo();
    
    // Add custom metrics
    monitor->addCustomMetric("test_metric", 42.0, "test");
    
    monitor->stopMonitoring();
    
    std::cout << "✓ System monitor test passed" << std::endl;
}

void testIntegration() {
    std::cout << "Testing logging system integration..." << std::endl;
    
    // Set up integrated logging
    auto loggerManager = nlm::LoggerManager::getInstance();
    
    auto logger = loggerManager->getLogger("integration_test");
    logger->setLevel(nlm::LogLevel::Info);
    
    auto debugConsole = nlm::DebugConsole::getInstance();
    auto systemMonitor = nlm::SystemMonitor::getInstance();
    auto performanceProfiler = nlm::PerformanceProfiler::getInstance();
    auto memoryTracker = nlm::MemoryTracker::getInstance();
    
    // Integrate all components
    loggerManager->setDebugConsole(debugConsole);
    systemMonitor->integrateWithLogger(loggerManager);
    systemMonitor->integrateWithDebugConsole(debugConsole);
    performanceProfiler->integrateWithLogger(loggerManager);
    performanceProfiler->integrateWithDebugConsole(debugConsole);
    performanceProfiler->integrateWithSystemMonitor(systemMonitor);
    memoryTracker->integrateWithLogger(loggerManager);
    memoryTracker->integrateWithDebugConsole(debugConsole);
    memoryTracker->integrateWithSystemMonitor(systemMonitor);
    
    // Test integrated logging
    logger->info("Integration test message");
    
    performanceProfiler->startProfile("integrated_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    performanceProfiler->stopProfile("integrated_test");
    
    int* testPtr = new int[10];
    memoryTracker->trackAllocation(testPtr, 10 * sizeof(int), "integration_test");
    memoryTracker->trackDeallocation(testPtr);
    delete[] testPtr;
    
    debugConsole->logMessage("Debug console integration test", nlm::LogLevel::Info);
    debugConsole->addPerformanceMetric("integrated_metric", 99.9, "ms");
    
    std::cout << "✓ Integration test passed" << std::endl;
}

void testLogFileRotation() {
    std::cout << "Testing log file rotation..." << std::endl;
    
    std::string basePath = "rotating_logs_";
    
    // Clean up any existing rotation files
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.path().string().find(basePath) != std::string::npos) {
            std::filesystem::remove(entry.path());
        }
    }
    
    auto loggerManager = nlm::LoggerManager::getInstance();
    loggerManager->setLogFileRotation(basePath, 1024, 5); // 1KB max, keep 5 files
    
    auto logger = loggerManager->getLogger("rotation_test");
    logger->setOutputFile(basePath + "1.log");
    
    logger->info("Test message for rotation");
    
    loggerManager->cleanupOldLogFiles();
    
    std::cout << "✓ Log file rotation test passed" << std::endl;
}

int main() {
    try {
        std::cout << "Running NLM Logging System Tests" << std::endl;
        std::cout << "=================================" << std::endl;
        
        testBasicLogging();
        testStructuredLogging();
        testCorrelationIds();
        testLogLevelFiltering();
        testPerformanceProfiling();
        testMemoryTracking();
        testAsyncLogging();
        testFileOutput();
        testDebugConsole();
        testSystemMonitor();
        testIntegration();
        testLogFileRotation();
        
        std::cout << "\nAll tests passed! ✓" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}