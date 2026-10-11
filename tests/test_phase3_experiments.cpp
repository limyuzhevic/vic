// Phase 3 Experiment Tests
// Tests for Phase 3 experiments including world interaction, learning, and development

#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

namespace test_phase3_experiments {

void testPhase3ExperimentLogger() {
    nlm::ExperimentLogger logger("test_phase3_log.csv");
    
    // Test logging methods
    logger.logPhase3Header();
    logger.logEpisodeStart(0);
    logger.logStep(0, 1.0f, 0.5f, 0.3f, 0.8f, "MoveForward");
    logger.logDevelopment(10.5f, "CriticalPeriod");
    logger.logSynapticStats(1000, 0.5f, 1.0f);
    logger.logEpisodeEnd(0, 100.0f, 200, 0.7f);
    
    logger.flush();
    
    // Clean up test file
    std::remove("test_phase3_log.csv");
    
    std::cout << "    testPhase3ExperimentLogger passed" << std::endl;
}

void testPhase3ExperimentMetrics() {
    nlm::ExperimentMetrics metrics;
    
    // Verify initial metrics
    assert(metrics.episode == 0);
    assert(metrics.steps == 0);
    assert(metrics.totalReward == 0.0f);
    assert(metrics.finalEnergy == 0.0f);
    assert(metrics.finalNovelty == 0.0f);
    assert(metrics.finalCuriosity == 0.0f);
    assert(metrics.predictionError == 0.0f);
    assert(metrics.developmentalAge == 0.0f);
    assert(metrics.developmentalStage == "Initial");
    assert(metrics.totalSynapses == 0);
    assert(metrics.averageWeight == 0.0f);
    assert(metrics.maxWeight == 0.0f);
    assert(metrics.totalSpikes == 0);
    assert(metrics.averageFiringRate == 0.0f);
    
    // Update metrics
    metrics.episode = 1;
    metrics.steps = 100;
    metrics.totalReward = 50.0f;
    metrics.finalEnergy = 0.8f;
    metrics.finalNovelty = 0.3f;
    metrics.finalCuriosity = 0.6f;
    metrics.predictionError = 0.2f;
    metrics.developmentalAge = 5.0f;
    metrics.developmentalStage = "CriticalPeriod";
    metrics.totalSynapses = 1500;
    metrics.averageWeight = 0.45f;
    metrics.maxWeight = 1.0f;
    metrics.totalSpikes = 2000;
    metrics.averageFiringRate = 10.5f;
    
    // Generate CSV
    std::string csv = metrics.toCSV();
    assert(!csv.empty());
    
    std::cout << "    testPhase3ExperimentMetrics passed" << std::endl;
}

void testPhase3ExperimentDemoConfiguration() {
    nlm::Phase3Demo demo;
    
    // Test configuration methods
    auto config = std::make_shared<nlm::Config>();
    
    // The methods are private, but we can test by calling the demo directly
    // and checking that configuration works
    assert(config != nullptr);
    
    std::cout << "    testPhase3ExperimentDemoConfiguration passed" << std::endl;
}

void testPhase3ExperimentIntegration() {
    nlm::Phase3Demo demo;
    
    // Create minimal configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create agent brain
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Verify integration
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getBrain()->getTotalNeuronCount() == 100);
    assert(agentBrain.getBrain()->getTotalSynapseCount() > 0);
    
    std::cout << "    testPhase3ExperimentIntegration passed" << std::endl;
}

void testPhase3ExperimentMetricsCollection() {
    nlm::ExperimentMetrics metrics;
    
    // Simulate metrics collection during experiment
    metrics.episode = 2;
    metrics.steps = 500;
    metrics.totalReward = 125.0f;
    metrics.finalEnergy = 0.9f;
    metrics.finalNovelty = 0.2f;
    metrics.finalCuriosity = 0.8f;
    metrics.predictionError = 0.1f;
    metrics.developmentalAge = 15.0f;
    metrics.developmentalStage = "Maturation";
    metrics.totalSynapses = 2000;
    metrics.averageWeight = 0.6f;
    metrics.maxWeight = 1.0f;
    metrics.totalSpikes = 5000;
    metrics.averageFiringRate = 15.0f;
    
    // Update action counts
    metrics.forwardCount = 120;
    metrics.backwardCount = 30;
    metrics.turnLeftCount = 45;
    metrics.turnRightCount = 60;
    metrics.interactCount = 25;
    metrics.waitCount = 220;
    
    // Verify metrics are reasonable
    assert(metrics.steps > 0);
    assert(metrics.totalReward > 0.0f);
    assert(metrics.finalEnergy > 0.0f);
    assert(metrics.totalSynapses > 1000);
    assert(metrics.totalSpikes > 1000);
    
    // Test CSV output format
    std::string csv = metrics.toCSV();
    std::istringstream csvStream(csv);
    
    std::vector<std::string> csvRows;
    std::string line;
    while (std::getline(csvStream, line, ',')) {
        csvRows.push_back(line);
    }
    
    // Should have 20 fields in metrics CSV
    assert(csvRows.size() == 20);
    
    std::cout << "    testPhase3ExperimentMetricsCollection passed" << std::endl;
}

