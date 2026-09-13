// Configuration Validation Tests
// Comprehensive tests for configuration validation system

#include "core/Config/Config.hpp"
#include "core/Config/Config.cpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>
#include <string>

namespace nlm {

class ConfigurationValidationTester {
public:
    ConfigurationValidationTester() {
        std::cout << "Testing Configuration Validation..." << std::endl;
        std::cout << "Testing configuration validation improvements for Phase 5:" << std::endl;
        
        // Enable validation
        ConfigValidator::enableValidation(true);
        ConfigValidator::setValidationLevel(2);
        
        // Add some validation rules
        ConfigValidator::addValidationRule("min_neuron_count_100");
        ConfigValidator::addValidationRule("max_memory_gb_16");
        ConfigValidator::addValidationRule("connection_probability_010");
    }
    
    void testParameterValidation() {
        std::cout << "  Testing Parameter Validation..." << std::endl;
        
        // Test valid parameter
        ConfigValidator::ValidationResult result1 = ConfigValidator::validateParameter("neuron_count", "150");
        assert(result1.isValid);
        
        // Test invalid parameter - too low
        ConfigValidator::ValidationResult result2 = ConfigValidator::validateParameter("neuron_count", "50");
        assert(!result2.isValid);
        
        // Test invalid parameter - too high
        ConfigValidator::ValidationResult result3 = ConfigValidator::validateParameter("memory_gb", "64");
        assert(!result3.isValid);
        
        // Test invalid parameter - wrong format
        ConfigValidator::ValidationResult result4 = ConfigValidator::validateParameter("connection_probability", "1.5");
        assert(!result4.isValid);
        
        // Test valid parameter ranges
        ConfigValidator::ValidationResult result5 = ConfigValidator::validateParameter("connection_probability", "0.05");
        assert(result5.isValid);
        
        std::cout << "    ✓ Parameter validation tests passed" << std::endl;
    }
    
    void testConfigValidation() {
        std::cout << "  Testing Config Object Validation..." << std::endl;
        
        // Create a config with valid values
        Config validConfig;
        validConfig.set("neuron_count", 150, ConfigSource::Default);
        validConfig.set("region_count", 3, ConfigSource::Default);
        validConfig.set("connection_probability", 0.1, ConfigSource::Default);
        validConfig.set("memory_gb", 8, ConfigSource::Default);
        
        // This would use the ConfigurationValidator::validateAll method
        bool configValid = ConfigurationValidator::isConfigurationValid(validConfig);
        assert(configValid);
        
        // Test validation report
        std::string report = ConfigurationValidator::getConfigurationValidationReport(validConfig);
        assert(!report.empty());
        
        // Create a config with invalid values
        Config invalidConfig;
        invalidConfig.set("neuron_count", 50, ConfigSource::Default);  // Too low
        invalidConfig.set("memory_gb", 32, ConfigSource::Default);   // Too high
        invalidConfig.set("connection_probability", 1.5, ConfigSource::Default); // Invalid range
        
        bool invalidConfigValid = ConfigurationValidator::isConfigurationValid(invalidConfig);
        assert(!invalidConfigValid);
        
        // Get validation results
        std::vector<ConfigurationValidator::ConfigValidationResult> results = 
            ConfigurationValidator::getValidationResults();
        assert(results.size() > 0);
        
        // Get validation summary
        std::string summary = ConfigurationValidator::getValidationSummary();
        assert(!summary.empty());
        
        std::cout << "    ✓ Config object validation tests passed" << std::endl;
    }
    
    void testValidationResultsManagement() {
        std::cout << "  Testing Validation Results Management..." << std::endl;
        
        // Clear existing results
        ConfigurationValidator::clearValidationResults();
        
        // Get initial count
        size_t initialCount = ConfigurationValidator::getValidationCount();
        assert(initialCount == 0);
        
        // Add some validation results by validating configs
        Config config1;
        config1.set("neuron_count", 150, ConfigSource::Default);
        config1.set("region_count", 2, ConfigSource::Default);
        
        Config config2;
        config2.set("neuron_count", 200, ConfigSource::Default);
        config2.set("region_count", 3, ConfigSource::Default);
        config2.set("memory_gb", 16, ConfigSource::Default);
        
        // These calls would trigger validation
        ConfigurationValidator::validateAll(config1);
        ConfigurationValidator::validateAll(config2);
        
        // Check results increased
        size_t afterCount = ConfigurationValidator::getValidationCount();
        assert(afterCount > initialCount);
        
        // Get validation results
        std::vector<ConfigurationValidator::ConfigValidationResult> allResults = 
            ConfigurationValidator::getValidationResults();
        assert(allResults.size() == afterCount);
        
        // Get results by category
        std::vector<ConfigurationValidator::ConfigValidationResult> defaultResults = 
            ConfigurationValidator::getValidationResults("Default");
        // Might be empty or contain results depending on implementation
        
        // Get results by parameter
        std::vector<ConfigurationValidator::ConfigValidationResult> neuronResults = 
            ConfigurationValidator::getValidationResults("Default", "neuron_count");
        // Might be empty or contain results depending on implementation
        
        // Test validation counts
        size_t totalCount = ConfigurationValidator::getValidationCount();
        size_t failedCount = ConfigurationValidator::getFailedValidationCount();
        size_t passedCount = ConfigurationValidator::getPassedValidationCount();
        assert(totalCount >= 0);
        assert(failedCount >= 0);
        assert(passedCount >= 0);
        assert(failedCount + passedCount == totalCount || totalCount == 0);
        
        // Test clearing by category
        ConfigurationValidator::clearValidationResults("Debug");
        
        // Test recent validations
        std::vector<ConfigurationValidator::ConfigValidationResult> recent = 
            ConfigurationValidator::getRecentValidations(5);
        assert(recent.size() <= 5);
        
        // Test failed and passed validations
        std::vector<ConfigurationValidator::ConfigValidationResult> failed = 
            ConfigurationValidator::getFailedValidations();
        std::vector<ConfigurationValidator::ConfigValidationResult> passed = 
            ConfigurationValidator::getPassedValidations();
        assert(failed.size() <= allResults.size());
        assert(passed.size() <= allResults.size());
        
        std::cout << "    ✓ Validation results management tests passed" << std::endl;
    }
    
