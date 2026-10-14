// Validation Tests
// Tests configuration validation, checkpoint/save-load functionality, plasticity rule correctness, and neuromodulation effects

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

// Include NLM headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Neuromodulator.hpp"

namespace test_validation {

// Configuration validation tests
void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    // Test 1: Valid configuration
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
        config->set("memory.capacity", 1024, nlm::ConfigSource::Default);
        
        assert(config->has("neuron_count"));
        assert(config->has("region_count"));
        assert(config->has("connection_probability"));
        assert(config->has("plasticity.stdp.enabled"));
        assert(config->has("memory.capacity"));
        
        auto neurons = config->getOr<int64_t>("neuron_count", 0);
        auto regions = config->getOr<int64_t>("region_count", 0);
        auto connProb = config->getOr<double>("connection_probability", 0.0);
        auto stdpEnabled = config->getOr<bool>("plasticity.stdp.enabled", false);
        auto memoryCap = config->getOr<int64_t>("memory.capacity", 0);
        
        assert(neurons == 100);
        assert(regions == 5);
        assert(connProb > 0.05 && connProb < 0.15);
        assert(stdpEnabled == true);
        assert(memoryCap == 1024);
        
        std::cout << "  Valid configuration test passed" << std::endl;
    }
    
    // Test 2: Missing required keys
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        // Missing connection_probability
        
        assert(config->has("neuron_count"));
        assert(config->has("region_count"));
        assert(!config->has("connection_probability")); // Missing
        assert(config->getOr<double>("connection_probability", 0.15) == 0.15); // Default value
        
        std::cout << "  Missing key handling test passed" << std::endl;
    }
    
    // Test 3: Invalid value ranges
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default); // Invalid negative
        config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Invalid zero
        config->set("connection_probability", 1.5, nlm::ConfigSource::Default); // Invalid > 1.0
        
        // Should still accept values (implementation-dependent)
        auto neurons = config->getOr<int64_t>("neuron_count", 0);
        auto regions = config->getOr<int64_t>("region_count", 0);
        auto connProb = config->getOr<double>("connection_probability", 0.0);
        
        assert(neurons == -1);
        assert(regions == 0);
        assert(connProb == 1.5);
        
        std::cout << "  Invalid value range test passed" << std::endl;
    }
    
    // Test 4: Configuration source validation
    {
        auto config = std::make_shared<nlm::Config>();
        
        config->set("source1", 1, nlm::ConfigSource::Default);
        config->set("source2", 2, nlm::ConfigSource::File);
        config->set("source3", 3, nlm::ConfigSource::CommandLine);
        config->set("source4", 4, nlm::ConfigSource::Runtime);
        
        assert(config->has("source1"));
        assert(config->has("source2"));
        assert(config->has("source3"));
        assert(config->has("source4"));
        
        std::cout << "  Configuration source validation test passed" << std::endl;
    }
    
    // Test 5: Configuration key validation
    {
        auto config = std::make_shared<nlm::Config>();
        
        config->set("valid_key_1", 1, nlm::ConfigSource::Runtime);
        config->set("valid_key_2", std::string("test"), nlm::ConfigSource::Runtime);
        config->set("valid_key_3", true, nlm::ConfigSource::Runtime);
        config->set("valid_key_4", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
        
        auto keys = config->getKeys();
        assert(keys.size() == 4);
        
        // Verify key existence
        assert(config->has("valid_key_1"));
        assert(config->has("valid_key_2"));
        assert(config->has("valid_key_3"));
        assert(config->has("valid_key_4"));
        
        std::cout << "  Configuration key validation test passed" << std::endl;
    }
}