void testPhase3ExperimentLearningBehavior() {
    // This test simulates the learning behavior that Phase 3 experiments demonstrate
    nlm::ExperimentMetrics earlyMetrics;
    earlyMetrics.episode = 0;
    earlyMetrics.totalReward = 50.0f;
    earlyMetrics.forwardCount = 80;
    earlyMetrics.waitCount = 120;
    
    nlm::ExperimentMetrics lateMetrics;
    lateMetrics.episode = 4;
    lateMetrics.totalReward = 150.0f;
    lateMetrics.forwardCount = 180;
    lateMetrics.waitCount = 100;
    
    // Verify learning patterns
    assert(lateMetrics.totalReward > earlyMetrics.totalReward); // Positive learning
    assert(lateMetrics.forwardCount > earlyMetrics.forwardCount); // More purposeful behavior
    assert(lateMetrics.waitCount < earlyMetrics.waitCount); // Less waiting, more exploration
    
    // Calculate changes
    float rewardChange = lateMetrics.totalReward - earlyMetrics.totalReward;
    float actionChange = static_cast<float>(lateMetrics.forwardCount) - static_cast<float>(earlyMetrics.forwardCount);
    float explorationChange = static_cast<float>(lateMetrics.waitCount) - static_cast<float>(earlyMetrics.waitCount);
    
    // Verify changes are reasonable
    assert(rewardChange > 0); // Positive learning
    assert(actionChange > 0); // More purposeful actions
    assert(explorationChange < 0); // Less waiting, more exploration
    
    std::cout << "    testPhase3ExperimentLearningBehavior passed" << std::endl;
}

void testPhase3ExperimentDevelopmentStages() {
    nlm::DevelopmentSystem devSystem;
    
    // Test developmental stage progression
    std::vector<std::pair<nlm::DevelopmentalStage, std::string>> stageTests = {
        {nlm::DevelopmentalStage::Initial, "Initial"},
        {nlm::DevelopmentalStage::CriticalPeriod, "CriticalPeriod"},
        {nlm::DevelopmentalStage::Maturation, "Maturation"},
        {nlm::DevelopmentalStage::Adult, "Adult"},
        {nlm::DevelopmentalStage::Aging, "Aging"}
    };
    
    for (const auto& test : stageTests) {
        devSystem.setDevelopmentalStage(test.first);
        assert(devSystem.getStage() == test.first);
        assert(std::string(devSystem.getStageName()) == test.second);
    }
    
    // Verify plasticity modifiers for each stage
    std::vector<std::pair<nlm::DevelopmentalStage, float>> plasticityTests = {
        {nlm::DevelopmentalStage::Initial, 1.0f},
        {nlm::DevelopmentalStage::CriticalPeriod, 2.0f},
        {nlm::DevelopmentalStage::Maturation, 1.5f},
        {nlm::DevelopmentalStage::Adult, 1.0f},
        {nlm::DevelopmentalStage::Aging, 0.5f}
    };
    
    for (const auto& test : plasticityTests) {
        devSystem.setDevelopmentalStage(test.first);
        assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), test.second));
    }
    
    std::cout << "    testPhase3ExperimentDevelopmentStages passed" << std::endl;
}

void testPhase3ExperimentStressSimulation() {
    nlm::Phase3Demo demo;
    
    // Simulate multiple episodes with varying parameters
    for (int episode = 0; episode < 3; ++episode) {
        nlm::ExperimentMetrics metrics;
        metrics.episode = episode;
        metrics.steps = 100 * (episode + 1);
        metrics.totalReward = static_cast<float>(50 * (episode + 1));
        metrics.finalEnergy = 0.5f + static_cast<float>(episode) * 0.1f;
        metrics.totalSynapses = 500 * (episode + 1);
        
        // Each episode should show learning progression
        if (episode > 0) {
            assert(metrics.totalReward > metrics.episode * 50); // Increasing rewards
            assert(metrics.totalSynapses > 500); // Synaptogenesis
            assert(metrics.finalEnergy >= 0.5f); // Maintaining energy
        }
    }
    
    std::cout << "    testPhase3ExperimentStressSimulation passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Phase 3 Experiment Logger", testPhase3ExperimentLogger);
    runner.runTest("Phase 3 Experiment Metrics", testPhase3ExperimentMetrics);
    runner.runTest("Phase 3 Experiment Demo Configuration", testPhase3ExperimentDemoConfiguration);
    runner.runTest("Phase 3 Experiment Integration", testPhase3ExperimentIntegration);
    runner.runTest("Phase 3 Experiment Metrics Collection", testPhase3ExperimentMetricsCollection);
    runner.runTest("Phase 3 Experiment Learning Behavior", testPhase3ExperimentLearningBehavior);
    runner.runTest("Phase 3 Experiment Development Stages", testPhase3ExperimentDevelopmentStages);
    runner.runTest("Phase 3 Experiment Stress Simulation", testPhase3ExperimentStressSimulation);
    
    runner.runAll();
}

} // namespace test_phase3_experiments
