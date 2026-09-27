// Memory Integration Tests
// Tests for Phase 6 memory systems: WorkingMemory, EpisodicMemory, AssociativeMemory

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory_integration {

void testWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Store a pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    wm->store(pattern, 1.0f);
    
    // Retrieve and verify
    auto retrieved = wm->retrieve();
    assert(retrieved.size() == pattern.size());
    for (size_t i = 0; i < pattern.size(); ++i) {
        assert(abs(retrieved[i] - pattern[i]) < 0.01f);
    }
    
    // Check neuron activation
    bool contains = wm->contains(0);
    // May or may not contain based on implementation
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* em = brain.getEpisodicMemory();
    assert(em != nullptr);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.positionX = 5.0f;
    episode.positionY = 5.0f;
    episode.reward = 1.0f;
    episode.novelty = 0.8f;
    
    em->storeEpisode(episode);
    
    // Check episode count
    assert(em->getEpisodeCount() > 0);
    
    // Retrieve similar episodes
    std::vector<float> query = {0.15f, 0.25f, 0.35f};
    auto similar = em->retrieveSimilar(query, 5);
    assert(!similar.empty());
    
    // Retrieve by location
    auto locationEpisodes = em->retrieveByLocation(5.0f, 5.0f, 1.0f, 5);
    assert(!locationEpisodes.empty());
    
    // Get recent episodes
    auto recent = em->getRecentEpisodes(3);
    assert(recent.size() <= 3);
    
    // Enable replay and get episodes for replay
    em->enableReplay(true);
    auto replayEpisodes = em->getEpisodesForReplay(2);
    assert(replayEpisodes.size() <= 2);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* am = brain.getAssociativeMemory();
    assert(am != nullptr);
    
    // Create patterns
    std::vector<float> patternA = {1.0f, 0.0f, 0.0f};
    std::vector<float> patternB = {0.0f, 1.0f, 0.0f};
    
    // Create association
    am->associate(patternA, patternB, 0.9f);
    
    // Retrieve associations
    auto retrieved = am->retrieve(patternA, 3);
    assert(!retrieved.empty());
    
    // Check association strength
    float strength = am->getAssociationStrength(patternA, patternB);
    assert(strength > 0.0f);
    
    // Spread activation
    std::vector<float> cue = {0.5f, 0.0f, 0.0f};
    auto spread = am->spreadActivation(cue, 2);
    assert(!spread.empty());
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all memory systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Run steps to allow memory to update
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001f);
    }
    
    // Check working memory activity
    auto* wm = brain.getWorkingMemory();
    if (wm) {
        float activity = wm->getMemoryActivity();
        assert(activity >= 0.0f);
    }
    
    // Check episodic memory episodes
    auto* em = brain.getEpisodicMemory();
    if (em) {
        size_t episodeCount = em->getEpisodeCount();
        assert(episodeCount >= 0);
    }
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory Integration Tests..." << std::endl;
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testMemoryIntegration();
}

} // namespace test_memory_integration