// Test program for enhanced NLM Phase 6 Config class
// This program tests the new advanced configuration methods

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

void test_basic_functionality() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get
    config.set("test_int", 42);
    config.set("test_string", std::string("hello"));
    config.set("test_double", 3.14);
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.get<int>("test_int") == 42);
    
    assert(config.has("test_string"));
    assert(config.get<std::string>("test_string") == "hello");
    
    assert(config.has("test_double"));
    assert(config.get<double>("test_double") > 3.13 && config.get<double>("test_double") < 3.15);
    
    assert(config.has("test_bool"));
    assert(config.get<bool>("test_bool") == true);
    
    std::cout << "✓ Basic functionality tests passed" << std::endl;
}

void test_advanced_configuration() {
    std::cout << "Testing advanced configuration methods..." << std::endl;
    
    nlm::Config config;
    
    // Test advanced brain configuration
    config.setBrainType("Phase6_Integrated");
    config.setNeuronCount(2000);
    config.setRegionCount(4);
    config.setConnectionProbability(0.15f);
    
    assert(config.getOr<std::string>("brain_type", "") == "Phase6_Integrated");
    assert(config.getOr<size_t>("neuron_count", 0) == 2000);
    assert(config.getOr<size_t>("region_count", 0) == 4);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    // Test advanced memory configuration
    config.setWorkingMemoryCapacity(500);
    config.setWorkingMemoryDecayRate(0.02f);
    config.setEpisodicMemoryMaxEpisodes(5000);
    config.setEpisodicMemoryConsolidationThreshold(0.7f);
    
    assert(config.getOr<size_t>("working_memory_capacity", 0) == 500);
    assert(config.getOr<float>("working_memory_decay_rate", 0.0f) == 0.02f);
    assert(config.getOr<size_t>("episodic_memory_max_episodes", 0) == 5000);
    assert(config.getOr<float>("episodic_memory_consolidation_threshold", 0.0f) == 0.7f);
    
    // Test advanced prediction configuration
    config.setPredictionDepth(8);
    config.setPredictionConfidenceThreshold(0.6f);
    config.setPredictionErrorSmoothing(0.9f);
    
    assert(config.getOr<int>("prediction_depth", 0) == 8);
    assert(config.getOr<float>("prediction_confidence_threshold", 0.0f) == 0.6f);
    assert(config.getOr<float>("prediction_error_smoothing", 0.0f) == 0.9f);
    
    // Test advanced neuromodulation configuration
    config.setDopaminePlasticityFactor(1.2f);
    config.setNoveltyDetectionThreshold(0.3f);
    config.setCuriosityExplorationWeight(0.8f);
    config.setSerotoninMoodInfluence(0.6f);
    config.setNorepinephrineArousalThreshold(0.4f);
    config.setAcetylcholineAttentionGain(1.5f);
    
    assert(config.getOr<float>("dopamine_plasticity_factor", 0.0f) == 1.2f);
    assert(config.getOr<float>("novelty_detection_threshold", 0.0f) == 0.3f);
    assert(config.getOr<float>("curiosity_exploration_weight", 0.0f) == 0.8f);
    assert(config.getOr<float>("serotonin_mood_influence", 0.0f) == 0.6f);
    assert(config.getOr<float>("norepinephrine_arousal_threshold", 0.0f) == 0.4f);
    assert(config.getOr<float>("acetylcholine_attention_gain", 0.0f) == 1.5f);
    
    // Test advanced configuration methods
    assert(config.hasAdvancedFeatures() == false); // No advanced features enabled by default
    
    std::cout << "✓ Advanced configuration tests passed" << std::endl;
}

void test_configuration_summary() {
    std::cout << "Testing configuration summary..." << std::endl;
    
    nlm::Config config;
    
    // Set some advanced configuration values
    config.setBrainType("Phase6_Integrated");
    config.setNeuronCount(1000);
    config.setWorkingMemoryCapacity(500);
    config.setPredictionDepth(5);
    config.setParallelProcessingEnabled(true);
    
    // Test summary generation
    std::string summary = config.getAdvancedConfigSummary();
    
    assert(!summary.empty());
    assert(summary.find("Advanced Configuration Summary") != std::string::npos);
    assert(summary.find("Phase6_Integrated") != std::string::npos);
    assert(summary.find("1000") != std::string::npos);
    assert(summary.find("500") != std::string::npos);
    assert(summary.find("5") != std::string::npos);
    
    // Test getAllAdvancedKeys
    std::vector<std::string> advanced_keys = config.getAllAdvancedKeys();
    assert(!advanced_keys.empty());
    
    // Check for some specific keys
    bool has_brain_keys = false;
    bool has_memory_keys = false;
    
    for (const auto& key : advanced_keys) {
        if (key == "brain_type" || key == "neuron_count") {
            has_brain_keys = true;
        }
        if (key == "working_memory_capacity" || key == "working_memory_decay_rate") {
            has_memory_keys = true;
        }
    }
    
    assert(has_brain_keys);
    assert(has_memory_keys);
    
    std::cout << "✓ Configuration summary tests passed" << std::endl;
}

void test_validation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test validation for string values
    bool valid_string = config.validateAdvancedSetting("brain_type", "Phase6_Test");
    assert(valid_string);
    
    // Test validation for size_t values
    bool valid_neuron_count = config.validateAdvancedSetting("neuron_count", 1000u);
    assert(valid_neuron_count);
    
    // Test validation for float values
    bool valid_probability = config.validateAdvancedSetting("connection_probability", 0.5f);
    assert(valid_probability);
    
    // Test validation for bool values
    bool valid_bool = config.validateAdvancedSetting("parallel_processing_enabled", true);
    assert(valid_bool);
    
    std::cout << "✓ Validation tests passed" << std::endl;
}

int main() {
    std::cout << "=== Enhanced NLM Phase 6 Config Class Tests ===" << std::endl;
    
    try {
        test_basic_functionality();
        test_advanced_configuration();
        test_configuration_summary();
        test_validation();
        
        std::cout << "\n=== All Tests Passed ===" << std::endl;
        std::cout << "The enhanced Config class is working correctly!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "✗ Test failed with unknown error" << std::endl;
        return 1;
    }
}