// Compatibility Tests - Backward Compatibility
// Comprehensive backward compatibility test suite

#include "tests/utils/TestUtils.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_backward_compatibility {

void testBackwardCompatibilityConfigAPI() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigAPI");
    
    // Test that old Config API still works
    nlm::Config config;
    
    // Old-style API usage
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Old-style getters
    auto intVal = config.get<int>("int_value");
    auto doubleVal = config.get<double>("double_value");
    auto stringVal = config.get<std::string>("string_value");
    auto boolVal = config.get<bool>("bool_value");
    
    TestAssert::true(intVal.has_value(), "Old API int getter should work");
    TestAssert::equal(*intVal, 42, "Old API should return correct int value");
    
    TestAssert::true(doubleVal.has_value(), "Old API double getter should work");
    TestAssert::equal(*doubleVal, 3.14, 0.001, "Old API should return correct double value");
    
    TestAssert::true(stringVal.has_value(), "Old API string getter should work");
    TestAssert::equal(*stringVal, "test", "Old API should return correct string value");
    
    TestAssert::true(boolVal.has_value(), "Old API bool getter should work");
    TestAssert::true(*boolVal == true, "Old API should return correct bool value");
    
    std::cout << "    testBackwardCompatibilityConfigAPI passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityConfigTypes() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigTypes");
    
    // Test that old ConfigValue types still work
    nlm::Config config;
    
    // Set values using old patterns
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("double_value", 45.67, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello world", nlm::ConfigSource::Runtime);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("vector_double", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("vector_string", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Get values using old patterns
    auto intVal = config.get<int>("int_value");
    auto doubleVal = config.get<double>("double_value");
    auto stringVal = config.get<std::string>("string_value");
    auto boolVal = config.get<bool>("bool_value");
    
    TestAssert::true(intVal.has_value(), "Old API should retrieve int value");
    TestAssert::equal(*intVal, 123, "Int value should be correct");
    
    TestAssert::true(doubleVal.has_value(), "Old API should retrieve double value");
    TestAssert::equal(*doubleVal, 45.67, 0.001, "Double value should be correct");
    
    TestAssert::true(stringVal.has_value(), "Old API should retrieve string value");
    TestAssert::equal(*stringVal, "hello world", "String value should be correct");
    
    TestAssert::true(boolVal.has_value(), "Old API should retrieve bool value");
    TestAssert::true(*boolVal == false, "Bool value should be correct");
    
    // Test vector values (if supported)
    auto vectorIntVal = config.get<std::vector<int>>("vector_int");
    if (vectorIntVal.has_value()) {
        TestAssert::true(vectorIntVal->size() == 3, "Vector int should have 3 elements");
    }
    
    std::cout << "    testBackwardCompatibilityConfigTypes passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityConfigSource() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigSource");
    
    // Test that old ConfigSource enum values still work
    nlm::Config config;
    
    // Use different sources
    config.set("default_value", 1, nlm::ConfigSource::Default);
    config.set("file_value", 2, nlm::ConfigSource::File);
    config.set("command_line_value", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_value", 4, nlm::ConfigSource::Runtime);
    config.set("template_value", 5, nlm::ConfigSource::Template);
    
    // All values should be accessible
    TestAssert::true(config.has("default_value"), "Default source value should be accessible");
    TestAssert::true(config.has("file_value"), "File source value should be accessible");
    TestAssert::true(config.has("command_line_value"), "Command line source value should be accessible");
    TestAssert::true(config.has("runtime_value"), "Runtime source value should be accessible");
    TestAssert::true(config.has("template_value"), "Template source value should be accessible");
    
    // Test getOr with old patterns
    int defaultVal = config.getOr<int>("nonexistent", 99);
    TestAssert::equal(defaultVal, 99, "getOr should return default for missing key");
    
    std::cout << "    testBackwardCompatibilityConfigSource passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityConfigFileIO() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigFileIO");
    
    // Test that old file I/O patterns still work
    std::string testDir = "/tmp/nlm_compat_test_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::filesystem::create_directories(testDir);
    
    std::string filepath = testDir + "/compat_config.json";
    
    // Create config with old patterns
    nlm::Config config;
    config.set("name", "compat_test", nlm::ConfigSource::Runtime);
    config.set("version", 1.0, nlm::ConfigSource::File);
    config.set("enabled", true, nlm::ConfigSource::CommandLine);
    
    // Save using old patterns
    bool saveSuccess = config.saveToFile(filepath);
    TestAssert::true(saveSuccess, "saveToFile should work with old patterns");
    
    // Load using old patterns
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(filepath);
    TestAssert::true(loadSuccess, "loadFromFile should work with old patterns");
    
    // Verify loaded values
    auto nameVal = loadedConfig.get<std::string>("name");
    TestAssert::true(nameVal.has_value(), "Loaded config should have name");
    TestAssert::equal(*nameVal, "compat_test", "Loaded name should match");
    
    auto versionVal = loadedConfig.get<double>("version");
    TestAssert::true(versionVal.has_value(), "Loaded config should have version");
    TestAssert::equal(*versionVal, 1.0, 0.001, "Loaded version should match");
    
    auto enabledVal = loadedConfig.get<bool>("enabled");
    TestAssert::true(enabledVal.has_value(), "Loaded config should have enabled");
    TestAssert::true(*enabledVal == true, "Loaded enabled should match");
    
    // Cleanup
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testBackwardCompatibilityConfigFileIO passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityConfigValidation() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigValidation");
    
    // Test that old validation patterns still work
    nlm::Config config;
    
    // Create valid config
    config.set("name", "valid_test", nlm::ConfigSource::Runtime);
    config.set("value", 42, nlm::ConfigSource::Runtime);
    config.set("active", true, nlm::ConfigSource::Runtime);
    
    // Old validation pattern
    bool isValid = config.validateWith(*nlm::createDefaultConfigValidator());
    TestAssert::true(isValid, "Old validation pattern should work");
    
    // Create invalid config
    nlm::Config invalidConfig;
    invalidConfig.set("name", "", nlm::ConfigSource::Runtime); // Empty name might be invalid
    
    bool isInvalid = invalidConfig.validateWith(*nlm::createDefaultConfigValidator());
    TestAssert::false(isInvalid, "Invalid config should fail validation");
    
    std::cout << "    testBackwardCompatibilityConfigValidation passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityConfigFactory() {
    TestUtils::TestSession session("testBackwardCompatibilityConfigFactory");
    
    // Test that old factory function patterns still work
    // These factory functions should still be available
    auto validator = nlm::createDefaultConfigValidator();
    auto manager = nlm::createDefaultConfigManager();
    auto template = nlm::createConfigTemplate("default_template");
    auto history = nlm::createConfigHistory();
    
    TestAssert::true(validator != nullptr, "Factory functions should return non-null validators");
    TestAssert::true(manager != nullptr, "Factory functions should return non-null managers");
    TestAssert::true(template != nullptr, "Factory functions should return non-null templates");
    TestAssert::true(history != nullptr, "Factory functions should return non-null history objects");
    
    // Test convenience functions
    auto config = nlm::loadConfig("test.json");
    // This might return null if file doesn't exist, which is expected
    
    std::cout << "    testBackwardCompatibilityConfigFactory passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityNeuronAPI() {
    TestUtils::TestSession session("testBackwardCompatibilityNeuronAPI");
    
    // Test that old Neuron API still works
    nlm::Neuron neuron;
    
    // Old-style neuron usage
    neuron.setMembranePotential(-65.0);
    neuron.setRefractoryPeriod(2.0);
    neuron.setAdaptationConductance(0.5);
    
    TestAssert::equal(neuron.getMembranePotential(), -65.0, 0.001, "Old neuron API should set potential");
    TestAssert::equal(neuron.getRefractoryPeriod(), 2.0, 0.001, "Old neuron API should set refractory period");
    TestAssert::equal(neuron.getAdaptationConductance(), 0.5, 0.001, "Old neuron API should set adaptation conductance");
    
    // Test spike recording
    neuron.recordPreSpike(0.0);
    neuron.recordPostSpike(10.0);
    
    auto preSpikes = neuron.getRecentPreSpikes(20.0);
    auto postSpikes = neuron.getRecentPostSpikes(20.0);
    
    TestAssert::true(preSpikes.size() > 0, "Old neuron API should record pre-spikes");
    TestAssert::true(postSpikes.size() > 0, "Old neuron API should record post-spikes");
    
    std::cout << "    testBackwardCompatibilityNeuronAPI passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilitySynapseAPI() {
    TestUtils::TestSession session("testBackwardCompatibilitySynapseAPI");
    
    // Test that old Synapse API still works
    nlm::Synapse synapse;
    
    // Old-style synapse usage
    synapse.setWeight(0.5);
    synapse.setDelay(0.001);
    
    TestAssert::equal(synapse.getWeight(), 0.5, 0.001, "Old synapse API should set weight");
    TestAssert::equal(synapse.getDelay(), 0.001, 0.000001, "Old synapse API should set delay");
    
    // Test transmission
    double input = 1.0;
    double output = synapse.transmit(input);
    
    TestAssert::true(output >= 0.0, "Synapse transmission should produce non-negative output");
    
    std::cout << "    testBackwardCompatibilitySynapseAPI passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityBrainAPI() {
    TestUtils::TestSession session("testBackwardCompatibilityBrainAPI");
    
    // Test that old Brain API still works
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_unique<nlm::Brain>(config);
    
    // Initialize brain
    brain->initialize();
    
    // Old-style brain usage
    double time = 0.0;
    double reward = 0.5;
    
    brain->step(time, reward);
    
    // Test basic brain operations
    auto observation = brain->getObservation(time);
    double brainReward = brain->getReward();
    bool done = brain->isDone();
    
    TestAssert::true(observation.size() > 0, "Brain should provide observations");
    TestAssert::true(brainReward >= 0.0, "Brain reward should be non-negative");
    TestAssert::true(done == false, "Brain should not be done initially");
    
    std::cout << "    testBackwardCompatibilityBrainAPI passed" << std::endl;
    session.setSuccess(true);
}

