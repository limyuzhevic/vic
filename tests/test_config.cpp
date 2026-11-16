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

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

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
    
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("brain.region_count", 2, nlm::ConfigSource::Default);
    config.set("simulation.timestep", 0.001, nlm::ConfigSource::Default);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config.set("plasticity.hebbian.enable", false, nlm::ConfigSource::Default);
    
    const std::string testFile = "/tmp/test_config.cfg";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Verify file was created and has content
    assert(std::filesystem::exists(testFile));
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    assert(!content.empty());
    assert(content.find("brain.neuron_count = 1000") != std::string::npos);
    assert(content.find("brain.region_count = 2") != std::string::npos);
    assert(content.find("simulation.timestep = 0.001") != std::string::npos);
    assert(content.find("plasticity.stdp.enable = true") != std::string::npos);
    assert(content.find("plasticity.hebbian.enable = false") != std::string::npos);
    
    file.close();
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigLoadFromFile() {
    nlm::Config config;
    
    config.set("test.int", 42, nlm::ConfigSource::Default);
    config.set("test.string", "hello", nlm::ConfigSource::Default);
    config.set("test.double", 3.14, nlm::ConfigSource::Default);
    config.set("test.bool", true, nlm::ConfigSource::Default);
    
    const std::string testFile = "/tmp/test_config_load.cfg";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Clear config and reload
    config.clear();
    bool loadResult = config.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify loaded values
    assert(config.has("test.int"));
    auto intVal = config.get<int>("test.int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    assert(config.has("test.string"));
    auto stringVal = config.get<std::string>("test.string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    assert(config.has("test.double"));
    auto doubleVal = config.get<double>("test.double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("test.bool"));
    auto boolVal = config.get<bool>("test.bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigToJSON() {
    nlm::Config config;
    
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("brain.region_count", 2, nlm::ConfigSource::Default);
    config.set("simulation.timestep", 0.001, nlm::ConfigSource::Default);
    
    std::string jsonOutput = config.toJSON();
    assert(!jsonOutput.empty());
    
    // Verify JSON structure (basic validation)
    assert(jsonOutput.find("version") != std::string::npos);
    assert(jsonOutput.find("description") != std::string::npos);
    assert(jsonOutput.find("config") != std::string::npos);
    assert(jsonOutput.find("brain.neuron_count") != std::string::npos);
    assert(jsonOutput.find("brain.region_count") != std::string::npos);
    assert(jsonOutput.find("simulation.timestep") != std::string::npos);
    
    // Verify JSON is valid (attempt to parse)
    nlohmann::json parsedJson = nlohmann::json::parse(jsonOutput);
    assert(parsedJson.contains("version"));
    assert(parsedJson.contains("config"));
    
    std::cout << "    testConfigToJSON passed" << std::endl;
}

void testConfigGetConfigSchema() {
    nlm::Config config;
    
    std::string schema = config.getConfigSchema();
    assert(!schema.empty());
    
    // Basic schema validation
    assert(schema.find("version") != std::string::npos);
    assert(schema.find("description") != std::string::npos);
    assert(schema.find("type") != std::string::npos);
    assert(schema.find("properties") != std::string::npos);
    
    std::cout << "    testConfigGetConfigSchema passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveToFile();
    testConfigLoadFromFile();
    testConfigToJSON();
    testConfigGetConfigSchema();
}

} // namespace test_config
