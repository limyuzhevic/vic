// Test for Config validation functionality
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

namespace test_config_validation {
    
    void testValidationBasic() {
        nlm::Config config;
        
        // Initially empty config should be valid
        assert(config.validate());
        
        // Set some valid values
        config.set("brain.neuron_count", static_cast<int64_t>(1000));
        config.set("brain.synapse_density", 0.1f);
        
        // Config should still be valid
        assert(config.validate());
        
        // Test validation details
        std::string details = config.validateWithDetails();
        std::cout << "    testValidationBasic: " << details << std::endl;
    }
    
    void testValidationRequiredKeys() {
        nlm::Config config;
        
        // Config without required keys should be invalid
        assert(!config.validate());
        
        // Add one required key
        config.set("brain.neuron_count", static_cast<int64_t>(500));
        
        // Still invalid (missing other required keys)
        assert(!config.validate());
        
        // Add all required keys
        config.set("brain.region_count", static_cast<int64_t>(2));
        config.set("brain.connection_probability", 0.15f);
        config.set("simulation.timestep", 0.001f);
        
        // Now should be valid
        assert(config.validate());
        
        std::cout << "    testValidationRequiredKeys passed" << std::endl;
    }
    
    void testValidationKeyFormat() {
        nlm::Config config;
        
        // Test valid keys
        assert(nlm::Config::isValidKey("neuron_count"));
        assert(nlm::Config::isValidKey("brain.neuron_count"));
        assert(nlm::Config::isValidKey("brain.synapse_density"));
        assert(nlm::Config::isValidKey("simulation.timestep"));
        
        // Test invalid keys
        assert(!nlm::Config::isValidKey(""));
        assert(!nlm::Config::isValidKey(".neuron_count"));
        assert(!nlm::Config::isValidKey("neuron_count."));
        assert(!nlm::Config::isValidKey("brain..neuron_count"));
        assert(!nlm::Config::isValidKey("brain.neuron count"));
        
        std::cout << "    testValidationKeyFormat passed" << std::endl;
    }
    
    void testValidationValueFormat() {
        nlm::Config config;
        
        // Test valid values
        assert(nlm::Config::isValidValue(42));
        assert(nlm::Config::isValidValue(42.0));
        assert(nlm::Config::isValidValue(true));
        assert(nlm::Config::isValidValue(std::string("test")));
        assert(nlm::Config::isValidValue(std::vector<int>({1, 2, 3})));
        assert(nlm::Config::isValidValue(std::vector<double>({1.0, 2.0, 3.0})));
        assert(nlm::Config::isValidValue(std::vector<std::string>({"a", "b", "c"})));
        
        std::cout << "    testValidationValueFormat passed" << std::endl;
    }
    
    void testValidationRange() {
        nlm::Config config;
        
        // Set validation rules
        nlm::Config::ValidationRules rules;
        rules.minNeuronCount = 1;
        rules.maxNeuronCount = 1000;
        rules.minSynapseDensity = 0.0;
        rules.maxSynapseDensity = 1.0;
        
        config.validationRules = rules;
        
        // Valid range
        config.clear();
        config.set("brain.neuron_count", static_cast<int64_t>(500));
        config.set("brain.synapse_density", 0.5f);
        assert(config.validate());
        
        // Out of range - neuron count too high
        config.clear();
        config.set("brain.neuron_count", static_cast<int64_t>(2000));
        config.set("brain.synapse_density", 0.5f);
        assert(!config.validate());
        
        // Out of range - synapse density too high
        config.clear();
        config.set("brain.neuron_count", static_cast<int64_t>(500));
        config.set("brain.synapse_density", 1.5f);
        assert(!config.validate());
        
        std::cout << "    testValidationRange passed" << std::endl;
    }
    
    void testValidationErrorMessages() {
        nlm::Config config;
        
        // Test validation error messages
        std::string error = nlm::Config::getValidationError("", 42);
        assert(!error.empty());
        
        error = nlm::Config::getValidationError("valid.key", 42);
        assert(error.empty()); // Should be valid
        
        error = nlm::Config::getValidationError("brain.neuron_count", std::vector<int>({1, 2, 3}));
        assert(!error.empty()); // Should be invalid
        
        std::cout << "    testValidationErrorMessages passed" << std::endl;
    }
    
    void runAll() {
        testValidationBasic();
        testValidationRequiredKeys();
        testValidationKeyFormat();
        testValidationValueFormat();
        testValidationRange();
        testValidationErrorMessages();
        
        std::cout << "All validation tests passed!" << std::endl;
    }
}

// Main function for the test
int main() {
    std::cout << "=== Config Validation Tests ===" << std::endl;
    test_config_validation::runAll();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
