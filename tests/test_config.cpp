#include <cassert>
#include <iostream>
#include <string>
#include <limits>

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

void testConfigToString() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    std::string str = config.toString();
    
    // Check that toString produces reasonable output
    assert(str.find("int_value=42") != std::string::npos);
    assert(str.find("string_value=\"hello\"") != std::string::npos);
    assert(str.find("bool_value=true") != std::string::npos);
    assert(str.find("double_value=3.14") != std::string::npos);
    assert(str.find("Config {") != std::string::npos);
    assert(str.find("}") != std::string::npos);
    
    std::cout << "    testConfigToString passed" << std::endl;
}

void testConfigSizeAndEmpty() {
    nlm::Config config;
    
    assert(config.isEmpty());
    assert(config.size() == 0);
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    assert(!config.isEmpty());
    assert(config.size() == 1);
    
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    assert(config.size() == 2);
    
    std::cout << "    testConfigSizeAndEmpty passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::File);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", 20, nlm::ConfigSource::Runtime); // Same key, different source
    config2.set("key3", 3, nlm::ConfigSource::File);
    
    config1.merge(config2);
    
    // key1 should be preserved (from config1)
    assert(config1.has("key1"));
    assert(config1.get<int>("key1").value() == 1);
    
    // key2 should be from config2 (Runtime overrides File)
    assert(config1.has("key2"));
    assert(config1.get<int>("key2").value() == 20);
    
    // key3 should be added
    assert(config1.has("key3"));
    assert(config1.get<int>("key3").value() == 3);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigValidate() {
    nlm::Config config;
    
    // Empty config should be valid
    assert(config.validate());
    
    // Config with reasonable values should be valid
    config.set("small_int", 42, nlm::ConfigSource::Runtime);
    config.set("negative_int", -100, nlm::ConfigSource::Runtime);
    config.set("large_double", 1.23e-10, nlm::ConfigSource::Runtime);
    config.set("zero", 0, nlm::ConfigSource::Runtime);
    config.set("true_bool", true, nlm::ConfigSource::Runtime);
    config.set("normal_string", "test", nlm::ConfigSource::Runtime);
    
    assert(config.validate());
    
    // Config with invalid values should fail
    nlm::Config invalidConfig;
    invalidConfig.set("inf_double", std::numeric_limits<double>::infinity(), nlm::ConfigSource::Runtime);
    assert(!invalidConfig.validate());
    
    std::cout << "    testConfigValidate passed" << std::endl;
}

void testConfigHasType() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Correct types should be found
    assert(config.hasType<int>("int_value"));
    assert(config.hasType<std::string>("string_value"));
    
    // Wrong types should not be found
    assert(!config.hasType<std::string>("int_value"));
    assert(!config.hasType<int>("string_value"));
    
    // Missing keys should not have type
    assert(!config.hasType<int>("missing"));
    
    std::cout << "    testConfigHasType passed" << std::endl;
}

void testConfigGetChecked() {
    nlm::Config config;
    
    config.set("value", 42, nlm::ConfigSource::Runtime);
    
    // Should return value when found with correct type
    int val = config.getChecked<int>("value");
    assert(val == 42);
    
    // Should throw when key doesn't exist
    try {
        config.getChecked<int>("missing");
        assert(false); // Should have thrown
    } catch (const std::runtime_error& e) {
        // Expected
    }
    
    // Should throw when wrong type
    try {
        config.getChecked<std::string>("value");
        assert(false); // Should have thrown
    } catch (const std::runtime_error& e) {
        // Expected
    }
    
    std::cout << "    testConfigGetChecked passed" << std::endl;
}

void testConfigGetChecked();

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigToString();
    testConfigSizeAndEmpty();
    testConfigMerge();
    testConfigValidate();
    testConfigHasType();
    testConfigGetChecked();
}
