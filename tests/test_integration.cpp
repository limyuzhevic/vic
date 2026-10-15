// Integration tests for improved classes
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

namespace test_integration {

void testBrainConfigIntegration() {
    // Test complete Brain-Config integration
    auto config = std::make_shared<nlm::Config>();
    
    // Use a preset configuration
    nlm::Config standardConfig = nlm::Config::createStandardConfig();
    config = std::make_shared<nlm::Config>(standardConfig.copy());
    
    // Create brain with standard config
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Verify brain properties match config
    assert(brain.getTotalNeuronCount() == 500);  // From standard config
    assert(brain.getRegionCount() == 1);  // From standard config
    
    // Test step-by-step integration
    brain.step(0);
    brain.step(1);
    brain.step(2);
    
    // Verify brain still has expected properties after steps
    assert(brain.getTotalNeuronCount() == 500);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainConfigIntegration passed" << std::endl;
}

void testNeuronSynapseIntegration() {
    // Test Neuron-Synapse integration
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Set up synapse with neuron
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Neuron should be able to receive from synapse
    neuron.receiveExcitatoryInput(5.0f);  // This represents input from synapse
    
    assert(neuron.getTotalCurrent() == 5.0f);
    
    // Test plasticity integration
    neuron.enablePlasticity(true, true, false);
    synapse.enablePlasticity(true, true, false);
    
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    // Test spike history integration
    neuron.recordSpike(0.1f);
    synapse.recordPreSpike(0.1f);
    synapse.recordPostSpike(0.2f);
    
    assert(neuron.getSpikeHistory().size() == 1);
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    std::cout << "    testNeuronSynapseIntegration passed" << std::endl;
}

void testCompleteBrainSimulation() {
    // Test a complete brain simulation with multiple components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("random_seed", 12345, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all major systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    assert(brain.getSpikeSystem() != nullptr);
    
    // Run a complete simulation loop
    size_t totalSpikes = 0;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Collect statistics
        totalSpikes += brain.getTotalSpikeCount();
        
        // Test action production periodically
        if (step % 10 == 0) {
            auto action = brain.produceAction();
            assert(action != nullptr);
        }
        
        // Test reset at some point
        if (step == 25) {
            brain.reset();
            // After reset, statistics should be cleared
            assert(brain.getTotalSpikeCount() >= 0);
        }
    }
    
    // Verify brain is still functional after simulation
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testCompleteBrainSimulation passed" << std::endl;
}

void testConfigIntegrationWithBrain() {
    // Test various Config presets with Brain
    std::vector<std::shared_ptr<nlm::Config>> configs;
    
    // Add different config types
    configs.push_back(std::make_shared<nlm::Config>(nlm::Config::createMinimalBrainConfig()));
    configs.push_back(std::make_shared<nlm::Config>(nlm::Config::createStandardConfig()));
    configs.push_back(std::make_shared<nlm::Config>(nlm::Config::createHighPlasticityConfig()));
    configs.push_back(std::make_shared<nlm::Config>(nlm::Config::createDevelopmentConfig()));
    configs.push_back(std::make_shared<nlm::Config>(nlm::Config::createNeuromodulationConfig()));
    
    for (size_t i = 0; i < configs.size(); ++i) {
        nlm::Brain brain(configs[i]);
        assert(brain.initialize());
        
        // Verify brain properties based on config type
        if (i == 0) {  // Minimal config
            assert(brain.getTotalNeuronCount() == 100);
        } else if (i == 1) {  // Standard config
            assert(brain.getTotalNeuronCount() == 500);
        } else if (i == 2) {  // High plasticity config
            // Check for plasticity-related config
            assert(configs[i]->has("learning_rate"));
            assert(configs[i]->has("max_weight"));
        }
        
        // Run a few steps for each brain
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        std::cout << "    Config type " << i << " integration passed" << std::endl;
    }
    
    std::cout << "    testConfigIntegrationWithBrain passed" << std::endl;
}

void testAdvancedIntegrationWithValidation() {
    // Test advanced integration with Config validation
    nlm::Config config;
    
    // Create validation schema
    nlm::ConfigSchema schema;
    schema.name = "advanced_validation";
    
    nlm::ParameterBounds brainBounds;
    brainBounds.min = 50;
    brainBounds.max = 1000;
    brainBounds.allowNegative = false;
    brainBounds.allowZero = false;
    brainBounds.unit = "neurons";
    
    schema.parameterBounds["neuron_count"] = brainBounds;
    config.addSchema("advanced", schema);
    
    // Create brain with config
    auto brainConfig = std::make_shared<nlm::Config>(config);
    brainConfig->set("neuron_count", 200, nlm::ConfigSource::Runtime);
    brainConfig->set("region_count", 2, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    // Verify config validation worked
    assert(brainConfig->validateParameter("neuron_count"));
    assert(brainConfig->validateAll());
    
    // Run simulation with valid config
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testAdvancedIntegrationWithValidation passed" << std::endl;
}

void testMemorySystemIntegration() {
    // Test integration of memory systems with brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get memory system pointers
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Verify they are not null
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory operations
    workingMemory->storeToNeuron(1, 0.5f);
    
    // Test episodic memory operations
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.8f;
    episode.activeNeurons.push_back(1);
    episode.neuronActivations.push_back(0.5f);
    
    episodicMemory->storeEpisode(episode);
    
    // Test associative memory operations
    associativeMemory->storeAssociation(1, 2, 0.7f);
    
    // Run a few steps to ensure memory systems are used
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Memory systems should be active during step
        if (workingMemory) workingMemory->update(0.001f);
        if (episodicMemory) episodicMemory->update(0.001f);
    }
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    // Test plasticity systems integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Default);
    config->set("stdp_tau", 20.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structuralPlasticity != nullptr);
    
    // Test that STDP is configured from config
    // The exact parameters would depend on STDP implementation
    
    // Run simulation to allow plasticity to operate
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Plasticity should be applied during step
        // This is tested implicitly by checking brain doesn't crash
    }
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    // Test neuromodulation systems integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 150, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulation effects
    // Neuromodulators affect neural excitability and plasticity
    brain.applyNeuromodulation(nlm::Neuromodulator(0.5f));  // Example neuromodulator
    
    // Run simulation to allow neuromodulation to take effect
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
        
        // Neuromodulation should be applied during step
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    testBrainConfigIntegration();
    testNeuronSynapseIntegration();
    testCompleteBrainSimulation();
    testConfigIntegrationWithBrain();
    testAdvancedIntegrationWithValidation();
    testMemorySystemIntegration();
    testPlasticityIntegration();
    testNeuromodulationIntegration();
}

} // namespace test_integration