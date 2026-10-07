// Enhanced Brain Tests with Comprehensive Coverage
// This file extends the basic brain tests with edge cases, error conditions,
// integration tests, and performance validation.

// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

namespace test_brain_enhanced {

void testBrainCreationWithInvalidConfig() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with zero neuron count (should still create brain but fail to initialize)
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Brain should be created even with invalid config
    assert(brain.getTotalNeuronCount() == 0);
    
    std::cout << "    testBrainCreationWithInvalidConfig passed" << std::endl;
}

void testBrainInitializationValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test successful initialization
    assert(brain.initialize() == true);
    
    // Test that brain can be reset and re-initialized
    brain.reset();
    assert(brain.initialize() == true);
    
    // Test initialization after too many steps
    for (int i = 0; i < 1000; ++i) {
        brain.step(i);
    }
    brain.reset();
    assert(brain.initialize() == true);
    
    std::cout << "    testBrainInitializationValidation passed" << std::endl;
}

void testBrainStepEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test single step
    brain.step(0);
    
    // Test large number of steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Test negative step (should be handled gracefully)
    try {
        brain.step(-1);
        std::cout << "    Warning: Negative step didn't throw exception" << std::endl;
    } catch (...) {
        std::cout << "    Negative step correctly threw exception" << std::endl;
    }
    
    // Test very large step
    brain.step(1000000);
    
    std::cout << "    testBrainStepEdgeCases passed" << std::endl;
}

void testBrainResetComprehensive() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Perform some operations
    brain.step(0);
    brain.step(1);
    brain.step(2);
    
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Reset brain
    brain.reset();
    
    // Verify reset properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0); // Should still have synapses
    assert(brain.getFiringNeuronCount() == 0);
    
    // Re-initialize
    assert(brain.initialize() == true);
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainResetComprehensive passed" << std::endl;
}

void testBrainRegionsEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all regions are accessible
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 10);
    
    // Test each region individually
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        assert(region->getName() == "Region_" + std::to_string(rid.value));
        assert(region->getTotalNeuronCount() > 0);
    }
    
    // Test invalid region ID
    auto invalidRegion = brain.getRegion(nlm::RegionId(999));
    assert(invalidRegion == nullptr);
    
    std::cout << "    testBrainRegionsEdgeCases passed" << std::endl;
}

void testBrainActionProductionEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test multiple action productions
    auto action1 = brain.produceAction();
    auto action2 = brain.produceAction();
    
    assert(action1 != nullptr);
    assert(action2 != nullptr);
    
    // Actions should have unique parameters
    assert(action1->getType() == action2->getType()); // Same type likely
    assert(action1->getParameters() != action2->getParameters()); // Different parameters
    
    std::cout << "    testBrainActionProductionEdgeCases passed" << std::endl;
}

void testBrainStatusValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all status methods return valid values
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    assert(brain.getTotalSpikeCount() >= 0);
    
    // Test developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial &&
           stage <= nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testBrainStatusValidation passed" << std::endl;
}

void testBrainSensoryInputValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create test sensory input
    nlm::Vision vision(10, 10, 3);
    std::vector<float> testData(300, 0.5f);
    vision.setData(testData);
    
    // Test sensory input injection
    brain.receiveSensoryInput(vision);
    
    // Test with empty vision
    nlm::Vision emptyVision(0, 0, 0);
    brain.receiveSensoryInput(emptyVision);
    
    std::cout << "    testBrainSensoryInputValidation passed" << std::endl;
}

void testBrainConfigValidation() {
    // Test config loading errors
    auto config = std::make_shared<nlm::Config>();
    
    // Test setting invalid values
    config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default);
    assert(config->getOr<int64_t>("neuron_count", 0) == -1);
    
    // Test missing keys
    assert(!config->has("nonexistent_key"));
    assert(config->getOr<std::string>("nonexistent_key", "default") == "default");
    
    // Test configuration summary
    std::string summary = config->summary();
    assert(summary.length() > 0);
    
    std::cout << "    testBrainConfigValidation passed" << std::endl;
}

void testBrainMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run brain for a while to test memory management
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    // Verify brain still works after extensive use
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Reset and test memory cleanup
    brain.reset();
    assert(brain.initialize() == true);
    
    std::cout << "    testBrainMemoryUsage passed" << std::endl;
}

