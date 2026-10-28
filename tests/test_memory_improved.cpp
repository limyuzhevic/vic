// Test_Memory_Improved.cpp - Comprehensive tests for enhanced memory systems
#include "core/Memory/Memory_Improved.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_memory_improved {

void testWorkingMemoryBasic() {
    std::cout << "Testing Working Memory Basic..." << std::endl;
    
    nlm::WorkingMemory memory(10, nlm::WorkingMemory::EvictionPolicy::FIFO);
    
    // Store items
    memory.store(1, 0.5f, 0.8f);
    memory.store(2, 0.3f, 0.6f);
    memory.store(3, 0.7f, 1.0f);
    
    // Retrieve items
    auto val1 = memory.retrieve(1);
    auto val2 = memory.retrieve(2);
    auto val3 = memory.retrieve(3);
    
    assert(val1.has_value());
    assert(val2.has_value());
    assert(val3.has_value());
    assert(std::abs(val1.value() - 0.5f) < 0.001f);
    assert(std::abs(val2.value() - 0.3f) < 0.001f);
    assert(std::abs(val3.value() - 0.7f) < 0.001f);
    
    // Check contains
    assert(memory.contains(1));
    assert(memory.contains(2));
    assert(memory.contains(3));
    assert(!memory.contains(4));
    
    // Check capacity
    assert(memory.getCurrentSize() == 3);
    assert(memory.getCapacity() == 10);
    
    std::cout << "    ✓ WorkingMemoryBasic test passed" << std::endl;
}

void testWorkingMemoryEviction() {
    std::cout << "Testing Working Memory Eviction..." << std::endl;
    
    // Test FIFO eviction
    nlm::WorkingMemory memory(3, nlm::WorkingMemory::EvictionPolicy::FIFO);
    
    memory.store(1, 0.1f);
    memory.store(2, 0.2f);
    memory.store(3, 0.3f);
    
    assert(memory.getCurrentSize() == 3);
    
    // Add one more item to trigger eviction
    memory.store(4, 0.4f);
    
    // Should have evicted item 1 (oldest)
    assert(memory.contains(1) == false);
    assert(memory.contains(2));
    assert(memory.contains(3));
    assert(memory.contains(4));
    
    assert(memory.getCurrentSize() == 3);
    
    // Test LRU eviction
    nlm::WorkingMemory memory2(3, nlm::WorkingMemory::EvictionPolicy::LRU);
    
    memory2.store(1, 0.1f);
    memory2.store(2, 0.2f);
    memory2.store(3, 0.3f);
    
    // Access item 1 to make it recently used
    memory2.retrieve(1);
    
    // Add item 4 - should evict item 2 (least recently used)
    memory2.store(4, 0.4f);
    
    assert(memory2.contains(1));
    assert(memory2.contains(2) == false);  // Should be evicted
    assert(memory2.contains(3));
    assert(memory2.contains(4));
    
    // Test LFU eviction
    nlm::WorkingMemory memory3(3, nlm::WorkingMemory::EvictionPolicy::LFU);
    
    memory3.store(1, 0.1f);
    memory3.store(2, 0.2f);
    memory3.store(3, 0.3f);
    
    // Access item 1 multiple times to make it LFU
    memory3.retrieve(1);
    memory3.retrieve(1);
    memory3.retrieve(1);
    
    // Add item 4 - should evict item 2 (least frequently used)
    memory3.store(4, 0.4f);
    
    assert(memory3.contains(1));
    assert(memory3.contains(2) == false);  // Should be evicted
    assert(memory3.contains(3));
    assert(memory3.contains(4));
    
    std::cout << "    ✓ WorkingMemoryEviction test passed" << std::endl;
}

