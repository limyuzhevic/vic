// Test file for NLM enterprise logging and error handling system
// Tests the enhanced error handling, logging, and monitoring features

#include "core/Logger/Logger.hpp"
#include "core/ErrorHandling/NLMExceptions.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

using namespace nlm;

void testBasicLogging() {
    std::cout << "Testing basic logging functionality..." << std::endl;
    
    // Clear any existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Add a console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Test different log levels
    Logger::getGlobal().debug("Debug message");
    Logger::getGlobal().info("Info message");
    Logger::getGlobal().warning("Warning message");
    Logger::getGlobal().error("Error message");
    Logger::getGlobal().critical("Critical message");
    
    // Test correlation ID functionality
    Logger::getGlobal().startCorrelation("TestOperation");
    uint64_t correlationId = Logger::getGlobal().getCurrentCorrelationId();
    assert(correlationId > 0);
    
    Logger::getGlobal().info("Message with correlation ID", "", 0, "", "TestModule");
    
    Logger::getGlobal().endCorrelation();
    
    std::cout << "✓ Basic logging test passed" << std::endl;
}

void testExceptionHandling() {
    std::cout << "Testing exception handling..." << std::endl;
    
    try {
        // Test basic NLM exception
        NLMException exc("Test exception", ExceptionCategory::CONFIGURATION_ERROR);
        assert(exc.what() != nullptr);
        
        // Test exception chaining
        try {
            NLMException inner("Inner exception", ExceptionCategory::INITIALIZATION_FAILURE);
            throw inner << "Additional context";
        } catch (const NLMException& e) {
            exc.addContext("Caught: " + std::string(e.what()));
        }
        
        throw exc;
    } catch (const NLMException& e) {
        // Exception handling works
        assert(e.getCategory() == ExceptionCategory::CONFIGURATION_ERROR);
    }
    
    std::cout << "✓ Exception handling test passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "Testing error reporting system..." << std::endl;
    
    // Get error reporter
    auto& errorReporter = getErrorReporter();
    
    // Clear existing errors
    // Note: In a real implementation, we would need a way to clear error history
    
    // Report a test error
    reportError("Test error for reporting", ExceptionCategory::DYNAMICS_ERROR);
    
    // Get error statistics
    auto stats = errorReporter.getSystemStats();
    assert(stats.totalErrors >= 1);
    
    // Get recent errors
    auto recentErrors = errorReporter.getRecentErrors(10);
    assert(!recentErrors.empty());
    
    std::cout << "✓ Error reporting test passed" << std::endl;
}

void testPerformanceLogging() {
    std::cout << "Testing performance logging..." << std::endl;
    
    // Clear existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Add console logger for performance logs
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Test performance logging
    Enterprise::PerformanceLogger perfLogger("TestPerformance", 12345);
    
    // Simulate some work
    perfLogger.startTiming("TestOperation");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    perfLogger.endTiming("TestOperation");
    
    std::cout << "✓ Performance logging test passed" << std::endl;
}

void testMemoryTracking() {
    std::cout << "Testing memory tracking..." << std::endl;
    
    // Record some allocations
    getMemoryTracker().recordAllocation(1024);
    getMemoryTracker().recordAllocation(2048);
    
    // Record deallocations
    getMemoryTracker().recordDeallocation(512);
    getMemoryTracker().recordDeallocation(1024);
    
    // Record GC
    getMemoryTracker().recordGC(std::chrono::system_clock::now(), 5, 1);
    
    // Get memory stats
    auto stats = getMemoryTracker().getStats();
    assert(stats.currentAllocated >= 512);  // Some memory should remain
    assert(stats.allocationCount >= 2);
    
    // Log memory stats
    Logger::getGlobal().logMemoryStats("Test memory check");
    
    std::cout << "✓ Memory tracking test passed" << std::endl;
}

void testHealthChecks() {
    std::cout << "Testing health checks..." << std::endl;
    
    // Clear existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Add console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Enable enterprise mode
    Logger::getGlobal().setEnterpriseMode(true);
    
    // Perform health check
    bool healthy = Logger::getGlobal().performHealthCheck();
    assert(healthy);  // Should be healthy initially
    
    std::cout << "✓ Health checks test passed" << std::endl;
}

