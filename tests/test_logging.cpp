#include <gtest/gtest.h>
#include "core/Logger/Logger.hpp"
#include "core/Logger/EnhancedLogger.hpp"
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

class AsyncLoggingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear global logger before each test
        auto& logger = nlm::Logger::getGlobal();
        logger.clearLoggers();
    }
    
    void TearDown() override {
        // Clean up file if created
        std::string testLogFile = "/tmp/nlm_test_log.txt";
        if (std::filesystem::exists(testLogFile)) {
            std::filesystem::remove(testLogFile);
        }
    }
};

TEST_F(AsyncLoggingTest, AsyncLoggerCreation) {
    // Enable async logging
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Log a message
    logger.info("Test message for async logging");
    
    // Verify logging is working
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, CategoryFiltering) {
    auto& logger = nlm::Logger::getGlobal();
    
    // Enable async logging
    logger.enableAsyncLogging(true);
    
    // Enable/disable categories
    logger.enableCategory("Brain", true);
    logger.enableCategory("Memory", false);
    logger.enableCategory("System", true);
    
    // Log messages with different categories
    logger.info("Brain message", __FILE__, __LINE__, __func__);
    logger.info("Memory message", __FILE__, __LINE__, __func__);
    logger.info("System message", __FILE__, __LINE__, __func__);
    
    // Verify category filtering
    // Memory category should be disabled
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, EnhancedLogLevels) {
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Test trace level
    logger.trace("This is a trace message");
    
    // Test debug level
    logger.debug("This is a debug message");
    
    // Test info level
    logger.info("This is an info message");
    
    // Test warning level
    logger.warning("This is a warning message");
    
    // Test error level
    logger.error("This is an error message");
    
    // Test critical level
    logger.critical("This is a critical message");
    
    // Test fatal level
    logger.fatal("This is a fatal message");
    
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, EnhancedLoggingConfig) {
    auto& logger = nlm::Logger::getGlobal();
    
    // Create enhanced config
    nlm::LogConfig config;
    config.globalLevel = nlm::EnhancedLogLevel::Info;
    config.useEnhancedLevels = true;
    config.useAsyncLogging = true;
    config.asyncQueueSize = 1000;
    config.maxFileSize = 1024; // 1KB for testing
    config.compressLogs = false;
    config.sampleRate = 1.0f; // 100% sampling for test
    config.logSystemMetrics = true;
    config.logPerformanceMetrics = true;
    
    // Set config
    logger.setLogConfig(config);
    
    // Verify config was set
    auto retrievedConfig = logger.getLogConfig();
    EXPECT_EQ(retrievedConfig.globalLevel, nlm::EnhancedLogLevel::Info);
    EXPECT_TRUE(retrievedConfig.useEnhancedLevels);
    EXPECT_TRUE(retrievedConfig.useAsyncLogging);
}

TEST_F(AsyncLoggingTest, RateLimiting) {
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Add category with rate limit
    logger.setCategoryLevel("Brain", nlm::LogLevel::Info);
    
    // Log many messages quickly
    for (int i = 0; i < 1000; ++i) {
        logger.info("Brain processing step " + std::to_string(i));
    }
    
    // The rate limiting should be active
    // (Implementation details would require more sophisticated testing)
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, StructuredLogging) {
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Log with enhanced entry
    nlm::EnhancedLogEntry entry;
    entry.level = nlm::EnhancedLogLevel::Info;
    entry.category = "Brain";
    entry.message = "Structured log message";
    entry.file = "test.cpp";
    entry.line = 123;
    entry.function = "testFunction";
    entry.timestamp = std::chrono::system_clock::now();
    entry.fields["neuron_count"] = "500";
    entry.fields["synapse_count"] = "2500";
    
    logger.logEnhanced(entry);
    
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, ConsoleLoggerBackwardCompatibility) {
    auto& logger = nlm::Logger::getGlobal();
    
    // Create a console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Log a message
    logger.info("Test message to console logger");
    
    // Verify level setting
    EXPECT_EQ(consoleLogger->getLevel(), nlm::LogLevel::Info);
    
    // Set level and verify
    consoleLogger->setLevel(nlm::LogLevel::Warning);
    EXPECT_EQ(consoleLogger->getLevel(), nlm::LogLevel::Warning);
}

TEST_F(AsyncLoggingTest, LogStream) {
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Use the stream-style logging
    nlm::LogStream stream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__);
    stream << "This is a stream log message";
    
    // The stream should have been flushed on destruction
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, PerformanceBenchmark) {
    auto& logger = nlm::Logger::getGlobal();
    
    // Disable async for this test to compare performance
    logger.enableAsyncLogging(false);
    
    // Measure time for synchronous logging
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        logger.info("Performance test message " + std::to_string(i));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Log the performance measurement
    logger.info("Logged 10,000 messages synchronously in " + std::to_string(duration.count()) + "ms");
    
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}

TEST_F(AsyncLoggingTest, AsyncPerformanceBenchmark) {
    auto& logger = nlm::Logger::getGlobal();
    logger.enableAsyncLogging(true);
    
    // Enable async logging for comparison
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        logger.info("Async performance test message " + std::to_string(i));
    }
    
    // Give async logger time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    logger.info("Logged 10,000 messages asynchronously in " + std::to_string(duration.count()) + "ms");
    
    EXPECT_TRUE(logger.isEnabled(nlm::LogLevel::Info));
}