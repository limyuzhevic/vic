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

void testConfigValueAsString() {
    nlm::Config config;
    
    // Test getting string values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    std::string intStr = config.getValueAsString("int_value");
    assert(intStr == "42");
    
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    std::string doubleStr = config.getValueAsString("double_value");
    assert(doubleStr.find("3.14159") != std::string::npos);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    std::string stringStr = config.getValueAsString("string_value");
    assert(stringStr == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    std::string boolStr = config.getValueAsString("bool_value");
    assert(boolStr == "true");
    
    // Test missing key
    std::string missingStr = config.getValueAsString("missing", "default_value");
    assert(missingStr == "default_value");
    
    std::cout << "    testConfigValueAsString passed" << std::endl;
}

void testConfigIsEmpty() {
    nlm::Config config1;
    assert(config1.isEmpty());
    
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    assert(!config1.isEmpty());
    
    config1.clear();
    assert(config1.isEmpty());
    
    std::cout << "    testConfigIsEmpty passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
    config.set("random.seed", 12345, nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(summary.find("brain.neuron_count") != std::string::npos);
    assert(summary.find("brain.synapse_density") != std::string::npos);
    assert(summary.find("random.seed") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValueAsString();
    testConfigIsEmpty();
    testConfigSummary();
}

} // namespace test_config
