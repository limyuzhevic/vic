#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

int main() {
    std::cout << "=== CONFIG SYSTEM VERIFICATION ===" << std::endl;
    
    // Test 1: Basic functionality
    {
        std::cout << "\n1. Basic set/get/remove..." << std::endl;
        nlm::Config config;
        
        config.set("int_val", 42);
        config.set("string_val", std::string("test"));
        config.set("double_val", 3.14);
        config.set("bool_val", true);
        
        assert(config.has("int_val"));
        assert(config.has("string_val"));
        assert(config.has("double_val"));
        assert(config.has("bool_val"));
        
        assert(*config.get<int>("int_val") == 42);
        assert(*config.get<std::string>("string_val") == "test");
        assert(*config.get<double>("double_val") == 3.14);
        assert(*config.get<bool>("bool_val") == true);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 2: getOr with defaults
    {
        std::cout << "\n2. getOr with defaults..." << std::endl;
        nlm::Config config;
        
        int val = config.getOr<int>("missing", 100);
        assert(val == 100);
        
        config.set("existing", 50);
        val = config.getOr<int>("existing", 100);
        assert(val == 50);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 3: Key management
    {
        std::cout << "\n3. Key management (getKeys, clear)..." << std::endl;
        nlm::Config config;
        
        config.set("key1", 1);
        config.set("key2", 2.0);
        config.set("key3", std::string("three"));
        
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        
        config.clear();
        assert(config.getKeys().size() == 0);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 4: JSON save/load
    {
        std::cout << "\n4. JSON save/load functionality..." << std::endl;
        nlm::Config config;
        
        config.set("json_int", 12345);
        config.set("json_string", std::string("json_value"));
        config.set("json_float", 67.89);
        config.set("json_bool", false);
        
        std::string jsonFile = "/tmp/config_verification.json";
        assert(config.saveToFile(jsonFile));
        
        nlm::Config config2;
        assert(config2.loadFromFile(jsonFile));
        
        assert(*config2.get<int>("json_int") == 12345);
        assert(*config2.get<std::string>("json_string") == "json_value");
        assert(*config2.get<double>("json_float") == 67.89);
        assert(*config2.get<bool>("json_bool") == false);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 5: Error handling
    {
        std::cout << "\n5. Error handling..." << std::endl;
        nlm::Config config;
        
        bool nonexistentLoad = config.loadFromFile("/tmp/nonexistent_xyz.json");
        assert(!nonexistentLoad);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    std::cout << "\n=== VERIFICATION COMPLETE ===" << std::endl;
    std::cout << "All core Config system functionality is working correctly!" << std::endl;
    
    return 0;
}