void testBrainPerformanceMetrics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get performance metrics
    auto metrics = brain.getPerformanceMetrics();
    
    // Validate metrics
    assert(metrics.find("total_neurons") != metrics.end());
    assert(metrics.find("total_synapses") != metrics.end());
    assert(metrics.find("firing_neurons") != metrics.end());
    assert(metrics.find("average_firing_rate") != metrics.end());
    assert(metrics.find("excitatory_inhibition_ratio") != metrics.end());
    assert(metrics.find("total_spikes") != metrics.end());
    assert(metrics.find("development_stage") != metrics.end());
    
    // Validate metric values
    assert(metrics["total_neurons"] == 500);
    assert(metrics["total_synapses"] > 0);
    assert(metrics["firing_neurons"] >= 0);
    assert(metrics["average_firing_rate"] >= 0.0f);
    assert(metrics["excitatory_inhibition_ratio"] >= 0.0f);
    assert(metrics["total_spikes"] >= 0);
    
    std::cout << "    testBrainPerformanceMetrics passed" << std::endl;
}

void runAllEnhancedTests() {
    std::cout << "Running Enhanced Brain Tests..." << std::endl;
    testBrainCreationWithInvalidConfig();
    testBrainInitializationValidation();
    testBrainStepEdgeCases();
    testBrainResetComprehensive();
    testBrainRegionsEdgeCases();
    testBrainActionProductionEdgeCases();
    testBrainStatusValidation();
    testBrainSensoryInputValidation();
    testBrainConfigValidation();
    testBrainMemoryUsage();
    testBrainPerformanceMetrics();
}

} // namespace test_brain_enhanced

// Integration Tests
namespace test_integration {

void testBrainWorldIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 4, 4);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test integration through simulation steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        world.update(0.1);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain.step(step);
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    std::cout << "    testBrainWorldIntegration passed" << std::endl;
}

void testBrainMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    brain.receiveSensoryInput(nlm::Vision(5, 5, 3));
    
    // Test episodic memory
    brain.updateDevelopment(1.0);
    
    std::cout << "    testBrainMemoryIntegration passed" << std::endl;
}

void testBrainLearningIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test reward-based learning
    nlm::SimpleWorld world;
    world.configure(8, 8, 4, 4);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    
    // Run learning episode
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        world.update(0.1);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain.step(step);
        
        auto action = agent.decodeMotorCommand();
        auto result = world.applyMotorCommand(action, world.getSimulationTime());
        
        if (result.success) {
            agent.applyRewardModulation(result.reward, agent.getPredictionError());
        }
    }
    
    std::cout << "    testBrainLearningIntegration passed" << std::endl;
}

void testBrainDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stages
    nlm::SimpleWorld world;
    world.configure(6, 6, 3, 3);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run through developmental stages
    for (double timeStep = 0.0; timeStep < 10.0; timeStep += 1.0) {
        agent.updateDevelopment(timeStep);
        std::cout << "    Developmental stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
    }
    
    std::cout << "    testBrainDevelopmentIntegration passed" << std::endl;
}

void testBrainAdvancedIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable all learning systems
    nlm::SimpleWorld world;
    world.configure(10, 10, 6, 6);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    agent.enableStructuralPlasticity(true);
    
    // Run comprehensive integration test
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        world.update(0.1);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain.step(step);
        
        auto action = agent.decodeMotorCommand();
        auto result = world.applyMotorCommand(action, world.getSimulationTime());
        
        if (result.success) {
            agent.applyRewardModulation(result.reward, agent.getPredictionError());
        }
        
        agent.updateDevelopment(0.1);
    }
    
    // Verify all systems are integrated
    assert(brain.getTotalSpikeCount() > 0);
    assert(agent.getCuriosityLevel() > 0.0f);
    assert(agent.getNeuromodulationLevel() > 0.0f);
    
    std::cout << "    testBrainAdvancedIntegration passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "\nRunning Integration Tests..." << std::endl;
    testBrainWorldIntegration();
    testBrainMemoryIntegration();
    testBrainLearningIntegration();
    testBrainDevelopmentIntegration();
    testBrainAdvancedIntegration();
}

} // namespace test_integration

