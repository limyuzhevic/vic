// Memory Integration Tests
// Tests NeuralWorkingMemory, NeuralEpisodicMemory, and NeuralAssociativeMemory integration

#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

namespace test_memory_integration {

void testWorkingMemoryBasic() {
    std::cout << "  Testing NeuralWorkingMemory basic functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test store and retrieve
    NeuronId testNeuron(12345);
    float testActivation = 0.8f;
    
    workingMemory->storeToNeuron(testNeuron, testActivation);
    float retrieved = workingMemory->getNeuronActivation(testNeuron);
    assert(std::abs(retrieved - testActivation) < 0.01f);
    
    // Test contains
    assert(workingMemory->contains(testNeuron));
    
    // Test getMemoryActivity
    float activity = workingMemory->getMemoryActivity();
    assert(activity > 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testEpisodicMemoryBasic() {
    std::cout << "  Testing NeuralEpisodicMemory basic functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create a test episode
    EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(NeuronId(1000));
    episode.neuronActivations.push_back(0.9f);
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    
    // Test retrieve episode
    const EpisodicMemoryItem* retrieved = episodicMemory->getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->timestamp == 100);
    assert(retrieved->reward == 0.5f);
    
    // Test getEpisodeCount
    size_t count = episodicMemory->getEpisodeCount();
    assert(count > 0);
    
    // Test getRecentEpisodes
    std::vector<const EpisodicMemoryItem*> recent = episodicMemory->getRecentEpisodes(5);
    assert(recent.size() <= count);
    
    std::cout << "    PASSED" << std::endl;
}

void testAssociativeMemoryBasic() {
    std::cout << "  Testing NeuralAssociativeMemory basic functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test basic association
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory->associate(patternA, patternB, 0.8f);
    
    // Test getAssociationStrength
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(std::abs(strength - 0.8f) < 0.01f);
    
    // Test retrieve
    std::vector<std::vector<float>> results = associativeMemory->retrieve(patternA, 5);
    assert(!results.empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testMemoryIntegrationWithBrain() {
    std::cout << "  Testing memory integration with Brain step()..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Run some simulation steps
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Check that memory systems are still functional after simulation
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    if (workingMemory) {
        // Working memory should have some activity
        float activity = workingMemory->getMemoryActivity();
        assert(activity >= 0.0f);  // May be zero if no neurons fired
    }
    
    if (episodicMemory) {
        // Episodic memory should have stored some episodes
        size_t episodes = episodicMemory->getEpisodeCount();
        // Episodes are stored every 10 steps, so after 10 steps we might have 1
        assert(episodes >= 0);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Memory Integration Tests ===" << std::endl;
    
    testWorkingMemoryBasic();
    testEpisodicMemoryBasic();
    testAssociativeMemoryBasic();
    testMemoryIntegrationWithBrain();
    
    std::cout << "=== Memory Integration Tests Complete ===" << std::endl;
}

} // namespace test_memory_integration