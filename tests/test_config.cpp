// Updated Config Tests with new API
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

void testConfigTypeSafeConvenienceMethods() {
    nlm::Config config;
    
    // Test setting values with convenience methods
    config.setString("name", "Neural Network");
    config.setInt("neuron_count", 1000);
    config.setDouble("connection_probability", 0.1);
    config.setBool("enabled", true);
    
    // Test getting values with convenience methods
    auto stringVal = config.getString("name");
    assert(stringVal.has_value());
    assert(*stringVal == "Neural Network");
    
    auto intVal = config.getInt("neuron_count");
    assert(intVal.has_value());
    assert(*intVal == 1000);
    
    auto doubleVal = config.getDouble("connection_probability");
    assert(doubleVal.has_value());
    assert(*doubleVal > 0.09 && *doubleVal < 0.11);
    
    auto boolVal = config.getBool("enabled");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test non-existent keys return nullopt
    assert(!config.getString("non_existent").has_value());
    assert(!config.getInt("non_existent").has_value());
    assert(!config.getDouble("non_existent").has_value());
    assert(!config.getBool("non_existent").has_value());
    
    std::cout << "    testConfigTypeSafeConvenienceMethods passed" << std::endl;
}

void testConfigConvenienceSetters() {
    nlm::Config config;
    
    // Test setIfExists - key doesn't exist
    bool result = config.setIfExists("new_key", 42);
    assert(!result); // Should return false since key doesn't exist
    
    // Test setIfNotExists - key doesn't exist
    result = config.setIfNotExists("new_key", 42);
    assert(result); // Should return true since key doesn't exist
    assert(config.has("new_key"));
    
    auto val = config.get<int>("new_key");
    assert(val.has_value());
    assert(*val == 42);
    
    // Test setIfExists on existing key
    result = config.setIfExists("new_key", 100);
    assert(result); // Should return true since key exists
    val = config.get<int>("new_key");
    assert(val.has_value());
    assert(*val == 100); // Value should be updated
    
    // Test setIfNotExists on existing key
    result = config.setIfNotExists("new_key", 200);
    assert(!result); // Should return false since key exists
    val = config.get<int>("new_key");
    assert(val.has_value());
    assert(*val == 100); // Value should not be updated
    
    std::cout << "    testConfigConvenienceSetters passed" << std::endl;
}

void testConfigFactoryMethods() {
    // Test minimal config
    nlm::Config minimal = nlm::Config::createMinimalConfig();
    assert(minimal.has("neuron_count"));
    assert(minimal.has("region_count"));
    assert(minimal.has("connection_probability"));
    
    // Test development config
    nlm::Config dev = nlm::Config::createDevelopmentConfig();
    assert(dev.getBool("debug.enabled").value_or(false));
    assert(dev.getBool("plasticity.stdp.enable").value_or(false));
    
    // Test production config
    nlm::Config prod = nlm::Config::createProductionConfig();
    assert(!prod.getBool("debug.enabled").value_or(true)); // Default should be false
    assert(prod.getString("performance.optimization").value_or("") == "aggressive");
    
    // Test research config
    nlm::Config research = nlm::Config::createResearchConfig();
    assert(research.getBool("research_mode").value_or(false));
    assert(research.getBool("record_every_step").value_or(false));
    
    std::cout << "    testConfigFactoryMethods passed" << std::endl;
}

