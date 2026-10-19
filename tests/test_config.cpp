// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
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

void testConfigJSONSaveLoad() {
    nlm::Config config;
    
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("brain.threshold", -55.0, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    config.set("neuromodulation.dopamine.scale", 1.5, nlm::ConfigSource::Runtime);
    config.set("learning.rate", 0.001, nlm::ConfigSource::Runtime);
    
    bool saveSuccess = config.saveToFile("test_config.json");
    assert(saveSuccess);
    
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("test_config.json");
    assert(loadSuccess);
    
    auto neuronCount = config2.get<int>("brain.neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto stdDev = config2.get<double>("brain.threshold");
    assert(stdDev.has_value());
    assert(*stdDev == -55.0);
    
    auto stdPlasticity = config2.get<bool>("plasticity.stdp.enable");
    assert(stdPlasticity.has_value());
    assert(*stdPlasticity == true);
    
    auto stdScale = config2.get<double>("neuromodulation.dopamine.scale");
    assert(stdScale.has_value());
    assert(*stdScale == 1.5);
    
    auto stdRate = config2.get<double>("learning.rate");
    assert(stdRate.has_value());
    assert(*stdRate == 0.001);
    
    std::cout << "    testConfigJSONSaveLoad passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    std::vector<nlm::ConfigSchema> schema;
    
    nlm::ConfigSchema neuronCountSchema("brain.neuron_count", nlm::ConfigType::INTEGER, 
                                        "Total number of neurons", "1000", true);
    schema.push_back(neuronCountSchema);
    
    nlm::ConfigSchema thresholdSchema("brain.threshold", nlm::ConfigType::DOUBLE,
                                       "Neuron firing threshold (mV)", "-65.0", false);
    schema.push_back(thresholdSchema);
    
    nlm::ConfigSchema enablePlasticitySchema("plasticity.stdp.enable", nlm::ConfigType::BOOLEAN,
                                              "Enable STDP plasticity", "true", false);
    schema.push_back(enablePlasticitySchema);
    
    config.setSchema(schema);
    
    config.set("brain.neuron_count", 2000, nlm::ConfigSource::Runtime);
    config.set("brain.threshold", -50.0, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    
    bool isValid = config.validateAgainstSchema(schema);
    assert(isValid);
    
    nlm::Config config2;
    config2.setSchema(schema);
    config2.set("brain.threshold", -55.0, nlm::ConfigSource::Runtime);
    config2.set("plasticity.stdp.enable", false, nlm::ConfigSource::Runtime);
    
    bool isValid2 = config2.validateAgainstSchema(schema);
    assert(!isValid2);
    std::string error = config2.getLastError();
    assert(!error.empty());
    
    nlm::Config config3;
    config3.setSchema(schema);
    config3.set("brain.neuron_count", "invalid", nlm::ConfigSource::Runtime);
    
    bool isValid3 = config3.validateAgainstSchema(schema);
    assert(!isValid3);
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigHelp() {
    nlm::Config config;
    
    std::vector<nlm::ConfigSchema> schema;
    
    nlm::ConfigSchema neuronCountSchema("brain.neuron_count", nlm::ConfigType::INTEGER,
                                        "Total number of neurons", "1000", true);
    schema.push_back(neuronCountSchema);
    
    nlm::ConfigSchema thresholdSchema("brain.threshold", nlm::ConfigType::DOUBLE,
                                       "Neuron firing threshold (mV)", "-65.0", false);
    schema.push_back(thresholdSchema);
    
    config.setSchema(schema);
    
    std::string help = config.getSchemaHelp();
    assert(!help.empty());
    assert(help.find("brain.neuron_count") != std::string::npos);
    assert(help.find("brain.threshold") != std::string::npos);
    assert(help.find("INTEGER") != std::string::npos);
    assert(help.find("DOUBLE") != std::string::npos);
    
    std::cout << "    testConfigHelp passed" << std::endl;
}

void testConfigYAMLSaveLoad() {
    nlm::Config config;
    
    config.set("brain.neuron_count", 1500, nlm::ConfigSource::Runtime);
    config.set("brain.threshold", -60.0, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.tau_plus", 20.0, nlm::ConfigSource::Runtime);
    config.set("neuromodulation.curiosity.enable", true, nlm::ConfigSource::Runtime);
    
    bool saveSuccess = config.saveToFile("test_config.yaml");
    assert(saveSuccess);
    
    nlm::Config config2;
    bool loadSuccess = config2.loadFromYAML("test_config.yaml");
    assert(loadSuccess);
    
    auto neuronCount = config2.get<int>("brain.neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1500);
    
    auto threshold = config2.get<double>("brain.threshold");
    assert(threshold.has_value());
    assert(*threshold == -60.0);
    
    auto tauPlus = config2.get<double>("plasticity.stdp.tau_plus");
    assert(tauPlus.has_value());
    assert(*tauPlus == 20.0);
    
    auto enableCuriosity = config2.get<bool>("neuromodulation.curiosity.enable");
    assert(enableCuriosity.has_value());
    assert(*enableCuriosity == true);
    
    std::cout << "    testConfigYAMLSaveLoad passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONSaveLoad();
    testConfigSchemaValidation();
    testConfigHelp();
    testConfigYAMLSaveLoad();
}

} // namespace test_config
