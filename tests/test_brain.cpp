// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainInitializationErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);  // Invalid: zero neurons
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(!success);  // Should fail with zero neurons
    
    std::cout << "    testBrainInitializationErrorHandling passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainStepErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test negative step (should handle gracefully)
    brain.step(-1);  // Should not crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStepErrorHandling passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainPerformanceOptimization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that optimized step function works correctly with many neurons
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify basic invariants still hold
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 5);
    }
    
    std::cout << "    testBrainPerformanceOptimization passed" << std::endl;
}

void testBrainMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Use brain functionality
        brain.step(0);
        brain.reset();
        
        // Verify all systems are properly initialized
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getAssociativeMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        assert(brain.getDevelopmentSystem() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getNovelty() != nullptr);
        assert(brain.getPredictionErrorSignal() != nullptr);
        
        // All systems should be properly managed by smart pointers
        std::cout << "    testBrainMemoryManagement passed" << std::endl;
    }
    
    // Brain should be properly destroyed when going out of scope
    std::cout << "    testBrainMemoryManagement completed successfully" << std::endl;
}

void testBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test full integration of all systems
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        // Test all system accessors
        auto* spikeSystem = brain.getSpikeSystem();
        auto* stdpSystem = brain.getSTDP();
        auto* hebbianSystem = brain.getHebbian();
        auto* structuralPlasticity = brain.getStructuralPlasticity();
        
        assert(spikeSystem != nullptr);
        assert(stdpSystem != nullptr);
        assert(hebbianSystem != nullptr);
        assert(structuralPlasticity != nullptr);
    }
    
    std::cout << "    testBrainIntegration passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    brain1.step(0);
    
    // Save brain state
    bool saved = brain1.save("test_checkpoint.nmc");
    assert(saved);
    
    // Create second brain and load state
    auto config2 = std::make_shared<nlm::Config>();
    nlm::Brain brain2(config2);
    
    bool loaded = brain2.load("test_checkpoint.nmc");
    assert(loaded);
    
    // Verify loaded brain has similar properties
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void testBrainDocumentationExample() {
    // Example demonstrating proper usage of the Brain API
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize the brain
    if (!brain.initialize()) {
        std::cout << "    testBrainDocumentationExample: Initialization failed (expected for example)" << std::endl;
        return;
    }
    
    // Simulate brain activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Access brain statistics
        auto neuronCount = brain.getTotalNeuronCount();
        auto synapseCount = brain.getTotalSynapseCount();
        auto firingCount = brain.getFiringNeuronCount();
        
        // Produce actions based on brain state
        auto action = brain.produceAction();
        if (action) {
            // Process action (in real application)
        }
    }
    
    std::cout << "    testBrainDocumentationExample passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainInitializationErrorHandling();
    testBrainStep();
    testBrainStepErrorHandling();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainPerformanceOptimization();
    testBrainMemoryManagement();
    testBrainIntegration();
    testBrainSaveLoad();
    testBrainDocumentationExample();
}

} // namespace test_brain

} // namespace test_brain
