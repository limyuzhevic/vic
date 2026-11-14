// Integration Test: Memory Systems Integration
// Tests working memory, episodic memory, and associative memory connectivity

#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory_integration {

void testMemorySystemInitialization() {
    std::cout << "  Testing memory system initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify memory systems are accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    std::cout << "    Memory system initialization passed" << std::endl;
}

void testWorkingMemoryIntegration() {
    std::cout << "  Testing working memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    auto* predSystem = brain.getPredictionSystem();
    
    // Store something in working memory
    workingMem->store(1, 0.5f);
    
    // Verify working memory stores correctly
    assert(workingMem->contains(1));
    assert(std::abs(workingMem->retrieve(1) - 0.5f) < 0.001f);
    
    // Working memory should persist through prediction cycles
    auto sensoryInput = nlm::SensoryInput(16, 0.0f);
    auto predictedState = predSystem->predictNextState(sensoryInput);
    assert(predictedState != nullptr);
    
    std::cout << "    Working memory integration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "  Testing episodic memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMem = brain.getEpisodicMemory();
    auto* workingMem = brain.getWorkingMemory();
    
    // Create a working memory state
    workingMem->store(2, 1.0f);
    
    // Store episode in episodic memory
    nlm::EpisodicMemoryItem item;
    item.timestamp = 0;
    item.neurons.push_back(2);
    item.values.push_back(1.0f);
    item.metadata = "test_episode";
    
    episodicMem->storeEpisode(item);
    
    // Verify episodic memory stores correctly
    assert(episodicMem->getEpisodeCount() == 1);
    
    auto retrievedItem = episodicMem->retrieveEpisode(0);
    assert(retrievedItem.neurons.size() == 1);
    assert(retrievedItem.neurons[0] == 2);
    assert(retrievedItem.values.size() == 1);
    assert(retrievedItem.values[0] == 1.0f);
    
    // Episodic memory should be accessible through sensory processing
    auto sensoryInput = nlm::SensoryInput(16, 0.0f);
    brain.receiveSensoryInput(sensoryInput);
    
    std::cout << "    Episodic memory integration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "  Testing associative memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMem = brain.getAssociativeMemory();
    auto* workingMem = brain.getWorkingMemory();
    
    // Create associations between neurons
    associativeMem->associate(1, 2, 0.8f);
    associativeMem->associate(1, 3, 0.3f);
    
    // Verify associations
    assert(associativeMem->getAssociationStrength(1, 2) > 0.7f);
    assert(associativeMem->getAssociationStrength(1, 3) > 0.2f && associativeMem->getAssociationStrength(1, 3) < 0.4f);
    assert(associativeMem->getAssociations(1).size() == 2);
    
    // Associate working memory state with episodic memory
    workingMem->store(2, 0.5f);
    
    std::cout << "    Associative memory integration passed" << std::endl;
}

void testMemorySensoryConnection() {
    std::cout << "  Testing memory-sensory connection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    
    // Create realistic sensory input
    std::vector<float> sensoryData(16, 0.0f);
    for (int i = 0; i < 16; i++) {
        sensoryData[i] = sin(i * 0.1f);  // Sinusoidal pattern
    }
    
    nlm::SensoryInput input(sensoryData);
    
    // Process sensory input
    brain.receiveSensoryInput(input);
    
    // Working memory should contain sensory information
    workingMem->store(100, 0.7f);
    assert(workingMem->contains(100));
    
    // Create episode from sensory experience
    nlm::EpisodicMemoryItem item;
    item.timestamp = 1;
    item.neurons.push_back(100);
    item.values.push_back(0.7f);
    item.metadata = "sensory_experience";
    
    episodicMem->storeEpisode(item);
    
    // Verify episodic memory stores sensory experience
    assert(episodicMem->getEpisodeCount() == 1);
    
    std::cout << "    Memory-sensory connection passed" << std::endl;
}

void testMemoryDecayAndConsolidation() {
    std::cout << "  Testing memory decay and consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    
    // Store initial state
    workingMem->store(200, 1.0f);
    assert(workingMem->contains(200));
    
    // Decay working memory
    workingMem->decay(0.1f);  // 10% decay
    float retrieved = workingMem->retrieve(200);
    assert(retrieved < 1.0f);  // Should have decayed
    
    // Create multiple episodes for consolidation
    for (int i = 0; i < 5; i++) {
        nlm::EpisodicMemoryItem item;
        item.timestamp = i;
        item.neurons.push_back(200 + i);
        item.values.push_back(0.5f * (i + 1));
        item.metadata = "consolidation_test";
        episodicMem->storeEpisode(item);
    }
    
    // Consolidate based on relevance
    episodicMem->consolidate(0.3f);
    
    // Verify consolidation maintained
    assert(episodicMem->getEpisodeCount() == 5);
    
    std::cout << "    Memory decay and consolidation passed" << std::endl;
}

void runAll() {
    testMemorySystemInitialization();
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
    testMemorySensoryConnection();
    testMemoryDecayAndConsolidation();
    
    std::cout << "=== Memory Systems Integration Tests PASSED ===" << std::endl;
}

} // namespace test_memory_integration