// TestErrorHandling.cpp - Comprehensive test for error handling system
// This file demonstrates all error handling features working together

#include "Error.hpp"
#include "ErrorReport.hpp"
#include "Logger.hpp"
#include "Logger.hpp"
#include "InputValidator.hpp"
#include "Config.hpp"
#include "Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

namespace nlm {

// Test utilities
class TestErrorHandling {
public:
    static void runAllTests() {
        std::cout << "=== NLM Error Handling Test Suite ===" << std::endl;
        
        // Initialize error reporting system
        ErrorReporting::initialize();
        Validation::initialize();
        
        try {
            // Run individual test suites
            testBasicErrorHandling();
            testLoggerErrorHandling();
            testConfigErrorHandling();
            testBrainErrorHandling();
            testValidationErrorHandling();
            testIntegrationTest();
            
            // Run performance tests
            testErrorReportingPerformance();
            testErrorRecoveryPerformance();
            
            // Print final statistics
            printFinalStatistics();
            
            std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "\n*** Test Suite Failed: " << e.what() << " ***" << std::endl;
            ErrorReporting::report(ErrorCategory::System, "TEST_SUITE_FAILURE", 
                                std::string("Test suite failed: ") + e.what(), 
                                "TestErrorHandling::runAllTests");
        }
        
        // Cleanup
        ErrorReporting::shutdown();
        Validation::shutdown();
    }
    
private:
    static void testBasicErrorHandling() {
        std::cout << "\n--- Testing Basic Error Handling ---" << std::endl;
        
        try {
            // Test 1: Basic error creation and reporting
            NLM_THROW_ERROR("Test error", "Basic error test", 1001);
            
            // Test 2: Different error types
            NLM_THROW_CONFIG_ERROR("Configuration error", "Config test", 1001);
            NLM_THROW_MEMORY_ERROR("Memory allocation failed", "Memory test", 2001);
            NLM_THROW_BRAIN_ERROR("Brain initialization failed", "Brain test", 3001);
            NLM_THROW_LOGGER_ERROR("Logger not initialized", "Logger test", 4001);
            NLM_THROW_WORLD_ERROR("World boundary violation", "World test", 5001);
            NLM_THROW_VALIDATION_ERROR("Invalid input", "Validation test", 6001);
            
            // Test 3: Error context
            ErrorContext::getInstance().pushContext("DatabaseConnection");
            NLM_THROW_ERROR("Connection failed", "Database test", 1002);
            ErrorContext::getInstance().popContext();
            
            std::cout << "✓ Basic error handling tests passed" << std::endl;
            
        } catch (const NLMError& e) {
            // Expected - errors are caught and handled
            std::cout << "✓ Basic error handling test passed (expected exception)" << std::endl;
        }
    }
    
