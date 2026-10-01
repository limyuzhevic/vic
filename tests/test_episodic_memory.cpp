// Episodic Memory Tests
// Tests for the improved episodic memory with bounds checking

#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_episodic_memory {
    
    void testEpisodicMemoryStoreRetrieve() {
        nlm::EpisodicMemory memory;
        
        // Create a test episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1000.0;
        episode.totalReward = 1.5f;
        episode.stepCount = 100;
        episode.neuronActivity = std::vector<float>{0.1f, 0.5f, 0.9f};
        
        // Store and retrieve
        memory.storeEpisode(episode);
        
        nlm::EpisodicMemoryItem retrieved = memory.retrieveEpisode(0);
        assert(retrieved.timestamp == episode.timestamp);
        assert(retrieved.totalReward == episode.totalReward);
        assert(retrieved.stepCount == episode.stepCount);
        assert(retrieved.neuronActivity.size() == episode.neuronActivity.size());
        
        std::cout << "    testEpisodicMemoryStoreRetrieve passed" << std::endl;
    }
    
    void testEpisodicMemoryMultipleEpisodes() {
        nlm::EpisodicMemory memory;
        
        std::vector<nlm::EpisodicMemoryItem> episodes;
        for (int i = 0; i < 10; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i * 100.0;
            episode.totalReward = i * 0.1f;
            episode.stepCount = i * 10;
            episode.neuronActivity = std::vector<float>{0.1f * i, 0.5f * i, 0.9f * i};
            memory.storeEpisode(episode);
            episodes.push_back(episode);
        }
        
        assert(memory.getEpisodeCount() == 10);
        
        // Retrieve last episode
        nlm::EpisodicMemoryItem last = memory.retrieveEpisode(9);
        assert(last.timestamp == episodes[9].timestamp);
        assert(last.totalReward == episodes[9].totalReward);
        
        std::cout << "    testEpisodicMemoryMultipleEpisodes passed" << std::endl;
    }
    
    void testEpisodicMemoryRecentEpisodes() {
        nlm::EpisodicMemory memory;
        
        // Store 20 episodes
        for (int i = 0; i < 20; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            memory.storeEpisode(episode);
        }
        
        // Get recent 5 episodes
        std::vector<nlm::EpisodicMemoryItem> recent = memory.getRecentEpisodes(5);
        assert(recent.size() == 5);
        assert(recent[0].timestamp == 19); // Most recent
        assert(recent[4].timestamp == 15); // 5th most recent
        
        // Get more episodes than available (should return all)
        std::vector<nlm::EpisodicMemoryItem> allRecent = memory.getRecentEpisodes(25);
        assert(allRecent.size() == 20);
        
        std::cout << "    testEpisodicMemoryRecentEpisodes passed" << std::endl;
    }
    
    void testEpisodicMemoryIndexValidation() {
        nlm::EpisodicMemory memory;
        
        nlm::EpisodicMemoryItem episode;
        memory.storeEpisode(episode);
        
        // Valid index
        nlm::EpisodicMemoryItem retrieved = memory.retrieveEpisode(0);
        assert(retrieved.timestamp == 0.0);
        
        // Invalid index should throw
        bool exceptionThrown = false;
        try {
            memory.retrieveEpisode(1);
        } catch (const std::out_of_range& e) {
            exceptionThrown = true;
            assert(std::string(e.what()).find("index out of range") != std::string::npos);
        }
        assert(exceptionThrown);
        
        std::cout << "    testEpisodicMemoryIndexValidation passed" << std::endl;
    }
    
    void testEpisodicMemoryCountValidation() {
        nlm::EpisodicMemory memory;
        
        bool exceptionThrown = false;
        try {
            std::vector<nlm::EpisodicMemoryItem> recent = memory.getRecentEpisodes(10);
        } catch (const std::invalid_argument& e) {
            exceptionThrown = true;
            assert(std::string(e.what()).find("exceeds episode count") != std::string::npos);
        }
        assert(exceptionThrown);
        
        std::cout << "    testEpisodicMemoryCountValidation passed" << std::endl;
    }
    
    void testEpisodicMemoryClear() {
        nlm::EpisodicMemory memory;
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1000.0;
        memory.storeEpisode(episode);
        
        assert(memory.getEpisodeCount() == 1);
        
        memory.clear();
        assert(memory.getEpisodeCount() == 0);
        
        std::cout << "    testEpisodicMemoryClear passed" << std::endl;
    }
    
    void runAll() {
        std::cout << "Running Episodic Memory tests..." << std::endl;
        testEpisodicMemoryStoreRetrieve();
        testEpisodicMemoryMultipleEpisodes();
        testEpisodicMemoryRecentEpisodes();
        testEpisodicMemoryIndexValidation();
        testEpisodicMemoryCountValidation();
        testEpisodicMemoryClear();
    }

} // namespace test_episodic_memory
