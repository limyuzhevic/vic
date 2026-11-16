// Logger Tests
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLogging() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info));
    
    // Test logging at different levels
    logger.info("Test info message");
    logger.warning("Test warning message");
    logger.error("Test error message");
    logger.debug("Test debug message");
    
    // Test global logger
    nlm::Logger::getGlobal().info("Test global message");
    
    std::cout << "    testLoggerLogging passed" << std::endl;
}

void testLoggerLevels() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    
    // Set logger level
    logger.setLevel(nlm::LogLevel::Warning);
    
    // Only warning and error should be logged
    logger.info("This should not appear");
    logger.warning("This should appear");
    logger.error("This should appear");
    logger.debug("This should not appear");
    
    std::cout << "    testLoggerLevels passed" << std::endl;
}

void testLoggerThreadSafety() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info));
    
    // Test thread safety by logging from multiple threads
    std::vector<std::thread> threads;
    const int numThreads = 10;
    const int messagesPerThread = 100;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < messagesPerThread; ++j) {
                logger.info("Thread " + std::to_string(i) + " message " + std::to_string(j));
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "    testLoggerThreadSafety passed" << std::endl;
}

void testLoggerFileOutput() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    
    const std::string testFile = "/tmp/test_logger.log";
    
    // Set up file output
    consoleLogger.setOutputFile(testFile);
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(consoleLogger));
    
    // Log some messages
    logger.info("Test message 1");
    logger.warning("Test message 2");
    logger.error("Test message 3");
    
    // Flush and close
    consoleLogger.closeOutputFile();
    
    // Verify file was created and has content
    assert(std::filesystem::exists(testFile));
    
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    assert(!content.empty());
    assert(content.find("Test message 1") != std::string::npos);
    assert(content.find("Test message 2") != std::string::npos);
    assert(content.find("Test message 3") != std::string::npos);
    
    file.close();
    std::filesystem::remove(testFile);
    
    std::cout << "    testLoggerFileOutput passed" << std::endl;
}

void testLoggerColorSupport() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info));
    
    // Test with colors enabled (default)
    consoleLogger.setUseColors(true);
    logger.info("Test colored message");
    
    // Test with colors disabled
    consoleLogger.setUseColors(false);
    logger.info("Test plain message");
    
    std::cout << "    testLoggerColorSupport passed" << std::endl;
}

void runAll() {
    testLoggerCreation();
    testLoggerLogging();
    testLoggerLevels();
    testLoggerThreadSafety();
    testLoggerFileOutput();
    testLoggerColorSupport();
}

} // namespace test_logger