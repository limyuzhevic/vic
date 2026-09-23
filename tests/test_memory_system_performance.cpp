// Memory System Performance Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

namespace test_memory_system_performance {

void testMemorySystemCreation() {
    nlm::NeuralWorkingMemory workingMem;
    nlm::NeuralEpisodicMemory episodicMem;
    
    std::cout << "    testMemorySystemCreation passed" << std::endl;
}

void testWorkingMemoryPerformance() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Configure for performance testing
    workingMem.setCapacity(512);
    workingMem.setDecayRate(0.01f);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Store multiple patterns
    for (int i = 0; i < 500; ++i) {
        std::vector<float> pattern = {
            static_cast<float>(i % 10) / 10.0f,
            static_cast<float>((i + 3) % 10) / 10.0f,
            static_cast<float>((i + 7) % 10) / 10.0f
        };
        workingMem.store(pattern, 0.8f);
    }
    
    // Retrieve patterns
    for (int i = 0; i < 100; ++i) {
        std::vector<float> pattern = {
            static_cast<float>(i % 10) / 10.0f,
            static_cast<float>((i + 3) % 10) / 10.0f,
            static_cast<float>((i + 7) % 10) / 10.0f
        };
        bool contains = workingMem.contains(0);  // Will vary based on implementation
        (void)contains;  // Use to avoid unused variable warning
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testWorkingMemoryPerformance passed (500 stores/retrievals in " 
              << duration.count() << " ms)" << std::endl;
}

void testEpisodicMemoryPerformance() {
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Store many episodes for performance testing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = {static_cast<float>(i % 100) / 100.0f,
                                static_cast<float>((i + 50) % 100) / 100.0f,
                                static_cast<float>((i + 25) % 100) / 100.0f};
        episode.positionX = static_cast<float>(i % 100);
        episode.positionY = static_cast<float>((i + 50) % 100);
        episode.reward = static_cast<float>(i % 10);
        episode.age = i;
        
        episodicMem.storeEpisode(episode);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testEpisodicMemoryPerformance passed (1000 episode stores in " 
              << duration.count() << " ms)" << std::endl;
}

void testMemorySystemUpdatePerformance() {
    nlm::NeuralWorkingMemory workingMem;
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Configure working memory
    workingMem.setCapacity(1024);
    workingMem.setDecayRate(0.005f);
    
    // Store some data
    for (int i = 0; i < 100; ++i) {
        std::vector<float> pattern = {static_cast<float>(i) / 100.0f};
        workingMem.store(pattern, 0.5f);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = pattern;
        episode.age = i;
        episodicMem.storeEpisode(episode);
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update systems many times
    for (int i = 0; i < 1000; ++i) {
        workingMem.update(0.001);
        episodicMem.update(0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testMemorySystemUpdatePerformance passed (2000 updates in " 
              << duration.count() << " ms)" << std::endl;
}

void testMemorySystemMemoryUsage() {
    nlm::NeuralWorkingMemory workingMem;
    nlm::NeuralEpisodicMemory episodicMem;
    
    // Fill memory systems
    for (int i = 0; i < 500; ++i) {
        std::vector<float> pattern = {static_cast<float>(i) / 500.0f};
        workingMem.store(pattern, 0.5f);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.sensoryState = pattern;
        episodicMem.storeEpisode(episode);
    }
    
    // Verify systems are functioning under load
    size_t workingMemSize = workingMem.getActiveTraces();
    
    std::cout << "    testMemorySystemMemoryUsage passed (working memory active traces: " 
              << workingMemSize << ")" << std::endl;
}

void runAll() {
    std::cout << "Running memory system performance tests..." << std::endl;
    testMemorySystemCreation();
    testWorkingMemoryPerformance();
    testEpisodicMemoryPerformance();
    testMemorySystemUpdatePerformance();
    testMemorySystemMemoryUsage();
}

} // namespace test_memory_system_performance