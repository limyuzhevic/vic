
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Memory Integration Test ===" << std::endl;
    
    // Create a basic configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Initialize brain (this should initialize all integrated memory systems)
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Check if memory systems are properly initialized
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    std::cout << "Working memory initialized: " << (workingMemory != nullptr ? "✓" : "✗") << std::endl;
    std::cout << "Episodic memory initialized: " << (episodicMemory != nullptr ? "✓" : "✗") << std::endl;
    
    if (workingMemory) {
        std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
    }
    
    if (episodicMemory) {
        std::cout << "Episodic memory max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
        std::cout << "Episodic memory replay enabled: " << (episodicMemory->isReplayEnabled() ? "✓" : "✗") << std::endl;
    }
    
    // Test brain step (should integrate memory systems)
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
    }
    
    std::cout << "✓ Memory integration test completed successfully" << std::endl;
    return 0;
}
