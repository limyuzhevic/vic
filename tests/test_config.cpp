// Config Tests
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

void testNestedKeySupport() {
    nlm::Config config;
    
    // Test nested key setting and getting
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.15, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.learning_rate", 0.001, nlm::ConfigSource::Runtime);
    
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synapse_density"));
    assert(config.has("plasticity.stdp.learning_rate"));
    
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto synapseDensity = config.get<double>("brain.synapse_density");
    assert(synapseDensity.has_value());
    assert(*synapseDensity == 0.15);
    
    auto stdpRate = config.get<double>("plasticity.stdp.learning_rate");
    assert(stdpRate.has_value());
    assert(*stdpRate == 0.001);
    
    // Test key prefix filtering
    auto brainKeys = config.getKeys("brain.");
    assert(brainKeys.size() == 2);
    assert(std::find(brainKeys.begin(), brainKeys.end(), "brain.neuron_count") != brainKeys.end());
    assert(std::find(brainKeys.begin(), brainKeys.end(), "brain.synapse_density") != brainKeys.end());
    
    std::cout << "    testNestedKeySupport passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Create a temporary config file with legacy format
    std::string testFile = "/tmp/test_config_legacy.cfg";
    std::ofstream outfile(testFile);
    outfile << "# Test legacy config file\n";
    outfile << "simple_key = simple_value\n";
    outfile << "number_key = 42\n";
    outfile << "float_key = 3.14\n";
    outfile.close();
    
    // Load using legacy format
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    assert(config.has("simple_key"));
    assert(config.has("number_key"));
    assert(config.has("float_key"));
    
    auto simpleVal = config.get<std::string>("simple_key");
    assert(simpleVal.has_value());
    assert(*simpleVal == "simple_value");
    
    auto numberVal = config.get<int>("number_key");
    assert(numberVal.has_value());
    assert(*numberVal == 42);
    
    auto floatVal = config.get<double>("float_key");
    assert(floatVal.has_value());
    assert(*floatVal > 3.13 && *floatVal < 3.15);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigCommandLine() {
    nlm::Config config;
    
    // Simulate command line arguments
    const int argc = 5;
    char* argv[argc];
    argv[0] = const_cast<char*("")("test");
    argv[1] = const_cast<char*>("--brain.neuron_count=500");
    argv[2] = const_cast<char*>("--plasticity.enable=true");
    argv[3] = const_cast<char*>("--random.seed=12345");
    argv[4] = const_cast<char*>("--test.value=3.14159");
    
    bool loaded = config.loadFromArgs(argc, argv);
    assert(loaded);
    
    assert(config.has("brain.neuron_count"));
    assert(config.has("plasticity.enable"));
    assert(config.has("random.seed"));
    assert(config.has("test.value"));
    
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 500);
    
    auto plasticityEnabled = config.get<bool>("plasticity.enable");
    assert(plasticityEnabled.has_value());
    assert(*plasticityEnabled == true);
    
    auto randomSeed = config.get<int64_t>("random.seed");
    assert(randomSeed.has_value());
    assert(*randomSeed == 12345);
    
    auto testValue = config.get<double>("test.value");
    assert(testValue.has_value());
    assert(*testValue > 3.141 && *testValue < 3.142);
    
    std::cout << "    testConfigCommandLine passed" << std::endl;
}

void testConfigClearAndSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::File);
    
    // Test clear
    config.clear();
    assert(config.getKeys().size() == 0);
    assert(!config.has("key1"));
    
    // Test summary
    config.set("test.nested.key", "value", nlm::ConfigSource::Runtime);
    config.set("another.key", 42, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("test.nested.key") != std::string::npos);
    assert(summary.find("another.key") != std::string::npos);
    assert(summary.find("total keys") != std::string::npos);
    
    std::cout << "    testConfigClearAndSummary passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("default_value", 1, nlm::ConfigSource::Default);
    config.set("file_value", 2, nlm::ConfigSource::File);
    config.set("command_line_value", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_value", 4, nlm::ConfigSource::Runtime);
    
    auto defaultVal = config.get<int>("default_value");
    assert(defaultVal.has_value() && *defaultVal == 1);
    
    auto fileVal = config.get<int>("file_value");
    assert(fileVal.has_value() && *fileVal == 2);
    
    auto commandLineVal = config.get<int>("command_line_value");
    assert(commandLineVal.has_value() && *commandLineVal == 3);
    
    auto runtimeVal = config.get<int>("runtime_value");
    assert(runtimeVal.has_value() && *runtimeVal == 4);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testNestedKeySupport();
    testConfigFileIO();
    testConfigCommandLine();
    testConfigClearAndSummary();
    testConfigSourceTracking();
}

} // namespace test_config
