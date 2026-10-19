// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/MemorySystem.hpp"
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
    
    // Test unified memory system
    auto* memorySystem = brain.getMemorySystem();
    assert(memorySystem != nullptr);
    
    // Test backward compatibility
    auto* neuralWorkingMemory = brain.getNeuralWorkingMemory();
    assert(neuralWorkingMemory != nullptr);
    
    auto* neuralEpisodicMemory = brain.getNeuralEpisodicMemory();
    assert(neuralEpisodicMemory != nullptr);
    
    auto* neuralAssociativeMemory = brain.getNeuralAssociativeMemory();
    assert(neuralAssociativeMemory != nullptr);
    
    // Verify old-style memory access still works
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
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

void testMemorySystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get the unified memory system
    auto* memorySystem = brain.getMemorySystem();
    assert(memorySystem != nullptr);
    
    // Test memory system operations
    assert(memorySystem->getSystemLoad() >= 0.0f);
    assert(memorySystem->getTotalMemoryUsage() >= 0);
    
    // Test propagation
    std::vector<float> testPattern{0.1f, 0.2f, 0.3f};
    memorySystem->propagateToLowerLevels(testPattern, 0.5f);
    memorySystem->propagateToHigherLevels(testPattern, 0.5f);
    
    std::cout << "    testMemorySystem passed" << std::endl;
}

void testMemorySystemBackwardCompatibility() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test backward compatibility with neural memory interfaces
    auto* neuralWorkingMemory = brain.getNeuralWorkingMemory();
    assert(neuralWorkingMemory != nullptr);
    
    auto* neuralEpisodicMemory = brain.getNeuralEpisodicMemory();
    assert(neuralEpisodicMemory != nullptr);
    
    auto* neuralAssociativeMemory = brain.getNeuralAssociativeMemory();
    assert(neuralAssociativeMemory != nullptr);
    
    // Test old-style memory access
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    std::cout << "    testMemorySystemBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testMemorySystem();
    testMemorySystemBackwardCompatibility();
}

} // namespace test_brain
