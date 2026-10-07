#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing NLM Configuration System Improvements..." << std::endl;
    
    // Test 1: Basic config creation
    nlm::Config config;
    assert(!config.has("test_key"));
    std::cout << "✓ Basic config creation works" << std::endl;
    
    // Test 2: Set and get values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_key"));
    assert(config.get<int>("int_key").value() == 42);
    assert(config.get<double>("double_key").value() > 3.13);
    assert(config.get<std::string>("string_key").value() == "test");
    assert(config.get<bool>("bool_key").value() == true);
    std::cout << "✓ Set and get operations work" << std::endl;
    
    // Test 3: getOr with default values
    int default_val = config.getOr<int>("missing_key", 100);
    assert(default_val == 100);
    std::cout << "✓ Default value handling works" << std::endl;
    
    // Test 4: Validation (add required keys)
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    bool is_valid = config.validateConfig();
    assert(is_valid);
    std::cout << "✓ Configuration validation works" << std::endl;
    
    // Test 5: Save to file
    std::string testFile = "test_improved_config.cfg";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    std::cout << "✓ Save to file works" << std::endl;
    
    // Test 6: Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    assert(config2.has("int_key"));
    assert(config2.get<int>("int_key").value() == 42);
    std::cout << "✓ Load from file works" << std::endl;
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << std::endl;
    std::cout << "=== All Configuration Tests PASSED ===" << std::endl;
    std::cout << "✓ Configuration system improvements are working correctly!" << std::endl;
    
    return 0;
}