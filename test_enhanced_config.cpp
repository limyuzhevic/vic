#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing enhanced NLM Configuration System..." << std::endl;
    
    nlm::Config config;
    
    // Test 1: Basic creation and checking
    std::cout << "Test 1: Basic creation..." << std::endl;
    assert(!config.has("test_key"));
    
    // Test 2: Setting and getting values
    std::cout << "Test 2: Setting and getting values..." << std::endl;
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
    
    // Test 3: GetOr with defaults
    std::cout << "Test 3: GetOr with defaults..." << std::endl;
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test 4: Remove
    std::cout << "Test 4: Remove..." << std::endl;
    config.set("test_to_remove", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test_to_remove"));
    
    config.remove("test_to_remove");
    assert(!config.has("test_to_remove"));
    
    // Test 5: Get keys
    std::cout << "Test 5: Get keys..." << endl;
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Test 6: Schema and validation
    std::cout << "Test 6: Schema and validation..." << endl;
    const auto& schema = config.getSchema();
    assert(schema.version == "1.0");
    
    std::string schemaSummary = schema.summary();
    assert(!schemaSummary.empty());
    assert(schemaSummary.find("random_seed") != std::string::npos);
    assert(schemaSummary.find("simulation_timestep") != std::string::npos);
    
    // Test 7: Save to file (JSON format)
    std::cout << "Test 7: Save to file (JSON)..." << endl;
    assert(config.saveToFile("test_config.json"));
    
    // Test 8: Load from file (JSON format)
    std::cout << "Test 8: Load from file (JSON)..." << endl;
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    assert(config2.has("int_value"));
    auto loadedInt = config2.get<int>("int_value");
    assert(loadedInt.has_value());
    assert(*loadedInt == 42);
    
    // Test 9: Summary
    std::cout << "Test 9: Summary..." << endl;
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "All tests passed!" << std::endl;
    
    // Clean up
    std::remove("test_config.json");
    
    return 0;
}
