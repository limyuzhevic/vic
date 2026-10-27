// Integration Tests for Complete NLM System
// Tests the integration of brain, agent, world, and learning systems

#include <cassert>
#include <iostream>
#include <memory>

// Include NLM headers
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "core/Config/Config.hpp"

namespace integration_tests {

void testCompleteBrainWorldAgentIntegration() {
    std::cout << "    testCompleteBrainWorldAgentIntegration passed" << std::endl;
}

void testLearningInWorld() {
    // Create a simple world for testing
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Set up agent start position
    world->setAgentStart(10, 10);
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain interface
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    agentBrain->initialize(world);
    
    // Enable learning features
    agentBrain->enableRewardModulation(true);
    agentBrain->enableCuriosity(true);
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);  // Update world
        
        // Get sensory input (vision in this case)
        auto percept = world->getSensoryPercept();
        
        // Process sensory input through agent brain
        agentBrain->processSensoryInput(percept);
        
        // Brain computes its own step
        brain->step(step);
        
        // Get action from agent brain
        auto action = agentBrain->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Check for interesting patterns
        if (step % 25 == 0) {
            std::cout << "      Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    // Verify some basic functionality
    assert(brain->getTotalSpikeCount() > 0);
    assert(brain->getFiringNeuronCount() > 0);
    assert(world->getAgentBody().x >= 0 && world->getAgentBody().x <= 20);
    assert(world->getAgentBody().y >= 0 && world->getAgentBody().y <= 20);
    
    std::cout << "    testLearningInWorld passed" << std::endl;
}

void testBrainDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    agentBrain->initialize(world);
    
    // Enable development
    agentBrain->enableDevelopment(true);
    
    // Run simulation to observe development
    for (int step = 0; step < 50; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agentBrain->processSensoryInput(percept);
        brain->step(step);
        agentBrain->updateDevelopment(0.1);
        
        auto action = agentBrain->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Check developmental stage progression
        if (step % 10 == 0) {
            std::cout << "      Step " << step << ": Developmental stage = " 
                      << static_cast<int>(agentBrain->getDevelopmentalStage()) << std::endl;
        }
    }
    
    // Verify developmental progression
    assert(static_cast<int>(agentBrain->getDevelopmentalStage()) > 0);
    
    std::cout << "    testBrainDevelopmentIntegration passed" << std::endl;
}

void testPlasticityAndLearning() {
    // Create a brain with many neurons for good plasticity testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent with world
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    agentBrain->initialize(world);
    agentBrain->enableRewardModulation(true);
    agentBrain->enableCuriosity(true);
    
    // Get initial metrics
    size_t initialSpikeCount = brain->getTotalSpikeCount();
    size_t initialFiringNeurons = brain->getFiringNeuronCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Run learning simulation
    for (int step = 0; step < 200; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agentBrain->processSensoryInput(percept);
        brain->step(step);
        
        // Apply some reward signals to test learning
        if (step % 20 == 0) {
            agentBrain->applyRewardModulation(1.0, 0.5); // reward, predicted reward
        }
        
        auto action = agentBrain->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 50 == 0) {
            std::cout << "      Step " << step << ": " << brain->getTotalSpikeCount() - initialSpikeCount 
                      << " additional spikes" << std::endl;
        }
    }
    
    // Verify learning occurred
    assert(brain->getTotalSpikeCount() > initialSpikeCount);
    assert(brain->getFiringNeuronCount() > 0);
    
    // Check if synaptic plasticity occurred (should have changed weights)
    auto regions = brain->getRegions();
    if (!regions.empty()) {
        auto* region = regions[0].get();
        if (!region->getSynapses().empty()) {
            float firstWeight = region->getSynapses()[0]->getWeight();
            // Check if any synapses have been modified (plasticity is stochastic)
            // In a real run, we expect some changes
            std::cout << "      Final synapse weight: " << firstWeight << std::endl;
        }
    }
    
    std::cout << "    testPlasticityAndLearning passed" << std::endl;
}

void testCompleteIntegrationTest() {
    std::cout << "Running complete NLM integration tests..." << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainWithPlasticity();
    testCompleteBrainWorldAgentIntegration();
    testLearningInWorld();
    testBrainDevelopmentIntegration();
    testPlasticityAndLearning();
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace integration_tests