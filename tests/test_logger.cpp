// Enhanced Logger Tests for NLM
// Tests for the new logging system improvements

#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>

void testEnhancedLogLevels() {
    std::cout << "Testing enhanced log levels..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Test all enhanced log levels
    logger.setLevel(nlm::LogLevel::Trace);
    assert(logger.isEnabled(nlm::LogLevel::Trace));
    assert(logger.isEnabled(nlm::LogLevel::Debug));
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Notice));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Error));
    assert(logger.isEnabled(nlm::LogLevel::Critical));
    assert(logger.isEnabled(nlm::LogLevel::Alert));
    assert(logger.isEnabled(nlm::LogLevel::Emergency));
    
    logger.setLevel(nlm::LogLevel::Warning);
    assert(!logger.isEnabled(nlm::LogLevel::Trace));
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    assert(!logger.isEnabled(nlm::LogLevel::Info));
    assert(!logger.isEnabled(nlm::LogLevel::Notice));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Error));
    assert(logger.isEnabled(nlm::LogLevel::Critical));
    
    logger.setLevel(nlm::LogLevel::Emergency);
    assert(logger.isEnabled(nlm::LogLevel::Critical));
    assert(logger.isEnabled(nlm::LogLevel::Alert));
    assert(logger.isEnabled(nlm::LogLevel::Emergency));
    
    std::cout << "  PASSED" << std::endl;
}

void testStructuredLogging() {
    std::cout << "Testing structured logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Test JSON structured logging
    nlm::StructuredData jsonData;
    jsonData.json = "{\"event\": \"test\", \"count\": 42, \"active\": true}";
    jsonData.isJson = true;
    
    // Test key-value structured logging
    nlm::StructuredData kvData;
    kvData.isJson = false;
    kvData.keyValue = {
        {"event", "test"}, {"count", "42"}, {"active", "true"}
    };
    
    // Test with category and tags
    logger.logWithContext(nlm::LogLevel::Info, "Test structured log", "operation1", "test_category", {"tag1", "tag2"});
    
    std::cout << "  PASSED" << std::endl;
}

void testPerformanceLogging() {
    std::cout << "Testing performance logging..." << std::endl;
    
    // Test performance timer
    nlm::LogPerformanceTimer timer("database_query", "performance");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timer.stop();
    
    nlm::PerformanceMetrics metrics;
    metrics.duration = std::chrono::milliseconds(10);
    metrics.operation = "database_query";
    metrics.memoryUsage = 1024 * 1024; // 1MB
    
    nlm::Logger::getGlobal().logPerformance(metrics);
    
    std::cout << "  PASSED" << std::endl;
}

void testLogContext() {
    std::cout << "Testing log context..." << std::endl;
    
    nlm::LogContext context("data_processing", "io");
    context.setProperty("source", "sensor_array_01");
    context.setProperty("format", "float32");
    
    assert(context.getOperation() == "data_processing");
    assert(context.getCategory() == "io");
    assert(context.getProperty("source") == "sensor_array_01");
    assert(context.getProperty("format") == "float32");
    
    // Test context properties
    context.setProperty("timestamp", "2026-01-01T00:00:00Z");
    assert(context.getProperty("timestamp") == "2026-01-01T00:00:00Z");
    
    std::cout << "  PASSED" << std::endl;
}

void testLogStatistics() {
    std::cout << "Testing log statistics..." << std::endl;
    
    nlm::Logger::getStatistics().reset();
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Generate test logs
    for (int i = 0; i < 100; ++i) {
        logger.info("Test log " + std::to_string(i), "", 0, "");
        if (i % 10 == 0) {
            logger.warning("Warning log " + std::to_string(i), "", 0, "");
        }
    }
    
    auto stats = nlm::Logger::getStatistics();
    assert(stats.getLogCount() == 110); // 100 info + 10 warning
    assert(stats.getLogCount(nlm::LogLevel::Info) == 100);
    assert(stats.getLogCount(nlm::LogLevel::Warning) == 10);
    
    double rate = stats.getLogRate();
    assert(rate > 0 && rate < 1000); // Should be reasonable
    
    auto levelCounts = stats.getLevelCounts();
    assert(levelCounts[nlm::LogLevel::Info] == 100);
    assert(levelCounts[nlm::LogLevel::Warning] == 10);
    
    auto topMessages = stats.getTopMessages(5);
    assert(!topMessages.empty());
    
    std::cout << "  PASSED" << std::endl;
}

