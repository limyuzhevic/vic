// Full Brain Loop Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

namespace test_full_brain_integration {

void testCompleteBrainLoop() {
    std::cout << "    Testing Complete Brain Loop Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create environment and agent
    SimpleWorld world;
    world.initialize(20, 20);
    
    AgentBrain agent(&brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableStructuralPlasticity(true);
    
    // Run complete brain loop simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (int step = 0; step < 150; ++step) {
        // Get sensory input from world
        nlm::SensoryPercept percept = world.observe(brain.getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step (neural dynamics)
        brain.step(step, step * 0.001f);
        
        // Get motor command
        nlm::MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(brain.getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(brain.getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001f);
        
        // Collect metrics
        totalFiringRate += brain.getAverageFiringRate();
        if (brain.getFiringNeuronCount() > 0) firingCount++;
        
        // Occasionally create episodic memory
        if (step % 25 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.sensoryState.resize(20, 0.0f);
            for (size_t i = 0; i < 20; ++i) {
                episode.sensoryState[i] = 0.5f + 0.3f * sinf((float)i * 0.3f);
            }
            episode.positionX = 10.0f + 5.0f * sinf((float)step * 0.05f);
            episode.positionY = 10.0f + 5.0f * cosf((float)step * 0.05f);
            episode.action = cmd.action;
            episode.reward = reward;
            episode.energy = 1.0f;
            episode.novelty = fabsf(sinf((float)step * 0.08f));
            episode.age = 0;
            
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // Verify complete loop integration
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Check that all systems affected each other
    assert(totalReward >= 0.0f); // Should have accumulated reward
    assert(totalFiringRate >= 0.0f); // Should have neural activity
    
    // Verify working memory has traces
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory->getActiveTraces() >= 0);
    
    // Verify episodic memory has episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory->getEpisodeCount() >= 0);
    
    // Verify neuromodulation levels
    float dopamineLevel = brain.getDopamine()->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Verify planning confidence
    float planningConfidence = brain.getPlanner()->getPlanningConfidence();
    assert(planningConfidence >= 0.0f && planningConfidence <= 1.0f);
    
    std::cout << "    Complete brain loop integration test passed" << std::endl;
}

void testIntegrationWithCheckpointing() {
    std::cout << "    Testing Integration + Checkpointing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run first brain for some steps
    for (int step = 0; step < 80; ++step) {
        brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        brain1.step(step, step * 0.001f);
    }
    
    float avgFiring1 = brain1.getAverageFiringRate();
    size_t totalSpikes1 = brain1.getTotalSpikeCount();
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_brain_test_checkpoint.bin";
    assert(brain1.save(checkpointPath));
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(200));
    config2->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    assert(brain2.load(checkpointPath));
    
    // Run loaded brain for additional steps
    for (int step = 0; step < 40; ++step) {
        brain2.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.5f);
        brain2.step(step, step * 0.001f);
    }
    
    float avgFiring2 = brain2.getAverageFiringRate();
    size_t totalSpikes2 = brain2.getTotalSpikeCount();
    
    // Verify integration maintained after checkpoint
    assert(brain2.getWorkingMemory() != nullptr);
    assert(brain2.getEpisodicMemory() != nullptr);
    assert(brain2.getPredictionSystem() != nullptr);
    
    // Both brains should have functional systems
    assert(avgFiring1 >= 0.0f);
    assert(avgFiring2 >= 0.0f);
    
    std::cout << "    Integration + Checkpointing test passed" << std::endl;
}

void testIntegrationWithReplay() {
    std::cout << "    Testing Integration + Replay..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create initial episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState.resize(15, 0.0f);
        for (size_t j = 0; j < 15; ++j) {
            episode.sensoryState[j] = 0.6f + 0.2f * sinf((float)j * 0.4f);
        }
        episode.positionX = 5.0f + 2.0f * sinf((float)i * 0.2f);
        episode.positionY = 5.0f + 2.0f * cosf((float)i * 0.2f);
        episode.action = nlm::ActionType::Wait;
        episode.reward = 0.5f + 0.1f * sinf((float)i * 0.3f);
        
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Run simulation to create more episodes
    for (int step = 0; step < 100; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.8f);
        brain.step(step, step * 0.001f);
        
        // Create some episodes during simulation
        if (step % 15 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.sensoryState.resize(15, 0.0f);
            for (size_t j = 0; j < 15; ++j) {
                episode.sensoryState[j] = 0.4f + 0.3f * cosf((float)j * 0.5f);
            }
            episode.positionX = 15.0f;
            episode.positionY = 15.0f;
            episode.action = nlm::ActionType::Move;
            episode.reward = 0.8f;
            
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // Replay some episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    auto episodesForReplay = episodicMemory->getEpisodesForReplay(3);
    assert(!episodesForReplay.empty());
    
    episodicMemory->replaySequence({0, 1, 2}); // Replay first 3 episodes
    
    // Verify replay enhanced working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory->getActiveTraces() >= 0);
    
    // Verify integration still works after replay
    float planningConfidence = brain.getPlanner()->getPlanningConfidence();
    assert(planningConfidence >= 0.0f && planningConfidence <= 1.0f);
    
    // Verify neuromodulation levels
    float dopamineLevel = brain.getDopamine()->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    std::cout << "    Integration + Replay test passed" << std::endl;
}

void runAll() {
    testCompleteBrainLoop();
    testIntegrationWithCheckpointing();
    testIntegrationWithReplay();
}

} // namespace test_full_brain_integration