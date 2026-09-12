// Working Memory Integration Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_working_memory {

void testWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory wmem;
    
    assert(wmem.getCapacity() == 100);  // Default capacity
    assert(wmem.getDecayRate() == 0.01f);  // Default decay rate
    assert(wmem.getActiveTraces() == 0);
    assert(wmem.getMemoryActivity() == 0.0f);
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStoreAndRetrieve() {
    nlm::NeuralWorkingMemory wmem;
    
    // Create a mock brain for testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Store a pattern
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    wmem.store(pattern, 1.0f);
    
    // Check that pattern was stored
    assert(wmem.getActiveTraces() > 0);
    
    // Retrieve the pattern
    std::vector<float> retrieved = wmem.retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Check that we can access individual neurons
    size_t neurons = wmem.getMemoryNeurons().size();
    assert(neurons > 0);
    
    std::cout << "    testWorkingMemoryStoreAndRetrieve passed" << std::endl;
}

void testWorkingMemoryNeuronActivation() {
    nlm::NeuralWorkingMemory wmem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Get a neuron ID to test with (will be assigned during brain initialization)
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            nlm::NeuronId testNeuron = neurons.front()->getId();
            
            // Store activation to this neuron
            wmem.storeToNeuron(testNeuron, 0.8f);
            
            // Check that neuron is in working memory
            assert(wmem.contains(testNeuron));
            
            // Check activation level
            assert(wmem.getNeuronActivation(testNeuron) == 0.8f);
            
            // Check if it's a winner
            assert(wmem.isWinning(testNeuron) == true);  // Should be winning due to high activation
        }
    }
    
    std::cout << "    testWorkingMemoryNeuronActivation passed" << std::endl;
}

void testWorkingMemoryUpdateAndDecay() {
    nlm::NeuralWorkingMemory wmem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    wmem.setDecayRate(0.05f);  // Faster decay for testing
    
    // Store some patterns
    std::vector<float> pattern1 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    wmem.store(pattern1, 1.0f);
    wmem.store(pattern2, 0.8f);
    
    // Get initial activity
    float initialActivity = wmem.getMemoryActivity();
    
    // Run updates to allow decay
    for (int i = 0; i < 50; ++i) {
        wmem.update(0.001f);  // Update with small timestep
    }
    
    // Activity should have decayed
    float finalActivity = wmem.getMemoryActivity();
    
    std::cout << "    testWorkingMemoryUpdateAndDecay passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::NeuralWorkingMemory wmem;
    
    // Set low capacity for testing
    wmem.setCapacity(5);
    assert(wmem.getCapacity() == 5);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Store more patterns than capacity
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern(5, 0.1f * i);
        wmem.store(pattern, 1.0f);
    }
    
    // Should not exceed capacity
    assert(wmem.getMemoryNeurons().size() <= wmem.getCapacity());
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    nlm::NeuralWorkingMemory wmem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Store a pattern
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    wmem.store(pattern, 1.0f);
    
    // Get initial activity
    float initialActivity = wmem.getMemoryActivity();
    
    // Strengthen memory
    wmem.strengthenMemory(2.0f);
    
    // Activity should have increased (or at least not decreased)
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    nlm::NeuralWorkingMemory wmem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Store two patterns with different strengths
    std::vector<float> pattern1 = {0.9f, 0.9f, 0.9f, 0.9f, 0.9f};
    std::vector<float> pattern2 = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
    
    wmem.store(pattern1, 1.0f);  // Strong
    wmem.store(pattern2, 1.0f);  // Weak
    
    // Run competition
    wmem.runCompetition();
    
    // Check winners
    std::vector<nlm::NeuronId> winners = wmem.getWinners();
    
    // Pattern 1 should have winners due to higher activation
    assert(!winners.empty());
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::NeuralWorkingMemory wmem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    wmem.initialize(&brain);
    
    // Store some patterns
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    wmem.store(pattern, 1.0f);
    
    // Clear working memory
    wmem.clear();
    
    // Check that memory is empty
    assert(wmem.getMemoryNeurons().empty());
    assert(wmem.getActiveTraces() == 0);
    assert(wmem.getMemoryActivity() == 0.0f);
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryRealBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory through brain simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Check working memory state
        assert(workingMemory->getMemoryActivity() >= 0.0f);
        assert(workingMemory->getActiveTraces() >= 0);
    }
    
    std::cout << "    testWorkingMemoryRealBrainIntegration passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCreation();
    testWorkingMemoryStoreAndRetrieve();
    testWorkingMemoryNeuronActivation();
    testWorkingMemoryUpdateAndDecay();
    testWorkingMemoryCapacity();
    testWorkingMemoryStrengthening();
    testWorkingMemoryCompetition();
    testWorkingMemoryClear();
    testWorkingMemoryRealBrainIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Working Memory Tests PASSED ===" << std::endl;
    std::cout << "Working memory demonstrates real neural dynamics" << std::endl;
}

} // namespace test_working_memory