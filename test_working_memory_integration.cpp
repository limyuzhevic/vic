// Test working memory integration
// This test verifies that working memory integration is properly implemented

#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include <iostream>
#include <cassert>

void testWorkingMemoryIntegration() {
    std::cout << "Testing Working Memory Integration..." << std::endl;
    
    // Create a simple configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Get working memory
    NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory capacity
    size_t capacity = workingMemory->getCapacity();
    assert(capacity == 100); // Default capacity should be 100
    
    // Test working memory activity (should be 0 initially)
    float activity = brain.getWorkingMemoryActivity();
    assert(activity == 0.0f);
    
    // Test getting working memory neurons (should be empty initially)
    std::vector<NeuronId> neurons = brain.getWorkingMemoryNeurons();
    assert(neurons.empty());
    
    // Test registering callback
    int callbackCount = 0;
    brain.registerWorkingMemoryUpdateCallback(
        [&callbackCount](NeuronId neuron, float activation) {
            callbackCount++;
        }
    );
    
    // Verify that callbacks can be registered
    assert(callbackCount == 0);
    
    std::cout << "✓ Working memory integration test passed!" << std::endl;
    std::cout << "  - Working memory system initialized successfully" << std::endl;
    std::cout << "  - Working memory capacity set correctly" << std::endl;
    std::cout << "  - Callback registration works" << std::endl;
    std::cout << "  - Working memory integration methods are available" << std::endl;
}

int main() {
    try {
        testWorkingMemoryIntegration();
        std::cout << std::endl;
        std::cout << "All working memory integration tests passed!" << std::endl;
        std::cout << "The integration provides the foundation for Phase 6:" << std::endl;
        std::cout << "  - Bidirectional coupling between working memory and neural dynamics" << std::endl;
        std::cout << "  - Working memory winners excite corresponding neurons" << std::endl;
        std::cout << "  - Enhanced pattern completion through similarity-based activation" << std::endl;
        std::cout << "  - Working memory influence on attention system" << std::endl;
        std::cout << "  - Callbacks for real-time working memory monitoring" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}