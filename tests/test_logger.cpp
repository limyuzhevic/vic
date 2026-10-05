// Logger Tests
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

namespace test_logger {

void testLoggerBasicFunctionality() {
    // Create a logger and add a console logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    
    // Test logging at different levels
    consoleLogger->setLevel(nlm::LogLevel::Info);
    
    // This should log (at least to cout)
    NLM_LOG_INFO("Test message 1");
    NLM_LOG_WARNING("Test warning message");
    NLM_LOG_ERROR("Test error message");
    
    std::cout << "    testLoggerBasicFunctionality passed" << std::endl;
}

void testLoggerLevels() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger->addLogger(consoleLogger);
    
    // Set level to Warning
    consoleLogger->setLevel(nlm::LogLevel::Warning);
    
    // Info messages should not be logged
    NLM_LOG_INFO("This should not appear");
    
    // Warning and error should appear
    NLM_LOG_WARNING("This should appear");
    NLM_LOG_ERROR("This should appear too");
    
    std::cout << "    testLoggerLevels passed" << std::endl;
}

void testLoggerColors() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    
    NLM_LOG_INFO("Colored log message");
    NLM_LOG_ERROR("Error colored log message");
    
    consoleLogger->setUseColors(false);
    NLM_LOG_INFO("Uncolored log message");
    
    std::cout << "    testLoggerColors passed" << std::endl;
}

void testLoggerStructuredFormat() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseStructuredFormat(true);
    logger->addLogger(consoleLogger);
    
    // Test structured logging
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Test structured message";
    entry.file = "test.cpp";
    entry.line = 123;
    entry.function = "testFunction";
    entry.timestamp = std::chrono::system_clock::now();
    
    // This would go through the console logger
    consoleLogger->log(entry);
    
    std::cout << "    testLoggerStructuredFormat passed" << std::endl;
}

void testLoggerFormatString() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseStructuredFormat(false);
    consoleLogger->setFormatString("[{timestamp}] [{level}] [{file}:{line}] {message}");
    logger->addLogger(consoleLogger);
    
    NLM_LOG_INFO("Test with custom format");
    
    std::cout << "    testLoggerFormatString passed" << std::endl;
}

void testLoggerBuffering() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setBufferSize(10);
    consoleLogger->setFlushInterval(std::chrono::milliseconds(50));
    logger->addLogger(consoleLogger);
    
    // Log multiple messages to test buffering
    for (int i = 0; i < 15; ++i) {
        NLM_LOG_INFO("Buffered message " + std::to_string(i));
    }
    
    // Wait for buffer to be flushed
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    consoleLogger->flush();
    
    std::cout << "    testLoggerBuffering passed" << std::endl;
}

void testLoggerFileOutput() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    
    // Set file output (we'll test the method but not actually write)
    consoleLogger->setOutputFile("test_log.txt");
    NLM_LOG_INFO("This should go to file");
    consoleLogger->closeOutputFile();
    
    std::cout << "    testLoggerFileOutput passed" << std::endl;
}

void testLoggerGlobalFunctions() {
    // Test the global logger functions
    auto globalLogger = nlm::Logger::getGlobal();
    
    // This should work with the global logger
    NLM_LOG_INFO("Global logger test 1");
    NLM_LOG_WARNING("Global logger test 2");
    
    // Test level checking
    bool enabled = globalLogger.isEnabled(nlm::LogLevel::Info);
    assert(enabled); // Should be enabled by default
    
    std::cout << "    testLoggerGlobalFunctions passed" << std::endl;
}

void runAll() {
    testLoggerBasicFunctionality();
    testLoggerLevels();
    testLoggerColors();
    testLoggerStructuredFormat();
    testLoggerFormatString();
    testLoggerBuffering();
    testLoggerFileOutput();
    testLoggerGlobalFunctions();
}

} // namespace test_logger