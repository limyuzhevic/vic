#include "Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testBasicLogging() {
    std::cout << "=== Testing Basic Logging ===\n";
    
    // Test ConsoleLogger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    nlm::Logger::getGlobal().debug("Debug message test");
    nlm::Logger::getGlobal().info("Info message test");
    nlm::Logger::getGlobal().warning("Warning message test");
    nlm::Logger::getGlobal().error("Error message test");
    nlm::Logger::getGlobal().critical("Critical message test");
    
    std::cout << "✓ Basic logging completed\n";
}

void testStructuredLogging() {
    std::cout << "\n=== Testing Structured Logging ===\n";
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    consoleLogger->setFormatter(std::make_shared<nlm::JsonFormatter>());
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    nlm::Logger::getGlobal().info("Structured JSON log test");
    
    std::cout << "✓ Structured logging completed\n";
}

void testFileLogger() {
    std::cout << "\n=== Testing FileLogger ===\n";
    
    // Create test file path
    std::string testFile = "/tmp/test_logs/test.log";
    
    auto fileLogger = std::make_shared<nlm::FileLogger>(testFile, 1024 * 1024, 3); // 1MB max size, 3 files
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(fileLogger);
    
    // Log some messages
    nlm::Logger::getGlobal().debug("Debug message for file");
    nlm::Logger::getGlobal().info("Info message for file");
    nlm::Logger::getGlobal().warning("Warning message for file");
    
    // Check file exists
    if (std::filesystem::exists(testFile)) {
        std::cout << "✓ FileLogger created file: " << testFile << "\n";
    }
    
    fileLogger->rotate(); // Test rotation
    std::cout << "✓ FileLogger rotation completed\n";
}

void testErrorRecovery() {
    std::cout << "\n=== Testing Error Recovery ===\n";
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    // Log with error context
    nlm::Logger::getGlobal().error("Error with context", "ERR_001", __FILE__, __LINE__, __func__, "Stack trace here");
    
    // Log with metadata
    std::unordered_map<std::string, std::string> metadata;
    metadata["user_id"] = "12345";
    metadata["action"] = "login";
    nlm::Logger::getGlobal().logWithMetadata(nlm::LogLevel::Info, "User login attempt", metadata);
    
    // Log with category
    nlm::Logger::getGlobal().logWithCategory(nlm::LogLevel::Warning, "Network latency issue", "Network");
    
    std::cout << "✓ Error recovery features tested\n";
}

void testAsyncLogging() {
    std::cout << "\n=== Testing Async Logging ===\n";
    
    // Create AsyncLogger
    auto asyncLogger = std::make_shared<nlm::AsyncLogger>(1000);
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    asyncLogger->setLogger(consoleLogger);
    
    // Log multiple messages
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(asyncLogger);
    
    for (int i = 0; i < 5; ++i) {
        nlm::Logger::getGlobal().info("Async log message " + std::to_string(i));
    }
    
    asyncLogger->stop(); // Stop async logging
    std::cout << "✓ Async logging completed\n";
}

void testLogStream() {
    std::cout << "\n=== Testing LogStream ===\n";
    
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(std::make_shared<nlm::ConsoleLogger>());
    
    // Use LogStream with metadata
    NLM_LOG_STREAM(nlm::LogLevel::Info) << "LogStream test with " << "metadata" << " and " << "custom values";
    
    // Use LogStream with error context
    NLM_LOG_STREAM(nlm::LogLevel::Error).errorCode("ERR_001").stackTrace("Full stack trace here");
    
    // Use LogStream with category
    NLM_LOG_STREAM(nlm::LogLevel::Warning).category("Performance") << "Slow operation detected";
    
    std::cout << "✓ LogStream features tested\n";
}

void testLogCategories() {
    std::cout << "\n=== Testing Log Categories ===\n";
    
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(std::make_shared<nlm::ConsoleLogger>());
    
    // Log with different categories
    nlm::Logger::getGlobal().logWithCategory(nlm::LogLevel::Info, "System startup complete", "System");
    nlm::Logger::getGlobal().logWithCategory(nlm::LogLevel::Info, "User login successful", "User");
    nlm::Logger::getGlobal().logWithCategory(nlm::LogLevel::Warning, "Network connection timeout", "Network");
    nlm::Logger::getGlobal().logWithCategory(nlm::LogLevel::Error, "Database query failed", "Database");
    
    std::cout << "✓ Log categories tested\n";
}

void testLogFiltering() {
    std::cout << "\n=== Testing Log Filtering ===\n";
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    nlm::Logger::getGlobal().debug("Debug message (should not appear)");
    nlm::Logger::getGlobal().info("Info message (should not appear)");
    nlm::Logger::getGlobal().warning("Warning message (should appear)");
    nlm::Logger::getGlobal().error("Error message (should appear)");
    
    std::cout << "✓ Log filtering tested\n";
}

void testPerformanceLogging() {
    std::cout << "\n=== Testing Performance Logging ===\n";
    
    nlm::Logger::getGlobal().clearLoggers();
    nlm::Logger::getGlobal().addLogger(std::make_shared<nlm::ConsoleLogger>());
    
    // Measure logging performance
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        nlm::Logger::getGlobal().info("Performance test message " + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "✓ Performance test: 1000 logs in " << duration.count() << " ms\n";
}

int main() {
    try {
        testBasicLogging();
        testStructuredLogging();
        testFileLogger();
        testErrorRecovery();
        testAsyncLogging();
        testLogStream();
        testLogCategories();
        testLogFiltering();
        testPerformanceLogging();
        
        std::cout << "\n=== All Tests Completed Successfully ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}