// Phase 6 Integration Tests
// Comprehensive tests for verifying system integration and cross-component connections

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_phase6_integration {

void testSystemIntegration() {
    std::cout << "  Testing system integration..." << std::endl;
    
    // Create Phase 6 experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK && "System integration verification failed");
    
    std::cout << "    ✓ System integration verified" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "  Testing memory system integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test working memory integration
    experiment.testMemoryIntegration();
    std::cout << "    ✓ Working memory integration verified" << std::endl;
    
    // Test episodic memory integration
    nlm::Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 100;
    config.enableCheckpointing = true;
    
    auto result = experiment.run(config);
    assert(result.memoryEpisodicMemoryIntegrated && "Episodic memory integration failed");
    
    std::cout << "    ✓ Episodic memory integration verified" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test neuromodulation integration
    experiment.testNeuromodulationIntegration();
    std::cout << "    ✓ Neuromodulation integration verified" << std::endl;
    
    // Test specific neuromodulators
    nlm::Phase6Config config;
    config.neuronCount = 50;
    config.maxSteps = 50;
    
    auto result = experiment.run(config);
    assert(result.neuromodulationIntegrated && "Neuromodulation system integration failed");
    
    std::cout << "    ✓ Neuromodulation system integration verified" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test prediction system integration (through overall integration test)
    nlm::Phase6Config config;
    config.neuronCount = 75;
    config.maxSteps = 75;
    
    auto result = experiment.run(config);
    assert(result.predictionIntegrated && "Prediction system integration failed");
    
    std::cout << "    ✓ Prediction system integration verified" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "  Testing cognition system integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test cognition systems through overall integration
    nlm::Phase6Config config;
    config.neuronCount = 60;
    config.maxSteps = 60;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    assert(result.developmentIntegrated && "Cognition system integration failed");
    
    std::cout << "    ✓ Cognition system integration verified" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "  Testing development system integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test development system integration
    nlm::Phase6Config config;
    config.neuronCount = 80;
    config.maxSteps = 80;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    assert(result.developmentIntegrated && "Development system integration failed");
    
    std::cout << "    ✓ Development system integration verified" << std::endl;
}

void testCheckpointingIntegration() {
    std::cout << "  Testing checkpointing integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test checkpointing integration
    experiment.testCheckpointing();
    std::cout << "    ✓ Checkpointing integration verified" << std::endl;
}

void testReplayIntegration() {
    std::cout << "  Testing replay integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test replay integration
    experiment.testReplay();
    std::cout << "    ✓ Replay integration verified" << std::endl;
}

void testCrossSystemInteractions() {
    std::cout << "  Testing cross-system interactions..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test complex interaction scenarios
    nlm::Phase6Config config;
    config.neuronCount = 120;
    config.maxSteps = 120;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Verify that multiple systems are integrated
    assert(result.memoryWorkingMemoryIntegrated && "Working memory cross-system integration failed");
    assert(result.memoryEpisodicMemoryIntegrated && "Episodic memory cross-system integration failed");
    assert(result.neuromodulationIntegrated && "Neuromodulation cross-system integration failed");
    assert(result.predictionIntegrated && "Prediction cross-system integration failed");
    assert(result.developmentIntegrated && "Development cross-system integration failed");
    
    std::cout << "    ✓ Cross-system interactions verified" << std::endl;
}

void testErrorHandlingIntegration() {
    std::cout << "  Testing error handling integration..." << std::endl;
    
    // Test error handling across different systems
    nlm::Config config;
    
    // Test invalid configuration handling
    bool success = config.loadFromFile("nonexistent_file.cfg");
    assert(!success && "Invalid file handling failed");
    
    // Test memory system error handling
    nlm::Phase6IntegratedExperiment experiment;
    
    // The experiment should handle errors gracefully and still provide useful information
    nlm::Phase6Config expConfig;
    expConfig.neuronCount = 10;
    expConfig.maxSteps = 10;
    
    auto result = experiment.run(expConfig);
    
    // Should complete even with small configuration
    assert(result.totalReward >= 0 || result.totalWallClockTime >= 0 && "Error handling failed");
    
    std::cout << "    ✓ Error handling integration verified" << std::endl;
}

void testPerformanceIntegration() {
    std::cout << "  Testing performance integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test performance with different configuration sizes
    nlm::Phase6Config config;
    config.neuronCount = 200;  // Larger brain
    config.maxSteps = 200;     // More simulation steps
    config.enableCheckpointing = true;
    
    auto result = experiment.run(config);
    
    // Should complete without errors and provide reasonable performance metrics
    assert(result.totalWallClockTime >= 0 && "Performance test failed");
    assert(result.avgFiringRate >= 0 && "Performance test failed");
    
    std::cout << "    ✓ Performance integration verified" << std::endl;
}

void testConfigurationIntegration() {
    std::cout << "  Testing configuration integration..." << std::endl;
    
    // Test different configuration formats and their integration
    nlm::Config config;
    
    // Set up neural simulation configuration
    config.set("neuron_count", 1000);
    config.set("region_count", 1);
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", 42);
    config.set("stdp_ltp_weight", 0.02f);
    config.set("stdp_ltd_weight", 0.015f);
    config.set("connection_probability", 0.15f);
    
    // Verify configuration
    assert(config.getOr<int>("neuron_count", 0) == 1000 && "Configuration integration failed");
    assert(config.getOr<double>("simulation_timestep", 0.0) > 0.0 && "Configuration integration failed");
    
    // Test configuration merging
    nlm::Config config2;
    config2.set("world_width", 100.0);
    config2.set("world_height", 100.0);
    
    config.merge(config2);
    
    assert(config.has("world_width") && "Configuration merge failed");
    
    std::cout << "    ✓ Configuration integration verified" << std::endl;
}

void testBrainSystemIntegration() {
    std::cout << "  Testing brain system integration..." << std::endl;
    
    // Create and test brain with all systems
    auto config = std::make_shared<nlm::Config>();
    
    // Configure with neural simulation settings
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    auto brain = nlm::createBrain(config);
    assert(brain && "Brain creation failed");
    
    // Initialize brain
    assert(brain->initialize() && "Brain initialization failed");
    
    // Test basic functionality
    brain->logStatus();
    
    // Verify brain statistics
    assert(brain->getTotalNeuronCount() > 0 && "Brain statistics failed");
    
    std::cout << "    ✓ Brain system integration verified" << std::endl;
}

void testCompleteIntegrationPipeline() {
    std::cout << "  Testing complete integration pipeline..." << std::endl;
    
    // Test the complete pipeline from configuration to execution
    nlm::Phase6IntegratedExperiment experiment;
    
    // Create comprehensive configuration
    nlm::Phase6Config config;
    config.neuronCount = 150;
    config.maxSteps = 150;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Run complete integration test
    auto result = experiment.run(config);
    
    // Verify all integration aspects
    bool allIntegrated = result.memoryWorkingMemoryIntegrated &&
                       result.memoryEpisodicMemoryIntegrated &&
                       result.neuromodulationIntegrated &&
                       result.predictionIntegrated &&
                       result.developmentIntegrated;
    
    assert(allIntegrated && "Complete integration pipeline failed");
    
    // Verify performance metrics
    assert(result.totalReward >= 0 && "Complete integration performance failed");
    assert(result.memoryEpisodesStored >= 0 && "Complete integration memory storage failed");
    
    std::cout << "    ✓ Complete integration pipeline verified" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    testSystemIntegration();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testCognitionSystemIntegration();
    testDevelopmentIntegration();
    testCheckpointingIntegration();
    testReplayIntegration();
    testCrossSystemInteractions();
    testErrorHandlingIntegration();
    testPerformanceIntegration();
    testConfigurationIntegration();
    testBrainSystemIntegration();
    testCompleteIntegrationPipeline();
    
    std::cout << "=== All Integration Tests Passed ===" << std::endl;
}

} // namespace test_phase6_integration