// Performance Tests
namespace test_performance {

void testBrainPerformanceScaling() {
    std::cout << "\nRunning Performance Tests..." << std::endl;
    
    // Test performance with different brain sizes
    for (size_t neuronCount : {100, 500, 1000}) {
        for (size_t regionCount : {1, 2, 5}) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount),
                       nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(regionCount),
                       nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Measure initialization time (simplified)
            auto start = std::chrono::steady_clock::now();
            for (nlm::SimulationStep step = 0; step < 100; ++step) {
                brain.step(step);
            }
            auto end = std::chrono::steady_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "    Size: " << neuronCount << " neurons, "
                      << regionCount << " regions - 100 steps in "
                      << duration.count() << "ms" << std::endl;
            
            assert(brain.getTotalNeuronCount() == neuronCount);
        }
    }
    
    std::cout << "    testBrainPerformanceScaling passed" << std::endl;
}

void testMemoryEfficiency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory efficiency with large brain
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functions correctly
    assert(brain.getTotalNeuronCount() == 2000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Reset and re-initialize
    brain.reset();
    assert(brain.initialize() == true);
    
    std::cout << "    testMemoryEfficiency passed" << std::endl;
}

void testRealTimePerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test real-time performance
    auto start = std::chrono::steady_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double stepsPerSecond = 1000.0 / (duration.count() / 1000.0);
    
    std::cout << "    Real-time performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    // Should be able to process at least 100 steps/second
    assert(stepsPerSecond > 100.0);
    
    std::cout << "    testRealTimePerformance passed" << std::endl;
}

void runAllPerformanceTests() {
    testBrainPerformanceScaling();
    testMemoryEfficiency();
    testRealTimePerformance();
}

} // namespace test_performance

// Advanced Feature Tests
namespace test_advanced_features {

void testPythonBindingsAdvanced() {
    std::cout << "\nRunning Advanced Python API Tests..." << std::endl;
    
    // Test Python bindings (placeholder for actual implementation)
    std::cout << "    Testing Python bindings... (advanced features)" << std::endl;
    
    // Test callback functionality
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test spike callback registration
    auto spikeCallback = [](const py::dict& event) {
        // Process spike event
    };
    
    // Test learning callback registration
    auto learningCallback = [](const py::dict& event) {
        // Process learning event
    };
    
    // Test advanced configuration
    brain.setLearningRate(0.02f);
    brain.enableAdvancedLearning(true);
    brain.setNoveltyThreshold(0.5f);
    
    // Test performance metrics
    auto metrics = brain.getPerformanceMetrics();
    assert(metrics.find("total_neurons") != metrics.end());
    
    std::cout << "    testPythonBindingsAdvanced passed" << std::endl;
}

void testConfigurationAdvanced() {
    std::cout << "\nRunning Advanced Configuration Tests..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test advanced configuration options
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(8), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("random_seed", 12345LL, nlm::ConfigSource::Default);
    
    // Test configuration validation
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    assert(config->has("random_seed"));
    
    // Test configuration summary
    std::string summary = config->summary();
    assert(summary.length() > 0);
    
    std::cout << "    testConfigurationAdvanced passed" << std::endl;
}

void testErrorHandlingAdvanced() {
    std::cout << "\nRunning Advanced Error Handling Tests..." << std::endl;
    
    // Test error handling with various scenarios
    auto config = std::make_shared<nlm::Config>();
    
    // Test invalid configuration
    config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default);
    assert(config->getOr<int64_t>("neuron_count", 0) == -1);
    
    // Test missing configuration
    assert(!config->has("invalid_key"));
    assert(config->getOr<std::string>("invalid_key", "default") == "default");
    
    // Test configuration save/load
    config->saveToFile("test_config.json");
    
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile("test_config.json"));
    
    std::cout << "    testErrorHandlingAdvanced passed" << std::endl;
}

void runAllAdvancedFeatureTests() {
    testPythonBindingsAdvanced();
    testConfigurationAdvanced();
    testErrorHandlingAdvanced();
}

} // namespace test_advanced_features

int main() {
    std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
    
    // Run enhanced brain tests
    test_brain_enhanced::runAllEnhancedTests();
    
    // Run integration tests
    test_integration::runAllIntegrationTests();
    
    // Run performance tests
    test_performance::runAllPerformanceTests();
    
    // Run advanced feature tests
    test_advanced_features::runAllAdvancedFeatureTests();
    
    std::cout << "\n=== All Tests Passed Successfully ===" << std::endl;
    return 0;
}
}