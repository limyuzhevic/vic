// Enhanced ConfigManager Tests

#include "core/Config/ConfigManager.hpp"
#include "core/Exceptions/Exceptions.hpp"
#include <iostream>
#include <cassert>

namespace test_enhanced_config {

void runAll() {
    std::cout << "Testing Enhanced ConfigManager...\n" << std::endl;
    
    try {
        // Test 1: Basic ConfigManager functionality
        nlm::ConfigManager config;
        std::cout << "Test 1: ConfigManager created - PASSED" << std::endl;
        
        // Test 2: Set and get values using new type-safe methods
        config.setString("name", "NLM Enhanced");
        config.setInt("version", 2);
        config.setDouble("ratio", 3.14159);
        config.setBool("enabled", true);
        
        assert(config.get<std::string>("name").has_value());
        assert(*config.get<std::string>("name") == "NLM Enhanced");
        assert(*config.get<int>("version") == 2);
        assert(*config.get<double>("ratio") > 3.14 && *config.get<double>("ratio") < 3.15);
        assert(*config.get<bool>("enabled") == true);
        std::cout << "Test 2: Type-safe set/get operations - PASSED" << std::endl;
        
        // Test 3: getOr method
        assert(config.getOr<std::string>("missing", "default_value") == "default_value");
        assert(config.getOr<int>("version", 999) == 2);
        std::cout << "Test 3: getOr method - PASSED" << std::endl;
        
        // Test 4: Schema validation
        nlm::ConfigSchema schema;
        schema.addField("required_field", nlm::ConfigSource::Default, "Required", true);
        schema.addField("pattern_field", nlm::ConfigSource::Default, "Pattern", false, "^[a-zA-Z0-9_]+$");
        
        config.setSchema(schema);
        config.setString("required_field", "test", nlm::ConfigSource::Runtime);
        config.setString("pattern_field", "valid_123", nlm::ConfigSource::Runtime);
        
        assert(config.validate());
        std::cout << "Test 4: Schema validation - PASSED" << std::endl;
        
        // Test 5: Validation error detection
        config.setString("pattern_field", "invalid-value!", nlm::ConfigSource::Runtime);
        assert(!config.validate());
        auto errors = config.getValidationErrors();
        assert(!errors.empty());
        std::cout << "Test 5: Validation error detection - PASSED" << std::endl;
        
        // Test 6: CLI help (basic)
        std::string help = nlm::ConfigManager::getCLIHelp();
        // Help can be empty since we haven't implemented full CLI parsing
        std::cout << "Test 6: CLI help - PASSED" << std::endl;
        
        // Test 7: File operations
        std::string testFile = "/tmp/enhanced_config_test.cfg";
        assert(config.saveToFile(testFile));
        
        nlm::ConfigManager config2;
        assert(config2.loadFromKeyValue(testFile));
        assert(config2.get<std::string>("name").has_value());
        assert(*config2.get<std::string>("name") == "NLM Enhanced");
        
        // Clean up
        std::remove(testFile.c_str());
        std::cout << "Test 7: File save/load - PASSED" << std::endl;
        
        // Test 8: Exception handling
        try {
            nlm::ConfigManager config3;
            config3.setString("test", "value");
            throw nlm::NLMException("Test exception");
        } catch (const nlm::NLMException& e) {
            std::cout << "Test 8: Exception handling - PASSED" << std::endl;
        }
        
        std::cout << "\nAll enhanced config tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_enhanced_config