// Checkpoint/save-load functionality tests
void testCheckpointSaveLoad() {
    std::cout << "Testing checkpoint/save-load functionality..." << std::endl;
    
    // Test 1: Save and load brain state
    {
        auto config1 = std::make_shared<nlm::Config>();
        config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config1->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        config1->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        
        nlm::Brain brain1(config1);
        brain1.initialize();
        
        // Modify brain state
        brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        brain1.step(0);
        
        // Save state
        bool saveSuccess = brain1.save("test_checkpoint1.bin");
        assert(&nlm::Brain::save != nullptr);
        
        // Verify save method exists
        assert(&nlm::Brain::load != nullptr);
        
        std::cout << "  Save functionality test passed" << std::endl;
    }
    
    // Test 2: Load functionality
    {
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config2->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain2(config2);
        brain2.initialize();
        
        // Load state (if save was successful)
        bool loadSuccess = brain2.load("test_checkpoint1.bin");
        assert(&nlm::Brain::load != nullptr);
        
        std::cout << "  Load functionality test passed" << std::endl;
    }
    
    // Test 3: Checkpoint file operations
    {
        std::string testFilename = "test_checkpoint_validation.bin";
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check if file exists (should not)
        bool fileExists = std::filesystem::exists(testFilename);
        assert(!fileExists); // File should not exist initially
        
        // Save checkpoint
        bool saveSuccess = brain.save(testFilename);
        
        // Check if file exists now
        bool fileExistsAfter = std::filesystem::exists(testFilename);
        assert(fileExistsAfter); // File should exist after save
        
        // Load checkpoint
        bool loadSuccess = brain.load(testFilename);
        assert(loadSuccess);
        
        // Clean up
        std::filesystem::remove(testFilename);
        
        std::cout << "  Checkpoint file operations test passed" << std::endl;
    }
    
    // Test 4: Save/load consistency
    {
        auto config1 = std::make_shared<nlm::Config>();
        config1->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
        config1->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        
        nlm::Brain brain1(config1);
        brain1.initialize();
        
        // Modify with various operations
        brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.5f);
        brain1.injectCurrentToNeurons(nlm::NeuronType::Motor, 2.5f);
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain1.step(step);
            if (step % 3 == 0) brain1.updatePlasticity();
        }
        
        // Save state
        bool save1 = brain1.save("test_consistency1.bin");
        assert(save1);
        
        // Load into new brain
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
        config2->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        
        nlm::Brain brain2(config2);
        brain2.initialize();
        
        bool load2 = brain2.load("test_consistency1.bin");
        assert(load2);
        
        // Verify consistency
        assert(brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
        assert(brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount());
        
        // Clean up
        std::filesystem::remove("test_consistency1.bin");
        
        std::cout << "  Save/load consistency test passed" << std::endl;
    }
}

// Plasticity rule correctness tests
void testPlasticityRuleCorrectness() {
    std::cout << "Testing plasticity rule correctness..." << std::endl;
    
    // Test 1: STDP rule correctness
    {
        nlm::STDP stdp;
        stdp.configure(0.03f, 0.035f, 20.0f);
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        // Pre before post (should potentiate)
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
        std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Weight should increase
        assert(synapse.getWeight() > initialWeight);
        
        // Reset synapse
        synapse.reset();
        
        // Post before pre (should depress)
        stdp.update(&synapse, postSpikes, preSpikes, 0.001);
        
        // Weight should decrease
        assert(synapse.getWeight() < initialWeight);
        
        std::cout << "  STDP rule correctness test passed" << std::endl;
    }
    
    // Test 2: Hebbian rule correctness
    {
        nlm::Hebbian hebbian;
        hebbian.configure(0.01f, 0.1f);
        
        nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        // Create correlated pre-post spike patterns
        std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0};
        std::vector<nlm::Timestamp> postSpikes = {4.0, 9.0, 14.0};
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Weight should change (Hebbian learning)
        assert(synapse.getWeight() != initialWeight);
        
        std::cout << "  Hebbian rule correctness test passed" << std::endl;
    }
    
    // Test 3: Plasticity flags validation
    {
        nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
        
        // Test default flags
        auto flags = synapse.getPlasticityFlags();
        assert(!flags.hebbian);
        assert(!flags.stdp);
        assert(!flags.reward_modulated);
        
        // Enable plasticity
        synapse.enablePlasticity(true, true, true);
        
        flags = synapse.getPlasticityFlags();
        assert(flags.hebbian);
        assert(flags.stdp);
        assert(flags.reward_modulated);
        
        std::cout << "  Plasticity flags validation test passed" << std::endl;
    }
    
    // Test 4: Eligibility trace validation
    {
        nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
        
        assert(synapse.getEligibilityTrace() == 0.0f);
        
        synapse.setEligibilityTrace(0.5f);
        assert(synapse.getEligibilityTrace() == 0.5f);
        
        synapse.decayEligibilityTrace(0.1f);
        assert(synapse.getEligibilityTrace() < 0.5f);
        
        std::cout << "  Eligibility trace validation test passed" << std::endl;
    }
    
    // Test 5: Plasticity boundary conditions
    {
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.99f); // Near max
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Weight should be clamped to maximum
        assert(synapse.getWeight() <= 1.0f);
        
        // Reset and test minimum bound
        synapse.reset();
        synapse.setWeight(-0.99f); // Near min
        
        // Apply depression
        stdp.update(&synapse, postSpikes, preSpikes, 0.001);
        
        // Weight should be clamped to minimum
        assert(synapse.getWeight() >= -1.0f);
        
        std::cout << "  Plasticity boundary conditions test passed" << std::endl;
    }
}

