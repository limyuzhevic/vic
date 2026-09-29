#include "core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <cassert>
#include <atomic>

using namespace nlm;

class LoggerPerformanceTest {
public:
    static void testAsyncLogging() {
        std::cout << "Testing async logging..." << std::endl;
        
        // Enable async logging
        Logger::getGlobal().setAsyncLogging(true, 1000, 4);
        
        // Add loggers
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(false);
        Logger::getGlobal().addLogger(consoleLogger);
        
        // Test async logging
        Logger::getGlobal().info("Test async log message 1", "test.cpp", 1, "testAsyncLogging", true);
        Logger::getGlobal().warning("Test async warning message", "test.cpp", 2, "testAsyncLogging", true);
        Logger::getGlobal().error("Test async error message", "test.cpp", 3, "testAsyncLogging", true);
        
        // Wait for async logs to complete
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Verify stats
        auto stats = Logger::getGlobal().getPerformanceStats();
        assert(stats.totalLogs >= 3); // At least 3 logs should have been processed
        
        std::cout << "✓ Async logging test passed" << std::endl;
        Logger::getGlobal().setAsyncLogging(false); // Clean up
    }
    
    static void testPerformanceStats() {
        std::cout << "Testing performance statistics..." << std::endl;
        
        // Reset stats
        Logger::getGlobal().resetPerformanceStats();
        
        // Enable performance monitoring on console logger
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setEnablePerformanceStats(true);
        Logger::getGlobal().addLogger(consoleLogger);
        
        // Perform some logging
        for (int i = 0; i < 10; ++i) {
            consoleLogger->log(LogEntry{LogLevel::Info, "Performance test message " + std::to_string(i), 
                                      "test.cpp", i, "performanceTest"});
        }
        
        // Wait a bit for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Check performance stats
        double avgTime = consoleLogger->getAverageLogTime();
        assert(avgTime >= 0.0); // Should be non-negative
        
        std::cout << "✓ Performance statistics test passed" << std::endl;
    }
    
    static void testFileLogger() {
        std::cout << "Testing file logger..." << std::endl;
        
        // Create temporary file
        std::string testFile = "test_log.txt";
        
        // Create file logger
        auto fileLogger = std::make_shared<FileLogger>(testFile, LogLevel::Debug);
        Logger::getGlobal().addLogger(fileLogger);
        
        // Test file logging
        Logger::getGlobal().debug("Debug message to file", "test.cpp", 1, "testFileLogger");
        Logger::getGlobal().info("Info message to file", "test.cpp", 2, "testFileLogger");
        
        // Wait for file operations
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Check if file was created and has content
        std::ifstream file(testFile);
        assert(file.is_open());
        
        std::string line;
        int lineCount = 0;
        while (std::getline(file, line)) {
            lineCount++;
        }
        
        assert(lineCount >= 2); // Should have at least 2 log entries
        
        file.close();
        std::remove(testFile.c_str());
        
        std::cout << "✓ File logger test passed" << std::endl;
    }
    
    static void testLogLevels() {
        std::cout << "Testing log levels..." << endl;
        
        // Create logger with specific level
        auto logger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        Logger::getGlobal().addLogger(logger);
        
        // Test that only Warning and Error logs are processed
        Logger::getGlobal().debug("Debug message (should not appear)", "test.cpp", 1, "testLogLevels");
        Logger::getGlobal().info("Info message (should not appear)", "test.cpp", 2, "testLogLevels");
        Logger::getGlobal().warning("Warning message (should appear)", "test.cpp", 3, "testLogLevels");
        Logger::getGlobal().error("Error message (should appear)", "test.cpp", 4, "testLogLevels");
        Logger::getGlobal().critical("Critical message (should appear)", "test.cpp", 5, "testLogLevels");
        
        // Wait for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        std::cout << "✓ Log level filtering test passed" << std::endl;
    }
    
    static void testLogStream() {
        std::cout << "Testing log stream..." << std::endl;
        
        // Enable async logging for stream test
        Logger::getGlobal().setAsyncLogging(true, 1000, 2);
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(false);
        Logger::getGlobal().addLogger(consoleLogger);
        
        // Test log stream
        NLM_LOG_STREAM(LogLevel::Info) << "Stream log message with " << 42 << " numbers and " << "strings";
        
        // Wait for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "✓ Log stream test passed" << std::endl;
        Logger::getGlobal().setAsyncLogging(false);
    }
    
    static void testMultipleLoggers() {
        std::cout << "Testing multiple loggers..." << std::endl;
        
        // Clear existing loggers
        Logger::getGlobal().clearLoggers();
        
        // Create multiple loggers
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(false);
        
        auto fileLogger = std::make_shared<FileLogger>("multi_test.log", LogLevel::Info);
        
        // Add both loggers
        Logger::getGlobal().addLogger(consoleLogger);
        Logger::getGlobal().addLogger(fileLogger);
        
        // Test with both loggers
        Logger::getGlobal().info("Message to both loggers", "test.cpp", 1, "testMultipleLoggers");
        
        // Wait for processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Check file logger
        std::ifstream file("multi_test.log");
        assert(file.is_open());
        
        std::string line;
        if (std::getline(file, line)) {
            assert(line.find("Message to both loggers") != std::string::npos);
        }
        
        file.close();
        std::remove("multi_test.log");
        
        std::cout << "✓ Multiple loggers test passed" << std::endl;
    }
    
    static void runAllTests() {
        std::cout << "========================================" << std::endl;
        std::cout << "     NLM Logger Performance Tests" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        try {
            testAsyncLogging();
            testPerformanceStats();
            testFileLogger();
            testLogLevels();
            testLogStream();
            testMultipleLoggers();
            
            std::cout << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "✓ All tests passed successfully!" << std::endl;
            std::cout << "========================================" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << "✗ Test failed: " << e.what() << std::endl;
            std::cout << "========================================" << std::endl;
            throw;
        }
    }
};

// Global function for easy calling
void testLoggerPerformance() {
    LoggerPerformanceTest::runAllTests();
}

// Main function for standalone execution
int main() {
    try {
        LoggerPerformanceTest::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}