// Simple test for the enhanced Config class
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

// Simple test function
bool runConfigEnhancedTests() {
    std::cout << "=== Enhanced Config Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test 1: Basic operations
    {
        std::cout << "Test 1: Basic operations..." << std::endl;
        nlm::Config config;
        
        // Set values of different types
        config.set("int_value", 42);
        config.set("double_value", 3.14159);
        config.set("string_value", std::string("NLM Test"));
        config.set("bool_value", true);
        
        // Check existence
        assert(config.has("int_value"));
        assert(config.has("double_value"));
        assert(config.has("string_value"));
        assert(config.has("bool_value"));
        
        // Get values
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
        
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value() && *stringVal == "NLM Test");
        
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value() && *boolVal == true);
        
        // Test getOr
        int defaultVal = config.getOr<int>("nonexistent", 999);
        assert(defaultVal == 999);
        
        int existingVal = config.getOr<int>("int_value", 999);
        assert(existingVal == 42);
        
        // Test keys
        auto keys = config.getKeys();
        assert(keys.size() == 4);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    // Test 2: Validation
    {
        std::cout << "Test 2: Validation..." << std::endl;
        nlm::Config config;
        
        // Create validation function
        auto rangeValidator = [](nlm::ConfigValue val) -> bool {
            bool result = false;
            std::visit([&](auto&& v) {
                result = (v >= 0 && v <= 100);
            }, val);
            return result;
        };
        
        // Set with validation - should succeed
        bool validSet = config.setWithValidation("valid_value", 50, rangeValidator);
        assert(validSet);
        
        // Set with validation - should fail
        bool invalidSet = config.setWithValidation("invalid_value", 150, rangeValidator);
        assert(!invalidSet);
        
        // Verify valid value was set
        assert(config.has("valid_value"));
        
        // Verify invalid value was NOT set
        assert(!config.has("invalid_value"));
        
        // Test getOrWithValidation
        int validatedValue = config.getOrWithValidation("valid_value", 0, rangeValidator);
        assert(validatedValue == 50);
        
        int defaultValue = config.getOrWithValidation("invalid_value", 100, rangeValidator);
        assert(defaultValue == 100);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    // Test 3: File operations
    {
        std::cout << "Test 3: File operations..." << std::endl;
        nlm::Config config;
        
        // Set some values
        config.set("file_test_int", 123);
        config.set("file_test_string", std::string("File Test"));
        config.set("file_test_bool", false);
        config.setDescription("file_test_int", "Test value for file save/load");
        
        // Save to file
        std::string filename = "/tmp/nlm_config_test.cfg";
        bool saveSuccess = config.saveToFile(filename);
        assert(saveSuccess);
        
        // Create new config and load
        nlm::Config config2;
        bool loadSuccess = config2.loadFromFile(filename);
        assert(loadSuccess);
        
        // Verify loaded values
        assert(config2.has("file_test_int"));
        assert(config2.getOr<int>("file_test_int", 0) == 123);
        
        assert(config2.has("file_test_string"));
        auto loadedString = config2.get<std::string>("file_test_string");
        assert(loadedString.has_value() && *loadedString == "File Test");
        
        assert(config2.has("file_test_bool"));
        assert(config2.getOr<bool>("file_test_bool", true) == false);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    // Test 4: Advanced features
    {
        std::cout << "Test 4: Advanced features..." << std::endl;
        nlm::Config config;
        
        // Test set default
        config.setDefault("default_value", 777);
        assert(config.has("default_value"));
        assert(config.getOr<int>("default_value", 0) == 777);
        
        // Test get by source
        auto defaultVal = config.getBySource<int>("default_value", nlm::ConfigSource::Default);
        assert(defaultVal.has_value() && *defaultVal == 777);
        
        // Test setIfNotExists
        bool exists = config.has("default_value");
        bool setResult = config.setIfNotExists("new_value", 888);
        assert(setResult);
        
        bool alreadyExists = config.has("default_value");
        assert(exists == alreadyExists);
        
        // Test validation errors
        auto validationErrors = config.getValidationErrors<int>();
        assert(validationErrors.empty());
        
        // Test isValid
        assert(config.isValid<int>());
        
        // Test export/import
        std::string text = config.exportAsText();
        assert(!text.empty());
        
        std::cout << "  PASSED" << std::endl;
    }
    
    if (allPassed) {
        std::cout << std::endl;
        std::cout << "=== All Enhanced Config Tests PASSED ===" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== Some Enhanced Config Tests FAILED ===" << std::endl;
    }
    
    return allPassed;
}

int main() {
    std::cout << "=== NLM Enhanced Configuration System Tests ===" << std::endl;
    std::cout << std::endl;
    
    if (runConfigEnhancedTests()) {
        std::cout << std::endl;
        std::cout << "=== All Tests Passed ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "=== Tests Failed ===" << std::endl;
        return 1;
    }
}
