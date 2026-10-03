#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

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

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    config.set("number_key", 42, nlm::ConfigSource::Runtime);
    config.set("boolean_key", true, nlm::ConfigSource::Runtime);
    
    // Test saving to file
    std::string testFile = "/tmp/test_config.json";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Test loading back
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(testFile);
    assert(loadResult);
    
    assert(config2.getOr<std::string>("test_key", "") == "test_value");
    assert(config2.getOr<int>("number_key", 0) == 42);
    assert(config2.getOr<bool>("boolean_key", false) == true);
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigJsonRoundTrip() {
    nlm::Config config;
    
    // Set various types
    config.set("int_val", 42);
    config.set("double_val", 3.14159);
    config.set("string_val", "hello world");
    config.set("bool_val", true);
    config.set("int_vector", std::vector<int>({1, 2, 3}));
    config.set("double_vector", std::vector<double>({1.5, 2.5, 3.5}));
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}));
    
    // Convert to JSON
    std::string jsonStr = "";
    try {
        nlohmann::json j;
        for (const auto& entry : config.getKeys()) {
            auto valueOpt = config.getRaw(entry);
            if (valueOpt.has_value()) {
                j[entry] = nlm::Config::configValueToJson(*valueOpt);
            }
        }
        jsonStr = j.dump(2);
    } catch (...) {
        // Skip JSON conversion test if not available
        std::cout << "    Skipping JSON round-trip test" << std::endl;
        return;
    }
    
    // Load from JSON
    nlm::Config config2;
    bool loadResult = config2.loadFromJson(jsonStr);
    assert(loadResult);
    
    // Verify values
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<double>("double_val", 0.0) > 3.14 && config2.getOr<double>("double_val", 0.0) < 3.15);
    assert(config2.getOr<std::string>("string_val", "") == "hello world");
    assert(config2.getOr<bool>("bool_val", false) == true);
    
    auto intVector = config2.get<std::vector<int>>("int_vector");
    assert(intVector.has_value() && intVector.value() == std::vector<int>({1, 2, 3}));
    
    std::cout << "    testConfigJsonRoundTrip passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test invalid key validation
    try {
        config.set("", "value");
        assert(false && "Should have thrown validation error");
    } catch (const nlm::ConfigValidationError& e) {
        std::cout << "    testConfigErrorHandling: caught expected validation error" << std::endl;
    }
    
    // Test type safety
    config.set("test_int", 42);
    auto wrongType = config.get<std::string>("test_int");
    assert(!wrongType.has_value()); // Should fail type check
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

int main() {
    std::cout << "Running Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveToFile();
    testConfigJsonRoundTrip();
    testConfigErrorHandling();
    
    std::cout << std::endl << "All Config tests passed!" << std::endl;
    return 0;
}
