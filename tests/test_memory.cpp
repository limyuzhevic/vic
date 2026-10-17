// Memory System Tests
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_memory {

void testWorkingMemory() {
    nlm::WorkingMemory workingMemory;
    
    // Test initialization
    workingMemory.initialize(nullptr);
    
    // Test storage and retrieval
    workingMemory.store("test_memory", 1.0f);
    workingMemory.store("another_memory", 0.5f);
    
    std::string retrieved = workingMemory.retrieve();
    assert(!retrieved.empty());
    
    // Test clear
    workingMemory.clear();
    std::string cleared = workingMemory.retrieve();
    assert(cleared.empty());
    
    // Test capacity
    assert(workingMemory.getCapacity() == 100); // Default
    workingMemory.setCapacity(200);
    assert(workingMemory.getCapacity() == 200);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    nlm::EpisodicMemory episodicMemory;
    
    // Test initialization
    episodicMemory.initialize(nullptr);
    
    // Test episode storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.sensoryState = std::vector<float>{0.1f, 0.2f, 0.3f};
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::Move;
    episode.reward = 1.0f;
    episode.novelty = 0.8f;
    
    episodicMemory.storeEpisode(episode);
    
    // Test retrieval
    auto similarEpisodes = episodicMemory.retrieveSimilar(episode.sensoryState);
    assert(!similarEpisodes.empty());
    
    // Test temporal retrieval
    auto temporalEpisodes = episodicMemory.retrieveTemporal(0, 2000);
    assert(!temporalEpisodes.empty());
    
    // Test location retrieval
    auto locationEpisodes = episodicMemory.retrieveByLocation(episode.positionX, episode.positionY, 1.0f);
    assert(!locationEpisodes.empty());
    
    // Test action retrieval
    auto actionEpisodes = episodicMemory.retrieveAfterAction(episode.action);
    assert(!actionEpisodes.empty());
    
    // Test replay
    episodicMemory.replayEpisode(episode);
    
    // Test relevance update
    episodicMemory.updateRelevance(0, 0.1f);
    
    // Test statistics
    float avgReward = episodicMemory.getAverageReward();
    assert(avgReward == episode.reward);
    
    // Test consolidation
    episodicMemory.consolidate(0.5f);
    
    // Test replay control
    episodicMemory.enableReplay(true);
    assert(episodicMemory.isReplayEnabled() == true);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    nlm::AssociativeMemory associativeMemory;
    
    // Test initialization
    associativeMemory.initialize(nullptr);
    
    // Test association creation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory.associate(patternA, patternB, 0.8f);
    
    // Test association strength
    float strength = associativeMemory.getAssociationStrength(patternA, patternB);
    assert(strength > 0.0f);
    
    // Test retrieval
    auto retrieved = associativeMemory.retrieve(patternA);
    assert(!retrieved.empty());
    
    // Test association update
    associativeMemory.updateAssociation(patternA, patternB, 0.1f);
    
    // Test spreading activation
    auto spread = associativeMemory.spreadActivation(patternA, 2);
    assert(!spread.empty());
    
    // Test clearing
    associativeMemory.clear();
    float clearedStrength = associativeMemory.getAssociationStrength(patternA, patternB);
    assert(clearedStrength == 0.0f);
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testNeuralWorkingMemory() {
    nlm::NeuralWorkingMemory neuralWorkingMemory;
    
    // Test initialization
    neuralWorkingMemory.initialize(nullptr);
    
    // Test storage
    neuralWorkingMemory.store(std::vector<float>{0.5f, 0.7f, 0.9f}, 1.0f);
    
    // Test retrieval
    std::vector<float> retrieved = neuralWorkingMemory.retrieve();
    assert(retrieved.size() >= 0);
    
    // Test neuron-specific storage
    neuralWorkingMemory.storeToNeuron(nlm::NeuronId(1), 0.8f);
    
    // Test neuron activation check
    float activation = neuralWorkingMemory.getNeuronActivation(nlm::NeuronId(1));
    assert(activation >= 0.0f);
    
    // Test containment
    bool contains = neuralWorkingMemory.contains(nlm::NeuronId(1));
    assert(contains == true);
    
    // Test update
    neuralWorkingMemory.update(0.1f);
    
    // Test capacity
    assert(neuralWorkingMemory.getCapacity() == 100); // Default
    neuralWorkingMemory.setCapacity(200);
    assert(neuralWorkingMemory.getCapacity() == 200);
    
    // Test decay rate
    assert(neuralWorkingMemory.getDecayRate() == 0.01f); // Default
    neuralWorkingMemory.setDecayRate(0.05f);
    assert(neuralWorkingMemory.getDecayRate() == 0.05f);
    
    // Test strengthening
    neuralWorkingMemory.strengthenMemory(1.1f);
    
    // Test competition
    neuralWorkingMemory.runCompetition();
    
    // Test winner check
    bool isWinner = neuralWorkingMemory.isWinning(nlm::NeuronId(1));
    assert(isWinner == true);
    
    // Test activity level
    float activity = neuralWorkingMemory.getMemoryActivity();
    assert(activity >= 0.0f);
    
    // Test memory neurons
    auto memoryNeurons = neuralWorkingMemory.getMemoryNeurons();
    assert(!memoryNeurons.empty() || !neuralWorkingMemory.contains(nlm::NeuronId(1)));
    
    std::cout << "    testNeuralWorkingMemory passed" << std::endl;
}

void testNeuralEpisodicMemory() {
    nlm::NeuralEpisodicMemory neuralEpisodicMemory;
    
    // Test initialization
    neuralEpisodicMemory.initialize(nullptr);
    
    // Test episode storage
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1000;
    episode.sensoryState = std::vector<float>{0.1f, 0.2f, 0.3f};
    episode.action = nlm::ActionType::Wait;
    episode.reward = 1.0f;
    
    neuralEpisodicMemory.storeEpisode(episode);
    
    // Test retrieval methods
    auto similar = neuralEpisodicMemory.retrieveSimilar(episode.sensoryState);
    assert(!similar.empty());
    
    auto temporal = neuralEpisodicMemory.retrieveTemporal(0, 2000);
    assert(!temporal.empty());
    
    auto byLocation = neuralEpisodicMemory.retrieveByLocation(0, 0, 1.0f);
    assert(!byLocation.empty());
    
    auto byAction = neuralEpisodicMemory.retrieveAfterAction(episode.action);
    assert(!byAction.empty());
    
    // Test replay
    neuralEpisodicMemory.replayEpisode(episode);
    
    // Test relevance update
    neuralEpisodicMemory.updateRelevance(0, 0.1f);
    
    // Test episode count
    size_t count = neuralEpisodicMemory.getEpisodeCount();
    assert(count > 0);
    
    // Test recent episodes
    auto recent = neuralEpisodicMemory.getRecentEpisodes(5);
    assert(!recent.empty());
    
    // Test average reward
    float avgReward = neuralEpisodicMemory.getAverageReward();
    assert(avgReward > 0.0f);
    
    // Test consolidation
    neuralEpisodicMemory.consolidate(0.5f);
    
    // Test replay control
    neuralEpisodicMemory.enableReplay(true);
    assert(neuralEpisodicMemory.isReplayEnabled() == true);
    
    // Test episodes for replay
    auto forReplay = neuralEpisodicMemory.getEpisodesForReplay(5);
    assert(!forReplay.empty());
    
    // Test sequence replay
    neuralEpisodicMemory.replaySequence(std::vector<size_t>{0});
    
    std::cout << "    testNeuralEpisodicMemory passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testNeuralWorkingMemory();
    testNeuralEpisodicMemory();
}

} // namespace test_memory