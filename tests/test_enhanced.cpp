// Enhanced NLM test main using the new test framework
// Includes tests for configuration system, error handling, and core functionality

#include "TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_enhanced_config {
    void testEnhancedConfigCreation() {
        PerformanceTimer timer("testEnhancedConfigCreation");
        
        nlm::Config config;
        
        ASSERT_TRUE(!config.has("test_key"), "New config should not have test_key");
        
        std::cout << "    ✓ Enhanced config creation" << std::endl;
    }
    
    void testEnhancedConfigSetGet() {
        PerformanceTimer timer("testEnhancedConfigSetGet");
        
        nlm::Config config;
        
        // Test all basic types
        ASSERT_TRUE(config.set("int_value", 42), "Set int value");
        ASSERT_TRUE(config.set("int64_value", static_cast<int64_t>(123456789012), "Set int64 value");
        ASSERT_TRUE(config.set("double_value", 3.14159), "Set double value");
        ASSERT_TRUE(config.set("bool_value", true), "Set bool value");
        ASSERT_TRUE(config.set("string_value", "test string"), "Set string value");
        
        ASSERT_TRUE(config.has("int_value"), "Has int_value");
        ASSERT_EQUAL(config.getIntOr("int_value", -1), 42, "Get int_value");
        ASSERT_EQUAL(config.getInt64Or("int64_value", -1), 123456789012, "Get int64_value");
        ASSERT_DOUBLE_EQUAL(config.getDoubleOr("double_value", -1.0), 3.14159, 0.00001, "Get double_value");
        ASSERT_EQUAL(config.getBoolOr("bool_value", false), true, "Get bool_value");
        ASSERT_EQUAL(config.getStringOr("string_value", ""), "test string", "Get string_value");
        
        std::cout << "    ✓ Enhanced config set/get" << std::endl;
    }
    
    void testEnhancedConfigValidation() {
        PerformanceTimer timer("testEnhancedConfigValidation");
        
        nlm::Config config;
        
        // Set some values
        config.set("neuron_count", 1000);
        config.set("connection_probability", 0.1);
        config.set("simulation_timestep", 0.001);
        
        // Validate configuration
        auto validation = config.validate();
        ASSERT_TRUE(validation.valid, "Configuration should be valid");
        
        std::cout << "    ✓ Enhanced config validation" << std::endl;
    }
    
    void testEnhancedConfigDefaults() {
        PerformanceTimer timer("testEnhancedConfigDefaults");
        
        nlm::Config config;
        
        // Check that defaults are applied
        ASSERT_TRUE(config.has("neuron_count"), "Should have default neuron_count");
        ASSERT_EQUAL(config.getIntOr("neuron_count", 0), 1000, "Default neuron_count should be 1000");
        
        ASSERT_TRUE(config.has("connection_probability"), "Should have default connection_probability");
        ASSERT_DOUBLE_EQUAL(config.getDoubleOr("connection_probability", -1.0), 0.1, 0.00001, "Default connection_probability should be 0.1");
        
        std::cout << "    ✓ Enhanced config defaults" << std::endl;
    }
    
    void testEnhancedConfigFileIO() {
        PerformanceTimer timer("testEnhancedConfigFileIO");
        
        std::string testFile = "test_config.cfg";
        
        // Create and save config
        {
            nlm::Config config;
            config.set("neuron_count", 2000);
            config.set("connection_probability", 0.2);
            config.set("test_string", "hello world");
            
            bool saved = config.saveToFile(testFile);
            ASSERT_TRUE(saved, "Save config to file");
        }
        
        // Load config
        nlm::Config config;
        bool loaded = config.loadFromFile(testFile);
        ASSERT_TRUE(loaded, "Load config from file");
        
        ASSERT_TRUE(config.has("neuron_count"), "Loaded config should have neuron_count");
        ASSERT_EQUAL(config.getIntOr("neuron_count", 0), 2000, "Loaded neuron_count");
        
        ASSERT_TRUE(config.has("connection_probability"), "Loaded config should have connection_probability");
        ASSERT_DOUBLE_EQUAL(config.getDoubleOr("connection_probability", -1.0), 0.2, 0.00001, "Loaded connection_probability");
        
        ASSERT_TRUE(config.has("test_string"), "Loaded config should have test_string");
        ASSERT_EQUAL(config.getStringOr("test_string", ""), "hello world", "Loaded test_string");
        
        // Clean up
        std::remove(testFile.c_str());
        
        std::cout << "    ✓ Enhanced config file I/O" << std::endl;
    }
    
    void testEnhancedConfigErrorHandling() {
        PerformanceTimer timer("testEnhancedConfigErrorHandling");
        
        nlm::Config config;
        
        // Test setting invalid types
        ASSERT_TRUE(config.set("int_value", 42), "Set int_value");
        ASSERT_TRUE(config.set("string_value", "hello"), "Set string_value");
        
        // This should not crash - type validation happens at retrieval
        auto intVal = config.getInt("string_value");
        ASSERT_TRUE(!intVal.has_value(), "Cannot get int from string key");
        
        auto stringVal = config.getString("int_value");
        ASSERT_TRUE(!stringVal.has_value(), "Cannot get string from int key");
        
        std::cout << "    ✓ Enhanced config error handling" << std::endl;
    }
    
    void runAll() {
        TestSuite suite("Enhanced Configuration Tests");
        
        suite.addTest("Config Creation", testEnhancedConfigCreation);
        suite.addTest("Config Set/Get", testEnhancedConfigSetGet);
        suite.addTest("Config Validation", testEnhancedConfigValidation);
        suite.addTest("Config Defaults", testEnhancedConfigDefaults);
        suite.addTest("Config File I/O", testEnhancedConfigFileIO);
        suite.addTest("Config Error Handling", testEnhancedConfigErrorHandling);
        
        suite.runAll();
    }
}

