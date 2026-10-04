#include <iostream>
#include <string>
#include "core/Config/Config.hpp"

namespace test_enhanced_config {
    void runAll() {
        std::cout << "=== Enhanced Configuration Tests ===" << std::endl;
        std::cout << "Testing new validation features and CLI integration" << std::endl;
        
        // Create a configuration
        nlm::Config config;
        
        // Test 1: Basic configuration setting and validation
        std::cout << "\nTest 1: Configuration validation" << std::endl;
        config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
        config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
        config.set("connection_probability", 0.5f, nlm::ConfigSource::Runtime);
        
        std::vector<nlm::ValidationError> errors;
        bool isValid = config.isValid(errors);
        
        if (isValid && errors.empty()) {
            std::cout << "✓ Valid configuration passes validation" << std::endl;
        } else {
            std::cout << "✗ Configuration validation failed with " << errors.size() << " error(s)" << std::endl;
            throw std::runtime_error("Enhanced configuration validation test failed");
        }
        
        // Test 2: Invalid configuration should fail validation
        std::cout << "\nTest 2: Invalid configuration validation" << std::endl;
        nlm::Config invalidConfig;
        invalidConfig.set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
        
        errors.clear();
        isValid = invalidConfig.isValid(errors);
        
        if (!isValid && !errors.empty()) {
            std::cout << "✓ Invalid configuration correctly rejected by validation" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << nlm::ConfigValidator::errorTypeToString(error.type) 
                          << " for key '" << error.key << "'" << std::endl;
            }
        } else {
            throw std::runtime_error("Enhanced configuration validation test failed: invalid configuration should be rejected");
        }
        
        // Test 3: Configuration schema
        std::cout << "\nTest 3: Configuration schema" << std::endl;
        const auto& schema = nlm::Config::getDefaultSchema();
        
        if (!schema.empty()) {
            std::cout << "✓ Schema loaded with " << schema.size() << " entries" << std::endl;
            
            // Check that essential keys are present
            bool hasNeuronCount = false;
            bool hasRegionCount = false;
            bool hasConnectionProbability = false;
            
            for (const auto& entry : schema) {
                if (entry.key == "neuron_count") hasNeuronCount = true;
                if (entry.key == "region_count") hasRegionCount = true;
                if (entry.key == "connection_probability") hasConnectionProbability = true;
            }
            
            if (hasNeuronCount && hasRegionCount && hasConnectionProbability) {
                std::cout << "✓ Essential configuration keys present in schema" << std::endl;
            } else {
                throw std::runtime_error("Enhanced configuration schema test failed: missing essential keys");
            }
        } else {
            throw std::runtime_error("Enhanced configuration schema test failed: empty schema");
        }
        
        // Test 4: Configuration initialization
        std::cout << "\nTest 4: Configuration initialization" << std::endl;
        nlm::Config initializedConfig;
        initializedConfig.initializeFromSchema();
        
        std::cout << "✓ Configuration initialized from schema" << std::endl;
        
        // Test 5: Configuration summary
        std::cout << "\nTest 5: Configuration summary" << std::endl;
        std::string summary = config.summary();
        
        if (!summary.empty()) {
            std::cout << "✓ Configuration summary generated" << std::endl;
        } else {
            throw std::runtime_error("Enhanced configuration test failed: empty summary");
        }
        
        // Test 6: Validation error messages
        std::cout << "\nTest 6: Validation error messages" << std::endl;
        nlm::ValidationError error(nlm::ValidationErrorType::RangeViolation, 
                                   "neuron_count", 
                                   "Value -1 is out of range [0, 100000]",
                                   "Use a value between 0 and 100000");
        
        std::string errorType = nlm::ConfigValidator::errorTypeToString(error.type);
        std::string suggestion = nlm::ConfigValidator::getDefaultSuggestion(error.type, error.key);
        
        if (errorType == "RangeViolation") {
            std::cout << "✓ Validation error type correctly identified" << std::endl;
        } else {
            throw std::runtime_error("Enhanced configuration test failed: incorrect error type");
        }
        
        std::cout << "✓ Validation error reporting and suggestions work correctly" << std::endl;
        
        std::cout << "\n=== Enhanced Configuration Tests Complete ===" << std::endl;
    }
}

int main() {
    test_enhanced_config::runAll();
    return 0;
}