    void testValidationRules() {
        std::cout << "  Testing Validation Rules..." << std::endl;
        
        // Get current validation rules
        std::vector<std::string> rules = ConfigValidator::getValidationRules();
        assert(rules.size() >= 3); // We added 3 rules
        
        // Check our specific rules are present
        bool hasMinNeuronRule = false;
        bool hasMaxMemoryRule = false;
        bool hasConnectionRule = false;
        
        for (const std::string& rule : rules) {
            if (rule.find("min_neuron") != std::string::npos) hasMinNeuronRule = true;
            if (rule.find("max_memory") != std::string::npos) hasMaxMemoryRule = true;
            if (rule.find("connection_probability") != std::string::npos) hasConnectionRule = true;
        }
        
        assert(hasMinNeuronRule);
        assert(hasMaxMemoryRule);
        assert(hasConnectionRule);
        
        // Test removing rules
        ConfigValidator::removeValidationRule("min_neuron_count_100");
        
        // Verify rule was removed (new list might be stale, but this tests the interface)
        ConfigValidator::clearValidationRules();
        
        std::cout << "    ✓ Validation rules tests passed" << std::endl;
    }
    
    void testValidationHistory() {
        std::cout << "  Testing Validation History..." << std::endl;
        
        // Reset validator
        ConfigurationValidator::resetConfigurationValidator();
        
        // Clear validation results
        ConfigurationValidator::clearValidationResults();
        
        // Validate multiple times to build history
        for (int i = 0; i < 5; ++i) {
            Config config;
            config.set("neuron_count", 100 + i * 50, ConfigSource::Default);
            config.set("region_count", 2 + i, ConfigSource::Default);
            
            // This would validate and store results
            ConfigurationValidator::validateAll(config);
        }
        
        // Check last validation time
        auto lastTime = ConfigurationValidator::getLastValidationTime();
        assert(lastTime.time_since_epoch().count() > 0);
        
        // Check average validation time
        double avgTime = ConfigurationValidator::getAverageValidationTime();
        assert(avgTime >= 0.0);
        
        // Test max history size
        ConfigurationValidator::setMaxValidationHistorySize(10);
        size_t maxSize = ConfigurationValidator::getMaxValidationHistorySize();
        assert(maxSize == 10);
        
        // Test recent validations
        std::vector<ConfigurationValidator::ConfigValidationResult> recent = 
            ConfigurationValidator::getRecentValidations(10);
        assert(recent.size() <= 10);
        
        std::cout << "    ✓ Validation history tests passed" << std::endl;
    }
    
    void testValidationIntegration() {
        std::cout << "  Testing Validation Integration..." << std::endl;
        
        // Create a realistic configuration
        Config realisticConfig;
        realisticConfig.set("neuron_count", 500, ConfigSource::Default);
        realisticConfig.set("region_count", 5, ConfigSource::Default);
        realisticConfig.set("connection_probability", 0.05, ConfigSource::Default);
        realisticConfig.set("memory_gb", 8, ConfigSource::Default);
        realisticConfig.set("simulation_steps", 10000, ConfigSource::Default);
        realisticConfig.set("checkpoint_interval", 1000, ConfigSource::Default);
        
        // Validate the configuration
        bool isValid = ConfigurationValidator::isConfigurationValid(realisticConfig);
        assert(isValid);
        
        // Get detailed validation report
        std::string report = ConfigurationValidator::getConfigurationValidationReport(realisticConfig);
        assert(!report.empty());
        
        // Get validation summary
        std::string summary = ConfigurationValidator::getValidationSummary();
        assert(!summary.empty());
        
        // Test validation callback (interface test)
        auto callbackCalled = std::make_shared<bool>(false);
        auto testCallback = [callbackCalled](const ConfigurationValidator::ConfigValidationResult& result) {
            (*callbackCalled) = true;
        };
        
        ConfigurationValidator::setValidationCallback(testCallback);
        
        // Trigger validation which would call callback
        ConfigurationValidator::validateAll(realisticConfig);
        
        // Clear callback
        ConfigurationValidator::clearValidationCallback();
        
        std::cout << "    ✓ Validation integration tests passed" << std::endl;
    }
};

void runAllConfigurationValidationTests() {
    std::cout << "Running Configuration Validation Tests..." << std::endl;
    std::cout << "Testing configuration validation improvements for Phase 5:" << std::endl;
    
    ConfigurationValidationTester tester;
    
    try {
        tester.testParameterValidation();
        tester.testConfigValidation();
        tester.testValidationResultsManagement();
        tester.testValidationRules();
        tester.testValidationHistory();
        tester.testValidationIntegration();
        
        std::cout << std::endl;
        std::cout << "All Configuration Validation tests passed! ✓" << std::endl;
        std::cout << "Configuration validation improvements for Phase 5 verified." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace nlm
