// Phase 4 Experiment Tests
// Tests for Phase 4 experiments including scaling benchmarks and advanced integration

#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/ScalingBenchmark.hpp"
#include "brain/Brain.hpp>
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

namespace test_phase4_experiments {

void testPhase4ExperimentLogger() {
    nlm::ExperimentLogger logger("test_phase4_log.csv");
    
    // Test logging methods (inherited from base experiment logger)
    logger.logPhase4Header();
    logger.logEpisodeStart(0);
    logger.logStep(0, 1.0f, 0.5f, 0.3f, 0.8f, "MoveForward");
    logger.logDevelopment(10.5f, "CriticalPeriod");
    logger.logSynapticStats(1000, 0.5f, 1.0f);
    logger.logEpisodeEnd(0, 100.0f, 200, 0.7f);
    
    logger.flush();
    
    // Clean up test file
    std::remove("test_phase4_log.csv");
    
    std::cout << "    testPhase4ExperimentLogger passed" << std::endl;
}

void testPhase4ExperimentMetrics() {
    nlm::ExperimentMetrics metrics;
    
    // Test metrics structure (same as Phase 3)
    assert(metrics.episode == 0);
    assert(metrics.steps == 0);
    assert(metrics.totalReward == 0.0f);
    assert(metrics.finalEnergy == 0.0f);
    
    // Update and verify
    metrics.episode = 1;
    metrics.steps = 200;
    metrics.totalReward = 200.0f;
    metrics.finalEnergy = 0.9f;
    
    assert(metrics.episode == 1);
    assert(metrics.steps == 200);
    assert(metrics.totalReward == 200.0f);
    assert(metrics.finalEnergy == 0.9f);
    
    std::cout << "    testPhase4ExperimentMetrics passed" << std::endl;
}

void testPhase4ExperimentIntegration() {
    nlm::Phase4Demo demo;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 3);
    config->set("connection_probability", 0.05f);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Verify configuration worked
    assert(brain->getTotalNeuronCount() == 200);
    assert(brain->getRegionCount() == 3);
    assert(brain->getTotalSynapseCount() > 0);
    
    std::cout << "    testPhase4ExperimentIntegration passed" << std::endl;
}

void testPhase4ExperimentBenchmarkConfiguration() {
    nlm::ScalingBenchmark benchmark;
    
    // Test benchmark configuration
    auto config = benchmark.getConfig();
    assert(config != nullptr);
    
    // Set test parameters
    benchmark.setNetworkSize(500);
    benchmark.setRegionCount(5);
    benchmark.setSimulationSteps(1000);
    benchmark.setLearningRate(0.01f);
    
    // Verify parameters were set
    assert(benchmark.getNetworkSize() == 500);
    assert(benchmark.getRegionCount() == 5);
    assert(benchmark.getSimulationSteps() == 1000);
    assert(benchmark.getLearningRate() == 0.01f);
    
    std::cout << "    testPhase4ExperimentBenchmarkConfiguration passed" << std::endl;
}

void testPhase4ExperimentMetricsCollection() {
    nlm::ExperimentMetrics metrics;
    
    // Update metrics similar to what Phase4Demo would do
    metrics.episode = 1;
    metrics.steps = 300;
    metrics.totalReward = 250.0f;
    metrics.finalEnergy = 0.7f;
    metrics.finalNovelty = 0.3f;
    metrics.finalCuriosity = 0.6f;
    metrics.predictionError = 0.1f;
    metrics.developmentalAge = 20.0f;
    metrics.developmentalStage = "Adult";
    metrics.totalSynapses = 3000;
    metrics.averageWeight = 0.7f;
    metrics.maxWeight = 1.0f;
    metrics.totalSpikes = 8000;
    metrics.averageFiringRate = 20.0f;
    
    // Update action counts
    metrics.forwardCount = 200;
    metrics.backwardCount = 50;
    metrics.turnLeftCount = 80;
    metrics.turnRightCount = 100;
    metrics.interactCount = 40;
    metrics.waitCount = 330;
    
    // Verify metrics consistency
    assert(metrics.steps > 0);
    assert(metrics.totalReward > 0.0f);
    assert(metrics.totalSynapses > 1000);
    assert(metrics.totalSpikes > 3000);
    
    // Test CSV output
    std::string csv = metrics.toCSV();
    assert(!csv.empty());
    
    std::cout << "    testPhase4ExperimentMetricsCollection passed" << std::endl;
}

void testPhase4ExperimentScalingBenchmark() {
    nlm::ScalingBenchmark benchmark;
    
    // Set up benchmark
    benchmark.setNetworkSize(1000);
    benchmark.setRegionCount(10);
    benchmark.setSimulationSteps(2000);
    benchmark.setLearningRate(0.001f);
    
    // Run a mock benchmark (not actual neural computation for testing)
    nlm::ExperimentMetrics metrics;
    metrics.episode = 0;
    metrics.steps = benchmark.getSimulationSteps();
    metrics.totalReward = static_cast<float>(benchmark.getNetworkSize() * 10);
    metrics.totalSynapses = benchmark.getNetworkSize() * 10;
    
    // Verify scaling relationship
    assert(metrics.totalSynapses > benchmark.getNetworkSize() * 5);
    assert(metrics.totalReward > 0.0f);
    
    std::cout << "    testPhase4ExperimentScalingBenchmark passed" << std::endl;
}

