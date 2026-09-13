#include <iostream>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <cassert>

void testConfigHas() {
    std::cout << "Testing Config::has() method..." << std::endl;
    
    nlm::Config config;
    
    // Test has() for non-existent key
    assert(!config.has("nonexistent_key"));
    
    // Test has() for existing key
    config.set("test_int", 42);
    assert(config.has("test_int"));
    
    config.set("test_string", "hello");
    assert(config.has("test_string"));
    
    config.set("test_double", 3.14);
    assert(config.has("test_double"));
    
    config.set("test_bool", true);
    assert(config.has("test_bool"));
    
    std::cout << "✓ Config::has() method works correctly" << std::endl;
}

void testBrainInitialization() {
    std::cout << "Testing Brain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set default configuration
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 12345ULL);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    
    // Test basic properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    
    // Test that developmental stage is properly initialized
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    // Should not crash - stage should be valid
    
    std::cout << "✓ Brain initialization works correctly" << std::endl;
}

void testBrainNullPointers() {
    std::cout << "Testing Brain for null pointer issues..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic configuration
    config->set("neuron_count", 10);
    config->set("region_count", 1);
    config->set("random_seed", 999ULL);
    
    nlm::Brain brain(config);
    
    // Initialize brain - this should not crash
    bool initResult = brain.initialize();
    assert(initResult);
    
    // Test that key components are accessible
    assert(brain.getSpikeSystem() != nullptr);
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
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
    
    std::cout << "✓ Brain has all required components (no null pointers)" << std::endl;
}

int main() {
    std::cout << "=" << std::endl;
    std::cout << "Testing NLM Phase 6 Fixes" << std::endl;
    std::cout << "=" << std::endl;
    
    testConfigHas();
    testBrainInitialization();
    testBrainNullPointers();
    
    std::cout << std::endl;
    std::cout << "=" << std::endl;
    std::cout << "All tests passed! ✓" << std::endl;
    std::cout << "=" << std::endl;
    
    return 0;
}

