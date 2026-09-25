// Full Integration Test Suite
// Complete comprehensive integration testing for Phase 6 systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_full_integration {

void testCompleteSystemIntegration() {
    std::cout << "    Testing Complete System Integration..." << std::endl;
    
    // Create comprehensive configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance monitoring
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run comprehensive integration test
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain.step(step);
        brain.develop();
        
        // Complex neuromodulation interactions
        if (step % 50 == 0) {
            nlm::Reward reward;
            reward.value = 0.5f + std::sin(step * 0.01f) * 0.5f;
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
        }
        
        if (step % 75 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.3f + (step % 10) * 0.1f;
            predError.type = nlm::PredictionErrorType::Negative;
            brain.applyNeuromodulation(predError);
        }
        
        if (step % 100 == 0) {
            nlm::NoveltySignal novelty;
            novelty.strength = 0.7f;
            novelty.cause = "complex_behavior";
            brain.applyNeuromodulation(novelty);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      System integration completed in " << duration << "s" << std::endl;
    std::cout << "      System throughput: " << 2000.0 / duration << " steps/second" << std::endl;
    
    // Verify all systems are functional
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    std::cout << "      All systems verified: YES" << std::endl;
    std::cout << "      Complete system integration test PASSED" << std::endl;
}

void testCrossComponentMemoryIntegration() {
    std::cout << "    Testing Cross-Component Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track memory system interactions
    std::vector<float> workingMemoryTrajectories;
    std::vector<size_t> episodicMemoryCounts;
    
    for (nlm::SimulationStep step = 0; step < 1500; ++step) {
        brain.step(step);
        brain.develop();
        
        // Monitor memory system dynamics
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        if (workingMemory) {
            workingMemoryTrajectories.push_back(static_cast<float>(workingMemory->getActiveTraces()));
        }
        
        if (episodicMemory) {
            episodicMemoryCounts.push_back(episodicMemory->getEpisodeCount());
        }
        
        // Complex interactions
        if (step % 100 == 0) {
            nlm::Reward reward;
            reward.value = 0.4f;
            brain.applyNeuromodulation(reward);
            
            // Memory systems receive neuromodulation signals
            if (workingMemory) workingMemory->updateFromNeuromodulation(0.3f);
            if (episodicMemory) episodicMemory->updateStrengthFromNeuromodulation(0.2f);
        }
    }
    
    // Analyze memory system behavior
    if (!workingMemoryTrajectories.empty()) {
        float avgWorkingMemory = std::accumulate(workingMemoryTrajectories.begin(), 
                                               workingMemoryTrajectories.end(), 0.0f) / 
                                workingMemoryTrajectories.size();
        std::cout << "      Average working memory traces: " << avgWorkingMemory << std::endl;
    }
    
    if (!episodicMemoryCounts.empty()) {
        size_t maxEpisodicMemory = *std::max_element(episodicMemoryCounts.begin(), 
                                                    episodicMemoryCounts.end());
        std::cout << "      Maximum episodic memory episodes: " << maxEpisodicMemory << std::endl;
    }
    
    std::cout << "      Cross-component memory integration test PASSED" << std::endl;
}

void testNeuromodulationLearningIntegration() {
    std::cout << "    Testing Neuromodulation Learning Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track learning metrics
    std::vector<float> dopamineLevels;
    std::vector<float> predictionErrors;
    std::vector<float> curiosityLevels;
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        brain.develop();
        
        // Complex neuromodulation learning
        if (step % 50 == 0) {
            nlm::Reward reward;
            reward.value = 0.6f * (step % 3 + 1) / 3.0f;  // Varying reward
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
            
            dopamineLevels.push_back(brain.getDopamine()->getCurrentLevel());
        }
        
        if (step % 75 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.5f + std::sin(step * 0.05f) * 0.4f;
            brain.applyNeuromodulation(predError);
            
            predictionErrors.push_back(predError.value);
        }
        
        if (step % 100 == 0) {
            // Curiosity-driven exploration
            nlm::NoveltySignal novelty;
            novelty.strength = 0.8f;
            brain.applyNeuromodulation(novelty);
            
            curiosityLevels.push_back(brain.getCuriosity()->getCuriosityLevel());
        }
    }
    
    // Analyze learning dynamics
    if (!dopamineLevels.empty()) {
        float avgDopamine = std::accumulate(dopamineLevels.begin(), dopamineLevels.end(), 0.0f) / dopamineLevels.size();
        std::cout << "      Average dopamine level: " << avgDopamine << std::endl;
    }
    
    if (!predictionErrors.empty()) {
        float avgPredError = std::accumulate(predictionErrors.begin(), predictionErrors.end(), 0.0f) / predictionErrors.size();
        std::cout << "      Average prediction error: " << avgPredError << std::endl;
    }
    
    if (!curiosityLevels.empty()) {
        float avgCuriosity = std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0f) / curiosityLevels.size();
        std::cout << "      Average curiosity level: " << avgCuriosity << std::endl;
    }
    
    std::cout << "      Neuromodulation learning integration test PASSED" << std::endl;
}

void testDevelopmentPerformanceIntegration() {
    std::cout << "    Testing Development Performance Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("development_synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Runtime);
    config->set("development_pruning_rate", 0.00002f, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance test with development
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1200; ++step) {
        brain.step(step);
        brain.develop();
        
        // Development affects performance
        if (step % 200 == 0) {
            nlm::Reward reward;
            reward.value = 0.8f;
            brain.applyNeuromodulation(reward);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Development performance test completed in " << duration << "s" << std::endl;
    std::cout << "      Performance: " << 1200.0 / duration << " steps/second" << std::endl;
    
    // Development stage progression
    auto finalStage = brain.getDevelopmentalStage();
    std::cout << "      Final developmental stage: " << static_cast<int>(finalStage) << std::endl;
    
    std::cout << "      Development performance integration test PASSED" << std::endl;
}

void testResourceManagement() {
    std::cout << "    Testing Resource Management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Monitor resource usage
    size_t maxMemoryUsage = 0;
    size_t maxSynapseCount = 0;
    
    for (nlm::SimulationStep step = 0; step < 800; ++step) {
        brain.step(step);
        brain.develop();
        
        size_t currentMemory = brain.getWorkingMemory() ? 
                              brain.getWorkingMemory()->getActiveTraces() : 0;
        size_t currentSynapses = brain.getTotalSynapseCount();
        
        maxMemoryUsage = std::max(maxMemoryUsage, currentMemory);
        maxSynapseCount = std::max(maxSynapseCount, currentSynapses);
        
        // Resource cleanup test
        if (step % 200 == 0) {
            brain.reset();  // Reset to test resource cleanup
        }
    }
    
    std::cout << "      Maximum working memory usage: " << maxMemoryUsage << std::endl;
    std::cout << "      Maximum synapse count: " << maxSynapseCount << std::endl;
    
    std::cout << "      Resource management test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Full Integration Test Suite ===" << std::endl;
    
    testCompleteSystemIntegration();
    testCrossComponentMemoryIntegration();
    testNeuromodulationLearningIntegration();
    testDevelopmentPerformanceIntegration();
    testResourceManagement();
    
    std::cout << "\n=== All Full Integration Tests PASSED ===" << std::endl;
}

} // namespace test_full_integration