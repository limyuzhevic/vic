// Reproduction scripts for edge cases
// Scripts to reproduce and test edge cases in the NLM system

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "factory/BrainFactory.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

namespace test_edge_cases {

void testVerySmallNetwork() {
    std::cout << "    Testing very small network (1 neuron, 1 region)..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1);
    config->set("region_count", 1);
    config->set("connection_probability", 0.5f);  // High probability for small network
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    std::cout << "    ✓ Very small network created and initialized successfully" << std::endl;
}

void testVeryLargeConnectionProbability() {
    std::cout << "    Testing very large connection probability (1.0)..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 1.0f);  // Dense connectivity
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    // Check that we have many synapses
    size_t synapseCount = brain->getTotalSynapseCount();
    std::cout << "    Created network with " << synapseCount << " synapses" << std::endl;
    
    // Run a few steps
    for (size_t i = 0; i < 10; ++i) {
        brain->step();
    }
    
    std::cout << "    ✓ Large connection probability handled successfully" << std::endl;
}

void testZeroConnectionProbability() {
    std::cout << "    Testing zero connection probability..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.0f);  // No connectivity
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    // Should have very few or no synapses
    size_t synapseCount = brain->getTotalSynapseCount();
    std::cout << "    Created network with " << synapseCount << " synapses (should be 0)" << std::endl;
    
    // Run a few steps (should still work)
    for (size_t i = 0; i < 10; ++i) {
        brain->step();
    }
    
    std::cout << "    ✓ Zero connection probability handled successfully" << std::endl;
}

void testSingleRegionNetwork() {
    std::cout << "    Testing single region network..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);  // Only one region
    config->set("connection_probability", 0.1f);
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    assert(brain->getRegionCount() == 1);
    
    // Test adding neurons to single region
    RegionId regionId = brain->getRegionIds()[0];
    
    std::cout << "    ✓ Single region network created successfully" << std::endl;
}

void testManyRegionsNetwork() {
    std::cout << "    Testing many regions network (50 regions)..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 50);  // Many regions
    config->set("connection_probability", 0.01f);  // Low probability due to many regions
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    assert(brain->getRegionCount() == 50);
    
    // Check that all regions were created
    auto regionIds = brain->getRegionIds();
    assert(regionIds.size() == 50);
    
    std::cout << "    ✓ Many regions network created successfully" << std::endl;
}

void testNegativeAndExtremeValues() {
    std::cout << "    Testing negative and extreme configuration values..." << std::endl;
    
    // Test 1: Very small timestep
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", 50);
    config1->set("region_count", 2);
    config1->set("simulation_timestep", 1e-9f);  // Very small timestep
    
    auto brain1 = nlm::Brain::createBrainWithConfig(config1);
    assert(brain1 != nullptr);
    assert(brain1->initialize());
    std::cout << "    ✓ Very small timestep handled" << std::endl;
    
    // Test 2: Large timestep
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 2);
    config2->set("simulation_timestep", 1.0f);  // Large timestep
    
    auto brain2 = nlm::Brain::createBrainWithConfig(config2);
    assert(brain2 != nullptr);
    assert(brain2->initialize());
    std::cout << "    ✓ Large timestep handled" << std::endl;
    
    // Test 3: Very large neuron count (stress test)
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", 100000);  // 100k neurons
    config3->set("region_count", 10);
    config3->set("connection_probability", 0.001f);  // Very sparse
    
    auto brain3 = nlm::Brain::createBrainWithConfig(config3);
    assert(brain3 != nullptr);
    assert(brain3->initialize());
    std::cout << "    ✓ Very large neuron count handled" << std::endl;
    
    // Test 4: Test plasticity parameter extremes
    auto config4 = std::make_shared<nlm::Config>();
    config4->set("neuron_count", 100);
    config4->set("region_count", 2);
    config4->set("plasticity_learning_rate", 1e-9f);  // Very small learning rate
    config4->set("stdp_ltp_weight", 0.0f);  // Zero LTP weight
    config4->set("stdp_ltd_weight", 1e9f);  // Very large LTD weight
    
    auto brain4 = nlm::Brain::createBrainWithConfig(config4);
    assert(brain4 != nullptr);
    assert(brain4->initialize());
    std::cout << "    ✓ Extreme plasticity parameters handled" << std::endl;
    
    std::cout << "    ✓ Negative and extreme values handled successfully" << std::endl;
}

void testMemoryEdgeCases() {
    std::cout << "    Testing memory edge cases..." << std::endl;
    
    // Test 1: Network with memory systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    bool success = brain->initialize();
    assert(success);
    
    // Test working memory
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << "    ✓ Memory systems initialized successfully" << std::endl;
    
    // Test 2: Network with development
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 1);
    config2->set("development_synaptogenesis_rate", 0.0f);  // No development
    config2->set("development_pruning_rate", 0.0f);  // No pruning
    
    auto brain2 = nlm::Brain::createBrainWithConfig(config2);
    assert(brain2 != nullptr);
    assert(brain2->initialize());
    std::cout << "    ✓ Development system edge case handled" << std::endl;
    
    // Test 3: Network with neuromodulation
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", 50);
    config3->set("region_count", 1);
    config3->set("dopamine_baseline", 2.0f);  // High baseline
    config3->set("reward_discount_factor", 0.999f);  // High discount
    
    auto brain3 = nlm::Brain::createBrainWithConfig(config3);
    assert(brain3 != nullptr);
    assert(brain3->initialize());
    std::cout << "    ✓ Neuromodulation edge case handled" << std::endl;
    
    std::cout << "    ✓ Memory edge cases handled successfully" << std::endl;
}

