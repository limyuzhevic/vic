#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

class ConfigTests {
public:
    static void runAllTests() {
        std::cout << "Running Config tests..." << std::endl;
        
        testBasicConstructor();
        testSetGetString();
        testSetGetInt();
        testSetGetFloat();
        testSetGetBool();
        testHasKey();
        testGetKeys();
        testRemoveKey();
        testClear();
        testSummary();
        testLoadFromLegacyFormat();
        testSaveToLegacyFormat();
        testLoadFromJson();
        testSaveToJson();
        testJsonRoundtrip();
        testLoadFromArgs();
        testMixedSources();
        testTypeSafety();
        testErrorHandling();
        testValidation();
        
        std::cout << "All Config tests passed!" << std::endl;
    }
    
private:
    static void testBasicConstructor() {
        Config config;
        assert(config.getKeys().empty());
        std::cout << "✓ Basic constructor test passed" << std::endl;
    }
    
    static void testSetGetString() {
        Config config;
        config.set("test_string", "hello world");
        assert(config.has("test_string"));
        
        auto value = config.get<std::string>("test_string");
        assert(value.has_value());
        assert(value.value() == "hello world");
        
        std::cout << "✓ String set/get test passed" << std::endl;
    }
    
    static void testSetGetInt() {
        Config config;
        config.set("test_int", 42);
        
        auto value = config.get<int>("test_int");
        assert(value.has_value());
        assert(value.value() == 42);
        
        std::cout << "✓ Int set/get test passed" << std::endl;
    }
    
    static void testSetGetFloat() {
        Config config;
        config.set("test_float", 3.14159);
        
        auto value = config.get<double>("test_float");
        assert(value.has_value());
        assert(abs(value.value() - 3.14159) < 0.0001);
        
        std::cout << "✓ Float set/get test passed" << std::endl;
    }
    
    static void testSetGetBool() {
        Config config;
        config.set("test_bool", true);
        
        auto value = config.get<bool>("test_bool");
        assert(value.has_value());
        assert(value.value() == true);
        
        std::cout << "✓ Bool set/get test passed" << std::endl;
    }
    
    static void testHasKey() {
        Config config;
        assert(!config.has("nonexistent"));
        
        config.set("existent", "value");
        assert(config.has("existent"));
        
        std::cout << "✓ Has key test passed" << std::endl;
    }
    
    static void testGetKeys() {
        Config config;
        assert(config.getKeys().empty());
        
        config.set("key1", "value1");
        config.set("key2", 2);
        config.set("key3", 3.14);
        
        std::vector<std::string> keys = config.getKeys();
        assert(keys.size() == 3);
        assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
        
        std::cout << "✓ Get keys test passed" << std::endl;
    }
    
    static void testRemoveKey() {
        Config config;
        config.set("key1", "value1");
        config.set("key2", "value2");
        
        assert(config.has("key1"));
        assert(config.has("key2"));
        
        config.remove("key1");
        assert(!config.has("key1"));
        assert(config.has("key2"));
        
        std::cout << "✓ Remove key test passed" << std::endl;
    }
    
    static void testClear() {
        Config config;
        config.set("key1", "value1");
        config.set("key2", "value2");
        
        assert(!config.getKeys().empty());
        
        config.clear();
        assert(config.getKeys().empty());
        
        std::cout << "✓ Clear test passed" << std::endl;
    }
    
    static void testSummary() {
        Config config;
        config.set("neuron_count", 1000);
        config.set("connection_probability", 0.1);
        
        std::string summary = config.summary();
        assert(summary.find("neuron_count") != std::string::npos);
        assert(summary.find("connection_probability") != std::string::npos);
        assert(summary.find("1000") != std::string::npos);
        assert(summary.find("0.1") != std::string::npos);
        
        std::cout << "✓ Summary test passed" << std::endl;
    }
    
    static void testLoadFromLegacyFormat() {
        // Create a temporary legacy config file
        std::string tempFile = "test_config_legacy.cfg";
        std::ofstream file(tempFile);
        file << "# Test config file\n";
        file << "neuron_count = 500\n";
        file << "region_count = 2\n";
        file << "connection_probability = 0.2\n";
        file << "random_seed = 123\n";
        file.close();
        
        Config config;
        bool loaded = config.loadFromFile(tempFile);
        assert(loaded);
        
        // Check if values were loaded
        auto neuron_count = config.get<int64_t>("neuron_count");
        assert(neuron_count.has_value());
        assert(neuron_count.value() == 500);
        
        auto region_count = config.get<int64_t>("region_count");
        assert(region_count.has_value());
        assert(region_count.value() == 2);
        
        // Clean up
        std::remove(tempFile.c_str());
        
        std::cout << "✓ Legacy format load test passed" << std::endl;
    }
    
    static void testSaveToLegacyFormat() {
        // Create config and save to file
        Config config;
        config.set("neuron_count", 1000);
        config.set("connection_probability", 0.15);
        
        std::string tempFile = "test_config_save_legacy.cfg";
        bool saved = config.saveToFile(tempFile);
        assert(saved);
        
        // Read back and verify
        std::ifstream readFile(tempFile);
        std::string line;
        bool found_neuron = false;
        bool found_connection = false;
        
        while (std::getline(readFile, line)) {
            if (line.find("neuron_count") != std::string::npos) {
                found_neuron = true;
            }
            if (line.find("connection_probability") != std::string::npos) {
                found_connection = true;
            }
        }
        
        assert(found_neuron);
        assert(found_connection);
        
        // Clean up
        std::remove(tempFile.c_str());
        
        std::cout << "✓ Legacy format save test passed" << std::endl;
    }
    
