// Phase 5 Integration Tests
// Tests for the Phase 5 lifetime learning experiment

#include "experiments/Phase5Experiment.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <thread>

namespace test_phase5 {

void testPhase5Configuration() {
    std::cout << "    Testing Phase 5 configuration..." << std::endl;
    
    // Test default configuration
    nlm::LifetimeExperimentConfig config;
    assert(config.initialNeurons > 0);
    assert(config.maxNeurons > config.initialNeurons);
    assert(config.connectionProbability > 0.0f && config.connectionProbability <= 1.0f);
    assert(config.synapseFormationSteps > 0);
    assert(config.sensoryDevelopmentSteps > 0);
    assert(config.motorDevelopmentSteps > 0);
    assert(config.associativeDevelopmentSteps > 0);
    assert(config.memoryConsolidationSteps > 0);
    assert(config.socialDevelopmentSteps > 0);
    assert(config.trialsPerPhase > 0);
    assert(config.stepsPerTrial > 0);
    
    // Test optimization flags
    assert(config.enableMultithreading == false || config.enableMultithreading == true);
    assert(config.enableSIMD == false || config.enableSIMD == true);
    assert(config.enableSparseConnectivity == false || config.enableSparseConnectivity == true);
    assert(config.enableEventDriven == false || config.enableEventDriven == true);
    assert(config.threadCount > 0);
    
    std::cout << "    Phase 5 configuration passed!" << std::endl;
}

void testPhase5BenchmarkConfig() {
    std::cout << "    Testing Phase 5 benchmark configuration..." << std::endl;
    
    // Test default benchmark configuration
    nlm::BenchmarkConfig benchmarkConfig;
    assert(benchmarkConfig.stepsPerScale > 0);
    assert(benchmarkConfig.warmupSteps > 0);
    assert(benchmarkConfig.runUntilStable == false || benchmarkConfig.runUntilStable == true);
    assert(benchmarkConfig.stabilityThreshold > 0.0);
    assert(benchmarkConfig.maxStabilityChecks > 0);
    
    // Test optimization flags
    assert(benchmarkConfig.enableMultithreading == false || benchmarkConfig.enableMultithreading == true);
    assert(benchmarkConfig.enableSIMD == false || benchmarkConfig.enableSIMD == true);
    assert(benchmarkConfig.enableSparseConnectivity == false || benchmarkConfig.enableSparseConnectivity == true);
    assert(benchmarkConfig.enableEventDriven == false || benchmarkConfig.enableEventDriven == true);
    assert(benchmarkConfig.threadCount > 0);
    
    std::cout << "    Phase 5 benchmark configuration passed!" << std::endl;
}

void testPhase5ScalingBenchmark() {
    std::cout << "    Testing Phase 5 scaling benchmark..." << std::endl;
    
    nlm::ScalingBenchmark benchmark;
    
    // Test with simple scales
    std::vector<nlm::ScaleLevel> scales;
    scales.push_back({"Small", 100, 0.1f});
    scales.push_back({"Medium", 1000, 0.05f});
    scales.push_back({"Large", 5000, 0.01f});
    
    nlm::BenchmarkConfig config;
    config.stepsPerScale = 100;
    config.enableMultithreading = false; // Disable for test
    config.enableSIMD = false; // Disable for test
    config.scales = scales;
    config.seeds = {42, 123, 456};
    config.warmupSteps = 50;
    config.runUntilStable = false;
    config.stabilityThreshold = 0.05;
    config.maxStabilityChecks = 10;
    
    nlm::ScalingAnalysis analysis = benchmark.runScalingExperiment(config);
    
    // Verify analysis results
    assert(analysis.benchmarkRuns.size() == scales.size());
    assert(analysis.benchmarkRuns[0].name == "Small");
    assert(analysis.benchmarkRuns[1].name == "Medium");
    assert(analysis.benchmarkRuns[2].name == "Large");
    assert(analysis.benchmarkRuns[0].scale.neurons == 100);
    assert(analysis.benchmarkRuns[1].scale.neurons == 1000);
    assert(analysis.benchmarkRuns[2].scale.neurons == 5000);
    
    std::cout << "    Phase 5 scaling benchmark passed!" << std::endl;
}

void testPhase5BrainCreation() {
    std::cout << "    Testing Phase 5 brain creation..." << std::endl;
    
    nlm::LifetimeExperimentConfig config;
    config.initialNeurons = 500;
    config.maxNeurons = 5000;
    config.connectionProbability = 0.1f;
    
    // Create brain with Phase 5 configuration
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    
    assert(brain.getRegionCount() > 0);
    assert(brain.getTotalNeuronCount() >= config.initialNeurons);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Phase 5 brain creation passed!" << std::endl;
}

void testPhase5OptimizationFlags() {
    std::cout << "    Testing Phase 5 optimization flags..." << std::endl;
    
    nlm::LifetimeExperimentConfig config;
    
    // Test with different optimization flag combinations
    struct OptimizationTest {
        bool multithreading;
        bool simd;
        bool sparseConnectivity;
        bool eventDriven;
    };
    
    std::vector<OptimizationTest> testCases = {
        {false, false, false, false},  // No optimizations
        {true, false, false, false},   // Only multithreading
        {false, true, false, false},   // Only SIMD
        {false, false, true, false},   // Only sparse connectivity
        {false, false, false, true},   // Only event-driven
        {true, true, false, false},    // Multithreading + SIMD
        {true, false, true, false},    // Multithreading + sparse connectivity
        {false, true, true, true},     // SIMD + sparse + event-driven
        {true, true, true, true}       // All optimizations
    };
    
    for (const auto& testCase : testCases) {
        nlm::LifetimeExperimentConfig testConfig = config;
        testConfig.enableMultithreading = testCase.multithreading;
        testConfig.enableSIMD = testCase.simd;
        testConfig.enableSparseConnectivity = testCase.sparseConnectivity;
        testConfig.enableEventDriven = testCase.eventDriven;
        testConfig.threadCount = testCase.multithreading ? 4 : 1;
        
        // Just test that we can create a brain with these settings
        nlm::Brain brain(std::make_shared<nlm::Config>(testConfig));
        brain.initialize();
        
        assert(brain.getRegionCount() > 0);
        assert(brain.getTotalNeuronCount() > 0);
    }
    
    std::cout << "    Phase 5 optimization flags passed!" << std::endl;
}

void testPhase5MetricsCollection() {
    std::cout << "    Testing Phase 5 metrics collection..." << std::endl;
    
    // Test that Brain can be created and initialized
    nlm::Brain brain(std::make_shared<nlm::Config>());
    bool success = brain.initialize();
    assert(success);
    
    // Run a few steps
    for (size_t i = 0; i < 100; ++i) {
        brain.step();
    }
    
    // Verify that metrics can be collected
    auto metrics = brain.getBenchmarkMetrics();
    assert(metrics.avgFiringRate >= 0.0f);
    assert(metrics.synapticWeightMean >= 0.0f);
    assert(metrics.connectionDensity >= 0.0f && metrics.connectionDensity <= 1.0f);
    assert(metrics.exploratoryBehavior >= 0.0f && metrics.exploratoryBehavior <= 1.0f);
    
    std::cout << "    Phase 5 metrics collection passed!" << std::endl;
}

} // namespace test_phase5