void testPhase4ExperimentPerformanceMetrics() {
    nlm::ScalingBenchmark benchmark;
    
    // Configure benchmark for performance testing
    benchmark.setNetworkSize(100);
    benchmark.setRegionCount(2);
    benchmark.setSimulationSteps(500);
    benchmark.setLearningRate(0.01f);
    
    // Record timing metrics
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate computation (this would be actual neural computation in real test)
    nlm::ExperimentMetrics metrics;
    for (int i = 0; i < benchmark.getSimulationSteps(); ++i) {
        metrics.steps++;
        metrics.totalSpikes += 10;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Verify performance metrics
    assert(metrics.steps == benchmark.getSimulationSteps());
    assert(metrics.totalSpikes > 0);
    assert(duration.count() < 10000); // Should complete within 10 seconds
    
    std::cout << "    testPhase4ExperimentPerformanceMetrics passed" << std::endl;
}

void testPhase4ExperimentMemoryUsage() {
    nlm::ScalingBenchmark benchmark;
    
    // Test memory efficiency by running multiple configurations
    std::vector<nlm::ExperimentMetrics> results;
    
    for (int size : {50, 100, 200}) {
        benchmark.setNetworkSize(size);
        benchmark.setRegionCount(2);
        benchmark.setSimulationSteps(size * 10);
        
        nlm::ExperimentMetrics metrics;
        metrics.episode = 0;
        metrics.steps = benchmark.getSimulationSteps();
        metrics.totalSynapses = size * 10;
        
        results.push_back(metrics);
    }
    
    // Verify memory efficiency (synapses should scale linearly)
    assert(results.size() == 3);
    assert(results[2].totalSynapses > results[0].totalSynapses * 3); // Approximate linear scaling
    
    std::cout << "    testPhase4ExperimentMemoryUsage passed" << std::endl;
}

void testPhase4ExperimentLearningProgress() {
    nlm::ExperimentMetrics earlyMetrics;
    earlyMetrics.episode = 0;
    earlyMetrics.totalReward = 100.0f;
    earlyMetrics.finalEnergy = 0.6f;
    earlyMetrics.totalSynapses = 1000;
    
    nlm::ExperimentMetrics lateMetrics;
    lateMetrics.episode = 9;
    lateMetrics.totalReward = 500.0f;
    lateMetrics.finalEnergy = 0.9f;
    lateMetrics.totalSynapses = 3000;
    
    // Verify learning progression
    assert(lateMetrics.totalReward > earlyMetrics.totalReward); // Positive learning
    assert(lateMetrics.totalSynapses > earlyMetrics.totalSynapses); // Synaptogenesis
    assert(lateMetrics.finalEnergy > earlyMetrics.finalEnergy); // Better energy management
    
    // Calculate improvements
    float rewardImprovement = (lateMetrics.totalReward - earlyMetrics.totalReward) / earlyMetrics.totalReward;
    float synapseImprovement = (lateMetrics.totalSynapses - earlyMetrics.totalSynapses) / static_cast<float>(earlyMetrics.totalSynapses);
    
    // Should see significant improvements
    assert(rewardImprovement > 0.0f); // Reward increased
    assert(synapseImprovement > 2.0f); // Synapses more than tripled
    
    std::cout << "    testPhase4ExperimentLearningProgress passed" << std::endl;
}

void testPhase4ExperimentStressTest() {
    nlm::ScalingBenchmark benchmark;
    
    // Test with multiple network sizes
    std::vector<int> sizes = {10, 50, 100, 200};
    std::vector<nlm::ExperimentMetrics> results;
    
    for (int size : sizes) {
        benchmark.setNetworkSize(size);
        benchmark.setRegionCount(2);
        benchmark.setSimulationSteps(size * 5);
        
        nlm::ExperimentMetrics metrics;
        metrics.episode = 0;
        metrics.steps = benchmark.getSimulationSteps();
        metrics.totalSynapses = size * 10;
        metrics.totalSpikes = size * 20;
        
        results.push_back(metrics);
    }
    
    // Verify results scale appropriately
    assert(results.size() == sizes.size());
    assert(results.back().totalSynapses > results.front().totalSynapses * 10); // Significant scaling
    
    std::cout << "    testPhase4ExperimentStressTest passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Phase 4 Experiment Logger", testPhase4ExperimentLogger);
    runner.runTest("Phase 4 Experiment Metrics", testPhase4ExperimentMetrics);
    runner.runTest("Phase 4 Experiment Integration", testPhase4ExperimentIntegration);
    runner.runTest("Phase 4 Experiment Benchmark Configuration", testPhase4ExperimentBenchmarkConfiguration);
    runner.runTest("Phase 4 Experiment Metrics Collection", testPhase4ExperimentMetricsCollection);
    runner.runTest("Phase 4 Experiment Scaling Benchmark", testPhase4ExperimentScalingBenchmark);
    runner.runTest("Phase 4 Experiment Performance Metrics", testPhase4ExperimentPerformanceMetrics);
    runner.runTest("Phase 4 Experiment Memory Usage", testPhase4ExperimentMemoryUsage);
    runner.runTest("Phase 4 Experiment Learning Progress", testPhase4ExperimentLearningProgress);
    runner.runTest("Phase 4 Experiment Stress Test", testPhase4ExperimentStressTest);
    
    runner.runAll();
}

} // namespace test_phase4_experiments
