// Simple test to verify ConfigManager implementation
#include "src/core/Config/ConfigManager.hpp"
#include <iostream>
#include <cassert>

int main() {
    nlm::ConfigManager config;
    
    // Test 1: Basic ConfigManager functionality
    config.setString("name", "TestConfig");
    config.setInt("version", 2);
    config.setDouble("ratio", 3.14159);
    config.setBool("enabled", true);
    
    std::cout << "Config created with: name=" << *config.get<std::string>("name") 
              << ", version=" << *config.get<int>("version") 
              << ", ratio=" << *config.get<double>("ratio") 
              << ", enabled=" << *config.get<bool>("enabled") 
              << std::endl;
    
    // Test type-safe get operations
    auto name = config.get<std::string>("name");
    auto version = config.get<int>("version");
    auto ratio = config.get<double>("ratio");
    auto enabled = config.get<bool>("enabled");
    
    std::cout << "name has_value: " << (name.has_value() ? "true" : "false") 
              << ", version: " << (version.has_value() ? std::to_string(*version) : "null")
              << ", ratio: " << (ratio.has_value() ? std::to_string(*ratio) : "null")
              << ", enabled: " << (enabled.has_value() ? std::to_string(*enabled) : "null") 
              << std::endl;
    
    assert(name.has_value() && *name == "TestConfig");
    assert(version.has_value() && *version == 2);
    assert(ratio.has_value() && *ratio > 3.14 && *ratio < 3.15);
    assert(enabled.has_value() && *enabled == true);
    
    std::cout << "All basic assertions passed!" << std::endl;
    
    // Test getOr method
    auto missing = config.getOr<std::string>("missing", "default_value");
    assert(missing == "default_value");
    
    std::cout << "getOr test passed!" << std::endl;
    
    // Test schema validation
    nlm::ConfigSchema schema;
    schema.addField("required_field", nlm::ConfigSource::Default, "Required", true);
    schema.addField("pattern_field", nlm::ConfigSource::Default, "Pattern", false, "^[a-zA-Z0-9_]+$");
    
    config.setSchema(schema);
    config.setString("required_field", "test", nlm::ConfigSource::Runtime);
    config.setString("pattern_field", "valid_123", nlm::ConfigSource::Runtime);
    
    assert(config.validate());
    std::cout << "Schema validation test passed!" << std::endl;
    
    // Test validation error detection
    config.setString("pattern_field", "invalid-value!", nlm::ConfigSource::Runtime);
    assert(!config.validate());
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    std::cout << "Validation error detection test passed! Errors: " << errors.size() << std::endl;
    
    // Test CLI help
    std::string help = nlm::ConfigManager::getCLIHelp();
    assert(!help.empty());
    std::cout << "CLI help test passed!" << std::endl;
    
    // Test file operations
    std::string testFile = "/tmp/test_config_verify.cfg";
    assert(config.saveToFile(testFile));
    std::cout << "File save test passed!" << std::endl;
    
    nlm::ConfigManager config2;
    assert(config2.loadFromKeyValue(testFile));
    assert(config2.get<std::string>("name").has_value());
    assert(*config2.get<std::string>("name") == "TestConfig");
    std::cout << "File load test passed!" << std::endl;
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "All ConfigManager tests passed!" << std::endl;
    return 0;
}
