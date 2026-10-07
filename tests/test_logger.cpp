// Logger Tests
// Tests for enhanced logging functionality

#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace test_logger {

void testBasicLogging() {
    std::cout << "    Testing basic logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    consoleLogger->setLevel(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Test different log levels
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.critical("Critical message");
    
    // Test with file
    std::string testFile = "test_log.txt";
    consoleLogger->setOutputFile(testFile);
    
    logger.info("Test message with file output");
    
    consoleLogger->closeOutputFile();
    
    // Clean up
    std::remove(testFile.c_str());
}

void testStructuredLogging() {
    std::cout << "    Testing structured logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto jsonLogger = std::make_shared<nlm::FileLogger>();
    jsonLogger->setLevel(nlm::LogLevel::Debug);
    jsonLogger->setFormatter(std::make_shared<nlm::JSONFormatter>());
    
    // Create test directory
    std::error_code ec;
    std::filesystem::create_directories("logs", ec);
    
    logger.addLogger(jsonLogger);
    
    // Test structured log with metadata
    std::map<std::string, std::string> metadata;
    metadata["operation"] = "test_operation";
    metadata["timestamp"] = "2024-01-01T00:00:00Z";
    metadata["source"] = "test_source";
    
    logger.info("Structured log message", "test.cpp", 123, "test_function", metadata);
    
    // Test with context
    std::map<std::string, std::string> context;
    context["user_id"] = "12345";
    context["session_id"] = "session_123";
    
    logger.logWithContext(nlm::LogLevel::Info, "Context log message", context, "test.cpp", 124, "test_function");
}

void testPerformanceLogging() {
    std::cout << "    Testing performance logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto perfLogger = std::make_shared<nlm::FileLogger>();
    perfLogger->enablePerformanceTracking(true);
    perfLogger->setPerformanceLogInterval(0.1); // 100ms
    logger.addLogger(perfLogger);
    
    // Enable performance tracker
    auto& perfTracker = nlm::Logger::getPerformanceTracker();
    perfTracker.enable(true);
    
    // Record some performance metrics
    perfTracker.recordTime("operation1", 0.1);
    perfTracker.recordTime("operation2", 0.2);
    perfTracker.recordMemoryUsage(1024 * 1024); // 1MB
    perfTracker.recordMemoryUsage(2 * 1024 * 1024); // 2MB
    
    // Trigger performance log
    logger.logWithPerformance(nlm::LogLevel::Info, "Performance check", "operation1");
}

