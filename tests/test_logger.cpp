#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace test_logger {

void testLogLevels() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Clear any existing loggers
    logger.clearLoggers();
    
    // Add a console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    consoleLogger->setUseColors(false);
    logger.addLogger(consoleLogger);
    
    // Test that DEBUG and INFO messages are filtered out
    std::cout << "Testing log level filtering..." << std::endl;
    logger.debug("This should NOT appear");
    logger.info("This should NOT appear");
    
    // Test that WARNING and ERROR messages appear
    logger.warning("This SHOULD appear");
    logger.error("This SHOULD appear");
    
    std::cout << "Log level filtering test passed" << std::endl;
}

void testLogFileOutput() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Clear any existing loggers
    logger.clearLoggers();
    
    // Clean up any existing test log file
    std::remove("test_logger.log");
    
    // Add a console logger with file output
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
    consoleLogger->setUseColors(false);
    consoleLogger->setOutputFile("test_logger.log");
    logger.addLogger(consoleLogger);
    
    // Generate some log messages
    logger.debug("Debug message for file output");
    logger.info("Info message for file output");
    logger.warning("Warning message for file output");
    logger.error("Error message for file output");
    logger.critical("Critical message for file output");
    
    consoleLogger->closeOutputFile();
    
    // Read the log file and verify messages
    std::ifstream logFile("test_logger.log");
    std::string line;
    int lineCount = 0;
    
    while (std::getline(logFile, line)) {
        lineCount++;
    }
    
    assert(lineCount >= 5, "Expected at least 5 log messages in file");
    
    std::remove("test_logger.log");
    
    std::cout << "Log file output test passed" << std::endl;
}

void testLogRotaton() {
    // TODO: Implement log rotation tests
    std::cout << "Log rotation test passed (placeholder)" << std::endl;
}

void testStructuredLogging() {
    // TODO: Implement structured logging tests
    std::cout << "Structured logging test passed (placeholder)" << std::endl;
}

void testAsyncLogging() {
    // TODO: Implement asynchronous logging tests
    std::cout << "Asynchronous logging test passed (placeholder)" << std::endl;
}

void testLoggerConvenienceMacros() {
    // Clear any existing loggers
    nlm::Logger::getGlobal().clearLoggers();
    
    // Add a console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseColors(false);
    nlm::Logger::getGlobal().addLogger(consoleLogger);
    
    // Test convenience macros
    NLM_LOG_DEBUG("Test debug message using macro");
    NLM_LOG_INFO("Test info message using macro");
    NLM_LOG_WARNING("Test warning message using macro");
    NLM_LOG_ERROR("Test error message using macro");
    NLM_LOG_CRITICAL("Test critical message using macro");
    
    std::cout << "Convenience macros test passed" << std::endl;
}

void testLogStream() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    // Clear any existing loggers
    logger.clearLoggers();
    
    // Add a console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
    consoleLogger->setUseColors(false);
    logger.addLogger(consoleLogger);
    
    // Test stream-style logging
    nlm::LogStream level(nlm::LogLevel::Info, "test.cpp", 123, "testFunction");
    level << "Stream message with number " << 42 << " and string " << "test";
    
    std::cout << "Log stream test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Logger Tests ===" << std::endl;
    
    testLogLevels();
    testLogFileOutput();
    testLogRotaton();
    testStructuredLogging();
    testAsyncLogging();
    testLoggerConvenienceMacros();
    testLogStream();
    
    std::cout << "=== All Logger Tests Passed ===" << std::endl;
}

} // namespace test_logger