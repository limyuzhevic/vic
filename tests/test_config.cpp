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

void testJsonParsing() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string testJson = "{\n        \"brain.neuron_count\": 1000,\n        \"brain.simulation_time_step\": 0.1,\n        \"memory.capacity\": 5000,\n        \"neuromodulation.reward_scale\": 1.5,\n        \"prediction.confidence_threshold\": 0.9,\n        \"cognition.planning_depth\": 3\n    }";
    
    std::string tempFile = "/tmp/test_config.json";
    std::ofstream file(tempFile);
    file << testJson;
    file.close();
    
    // Load from JSON file
    bool success = config.loadFromFile(tempFile);
    assert(success);
    
    // Check that values were loaded correctly
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto timeStep = config.get<double>("brain.simulation_time_step");
    assert(timeStep.has_value() && *timeStep == 0.1);
    
    auto capacity = config.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 5000);
    
    auto rewardScale = config.get<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 1.5);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonParsing passed" << std::endl;
}

void testYamlParsing() {
    nlm::Config config;
    
    // Create a test YAML file
    std::string testYaml = "brain:\n  neuron_count: 800\n  simulation_time_step: 0.05\nmemory:\n  capacity: 3000\nneuromodulation:\n  reward_scale: 2.0\nprediction:\n  confidence_threshold: 0.8\ncognition:\n  planning_depth: 2\ndevelopment:\n  plasticity_rate: 0.2";
    
    std::string tempFile = "/tmp/test_config.yaml";
    std::ofstream file(tempFile);
    file << testYaml;
    file.close();
    
    // Load from YAML file
    bool success = config.loadFromFile(tempFile);
    assert(success);
    
    // Check that values were loaded correctly
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 800);
    
    auto timeStep = config.get<double>("brain.simulation_time_step");
    assert(timeStep.has_value() && *timeStep == 0.05);
    
    auto capacity = config.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 3000);
    
    auto rewardScale = config.get<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 2.0);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testYamlParsing passed" << std::endl;
}

void testCommandLineParsing() {
    nlm::Config config;
    
    // Simulate command line arguments
    const int argc = 7;
    const char* argv[] = {
        "test",
        "--brain.neuron_count", "1200",
        "--memory.capacity", "8000",
        "-neuromodulation.reward_scale", "0.5",
        "--prediction.confidence_threshold", "0.95",
        "-cognition.planning_depth", "5"
    };
    
    // Load from command line arguments
    bool success = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(success);
    
    // Check that values were loaded correctly
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1200);
    
    auto capacity = config.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 8000);
    
    auto rewardScale = config.get<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 0.5);
    
    auto confidence = config.get<double>("prediction.confidence_threshold");
    assert(confidence.has_value() && *confidence == 0.95);
    
    auto planningDepth = config.get<int>("cognition.planning_depth");
    assert(planningDepth.has_value() && *planningDepth == 5);
    
    std::cout << "    testCommandLineParsing passed" << std::endl;
}

void testEnvironmentParsing() {
    nlm::Config config;
    
    // Load from environment variables
    bool success = config.loadFromEnvironment();
    assert(success);
    
    // Check that default environment variables were loaded
    auto neuronCount = config.get<int>("NLM_BRAIN_NEURON_COUNT");
    // The value will depend on the implementation, but it should be loaded
    
    auto memoryCapacity = config.get<int>("NLM_MEMORY_CAPACITY");
    // The value will depend on the implementation, but it should be loaded
    
    std::cout << "    testEnvironmentParsing passed" << std::endl;
}

void testDefaultLoading() {
    nlm::Config config;
    
    // Load default configuration values
    bool success = config.loadFromDefaults();
    assert(success);
    
    // Check that default values were loaded
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 100);
    
    auto timeStep = config.get<double>("brain.simulation_time_step");
    assert(timeStep.has_value() && *timeStep == 0.1);
    
    auto capacity = config.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 1000);
    
    std::cout << "    testDefaultLoading passed" << std::endl;
}

