// Complete System Integration Tests
// Phase 6: Comprehensive end-to-end system integration testing

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/DopamineSystem.hpp"
#include "neuromodulation/CuriositySystem.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

namespace test_integration_complete {

void testBrainAgentWorldIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    for (uint64_t step = 0; step < 50; ++step) {
        auto percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        brain.step(step, step * 0.001);
        auto cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        agent.applyRewardModulation(reward, 0.0f);
        
        if (config->getOr("enable_development", false)) {
            agent.updateDevelopment(0.001);
        }
        
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getTotalNeuronCount() == 200);
    }
    
    std::cout << "    testBrainAgentWorldIntegration passed" << std::endl;
}

void testMemoryNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_neuromodulation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    
    for (int step = 0; step < 100; ++step) {
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.5f);
        }
        
        brain.step(step);
        
        if (workingMemory->getActiveTraces() > 0) {
            std::cout << "    [INFO] Working memory active traces: " << workingMemory->getActiveTraces() << std::endl;
        }
        
        if (episodicMemory->getEpisodeCount() > 0 && step % 50 == 0) {
            auto episodes = episodicMemory->getEpisodesForReplay(3);
            if (!episodes.empty()) {
                std::cout << "    [INFO] Replaying " << episodes.size() << " episodes" << std::endl;
            }
        }
    }
    
    std::cout << "    testMemoryNeuromodulationIntegration passed" << std::endl;
}

void testCompleteSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("enable_neuromodulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction", true, nlm::ConfigSource::Default);
    config->set("enable_attention", true, nlm::ConfigSource::Default);
    config->set("enable_concept_formation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    SimpleWorld world;
    world.initialize(32, 32);
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableAllSystems();
    
    nlm::ExperimentMetrics metrics;
    
    for (uint64_t step = 0; step < 200; ++step) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        brain.step(step, step * 0.001);
        auto cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        
        agent.applyRewardModulation(reward, 0.0f);
        
        if (config->getOr("enable_development", false)) {
            agent.updateDevelopment(0.001);
        }
        
        metrics.totalReward += reward;
        metrics.totalSpikes += brain.getFiringNeuronCount();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.totalTime += std::chrono::duration<double>(endTime - startTime).count();
        
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getTotalNeuronCount() == 400);
        
        if (step % 50 == 0) {
            std::cout << "    Step " << step 
                      << ": Reward=" << reward
                      << ", Firing=" << brain.getAverageFiringRate()
                      << ", MemoryTraces=" << (brain.getWorkingMemory() ? brain.getWorkingMemory()->getActiveTraces() : 0)
                      << std::endl;
        }
    }
    
    std::cout << "    Complete system integration summary:" << std::endl;
    std::cout << "      Total steps: " << 200 << std::endl;
    std::cout << "      Total reward: " << metrics.totalReward << std::endl;
    std::cout << "      Average firing rate: " << (metrics.totalSpikes / 200.0f) << std::endl;
    std::cout << "      Average step time: " << (metrics.totalTime / 200.0f) << "s" << std::endl;
    
    std::cout << "    testCompleteSystemIntegration passed" << std::endl;
}

void testMemorySystemsTogether() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("episodic_memory_size", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    for (int step = 0; step < 150; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.3f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.2f);
        brain.step(step);
        
        if (step % 20 == 0) {
            int wmTraces = workingMemory->getActiveTraces();
            int emEpisodes = episodicMemory->getEpisodeCount();
            std::cout << "    Step " << step 
                      << ": WorkingMemoryTraces=" << wmTraces
                      << ", EpisodicEpisodes=" << emEpisodes
                      << std::endl;
        }
    }
    
    std::cout << "    testMemorySystemsTogether passed" << std::endl;
}

void runAll() {
    testBrainAgentWorldIntegration();
    testMemoryNeuromodulationIntegration();
    testCompleteSystemIntegration();
    testMemorySystemsTogether();
}

} // namespace test_integration_complete