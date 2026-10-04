#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== COMPLETE CONFIG SYSTEM TEST ===" << std::endl;
    
    // Test 1: Basic functionality
    {
        std::cout << "\nTest 1: Basic set/get/remove" << std::endl;
        nlm::Config config;
        
        config.set("int_val", 42);
        config.set("double_val", 3.14);
        config.set("string_val", std::string("test"));
        config.set("bool_val", true);
        
        assert(config.has("int_val"));
        assert(config.has("double_val"));
        assert(config.has("string_val"));
        assert(config.has("bool_val"));
        
        assert(*config.get<int>("int_val") == 42);
        assert(*config.get<double>("double_val") > 3.13 && *config.get<double>("double_val") < 3.15);
        assert(*config.get<std::string>("string_val") == "test");
        assert(*config.get<bool>("bool_val") == true);
        
        config.remove("int_val");
        assert(!config.has("int_val"));
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 2: getOr with defaults
    {
        std::cout << "\nTest 2: getOr with defaults" << std::endl;
        nlm::Config config;
        
        int val = config.getOr<int>("missing", 100);
        assert(val == 100);
        
        config.set("existing", 50);
        val = config.getOr<int>("existing", 100);
        assert(val == 50);
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 3: Key management
    {
        std::cout << "\nTest 3: Key management (getKeys, clear)" << std::endl;
        nlm::Config config;
        
        assert(config.getKeys().size() == 0);
        
        config.set("key1", 1);
        config.set("key2", 2.0);
        config.set("key3", std::string("three"));
        
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        
        bool hasKey1 = std::any_of(keys.begin(), keys.end(),
                                  [](const std::string& k) { return k == "key1"; });
        bool hasKey2 = std::any_of(keys.begin(), keys.end(),
                                  [](const std::string& k) { return k == "key2"; });
        bool hasKey3 = std::any_of(keys.begin(), keys.end(),
                                  [](const std::string& k) { return k == "key3"; });
        
        assert(hasKey1 && hasKey2 && hasKey3);
        
        config.clear();
        assert(config.getKeys().size() == 0);
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 4: JSON save/load
    {
        std::cout << "\nTest 4: JSON save/load functionality" << std::endl;
        nlm::Config config;
        
        config.set("test_int", 12345);
        config.set("test_string", std::string("json_value"));
        config.set("test_float", 67.89);
        config.set("test_bool", false);
        
        assert(config.saveToFile("/tmp/test_complete.json"));
        
        nlm::Config config2;
        assert(config2.loadFromFile("/tmp/test_complete.json"));
        
        assert(*config2.get<int>("test_int") == 12345);
        assert(*config2.get<std::string>("test_string") == "json_value");
        assert(*config2.get<double>("test_float") == 67.89);
        assert(*config2.get<bool>("test_bool") == false);
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 5: Command line parsing
    {
        std::cout << "\nTest 5: Command line argument parsing" << std::endl;
        nlm::Config config;
        
        const int argc = 5;
        const char* argv[] = {
            "test_program",
            "--cli_int=999",
            "-cli_double=2.718",
            "--cli_bool=true",
            "-cli_string=command_line"
        };
        
        config.loadFromArgs(argc, const_cast<char**>(argv));
        
        assert(config.has("cli_int"));
        assert(config.has("cli_double"));
        assert(config.has("cli_bool"));
        assert(config.has("cli_string"));
        
        assert(*config.get<int>("cli_int") == 999);
        assert(*config.get<double>("cli_double") > 2.717 && *config.get<double>("cli_double") < 2.719);
        assert(*config.get<bool>("cli_bool") == true);
        assert(*config.get<std::string>("cli_string") == "command_line");
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 6: Error handling
    {
        std::cout << "\nTest 6: Error handling" << std::endl;
        nlm::Config config;
        
        bool nonexistentLoad = config.loadFromFile("/tmp/nonexistent_xyz.json");
        assert(!nonexistentLoad);
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 7: Summary
    {
        std::cout << "\nTest 7: Summary functionality" << std::endl;
        nlm::Config config;
        
        config.set("summary_int", 1);
        config.set("summary_double", 2.0);
        
        std::string summary = config.summary();
        assert(!summary.empty());
        assert(summary.find("Configuration (2 entries):") != std::string::npos);
        
        std::cout << "PASSED" << std::endl;
    }
    
    // Test 8: Type conversion and validation
    {
        std::cout << "\nTest 8: Type conversion and validation" << std::endl;
        nlm::Config config;
        
        // Test that different types can be set and retrieved
        config.set("int_as_double", 5); // Should be stored as int but retrievable as double
        assert(config.getOr<double>("int_as_double", 0.0) == 5.0);
        
        // Test array types
        config.set("int_array", std::vector<int>({1, 2, 3}));
        assert(config.has("int_array"));
        
        std::cout << "PASSED" << std::endl;
    }
    
    std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
    std::cout << "Config system is working correctly with JSON, CLI, and all features!" << std::endl;
    
    return 0;
}