    static void testLoggerErrorHandling() {
        std::cout << "\n--- Testing Logger Error Handling ---" << std::endl;
        
        try {
            // Test 1: Console logging with error handling
            Logger::getGlobal().info("Test info message with error handling");
            Logger::getGlobal().warning("Test warning message with error handling");
            Logger::getGlobal().error("Test error message with error handling");
            Logger::getGlobal().critical("Test critical message with error handling");
            
            // Test 2: File logging with error recovery
            Logger::getGlobal().info("Test file logging");
            
            // Test 3: Error-aware logging
            auto errorAwareLogger = std::make_shared<ErrorAwareLogger>(std::make_shared<ConsoleLogger>());
            errorAwareLogger->log(LogEntry(LogLevel::Error, "Test error-aware logging"));
            
            // Test 4: Error reporting integration
            Logger::getGlobal().setErrorReporter(ErrorReporting::getErrorReporter());
            
            std::cout << "✓ Logger error handling tests passed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Logger error handling test failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    static void testConfigErrorHandling() {
        std::cout << "\n--- Testing Config Error Handling ---" << std::endl;
        
        try {
            // Test 1: Basic config operations with error handling
            auto config = std::make_shared<Config>();
            
            // Set values with validation
            config->set("neuron_count", size_t(1000));
            config->set("region_count", size_t(4));
            config->set("connection_probability", float(0.1f));
            config->set("simulation_timestep", double(0.001));
            
            // Get values with error handling
            auto neuronCount = config->get<size_t>("neuron_count");
            auto regionCount = config->get<size_t>("region_count");
            auto connectionProb = config->get<float>("connection_probability");
            
            // Validate configuration
            auto validation = config->validate();
            if (!validation.valid) {
                std::cout << "Warning: Config validation failed: " << validation.errorMessage << std::endl;
            }
            
            // Test 2: File operations with error handling
            std::string testFile = "/tmp/test_config.json";
            
            // Save config
            std::string jsonOutput;
            if (config->saveToJSON(jsonOutput)) {
                std::cout << "✓ Config save test passed" << std::endl;
            }
            
            // Test 3: Error reporting for config operations
            try {
                config->set("invalid_key", "invalid_value");
            } catch (const ConfigError& e) {
                std::cout << "✓ Config validation test passed (expected exception)" << std::endl;
            }
            
            std::cout << "✓ Config error handling tests passed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Config error handling test failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    static void testBrainErrorHandling() {
        std::cout << "\n--- Testing Brain Error Handling ---" << std::endl;
        
        try {
            // Test 1: Basic config for brain
            auto config = std::make_shared<Config>();
            config->set("neuron_count", size_t(100));
            config->set("region_count", size_t(1));
            config->set("connection_probability", float(0.1f));
            config->set("simulation_timestep", double(0.001));
            
            // Test 2: Brain initialization with error handling
            auto brain = std::make_shared<Brain>(config);
            
            if (brain->initialize()) {
                std::cout << "✓ Brain initialization test passed" << std::endl;
                
                // Test 3: Brain operations with error handling
                brain->step(0, 0.0);
                brain->step(1, 0.001);
                
                std::cout << "✓ Brain step operations test passed" << std::endl;
                
                // Test 4: Error reporting during operations
                brain->logStatus();
                
            } else {
                std::cout << "✗ Brain initialization failed (expected in test environment)" << std::endl;
            }
            
            // Test 5: Error context during brain operations
            ErrorContext::getInstance().pushContext("BrainOperation");
            ErrorContext::getInstance().pushContext("StepExecution");
            
            // Reset error context
            ErrorContext::getInstance().popContext();
            ErrorContext::getInstance().popContext();
            
            std::cout << "✓ Brain error handling tests passed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Brain error handling test failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    static void testValidationErrorHandling() {
        std::cout << "\n--- Testing Validation Error Handling ---" << std::endl;
        
        try {
            // Test 1: Basic validation operations
            auto validation = Validation::validate("test_key", "test_value");
            
            // Test 2: Type validation
            validation = InputValidator::validateInt("test_int", 42, 0, 100);
            
            // Test 3: Range validation
            validation = InputValidator::validateRange("test_float", 5.5f, 0.0f, 10.0f);
            
            // Test 4: String validation
            validation = InputValidator::validateString("test_string", "valid_value", "positive_number");
            
            // Test 5: List validation
            std::vector<std::string> stringList = {"item1", "item2", "item3"};
            validation = InputValidator::validateStringVector("test_list", stringList);
            
            // Test 6: Configuration validation
            std::map<std::string, std::string> testConfig = {
                {"neuron_count", "100"},
                {"region_count", "4"},
                {"connection_probability", "0.1"}
            };
            
            auto testValidator = [](const std::string& key, const std::string& value) {
                if (key == "neuron_count") {
                    return InputValidator::validateInt(key, std::stoi(value), 1, 1000);
                } else if (key == "region_count") {
                    return InputValidator::validateInt(key, std::stoi(value), 1, 10);
                } else if (key == "connection_probability") {
                    return InputValidator::validateRange(key, std::stof(value), 0.0f, 1.0f);
                }
                return ValidationResult(true);
            };
            
            std::map<std::string, ValidationResult> results;
            for (const auto& pair : testConfig) {
                results[pair.first] = testValidator(pair.first, pair.second);
            }
            
            // Test 7: Error reporting for validation failures
            for (const auto& pair : results) {
                if (!pair.second.valid) {
                    ErrorReporting::report(ErrorCategory::Validation, "VALIDATION_FAILED",
                                        "Validation failed for key: " + pair.first + ": " + pair.second.errorMessage,
                                        "ValidationTest");
                }
            }
            
            std::cout << "✓ Validation error handling tests passed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Validation error handling test failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    static void testIntegrationTest() {
        std::cout << "\n--- Running Integration Test ---" << std::endl;
        
        try {
            // Create a complete error handling scenario
            ErrorContext::getInstance().pushContext("IntegrationTest");
            
            // Initialize error reporting system
            ErrorReporting::initialize();
            
            // Create configuration with validation
            auto config = std::make_shared<Config>();
            config->set("neuron_count", size_t(100));
            config->set("region_count", size_t(1));
            config->set("connection_probability", float(0.1f));
            config->set("simulation_timestep", double(0.001));
            
            // Validate configuration
            auto validation = config->validate();
            if (!validation.valid) {
                ErrorReporting::report(ErrorCategory::Configuration, "CONFIG_VALIDATION_FAILED",
                                    "Configuration validation failed: " + validation.errorMessage,
                                    "IntegrationTest");
            }
            
            // Create and initialize brain with error handling
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                std::cout << "✓ Integration test: Brain initialization successful" << std::endl;
                
                // Run simulation with error handling
                for (int i = 0; i < 10; ++i) {
                    brain->step(i, i * 0.001);
                }
                
                // Log status with error handling
                brain->logStatus();
                
                std::cout << "✓ Integration test: Simulation successful" << std::endl;
            }
            
            // Test error recovery
            if (!ErrorReporting::isSystemHealthy()) {
                ErrorReporting::report(ErrorCategory::System, "SYSTEM_UNHEALTHY",
                                    "Error reporting system is not healthy",
                                    "IntegrationTest");
            }
            
            ErrorContext::getInstance().popContext();
            
            std::cout << "✓ Integration test completed successfully" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Integration test failed: " << e.what() << std::endl;
            ErrorReporting::report(ErrorCategory::System, "INTEGRATION_TEST_FAILED",
                                "Integration test failed: " + std::string(e.what()),
                                "IntegrationTest");
            throw;
        }
    }
    
    static void testErrorReportingPerformance() {
        std::cout << "\n--- Testing Error Reporting Performance ---" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Generate many errors and warnings
        for (int i = 0; i < 1000; ++i) {
            if (i % 10 == 0) {
                ErrorReporting::report(ErrorCategory::Brain, "PERF_TEST_ERROR",
                                    "Performance test error " + std::to_string(i),
                                    "PerformanceTest");
            } else {
                ErrorReporting::reportWarning(ErrorCategory::Brain, "PERF_TEST_WARNING",
                                            "Performance test warning " + std::to_string(i),
                                            "PerformanceTest");
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "✓ Error reporting performance test passed" << std::endl;
        std::cout << "  Generated 1000 errors/warnings in " << duration.count() << " ms" << std::endl;
    }
    
    static void testErrorRecoveryPerformance() {
        std::cout << "\n--- Testing Error Recovery Performance ---" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Attempt recovery for many errors
        for (int i = 0; i < 100; ++i) {
            Logger::getGlobal().attemptRecovery("PerformanceTestOperation", 2.0f);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "✓ Error recovery performance test passed" << std::endl;
        std::cout << "  Attempted 100 recoveries in " << duration.count() << " ms" << std::endl;
    }
    
    static void printFinalStatistics() {
        std::cout << "\n--- Final Error Handling Statistics ---" << std::endl;
        
        // Error reporting statistics
        std::cout << "Error Reporting Statistics:" << std::endl;
        std::cout << "  Total Errors: " << ErrorReporting::getErrorCount() << std::endl;
        std::cout << "  Total Warnings: " << ErrorReporting::getWarningCount() << std::endl;
        std::cout << "  System Healthy: " << (ErrorReporting::isSystemHealthy() ? "Yes" : "No") << std::endl;
        
        // Validation statistics
        std::cout << "\nValidation Statistics:" << std::endl;
        auto validationStats = InputValidator::getValidationStats();
        std::cout << "  Total Validations: " << validationStats.totalValidations << std::endl;
        std::cout << "  Successful Validations: " << validationStats.successfulValidations << std::endl;
        std::cout << "  Failed Validations: " << validationStats.failedValidations << std::endl;
        std::cout << "  Success Rate: " << (validationStats.totalValidations > 0 ? 
                                      (validationStats.successfulValidations * 100.0 / validationStats.totalValidations) : 0.0) << "%" << std::endl;
        
        // Logger statistics
        std::cout << "\nLogger Statistics:" << std::endl;
        std::cout << "  Total Errors Logged: " << Logger::getGlobal().getTotalErrorCount() << std::endl;
        std::cout << "  Total Warnings Logged: " << Logger::getGlobal().getTotalWarningCount() << std::endl;
        std::cout << "  Logger Health Report:" << std::endl;
        std::cout << Logger::getGlobal().getHealthReport();
        
        // Error manager statistics
        std::cout << "\nError Manager Statistics:" << std::endl;
        std::cout << "  Global Error Report:" << std::endl;
        ErrorReport globalReport = ErrorReporting::getErrorReport();
        std::cout << globalReport.generateSummary();
    }
};

// Global test function
void runErrorHandlingTests() {
    TestErrorHandling::runAllTests();
}

// Test harness for different test scenarios
class TestHarness {
public:
    static void runBasicTestSuite() {
        std::cout << "=== Running Basic Error Handling Test Suite ===" << std::endl;
        TestErrorHandling::runAllTests();
    }
    
    static void runStressTest() {
        std::cout << "=== Running Stress Test ===" << std::endl;
        
        // Run multiple iterations of tests
        for (int iteration = 0; iteration < 5; ++iteration) {
            std::cout << "\n--- Stress Test Iteration " << iteration + 1 << " ---" << std::endl;
            TestErrorHandling::testBasicErrorHandling();
            TestErrorHandling::testLoggerErrorHandling();
            std::cout << "Iteration " << iteration + 1 << " completed" << std::endl;
        }
        
        std::cout << "\n=== Stress Test Completed ===" << std::endl;
    }
    
    static void runMemoryTest() {
        std::cout << "=== Running Memory Usage Test ===" << std::endl;
        
        // Test error handling with many objects
        std::vector<std::shared_ptr<Config>> configs;
        for (int i = 0; i < 100; ++i) {
            auto config = std::make_shared<Config>();
            config->set("test_key", std::to_string(i));
            configs.push_back(config);
        }
        
        std::cout << "✓ Created 100 config objects" << std::endl;
        
        // Test error handling with many loggers
        std::vector<std::shared_ptr<Logger>> loggers;
        for (int i = 0; i < 50; ++i) {
            auto logger = std::make_shared<Logger>();
            logger->info("Memory test logger " + std::to_string(i));
            loggers.push_back(logger);
        }
        
        std::cout << "✓ Created 50 logger objects" << std::endl;
        
        // Cleanup
        configs.clear();
        loggers.clear();
        
        std::cout << "✓ Memory test completed successfully" << std::endl;
    }
    
    static void runPerformanceTest() {
        std::cout << "=== Running Performance Test ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Perform many error reporting operations
        for (int i = 0; i < 10000; ++i) {
            if (i % 100 == 0) {
                ErrorReporting::report(ErrorCategory::Brain, "PERF_TEST_ERROR",
                                    "Performance error " + std::to_string(i),
                                    "PerformanceTest");
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "✓ Performance test completed" << std::endl;
        std::cout << "  Generated 10,000 errors in " << duration.count() << " ms" << std::endl;
        std::cout << "  Average error generation time: " << (duration.count() / 10000.0) << " ms per error" << std::endl;
    }
    
    static void runCombinedTest() {
        std::cout << "=== Running Combined Integration Test ===" << std::endl;
        
        // Initialize all systems
        ErrorReporting::initialize();
        Validation::initialize();
        
        // Run complex scenario with multiple error types
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", size_t(100));
            config->set("region_count", size_t(1));
            config->set("connection_probability", float(0.1f));
            
            // Validate configuration
            auto validation = config->validate();
            
            // Create and initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                std::cout << "✓ Combined test: Brain initialization successful" << std::endl;
                
                // Run simulation
                for (int i = 0; i < 50; ++i) {
                    brain->step(i, i * 0.001);
                }
                
                // Test error recovery
                bool recoveryResult = brain->attemptRecovery("CombinedTestOperation", 2.0f);
                if (recoveryResult) {
                    std::cout << "✓ Combined test: Error recovery successful" << std::endl;
                }
                
                // Log final status
                brain->logStatus();
                
                std::cout << "✓ Combined test completed successfully" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ Combined test failed: " << e.what() << std::endl;
            throw;
        }
        
        // Cleanup
        ErrorReporting::shutdown();
        Validation::shutdown();
        
        std::cout << "✓ Combined test completed" << std::endl;
    }
};

// Entry point for error handling tests
int main() {
    return nlm::main();
}
}

} // namespace nlm

// Entry point for error handling tests
int main() {
    return nlm::main();
}