// Memory System Integration Tests
// Tests integration of Working Memory, Episodic Memory, and Associative Memory

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_memory_integration {

void testWorkingMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory exists
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Test basic properties
    assert(wm->getCapacity() > 0);
    assert(wm->getCurrentSize() == 0); // Initially empty
    
    std::cout << "    testWorkingMemoryInitialization passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Create a simple pattern
    std::vector<float> pattern = {1.0f, 0.5f, 0.2f, 0.8f};
    
    // Store pattern
    wm->store(pattern);
    
    // Retrieve pattern
    std::vector<float> retrieved = wm->retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Check some values are preserved (with tolerance)
    for (size_t i = 0; i < std::min(pattern.size(), retrieved.size()); ++i) {
        assert(std::abs(retrieved[i] - pattern[i]) < 0.1f);
    }
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testEpisodicMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* em = brain.getEpisodicMemory();
    assert(em != nullptr);
    
    // Test basic properties
    assert(em->getEpisodeCount() == 0); // Initially empty
    assert(em->isReplayEnabled()); // Replay typically enabled
    
    std::cout << "    testEpisodicMemoryInitialization passed" << std::endl;
}

void testEpisodicMemoryStorageRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* em = brain.getEpisodicMemory();
    assert(em != nullptr);
    
    // Create an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.5f, 0.7f, 0.3f};
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.5f;
    episode.action = nlm::ActionType::Move;
    episode.reward = 1.0f;
    episode.energy = 0.8f;
    episode.novelty = 0.6f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    episode.neuronActivations = {0.7f, 0.5f};
    
    // Store episode
    em->storeEpisode(episode);
    
    // Check episode was stored
    assert(em->getEpisodeCount() == 1);
    
    // Retrieve episode
    const nlm::EpisodicMemoryItem* retrieved = em->getEpisode(0);
    assert(retrieved != nullptr);
    assert(retrieved->timestamp == 100);
    assert(retrieved->reward == 1.0f);
    
    // Test retrieval methods
    std::vector<const nlm::EpisodicMemoryItem*> similar = 
        em->retrieveSimilar(episode.sensoryState, 3);
    assert(!similar.empty());
    
    std::cout << "    testEpisodicMemoryStorageRetrieval passed" << std::endl;
}

void testAssociativeMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* am = brain.getAssociativeMemory();
    assert(am != nullptr);
    
    // Test basic properties
    assert(am->getAssociationCount() == 0); // Initially empty
    
    std::cout << "    testAssociativeMemoryInitialization passed" << std::endl;
}

void testAssociativeMemoryCreationAndRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* am = brain.getAssociativeMemory();
    assert(am != nullptr);
    
    // Create two patterns
    std::vector<float> patternA = {1.0f, 0.5f, 0.2f};
    std::vector<float> patternB = {0.3f, 0.8f, 0.7f};
    
    // Create association
    am->associate(patternA, patternB, 0.8f);
    
    // Check association exists
    float strength = am->getAssociationStrength(patternA, patternB);
    assert(strength > 0.7f); // Should be close to 0.8f
    
    // Test retrieval
    std::vector<std::vector<float>> retrieved = am->retrieve(patternA, 3);
    assert(!retrieved.empty());
    
    // Test spread activation
    std::vector<std::vector<float>> spread = am->spreadActivation(patternA, 2);
    assert(!spread.empty());
    
    std::cout << "    testAssociativeMemoryCreationAndRetrieval passed" << std::endl;
}

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Store some information in working memory
    std::vector<float> pattern = {0.5f, 0.8f, 0.3f, 0.6f, 0.9f};
    brain.getWorkingMemory()->store(pattern);
    
    // Create an episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = pattern;
    episode.action = nlm::ActionType::Wait;
    episode.reward = 0.5f;
    episode.novelty = 0.3f;
    
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Create an associative memory link
    std::vector<float> similarPattern = {0.4f, 0.7f, 0.2f, 0.5f, 0.8f};
    brain.getAssociativeMemory()->associate(pattern, similarPattern, 0.7f);
    
    // Verify all systems are integrated
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    assert(brain.getAssociativeMemory()->getAssociationCount() > 0);
    
    // Run simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify memory systems persist and interact
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory System Integration tests..." << std::endl;
    testWorkingMemoryInitialization();
    testWorkingMemoryStoreRetrieve();
    testEpisodicMemoryInitialization();
    testEpisodicMemoryStorageRetrieval();
    testAssociativeMemoryInitialization();
    testAssociativeMemoryCreationAndRetrieval();
    testMemorySystemIntegration();
}

} // namespace test_memory_integration
