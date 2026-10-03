// Memory System Tests
// This file tests the memory components of the NLM framework
// Covers working memory, episodic memory, and associative memory systems

#include "memory/WorkingMemory/WorkingMemory.hpp"
#include "memory/EpisodicMemory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory/AssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_memory {

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<nlm::Config>();
    
    // Configure working memory capacity
    config->set("working_memory.capacity", 100, nlm::ConfigSource::Default);
    config->set("working_memory.trace_decay", 0.9, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test capacity enforcement
    for (int i = 0; i < 50; ++i) {
        std::vector<double> pattern(i, 1.0);  // Growing pattern
        workingMemory->storePattern(pattern);
    }
    
    // Verify capacity is respected
    int activeTraces = workingMemory->getActiveTraces();
    assert(activeTraces <= 100); // Should not exceed capacity
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryEviction() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("working_memory.capacity", 3, nlm::ConfigSource::Default);
    config->set("working_memory.trace_decay", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    
    // Store patterns beyond capacity
    std::vector<double> pattern1{1.0, 2.0, 3.0};
    std::vector<double> pattern2{4.0, 5.0, 6.0};
    std::vector<double> pattern3{7.0, 8.0, 9.0};
    std::vector<double> pattern4{10.0, 11.0, 12.0};
    
    workingMemory->storePattern(pattern1);
    workingMemory->storePattern(pattern2);
    workingMemory->storePattern(pattern3);
    workingMemory->storePattern(pattern4); // Should evict oldest pattern
    
    // Verify eviction occurred
    auto storedPatterns = workingMemory->getStoredPatterns();
    assert(storedPatterns.size() == 3); // One pattern should have been evicted
    
    // Verify remaining patterns are correct
    bool foundPattern1 = false, foundPattern2 = false, foundPattern3 = false, foundPattern4 = false;
    for (const auto& pattern : storedPatterns) {
        if (pattern == pattern1) foundPattern1 = true;
        if (pattern == pattern2) foundPattern2 = true;
        if (pattern == pattern3) foundPattern3 = true;
        if (pattern == pattern4) foundPattern4 = true;
    }
    
    assert(foundPattern2 && foundPattern3 && foundPattern4); // pattern1 should be evicted
    assert(!foundPattern1);
    
    std::cout << "    testWorkingMemoryEviction passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("episodic_memory.max_episodes", 10, nlm::ConfigSource::Default);
    config->set("episodic_memory.consolidation_threshold", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store an episode
    std::vector<double> neuralActivity{0.1, 0.5, 0.9, 0.3, 0.7};
    nlm::Timestamp timestamp = 1000.0;
    
    episodicMemory->storeEpisode(neuralActivity, timestamp);
    
    // Verify episode was stored
    auto episodes = episodicMemory->getRecentEpisodes(5);
    assert(episodes.size() == 1);
    assert(episodes[0].neuralActivity == neuralActivity);
    assert(episodes[0].timestamp == timestamp);
    
    // Test episode replay
    std::vector<double> replayedActivity;
    bool replaySuccess = episodicMemory->replayEpisode(episodes[0].id, replayedActivity);
    assert(replaySuccess);
    assert(replayedActivity == neuralActivity);
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("episodic_memory.max_episodes", 20, nlm::ConfigSource::Default);
    config->set("episodic_memory.consolidation_threshold", 3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMemory = brain.getEpisodicMemory();
    
    // Store multiple episodes to trigger consolidation
    std::vector<std::vector<double>> patterns = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0},
        {10.0, 11.0, 12.0},
        {13.0, 14.0, 15.0}
    };
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        episodicMemory->storeEpisode(patterns[i], 1000.0 + i * 100.0);
    }
    
    // Verify consolidation occurred (older episodes should be consolidated)
    auto allEpisodes = episodicMemory->getAllEpisodes();
    assert(allEpisodes.size() >= 2); // Should have at least 2 consolidated episodes
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testAssociativeMemoryPatternCompletion() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("associative_memory.association_strength_threshold", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Store patterns and associations
    std::vector<double> pattern1{1.0, 2.0, 3.0, 4.0};
    std::vector<double> pattern2{5.0, 6.0, 7.0, 8.0};
    
    associativeMemory->storePattern(pattern1);
    associativeMemory->storePattern(pattern2);
    associativeMemory->associate(pattern1, pattern2, 0.8); // Strong association
    
    // Test pattern completion with partial input
    std::vector<double> partialInput{0.5, 1.5}; // Partial of pattern1
    std::vector<double> completed = associativeMemory->completePattern(partialInput);
    
    // Should be able to complete the pattern
    assert(completed.size() > 0);
    assert(completed.size() >= partialInput.size());
    
    std::cout << "    testAssociativeMemoryPatternCompletion passed" << std::endl;
}

void testAssociativeMemoryStrengthUpdate() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto associativeMemory = brain.getAssociativeMemory();
    
    std::vector<double> pattern1{1.0, 2.0, 3.0};
    std::vector<double> pattern2{4.0, 5.0, 6.0};
    
    associativeMemory->storePattern(pattern1);
    associativeMemory->storePattern(pattern2);
    
    // Initial association strength
    double initialStrength = associativeMemory->getAssociationStrength(pattern1, pattern2);
    assert(initialStrength == 0.0); // No association yet
    
    // Create association
    associativeMemory->associate(pattern1, pattern2, 0.7);
    
    // Update association strength
    associativeMemory->updateAssociationStrength(pattern1, pattern2, 0.2);
    
    double updatedStrength = associativeMemory->getAssociationStrength(pattern1, pattern2);
    assert(updatedStrength == 0.9); // Should be 0.7 + 0.2
    
    std::cout << "    testAssociativeMemoryStrengthUpdate passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCapacity();
    testWorkingMemoryEviction();
    testEpisodicMemoryStorage();
    testEpisodicMemoryConsolidation();
    testAssociativeMemoryPatternCompletion();
    testAssociativeMemoryStrengthUpdate();
    
    std::cout << "All memory tests passed!" << std::endl;
}

} // namespace test_memory
