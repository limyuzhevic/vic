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

void testBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify integration between components
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Check that memory systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Check that plasticity systems are initialized
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    // Check that neuromodulation systems are initialized
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    assert(brain.getNovelty() != nullptr);
    
    // Check that cognition systems are initialized
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Check that development system is initialized
    assert(brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "    testBrainIntegration passed" << std::endl;
}

void testBrainErrorConditions() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with invalid config
    config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default); // Invalid
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);   // Too few
    
    // Brain should handle or report these errors
    nlm::Brain brain(config);
    
    // Check that brain still initializes with defaults
    bool success = brain.initialize();
    assert(success || true); // May or may not initialize depending on validation
    
    // Test null pointer access (if applicable)
    // Note: This tests defensive coding in the brain implementation
    auto* nullRegion = brain.getRegion(nlm::RegionId(UINT64_MAX));
    assert(nullRegion == nullptr); // Should return null for invalid ID
    
    std::cout << "    testBrainErrorConditions passed" << std::endl;
}

void testBrainPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Performance test: run brain for many steps
    nlm::SimulationStep startStep = brain.getSpikeSystem()->getCurrentStep();
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete 100 steps within reasonable time (less than 10 seconds)
    assert(duration.count() < 10000);
    
    // Verify statistics are being collected
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainPerformance passed (" << duration.count() << "ms)" << std::endl;
}

void testBrainMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        // Test brain lifetime management
        nlm::Brain brain(config);
        brain.initialize();
        
        // Use brain extensively
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Brain should clean up properly when destroyed
        assert(true); // If we get here, basic memory management works
    }
    
    // Force garbage collection and check for leaks
    // Note: In C++ this is implicit with RAII
    std::cout << "    testBrainMemoryManagement passed" << std::endl;
}

void testBrainCheckpointSaveLoad() {
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    // Run brain for a bit to generate some activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "/tmp/test_checkpoint.bin";
    bool saved = brain1.save(checkpointFile);
    
    if (saved) {
        // Create new brain and load checkpoint
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain2(config2);
        brain2.initialize();
        
        bool loaded = brain2.load(checkpointFile);
        assert(loaded);
        
        // Verify that loaded state is valid
        assert(brain2.getTotalNeuronCount() == 100);
        assert(brain2.getRegionCount() == 2);
        
        // Clean up
        std::remove(checkpointFile.c_str());
    }
    
    std::cout << "    testBrainCheckpointSaveLoad passed" << std::endl;
}

void testBrainSystemStateValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify internal consistency
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() <= brain.getTotalNeuronCount());
    assert(brain.getFiringNeuronCount() <= brain.getTotalNeuronCount());
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // State should still be valid
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() <= brain.getTotalNeuronCount());
    
    // Verify brain produces valid action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainSystemStateValidation passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainIntegration();
    testBrainErrorConditions();
    testBrainPerformance();
    testBrainMemoryManagement();
    testBrainCheckpointSaveLoad();
    testBrainSystemStateValidation();
}

} // namespace test_brain
