// Test program to verify working memory integration
// This program tests that the working memory system is properly integrated with the brain

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <iostream>
#include <cassert>

void testWorkingMemoryCreation() {
    std::cout << "Testing working memory creation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test that getWorkingMemory() returns a non-null pointer after initialization
    assert(brain.initialize());
    assert(brain.getWorkingMemory() != nullptr);
    
    // Test that working memory has been initialized with brain reference
    // Note: We can't directly access the internal brain_ pointer, but we can test functionality
    brain.getWorkingMemory()->setCapacity(50);
    assert(brain.getWorkingMemory()->getCapacity() == 50);
    
    std::cout << "  ✓ Working memory creation test passed" << std::endl;
}

void testWorkingMemoryFunctionality() {
    std::cout << "Testing working memory functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Get some neurons from the brain to store in working memory
    std::vector<nlm::NeuronId> testNeurons;
    size_t neuronsNeeded = std::min(static_cast<size_t>(5), brain.getTotalNeuronCount());
    
    size_t neuronCount = 0;
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            auto neurons = pop->getNeurons();
            for (auto* n : neurons) {
                if (neuronCount < neuronsNeeded) {
                    testNeurons.push_back(n->getId());
                    neuronCount++;
                }
            }
            if (neuronCount >= neuronsNeeded) break;
        }
        if (neuronCount >= neuronsNeeded) break;
    }
    
    // Test storing neurons in working memory
    for (size_t i = 0; i < testNeurons.size() && i < wm->getCapacity(); ++i) {
        wm->storeToNeuron(testNeurons[i], 0.5f + 0.1f * i);
        assert(wm->contains(testNeurons[i]));
        assert(wm->getNeuronActivation(testNeurons[i]) >= 0.5f);
    }
    
    // Test retrieval
    auto retrieved = wm->retrieve();
    assert(retrieved.size() == std::min(testNeurons.size(), wm->getCapacity()));
    
    // Test update (inject maintenance currents)
    wm->update(0.001);
    
    std::cout << "  ✓ Working memory functionality test passed" << std::endl;
}

void testWorkingMemoryBrainIntegration() {
    std::cout << "Testing working memory brain integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Run several brain steps to ensure working memory is updated
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
        
        // Verify working memory is still accessible
        assert(wm->getActiveTraces() >= 0);
    }
    
    std::cout << "  ✓ Working memory brain integration test passed" << std::endl;
}

int main() {
    std::cout << "=== Working Memory Integration Test ===" << std::endl;
    std::cout << "Testing NeuralWorkingMemory integration with Brain class..." << std::endl;
    std::cout << std::endl;
    
    try {
        testWorkingMemoryCreation();
        std::cout << std::endl;
        
        testWorkingMemoryFunctionality();
        std::cout << std::endl;
        
        testWorkingMemoryBrainIntegration();
        std::cout << std::endl;
        
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "=== Test FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}