void testWorkingMemoryImportance() {
    std::cout << "Testing Working Memory Importance..." << std::endl;
    
    nlm::WorkingMemory memory(10, nlm::WorkingMemory::EvictionPolicy::HEBBIAN);
    
    // Store items with different importance
    memory.store(1, 0.5f, 0.2f);  // Low importance
    memory.store(2, 0.3f, 0.8f);  // High importance
    memory.store(3, 0.7f, 0.1f);  // Very low importance
    
    assert(std::abs(memory.getImportance(1) - 0.2f) < 0.001f);
    assert(std::abs(memory.getImportance(2) - 0.8f) < 0.001f);
    assert(std::abs(memory.getImportance(3) - 0.1f) < 0.001f);
    
    // Update importance
    memory.updateImportance(1, 0.9f);
    assert(std::abs(memory.getImportance(1) - 0.9f) < 0.001f);
    
    // Evict item 3 (lowest importance)
    memory.store(4, 0.4f);  // This should trigger eviction
    
    assert(memory.contains(1));
    assert(memory.contains(2));
    assert(memory.contains(3) == false);  // Should be evicted
    assert(memory.contains(4));
    
    std::cout << "    ✓ WorkingMemoryImportance test passed" << std::endl;
}

void testWorkingMemoryDecay() {
    std::cout << "Testing Working Memory Decay..." << std::endl;
    
    nlm::WorkingMemory memory(10, nlm::WorkingMemory::EvictionPolicy::LRU,
                            nlm::WorkingMemory::DecayModel::EXPONENTIAL, 0.1f);
    
    // Store item
    memory.store(1, 1.0f);
    float initialValue = memory.getValue(1);
    assert(std::abs(initialValue - 1.0f) < 0.001f);
    
    // Apply decay
    memory.applyDecay();
    float decayedValue = memory.getValue(1);
    
    // Value should decay (exponential decay)
    assert(decayedValue < initialValue);
    assert(decayedValue > 0.0f);
    
    // Test different decay rates
    nlm::WorkingMemory memory2(10, nlm::WorkingMemory::EvictionPolicy::LRU,
                             nlm::WorkingMemory::DecayModel::LINEAR, 0.5f);
    
    memory2.store(2, 1.0f);
    memory2.applyDecay();
    
    float linearDecayed = memory2.getValue(2);
    assert(std::abs(linearDecayed - 0.5f) < 0.001f);
    
    std::cout << "    ✓ WorkingMemoryDecay test passed" << std::endl;
}

void testEpisodicMemoryBasic() {
    std::cout << "Testing Episodic Memory Basic..." << std::endl;
    
    nlm::EpisodicMemory memory(100);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.description = "Test episode";
    episode.timestamp = std::chrono::system_clock::now();
    episode.coherence = 0.8f;
    episode.predictedOutcome = 0.5f;
    episode.actualOutcome = 0.6f;
    
    // Store episode
    memory.storeEpisode(episode);
    
    // Retrieve episode
    assert(memory.getEpisodeCount() == 1);
    
    std::cout << "    ✓ EpisodicMemoryBasic test passed" << std::endl;
}

void testEpisodicMemoryTemporal() {
    std::cout << "Testing Episodic Memory Temporal..." << std::endl;
    
    nlm::EpisodicMemory memory(100);
    
    // Create episodes with different timestamps
    auto now = std::chrono::system_clock::now();
    auto past = now - std::chrono::hours(1);
    auto future = now + std::chrono::hours(1);
    
    nlm::EpisodicMemoryItem episode1;
    episode1.description = "Past episode";
    episode1.timestamp = past;
    episode1.coherence = 0.6f;
    
    nlm::EpisodicMemoryItem episode2;
    episode2.description = "Current episode";
    episode2.timestamp = now;
    episode2.coherence = 0.8f;
    
    nlm::EpisodicMemoryItem episode3;
    episode3.description = "Future episode";
    episode3.timestamp = future;
    episode3.coherence = 0.9f;
    
    memory.storeEpisode(episode1);
    memory.storeEpisode(episode2);
    memory.storeEpisode(episode3);
    
    // Test temporal retrieval
    auto retrieved = memory.retrieveEpisodesByTime(past, now + std::chrono::minutes(1));
    assert(retrieved.size() == 2);  // episode1 and episode2
    
    std::cout << "    ✓ EpisodicMemoryTemporal test passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "Testing Memory System Integration..." << std::endl;
    
    nlm::MemorySystem memorySystem;
    
    // Test initialization
    assert(memorySystem.initialize());
    
    // Test access to subsystems
    auto workingMemory = memorySystem.getWorkingMemory();
    auto episodicMemory = memorySystem.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test basic operations
    workingMemory->store(1, 0.5f);
    episodicMemory->storeEpisode(nlm::EpisodicMemoryItem());
    
    // Test stats
    auto stats = memorySystem.getStats();
    assert(stats.totalMemoryFootprint > 0);
    
    std::cout << "    ✓ MemorySystemIntegration test passed" << std::endl;
}

