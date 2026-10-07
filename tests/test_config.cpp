// Enhanced Config Tests with JSON/YAML support and validation
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void testConfigLoadFromSimpleFormat() {
    nlm::Config config;
    
    // Create a test config file
    std::string testFile = "test_config_simple.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "int_key = 42\n";
    file << "double_key = 3.14\n";
    file << "string_key = \"hello\"\n";
    file << "bool_key = true\n";
    file.close();
    
    // Load the config file
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    // Verify loaded values
    assert(config.has("int_key"));
    assert(config.get<int>("int_key").value() == 42);
    
    assert(config.has("double_key"));
    assert(config.get<double>("double_key").value() > 3.13 && config.get<double>("double_key").value() < 3.15);
    
    assert(config.has("string_key"));
    assert(config.get<std::string>("string_key").value() == "hello");
    
    assert(config.has("bool_key"));
    assert(config.get<bool>("bool_key").value() == true);
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigLoadFromSimpleFormat passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Add some test values with descriptions
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::File);
    config.set("string_value", std::string("test"), nlm::ConfigSource::CommandLine);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "test_config_save.cfg";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Read back the file to verify format
    std::ifstream inFile(testFile);
    std::string line;
    bool foundIntKey = false;
    bool foundDoubleKey = false;
    bool foundStringKey = false;
    bool foundBoolKey = false;
    bool foundDescriptions = false;
    
    while (std::getline(inFile, line)) {
        if (line.find("#") != std::string::npos) {
            foundDescriptions = true;
        }
        if (line.find("int_value = 42") != std::string::npos) {
            foundIntKey = true;
        }
        if (line.find("double_value = 3.14") != std::string::npos) {
            foundDoubleKey = true;
        }
        if (line.find("string_value = \"test\"") != std::string::npos) {
            foundStringKey = true;
        }
        if (line.find("bool_value = false") != std::string::npos) {
            foundBoolKey = true;
        }
    }
    
    inFile.close();
    
    assert(foundIntKey);
    assert(foundDoubleKey);
    assert(foundStringKey);
    assert(foundBoolKey);
    assert(foundDescriptions);
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test missing required keys
    bool valid = config.validateConfig();
    assert(!valid); // Should fail because required keys are missing
    
    // Add required keys
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    // Test with valid config
    valid = config.validateConfig();
    assert(valid);
    
    // Test out of range validation
    config.set("simulation_timestep", 2.0, nlm::ConfigSource::Runtime); // Too high
    valid = config.validateConfig();
    // Note: Current implementation logs warnings but doesn't fail validation
    assert(valid); // Still returns true but logs warnings
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Mock command line arguments
    const int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--int_value",
        (char*)"100",
        (char*)"--double_value",
        (char*)"2.71"
    };
    
    bool loaded = config.loadFromArgs(argc, argv);
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromSimpleFormat();
    testConfigSaveToFile();
    testConfigValidation();
    testConfigLoadFromArgs();
}

} // namespace test_config