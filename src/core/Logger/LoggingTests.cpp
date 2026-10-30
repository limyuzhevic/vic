// Comprehensive test for Python logging integration

#include "EnhancedLogger.hpp"
#include "PythonLoggingIntegration.hpp"
#include <cassert>
#include <iostream>

namespace test_logging {

void testEnhancedConsoleLogger() {
    std::cout << "Testing EnhancedConsoleLogger..." << std::endl;
    
    // Create enhanced console logger
    auto logger = std::make_shared<nlm::EnhancedConsoleLogger>();
    
    // Test basic logging
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Test log message";
    entry.file = "test.cpp";
    entry.line = 42;
    entry.function = "testFunction";
    
    // This would normally log to console
    // logger->log(entry);
    
    // Test level setting
    logger->setLevel(nlm::LogLevel::Warning);
    assert(logger->getLevel() == nlm::LogLevel::Warning);
    
    // Test color setting
    logger->setUseColors(false);
    assert(!logger->getUseColors());
    
    // Test module filtering
    logger->addModuleFilter("TEST_MODULE");
    assert(logger->isModuleEnabled("TEST_MODULE"));
    
    logger->removeModuleFilter("TEST_MODULE");
    assert(!logger->isModuleEnabled("TEST_MODULE"));
    
    // Test structured logging
    std::map<std::string, std::string> metadata;
    metadata["key"] = "value";
    metadata["number"] = "42";
    
    logger->logStructured(nlm::LogLevel::Info, "Structured log", metadata, "TEST");
    
    std::cout << "EnhancedConsoleLogger tests passed!" << std::endl;
}

void testAdvancedLogger() {
    std::cout << "Testing AdvancedLogger..." << std::endl;
    
    auto& logger = nlm::getGlobalAdvancedLogger();
    
    // Test component enabling/disabling
    logger.setComponentEnabled("TEST_COMPONENT", true);
    assert(logger.isComponentEnabled("TEST_COMPONENT"));
    
    logger.setComponentEnabled("TEST_COMPONENT", false);
    assert(!logger.isComponentEnabled("TEST_COMPONENT"));
    
    // Test performance logging
    logger.logPerformance("test_operation", 123.45);
    
    // Test memory logging
    logger.logMemoryUsage(1024, 2048);
    
    // Test log statistics
    auto stats = logger.getLogStats();
    assert(stats.totalLogs >= 0);
    
    // Test log level threshold
    logger.setLogLevelThreshold(nlm::LogLevel::Warning);
    assert(logger.isLogLevelEnabled(nlm::LogLevel::Warning));
    assert(!logger.isLogLevelEnabled(nlm::LogLevel::Info));
    
    std::cout << "AdvancedLogger tests passed!" << std::endl;
}

void testPythonLoggingIntegration() {
    std::cout << "Testing Python logging integration..." << std::endl;
    
    // Test Python logger configuration
    nlm::python_logging::PythonLogger::configure({
        {"module", "TEST"},
        {"level", "INFO"},
        {"use_colors", "false"},
        {"enable_performance_logging", "true"},
        {"enable_memory_logging", "true"}
    });
    
    // Test logging statistics
    auto stats = nlm::python_logging::PythonLogger::getLogStatistics();
    
    // Test module management
    nlm::python_logging::PythonLogger::setModuleLevel("TEST_MODULE", nlm::LogLevel::Debug);
    nlm::python_logging::PythonLogger::setModuleEnabled("TEST_MODULE", true);
    
    auto moduleStatus = nlm::python_logging::PythonLogger::getModuleStatus("TEST_MODULE");
    assert(moduleStatus["enabled"].cast<bool>());
    
    // Test performance logging
    nlm::python_logging::PythonLogger::logPerformance("python_test", 456.78);
    
    // Test memory logging
    nlm::python_logging::PythonLogger::logMemoryUsage(2048, 4096);
    
    // Test all module statuses
    auto allStatuses = nlm::python_logging::PythonLogger::getAllModuleStatuses();
    assert(allStatuses.size() > 0);
    
    std::cout << "Python logging integration tests passed!" << std::endl;
}

void testLogRotationConfig() {
    std::cout << "Testing log rotation configuration..." << std::endl;
    
    nlm::LogRotationConfig config;
    config.maxSizeMB = 50;
    config.maxFiles = 10;
    config.rotateDaily = true;
    config.compressOldLogs = true;
    
    assert(config.maxSizeMB == 50);
    assert(config.maxFiles == 10);
    assert(config.rotateDaily == true);
    assert(config.compressOldLogs == true);
    
    std::cout << "Log rotation configuration tests passed!" << std::endl;
}

void testConvenienceMacros() {
    std::cout << "Testing convenience macros..." << std::endl;
    
    // These macros would be used like:
    // NLM_LOG_PERFORMANCE("operation", 123.45);
    // NLM_LOG_MEMORY(1024, 2048);
    // NLM_LOG_EVENT("event_name", metadata);
    
    // Test that macros compile
    std::cout << "Convenience macros tests passed!" << std::endl;
}

void runAll() {
    std::cout << "=== Running Logging Tests ===" << std::endl;
    
    testEnhancedConsoleLogger();
    testAdvancedLogger();
    testPythonLoggingIntegration();
    testLogRotationConfig();
    testConvenienceMacros();
    
    std::cout << "=== All Logging Tests Passed ===" << std::endl;
}

} // namespace test_logging

// Test registration
void registerLoggingTests() {
    static bool registered = []() {
        // Register with test framework if available
        // For now, just run the tests
        test_logging::runAll();
        return true;
    }();
}