void testAsyncLogging() {
    std::cout << "    Testing async logging..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto asyncLogger = std::make_shared<nlm::AsyncLogger>(1000, 2);
    asyncLogger->start();
    logger.addLogger(asyncLogger);
    
    // Log some messages
    for (int i = 0; i < 10; ++i) {
        logger.info("Async message " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Wait for async logger to process
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    asyncLogger->stop();
}

void testFileRotation() {
    std::cout << "    Testing file rotation..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto fileLogger = std::make_shared<nlm::FileLogger>();
    fileLogger->setRotationSize(1024); // 1KB for testing
    fileLogger->setRotationCount(3);
    fileLogger->setCompressionEnabled(true);
    
    logger.addLogger(fileLogger);
    
    // Create test directory
    std::error_code ec;
    std::filesystem::create_directories("logs", ec);
    
    // Log enough messages to trigger rotation
    for (int i = 0; i < 100; ++i) {
        logger.info("Rotation test message " + std::to_string(i));
    }
}

void testLogConfiguration() {
    std::cout << "    Testing log configuration..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto& logConfig = nlm::Logger::getConfig();
    
    // Set component levels
    logConfig.setComponentLevel("neuron", nlm::LogLevel::Info);
    logConfig.setComponentLevel("synapse", nlm::LogLevel::Debug);
    logConfig.setComponentLevel("brain", nlm::LogLevel::Warning);
    
    // Test configuration getters
    assert(logConfig.getComponentLevel("neuron") == nlm::LogLevel::Info);
    assert(logConfig.getComponentLevel("synapse") == nlm::LogLevel::Debug);
    assert(logConfig.getComponentLevel("brain") == nlm::LogLevel::Warning);
    
    // Test default level for unknown component
    assert(logConfig.getComponentLevel("unknown") == nlm::LogLevel::Info);
    
    // Test configuration features
    logConfig.enableFiltering(true);
    assert(logConfig.isFilteringEnabled() == true);
    
    logConfig.enableMasking(true);
    assert(logConfig.isMaskingEnabled() == true);
    
    std::vector<std::string> maskedFields = {"password", "secret", "token"};
    logConfig.setMaskedFields(maskedFields);
    
    logConfig.enableAsync(true);
    assert(logConfig.isAsyncEnabled() == true);
    
    logConfig.enableEscalation(true);
    assert(logConfig.isEscalationEnabled() == true);
    
    logConfig.setEscalationThreshold(nlm::LogLevel::Error);
    assert(logConfig.getEscalationThreshold() == nlm::LogLevel::Error);
    
    logConfig.reset();
    assert(logConfig.isFilteringEnabled() == false);
}

void testFormatter() {
    std::cout << "    Testing log formatters..." << std::endl;
    
    // Test JSON formatter
    auto jsonFormatter = std::make_shared<nlm::JSONFormatter>();
    
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Test JSON log";
    entry.file = "test.cpp";
    entry.line = 123;
    entry.function = "test_function";
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = "123";
    entry.processId = "456";
    entry.elapsedTime = 0.1;
    entry.memoryUsage = 1024 * 1024;
    
    std::string jsonOutput = jsonFormatter->format(entry);
    assert(jsonOutput.find("Test JSON log") != std::string::npos);
    
    // Test console formatter
    auto consoleFormatter = std::make_shared<nlm::ConsoleFormatter>(true, nlm::ConsoleFormatter::Intensity::Bright);
    std::string consoleOutput = consoleFormatter->format(entry);
    assert(consoleOutput.find("Test JSON log") != std::string::npos);
    
    // Test structured output
    std::string structuredOutput = jsonFormatter->formatStructured(entry);
    assert(structuredOutput.find("Test JSON log") != std::string::npos);
}

void testBackwardCompatibility() {
    std::cout << "    Testing backward compatibility..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    // Test original ConsoleLogger API (no new parameters)
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    consoleLogger->setLevel(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Test original logging methods
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.critical("Critical message");
    
    // Test original macros (if they exist)
    // NLM_LOG_DEBUG("Macro test");
    // NLM_LOG_INFO("Macro test");
}

void testLogStream() {
    std::cout << "    Testing LogStream..." << std::endl;
    
    nlm::Logger& logger = nlm::Logger::getGlobal();
    logger.clearLoggers();
    
    auto streamLogger = std::make_shared<nlm::ConsoleLogger>();
    streamLogger->setLevel(nlm::LogLevel::Debug);
    logger.addLogger(streamLogger);
    
    // Test streaming logging
    nlm::LogStream stream(nlm::LogLevel::Info, "test.cpp", 123, "test_function");
    stream << "Streamed log message with number " << 42 << " and string " << "test";
    
    // Test with metadata
    nlm::LogStream streamWithMetadata(nlm::LogLevel::Info, "test.cpp", 124, "test_function");
    streamWithMetadata.withMetadata("key1", "value1");
    streamWithMetadata.withMetadata("key2", "value2");
    streamWithMetadata << "Message with metadata";
}

void runAll() {
    testBasicLogging();
    testStructuredLogging();
    testPerformanceLogging();
    testAsyncLogging();
    testFileRotation();
    testLogConfiguration();
    testFormatter();
    testBackwardCompatibility();
    testLogStream();
    
    std::cout << "    All Logger tests passed!" << std::endl;
}

} // namespace test_logger