namespace test_error_handling {
    void testErrorLoggerCreation() {
        PerformanceTimer timer("testErrorLoggerCreation");
        
        nlm::ErrorLogger logger;
        nlm::ErrorStats stats = logger.getStats();
        ASSERT_EQUAL(stats.totalErrors, 0, "Should start with 0 errors");
        
        std::cout << "    ✓ Error logger creation" << std::endl;
    }
    
    void testErrorLogging() {
        PerformanceTimer timer("testErrorLogging");
        
        nlm::ErrorLogger logger;
        
        // Log an error
        logger.logError("Test error message", nlm::ErrorCategory::Configuration, "test_context");
        
        nlm::ErrorStats stats = logger.getStats();
        ASSERT_EQUAL(stats.totalErrors, 1, "Should have 1 error");
        
        auto recentErrors = logger.getRecentErrors();
        ASSERT_EQUAL(recentErrors.size(), 1, "Should have 1 recent error");
        
        std::cout << "    ✓ Error logging" << std::endl;
    }
    
    void testExceptionCreation() {
        PerformanceTimer timer("testExceptionCreation");
        
        nlm::ConfigurationError configError("Test config error", "test_context");
        ASSERT_TRUE(configError.getCategory() == nlm::ErrorCategory::Configuration, "Should be config error");
        ASSERT_TRUE(configError.getContext() == "test_context", "Should have test_context");
        
        nlm::SimulationError simError("Test simulation error");
        ASSERT_TRUE(simError.getCategory() == nlm::ErrorCategory::Simulation, "Should be simulation error");
        
        std::cout << "    ✓ Exception creation" << std::endl;
    }
    
    void testErrorStatistics() {
        PerformanceTimer timer("testErrorStatistics");
        
        nlm::ErrorLogger logger;
        nlm::ErrorStats initialStats = logger.getStats();
        ASSERT_EQUAL(initialStats.totalErrors, 0, "Initial stats should have 0 errors");
        
        // Log some errors
        logger.logError("Config error", nlm::ErrorCategory::Configuration);
        logger.logError("Simulation error", nlm::ErrorCategory::Simulation);
        logger.logError("Memory error", nlm::ErrorCategory::Memory);
        
        nlm::ErrorStats stats = logger.getStats();
        ASSERT_EQUAL(stats.totalErrors, 3, "Should have 3 errors");
        ASSERT_EQUAL(stats.errorsByCategory[static_cast<size_t>(nlm::ErrorCategory::Configuration)], 1, "1 config error");
        ASSERT_EQUAL(stats.errorsByCategory[static_cast<size_t>(nlm::ErrorCategory::Simulation)], 1, "1 simulation error");
        ASSERT_EQUAL(stats.errorsByCategory[static_cast<size_t>(nlm::ErrorCategory::Memory)], 1, "1 memory error");
        
        std::cout << "    ✓ Error statistics" << std::endl;
    }
    
    void runAll() {
        TestSuite suite("Error Handling Tests");
        
        suite.addTest("Error Logger Creation", testErrorLoggerCreation);
        suite.addTest("Error Logging", testErrorLogging);
        suite.addTest("Exception Creation", testExceptionCreation);
        suite.addTest("Error Statistics", testErrorStatistics);
        
        suite.runAll();
    }
}

namespace test_integration {
    void testConfigurationIntegration() {
        PerformanceTimer timer("testConfigurationIntegration");
        
        nlm::Config config;
        
        // Test integration with existing functionality
        config.set("neuron_count", 500);
        config.set("region_count", 2);
        config.set("connection_probability", 0.05);
        
        ASSERT_EQUAL(config.getIntOr("neuron_count", 0), 500, "Neuron count integration");
        ASSERT_EQUAL(config.getIntOr("region_count", 0), 2, "Region count integration");
        ASSERT_DOUBLE_EQUAL(config.getDoubleOr("connection_probability", -1.0), 0.05, 0.00001, "Connection probability integration");
        
        std::cout << "    ✓ Configuration integration" << std::endl;
    }
    
    void testErrorHandlingIntegration() {
        PerformanceTimer timer("testErrorHandlingIntegration");
        
        nlm::ErrorLogger errorLogger;
        
        // Test error handling integration
        errorLogger.logError("Integration test error", nlm::ErrorCategory::User, "integration_test");
        
        nlm::ErrorStats stats = errorLogger.getStats();
        ASSERT_EQUAL(stats.totalErrors, 1, "Error logger should have 1 error");
        
        std::cout << "    ✓ Error handling integration" << std::endl;
    }
    
    void runAll() {
        TestSuite suite("Integration Tests");
        
        suite.addTest("Configuration Integration", testConfigurationIntegration);
        suite.addTest("Error Handling Integration", testErrorHandlingIntegration);
        
        suite.runAll();
    }
}

int main() {
    std::cout << "=== Enhanced NLM Tests ===" << std::endl;
    std::cout << "Testing new configuration system and error handling:" << std::endl;
    std::cout << "  - Enhanced configuration validation" << std::endl;
    std::cout << "  - Error handling and exception system" << std::endl;
    std::cout << "  - Integration testing" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        std::cout << "Running Enhanced Config tests..." << std::endl;
        test_enhanced_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Error Handling tests..." << std::endl;
        test_error_handling::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Integration tests..." << std::endl;
        test_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Enhanced Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Enhanced Tests FAILED ===" << std::endl;
        return 1;
    }
}
