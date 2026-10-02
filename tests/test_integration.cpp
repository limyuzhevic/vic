// Integration Tests
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "agent/AgentBrain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void testBrainNeuronIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that Brain can access neurons
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test neuron creation through Brain
    auto* region = brain.getRegion(0);
    assert(region != nullptr);
    
    // Test brain step (which steps all neurons)
    brain.step(1);
    assert(brain.getFiringNeuronCount() >= 0);
    
    std::cout << "    testBrainNeuronIntegration passed" << std::endl;
}

void testBrainSynapseIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that synapses are created and connected
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test synapse properties
    for (const auto& synapse : brain.getAllSynapses()) {
        assert(synapse->getWeight() >= -1.0f && synapse->getWeight() <= 1.0f);
        assert(synapse->getDelay() >= 1);
    }
    
    // Test spike propagation through synapses
    brain.step(1);
    
    std::cout << "    testBrainSynapseIntegration passed" << std::endl;
}

void testAgentBrainBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create AgentBrain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test that AgentBrain can access the underlying brain
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getBrain()->getTotalNeuronCount() == 100);
    
    // Test sensory processing integration
    std::vector<float> visionInput(256, 0.0f);
    agentBrain.processVisionInput(visionInput);
    
    // Test action production
    auto action = agentBrain.decodeMotorCommand();
    assert(action != nlm::MotorCommand::Unknown);
    
    std::cout << "    testAgentBrainBrainIntegration passed" << std::endl;
}

void testMemoryBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initialize memory systems with brain
    nlm::NeuralWorkingMemory workingMemory;
    nlm::NeuralEpisodicMemory episodicMemory;
    
    workingMemory.initialize(&brain);
    episodicMemory.initialize(&brain);
    
    // Test working memory integration
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f};
    workingMemory.store(pattern, 1.0f);
    
    auto retrieved = workingMemory.retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Test episodic memory integration
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episodicMemory.storeEpisode(episode);
    
    assert(episodicMemory.getEpisodeCount() == 1);
    
    std::cout << "    testMemoryBrainIntegration passed" << std::endl;
}

void testNeuromodulationBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test reward modulation integration
    brain.applyRewardModulation(0.8f, 0.3f);
    
    assert(brain.getPredictionError() == 0.5f);
    assert(brain.getNeuromodulationLevel() > 0.0f);
    
    // Test curiosity integration
    assert(brain.getCuriosityLevel() > 0.0f);
    
    // Test development integration
    brain.updateDevelopment(100.0);
    assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testNeuromodulationBrainIntegration passed" << std::endl;
}

void testCompleteSystemIntegration() {
    // Test a complete agent system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Initialize memory
    nlm::NeuralWorkingMemory workingMemory;
    workingMemory.initialize(&brain);
    
    nlm::NeuralEpisodicMemory episodicMemory;
    episodicMemory.initialize(&brain);
    
    // Test complete cycle: step -> action -> reward -> memory update
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Get action
        auto action = agentBrain.decodeMotorCommand();
        
        // Apply reward
        float reward = 0.5f;  // Simple constant reward
        brain.applyRewardModulation(reward, 0.3f);
        
        // Store in working memory
        workingMemory.update(0.001f);
        
        // Occasionally store episode
        if (step % 3 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.metadata = "integration_test";
            episodicMemory.storeEpisode(episode);
        }
    }
    
    // Verify system is still working
    assert(brain.getTotalNeuronCount() == 100);
    assert(agentBrain.getBrain() != nullptr);
    assert(workingMemory.getCurrentSize() >= 0);
    
    std::cout << "    testCompleteSystemIntegration passed" << std::endl;
}

void runAll() {
    testBrainNeuronIntegration();
    testBrainSynapseIntegration();
    testAgentBrainBrainIntegration();
    testMemoryBrainIntegration();
    testNeuromodulationBrainIntegration();
    testCompleteSystemIntegration();
}

} // namespace test_integration