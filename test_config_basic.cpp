# Simple Config Test to verify new functionality
// This test demonstrates the new Config features implemented in this codebase

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testBasicConfigFunctionality() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    // Test 1: Basic creation
    nlm::Config config;
    assert(!config.has("test_key"));
    std::cout << "  ✓ Config creation works" << std::endl;
    
    // Test 2: Set and get values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    assert(config.getOr<int>("int_value", 0) == 42);
    std::cout << "  ✓ Basic set/get operations work" << std::endl;
    
    // Test 3: Type conversion
    config.set("double_value", std::string("3.14"), nlm::ConfigSource::Runtime);
    assert(config.getOr<double>("double_value", 0.0) > 3.13);
    std::cout << "  ✓ Type conversion works" << std::endl;
    
    // Test 4: Environment variable support
    setenv("NLM_TEST_ENV_VAR", "test_value", 1);
    bool loaded = config.loadFromEnv("NLM_TEST_ENV_VAR");
    assert(loaded);
    assert(config.getOr<std::string>("test_env_var", "") == "test_value");
    unsetenv("NLM_TEST_ENV_VAR");
    std::cout << "  ✓ Environment variable loading works" << std::endl;
    
    // Test 5: Configuration validation
    bool validator = [](const int& val) { return val > 0 && val < 100; };
    assert(config.setValidated("validated_value", 50, validator, nlm::ConfigSource::Runtime, "Validated test value"));
    assert(config.hasValidated("validated_value", validator));
    std::cout << "  ✓ Configuration validation works" << std::endl;
    
    // Test 6: Configuration saving
    bool saved = config.saveToFile("/tmp/test_config.cfg");
    assert(saved);
    std::cout << "  ✓ Configuration saving works" << std::endl;
    
    // Test 7: Configuration loading from file
    bool loadedFromFile = config.loadFromFile("/tmp/test_config.cfg");
    assert(loadedFromFile);
    std::cout << "  ✓ Configuration loading from file works" << std::endl;
    
    // Clean up
    std::remove("/tmp/test_config.cfg");
    
    std::cout << "All basic Config tests passed!" << std::endl;
}

int main() {
    try {
        testBasicConfigFunctionality();
        std::cout << std::endl;
        std::cout << "=== SUCCESS: Config enhancements are working correctly ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== FAILURE: " << e.what() << " ===" << std::endl;
        return 1;
    }
}
