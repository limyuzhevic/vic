// Config Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace nlm;

void testConfigCreation() {
    Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    Config config;
    
    config.set("int_value", 42, ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    Config config;
    
    config.set("test", 123, ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    Config config;
    
    config.set("key1", 1, ConfigSource::Runtime);
    config.set("key2", 2, ConfigSource::Runtime);
    config.set("key3", 3, ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigValidationBasic() {
    Config config;
    
    // Test valid configuration
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::File);
    config.set("region_count", static_cast<int64_t>(5), ConfigSource::File);
    config.set("connection_probability", 0.1f, ConfigSource::File);
    
    assert(config.validate());
    
    // Test invalid configuration (region_count > neuron_count)
    config.clear();
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::File);
    config.set("region_count", static_cast<int64_t>(200), ConfigSource::File);
    
    assert(!config.validate());
    
    std::cout << "    testConfigValidationBasic passed" << std::endl;
}

void testConfigFileValidation() {
    Config config;
    std::string testFile = "test_config_validation.txt";
    
    // Create a valid test configuration file
    std::ofstream file(testFile);
    file << "neuron_count = 100\n";
    file << "region_count = 5\n";
    file << "connection_probability = 0.2\n";
    file << "stdp_ltp_weight = 0.01\n";
    file << "stdp_ltd_weight = 0.012\n";
    file << "stdp_tau = 20.0\n";
    file << "synaptogenesis_rate = 0.0001\n";
    file << "pruning_rate = 0.00001\n";
    file << "simulation_timestep = 0.001\n";
    file << "replay_interval = 100\n";
    file << "consolidation_interval = 1000\n";
    file.close();
    
    // Test file validation with validation only
    bool validationPassed = config.loadFromFile(testFile, true);
    assert(validationPassed);
    
    // Test actual file loading
    config.clear();
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess);
    assert(config.validate());
    
    // Test invalid file format
    std::ofstream invalidFile("invalid_config.txt");
    invalidFile << "invalid_line_without_equals\n";
    invalidFile << "neuron_count = 100\n";
    invalidFile.close();
    
    config.clear();
    bool invalidLoad = config.loadFromFile("invalid_config.txt");
    assert(!invalidLoad);
    
    std::remove(testFile.c_str());
    std::remove("invalid_config.txt");
    
    std::cout << "    testConfigFileValidation passed" << std::endl;
}

void testConfigCommandLineValidation() {
    Config config;
    
    const char* argv[] = {"test", "--neuron_count=100", "--region_count=5", "--connection_probability=0.2"};
    int argc = 4;
    
    bool loadSuccess = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(loadSuccess);
    assert(config.validate());
    
    // Test invalid command line argument
    const char* invalidArgv[] = {"test", "--neuron_count=1000000", "--region_count=5", "--connection_probability=0.2"};
    int invalidArgc = 4;
    
    config.clear();
    bool invalidLoad = config.loadFromArgs(invalidArgc, const_cast<char**>(invalidArgv));
    assert(!invalidLoad);
    
    std::cout << "    testConfigCommandLineValidation passed" << std::endl;
}

void testConfigParameterValidation() {
    auto& validator = ConfigValidator();
    
    std::vector<ConfigEntry> validEntries;
    validEntries.emplace_back("neuron_count", ConfigValue(static_cast<int64_t>(100)), ConfigSource::File);
    validEntries.emplace_back("region_count", ConfigValue(static_cast<int64_t>(5)), ConfigSource::File);
    validEntries.emplace_back("connection_probability", ConfigValue(0.1f), ConfigSource::File);
    
    auto validErrors = validator.validate(validEntries);
    assert(validErrors.empty());
    
    std::vector<ConfigEntry> invalidEntries;
    invalidEntries.emplace_back("neuron_count", ConfigValue(static_cast<int64_t>(2000000)), ConfigSource::File);
    invalidEntries.emplace_back("region_count", ConfigValue(static_cast<int64_t>(5)), ConfigSource::File);
    
    auto invalidErrors = validator.validate(invalidEntries);
    assert(!invalidErrors.empty());
    
    std::cout << "    testConfigParameterValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidationBasic();
    testConfigFileValidation();
    testConfigCommandLineValidation();
    testConfigParameterValidation();
}

} // namespace test_config