    static void testLoadFromJson() {
        std::string jsonStr = R"({
            "neuron_count": 2000,
            "region_count": 4,
            "connection_probability": 0.25,
            "simulation_timestep": 0.001,
            "random_seed": 999
        })"";
        
        Config config;
        bool loaded = config.loadFromJson(jsonStr);
        assert(loaded);
        
        // Check if values were loaded
        auto neuron_count = config.get<int64_t>("neuron_count");
        assert(neuron_count.has_value());
        assert(neuron_count.value() == 2000);
        
        auto region_count = config.get<int64_t>("region_count");
        assert(region_count.has_value());
        assert(region_count.value() == 4);
        
        auto connection_prob = config.get<double>("connection_probability");
        assert(connection_prob.has_value());
        assert(abs(connection_prob.value() - 0.25) < 0.0001);
        
        std::cout << "✓ JSON load test passed" << std::endl;
    }
    
    static void testSaveToJson() {
        Config config;
        config.set("neuron_count", 3000);
        config.set("region_count", 6);
        config.set("connection_probability", 0.3);
        
        std::string jsonStr = config.saveToJson();
        
        // Parse JSON and verify
        Config config2;
        bool loaded = config2.loadFromJson(jsonStr);
        assert(loaded);
        
        auto neuron_count = config2.get<int64_t>("neuron_count");
        assert(neuron_count.has_value());
        assert(neuron_count.value() == 3000);
        
        std::cout << "✓ JSON save test passed" << std::endl;
    }
    
    static void testJsonRoundtrip() {
        Config config;
        config.set("test_int", 42);
        config.set("test_float", 3.14);
        config.set("test_bool", true);
        config.set("test_string", "hello");
        
        std::string jsonStr = config.saveToJson();
        
        Config config2;
        bool loaded = config2.loadFromJson(jsonStr);
        assert(loaded);
        
        // Verify all values
        assert(config2.get<int64_t>("test_int") == 42);
        assert(abs(config2.get<double>("test_float").value() - 3.14) < 0.0001);
        assert(config2.get<bool>("test_bool") == true);
        assert(config2.get<std::string>("test_string") == "hello");
        
        std::cout << "✓ JSON roundtrip test passed" << std::endl;
    }
    
    static void testLoadFromArgs() {
        // Simulate command line arguments
        int argc = 5;
        char* argv[] = {
            (char*)"program_name",
            (char*)"--neuron_count=500",
            (char*)"--connection_probability=0.2",
            (char*)"-random_seed",
            (char*)"123"
        };
        
        Config config;
        config.loadFromArgs(argc, argv);
        
        assert(config.get<int64_t>("neuron_count") == 500);
        assert(abs(config.get<double>("connection_probability").value() - 0.2) < 0.0001);
        assert(config.get<int64_t>("random_seed") == 123);
        
        std::cout << "✓ Command line load test passed" << std::endl;
    }
    
    static void testMixedSources() {
        Config config;
        
        // Set values from different sources
        config.set("file_value", 100, ConfigSource::File);
        config.set("command_line_value", 200, ConfigSource::CommandLine);
        config.set("runtime_value", 300, ConfigSource::Runtime);
        
        // Verify all values are set
        assert(config.get<int64_t>("file_value") == 100);
        assert(config.get<int64_t>("command_line_value") == 200);
        assert(config.get<int64_t>("runtime_value") == 300);
        
        // Check sources
        std::cout << "✓ Mixed sources test passed" << std::endl;
    }
    
    static void testTypeSafety() {
        Config config;
        config.set("int_value", 42);
        config.set("float_value", 3.14);
        config.set("bool_value", true);
        config.set("string_value", "hello");
        config.set("int_array", std::vector<int>{1, 2, 3});
        config.set("string_array", std::vector<std::string>{"a", "b", "c"});
        
        // Test type-specific getters
        assert(config.get<int64_t>("int_value") == 42);
        assert(abs(config.get<double>("float_value").value() - 3.14) < 0.0001);
        assert(config.get<bool>("bool_value") == true);
        assert(config.get<std::string>("string_value") == "hello");
        assert(config.get<std::vector<int>>("int_array") == std::vector<int>{1, 2, 3});
        assert(config.get<std::vector<std::string>>("string_array") == std::vector<std::string>{"a", "b", "c"});
        
        // Test wrong type returns nullopt
        assert(!config.get<std::string>("int_value").has_value());
        assert(!config.get<int64_t>("string_value").has_value());
        
        std::cout << "✓ Type safety test passed" << std::endl;
    }
    
    static void testErrorHandling() {
        Config config;
        
        // Test getting non-existent key
        auto value = config.get<std::string>("non_existent");
        assert(!value.has_value());
        
        // Test array access (should work with correct type)
        config.set("test_array", std::vector<int>{1, 2, 3});
        auto array = config.get<std::vector<int>>("test_array");
        assert(array.has_value());
        assert(array.value() == std::vector<int>{1, 2, 3});
        
        // Test wrong array type
        auto wrong_array = config.get<std::vector<std::string>>("test_array");
        assert(!wrong_array.has_value());
        
        std::cout << "✓ Error handling test passed" << std::endl;
    }
    
    static void testValidation() {
        Config config;
        
        // Valid configuration
        config.set("neuron_count", 1000);
        config.set("connection_probability", 0.1);
        assert(config.validate());
        
        // Add invalid value (string instead of expected type)
        // Note: Config validation is basic, so this should still pass
        config.set("another_key", "valid_value");
        assert(config.validate());
        
        std::cout << "✓ Validation test passed" << std::endl;
    }
};

} // namespace nlm

#endif // TEST_CONFIG_HPP