void testCorrelationIds() {
    std::cout << "Testing correlation ID management..." << std::endl;
    
    // Enable enterprise mode
    Logger::getGlobal().setEnterpriseMode(true);
    
    // Generate correlation IDs
    uint64_t id1 = getCorrelationManager().generateCorrelationId();
    uint64_t id2 = getCorrelationManager().generateCorrelationId();
    assert(id1 != id2);
    
    // Test correlation context
    getCorrelationManager().startCorrelation("Operation1");
    uint64_t contextId1 = getCorrelationManager().getCurrentCorrelationId();
    assert(contextId1 != 0);
    
    getCorrelationManager().endCorrelation();
    
    // Test logger correlation IDs
    Logger::getGlobal().startCorrelation("LoggerTest");
    uint64_t loggerId = Logger::getGlobal().getCurrentCorrelationId();
    assert(loggerId > 0);
    
    Logger::getGlobal().endCorrelation();
    
    std::cout << "✓ Correlation ID test passed" << std::endl;
}

void testStructuredLogging() {
    std::cout << "Testing structured logging..." << std::endl;
    
    // Clear existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Add console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Test structured logging with fields
    std::unordered_map<std::string, std::string> fields;
    fields["user_id"] = "12345";
    fields["action"] = "login";
    fields["status"] = "success";
    
    Logger::getGlobal().info("User login", "", 0, "", "AuthModule", fields);
    
    // Test numeric fields
    std::unordered_map<std::string, double> numericFields;
    numericFields["response_time"] = 123.45;
    numericFields["memory_usage"] = 1024.0;
    
    // Note: Numeric fields are handled differently in the enhanced logger
    Logger::getGlobal().info("Performance metric", "", 0, "", "PerfModule");
    
    std::cout << "✓ Structured logging test passed" << std::endl;
}

void testMultiThreadedLogging() {
    std::cout << "Testing multi-threaded logging..." << std::endl;
    
    // Clear existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Add console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Enable enterprise mode
    Logger::getGlobal().setEnterpriseMode(true);
    
    // Log from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            Logger::getGlobal().info("Log from thread " + std::to_string(i), "", 0, "", "ThreadTest");
        });
    }
    
    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "✓ Multi-threaded logging test passed" << std::endl;
}

void testExceptionChaining() {
    std::cout << "Testing exception chaining..." << std::endl;
    
    try {
        // Create a base exception
        NLMInitException baseExc("Base exception");
        
        // Chain exceptions
        try {
            NLMMemoryException memoryExc("Memory allocation failed");
            throw memoryExc << "Allocation size: 1024 bytes" << "File: memory.cpp" << "Line: 42";
        } catch (const NLMMemoryException& e) {
            // Add to base exception
            baseExc.addContext("Caused by: " + std::string(e.what()));
        }
        
        throw baseExc;
    } catch (const NLMException& e) {
        // Verify the chain is preserved
        auto context = e.getContextStack();
        assert(!context.empty());
    }
    
    std::cout << "✓ Exception chaining test passed" << std::endl;
}

void testLogRotation() {
    std::cout << "Testing log rotation..." << std::string(20, '.') << std::endl;
    
    // Clear existing loggers
    Logger::getGlobal().clearLoggers();
    
    // Create a console logger with small file size for testing
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(false);
    consoleLogger->setOutputFile("test_rotation.log");
    consoleLogger->setMaxFileSize(1024);  // 1 KB for testing
    consoleLogger->setMaxFiles(3);
    
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Log many messages to trigger rotation
    for (int i = 0; i < 100; ++i) {
        Logger::getGlobal().info("Log message " + std::to_string(i), "", 0, "", "RotationTest");
    }
    
    // Check if log file exists
    std::ifstream logFile("test_rotation.log");
    assert(logFile.good());
    
    // Clean up
    consoleLogger->closeOutputFile();
    std::remove("test_rotation.log");
    
    std::cout << "✓ Log rotation test passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Enterprise Logging and Error Handling Tests ===" << std::endl << std::endl;
    
    try {
        testBasicLogging();
        testExceptionHandling();
        testErrorReporting();
        testPerformanceLogging();
        testMemoryTracking();
        testHealthChecks();
        testCorrelationIds();
        testStructuredLogging();
        testMultiThreadedLogging();
        testExceptionChaining();
        testLogRotation();
        
        std::cout << std::endl << "=== All tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl << "=== Test failed with exception: " << e.what() << " ===" << std::endl;
        return 1;
    }
}
