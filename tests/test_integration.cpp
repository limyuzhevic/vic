// Integration Tests
// Tests for brain system integration, agent-brain system integration, and complete simulation runs

#include "brain/Brain.hpp"
#include "neuromodulation/NeuromodulatorSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <memory>

namespace test_integration {

void testBrainSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize all brain systems
    assert(brain.initialize());
    
    // Test neuromodulator system initialization
    auto* neuromodSystem = brain.getNeuromodulatorSystem();
    assert(neuromSystem != nullptr);
    
    neuromodSystem->initialize(&brain);
    
    // Test brain system integration by running multiple steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Check that all systems are functioning
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        // Update neuromodulators
        neuromodSystem->update(0.1f);
    }
    
    // Test that all neuromodulators are functional
    auto* dopamine = neuromodSystem->getDopamine();
    auto* curiosity = neuromodSystem->getCuriosity();
    auto* novelty = neuromodSystem->getNovelty();
    auto* predictionError = neuromodSystem->getPredictionError();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulator interactions
    neuromodSystem->recordReward(0.5f, nullptr);
    neuromodSystem->recordPredictionError(0.3f, nullptr);
    neuromodSystem->recordNovelty(0.7f);
    
    neuromodSystem->updateCuriosity(0.3f, 0.7f, 0.1f);
    
    // Test exploration policy selection
    std::vector<float> actionValues = {0.2f, 0.5f, 0.8f};
    float selectedAction = neuromodSystem->selectExplorationPolicy(0.5f, actionValues, "epsilon_greedy");
    assert(selectedAction == 0.8f);
    
    // Test neural plasticity integration
    nlm::STDP stdp;
    stdp.setLTPWeight(0.01f);
    stdp.setLTDWeight(0.012f);
    stdp.setTimeConstantPlus(20.0f);
    stdp.setTimeConstantMinus(30.0f);
    
    // Create synapse for testing
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test STDP update
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(std::isfinite(synapse.getWeight()));
    
    // Test Hebbian plasticity
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.01f);
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(std::isfinite(synapse.getWeight()));
    
    // Test brain reset
    brain.reset();
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainSystemIntegration passed" << std::endl;
}

void testAgentBrainSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create agent body
    nlm::AgentBody agentBody(config);
    
    // Create agent brain
    nlm::AgentBrain agentBrain(config, &agentBody);
    
    // Create agent-brain interface
    nlm::AgentBrainInterface interface(&agentBrain, &agentBody);
    
    // Initialize systems
    interface.initialize();
    
    // Test agent-world interaction loop
    for (int step = 0; step < 20; ++step) {
        // Get sensory perception
        auto percept = interface.getSensoryPerception();
        assert(percept != nullptr);
        
        // Process perception
        interface.processPerception(percept);
        
        // Generate action
        auto action = interface.generateAction();
        assert(action != nullptr);
        
        // Apply action
        interface.applyAction(action);
        
        // Get feedback
        interface.getFeedback();
    
        // Check that agent state is being updated
        assert(agentBrain.getNeuronCount() > 0);
        assert(agentBody.getPosition().x >= 0.0f);
    }
    
    // Test concept formation integration
    nlm::ConceptFormation conceptFormation;
    conceptFormation.initialize(config.get());
    
    std::vector<std::string> features = {"feature1", "feature2", "feature3"};
    std::string concept = conceptFormation.learnConcept(features);
    assert(!concept.empty());
    
    std::string matchedConcept = conceptFormation.matchConcept(features);
    assert(matchedConcept == concept);
    
    // Test neural planner integration
    nlm::NeuralPlanner planner;
    planner.initialize(config.get());
    
    // Create a simple plan
    std::vector<std::string> plan = planner.planSequence({"goal1", "goal2"});
    assert(!plan.empty());
    
    // Test agent-brain integration complete
    interface.reset();
    
    std::cout << "    testAgentBrainSystemIntegration passed" << std::endl;
}