void testMemoryConfiguration() {
    std::cout << "Testing Memory Configuration..." << std::endl;
    
    // Test configuration parsing
    assert(nlm::MemoryUtils::parseEvictionPolicy("FIFO") == 
           nlm::WorkingMemory::EvictionPolicy::FIFO);
    assert(nlm::MemoryUtils::parseEvictionPolicy("LRU") == 
           nlm::WorkingMemory::EvictionPolicy::LRU);
    assert(nlm::MemoryUtils::parseEvictionPolicy("LFU") == 
           nlm::WorkingMemory::EvictionPolicy::LFU);
    assert(nlm::MemoryUtils::parseEvictionPolicy("HEBBIAN") == 
           nlm::WorkingMemory::EvictionPolicy::HEBBIAN);
    assert(nlm::MemoryUtils::parseEvictionPolicy("ADAPTIVE") == 
           nlm::WorkingMemory::EvictionPolicy::ADAPTIVE);
    
    // Test decay model parsing
    assert(nlm::MemoryUtils::parseDecayModel("EXPONENTIAL") == 
           nlm::WorkingMemory::DecayModel::EXPONENTIAL);
    assert(nlm::MemoryUtils::parseDecayModel("LINEAR") == 
           nlm::WorkingMemory::DecayModel::LINEAR);
    assert(nlm::MemoryUtils::parseDecayModel("NONE") == 
           nlm::WorkingMemory::DecayModel::NONE);
    assert(nlm::MemoryUtils::parseDecayModel("CONTEXTUAL") == 
           nlm::WorkingMemory::DecayModel::CONTEXTUAL);
    
    // Test encoding strategy parsing
    assert(nlm::MemoryUtils::parseEncodingStrategy("SIMPLE") == 
           nlm::EpisodicMemory::EncodingStrategy::SIMPLE);
    assert(nlm::MemoryUtils::parseEncodingStrategy("CONNECTED") == 
           nlm::EpisodicMemory::EncodingStrategy::CONNECTED);
    assert(nlm::MemoryUtils::parseEncodingStrategy("HIERARCHICAL") == 
           nlm::EpisodicMemory::EncodingStrategy::HIERARCHICAL);
    assert(nlm::MemoryUtils::parseEncodingStrategy("PREDICTIVE") == 
           nlm::EpisodicMemory::EncodingStrategy::PREDICTIVE);
    assert(nlm::MemoryUtils::parseEncodingStrategy("AUTO") == 
           nlm::EpisodicMemory::EncodingStrategy::AUTO);
    
    std::cout << "    ✓ MemoryConfiguration test passed" << std::endl;
}

void testMemoryStatsFormatting() {
    std::cout << "Testing Memory Stats Formatting..." << std::endl;
    
    nlm::MemorySystem::SystemStats stats;
    stats.totalMemoryFootprint = 1024;
    stats.efficiencyRatio = 0.85f;
    
    // Test stats formatting
    std::string formatted = nlm::MemoryUtils::formatMemoryStats(stats);
    assert(!formatted.empty());
    assert(formatted.length() > 0);
    
    std::cout << "    ✓ MemoryStatsFormatting test passed" << std::endl;
}

void runAllMemoryTests() {
    std::cout << "=== Running Enhanced Memory System Tests ===" << std::endl;
    
    testWorkingMemoryBasic();
    testWorkingMemoryEviction();
    testWorkingMemoryImportance();
    testWorkingMemoryDecay();
    testEpisodicMemoryBasic();
    testEpisodicMemoryTemporal();
    testMemorySystemIntegration();
    testMemoryConfiguration();
    testMemoryStatsFormatting();
    
    std::cout << "=== All Enhanced Memory System Tests Passed ===" << std::endl;
}

} // namespace test_memory_improved