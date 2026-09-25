// src/testing/unit/memory/MemoryUnitTests.cpp
// Unit tests for memory system

#include "Memory.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

class MemoryUnitTests {
public:
    static void testWorkingMemoryCreation() {
        std::cout << "Testing working memory creation..." << std::endl;
        
        auto workingMemory = std::make_shared<NeuralWorkingMemory>("test_memory");
        assert(workingMemory != nullptr);
        assert(workingMemory->getName() == "test_memory");
        assert(workingMemory->getCapacity() > 0);
        
        std::cout << "✓ Working memory creation test passed" << std::endl;
    }
    
    static void testWorkingMemoryStorage() {
        std::cout << "Testing working memory storage..." << std::endl;
        
        auto workingMemory = std::make_shared<NeuralWorkingMemory>("test_memory");
        
        // Store a value
        workingMemory->storeValue("test_key", 42);
        
        // Retrieve the value
        auto value = workingMemory->getValue("test_key");
        assert(value != nullptr);
        assert(*value == 42);
        
        std::cout << "✓ Working memory storage test passed" << std::endl;
    }
    
    static void testWorkingMemoryCapacity() {
        std::cout << "Testing working memory capacity..." << std::endl;
        
        auto workingMemory = std::make_shared<NeuralWorkingMemory>("test_memory", 100);
        
        // Store multiple values
        for (int i = 0; i < 50; ++i) {
            workingMemory->storeValue(std::to_string(i), i);
        }
        
        // Memory should be within capacity
        assert(workingMemory->getCurrentSize() <= workingMemory->getCapacity());
        
        // Clear memory
        workingMemory->clear();
        assert(workingMemory->getCurrentSize() == 0);
        
        std::cout << "✓ Working memory capacity test passed" << std::endl;
    }
    
    static void testEpisodicMemoryCreation() {
        std::cout << "Testing episodic memory creation..." << std::endl;
        
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>("test_episodes");
        assert(episodicMemory != nullptr);
        assert(episodicMemory->getName() == "test_episodes");
        
        std::cout << "✓ Episodic memory creation test passed" << std::endl;
    }
    
    static void testEpisodicMemoryStorage() {
        std::cout << "Testing episodic memory storage..." << std::endl;
        
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>("test_episodes");
        
        // Store an episode
        std::string episodeContent = "Test episode content";
        episodicMemory->storeEpisode(episodeContent);
        
        // Retrieve the episode
        std::string retrievedEpisode = episodicMemory->getLastEpisode();
        assert(retrievedEpisode == episodeContent);
        
        std::cout << "✓ Episodic memory storage test passed" << std::endl;
    }
    
    static void testEpisodicMemoryHistory() {
        std::cout << "Testing episodic memory history..." << std::endl;
        
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>("test_episodes", 10);
        
        // Store multiple episodes
        for (int i = 0; i < 5; ++i) {
            episodicMemory->storeEpisode("Episode " + std::to_string(i));
        }
        
        // Get episode history
        auto history = episodicMemory->getEpisodeHistory();
        assert(history.size() == 5);
        
        // Clear episodes
        episodicMemory->clearEpisodes();
        assert(episodicMemory->getEpisodeCount() == 0);
        
        std::cout << "✓ Episodic memory history test passed" << std::endl;
    }
    
    static void testMemoryIntegration() {
        std::cout << "Testing memory integration..." << std::endl;
        
        // Create both types of memory
        auto workingMemory = std::make_shared<NeuralWorkingMemory>("working", 50);
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>("episodic", 20);
        
        // Test they can coexist
        workingMemory->storeValue("test", 123);
        episodicMemory->storeEpisode("Test episode");
        
        // Verify both are functional
        assert(workingMemory->getValue("test") == 123);
        assert(episodicMemory->getLastEpisode() == "Test episode");
        
        std::cout << "✓ Memory integration test passed" << std::endl;
    }
};

}} // namespace nlm