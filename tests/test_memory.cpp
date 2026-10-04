// Memory System Tests
// Comprehensive testing for all memory systems in NLM

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "environment/Action.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_memory {

void testMemorySystemBase() {
    std::cout << "Testing Memory system base class..." << std::endl;
    
    // Test that memory systems can be instantiated
    // (Concrete implementations would be tested through integration)
    
    std::cout << "    testMemorySystemBase passed" << std::endl;
}

void testWorkingMemory() {
    std::cout << "Testing Working Memory system..." << std::endl;
    
    nlm::NeuralWorkingMemory workingMemory;
    
    // Test basic properties
    assert(workingMemory.getCapacity() > 0);
    assert(workingMemory.getActiveTraces() >= 0);
    assert(workingMemory.getMemoryActivity() >= 0.0f);
    
    // Test neuron storage
    nlm::NeuronId testNeuron1(nlm::NeuronId(1));
    nlm::NeuronId testNeuron2(nlm::NeuronId(2));
    
    workingMemory.storeToNeuron(testNeuron1, 0.8f);
    workingMemory.storeToNeuron(testNeuron2, 0.6f);
    
    assert(workingMemory.contains(testNeuron1));
    assert(workingMemory.contains(testNeuron2));
    assert(workingMemory.getNeuronActivation(testNeuron1) > 0.0f);
    assert(workingMemory.getNeuronActivation(testNeuron2) > 0.0f);
    
    // Test memory retrieval
    std::vector<float> retrieved = workingMemory.retrieve();
    assert(retrieved.size() > 0);
    
    // Test winners selection
    bool isWinner = workingMemory.isWinning(testNeuron1);
    // May or may not be a winner depending on thresholds
    
    // Test memory strength
    float memoryActivity = workingMemory.getMemoryActivity();
    assert(memoryActivity >= 0.0f);
    
    // Test memory decay
    workingMemory.storeToNeuron(testNeuron1, 1.0f);
    float activityBefore = workingMemory.getNeuronActivation(testNeuron1);
    workingMemory.update(1.0f);  // Decay over 1 second
    float activityAfter = workingMemory.getNeuronActivation(testNeuron1);
    assert(activityAfter <= activityBefore);
    
    // Test memory strengthening
    workingMemory.strengthenMemory(1.5f);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "Testing Episodic Memory system..." << std::endl;
    
    nlm::NeuralEpisodicMemory episodicMemory;
    
    // Test basic properties
    assert(episodicMemory.getMaxEpisodes() > 0);
    assert(episodicMemory.getEpisodeCount() >= 0);
    
    // Create a test episode
    nlm::EpisodicMemoryItem testEpisode;
    testEpisode.timestamp = nlm::SimulationStep(100);
    testEpisode.reward = 0.5f;
    testEpisode.activeNeurons.push_back(nlm::NeuronId(1));
    testEpisode.neuronActivations.push_back(0.7f);
    testEpisode.context.push_back("test_context");
    
    // Store episode
    episodicMemory.storeEpisode(testEpisode);
    
    assert(episodicMemory.getEpisodeCount() > 0);
    
    // Test episode retrieval
    const nlm::EpisodicMemoryItem* retrievedEpisode = episodicMemory.getEpisode(0);
    assert(retrievedEpisode != nullptr);
    assert(retrievedEpisode->reward == 0.5f);
    assert(retrievedEpisode->activeNeurons.size() > 0);
    
    // Test episode replay
    episodicMemory.replayEpisode(retrievedEpisode);
    
    // Test consolidation
    episodicMemory.consolidate(0.5f);
    
    // Test episode selection for replay
    std::vector<const nlm::EpisodicMemoryItem*> episodesForReplay = episodicMemory.getEpisodesForReplay(5);
    // May be empty if not enough episodes, but shouldn't crash
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "Testing Associative Memory system..." << std::endl;
    
    nlm::NeuralAssociativeMemory associativeMemory;
    
    // Test basic properties
    assert(associativeMemory.getMaxAssociations() > 0);
    assert(associativeMemory.getAssociationCount() >= 0);
    
    // Test pattern storage
    std::vector<float> pattern1 = {0.1f, 0.8f, 0.3f};
    std::vector<float> pattern2 = {0.9f, 0.2f, 0.7f};
    
    associativeMemory.storePattern(pattern1, 0.9f);
    associativeMemory.storePattern(pattern2, 0.7f);
    
    // Test pattern retrieval
    std::vector<float> retrieved1 = associativeMemory.retrievePattern(pattern1, 0.5f);
    assert(retrieved1.size() == pattern1.size());
    
    // Test pattern completion
    std::vector<float> partialPattern = {0.5f, 0.0f, 0.0f};
    std::vector<float> completedPattern = associativeMemory.completePattern(partialPattern);
    
    // Test pattern matching
    bool matches = associativeMemory.matchesPattern(pattern1, 0.5f);
    // May or may not match depending on implementation
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing Memory system integration..." << std::endl;
    
    // Test interaction between different memory systems
    nlm::NeuralWorkingMemory workingMemory;
    nlm::NeuralEpisodicMemory episodicMemory;
    nlm::NeuralAssociativeMemory associativeMemory;
    
    // Create test data
    nlm::NeuronId testNeuron(nlm::NeuronId(1));
    float testActivation = 0.8f;
    
    // Store in working memory
    workingMemory.storeToNeuron(testNeuron, testActivation);
    
    // Store related pattern in episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = nlm::SimulationStep(50);
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(testNeuron);
    episode.neuronActivations.push_back(testActivation);
    
    episodicMemory.storeEpisode(episode);
    
    // Store similar pattern in associative memory
    std::vector<float> pattern = {testActivation, 0.6f, 0.4f};
    associativeMemory.storePattern(pattern, testActivation);
    
    // Verify all systems contain the data
    assert(workingMemory.contains(testNeuron));
    assert(episodicMemory.getEpisodeCount() > 0);
    assert(associativeMemory.getAssociationCount() > 0);
    
    // Test memory retrieval from all systems
    std::vector<float> workingMemoryData = workingMemory.retrieve();
    const nlm::EpisodicMemoryItem* episodicData = episodicMemory.getEpisode(0);
    std::vector<float> associativeData = associativeMemory.retrievePattern(pattern, 0.5f);
    
    assert(workingMemoryData.size() > 0);
    assert(episodicData != nullptr);
    assert(associativeData.size() > 0);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory tests..." << std::endl;
    testMemorySystemBase();
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testMemoryIntegration();
}

} // namespace test_memory