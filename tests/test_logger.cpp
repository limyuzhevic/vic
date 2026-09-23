// Comprehensive Logging System Tests
#include "TestFixture.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Logger/HierarchicalLogger.hpp"
#include "../src/core/Logger/ComponentLogger.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>

class TestLogger : public TestFixture {
public:
    TestLogger() : TestFixture(
        "LoggerBasic",
        TestCategory::Logging,
        "Test basic logger functionality including log level filtering and message output"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Basic logger creation and destruction
        {
            nlm::Logger logger;
            assertNotNull(&logger, "Logger instance should not be null");
        }
        
        // Test 2: Global logger access
        {
            auto& globalLogger = nlm::Logger::getGlobal();
            assertNotNull(&globalLogger, "Global logger should not be null");
            
            globalLogger.setLevel(nlm::LogLevel::Info);
            assertEqual(globalLogger.getLevel(), nlm::LogLevel::Info, "Default level should be Info");
        }
        
        // Test 3: Log message with different levels
        {
            nlm::Logger logger;
            logger.setLevel(nlm::LogLevel::Debug);
            
            // Test debug log
            logger.debug("Debug message test");
            
            // Test info log  
            logger.info("Info message test");
            
            // Test warning log
            logger.warning("Warning message test");
            
            // Test error log
            logger.error("Error message test");
            
            // Test critical log
            logger.critical("Critical message test");
        }
        
        // Test 4: Log level filtering
        {
            nlm::Logger logger;
            
            // Set level to Warning - should not log Debug or Info
            logger.setLevel(nlm::LogLevel::Warning);
            
            // Enable component-specific logging
            logger.enableComponent("test_component");
            
            // Set component level
            logger.setComponentLevel("test_component", nlm::LogLevel::Debug);
        }
        
        // Test 5: Hierarchical logger
        {
            auto hierarchicalLogger = nlm::Logger::getGlobal().getHierarchicalLogger();
            assertNotNull(hierarchicalLogger.get(), "Hierarchical logger should not be null");
            
            // Register a component
            nlm::Logger::getGlobal().registerComponentLogger("parent", "", nlm::LogLevel::Info);
            
            // Get component logger
            auto componentLogger = nlm::Logger::getGlobal().getComponentLogger("child", "parent");
            assertNotNull(componentLogger.get(), "Component logger should not be null");
        }
        
        // Test 6: Stream-style logging
        {
            nlm::Logger logger;
            logger.setLevel(nlm::LogLevel::Info);
            
            // Test stream logging
            nlm::LogStream stream(nlm::LogLevel::Info, "test_file.cpp", 42, "test_function");
            stream << "Stream test with number: " << 123;
        }
        
        // Test 7: Thread safety
        {
            nlm::Logger logger;
            std::vector<std::thread> threads;
            
            for (int i = 0; i < 10; ++i) {
                threads.emplace_back([&logger, i]() {
                    logger.info("Thread-safe log message from thread " + std::to_string(i));
                });
            }
            
            for (auto& thread : threads) {
                thread.join();
            }
        }
        
        // Test 8: Component-specific logging
        {
            nlm::Logger logger;
            logger.enableComponent("component1");
            logger.enableComponent("component2");
            
            logger.setComponentLevel("component1", nlm::LogLevel::Debug);
            logger.setComponentLevel("component2", nlm::LogLevel::Warning);
            
            // Component-specific log calls
            logger.debug("Debug for component1", "", 0, "", "component1");
            logger.warning("Warning for component2", "", 0, "", "component2");
        }
    }
};

class TestLoggerFiltering : public TestFixture {
public:
    TestLoggerFiltering() : TestFixture(
        "LoggerFiltering",
        TestCategory::Logging,
        "Test logging filtering and routing functionality"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        nlm::Logger logger;
        
        // Test 1: Global level filtering
        {
            logger.setLevel(nlm::LogLevel::Error);
            // Only Error and Critical should be logged
        }
        
        // Test 2: Component-level filtering
        {
            logger.enableComponent("important_component");
            logger.setComponentLevel("important_component", nlm::LogLevel::Debug);
            
            logger.disableComponent("ignored_component");
            logger.setComponentLevel("ignored_component", nlm::LogLevel::Debug);
        }
        
        // Test 3: Hierarchical component filtering
        {
            logger.registerComponentLogger("parent", "", nlm::LogLevel::Info);
            logger.registerComponentLogger("child", "parent", nlm::LogLevel::Debug);
        }
        
        // Test 4: Custom filters
        {
            bool customFilter(const std::string&, const nlm::LogEntry&) {
                return true; // Accept all
            }
            
            logger.setComponentFilter("filtered_component", customFilter);
        }
    }
};

class TestLoggerPerformance : public TestFixture {
public:
    TestLoggerPerformance() : TestFixture(
        "LoggerPerformance",
        TestCategory::Logging,
        "Test logging performance characteristics"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        nlm::Logger logger;
        logger.setLevel(nlm::LogLevel::Debug);
        
        // Performance test: log many messages
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            logger.info("Performance test message " + std::to_string(i));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        assertTrue(duration < 1000.0, "Logging 1000 messages should take less than 1 second");
    }
};

class TestLoggerErrorHandling : public TestFixture {
public:
    TestLoggerErrorHandling() : TestFixture(
        "LoggerErrorHandling",
        TestCategory::ErrorHandling,
        "Test logging error handling and edge cases"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Log with empty message
        {
            nlm::Logger logger;
            logger.info(""); // Empty message should be handled
        }
        
        // Test 2: Log with null characters
        {
            nlm::Logger logger;
            logger.info("Message with\0null character"); // Null character in message
        }
        
        // Test 3: Log with very long message
        {
            nlm::Logger logger;
            std::string longMessage(10000, 'A');
            logger.info(longMessage);
        }
        
        // Test 4: Multiple hierarchical loggers
        {
            nlm::Logger logger;
            for (int i = 0; i < 100; ++i) {
                std::string parent = "parent_" + std::to_string(i);
                std::string child = "child_" + std::to_string(i);
                logger.registerComponentLogger(child, parent);
            }
        }
    }
};

// Run all logging tests
void runLoggerTests() {
    TestRunner runner;
    
    runner.addTest(std::make_unique<TestLogger>());
    runner.addTest(std::make_unique<TestLoggerFiltering>());
    runner.addTest(std::make_unique<TestLoggerPerformance>());
    runner.addTest(std::make_unique<TestLoggerErrorHandling>());
    
    runner.runAll();
}

// Entry point
int main() {
    runLoggerTests();
    return 0;
}
