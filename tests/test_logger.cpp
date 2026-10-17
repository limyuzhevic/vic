// Logger Tests
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <memory>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLevel() {
    nlm::Logger logger;
    
    // Test level setting
    logger.setLevel(nlm::LogLevel::Warning);
    
    // Debug should not be enabled at Warning level
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Error));
    assert(logger.isEnabled(nlm::LogLevel::Critical));
    
    std::cout << "    testLoggerLevel passed" << std::endl;
}

void testLogStream() {
    nlm::Logger logger;
    
    // This test verifies that log stream works without crashing
    // The actual output verification would require capturing console output
    NLM_LOG_STREAM(nlm::LogLevel::Info) << "Test message";
    
    std::cout << "    testLogStream passed" << std::endl;
}

void testConvenienceMacros() {
    nlm::Logger logger;
    
    // Test that convenience macros compile and work
    // These macros would need to be captured for actual verification
    NLM_LOG_INFO("Info test message");
    NLM_LOG_WARNING("Warning test message");
    NLM_LOG_ERROR("Error test message");
    NLM_LOG_DEBUG("Debug test message");
    
    std::cout << "    testConvenienceMacros passed" << std::endl;
}

void runAll() {
    testLoggerCreation();
    testLoggerLevel();
    testLogStream();
    testConvenienceMacros();
}

} // namespace test_logger
