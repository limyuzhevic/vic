#include <iostream>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

int main() {
    std::cout << "=== Testing Memory System Integration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    std::cout << "Creating brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    // Check memory systems before initialization
    std::cout << "Before initialize() - WorkingMemory: " << (brain->getWorkingMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "Before initialize() - EpisodicMemory: " << (brain->getEpisodicMemory() ? "NOT NULL" : "NULL") << std::endl;
    
    // Initialize brain
    std::cout << "Initializing brain..." << std::endl;
    if (!brain->initialize()) {
        std::cout << "ERROR: Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Check memory systems after initialization
    std::cout << "After initialize() - WorkingMemory: " << (brain->getWorkingMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "After initialize() - EpisodicMemory: " << (brain->getEpisodicMemory() ? "NOT NULL" : "NULL") << std::endl;
    
    if (brain->getWorkingMemory()) {
        std::cout << "WorkingMemory active traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        std::cout << "WorkingMemory capacity: " << brain->getWorkingMemory()->getCapacity() << std::endl;
    }
    
    if (brain->getEpisodicMemory()) {
        std::cout << "EpisodicMemory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        std::cout << "EpisodicMemory max episodes: " << brain->getEpisodicMemory()->getMaxEpisodes() << std::endl;
    }
    
    // Run some steps
    std::cout << "Running simulation steps..." << std::endl;
    for (uint64_t step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Check again
    std::cout << "After simulation - WorkingMemory: " << (brain->getWorkingMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "After simulation - EpisodicMemory: " << (brain->getEpisodicMemory() ? "NOT NULL" : "NULL") << std::endl;
    
    if (brain->getWorkingMemory()) {
        std::cout << "WorkingMemory active traces after simulation: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
    }
    
    if (brain->getEpisodicMemory()) {
        std::cout << "EpisodicMemory episodes after simulation: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
    }
    
    // Test reset
    std::cout << "Testing reset..." << std::endl;
    brain->reset();
    brain->initialize(); // Re-initialize after reset
    
    std::cout << "After reset/reinitialize - WorkingMemory: " << (brain->getWorkingMemory() ? "NOT NULL" : "NULL") << std::endl;
    std::cout << "After reset/reinitialize - EpisodicMemory: " << (brain->getEpisodicMemory() ? "NOT NULL" : "NULL") << std::endl;
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}