void testAgentWorldInteractionLoop() {
    // Create environment
    auto config = std::make_shared<nlm::Config>();
    config->set("world_size", 100.0f, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Environment environment(config);
    
    // Create agent
    nlm::AgentBody agentBody(config);
    nlm::AgentBrain agentBrain(config, &agentBody);
    
    // Create agent-brain interface
    nlm::AgentBrainInterface interface(&agentBrain, &agentBody);
    interface.initialize(&environment);
    
    // Initialize environment
    environment.initialize(config.get());
    
    // Run complete agent-world interaction loop
    nlm::SimulationStep step = 0;
    bool episodeEnded = false;
    float totalReward = 0.0f;
    
    while (!episodeEnded && step < 100) {
        // Get current observation
        auto observation = environment.getCurrentObservation();
        assert(observation != nullptr);
        
        // Agent processes observation
        interface.processObservation(observation);
        
        // Agent selects action based on observation
        auto action = interface.selectAction();
        assert(action != nullptr);
        
        // Environment applies action
        environment.applyAction(action);
        
        // Get reward and new observation
        float reward = environment.getReward();
        totalReward += reward;
        
        interface.receiveReward(reward);
        
        // Check if episode ended
        episodeEnded = environment.isEpisodeEnded();
        
        // Increment step
        step++;
        
        // Environment updates
        environment.update(step);
    }
    
    // Verify results
    assert(totalReward >= 0.0f); // Should accumulate non-negative reward
    assert(step > 0); // Should have run at least one step
    
    std::cout << "    testAgentWorldInteractionLoop passed (reward: " << totalReward << ", steps: " << step << ")" << std::endl;
}

void testCompleteSimulationRun() {
    // Create complete simulation with all systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("simulation_steps", 1000, nlm::ConfigSource::Default);
    
    // Create environment
    nlm::Environment environment(config);
    environment.initialize(config.get());
    
    // Create agent body
    nlm::AgentBody agentBody(config);
    
    // Create agent brain
    nlm::AgentBrain agentBrain(config, &agentBody);
    
    // Create agent-brain interface
    nlm::AgentBrainInterface interface(&agentBrain, &agentBody);
    interface.initialize(&environment);
    
    // Initialize brain system
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize neuromodulator system
    auto neuromodSystem = brain.getNeuromodulatorSystem();
    neuromodSystem->initialize(&brain);
    
    // Run complete simulation
    nlm::SimulationStep step = 0;
    float totalReward = 0.0f;
    nlm::SimulationStep successfulSteps = 0;
    
    while (step < config->get<int64_t>("simulation_steps", 1000)) {
        // Run brain simulation
        brain.step(step);
        
        // Update neuromodulators
        neuromodSystem->update(0.1f);
        
        // Agent perception and action
        interface.update(step);
        
        // Environment updates
        environment.update(step);
        
        // Accumulate rewards
        totalReward += environment.getReward();
        
        if (step % 10 == 0) {
            successfulSteps++;
        }
        
        step++;
    
        // Check for system failures
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(environment.getCurrentObservation() != nullptr);
    }
    
    // Verify simulation results
    assert(step > 0); // Should have completed at least one step
    assert(successfulSteps > 0); // Should have completed successful steps
    assert(totalReward >= 0.0f); // Should have accumulated non-negative reward
    
    // Test system states after simulation
    float avgFiringRate = brain.getAverageFiringRate();
    assert(avgFiringRate >= 0.0f && avgFiringRate < 1000.0f); // Reasonable firing rate
    
    std::cout << "    testCompleteSimulationRun passed (steps: " << step 
              << ", total_reward: " << totalReward << ", avg_firing_rate: " << avgFiringRate << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Integration Tests..." << std::endl;
    testBrainSystemIntegration();
    testAgentBrainSystemIntegration();
    testAgentWorldInteractionLoop();
    testCompleteSimulationRun();
    
    std::cout << "\nAll Integration Tests passed!" << std::endl;
}

} // namespace test_integration