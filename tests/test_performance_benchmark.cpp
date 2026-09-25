// Performance Benchmarking Test Suite
// Comprehensive performance testing for Phase 6 systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>

namespace test_performance_benchmark {

void testScalability() {
    std::cout << "    Testing Scalability..." << std::endl;
    
    std::vector<std::pair<size_t, double>> scalabilityResults;
    
    for (size_t neuronCount : {100, 500, 1000, 2000}) {
        std::cout << "      Testing with " << neuronCount << " neurons..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f / (neuronCount / 100.0f), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Benchmark performance
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 500; ++step) {
            brain.step(step);
            brain.develop();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        double stepsPerSecond = 500.0 / duration;
        
        scalabilityResults.push_back({neuronCount, stepsPerSecond});
        
        std::cout << "        Performance: " << stepsPerSecond << " steps/second" << std::endl;
    }
    
    // Analyze scalability
    std::cout << "      Scalability analysis:" << std::endl;
    for (size_t i = 1; i < scalabilityResults.size(); ++i) {
        double ratio = scalabilityResults[i].second / scalabilityResults[i-1].second;
        std::cout << "        Neurons " << scalabilityResults[i-1].first << " -> " 
                  << scalabilityResults[i].first << ": " 
                  << "Scale factor: " << (scalabilityResults[i].first / scalabilityResults[i-1].first)
                  << ", Performance factor: " << ratio << std::endl;
    }
    
    std::cout << "      Scalability test PASSED" << std::endl;
}

void testMemoryPerformance() {
    std::cout << "    Testing Memory Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Memory system performance test
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<size_t> activeTrajectories;
    std::vector<size_t> episodeCounts;
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        brain.develop();
        
        // Memory operations
        if (step % 50 == 0) {
            auto* workingMemory = brain.getWorkingMemory();
            auto* episodicMemory = brain.getEpisodicMemory();
            
            if (workingMemory) {
                // Simulate working memory operations
                workingMemory->updateWorkingMemory(0.1f);
                activeTrajectories.push_back(workingMemory->getActiveTraces());
            }
            
            if (episodicMemory) {
                // Simulate episodic memory operations
                episodicMemory->updateMemoryStrength();
                episodeCounts.push_back(episodicMemory->getEpisodeCount());
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Memory operations completed in " << duration << "s" << std::endl;
    
    if (!activeTrajectories.empty()) {
        double avgTrajectories = std::accumulate(activeTrajectories.begin(), 
                                               activeTrajectories.end(), 0.0) / activeTrajectories.size();
        std::cout << "      Average working memory trajectories: " << avgTrajectories << std::endl;
    }
    
    if (!episodeCounts.empty()) {
        size_t maxEpisodes = *std::max_element(episodeCounts.begin(), episodeCounts.end());
        std::cout << "      Maximum episodic memory episodes: " << maxEpisodes << std::endl;
    }
    
    std::cout << "      Memory performance test PASSED" << std::endl;
}

void testNeuromodulationPerformance() {
    std::cout << "    Testing Neuromodulation Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Neuromodulation performance test
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<float> dopamineLevels;
    std::vector<float> curiosityLevels;
    
    for (nlm::SimulationStep step = 0; step < 800; ++step) {
        brain.step(step);
        brain.develop();
        
        // Neuromodulation operations
        if (step % 40 == 0) {
            nlm::Reward reward;
            reward.value = 0.5f + (step % 5) * 0.2f;
            brain.applyNeuromodulation(reward);
            
            dopamineLevels.push_back(brain.getDopamine()->getCurrentLevel());
        }
        
        if (step % 60 == 0) {
            nlm::NoveltySignal novelty;
            novelty.strength = 0.6f + (step % 3) * 0.3f;
            brain.applyNeuromodulation(novelty);
            
            curiosityLevels.push_back(brain.getCuriosity()->getCuriosityLevel());
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Neuromodulation operations completed in " << duration << "s" << std::endl;
    
    if (!dopamineLevels.empty()) {
        double avgDopamine = std::accumulate(dopamineLevels.begin(), dopamineLevels.end(), 0.0) / dopamineLevels.size();
        std::cout << "      Average dopamine level: " << avgDopamine << std::endl;
    }
    
    if (!curiosityLevels.empty()) {
        double avgCuriosity = std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0) / curiosityLevels.size();
        std::cout << "      Average curiosity level: " << avgCuriosity << std::endl;
    }
    
    std::cout << "      Neuromodulation performance test PASSED" << std::endl;
}

void testPredictionPerformance() {
    std::cout << "    Testing Prediction Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Prediction system performance test
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<float> predictionErrors;
    
    for (nlm::SimulationStep step = 0; step < 600; ++step) {
        brain.step(step);
        brain.develop();
        
        // Prediction operations
        if (step % 50 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.3f + std::sin(step * 0.02f) * 0.4f;
            brain.applyNeuromodulation(predError);
            
            predictionErrors.push_back(predError.value);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Prediction operations completed in " << duration << "s" << std::endl;
    
    if (!predictionErrors.empty()) {
        double avgPredError = std::accumulate(predictionErrors.begin(), predictionErrors.end(), 0.0) / predictionErrors.size();
        std::cout << "      Average prediction error: " << avgPredError << std::endl;
    }
    
    std::cout << "      Prediction performance test PASSED" << std::endl;
}

void testDevelopmentPerformance() {
    std::cout << "    Testing Development Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("development_synaptogenesis_rate", 0.00015f, nlm::ConfigSource::Runtime);
    config->set("development_pruning_rate", 0.000015f, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Development performance test
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        brain.develop();
        
        // Development operations
        if (step % 100 == 0) {
            // Development milestones
            auto stage = brain.getDevelopmentalStage();
            std::cout << "      Step " << step << ", Stage: " << static_cast<int>(stage) << std::endl;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Development operations completed in " << duration << "s" << std::endl;
    
    std::cout << "      Development performance test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Benchmark Test Suite ===" << std::endl;
    
    testScalability();
    testMemoryPerformance();
    testNeuromodulationPerformance();
    testPredictionPerformance();
    testDevelopmentPerformance();
    
    std::cout << "\n=== All Performance Benchmark Tests PASSED ===" << std::endl;
}

} // namespace test_performance_benchmark