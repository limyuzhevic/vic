// Logger Tests
// Tests for Logger and ConsoleLogger functionality

#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Error));
    
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLevelControl() {
    nlm::Logger logger;
    
    // Set logger level
    logger.setLevel(nlm::LogLevel::Warning);
    
    // At this point, we can't easily verify level filtering without
    // intercepting output, but we can at least test that it doesn't crash
    
    // Reset to default
    logger.setLevel(nlm::LogLevel::Info);
    
    std::cout << "    testLoggerLevelControl passed" << std::endl;
}

void testConsoleLoggerCreation() {
    nlm::ConsoleLogger logger;
    
    assert(logger.getLevel() == nlm::LogLevel::Info);
    assert(logger.getUseColors() == true);
    
    std::cout << "    testConsoleLoggerCreation passed" << std::endl;
}

void testConsoleLoggerLevelControl() {
    nlm::ConsoleLogger logger(nlm::LogLevel::Warning);
    assert(logger.getLevel() == nlm::LogLevel::Warning);
    
    std::cout << "    testConsoleLoggerLevelControl passed" << std::endl;
}

void testConsoleLoggerColors() {
    nlm::ConsoleLogger logger;
    
    assert(logger.getUseColors() == true);
    
    logger.setUseColors(false);
    assert(logger.getUseColors() == false);
    
    logger.setUseColors(true);
    assert(logger.getUseColors() == true);
    
    std::cout << "    testConsoleLoggerColors passed" << std::endl;
}

void testConsoleLoggerFileOutput() {
    nlm::ConsoleLogger logger;
    
    std::string testFile = "./test_output.log";
    
    // Enable file output
    logger.setOutputFile(testFile);
    
    // Write a log message
    logger.info("Test log message");
    
    // Note: We can't easily verify file output without complex mocking,
    // but we can at least test that it doesn't crash
    
    logger.closeOutputFile();
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConsoleLoggerFileOutput passed" << std::endl;
}

void testLogStream() {
    // LogStream is a macro-based helper, we can test basic functionality
    // by ensuring the logger is working
    
    nlm::Logger logger;
    
    // Use the convenience macro-style logging
    // Note: In real usage, these would be macros, but we can call the functions directly
    
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    logger.critical("Critical message");
    
    std::cout << "    testLogStream passed" << std::endl;
}

void testGlobalLogger() {
    nlm::Logger& globalLogger = nlm::Logger::getGlobal();
    
    // Should not crash
    globalLogger.info("Test global logger");
    
    // Test level filtering on global logger
    globalLogger.setLevel(nlm::LogLevel::Error);
    globalLogger.info("This should not be logged");  // Can't easily test, but no crash
    
    std::cout << "    testGlobalLogger passed" << std::endl;
}

void testLoggerAddRemoveSinks() {
    nlm::Logger logger;
    
    // Create and add a logger sink
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Write a log message
    logger.info("Test sink");
    
    // Clear all loggers
    logger.clearLoggers();
    
    std::cout << "    testLoggerAddRemoveSinks passed" << std::endl;
}

void testLogLevelStrings() {
    nlm::ConsoleLogger logger;
    
    // Test level to string conversion
    std::string debugStr = logger.levelToString(nlm::LogLevel::Debug);
    std::string infoStr = logger.levelToString(nlm::LogLevel::Info);
    std::string warnStr = logger.levelToString(nlm::LogLevel::Warning);
    std::string errorStr = logger.levelToString(nlm::LogLevel::Error);
    std::string criticalStr = logger.levelToString(nlm::LogLevel::Critical);
    
    assert(debugStr == "DEBUG");
    assert(infoStr == "INFO");
    assert(warnStr == "WARN");
    assert(errorStr == "ERROR");
    assert(criticalStr == "CRIT");
    
    std::cout << "    testLogLevelStrings passed" << std::endl;
}

void testLogLevelColors() {
    nlm::ConsoleLogger logger;
    
    // Test level to color conversion
    std::string debugColor = logger.levelToColor(nlm::LogLevel::Debug);
    std::string infoColor = logger.levelToColor(nlm::LogLevel::Info);
    std::string warnColor = logger.levelToColor(nlm::LogLevel::Warning);
    std::string errorColor = logger.levelToColor(nlm::LogLevel::Error);
    std::string criticalColor = logger.levelToColor(nlm::LogLevel::Critical);
    
    // Colors should not be empty
    assert(!debugColor.empty());
    assert(!infoColor.empty());
    assert(!warnColor.empty());
    assert(!errorColor.empty());
    assert(!criticalColor.empty());
    
    std::cout << "    testLogLevelColors passed" << std::endl;
}

void testLogEntryStructure() {
    // Test LogEntry structure by creating and checking components
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Test message";
    entry.file = "test.cpp";
    entry.line = 42;
    entry.function = "testFunction";
    
    assert(entry.level == nlm::LogLevel::Info);
    assert(entry.message == "Test message");
    assert(entry.file == "test.cpp");
    assert(entry.line == 42);
    assert(entry.function == "testFunction");
    
    std::cout << "    testLogEntryStructure passed" << std::endl;
}

void testLoggerThreadSafety() {
    nlm::Logger logger;
    
    // Log from multiple "threads" by calling from same thread multiple times
    // This tests that the mutex doesn't cause deadlocks
    for (int i = 0; i < 10; ++i) {
        logger.info("Test thread safety " + std::to_string(i));
    }
    
    std::cout << "    testLoggerThreadSafety passed" << std::endl;
}

void runAll() {
    std::cout << "=== Logger Tests ===" << std::endl;
    testLoggerCreation();
    testLoggerLevelControl();
    testConsoleLoggerCreation();
    testConsoleLoggerLevelControl();
    testConsoleLoggerColors();
    testConsoleLoggerFileOutput();
    testLogStream();
    testGlobalLogger();
    testLoggerAddRemoveSinks();
    testLogLevelStrings();
    testLogLevelColors();
    testLogEntryStructure();
    testLoggerThreadSafety();
    std::cout << "All logger tests passed!" << std::endl << std::endl;
}

} // namespace test_logger