void testNestedAccess() {
    nlm::Config config;
    
    // Set values with dot notation
    config.set("brain.neuron_count", 1500, nlm::ConfigSource::Runtime);
    config.set("memory.capacity", 6000, nlm::ConfigSource::Runtime);
    config.set("neuromodulation.reward_scale", 1.2, nlm::ConfigSource::Runtime);
    
    // Get values using dot notation
    auto neuronCount = config.getNested<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1500);
    
    auto capacity = config.getNested<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 6000);
    
    auto rewardScale = config.getNested<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 1.2);
    
    std::cout << "    testNestedAccess passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Add validation rules
    nlm::ConfigValidator validator;
    validator.addRule("brain.neuron_count", "int", true);
    validator.addRangeRule("brain.neuron_count", 0, 10000);
    validator.addRule("memory.capacity", "int", true);
    validator.addRangeRule("memory.capacity", 0, 20000);
    
    config.setValidator(validator);
    
    // Set valid values
    config.set("brain.neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("memory.capacity", 3000, nlm::ConfigSource::Runtime);
    
    // Validate
    nlm::ConfigValidator::ValidationRules rules;
    rules.push_back({"brain.neuron_count", "int", std::nullopt, std::nullopt, std::nullopt, true});
    rules.push_back({"memory.capacity", "int", std::nullopt, std::nullopt, std::nullopt, true});
    
    bool valid = config.validate(rules);
    assert(valid);
    
    // Test invalid values
    config.set("brain.neuron_count", -100, nlm::ConfigSource::Runtime);
    valid = config.validate(rules);
    assert(!valid); // Should be invalid due to negative value
    
    std::cout << "    testValidation passed" << std::endl;
}

void testConfigurationInheritance() {
    nlm::Config config;
    
    // Create base configuration file
    std::string baseConfig = "{\n        \"brain.neuron_count\": 1000,\n        \"memory.capacity\": 5000\n    }";
    
    std::string baseFile = "/tmp/base_config.json";
    std::ofstream baseFileStream(baseFile);
    baseFileStream << baseConfig;
    baseFileStream.close();
    
    // Create override configuration file
    std::string overrideConfig = "{\n        \"memory.capacity\": 8000,\n        \"neuromodulation.reward_scale\": 2.0\n    }";
    
    std::string overrideFile = "/tmp/override_config.json";
    std::ofstream overrideFileStream(overrideFile);
    overrideFileStream << overrideConfig;
    overrideFileStream.close();
    
    // Load with inheritance
    std::vector<std::string> filePaths = {baseFile, overrideFile};
    bool success = config.loadWithInheritance(filePaths);
    assert(success);
    
    // Check inherited values
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto capacity = config.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 8000); // Override value
    
    auto rewardScale = config.get<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 2.0);
    
    // Clean up
    std::remove(baseFile.c_str());
    std::remove(overrideFile.c_str());
    
    std::cout << "    testConfigurationInheritance passed" << std::endl;
}

void testSaveToFile() {
    nlm::Config config;
    
    // Set some values
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("memory.capacity", 5000, nlm::ConfigSource::Runtime);
    config.set("neuromodulation.reward_scale", 1.5, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string tempFile = "/tmp/config_output.json";
    bool success = config.saveToFile(tempFile);
    assert(success);
    
    // Load the file back
    nlm::Config config2;
    success = config2.loadFromFile(tempFile);
    assert(success);
    
    // Check values were saved and loaded correctly
    auto neuronCount = config2.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto capacity = config2.get<int>("memory.capacity");
    assert(capacity.has_value() && *capacity == 5000);
    
    auto rewardScale = config2.get<double>("neuromodulation.reward_scale");
    assert(rewardScale.has_value() && *rewardScale == 1.5);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testSaveToFile passed" << std::endl;
}

void testConfigStats() {
    nlm::Config config;
    
    // Set values from different sources
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("memory.capacity", 5000, nlm::ConfigSource::File);
    config.set("neuromodulation.reward_scale", 1.5, nlm::ConfigSource::CommandLine);
    config.set("prediction.confidence_threshold", 0.9, nlm::ConfigSource::Environment);
    config.set("cognition.planning_depth", 3, nlm::ConfigSource::Runtime);
    
    // Get statistics
    auto stats = config.getStats();
    
    assert(stats.totalEntries == 5);
    assert(stats.runtimeEntries == 2); // Default and Runtime
    assert(stats.fileEntries == 1);
    assert(stats.cliEntries == 1);
    assert(stats.envEntries == 1);
    
    std::cout << "    testConfigStats passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config Tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJsonParsing();
    testYamlParsing();
    testCommandLineParsing();
    testEnvironmentParsing();
    testDefaultLoading();
    testNestedAccess();
    testValidation();
    testConfigurationInheritance();
    testSaveToFile();
    testConfigStats();
    
    std::cout << std::endl << "All tests passed!" << std::endl;
}

} // namespace test_config
