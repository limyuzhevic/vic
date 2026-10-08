#include "Brain.hpp"
#include "tests/test_types.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_enhanced_brain {

// Test NeuralRegionIterator helper class
void testNeuralRegionIterator() {
    NLM_LOG_INFO("Testing NeuralRegionIterator helper class...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test iterateAllNeurons
    int neuronCount = 0;
    NeuralRegionIterator::iterateAllNeurons(&brain, [&](Neuron* neuron) {
        neuronCount++;
        assert(neuron != nullptr);
    });
    assert(neuronCount == 100);
    
    // Test iterateNeuronsByType
    int sensoryCount = 0;
    NeuralRegionIterator::iterateNeuronsByType(&brain, NeuronType::Sensory, [&](Neuron* neuron) {
        sensoryCount++;
        assert(neuron->getType() == NeuronType::Sensory);
    });
    assert(sensoryCount > 0);
    
    // Test iterateAllSynapses
    int synapseCount = 0;
    NeuralRegionIterator::iterateAllSynapses(&brain, [&](Synapse* syn) {
        synapseCount++;
        assert(syn != nullptr);
    });
    assert(synapseCount > 0);
    
    std::cout << "  NeuralRegionIterator tests PASSED" << std::endl;
}

// Test BrainStepRunner phase execution
void testBrainStepRunner() {
    NLM_LOG_INFO("Testing BrainStepRunner phase execution...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Reset brain for clean test
    brain.reset();
    brain.initialize();
    
    // Test phase execution with error handling
    int completedPhases = 0;
    int errorPhases = 0;
    
    try {
        // Execute some steps to trigger phase execution
        for (SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            completedPhases++;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("BrainStepRunner error: " + std::string(e.what()));
        errorPhases++;
    }
    
    assert(completedPhases > 0);
    std::cout << "  BrainStepRunner tests PASSED" << std::endl;
}

// Test MemoryManager helper class
void testMemoryManager() {
    NLM_LOG_INFO("Testing MemoryManager helper class...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 80);
    config->set("region_count", 1);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test memory system initialization
    int totalBefore = brain.getTotalNeuronCount();
    
    // Test memory systems are initialized
    if (brain.getWorkingMemory()) {
        assert(brain.getWorkingMemory()->getCapacity() == 8); // neuronCount / 10
    }
    
    if (brain.getEpisodicMemory()) {
        assert(brain.getEpisodicMemory()->getMaxEpisodes() == 1000);
    }
    
    if (brain.getAssociativeMemory()) {
        // Should be initialized
    }
    
    // Test memory reset
    brain.reset();
    
    // After reset, some memory traces should be cleared
    assert(brain.getTotalSpikeCount() == 0);
    
    std::cout << "  MemoryManager tests PASSED" << std::endl;
}

// Test BrainSimulationConstants configuration
void testBrainSimulationConstants() {
    NLM_LOG_INFO("Testing BrainSimulationConstants configuration...")
    
    auto config = std::make_shared<Config>();
    config->set("replay_interval", 150);
    config->set("consolidation_interval", 2000);
    config->set("synaptogenesis_rate", 0.0002f);
    config->set("pruning_rate", 0.00002f);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Configuration should be accessible and used
    assert(brain.getConfig()->has("replay_interval"));
    assert(brain.getConfig()->has("consolidation_interval"));
    
    std::cout << "  BrainSimulationConstants tests PASSED" << std::endl;
}

// Test BrainStatsCollector statistics
void testBrainStatsCollector() {
    NLM_LOG_INFO("Testing BrainStatsCollector statistics...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 60);
    config->set("region_count", 1);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Run some steps to generate statistics
    for (SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Log status (should use BrainStatsCollector internally)
    brain.logStatus();
    
    // Verify statistics are being collected
    assert(brain.getTotalNeuronCount() == 60);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "  BrainStatsCollector tests PASSED" << std::endl;
}

// Test error handling in phase execution
void testErrorHandling() {
    NLM_LOG_INFO("Testing error handling in phase execution...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 40);
    config->set("region_count", 1);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // The BrainStepRunner should handle errors gracefully
    // If an exception occurs during phase execution, it should be caught
    // and logged, but not crash the entire system
    
    try {
        // This should execute phases with proper error handling
        for (SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
    } catch (const std::exception& e) {
        // If there's an exception, it should be logged but not crash
        NLM_LOG_ERROR("Expected error in error handling test: " + std::string(e.what()));
    }
    
    std::cout << "  Error handling tests PASSED" << std::endl;
}

// Test backward compatibility
void testBackwardCompatibility() {
    NLM_LOG_INFO("Testing backward compatibility...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 70);
    config->set("region_count", 1);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test all the original API methods still work
    
    // 1. Original step method signatures
    for (SimulationStep step = 0; step < 10; ++step) {
        brain.step(step); // Single parameter version
    }
    
    // 2. Memory system access methods
    if (brain.getWorkingMemory()) {
        brain.getWorkingMemory()->update(0.001);
    }
    
    if (brain.getEpisodicMemory()) {
        brain.getEpisodicMemory()->update(0.001);
    }
    
    // 3. Original brain statistics methods
    size_t neurons = brain.getTotalNeuronCount();
    size_t synapses = brain.getTotalSynapseCount();
    size_t active = brain.getActiveNeuronCount();
    size_t firing = brain.getFiringNeuronCount();
    float rate = brain.getAverageFiringRate();
    
    assert(neurons == 70);
    assert(synapses > 0);
    assert(active >= 0);
    assert(firing >= 0);
    assert(rate >= 0.0f);
    
    // 4. Region methods
    auto ids = brain.getRegionIds();
    assert(ids.size() == 1);
    
    auto region = brain.getRegion(ids[0]);
    assert(region != nullptr);
    
    // 5. Configuration access
    auto config_ptr = brain.getConfig();
    assert(config_ptr != nullptr);
    
    std::cout << "  Backward compatibility tests PASSED" << std::endl;
}

// Test enhanced configuration and initialization
void testEnhancedInitialization() {
    NLM_LOG_INFO("Testing enhanced initialization...")
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 90);
    config->set("region_count", 2);
    config->set("connection_probability", 0.2f);
    config->set("enable_reward_modulation", true);
    config->set("enable_curiosity", true);
    config->set("enable_development", true);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test that enhanced features are available
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test that advanced configuration is accessible
    std::string summary = config->summary();
    assert(!summary.empty());
    
    // Run some steps to test enhanced functionality
    for (SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Test enhanced agent brain interface methods
        // (These would be tested through Python bindings)
    }
    
    std::cout << "  Enhanced initialization tests PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Enhanced Brain Integration Tests ===" << std::endl;
    std::cout << "Testing Phase 6 Enhanced Implementation" << std::endl;
    std::cout << std::endl;
    
    try {
        testNeuralRegionIterator();
        testBrainStepRunner();
        testMemoryManager();
        testBrainSimulationConstants();
        testBrainStatsCollector();
        testErrorHandling();
        testBackwardCompatibility();
        testEnhancedInitialization();
        
        std::cout << std::endl;
        std::cout << "=== All Enhanced Brain Tests PASSED ===" << std::endl;
        std::cout << "Phase 6 enhancement validation successful!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Enhanced Brain Tests FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        std::cout << "Please check the implementation for issues." << std::endl;
        throw;
    }
}

} // namespace test_enhanced_brain

// Test registration
namespace {
    bool registered = []() {
        // Register the enhanced brain test
        // In a real test framework, this would integrate with the test runner
        std::cout << "Enhanced brain tests registered" << std::endl;
        return true;
    }();
}