void testBackwardCompatibilityExperimentAPI() {
    TestUtils::TestSession session("testBackwardCompatibilityExperimentAPI");
    
    // Test that old Experiment API still works
    nlm::Experiment experiment("backward_compat_test");
    
    // Set experiment parameters
    experiment.setConfigValue("learning_rate", 0.01);
    experiment.setConfigValue("population_size", 100);
    experiment.setConfigValue("max_steps", 1000);
    
    // Get experiment parameters
    auto learningRate = experiment.getConfigValue<double>("learning_rate");
    auto populationSize = experiment.getConfigValue<size_t>("population_size");
    auto maxSteps = experiment.getConfigValue<size_t>("max_steps");
    
    TestAssert::true(learningRate.has_value(), "Experiment API should get learning rate");
    TestAssert::equal(*learningRate, 0.01, "Learning rate should be correct");
    
    TestAssert::true(populationSize.has_value(), "Experiment API should get population size");
    TestAssert::equal(*populationSize, 100, "Population size should be correct");
    
    TestAssert::true(maxSteps.has_value(), "Experiment API should get max steps");
    TestAssert::equal(*maxSteps, 1000UL, "Max steps should be correct");
    
    std::cout << "    testBackwardCompatibilityExperimentAPI passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Backward Compatibility Tests ===" << std::endl;
    
    testBackwardCompatibilityConfigAPI();
    testBackwardCompatibilityConfigTypes();
    testBackwardCompatibilityConfigSource();
    testBackwardCompatibilityConfigFileIO();
    testBackwardCompatibilityConfigValidation();
    testBackwardCompatibilityConfigFactory();
    testBackwardCompatibilityNeuronAPI();
    testBackwardCompatibilitySynapseAPI();
    testBackwardCompatibilityBrainAPI();
    testBackwardCompatibilityExperimentAPI();
    
    std::cout << std::endl;
    std::cout << "=== All Backward Compatibility Tests PASSED ===" << std::endl;
}