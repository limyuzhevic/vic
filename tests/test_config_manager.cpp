// ConfigManager Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_config_manager {

void testConfigManagerCreation() {
    std::cout << "    testConfigManagerCreation starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    
    // Test singleton pattern
    assert(&manager == &nlm::ConfigManager::getInstance());
    
    // Test empty state
    std::vector<std::string> keys = manager.getRegisteredKeys();
    assert(keys.empty());
    
    std::cout << "    testConfigManagerCreation passed" << std::endl;
}

void testKeyRegistration() {
    std::cout << "    testKeyRegistration starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    manager.clear();
    
    // Register a key with metadata
    nlm::ConfigMetadata metadata;
    metadata.description = "Test key";
    metadata.category = "Test";
    metadata.required = true;
    metadata.defaultValue = "default";
    metadata.validator = [](const nlm::ConfigValue& v) { return std::holds_alternative<std::string>(v); };
    
    manager.registerKey("test_key", metadata);
    
    // Test that key was registered
    auto retrieved = manager.getMetadata("test_key");
    assert(retrieved.has_value());
    assert(retrieved->key == "test_key");
    assert(retrieved->description == "Test key");
    assert(retrieved->required == true);
    assert(retrieved->defaultValue == "default");
    
    // Test that non-existent key returns empty optional
    auto notFound = manager.getMetadata("non_existent");
    assert(!notFound.has_value());
    
    std::cout << "    testKeyRegistration passed" << std::endl;
}

void testValidation() {
    std::cout << "    testValidation starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    manager.clear();
    
    // Register a key with a validator that only accepts "valid"
    nlm::ConfigMetadata metadata;
    metadata.description = "Validated key";
    metadata.category = "Test";
    metadata.required = false;
    metadata.validator = [](const nlm::ConfigValue& v) {
        return std::holds_alternative<std::string>(v) && std::get<std::string>(v) == "valid";
    };
    
    manager.registerKey("validated_key", metadata);
    
    nlm::ConfigValue validValue = std::string("valid");
    nlm::ConfigValue invalidValue = std::string("invalid");
    nlm::ConfigValue wrongTypeValue = int(42);
    
    // Test validation passes for valid value
    assert(manager.validate("validated_key", validValue) == true);
    
    // Test validation fails for invalid value
    assert(manager.validate("validated_key", invalidValue) == false);
    
    // Test validation fails for wrong type
    assert(manager.validate("validated_key", wrongTypeValue) == false);
    
    // Test validation for non-existent key returns true (no rules)
    assert(manager.validate("non_existent", validValue) == true);
    
    std::cout << "    testValidation passed" << std::endl;
}

void testRequiredValidation() {
    std::cout << "    testRequiredValidation starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    manager.clear();
    
    // Register required and optional keys
    nlm::ConfigMetadata requiredMetadata;
    requiredMetadata.description = "Required key";
    requiredMetadata.category = "Test";
    requiredMetadata.required = true;
    manager.registerKey("required_key", requiredMetadata);
    
    nlm::ConfigMetadata optionalMetadata;
    optionalMetadata.description = "Optional key";
    optionalMetadata.category = "Test";
    optionalMetadata.required = false;
    manager.registerKey("optional_key", optionalMetadata);
    
    // Test validateRequired returns only required keys when none are present
    std::vector<std::string> missing = manager.validateRequired();
    assert(missing.size() == 1);
    assert(missing[0] == "required_key");
    
    // Clear and register only optional key
    manager.clear();
    manager.registerKey("optional_key", optionalMetadata);
    
    missing = manager.validateRequired();
    assert(missing.empty()); // No required keys registered
    
    std::cout << "    testRequiredValidation passed" << std::endl;
}

void testGetRegisteredKeys() {
    std::cout << "    testGetRegisteredKeys starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    manager.clear();
    
    // Register several keys
    std::vector<std::string> keys = {"key1", "key2", "key3", "key4", "key5"};
    
    for (size_t i = 0; i < keys.size(); ++i) {
        nlm::ConfigMetadata metadata;
        metadata.description = "Key " + std::to_string(i);
        metadata.category = "Test";
        manager.registerKey(keys[i], metadata);
    }
    
    // Get all registered keys
    auto retrievedKeys = manager.getRegisteredKeys();
    assert(retrievedKeys.size() == keys.size());
    
    // Check that all keys are present
    for (const auto& key : keys) {
        assert(std::find(retrievedKeys.begin(), retrievedKeys.end(), key) != retrievedKeys.end());
    }
    
    // Test clear
    manager.clear();
    assert(manager.getRegisteredKeys().empty());
    
    std::cout << "    testGetRegisteredKeys passed" << std::endl;
}

void testInitializeDefaultConfig() {
    std::cout << "    testInitializeDefaultConfig starting..." << std::endl;
    
    nlm::ConfigManager& manager = nlm::ConfigManager::getInstance();
    manager.clear();
    
    // Initialize default configuration
    manager.initializeDefaultConfig();
    
    // Check that expected keys are registered
    std::vector<std::string> expectedKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "plasticity_learning_rate", "stdp_ltp_weight",
        "stdp_ltd_weight", "stdp_tau", "synaptogenesis_rate", "pruning_rate",
        "dopamine_baseline", "reward_discount_factor", "environment_name",
        "environment_width", "environment_height", "log_level", "log_to_file",
        "log_filename", "max_simulation_steps", "simulation_time_limit",
        "visualization_enabled", "visualization_update_rate"
    };
    
    auto registeredKeys = manager.getRegisteredKeys();
    assert(registeredKeys.size() >= expectedKeys.size()); // There might be additional keys
    
    // Check that some key categories are present
    size_t coreCount = 0, plasticityCount = 0, neuromodulationCount = 0;
    for (const auto& key : registeredKeys) {
        if (key.find("random_seed") != std::string::npos || 
            key.find("simulation_timestep") != std::string::npos ||
            key.find("neuron_count") != std::string::npos) {
            coreCount++;
        } else if (key.find("plasticity") != std::string::npos) {
            plasticityCount++;
        } else if (key.find("dopamine") != std::string::npos || 
                   key.find("reward") != std::string::npos) {
            neuromodulationCount++;
        }
    }
    
    assert(coreCount > 0);
    assert(plasticityCount > 0);
    assert(neuromodulationCount > 0);
    
    std::cout << "    testInitializeDefaultConfig passed" << std::endl;
}

void runAll() {
    testConfigManagerCreation();
    testKeyRegistration();
    testValidation();
    testRequiredValidation();
    testGetRegisteredKeys();
    testInitializeDefaultConfig();
}

} // namespace test_config_manager
