// Memory System Tests
// Tests for working memory, episodic memory, associative memory, etc.

#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_memory {

void testWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::WorkingMemory memory(config);
    
    // Test basic memory operations
    assert(memory.getActiveTraces() == 0);
    assert(memory.getMaxTraceCount() > 0);
    
    // Test adding traces
    memory.addTrace("test_trace", std::vector<float>{1.0f, 2.0f, 3.0f});
    assert(memory.getActiveTraces() == 1);
    
    // Test getting traces
    auto trace = memory.getTrace("test_trace");
    assert(trace.has_value());
    assert(trace.value().size() == 3);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::EpisodicMemory memory(config);
    
    // Test basic memory operations
    assert(memory.getEpisodeCount() == 0);
    assert(memory.getMaxEpisodes() > 0);
    
    // Test storing episode
    nlm::EpisodicEntry entry;
    entry.timestamp = 1.0f;
    entry.observation = std::vector<float>{0.5f, 0.8f, 0.3f};
    entry.action = std::vector<float>{1.0f, 0.0f};
    entry.reward = 1.0f;
    
    memory.storeEpisode(entry);
    assert(memory.getEpisodeCount() == 1);
    
    // Test replay
    auto episodes = memory.getEpisodesForReplay(1);
    assert(episodes.size() == 1);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::AssociativeMemory memory(config);
    
    // Test basic operations
    assert(memory.getAssociationCount() == 0);
    
    // Test adding association
    std::string pattern1 = "pattern1";
    std::string pattern2 = "pattern2";
    float strength = 0.8f;
    
    memory.addAssociation(pattern1, pattern2, strength);
    assert(memory.getAssociationCount() == 1);
    
    // Test getting strength
    float retrieved_strength = memory.getAssociationStrength(pattern1, pattern2);
    assert(std::abs(retrieved_strength - strength) < 0.01f);
    
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
}

} // namespace test_memory