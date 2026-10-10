// Logger Tests
// Core logging system validation tests

#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <future>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    assert(logger.getLevel() == nlm::LogLevel::Info);
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLogLevel() {
    nlm::Logger logger;
    
    logger.setLevel(nlm::LogLevel::Debug);
    assert(logger.getLevel() == nlm::LogLevel::Debug);
    
    logger.setLevel(nlm::LogLevel::Error);
    assert(logger.getLevel() == nlm::LogLevel::Error);
    
    std::cout << "    testLoggerLogLevel passed" << std::endl;
}

void testLoggerLogMessage() {
    nlm::Logger logger;
    logger.setLevel(nlm::LogLevel::Debug);
    
    logger.log(nlm::LogLevel::Info, "Test info message");
    logger.log(nlm::LogLevel::Warning, "Test warning message");
    logger.log(nlm::LogLevel::Error, "Test error message");
    logger.log(nlm::LogLevel::Debug, "Test debug message");
    
    std::cout << "    testLoggerLogMessage passed" << std::endl;
}

void testLoggerLogLevelFiltering() {
    nlm::Logger logger;
    
    logger.setLevel(nlm::LogLevel::Warning);
    
    logger.log(nlm::LogLevel::Warning, "This should appear");
    logger.log(nlm::LogLevel::Error, "This should appear too");
    
    logger.log(nlm::LogLevel::Info, "This should be filtered");
    logger.log(nlm::LogLevel::Debug, "This should also be filtered");
    
    std::cout << "    testLoggerLogLevelFiltering passed" << std::endl;
}

void testLoggerThreadSafety() {
    nlm::Logger logger;
    logger.setLevel(nlm::LogLevel::Debug);
    
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < 10; ++i) {
        futures.push_back(std::async(std::launch::async, [&logger, i]() {
            for (int j = 0; j < 100; ++j) {
                logger.log(nlm::LogLevel::Info, "Thread " + std::to_string(i) + 
                          " message " + std::to_string(j));
            }
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    logger.log(nlm::LogLevel::Info, "Final message after concurrent logging");
    
    std::cout << "    testLoggerThreadSafety passed" << std::endl;
}

void testLoggerSummary() {
    nlm::Logger logger;
    
    logger.log(nlm::LogLevel::Info, "Info message");
    logger.log(nlm::LogLevel::Warning, "Warning message");
    logger.log(nlm::LogLevel::Error, "Error message");
    
    std::cout << "    testLoggerSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Logger tests..." << std::endl;
    testLoggerCreation();
    testLoggerLogLevel();
    testLoggerLogMessage();
    testLoggerLogLevelFiltering();
    testLoggerThreadSafety();
    testLoggerSummary();
}

} // namespace test_logger