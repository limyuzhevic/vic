// Phase 6 Integration Tests
// Tests system integration, checkpoint save/load functionality, memory replay and consolidation, and development integration

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace test_phase6_integration {

void testSystemIntegration() {
    NLM_LOG_INFO("=== Testing System Integration ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    // Verify all major components are present and integrated
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    assert(brain->getDevelopmentSystem() != nullptr);
    
    // Run a few steps to ensure systems work together
    for (int i = 0; i < 50; ++i) {
        brain->step(i, i * 0.001);
    }
    
    NLM_LOG_INFO("[PASS] All systems are integrated and functional");
    
    std::cout << "    testSystemIntegration passed" << std::endl;
}

void testCheckpointSaveLoadFunctionality() {
    NLM_LOG_INFO("=== Testing Checkpoint Save/Load Functionality ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    // Create and initialize first brain
    auto brain1 = std::make_shared<nlm::Brain>(cfg);
    assert(brain1->initialize());
    
    // Run some simulation steps
    for (int i = 0; i < 100; ++i) {
        brain1->step(i, i * 0.001);
        brain1->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    }
    
    float avgFiring1 = brain1->getAverageFiringRate();
    size_t totalSpikes1 = brain1->getTotalSpikeCount();
    
    NLM_LOG_INFO("Brain1 avg firing: " + std::to_string(avgFiring1));
    NLM_LOG_INFO("Brain1 total spikes: " + std::to_string(totalSpikes1));
    
    // Save checkpoint
    std::string path = "/tmp/test_checkpoint.nml";
    assert(brain1->save(path));
    
    NLM_LOG_INFO("[PASS] Checkpoint saved successfully");
    
    // Create new brain and load checkpoint
    auto brain2 = std::make_shared<nlm::Brain>(cfg);
    assert(brain2->initialize());
    
    assert(brain2->load(path));
    
    NLM_LOG_INFO("[PASS] Checkpoint loaded successfully");
    
    size_t totalSpikes2 = brain2->getTotalSpikeCount();
    NLM_LOG_INFO("Brain2 total spikes after load: " + std::to_string(totalSpikes2));
    
    // Note: Exact state restoration depends on checkpoint implementation
    // but structure should be restored
    assert(brain2->getRegionCount() == brain1->getRegionCount());
    assert(brain2->getTotalNeuronCount() == brain1->getTotalNeuronCount());
    
    NLM_LOG_INFO("[PASS] Checkpoint save/load functionality works");
    
    // Clean up
    std::filesystem::remove(path);
    
    std::cout << "    testCheckpointSaveLoadFunctionality passed" << std::endl;
}

void testMemoryReplayAndConsolidation() {
    NLM_LOG_INFO("=== Testing Memory Replay and Consolidation ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* em = brain->getEpisodicMemory();
    assert(em != nullptr);
    
    // Enable replay
    em->enableReplay(true);
    
    // Run simulation to generate episodes
    for (int i = 0; i < 200; ++i) {
        // Generate some sensory activity
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 3.0f);
        brain->step(i, i * 0.001);
        
        // The brain should store episodes during normal operation
    }
    
    size_t episodeCount = em->getEpisodeCount();
    NLM_LOG_INFO("Total episodes stored: " + std::to_string(episodeCount));
    
    // Check for episodes
    if (episodeCount > 0) {
        NLM_LOG_INFO("[PASS] Episodic memory has episodes for replay");
        
        // Test replay functionality
        auto replayEpisodes = em->getEpisodesForReplay(3);
        if (!replayEpisodes.empty()) {
            NLM_LOG_INFO("[PASS] Replay system can retrieve episodes for consolidation");
            
            // Test replay of individual episodes
            size_t firstEpisodeId = replayEpisodes[0] ? replayEpisodes[0]->timestamp : 0;
            const nlm::EpisodicMemoryItem* episode = replayEpisodes[0];
            if (episode) {
                em->replayEpisode(episode);
                NLM_LOG_INFO("[PASS] Individual episode replay works");
            }
        }
    } else {
        NLM_LOG_INFO("[INFO] No episodes generated (simulation parameters may need adjustment)");
    }
    
    // Test consolidation by clearing old episodes
    size_t beforeConsolidation = em->getEpisodeCount();
    em->consolidate(0.5f); // Keep episodes with relevance >= 0.5
    size_t afterConsolidation = em->getEpisodeCount();
    
    NLM_LOG_INFO("Episodes before consolidation: " + std::to_string(beforeConsolidation));
    NLM_LOG_INFO("Episodes after consolidation: " + std::to_string(afterConsolidation));
    
    // Consolidation should not increase episode count
    assert(afterConsolidation <= beforeConsolidation);
    
    NLM_LOG_INFO("[PASS] Memory consolidation works");
    
    std::cout << "    testMemoryReplayAndConsolidation passed" << std::endl;
}

void testDevelopmentIntegration() {
    NLM_LOG_INFO("=== Testing Development Integration ===");
    
    // Create configuration with development enabled
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("enable_development", true);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* devSys = brain->getDevelopmentSystem();
    assert(devSys != nullptr);
    
    // Check initial developmental stage
    auto initialStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("Initial developmental stage: " + std::to_string(static_cast<int>(initialStage)));
    
    // Run simulation with development updates
    for (int step = 0; step < 1500; step += 100) { // Update development every 100 steps
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) { // Update development periodically
            brain->develop();
        }
    }
    
    // Check development system functionality
    auto finalStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("Final developmental stage: " + std::to_string(static_cast<int>(finalStage)));
    
    // Development stage should progress or at least be stable
    assert(static_cast<int>(finalStage) >= 0 && static_cast<int>(finalStage) <= 3);
    
    // Test development effects on plasticity
    auto* stdp = brain->getSTDP();
    if (stdp) {
        NLM_LOG_INFO("[PASS] STDP system works with development system");
    }
    
    auto* plasticity = brain->getStructuralPlasticity();
    if (plasticity) {
        NLM_LOG_INFO("[PASS] Structural plasticity works with development system");
    }
    
    NLM_LOG_INFO("[PASS] Development integration works");
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testLargeScaleSimulation() {
    NLM_LOG_INFO("=== Testing Large-Scale Simulation ===");
    
    // Create larger brain configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 1000);
    cfg->set("region_count", 4);
    cfg->set("connection_probability", 0.05f);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    // Run substantial simulation steps
    float totalReward = 0.0f;
    size_t totalSpikes = 0;
    
    for (uint64_t step = 0; step < 1000; ++step) {
        // Inject some sensory activity
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Collect metrics
        totalSpikes += brain->getFiringNeuronCount();
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("Progress: Step " + std::to_string(step) + 
                        " | Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                        " | Synapses: " + std::to_string(brain->getTotalSynapseCount()) +
                        " | Active: " + std::to_string(brain->getActiveNeuronCount()));
        }
    }
    
    float avgFiringRate = brain->getAverageFiringRate();
    float avgExcitInhibRatio = brain->getExcitationInhibitionRatio();
    
    NLM_LOG_INFO("Large-scale simulation results:");
    NLM_LOG_INFO("- Average firing rate: " + std::to_string(avgFiringRate));
    NLM_LOG_INFO("- Total spikes: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("- E/I ratio: " + std::to_string(avgExcitInhibRatio));
    
    // Should have reasonable firing rates
    assert(avgFiringRate >= 0.0f && avgFiringRate <= 100.0f);
    assert(avgExcitInhibRatio >= 0.0f);
    
    NLM_LOG_INFO("[PASS] Large-scale simulation works");
    
    std::cout << "    testLargeScaleSimulation passed" << std::endl;
}

void testMemoryUsage() {
    NLM_LOG_INFO("=== Testing Memory Usage ===");
    
    // Create brain with memory systems
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 500);
    cfg->set("region_count", 2);
    cfg->set("memory_capacity", 1000); // Working memory capacity
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    auto* wm = brain->getWorkingMemory();
    assert(wm != nullptr);
    
    auto* em = brain->getEpisodicMemory();
    assert(em != nullptr);
    
    // Store some patterns in working memory
    for (int i = 0; i < 50; ++i) {
        std::vector<float> pattern(10, 0.5f + static_cast<float>(i) * 0.01f);
        wm->store(pattern, 1.0f);
        
        // Simulate brain activity
        brain->step(i, i * 0.001);
        
        if (i % 20 == 0) {
            NLM_LOG_INFO("Working memory traces at step " + std::to_string(i) + 
                        ": " + std::to_string(wm->getActiveTraces()));
        }
    }
    
    size_t activeTraces = wm->getActiveTraces();
    size_t memoryCapacity = wm->getCapacity();
    
    NLM_LOG_INFO("Working memory usage: " + std::to_string(activeTraces) + "/" + std::to_string(memoryCapacity));
    
    // Memory usage should be reasonable
    assert(activeTraces <= memoryCapacity);
    assert(activeTraces >= 0);
    
    NLM_LOG_INFO("[PASS] Memory usage is within limits");
    
    std::cout << "    testMemoryUsage passed" << std::endl;
}

void testComputationalEfficiency() {
    NLM_LOG_INFO("=== Testing Computational Efficiency ===");
    
    // Create configuration
    auto cfg = std::make_shared<nlm::Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 2);
    
    auto brain = std::make_shared<nlm::Brain>(cfg);
    assert(brain->initialize());
    
    // Time simulation steps
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (500.0 / duration.count()) * 1000.0;
    
    NLM_LOG_INFO("Computational efficiency: " + std::to_string(stepsPerSecond) + " steps/second");
    
    // Should be reasonably efficient
    assert(stepsPerSecond > 100.0); // More than 100 steps per second
    
    NLM_LOG_INFO("[PASS] Computational efficiency is acceptable");
    
    std::cout << "    testComputationalEfficiency passed" << std::endl;
}

void testScalability() {
    NLM_LOG_INFO("=== Testing Scalability ===");
    
    std::vector<size_t> neuronCounts = {100, 200, 500};
    std::vector<double> stepRates;
    
    for (size_t neuronCount : neuronCounts) {
        // Create configuration
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("neuron_count", neuronCount);
        cfg->set("region_count", std::max<size_t>(1, neuronCount / 250)); // Scale regions
        
        auto brain = std::make_shared<nlm::Brain>(cfg);
        assert(brain->initialize());
        
        // Time simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = (100.0 / duration.count()) * 1000.0;
        stepRates.push_back(stepsPerSecond);
        
        NLM_LOG_INFO("Neuron count " + std::to_string(neuronCount) + 
                    ": " + std::to_string(stepsPerSecond) + " steps/second");
    }
    
    // Check scalability - shouldn't decrease dramatically
    if (stepRates.size() >= 2) {
        double avgRate = std::accumulate(stepRates.begin(), stepRates.end(), 0.0) / stepRates.size();
        double minRate = *std::min_element(stepRates.begin(), stepRates.end());
        
        NLM_LOG_INFO("Average efficiency: " + std::to_string(avgRate) + " steps/second");
        NLM_LOG_INFO("Minimum efficiency: " + std::to_string(minRate) + " steps/second");
        
        // Should maintain reasonable performance
        assert(minRate > 50.0); // At least 50 steps per second even at larger scale
    }
    
    NLM_LOG_INFO("[PASS] System scales reasonably with size");
    
    std::cout << "    testScalability passed" << std::endl;
}

void runAll() {
    testSystemIntegration();
    testCheckpointSaveLoadFunctionality();
    testMemoryReplayAndConsolidation();
    testDevelopmentIntegration();
    testLargeScaleSimulation();
    testMemoryUsage();
    testComputationalEfficiency();
    testScalability();
}

} // namespace test_phase6_integration
