// Test file for Enhanced Memory Systems
// Validates the integrated memory system functionality

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace nlm;

void testNeuralWorkingMemory() {
    std::cout << "Testing NeuralWorkingMemory..." << std::endl;
    
    // Create memory instance
    NeuralWorkingMemory workingMemory;
    
    // Test capacity setting
    workingMemory.setCapacity(100);
    assert(workingMemory.getCapacity() == 100);
    
    // Test decay rate setting
    workingMemory.setDecayRate(0.02f);
    assert(workingMemory.getDecayRate() == 0.02f);
    
    // Test neuron activation storage
    NeuronId testNeuron(1000);
    workingMemory.storeToNeuron(testNeuron, 0.8f);
    assert(workingMemory.contains(testNeuron));
    assert(workingMemory.getNeuronActivation(testNeuron) == 0.8f);
    
    // Test memory retrieval
    std::vector<float> retrieved = workingMemory.retrieve();
    assert(!retrieved.empty());
    assert(retrieved[0] == 0.8f);
    
    // Test competition mechanism
    workingMemory.runCompetition();
    // Winners should be identified
    
    // Test strengthening
    workingMemory.strengthenMemory(1.5f);
    assert(workingMemory.getNeuronActivation(testNeuron) > 0.8f);
    
    // Test clear
    workingMemory.clear();
    assert(!workingMemory.contains(testNeuron));
    assert(workingMemory.getActiveTraces() == 0);
    
    std::cout << "✓ NeuralWorkingMemory tests passed" << std::endl;
}

void testNeuralEpisodicMemory() {
    std::cout << "Testing NeuralEpisodicMemory..." << std::endl;
    
    NeuralEpisodicMemory episodicMemory;
    
    // Test capacity setting
    episodicMemory.setMaxEpisodes(500);
    // Note: This would require a getter method
    
    // Test episode storage
    EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.5f;
    episode.activeNeurons = {NeuronId(1), NeuronId(2), NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.9f};
    
    episodicMemory.storeEpisode(episode);
    
    // Test episode retrieval
    auto episodes = episodicMemory.getEpisodes();
    assert(!episodes.empty());
    assert(episodes.size() == 1);
    
    // Test consolidation
    episodicMemory.consolidate(0.3f);
    
    // Test replay
    auto replayEpisodes = episodicMemory.getEpisodesForReplay(1);
    assert(!replayEpisodes.empty());
    
    episodicMemory.clear();
    assert(episodicMemory.getEpisodeCount() == 0);
    
    std::cout << "✓ NeuralEpisodicMemory tests passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing Memory Integration..." << std::endl;
    
    // Test that memory systems can be accessed through Brain
    // This requires a Brain instance with proper configuration
    // For now, we test the interface
    
    // Test working memory integration methods
    assert(std::is_same<decltype(&NeuralWorkingMemory::storeToNeuron), 
                        void (NeuralWorkingMemory::*)(NeuronId, float)>::value);
    
    // Test episodic memory integration methods
    assert(std::is_same<decltype(&NeuralEpisodicMemory::storeEpisode),
                        void (NeuralEpisodicMemory::*)(const EpisodicMemoryItem&)>::value);
    
    // Test attentional selection integration
    // This would test the AttentionalSelection class
    
    std::cout << "✓ Memory Integration tests passed" << std::endl;
}

void testMemoryStatistics() {
    std::cout << "Testing Memory Statistics..." << std::endl;
    
    NeuralWorkingMemory workingMemory;
    workingMemory.setCapacity(50);
    
    // Add some test neurons
    std::vector<NeuronId> testNeurons;
    for (int i = 0; i < 10; ++i) {
        testNeurons.emplace_back(NeuronId(1000 + i));
        workingMemory.storeToNeuron(testNeurons[i], 0.5f + (i * 0.05f));
    }
    
    // Test statistics
    assert(workingMemory.getActiveTraces() == 10);
    assert(workingMemory.getMemoryActivity() > 0.0f);
    
    // Test competition winners
    workingMemory.runCompetition();
    auto winners = workingMemory.getWinners();
    assert(!winners.empty());
    
    std::cout << "✓ Memory Statistics tests passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Memory Systems Test Suite ===" << std::endl;
    
    try {
        testNeuralWorkingMemory();
        testNeuralEpisodicMemory();
        testMemoryIntegration();
        testMemoryStatistics();
        
        std::cout << std::endl;
        std::cout << "=== All memory system tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}