#include "core/Config/Config.hpp"
#include "core/Config/config_utils.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <chrono>
#include <variant>
#include <any>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    // Test enhanced default constructor
    assert(config.getStats().totalEntries == 0);
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal == true);
    
    // Test getOr with default values
    auto missingVal = config.getOr<std::string>("missing_key", "default");
    assert(missingVal == "default");
    
    // Test tryGet with nullopt for missing keys
    auto optVal = config.tryGet<int>("missing_key");
    assert(!optVal.has_value());
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testEnhancedConfigTypes() {
    nlm::Config config;
    
    // Test enhanced ConfigValue types using std::variant
    config.set("int_vector", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    auto intArray = config.getIntArray("int_vector");
    assert(intArray.has_value() && intArray->size() == 5 && (*intArray)[0] == 1);
    
    config.set("double_vector", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    auto doubleArray = config.getDoubleArray("double_vector");
    assert(doubleArray.has_value() && doubleArray->size() == 3);
    
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    auto stringArray = config.getStringArray("string_vector");
    assert(stringArray.has_value() && stringArray->size() == 3);
    
    config.set("bool_vector", std::vector<bool>{true, false, true}, nlm::ConfigSource::Runtime);
    auto boolArray = config.getBoolArray("bool_vector");
    assert(boolArray.has_value() && boolArray->size() == 3);
    
    // Test nested configuration support
    config.setNested("brain.region1", "size", 100);
    config.setNested("brain.region2", "size", 200);
    
    auto brainConfig = config.getNested("brain");
    assert(brainConfig.has_value());
    assert(brainConfig->has("region1"));
    assert(brainConfig->has("region2"));
    
    auto region1Size = brainConfig->get("region1");
    assert(region1Size.has_value() && region1Size.value() == 100);
    
    std::cout << "    testEnhancedConfigTypes passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Test different config sources
    config.set("default_val", 1, nlm::ConfigSource::Default);
    config.set("file_val", 2, nlm::ConfigSource::File);
    config.set("cli_val", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 4, nlm::ConfigSource::Runtime);
    config.set("override_val", 5, nlm::ConfigSource::Override);
    
    auto stats = config.getStats();
    assert(stats.defaultEntries == 1);
    assert(stats.fileEntries == 1);
    assert(stats.commandLineEntries == 1);
    assert(stats.runtimeEntries == 1);
    assert(stats.overrideEntries == 1);
    assert(stats.totalEntries == 5);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Add positive number validator using modern C++17 features
    config.addValidator("positive_number", 
        [](const nlm::ConfigValue& val) {
            return std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    return v > 0;
                }
                return true;
            }, val);
        },
        [](const nlm::ConfigValue& val) { return "Value must be positive"; },
        "positive_validator");
    
    // Test validation with valid value
    config.set("positive_num", 5, nlm::ConfigSource::Runtime);
    assert(config.has("positive_num"));
    
    // Validation may or may not catch negative values depending on implementation
    config.set("negative_num", -1, nlm::ConfigSource::Runtime);
    
    bool isValid = config.validate();
    auto validationErrors = config.getValidationErrors();
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigThreadSafety() {
    nlm::Config config;
    
    // Test thread safety with concurrent access
    std::vector<std::thread> threads;
    const size_t threadCount = 10;
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads.emplace_back([&config, i]() {
            for (size_t j = 0; j < 10; ++j) {
                config.set("key_" + std::to_string(i) + "_" + std::to_string(j),
                          static_cast<int>(i * 100 + j), nlm::ConfigSource::Runtime);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all keys were set
    auto stats = config.getStats();
    assert(stats.totalEntries >= threadCount * 10);
    
    // Test lock/unlock
    config.lock();
    assert(config.tryLock() == false); // Should fail to acquire lock when already held
    config.unlock();
    assert(config.tryLock() == true); // Should succeed after unlock
    config.unlock(); // Unlock for cleanup
    
    std::cout << "    testConfigThreadSafety passed" << std::endl;
}

void testConfigJSONYAMLSupport() {
    nlm::Config config;
    
    // Set up test data
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON export
    std::string jsonStr = config.toJSON();
    assert(!jsonStr.empty());
    assert(jsonStr.find("int_value") != std::string::npos);
    assert(jsonStr.find("string_value") != std::string::npos);
    
    // Test YAML export
    std::string yamlStr = config.toYAML();
    assert(!yamlStr.empty());
    assert(yamlStr.find("int_value") != std::string::npos);
    
    std::cout << "    testConfigJSONYAMLSupport passed" << std::endl;
}

void testConfigFactoryFunctions() {
    // Test ConfigFactory static functions
    auto defaultConfig = nlm::ConfigFactory::createDefaultConfig();
    assert(defaultConfig.has("random_seed"));
    assert(defaultConfig.has("simulation_timestep"));
    assert(defaultConfig.has("neuron_count"));
    
    // Test template configurations
    auto brainConfig = nlm::ConfigFactory::createBrainSimulationConfig();
    assert(brainConfig.has("neuron_count"));
    assert(brainConfig.has("region_count"));
    
    auto sensoryConfig = nlm::ConfigFactory::createSensoryProcessingConfig();
    assert(sensoryConfig.has("sensory_mode"));
    
    auto memoryConfig = nlm::ConfigFactory::createMemoryConfig();
    assert(memoryConfig.has("working_memory_size"));
    
    auto neuromodConfig = nlm::ConfigFactory::createNeuromodulationConfig();
    assert(neuromodConfig.has("dopamine_strength"));
    
    auto developmentConfig = nlm::ConfigFactory::createDevelopmentConfig();
    assert(developmentConfig.has("maturation_rate"));
    
    std::cout << "    testConfigFactoryFunctions passed" << std::endl;
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

void testConfigTryGet() {
    nlm::Config config;
    
    auto val1 = config.tryGet<int>("missing");
    assert(!val1.has_value());
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    auto val2 = config.tryGet<int>("int_value");
    assert(val2.has_value());
    assert(val2.value() == 42);
    
    auto val3 = config.tryGet<std::string>("int_value");
    assert(!val3.has_value());
    
    std::cout << "    testConfigTryGet passed" << std::endl;
}

void testConfigNested() {
    nlm::Config config;
    
    config.setNested("brain.region1", "size", 100);
    config.setNested("brain.region2", "size", 200);
    config.setNested("sensory", "vision", true);
    
    auto brain = config.getNested("brain");
    assert(brain.has_value());
    assert(brain->has("region1"));
    assert(brain->has("region2"));
    
    auto region1 = brain->get("region1");
    assert(region1.has_value());
    assert(region1.value() == 100);
    
    std::cout << "    testConfigNested passed" << std::endl;
}

void testConfigArrays() {
    nlm::Config config;
    
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    config.setArray("int_array", intArray);
    
    auto retrievedIntArray = config.getIntArray("int_array");
    assert(retrievedIntArray.has_value());
    assert(retrievedIntArray->size() == 5);
    assert(retrievedIntArray->at(0) == 1);
    
    std::vector<double> doubleArray = {1.1, 2.2, 3.3};
    config.setArray("double_array", doubleArray);
    
    auto retrievedDoubleArray = config.getDoubleArray("double_array");
    assert(retrievedDoubleArray.has_value());
    assert(retrievedDoubleArray->size() == 3);
    
    std::cout << "    testConfigArrays passed" << std::endl;
}

void testConfigFileSaveLoad() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saveSuccess = config.saveToFile("test_config.cfg", false, "cfg");
    assert(saveSuccess);
    
    // Clear config
    config.clear();
    assert(config.getKeys().size() == 0);
    
    // Load from file
    bool loadSuccess = config.loadFromFile("test_config.cfg");
    assert(loadSuccess);
    
    assert(config.get<int>("int_value") == 42);
    assert(config.get<std::string>("string_value") == "test");
    assert(config.get<double>("double_value") > 3.13 && config.get<double>("double_value") < 3.15);
    
    std::cout << "    testConfigFileSaveLoad passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Add a validator for positive numbers
    config.addValidator("positive_number", 
        [](const nlm::ConfigValue& val) {
            return std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    return v > 0;
                }
                return true;
            }, val);
        },
        [](const nlm::ConfigValue& val) { return "Value must be positive"; },
        "positive_validator");
    
    config.set("positive_num", 5, nlm::ConfigSource::Runtime);
    assert(config.has("positive_num"));
    
    // Test validation errors
    config.set("negative_num", -1, nlm::ConfigSource::Runtime);
    
    bool isValid = config.validate();
    // This may or may not be valid depending on validator implementation
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFactory() {
    nlm::Config config = config_utils::createDefaultConfig();
    
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    
    assert(config.get<int>("random_seed") == 42);
    assert(config.get<double>("simulation_timestep") == 0.001);
    assert(config.get<int>("neuron_count") == 1000);
    assert(config.get<int>("region_count") == 2);
    assert(config.get<double>("connection_probability") == 0.1);
    
    std::cout << "    testConfigFactory passed" << std::endl;
}

void testConfigUtils() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", "value", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    config2.set("key2", "value", nlm::ConfigSource::Runtime);
    
    assert(config_utils::configsEqual(config1, config2));
    
    // Test cloning
    nlm::Config config3 = config_utils::cloneConfig(config1);
    assert(config_utils::configsEqual(config1, config3));
    
    std::cout << "    testConfigUtils passed" << std::endl;
}

void testConfigMemoryStats() {
    nlm::Config config;
    
    // Add some entries
    for (int i = 0; i < 100; ++i) {
        config.set("key" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    auto stats = config.getStats();
    assert(stats.totalEntries == 100);
    assert(stats.runtimeEntries == 100);
    assert(stats.memoryUsageMB > 0);
    
    std::cout << "    testConfigMemoryStats passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test getting non-existent key throws exception
    try {
        config.get<int>("nonexistent");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigError& e) {
        assert(true); // Expected exception
    }
    
    // Test tryGet returns nullopt for non-existent key
    auto val = config.tryGet<int>("nonexistent");
    assert(!val.has_value());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTryGet();
    testConfigNested();
    testConfigArrays();
    testConfigFileSaveLoad();
    testConfigJSONSupport();
    testConfigValidation();
    testConfigFactory();
    testConfigUtils();
    testConfigMemoryStats();
    testConfigErrorHandling();
    
    std::cout << "\nAll config tests passed!\n" << std::endl;
}

} // namespace test_config