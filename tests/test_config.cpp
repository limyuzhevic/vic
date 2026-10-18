// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
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

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Add various types of config entries
    config.set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("int_vector_key", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_vector_key", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    config.set("double_vector_key", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    
    // Test with custom descriptions
    config.set("key_with_desc", "value", nlm::ConfigSource::Runtime);
    // Note: description is set during construction, not through set() method
    
    // Save to file
    bool saved = config.saveToFile("test_config_output.txt");
    assert(saved);
    
    // Verify file was created and contains expected content
    std::ifstream file("test_config_output.txt");
    assert(file.is_open());
    
    std::string line;
    std::vector<std::string> keys = {"string_key", "int_key", "double_key", "bool_key", 
                                      "int_vector_key", "string_vector_key", "double_vector_key", "key_with_desc"};
    
    bool foundKeys[8] = {false, false, false, false, false, false, false, false};
    bool foundDescriptions = false;
    
    while (std::getline(file, line)) {
        // Check for key=value pairs
        for (size_t i = 0; i < keys.size(); ++i) {
            if (line.find(keys[i] + " =") != std::string::npos) {
                foundKeys[i] = true;
                // Verify values are saved correctly (not PLACEHOLDER_VALUE)
                assert(line.find("PLACEHOLDER_VALUE") == std::string::npos);
            }
        }
        
        // Check for comments (descriptions)
        if (line.find("#") != std::string::npos) {
            foundDescriptions = true;
        }
    }
    
    file.close();
    
    // Remove test file
    std::remove("test_config_output.txt");
    
    // Verify all keys were found
    for (size_t i = 0; i < 8; ++i) {
        assert(foundKeys[i]);
    }
    assert(foundDescriptions);  // Should have descriptions for all entries
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

} // namespace test_config