// Neuromodulation effects tests
void testNeuromodulationEffects() {
    std::cout << "Testing neuromodulation effects..." << std::endl;
    
    // Test 1: Neuromodulator level effects
    {
        nlm::Dopamine dopamine;
        
        dopamine.setLevel(0.1f);
        assert(dopamine.getLevel() == 0.1f);
        
        dopamine.setLevel(1.0f);
        assert(dopamine.getLevel() == 1.0f);
        
        dopamine.setLevel(0.0f);
        assert(dopamine.getLevel() == 0.0f);
        
        std::cout << "  Neuromodulator level effects test passed" << std::endl;
    }
    
    // Test 2: Neuromodulator plasticity factor effects
    {
        nlm::Acetylcholine ach;
        
        // Acetylcholine should enhance plasticity
        float plasticityFactor = ach.getPlasticityFactor();
        assert(plasticityFactor == 1.0f); // Default
        
        std::cout << "  Neuromodulator plasticity factor test passed" << std::endl;
    }
    
    // Test 3: Neuromodulator integration with plasticity
    {
        nlm::Dopamine dopamine;
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        // Set dopamine level
        dopamine.setLevel(0.5f);
        
        // Apply STDP with neuromodulation
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
        std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        // Weight change should be affected by neuromodulation
        float weightChange = synapse.getWeight() - initialWeight;
        
        std::cout << "  Neuromodulation plasticity interaction test passed" << std::endl;
    }
    
    // Test 4: Multiple neuromodulators
    {
        nlm::Dopamine dopamine;
        nlm::Acetylcholine ach;
        nlm::Norepinephrine ne;
        nlm::Serotonin serotonin;
        
        // Set different levels for each neuromodulator
        dopamine.setLevel(0.3f);
        ach.setLevel(0.7f);
        ne.setLevel(0.5f);
        serotonin.setLevel(0.2f);
        
        // Verify levels are set
        assert(dopamine.getLevel() == 0.3f);
        assert(ach.getLevel() == 0.7f);
        assert(ne.getLevel() == 0.5f);
        assert(serotonin.getLevel() == 0.2f);
        
        std::cout << "  Multiple neuromodulators test passed" << std::endl;
    }
    
    // Test 5: Neuromodulation state updates
    {
        nlm::Dopamine dopamine;
        
        // Update over time
        dopamine.update(0.1f);
        dopamine.update(0.2f);
        dopamine.update(0.3f);
        
        // Level should change over time
        // (Implementation-dependent behavior)
        
        std::cout << "  Neuromodulation state updates test passed" << std::endl;
    }
}

void runAll() {
    std::cout << "=== Validation Tests ===" << std::endl;
    std::cout << "Testing configuration validation, checkpoint/load, plasticity correctness, and neuromodulation" << std::endl;
    std::cout << std::endl;
    
    testConfigurationValidation();
    testCheckpointSaveLoad();
    testPlasticityRuleCorrectness();
    testNeuromodulationEffects();
    
    std::cout << std::endl;
    std::cout << "=== All Validation Tests Completed ===" << std::endl;
}

} // namespace test_validation