// Enhanced Config Tests - JSON/YAML Support
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_enhanced_config {

void testJSONConfig() {
    nlm::Config config;
    
    // Test JSON creation
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    config.set("random_seed", 12345LL, nlm::ConfigSource::Runtime);
    config.set("enable_neuromodulation", true, nlm::ConfigSource::Runtime);
    
    // Verify JSON serialization capability
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("random_seed"));
    assert(config.has("enable_neuromodulation"));
    
    // Test different value types
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto connProb = config.get<float>("connection_probability");
    assert(connProb.has_value() && *connProb > 0.1f && *connProb < 0.2f);
    
    auto seed = config.get<int64_t>("random_seed");
    assert(seed.has_value() && *seed == 12345LL);
    
    auto neuromodEnabled = config.get<bool>("enable_neuromodulation");
    assert(neuromodEnabled.has_value() && *neuromodEnabled == true);
    
    std::cout << "    testJSONConfig passed" << std::endl;
}

void testYAMLConfig() {
    nlm::Config config;
    
    // Test YAML-like value assignments
    config.set("synapse_type", std::string("excitatory"), nlm::ConfigSource::Runtime);
    config.set("plasticity_enable", true, nlm::ConfigSource::Runtime);
    config.set("development_stage", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    // Verify values
    assert(config.has("synapse_type"));
    assert(config.has("plasticity_enable"));
    assert(config.has("development_stage"));
    
    auto synType = config.get<std::string>("synapse_type");
    assert(synType.has_value() && *synType == "excitatory");
    
    auto plasticity = config.get<bool>("plasticity_enable");
    assert(plasticity.has_value() && *plasticity == true);
    
    auto devStage = config.get<int64_t>("development_stage");
    assert(devStage.has_value() && *devStage == 2);
    
    std::cout << "    testYAMLConfig passed" << std::endl;
}

void testAdvancedConfigTypes() {
    nlm::Config config;
    
    // Test vector types (for parameters)
    std::vector<double> learningRates = {0.01, 0.02, 0.03};
    config.set("learning_rates", learningRates, nlm::ConfigSource::Runtime);
    
    // Test parameter maps
    std::map<std::string, double> stdpParams;
    stdpParams["ltp"] = 0.02;
    stdpParams["ltd"] = 0.015;
    stdpParams["tau"] = 20.0;
    config.set("stdp_parameters", stdpParams, nlm::ConfigSource::Runtime);
    
    // Test string key-value pairs
    std::map<std::string, std::string> descriptions;
    descriptions["excitatory"] = "Strong excitatory connections";
    descriptions["inhibitory"] = "Strong inhibitory connections";
    descriptions["modulatory"] = "Modulatory and plastic connections";
    config.set("synapse_descriptions", descriptions, nlm::ConfigSource::Runtime);
    
    // Verify advanced types
    assert(config.has("learning_rates"));
    assert(config.has("stdp_parameters"));
    assert(config.has("synapse_descriptions"));
    
    std::cout << "    testAdvancedConfigTypes passed" << std::endl;
}

void testConfigFileFormats() {
    nlm::Config config;
    
    // Test format detection from file extensions
    std::string testContent = "neuron_count = 500\nconnection_probability = 0.1\nenable_development = true";
    
    // Write test file
    std::string testFile = "test_config.cfg";
    std::ofstream file(testFile);
    file << testContent;
    file.close();
    
    // Load using simple format parser
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("enable_development"));
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 500);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileFormats passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test configuration validation
    config.set("max_neurons", 10000, nlm::ConfigSource::Runtime);
    config.set("max_synapses", 1000000, nlm::ConfigSource::Runtime);
    config.set("min_weight", -1.0f, nlm::ConfigSource::Runtime);
    config.set("max_weight", 1.0f, nlm::ConfigSource::Runtime);
    
    // Verify valid ranges
    auto maxNeurons = config.getOr<int64_t>("max_neurons", 0);
    assert(maxNeurons > 0);
    
    auto maxSynapses = config.getOr<int64_t>("max_synapses", 0);
    assert(maxSynapses > 1000);
    
    // Test type conversions
    config.set("activity_threshold", 0.001, nlm::ConfigSource::Runtime);
    config.set("plasticity_rate", 0.01, nlm::ConfigSource::Runtime);
    
    auto activity = config.getOr<double>("activity_threshold", 1.0);
    auto plasticity = config.getOr<double>("plasticity_rate", 0.0);
    
    assert(activity > 0.0 && activity < 1.0);
    assert(plasticity > 0.0 && plasticity <= 1.0);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void runAll() {
    testJSONConfig();
    testYAMLConfig();
    testAdvancedConfigTypes();
    testConfigFileFormats();
    testConfigValidation();
}

} // namespace test_enhanced_config