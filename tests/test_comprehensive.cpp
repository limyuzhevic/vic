// Additional comprehensive unit tests for NLM
// This file extends the basic test suite with more thorough testing
// of all NLM components and edge cases

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <random>

namespace test_comprehensive {

void testConfigValidation() {
    nlm::Config config;
    
    // Test basic configuration
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(!config.has("nonexistent_key"));
    
    // Test default values
    assert(config.getOr<int64_t>("neuron_count", 0) == 100);
    assert(config.getOr<int64_t>("region_count", 0) == 2);
    assert(config.getOr<float>("nonexistent", 42.0f) == 42.0f);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test that brain was created properly
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testWorldInteraction() {
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Test world properties
    assert(world.getWidth() == 10);
    assert(world.getHeight() == 10);
    assert(world.getSimulationTime() == 0.0);
    
    // Test position validation
    assert(world.isValidPosition(5.0, 5.0));
    assert(!world.isValidPosition(-1.0, 5.0));
    assert(!world.isValidPosition(5.0, 11.0));
    
    std::cout << "    testWorldInteraction passed" << std::endl;
}

void testSensoryInput() {
    nlm::Vision vision(8, 8, 3);
    
    // Create test data
    std::vector<float> testData(8 * 8 * 3, 0.5f);
    vision.setData(testData);
    
    assert(vision.getWidth() == 8);
    assert(vision.getHeight() == 8);
    assert(vision.getChannels() == 3);
    assert(vision.getData() == testData);
    
    std::cout << "    testSensoryInput passed" << std::endl;
}

void testAgentBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    nlm::AgentBrain agent(brain);
    
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Test that agent is properly connected
    assert(agent.getBrain() != nullptr);
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

void testMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (brain.initialize()) {
        // Test working memory
        nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
        if (workingMem) {
            workingMem->storeToNeuron(nlm::NeuronId(1), 0.5f);
            assert(workingMem->getActiveTraces() > 0);
        }
        
        // Test episodic memory
        nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
        if (episodicMem) {
            nlm::EpisodicMemoryItem item;
            item.timestamp = 1;
            item.reward = 0.5f;
            episodicMem->storeEpisode(item);
            assert(episodicMem->getEpisodeCount() > 0);
        }
        
        std::cout << "    testMemorySystems passed" << std::endl;
    }
}

void testNeuromodulation() {
    nlm::AgentBrain agent(nullptr);
    
    // Test neuromodulation initialization
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);
    
    // Verify neuromodulation effects
    assert(agent.getPredictionError() == 0.5f);
    
    std::cout << "    testNeuromodulation passed" << std::endl;
}

void testDevelopment() {
    nlm::AgentBrain agent(nullptr);
    
    // Test developmental stage initialization
    assert(agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Test development update
    agent.updateDevelopment(1.0);
    assert(agent.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopment passed" << std::endl;
}

void testPlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    // Test plasticity initialization
    synapse.enablePlasticity(true, true, false);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    
    // Test weight change
    synapse.addToWeight(0.1f);
    assert(synapse.getWeight() == 0.6f);
    
    std::cout << "    testPlasticity passed" << std::endl;
}

void testPerformance() {
    // Test performance monitoring
    nlm::Brain brain(nullptr); // Without config for performance test
    
    // Just ensure it doesn't crash
    assert(brain.getTotalNeuronCount() == 0);
    assert(brain.getTotalSynapseCount() == 0);
    
    std::cout << "    testPerformance passed" << std::endl;
}

void runAllComprehensiveTests() {
    testConfigValidation();
    testBrainInitialization();
    testWorldInteraction();
    testSensoryInput();
    testAgentBrainIntegration();
    testMemorySystems();
    testNeuromodulation();
    testDevelopment();
    testPlasticity();
    testPerformance();
    
    std::cout << "\n=== ALL COMPREHENSIVE TESTS PASSED ===" << std::endl;
}

} // namespace test_comprehensive
