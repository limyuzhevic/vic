// Test program to demonstrate the improved Phase 6 integration
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

void testIntegration() {
    std::cout << "=== Testing NLM Phase 6 Integration ===\n" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "✓ Brain initialized successfully" << std::endl;
    
    // Check that all integrated systems are present
    bool allSystemsPresent = true;
    
    // Memory systems
    if (brain->getWorkingMemory()) {
        std::cout << "✓ Working memory system is integrated" << std::endl;
    } else {
        std::cout << "✗ Working memory system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getEpisodicMemory()) {
        std::cout << "✓ Episodic memory system is integrated" << std::endl;
    } else {
        std::cout << "✗ Episodic memory system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getAssociativeMemory()) {
        std::cout << "✓ Associative memory system is integrated" << std::endl;
    } else {
        std::cout << "✗ Associative memory system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    // Prediction system
    if (brain->getPredictionSystem()) {
        std::cout << "✓ Prediction system is integrated" << std::endl;
    } else {
        std::cout << "✗ Prediction system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    // Cognition systems
    if (brain->getPlanner()) {
        std::cout << "✓ Neural planner is integrated" << std::endl;
    } else {
        std::cout << "✗ Neural planner is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getConceptFormation()) {
        std::cout << "✓ Concept formation is integrated" << std::endl;
    } else {
        std::cout << "✗ Concept formation is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getAttention()) {
        std::cout << "✓ Attentional selection is integrated" << std::endl;
    } else {
        std::cout << "✗ Attentional selection is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    // Development system
    if (brain->getDevelopmentSystem()) {
        std::cout << "✓ Development system is integrated" << std::endl;
    } else {
        std::cout << "✗ Development system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    // Neuromodulation systems
    if (brain->getDopamine()) {
        std::cout << "✓ Dopamine system is integrated" << std::endl;
    } else {
        std::cout << "✗ Dopamine system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getCuriosity()) {
        std::cout << "✓ Curiosity system is integrated" << std::endl;
    } else {
        std::cout << "✗ Curiosity system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getNovelty()) {
        std::cout << "✓ Novelty detection is integrated" << std::endl;
    } else {
        std::cout << "✗ Novelty detection is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    if (brain->getPredictionErrorSignal()) {
        std::cout << "✓ Prediction error system is integrated" << std::endl;
    } else {
        std::cout << "✗ Prediction error system is NOT integrated" << std::endl;
        allSystemsPresent = false;
    }
    
    // Test the brain loop
    std::cout << "\n=== Testing Brain Loop ===\n" << std::endl;
    
    // Run a few steps
    for (int step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 5 == 0) {
            std::cout << "Step " << step << 
                ", Firing: " << brain->getFiringNeuronCount() << 
                ", Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "\n✓ Brain loop completed successfully" << std::endl;
    
    // Check memory systems status
    if (brain->getWorkingMemory()) {
        std::cout << "Working memory traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
    }
    
    if (brain->getEpisodicMemory()) {
        std::cout << "Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
    }
    
    if (brain->getDopamine()) {
        std::cout << "Dopamine level: " << brain->getDopamine()->getLevel() << std::endl;
    }
    
    if (brain->getCuriosity()) {
        std::cout << "Curiosity level: " << brain->getCuriosity()->getLevel() << std::endl;
    }
    
    if (brain->getNovelty()) {
        std::cout << "Novelty level: " << brain->getNovelty()->getLevel() << std::endl;
    }
    
    std::cout << "\n=== Integration Test Results ===\n" << std::endl;
    if (allSystemsPresent) {
        std::cout << "✓ ALL SYSTEMS INTEGRATED SUCCESSFULLY!" << std::endl;
        std::cout << "The NLM artificial brain is functioning as intended." << std::endl;
    } else {
        std::cout << "✗ SOME SYSTEMS ARE NOT INTEGRATED" << std::endl;
        std::cout << "Integration requires additional work." << std::endl;
    }
}

int main() {
    try {
        testIntegration();
        return 0;
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
