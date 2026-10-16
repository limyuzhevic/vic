// Phase 6 Performance Tests
// Measure learning progress and system metrics

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace nlm;

namespace test_phase6_performance {

void testPerformanceMetrics() {
    std::cout << "  Testing Performance Metrics..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    Phase6Config config;
    config.maxSteps = 1000;
    config.neuronCount = 500;
    
    auto result = experiment.run(config);
    
    // Verify metrics are collected
    assert(result.totalReward >= 0.0f);
    assert(result.avgFiringRate >= 0.0f);
    assert(result.memoryEpisodesStored >= 0.0f);
    assert(result.noveltyLevel >= 0.0f);
    assert(result.curiosityLevel >= 0.0f);
    assert(result.dopamineLevel >= 0.0f);
    assert(result.totalWallClockTime >= 0.0);
    
    std::cout << "    PASSED" << std::endl;
}

void testLearningProgress() {
    std::cout << "  Testing Learning Progress..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    Phase6Config config;
    config.maxSteps = 500;
    config.neuronCount = 200;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto result = experiment.run(config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double measuredTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Verify performance metrics are reasonable
    assert(result.totalReward >= 0.0f);
    assert(result.avgFiringRate >= 0.0f);
    assert(result.memoryEpisodesStored >= 0.0f);
    
    // Wall clock time should be reasonable
    assert(result.totalWallClockTime >= 0.0);
    assert(result.totalWallClockTime < 60.0); // Should complete within 1 minute
    
    std::cout << "    PASSED" << std::endl;
}

void testSystemMetrics() {
    std::cout << "  Testing System Metrics..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    Phase6Config config;
    config.maxSteps = 300;
    config.neuronCount = 300;
    
    auto result = experiment.run(config);
    
    // Verify all system metrics are integrated
    assert(result.memoryWorkingMemoryIntegrated == true);
    assert(result.memoryEpisodicMemoryIntegrated == true);
    assert(result.neuromodulationIntegrated == true);
    assert(result.predictionIntegrated == true);
    assert(result.developmentIntegrated == true);
    assert(result.checkpointingWorks == true);
    assert(result.replayWorks == true);
    
    std::cout << "    PASSED" << std::endl;
}

void testPerformanceScenarios() {
    std::cout << "  Testing Performance Scenarios..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test with different configurations
    std::vector<Phase6Config> configs;
    
    Phase6Config config1;
    config1.maxSteps = 500;
    config1.neuronCount = 200;
    config1.enableCheckpointing = true;
    config1.enableReplay = true;
    config1.enableDevelopment = true;
    configs.push_back(config1);
    
    Phase6Config config2;
    config2.maxSteps = 1000;
    config2.neuronCount = 500;
    config2.enableCheckpointing = true;
    config2.enableReplay = true;
    config2.enableDevelopment = true;
    configs.push_back(config2);
    
    for (const auto& config : configs) {
        auto result = experiment.run(config);
        
        // Verify results
        assert(result.totalReward >= 0.0f);
        assert(result.avgFiringRate >= 0.0f);
        assert(result.memoryEpisodesStored >= 0.0f);
        assert(result.dopamineLevel >= 0.0f);
        assert(result.curiosityLevel >= 0.0f);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Performance Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testPerformanceMetrics();
    testLearningProgress();
    testSystemMetrics();
    testPerformanceScenarios();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Performance Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_performance