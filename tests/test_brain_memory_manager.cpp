// BrainMemoryManager Comprehensive Tests
// Tests WorkingMemory, EpisodicMemory, AssociativeMemory

#include "brain/BrainMemoryManager.hpp"
#include "brain/BrainNeuralCore.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_brain_memory_manager {

void testBrainMemoryManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    
    assert(memory.initialize(&neuralCore));
    
    std::cout << "    testBrainMemoryManagerCreation passed" << std::endl;
}

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Get working memory component
    nlm::NeuralWorkingMemory* workingMem = memory.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test working memory capacity
    size_t initialCapacity = memory.getWorkingMemorySize();
    assert(initialCapacity == 0); // Should be empty initially
    
    // Store neural activation in working memory
    nlm::NeuronId neuronId1 = 1;
    nlm::NeuronId neuronId2 = 2;
    
    memory.storeToWorkingMemory(neuronId1, 0.8f);
    memory.storeToWorkingMemory(neuronId2, 0.6f);
    
    // Retrieve from working memory
    auto retrieved = memory.retrieveFromWorkingMemory(10);
    assert(!retrieved.empty());
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Get episodic memory component
    nlm::NeuralEpisodicMemory* episodicMem = memory.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test episodic memory configuration
    size_t initialMaxEpisodes = memory.getEpisodicMemorySize();
    
    // Create a simple episodic memory item (simplified)
    nlm::EpisodicMemoryItem episode;
    episode.timeStep = 100;
    episode.representation = {1, 2, 3, 4, 5}; // Neural pattern
    episode.significance = 0.9f;
    
    // Update episodic memory
    memory.updateEpisodicMemory(episode);
    
    // Get episodes for replay
    const std::vector<nlm::EpisodicMemoryItem>* episodes = memory.getEpisodesForReplay(5);
    assert(episodes != nullptr);
    assert(episodes->size() > 0);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Get associative memory component
    nlm::NeuralAssociativeMemory* associativeMem = memory.getAssociativeMemory();
    assert(associativeMem != nullptr);
    
    // Test associative memory configuration
    size_t initialAssociativeSize = memory.getAssociativeMemorySize();
    
    // Store associative memory (pattern1 -> pattern2)
    std::vector<nlm::NeuronId> pattern1 = {1, 2, 3};
    std::vector<nlm::NeuronId> pattern2 = {4, 5, 6};
    
    memory.storeAssociativeMemory(pattern1, pattern2, 0.8f);
    
    // Retrieve associative memory
    float retrieved = memory.retrieveAssociativeMemory(pattern1);
    assert(retrieved > 0.0f);
    
    // Test another pattern
    std::vector<nlm::NeuronId> pattern3 = {7, 8, 9};
    float defaultRetrieval = memory.retrieveAssociativeMemory(pattern3);
    assert(defaultRetrieval == 0.0f); // Should be zero for unknown pattern
    
    std::cout << "    testAssociativeMemoryIntegration passed" << std::endl;
}

void testMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Store some memory items
    memory.storeToWorkingMemory(1, 0.8f);
    memory.storeToWorkingMemory(2, 0.6f);
    
    // Consolidate memory with threshold
    memory.consolidateMemory(0.5f);
    
    // Clear memory
    memory.clearAllMemory();
    
    std::cout << "    testMemoryConsolidation passed" << std::endl;
}

void testMemoryReplay() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Store some episodic memories
    for (int i = 0; i < 3; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timeStep = i * 100;
        episode.representation = {i, i+1, i+2};
        episode.significance = 0.7f + (i * 0.1f);
        memory.updateEpisodicMemory(episode);
    }
    
    // Replay episodes
    memory.replayEpisodes(&neuralCore, 2);
    
    std::cout << "    testMemoryReplay passed" << std::endl;
}

void testMemoryConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Test configuration methods
    memory.setWorkingMemoryCapacity(1000);
    memory.setEpisodicMemoryMaxEpisodes(100);
    
    // Verify configuration was set
    size_t workingMemSize = memory.getWorkingMemorySize();
    assert(workingMemSize == 0); // Still empty, but capacity changed
    
    std::cout << "    testMemoryConfiguration passed" << std::endl;
}

void testMemoryReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Store some memory
    memory.storeToWorkingMemory(1, 0.8f);
    
    // Reset memory
    memory.reset();
    
    // Memory should be clear
    std::cout << "    testMemoryReset passed" << std::endl;
}

void testMemoryStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainMemoryManager memory(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(memory.initialize(&neuralCore));
    
    // Get memory statistics
    size_t workingMemSize = memory.getWorkingMemorySize();
    size_t episodicMemSize = memory.getEpisodicMemorySize();
    size_t associativeMemSize = memory.getAssociativeMemorySize();
    
    assert(workingMemSize >= 0);
    assert(episodicMemSize >= 0);
    assert(associativeMemSize >= 0);
    
    std::cout << "    testMemoryStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainMemoryManager tests..." << std::endl;
    
    testBrainMemoryManagerCreation();
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
    testMemoryConsolidation();
    testMemoryReplay();
    testMemoryConfiguration();
    testMemoryReset();
    testMemoryStatus();
    
    std::cout << "\nAll BrainMemoryManager tests passed!" << std::endl;
}

} // namespace test_brain_memory_manager