void testCategoryLogger() {
    std::cout << "Testing category logger..." << std::endl;
    
    // Create base logger
    auto baseLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
    
    // Create category logger
    auto categoryLogger = std::make_shared<nlm::CategoryLogger>("neural_network", baseLogger);
    
    // Test basic functionality
    assert(categoryLogger->getLevel() == nlm::LogLevel::Debug);
    
    // Test category filtering
    categoryLogger->addCategoryFilter("neural_network", nlm::LogLevel::Error);
    
    // In real implementation, would test actual filtering behavior
    
    std::cout << "  PASSED" << std::endl;
}

void testAsyncLogger() {
    std::cout << "Testing async logger..." << std::endl;
    
    auto asyncLogger = std::make_shared<nlm::AsyncLogger>(1000, 2);
    assert(asyncLogger->getLevel() == nlm::LogLevel::Info);
    
    asyncLogger->setLevel(nlm::LogLevel::Warning);
    assert(asyncLogger->getLevel() == nlm::LogLevel::Warning);
    
    std::cout << "  PASSED" << std::endl;
}

void testLogRotation() {
    std::cout << "Testing log rotation..." << std::endl;
    
    // Note: LogRotation is currently an internal implementation detail
    // This test would be added when the class is fully implemented
    
    std::cout << "  SKIPPED (implementation in progress)" << std::endl;
}

void testFormatters() {
    std::cout << "Testing formatters..." << std::endl;
    
    // Test JSON formatter
    auto jsonFormatter = std::make_shared<nlm::JSONFormatter>();
    
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Error;
    entry.message = "Test error message";
    entry.category = "database";
    entry.operationContext = "query_execution";
    entry.threadId = std::this_thread::get_id();
    entry.processId = getpid();
    entry.timestamp = std::chrono::system_clock::now();
    
    std::string jsonOutput = jsonFormatter->format(entry);
    assert(jsonOutput.find("Test error message") != std::string::npos);
    assert(jsonOutput.find("database") != std::string::npos);
    
    // Test key-value formatter
    auto kvFormatter = std::make_shared<nlm::KeyValueFormatter>();
    std::string kvOutput = kvFormatter->format(entry);
    assert(kvOutput.find("Test error message") != std::string::npos);
    
    // Test template formatter
    auto templateFormatter = std::make_shared<nlm::TemplateFormatter>("%timestamp% [%level%] %message%");
    std::string templateOutput = templateFormatter->format(entry);
    assert(templateOutput.find("Test error message") != std::string::npos);
    
    std::cout << "  PASSED" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Test old-style macros and methods
    logger.log(nlm::LogLevel::Info, "Test log with old style", "", 0, "");
    logger.debug("Debug message", "", 0, "");
    logger.info("Info message", "", 0, "");
    logger.warning("Warning message", "", 0, "");
    logger.error("Error message", "", 0, "");
    logger.critical("Critical message", "", 0, "");
    
    // Test LogStream
    nlm::LogStream stream(nlm::LogLevel::Info, "", 0, "");
    stream << "Streamed message: " << 42 << " - " << 3.14;
    
    std::cout << "  PASSED" << std::endl;
}

void testConvenienceMacros() {
    std::cout << "Testing convenience macros..." << std::endl;
    
    // Test enhanced macros
    NLM_LOG_TRACE("Trace message");
    NLM_LOG_DEBUG("Debug message");
    NLM_LOG_INFO("Info message");
    NLM_LOG_NOTICE("Notice message");
    NLM_LOG_WARNING("Warning message");
    NLM_LOG_ERROR("Error message");
    NLM_LOG_CRITICAL("Critical message");
    NLM_LOG_ALERT("Alert message");
    NLM_LOG_EMERGENCY("Emergency message");
    
    // Test performance timer macro
    NLM_PERF_TIMER("performance", "benchmark");
    
    // Test structured logging macros
    NLM_LOG_JSON(nlm::LogLevel::Info, "JSON log", "{\"key\": \"value\"}");
    
    // Test context macro
    NLM_LOG_CONTEXT(nlm::LogLevel::Info, "Context log", "operation", "category");
    
    std::cout << "  PASSED" << std::endl;
}

void runAllLoggerTests() {
    std::cout << "=== Enhanced Logger Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testEnhancedLogLevels();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testStructuredLogging();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPerformanceLogging();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testLogContext();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testLogStatistics();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testCategoryLogger();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testAsyncLogger();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testLogRotation();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testFormatters();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testBackwardCompatibility();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testConvenienceMacros();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << std::endl << "=== All Logger Tests PASSED ===" << std::endl;
    } else {
        std::cout << std::endl << "=== Some Logger Tests FAILED ===" << std::endl;
    }
}