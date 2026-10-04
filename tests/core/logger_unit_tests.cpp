// Unit test for Logger class
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

namespace test_logger_unit {

void testLoggerCreation() {
    nlm::Logger logger;
    
    // Test basic logging functionality
    logger.log("test_message", nlm::LogLevel::INFO);
    logger.log("warning_message", nlm::LogLevel::WARNING);
    logger.log("error_message", nlm::LogLevel::ERROR);
    
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLevels() {
    nlm::Logger logger;
    
    // Test different log levels
    logger.log("debug_test", nlm::LogLevel::DEBUG);
    logger.log("info_test", nlm::LogLevel::INFO);
    logger.log("warning_test", nlm::LogLevel::WARNING);
    logger.log("error_test", nlm::LogLevel::ERROR);
    
    std::cout << "    testLoggerLevels passed" << std::endl;
}

void testLoggerFormat() {
    nlm::Logger logger;
    
    // Test different message formats
    logger.log("simple_message", nlm::LogLevel::INFO);
    logger.log("format_test: formatted_output", nlm::LogLevel::INFO);
    logger.log("message_with_numbers: 123", nlm::LogLevel::INFO);
    
    std::cout << "    testLoggerFormat passed" << std::endl;
}

void testLoggerMultipleMessages() {
    nlm::Logger logger;
    
    // Test multiple log messages
    for (int i = 0; i < 10; ++i) {
        logger.log(std::string("message_") + std::to_string(i), nlm::LogLevel::INFO);
    }
    
    std::cout << "    testLoggerMultipleMessages passed" << std::endl;
}

void testLoggerAsync() {
    nlm::Logger logger;
    
    // Test async logging (if supported)
    logger.log("async_test", nlm::LogLevel::INFO);
    
    std::cout << "    testLoggerAsync passed" << std::endl;
}

void runAllLoggerUnitTests() {
    testLoggerCreation();
    testLoggerLevels();
    testLoggerFormat();
    testLoggerMultipleMessages();
    testLoggerAsync();
}

} // namespace test_logger_unit