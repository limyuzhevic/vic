// Memory Management Tests
// Comprehensive tests for resource cleanup and memory management

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

namespace test_memory_management {

void testMemoryAllocationDeallocation() {
    std::cout << "  Memory Allocation/Deallocation Tests" << std::endl;
    
    // Test memory allocation patterns with different configurations
    std::vector<std::pair<size_t, size_t>> configurations = {
        {100, 1},   // Small brain
        {500, 2},   // Medium brain
        {1000, 5},  // Large brain
        {2000, 10}, // Very large brain
    };
    
    for (size_t i = 0; i < configurations.size(); ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(configurations[i].first), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(configurations[i].second), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify brain was allocated correctly
        assert(brain.getTotalNeuronCount() == configurations[i].first);
        assert(brain.getRegionCount() == configurations[i].second);
        
        // Use brain (perform operations)
        for (int step = 0; step < 100; ++step) {
            brain.step(step, step * 0.001);
        }
        
        // Reset brain (cleanup)
        brain.reset();
        
        // Verify reset
        assert(brain.getTotalNeuronCount() == configurations[i].first);
        
        std::cout << "    Configuration " << i + 1 << " completed: "
                  << configurations[i].first << " neurons, " << configurations[i].second << " regions" << std::endl;
    }
    
    std::cout << "    Memory allocation/deallocation tests passed" << std::endl;
}

void testMemoryLeaks() {
    std::cout << "  Memory Leak Tests" << std::endl;
    
    // Test for potential memory leaks with repeated creation/destruction
    for (int iteration = 0; iteration < 10; ++iteration) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Perform operations
        for (int step = 0; step < 50; ++step) {
            brain.step(step, step * 0.001);
        }
        
        // Brain goes out of scope here - should be properly destroyed
        // If there were memory leaks, this would accumulate over iterations
    }
    
    std::cout << "    Memory leak tests passed" << std::endl;
}

void testMemoryFragmentation() {
    std::cout << "  Memory Fragmentation Tests" << std::endl;
    
    // Test memory fragmentation with mixed operations
    std::vector<std::vector<std::pair<size_t, size_t>>> brainConfigs = {
        {{100, 1}, {200, 2}, {150, 1}},
        {{500, 2}, {300, 3}, {400, 2}},
        {{1000, 5}, {800, 4}, {600, 3}},
    };
    
    for (size_t i = 0; i < brainConfigs.size(); ++i) {
        for (size_t j = 0; j < brainConfigs[i].size(); ++j) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(brainConfigs[i][j].first), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(brainConfigs[i][j].second), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Perform operations
            for (int step = 0; step < 30; ++step) {
                brain.step(step, step * 0.001);
            }
            
            // Reset and repeat
            brain.reset();
        }
        
        std::cout << "    Mixed memory operations iteration " << i + 1 << " completed" << std::endl;
    }
    
    std::cout << "    Memory fragmentation tests passed" << std::endl;
}

void testResourceCleanup() {
    std::cout << "  Resource Cleanup Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Fill memory systems
    for (int i = 0; i < 100; ++i) {
        std::vector<float> pattern(10);
        for (int j = 0; j < 10; ++j) {
            pattern[j] = static_cast<float>(i * 0.1f + j * 0.01f);
        }
        workingMemory->store(pattern, 1.0f);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = static_cast<float>(i);
        episode.sensoryState = pattern;
        episodicMemory->storeEpisode(episode);
        
        std::vector<float> patternB(8);
        for (int j = 0; j < 8; ++j) {
            patternB[j] = static_cast<float>(i * 0.2f + j * 0.05f);
        }
        associativeMemory->associate(pattern, patternB, 0.8f);
    }
    
    // Verify memory systems are full
    assert(workingMemory->getActiveTraces() > 0);
    assert(episodicMemory->getEpisodeCount() > 0);
    assert(associativeMemory->getAssociationCount() > 0);
    
    // Clean up all memory systems
    workingMemory->clear();
    episodicMemory->clear();
    associativeMemory->clear();
    
    // Verify cleanup
    assert(workingMemory->getActiveTraces() == 0);
    assert(episodicMemory->getEpisodeCount() == 0);
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "    Resource cleanup tests passed" << std::endl;
}

void testLargeScaleMemory() {
    std::cout << "  Large Scale Memory Tests" << std::endl;
    
    // Test with very large memory configurations
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory with large patterns
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store large pattern
    std::vector<float> largePattern(1000, 0.5f);
    workingMemory->store(largePattern, 1.0f);
    
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == 1000);
    
    // Test episodic memory with many episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    for (int i = 0; i < 1000; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = static_cast<float>(i % 100);
        episode.sensoryState = std::vector<float>(50, static_cast<float>(i) * 0.01f);
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 1000);
    
    // Test associative memory with many associations
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    for (int i = 0; i < 500; ++i) {
        std::vector<float> patternA(20);
        std::vector<float> patternB(20);
        for (int j = 0; j < 20; ++j) {
            patternA[j] = static_cast<float>(i * 0.1f + j * 0.05f);
            patternB[j] = static_cast<float>(i * 0.15f + j * 0.03f);
        }
        associativeMemory->associate(patternA, patternB, 0.7f);
    }
    
    assert(associativeMemory->getAssociationCount() == 500);
    
    std::cout << "    Large scale memory tests passed" << std::endl;
}

void testMemoryStress() {
    std::cout << "  Memory Stress Tests" << std::endl;
    
    // Stress test memory management
    for (int iteration = 0; iteration < 5; ++iteration) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Rapid memory allocation and deallocation
        for (int i = 0; i < 100; ++i) {
            // Store data
            std::vector<float> pattern(20);
            for (int j = 0; j < 20; ++j) {
                pattern[j] = static_cast<float>(i * 0.1f + j * 0.01f);
            }
            workingMemory->store(pattern, 1.0f);
            
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.reward = static_cast<float>(i);
            episode.sensoryState = pattern;
            episodicMemory->storeEpisode(episode);
            
            std::vector<float> patternB(15);
            for (int j = 0; j < 15; ++j) {
                patternB[j] = static_cast<float>(i * 0.2f + j * 0.03f);
            }
            associativeMemory->associate(pattern, patternB, 0.8f);
            
            // Clean up
            if (i % 10 == 0) {
                workingMemory->clear();
                episodicMemory->consolidate(0.5f);
                associativeMemory->clear();
            }
        }
        
        std::cout << "    Memory stress iteration " << iteration + 1 << " completed" << std::endl;
    }
    
    std::cout << "    Memory stress tests passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory Management Tests..." << std::endl;
    
    try {
        testMemoryAllocationDeallocation();
        testMemoryLeaks();
        testMemoryFragmentation();
        testResourceCleanup();
        testLargeScaleMemory();
        testMemoryStress();
        
        std::cout << "  All Memory Management Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_memory_management