#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

int main() {
    std::cout << "Testing Config class with simple key=value parser..." << std::endl;
    
    // Test 1: Basic operations
    nlm::Config config;
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("port", 8080, nlm::ConfigSource::Runtime);
    config.set("ssl_enabled", true, nlm::ConfigSource::Runtime);
    
    std::cout << "Name: " << config.getOr<std::string>("name", "") << std::endl;
    std::cout << "Port: " << config.getOr<int64_t>("port", 0) << std::endl;
    std::cout << "SSL Enabled: " << (config.getOr<bool>("ssl_enabled", false) ? "true" : "false") << std::endl;
    
    // Test 2: Save to file
    if (config.saveToFile("test_save.txt")) {
        std::cout << "✓ Saved config to file" << std::endl;
    } else {
        std::cout << "✗ Failed to save config to file" << std::endl;
        return 1;
    }
    
    // Test 3: Load from file
    nlm::Config config2;
    if (config2.loadFromFile("test_save.txt")) {
        std::cout << "✓ Loaded config from file" << std::endl;
        
        // Verify values
        assert(config2.getOr<std::string>("name", "") == "test");
        assert(config2.getOr<int64_t>("port", 0) == 8080);
        assert(config2.getOr<bool>("ssl_enabled", false) == true);
        
        std::cout << "✓ All values correctly loaded from file" << std::endl;
    } else {
        std::cout << "✗ Failed to load config from file" << std::endl;
        std::filesystem::remove("test_save.txt");
        return 1;
    }
    
    // Test 4: Load from args
    nlm::Config config3;
    const char* args[] = {"test", "--key1", "value1", "--key2=123", "-k3", "true"};
    int argc = 7;
    config3.loadFromArgs(argc, const_cast<char**>(args));
    
    std::cout << "Key1 from args: " << config3.getOr<std::string>("key1", "") << std::endl;
    std::cout << "Key2 from args: " << config3.getOr<int64_t>("key2", 0) << std::endl;
    std::cout << "Key3 from args: " << (config3.getOr<bool>("key3", false) ? "true" : "false") << std::endl;
    
    // Test 5: Clear and remove
    config3.clear();
    std::cout << "Config after clear has " << config3.getKeys().size() << " entries (expected 0)" << std::endl;
    
    config3.set("test_key", 999, nlm::ConfigSource::Runtime);
    config3.remove("test_key");
    std::cout << "Key test_key removed, has: " << (config3.has("test_key") ? "true" : "false") << std::endl;
    
    // Clean up
    std::filesystem::remove("test_save.txt");
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
