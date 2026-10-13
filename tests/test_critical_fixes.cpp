// Test for Critical Integration Fixes in NLM Brain System

// Test 1: Memory System Accessors Return Valid Pointers
#include "Brain.hpp"
#include "Config.hpp"
#include <cassert>
#include <iostream>

void testMemorySystemAccessors() {
    std::cout << "Testing memory system accessors..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Test memory system accessors - these should return valid pointers, not nullptr
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    
    std::cout << "✓ Memory system accessors working" << std::endl;
}

// Test 2: Episodic Memory Stores Experiences
#include <iostream>

void testEpisodicMemoryStorage() {
    std::cout << "Testing episodic memory storage..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run brain for enough steps to trigger episodic memory storage
    // Episodic memory stores every 10 steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Inject some input to stimulate activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 5.0f);
    }
    
    // Check that episodic memory has stored some episodes
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    size_t episodeCount = episodicMem->getEpisodeCount();
    std::cout << "  Episodic memory has " << episodeCount << " episodes" << std::endl;
    
    // We should have at least one episode from the 50 steps
    // (Episodic memory stores every 10 steps, so with 50 steps we'd have ~5 episodes)
    assert(episodeCount > 0);
    
    std::cout << "✓ Episodic memory storage working" << std::endl;
}

// Test 3: Checkpoint System Works
#include <iostream>
#include <fstream>
#include <string>

void testCheckpointSystem() {
    std::cout << "Testing checkpoint system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::string checkpointFile = "test_checkpoint.bin";
    
    // Save brain state
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess);
    
    // Check file was created and has content
    std::ifstream file(checkpointFile, std::ios::binary | std::ios::ate);
    assert(file.good());
    assert(file.tellg() > 1000); // Should be substantial size
    file.close();
    
    // Create a new brain and load the checkpoint
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Compare some basic stats
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain.getTotalSynapseCount());
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "✓ Checkpoint system working" << std::endl;
}

// Test 4: Memory Systems Are Properly Integrated
void testMemoryIntegration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that memory systems are actually being used
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    auto* assocMem = brain.getAssociativeMemory();
    
    assert(workingMem != nullptr);
    assert(episodicMem != nullptr);
    assert(assocMem != nullptr);
    
    // Test working memory
    assert(workingMem->getCapacity() > 0);
    assert(workingMem->getActiveTraces() == 0); // Initially no traces
    
    // Test episodic memory
    assert(episodicMem->getMaxEpisodes() > 0);
    assert(episodicMem->getEpisodeCount() == 0); // Initially no episodes
    
    // Test associative memory
    assert(assocMem->getAssociationCount() == 0); // Initially no associations
    
    // Run some steps to activate systems
    for (SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 3.0f);
    }
    
    // Now episodic memory should have some episodes
    // (it stores every 10 steps)
    size_t episodeCount = episodicMem->getEpisodeCount();
    assert(episodeCount > 0);
    
    std::cout << "✓ Memory systems integration working" << std::endl;
}

// Main test runner
int main() {
    std::cout << "=== Testing NLM Brain System Integration ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testMemorySystemAccessors();
        testEpisodicMemoryStorage();
        testCheckpointSystem();
        testMemoryIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Critical Integration Tests PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED: " << e.what() << " ===" << std::endl;
        return 1;
    }
}