void testConfigPresets() {
    // Test debug preset
    nlm::Config debug = nlm::Config::createConfigWithPreset("debug");
    assert(debug.getBool("debug.enabled").value_or(false));
    assert(debug.getString("debug.level").value_or("") == "verbose");
    
    // Test performance preset
    nlm::Config performance = nlm::Config::createConfigWithPreset("performance");
    assert(performance.getBool("debug.enabled").value_or(true)); // Default false, but development config sets it
    assert(performance.getString("performance.optimization").value_or("") == "speed");
    
    // Test accuracy preset
    nlm::Config accuracy = nlm::Config::createConfigWithPreset("accuracy");
    assert(accuracy.getBool("accuracy.enabled").value_or(false));
    assert(accuracy.getDouble("accuracy.tolerance").value_or(1.0) < 0.0001);
    
    // Test balanced preset
    nlm::Config balanced = nlm::Config::createConfigWithPreset("balanced");
    assert(balanced.getBool("debug.enabled").value_or(false));
    assert(balanced.getBool("debug.verbose").value_or(true)); // Default true, balanced sets to false
    
    // Test simple preset
    nlm::Config simple = nlm::Config::createConfigWithPreset("simple");
    assert(simple.getInt("neuron_count").value_or(0) == 1000);
    
    // Test cortical preset
    nlm::Config cortical = nlm::Config::createConfigWithPreset("cortical");
    assert(cortical.getInt("region_count").value_or(0) == 2);
    assert(cortical.getInt("neuron_count").value_or(0) == 5000);
    
    // Test demo preset
    nlm::Config demo = nlm::Config::createConfigWithPreset("demo");
    assert(demo.getInt("neuron_count").value_or(0) == 500);
    assert(demo.getBool("debug.verbose").value_or(false));
    
    // Test research preset
    nlm::Config research = nlm::Config::createConfigWithPreset("research");
    assert(research.getBool("research_mode").value_or(false));
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigCreateWithValues() {
    std::map<std::string, std::string> values;
    values["neuron_count"] = "2000";
    values["debug.enabled"] = "true";
    values["connection_probability"] = "0.15";
    values["name"] = "Test Brain";
    
    nlm::Config config = nlm::Config::createConfigWithValues(values);
    
    assert(config.getInt("neuron_count").value_or(0) == 2000);
    assert(config.getBool("debug.enabled").value_or(false));
    assert(config.getDouble("connection_probability").value_or(0.0) > 0.14);
    assert(config.getDouble("connection_probability").value_or(0.0) < 0.16);
    assert(config.getString("name").value_or("") == "Test Brain");
    
    std::cout << "    testConfigCreateWithValues passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.setInt("key1", 1);
    config1.setString("key2", "value1");
    config1.setDouble("key3", 3.14);
    
    nlm::Config config2;
    config2.setInt("key2", 2);
    config2.setString("key3", "value2");
    config2.setBool("key4", true);
    
    // Merge with override = true (default)
    config1.merge(config2);
    assert(config1.getInt("key1").value_or(0) == 1); // Should remain unchanged
    assert(config1.getInt("key2").value_or(0) == 2); // Should be overridden
    assert(config1.getString("key3").value_or("") == "value2"); // Should be overridden (string overwrites double)
    assert(config1.getBool("key4").value_or(false)); // Should be added
    
    // Test merge with override = false
    nlm::Config config3 = nlm::Config::createMinimalConfig();
    config3.setInt("extra_key", 999);
    
    bool success = config3.merge(config1, false); // Don't override existing keys
    assert(success);
    assert(config3.getInt("extra_key").value_or(0) == 999); // Should remain
    assert(config3.getInt("key2").value_or(0) == 2); // Should remain (was overridden from original)
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigBuilder() {
    auto config = nlm::Config::builder()
        .withBrain(500, 2, 0.2f)
        .withDevelopment(true, true, true, true)
        .withDevelopmentRates(0.002f, 0.0002f, 0.02f)
        .withPruningRates(0.00002f, 0.0002f, 0.002f, 0.2f)
        .withSynaptogenesis(0.002f, 0.2f, 200)
        .withNeuromodulation(true, true, true, true, true)
        .withPlasticity(true, true, true)
        .withSTDP(0.02f, 0.022f, 25.0f)
        .withMemory(true, true, true)
        .withPrediction(true)
        .withCognition(true, true, true)
        .withPlanningDepth(10)
        .withAttention(0.3f, 2.0f)
        .withSimulation(0.002, 123, 200, 2000)
        .withDebug(true, false, "debug")
        .withPerformance(60.0f, "balanced")
        .withAccuracy(true, 0.005f)
        .build();
    
    assert(config.getInt("neuron_count").value_or(0) == 500);
    assert(config.getInt("region_count").value_or(0) == 2);
    assert(config.getDouble("connection_probability").value_or(0.0) > 0.19);
    assert(config.getDouble("connection_probability").value_or(0.0) < 0.21);
    assert(config.getBool("debug.enabled").value_or(false));
    assert(config.getBool("debug.verbose").value_or(false));
    assert(config.getString("debug.level").value_or("") == "debug");
    assert(config.getDouble("maturation_rate").value_or(0.0) > 0.001);
    assert(config.getDouble("maturation_rate").value_or(0.0) < 0.003);
    
    std::cout << "    testConfigBuilder passed" << std::endl;
}

void testConfigCreateDefault() {
    nlm::Config config = nlm::Config::createDefaultConfig();
    
    assert(config.getInt("neuron_count").value_or(0) == 1000);
    assert(config.getBool("development.enabled").value_or(false));
    assert(config.getBool("neuromodulation.enabled").value_or(false));
    assert(config.getBool("plasticity.enabled").value_or(false));
    
    std::cout << "    testConfigCreateDefault passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeSafeConvenienceMethods();
    testConfigConvenienceSetters();
    testConfigFactoryMethods();
    testConfigPresets();
    testConfigCreateWithValues();
    testConfigMerge();
    testConfigBuilder();
    testConfigCreateDefault();
}

} // namespace test_config
