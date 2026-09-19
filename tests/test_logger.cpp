#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

namespace test_logger {

void testBasicLogging() {
    std::cout << "  Testing basic logging..." << std::endl;
    
    // Create logger with Info level
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Test different log levels
    NLM_LOG_INFO("This is an info message");
    NLM_LOG_WARNING("This is a warning message");
    NLM_LOG_ERROR("This is an error message");
    
    // Debug level should not appear
    NLM_LOG_DEBUG("This debug message should NOT appear");
    
    // Verify logger state
    assert(consoleLogger->getLevel() == nlm::LogLevel::Info);
    assert(consoleLogger->getTotalLogCount() > 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testLevelFiltering() {
    std::cout << "  Testing level filtering..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Only warning and above should appear
    NLM_LOG_INFO("This info should NOT appear");
    NLM_LOG_WARNING("This warning should appear");
    NLM_LOG_ERROR("This error should appear");
    
    assert(consoleLogger->getLevel() == nlm::LogLevel::Warning);
    
    std::cout << "    PASSED" << std::endl;
}

void testStructuredLogging() {
    std::cout << "  Testing structured logging..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogConfig{});
    consoleLogger->configureFromConfig({});
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Test structured logging
    std::unordered_map<std::string, std::string> fields;
    fields["user_id"] = "12345";
    fields["action"] = "login";
    consoleLogger->logStructured("User authentication", fields);
    
    std::unordered_map<std::string, std::variant<int, double, std::string, bool>> data;
    data["error_code"] = 404;
    data["severity"] = std::string("high");
    data["retry_count"] = 3;
    data["success"] = false;
    consoleLogger->logStructured("API call failed", data);
    
    std::cout << "    PASSED" << std::endl;
}

void testFileOutput() {
    std::cout << "  Testing file output..." << std::endl;
    
    // Create a temporary log file
    const std::string logFile = "/tmp/test_log.txt";
    
    // Clean up any existing file
    std::remove(logFile.c_str());
    
    LogConfig config;
    config.level = nlm::LogLevel::Info;
    config.filePath = logFile;
    config.logToFile = true;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(config);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Log some messages
    NLM_LOG_INFO("Test message 1");
    NLM_LOG_WARNING("Test warning 1");
    NLM_LOG_ERROR("Test error 1");
    
    // Close logger to flush file
    consoleLogger->closeOutputFile();
    
    // Verify file was created and has content
    std::ifstream file(logFile);
    assert(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        assert(!line.empty());
    }
    
    file.close();
    std::remove(logFile.c_str());
    
    assert(lineCount >= 3); // At least 3 log messages
    
    std::cout << "    PASSED" << std::endl;
}

void testConfigIntegration() {
    std::cout << "  Testing configuration integration..." << std::endl;
    
    // Create config file
    const std::string configFile = "/tmp/test_logger_config.json";
    
    std::ofstream configOut(configFile);
    configOut << "{\n";
    configOut << "  \"logging\": {\n";
    configOut << "    \"level\": \"DEBUG\",\n";
    configOut << "    \"use_colors\": false,\n";
    configOut << "    \"to_file\": false,\n";
    configOut << "    \"structured\": true\n";
    configOut << "  }\n";
    configOut << "}\n";
    configOut.close();
    
    // Create logger and load config
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Load configuration
    consoleLogger->loadConfigFromFile(configFile);
    
    // Verify config was loaded
    assert(consoleLogger->getLevel() == nlm::LogLevel::Debug);
    assert(!consoleLogger->getUseColors());
    
    // Test with config
    NLM_LOG_DEBUG("Debug message with config");
    NLM_LOG_INFO("Info message with config");
    
    std::remove(configFile.c_str());
    
    std::cout << "    PASSED" << std::endl;
}

void testPerformanceMonitoring() {
    std::cout << "  Testing performance monitoring..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Log many messages
    for (int i = 0; i < 100; ++i) {
        NLM_LOG_INFO("Performance test message " + std::to_string(i));
    }
    
    // Check performance metrics
    assert(consoleLogger->getTotalLogCount() >= 100);
    
    // Reset metrics
    consoleLogger->resetPerformanceMetrics();
    assert(consoleLogger->getTotalLogCount() == 0);
    
    // Test performance summary
    std::string summary = consoleLogger->getPerformanceSummary();
    assert(!summary.empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testThreadSafety() {
    std::cout << "  Testing thread safety..." << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Log from multiple threads
    const int numThreads = 10;
    const int messagesPerThread = 50;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([i, messagesPerThread]() {
            for (int j = 0; j < messagesPerThread; ++j) {
                NLM_LOG_INFO("Thread " + std::to_string(i) + ", message " + std::to_string(j));
            }
        });
    }
    
    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all messages were logged
    assert(consoleLogger->getTotalLogCount() == numThreads * messagesPerThread);
    
    std::cout << "    PASSED" << std::endl;
}

void testFiltering() {
    std::cout << "  Testing filtering..." << std::endl;
    
    LogConfig config;
    config.level = nlm::LogLevel::Info;
    config.filters["performance"] = "true";
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(config);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Test with structured logging that has performance field
    std::unordered_map<std::string, std::string> fields;
    fields["performance"] = "true";
    consoleLogger->logStructured("Performance metric", fields);
    
    // Test without performance field
    std::unordered_map<std::string, std::string> fields2;
    fields2["user"] = "test";
    consoleLogger->logStructured("User action", fields2);
    
    // Check that performance field was filtered
    uint64_t filteredCount = consoleLogger->getFilteredCount("performance");
    assert(filteredCount >= 1);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Logger Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testBasicLogging();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testLevelFiltering();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testStructuredLogging();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testFileOutput();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testConfigIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPerformanceMonitoring();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testThreadSafety();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testFiltering();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Logger Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Logger Tests FAILED ===" << std::endl;
    }
}

} // namespace test_logger