// ex02_memory_systems.cpp - Memory Systems Integration

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 2: Memory Systems Integration ===" << std::endl;
    
    // Configure with all memory systems enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 200);
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.workingMemory.capacity", 50);
    config->set("brain.memory.episodicMemory.enabled", true);
    config->set("brain.memory.episodicMemory.maxEpisodes", 100);
    config->set("brain.memory.associativeMemory.enabled", true);
    config->set("brain.memory.associativeMemory.similarityThreshold", 0.8);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    std::cout << "Memory systems brain initialized!" << std::endl;
    
    // Get memory system pointers
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    std::cout << "\n--- Working Memory Demo ---" << std::endl;
    // Create and store experiences in working memory
    std::vector<std::vector<float>> experiences = {
        {0.1f, 0.5f, 0.8f, 0.3f, 0.7f},
        {0.9f, 0.2f, 0.4f, 0.6f, 0.1f},
        {0.3f, 0.7f, 0.2f, 0.9f, 0.4f},
        {0.6f, 0.8f, 0.3f, 0.5f, 0.9f}
    };
    
    for (size_t i = 0; i < experiences.size(); ++i) {
        workingMemory->store(experiences[i]);
        std::cout << "Stored experience " << i << ": "
                 << "Active patterns: " << workingMemory->getActiveCount() << std::endl;
    }
    
    // Retrieve from working memory
    auto retrieved = workingMemory->retrieve();
    std::cout << "Retrieved pattern: ";
    for (float val : retrieved) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n--- Episodic Memory Demo ---" << std::endl;
    // Store experiences in episodic memory with timestamps
    auto currentTime = std::chrono::duration<double>::zero();
    for (size_t i = 0; i < experiences.size(); ++i) {
        auto episodeTime = currentTime + std::chrono::duration<double>(i * 10.0);
        episodicMemory->encode(experiences[i], episodeTime);
        std::cout << "Encoded experience at time " 
                  << std::chrono::duration<double>(episodeTime).count() << "s" << std::endl;
    }
    
    // Replay memories
    auto replayed = episodicMemory->replay(currentTime - 30.0, currentTime);
    std::cout << "Replayed " << replayed.size() << " episodes" << std::endl;
    
    std::cout << "\n--- Associative Memory Demo ---" << std::endl;
    // Create pattern associations
    for (size_t i = 0; i < experiences.size() - 1; ++i) {
        associativeMemory->associate(experiences[i], experiences[i + 1]);
        std::cout << "Associated pattern " << i << " with pattern " << i + 1 << std::endl;
    }
    
    // Retrieve associated patterns
    std::vector<float> queryPattern = {0.1f, 0.5f, 0.8f, 0.3f, 0.7f};
    auto result = associativeMemory->retrieve(queryPattern);
    std::cout << "Retrieved from associative memory: ";
    for (float val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Clear working memory to free resources
    workingMemory->clear();
    std::cout << "\nWorking memory cleared. Active patterns: " << workingMemory->getActiveCount() << std::endl;
    
    // Simulate memory consolidation
    std::cout << "\n--- Memory Consolidation ---" << std::endl;
    episodicMemory->consolidate(0.3f);
    std::cout << "Consolidated episodic memory (retention threshold: 0.3)" << std::endl;
    
    // Simulate replay during sleep
    std::cout << "\n--- Sleep/Replay Simulation ---" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        // Replay important memories during 'sleep'
        auto sleepReplay = episodicMemory->getEpisodesForReplay(2);
        std::cout << "Replay cycle " << i + 1 << ": Replayed " << sleepReplay.size() << " episodes" << std::endl;
    }
    
    std::cout << "\n=== Example 2 Complete ===" << std::endl;
    return 0;
}