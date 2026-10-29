// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

def testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

def testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

def testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

def testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

def testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

def testConfigJSONSaveLoad() {
    nlm::Config config;
    
    // Test basic types
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test nested configuration
    config.set("brain.neuron_count", 1000);
    config.set("brain.region_count", 2);
    
    // Test array types
    std::vector<int> int_vec{1, 2, 3};
    config.set("int_array", int_vec);
    
    std::vector<std::string> str_vec{"a", "b", "c"};
    config.set("string_array", str_vec);
    
    // Save to JSON file
    assert(config.saveToFile("/tmp/test_config.json"));
    
    // Clear config
    config.clear();
    
    // Load from JSON file
    assert(config.loadFromFile("/tmp/test_config.json"));
    
    // Verify loaded values
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.has("double_value"));
    assert(abs(config.get<double>("double_value").value() - 3.14) < 0.001);
    
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    assert(config.has("brain.neuron_count"));
    assert(config.get<int>("brain.neuron_count").value() == 1000);
    
    assert(config.has("brain.region_count"));
    assert(config.get<int>("brain.region_count").value() == 2);
    
    // Clean up
    std::filesystem::remove("/tmp/test_config.json");
    
    std::cout << "    testConfigJSONSaveLoad passed" << std::endl;
}

def testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Save config in old format
    config.set("neuron_count", 1000);
    config.set("region_count", 2);
    
    assert(config.saveToFile("/tmp/test_config_compat.txt"));
    
    // Clear and load in old format
    config.clear();
    assert(config.loadFromFile("/tmp/test_config_compat.txt"));
    
    // Verify backward compatibility
    assert(config.has("neuron_count"));
    assert(config.get<int>("neuron_count").value() == 1000);
    
    assert(config.has("region_count"));
    assert(config.get<int>("region_count").value() == 2);
    
    std::filesystem::remove("/tmp/test_config_compat.txt");
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

def testConfigValidation() {
    nlm::Config config;
    
    // Test validation of empty key
    try {
        config.set("", "value");
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    // Test validation of invalid key
    try {
        config.set("key with spaces", "value");
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    // Test validation of key with invalid characters
    try {
        config.set("key@symbol", "value");
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

def testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Mock command line arguments
    const int argc = 8;
    const char* argv[] = {
        "test",
        "--int_value=42",
        "-double_value=3.14",
        "-string_value=hello",
        "--bool_value=true",
        "--brain.neuron_count=1000",
        "--brain.region_count=2",
        "--help=true"
    };
    
    assert(config.loadFromArgs(argc, const_cast<char**>(argv)));
    
    // Verify loaded values
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.has("double_value"));
    assert(abs(config.get<double>("double_value").value() - 3.14) < 0.001);
    
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    assert(config.has("brain.neuron_count"));
    assert(config.get<int>("brain.neuron_count").value() == 1000);
    
    assert(config.has("brain.region_count"));
    assert(config.get<int>("brain.region_count").value() == 2);
    
    assert(config.has("help"));
    assert(config.get<bool>("help").value() == true);
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

def runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONSaveLoad();
    testConfigBackwardCompatibility();
    testConfigValidation();
    testConfigLoadFromArgs();
}

} // namespace test_config
