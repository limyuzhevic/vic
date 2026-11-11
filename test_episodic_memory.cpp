#!/usr/bin/env g++

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main() {
    std::cout << "=== NLM Phase 6 Episodic Memory Test ===" << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Test episodic memory storage
    std::cout << "--- Testing Episodic Memory Storage ---" << std::endl;
    
    Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 500;
    config.enableCheckpointing = false;
    config.enableReplay = false;
    config.enableDevelopment = true;
    
    std::cout << "Running integration experiment with episodic memory..." << std::endl;
    Phase6IntegrationResult result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== EPISODIC MEMORY TEST RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    if (result.memoryEpisodesStored > 0) {
        std::cout << std::endl;
        std::cout << "✅ SUCCESS: Episodic memory is storing experiences!" << std::endl;
        std::cout << "The brain is capturing experiences over time." << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "❌ ISSUE: No episodes stored!" << std::endl;
        std::cout << "Episodic memory is not capturing experiences." << std::endl;
        std::cout << "This is the main Phase 6 integration issue." << std::endl;
    }
    
    return 0;
}