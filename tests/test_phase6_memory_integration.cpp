// Phase 6 Memory Integration Tests
// Tests integration of all memory systems with neural dynamics

#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/Memory.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_phase6_memory {

void testWorkingMemoryIntegration() {
    std::cout << "    Testing working memory integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", 1);
    config->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return;
    }
    
    // Test that working memory is integrated
    if (!brain->getWorkingMemory()) {
        std::cout << "    Working memory is null (expected in some configurations)" << std::endl;
        return;
    }
    
    // Simulate some spikes to test working memory functionality
    auto* workingMemory = brain->getWorkingMemory();
    
    // Check that working memory integration is successful
    bool integrationSuccess = false;
    
    if (integrationSuccess) {
        std::cout << "    ✓ Working memory integration passed" << std::endl;
    } else {
        std::cout << "    ✗ Working memory integration failed" << std::endl;
    }
}

void testEpisodicMemoryIntegration() {
    std::cout << "    Testing episodic memory integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", 1);
    config->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return;
    }
    
    // Test episodic memory functionality
    bool integrationSuccess = false;
    
    if (integrationSuccess) {
        std::cout << "    ✓ Episodic memory integration passed" << std::endl;
    } else {
        std::cout << "    ✗ Episodic memory integration failed" << std::endl;
    }
}

void testAssociativeMemoryIntegration() {
    std::cout << "    Testing associative memory integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", 1);
    config->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return;
    }
    
    // Test associative memory integration
    bool integrationSuccess = false;
    
    if (integrationSuccess) {
        std::cout << "    ✓ Associative memory integration passed" << std::endl;
    } else {
        std::cout << "    ✗ Associative memory integration failed" << std::endl;
    }
}

void testMemoryConnectivity() {
    std::cout << "    Testing memory-neural connectivity..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", 1);
    config->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return;
    }
    
    // Test memory systems connectivity with neural processing
    bool connectivitySuccess = false;
    
    if (connectivitySuccess) {
        std::cout << "    ✓ Memory-neural connectivity integration passed" << std::endl;
    } else {
        std::cout << "    ✗ Memory-neural connectivity integration failed" << std::endl;
    }
}

void runAll() {
    std::cout << "=== Phase 6 Memory Integration Tests ===" << std::endl;
    
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
    testMemoryConnectivity();
    
    std::cout << "=== Memory Integration Tests Complete ===" << std::endl;
}

} // namespace test_phase6_memory