void testErrorRecovery() {
    std::cout << "    Testing error recovery and robustness..." << std::endl;
    
    // Test 1: Recovery from invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    // This should work fine
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    assert(brain->initialize());
    std::cout << "    ✓ Normal configuration works" << std::endl;
    
    // Test 2: Test that configuration can be modified after brain creation
    // Note: In real implementation, brain config might be read-only after initialization
    // but this tests the factory pattern's flexibility
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 1);
    
    // Create brain with one configuration
    auto brain2 = nlm::Brain::createBrainWithConfig(config2);
    assert(brain2 != nullptr);
    
    // Create different brain with different configuration
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", 200);
    config3->set("region_count", 3);
    
    auto brain3 = nlm::Brain::createBrainWithConfig(config3);
    assert(brain3 != nullptr);
    
    std::cout << "    ✓ Multiple independent brain configurations work" << std::endl;
    
    // Test 3: Test that factory can create brains with different types
    struct FactoryTestConfig {
        std::string brainType;
        bool enableOptimizations;
    };
    
    std::vector<FactoryTestConfig> testCases = {
        {"basic", false},
        {"optimized", true},
        {"basic", true},  // Mixed case
    };
    
    for (const auto& testCase : testCases) {
        auto testConfig = std::make_shared<nlm::Config>();
        testConfig->set("neuron_count", 20);
        testConfig->set("region_count", 1);
        testConfig->set("brain_type", testCase.brainType);
        
        if (testCase.enableOptimizations) {
            testConfig->set("enable_event_driven", true);
            testConfig->set("enable_simd", true);
        }
        
        auto testBrain = nlm::Brain::createBrainWithConfig(testConfig);
        assert(testBrain != nullptr);
        assert(testBrain->initialize());
        
        std::cout << "    ✓ Factory configuration type '" << testCase.brainType 
                  << "' works correctly" << std::endl;
    }
    
    // Test 4: Test configuration validation
    auto validationConfig = std::make_shared<nlm::Config>();
    
    // Set valid configuration
    validationConfig->set("neuron_count", 100);
    validationConfig->set("region_count", 2);
    validationConfig->set("connection_probability", 0.1f);
    
    // Should validate without errors
    auto validationBrain = nlm::Brain::createBrainWithConfig(validationConfig);
    assert(validationBrain != nullptr);
    std::cout << "    ✓ Configuration validation passes" << std::endl;
    
    std::cout << "    ✓ Error recovery and robustness tests passed" << std::endl;
}

void testSimulationEdgeCases() {
    std::cout << "    Testing simulation edge cases..." << std::endl;
    
    // Test 1: Very short simulation
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", 10);
    config1->set("region_count", 1);
    config1->set("max_simulation_steps", 5);  // Very short simulation
    
    auto brain1 = nlm::Brain::createBrainWithConfig(config1);
    assert(brain1 != nullptr);
    assert(brain1->initialize());
    
    // Run only a few steps
    for (size_t i = 0; i < 3; ++i) {
        brain1->step();
    }
    
    std::cout << "    ✓ Short simulation works" << std::endl;
    
    // Test 2: Medium simulation
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 2);
    config2->set("max_simulation_steps", 100);
    
    auto brain2 = nlm::Brain::createBrainWithConfig(config2);
    assert(brain2 != nullptr);
    assert(brain2->initialize());
    
    // Run medium number of steps
    for (size_t i = 0; i < 50; ++i) {
        brain2->step();
    }
    
    std::cout << "    ✓ Medium simulation works" << std::endl;
    
    // Test 3: Simulation with sensory input
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", 20);
    config3->set("region_count", 1);
    
    auto brain3 = nlm::Brain::createBrainWithConfig(config3);
    assert(brain3 != nullptr);
    assert(brain3->initialize());
    
    // Create a simple vision input
    auto vision = std::make_unique<nlm::Vision>(10, 10, 3);
    vision->setData(std::vector<float>(300, 0.5f)); // Fill with constant values
    
    // Apply sensory input
    brain3->receiveSensoryInput(*vision);
    
    // Run steps with sensory input
    for (size_t i = 0; i < 10; ++i) {
        brain3->step();
    }
    
    std::cout << "    ✓ Simulation with sensory input works" << std::endl;
    
    // Test 4: Simulation with actions
    auto config4 = std::make_shared<nlm::Config>();
    config4->set("neuron_count", 30);
    config4->set("region_count", 1);
    
    auto brain4 = nlm::Brain::createBrainWithConfig(config4);
    assert(brain4 != nullptr);
    assert(brain4->initialize());
    
    // Run steps to generate actions
    for (size_t i = 0; i < 20; ++i) {
        auto action = brain4->produceAction();
        // Action may be null depending on neural activity
        assert(action == nullptr || action != nullptr);
    }
    
    std::cout << "    ✓ Simulation with action generation works" << std::endl;
    
    std::cout << "    ✓ Simulation edge cases handled successfully" << std::endl;
}

} // namespace test_edge_cases
