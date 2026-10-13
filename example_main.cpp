// NLM Simple Example - Basic Brain Simulation
// This example demonstrates the core functionality of the NLM (熙然) neural learning machine

#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    std::cout << "=== NLM (熙然) Neural Learning Machine - Example ===" << std::endl;
    std::cout << "Creating an integrated artificial brain..." << std::endl << std::endl;
    
    // Create configuration using the research preset
    auto config = Config::createResearch();
    std::cout << "Configuration loaded:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Create the brain with configuration
    nlm::Brain brain(config);
    std::cout << "Brain created successfully." << std::endl;
    
    // Initialize the brain (this sets up all neural systems)
    if (!brain.initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    std::cout << "Starting simulation..." << std::endl << std::endl;
    
    // Record start time for performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Main simulation loop
    for (size_t step = 0; step < config.maxSteps; ++step) {
        // Step the brain forward in time
        brain.step(step);
        
        // Get action based on current neural activity
        auto action = brain.produceAction();
        
        // Simulate some environmental interaction
        // (In a real application, this would interface with the environment)
        
        // Log status every 500 steps
        if (step % 500 == 0 && step > 0) {
            std::cout << "Step " << step << ": " 
                      << "Neurons: " << brain.getTotalNeuronCount() << ", "
                      << "Spikes: " << brain.getTotalSpikeCount() << ", "
                      << "Active: " << brain.getActiveNeuronCount() << ", "
                      << "Firing: " << brain.getFiringNeuronCount() << std::endl;
        }
        
        // Simulate sensory input (in a real application, this would come from environment)
        // For this example, we skip actual sensory processing
    }
    
    // Calculate and display final statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "=== SIMULATION COMPLETE ===" << std::endl;
    std::cout << "Total steps: " << config.maxSteps << std::endl;
    std::cout << "Wall clock time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Average steps per second: " << (config.maxSteps / duration.count()) << std::endl << std::endl;
    
    // Display final brain status
    brain.logStatus();
    
    // Demonstrate memory system integration
    std::cout << std::endl;
    std::cout << "=== MEMORY SYSTEM INTEGRATION ===" << std::endl;
    
    if (auto* workingMemory = brain.getWorkingMemory()) {
        std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
        std::cout << "Current active traces: " << workingMemory->getActiveTraces() << std::endl;
        std::cout << "Memory activity level: " << workingMemory->getMemoryActivity() << std::endl;
    }
    
    if (auto* episodicMemory = brain.getEpisodicMemory()) {
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        std::cout << "Average reward: " << episodicMemory->getAverageReward() << std::endl;
    }
    
    if (auto* associativeMemory = brain.getAssociativeMemory()) {
        std::cout << "Associative memory associations: " << associativeMemory->getAssociationCount() << std::endl;
    }
    
    // Demonstrate neuromodulation
    std::cout << std::endl;
    std::cout << "=== NEUROMODULATION STATUS ===" << std::endl;
    
    if (auto* dopamine = brain.getDopamine()) {
        std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
        std::cout << "Dopamine plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
    }
    
    if (auto* curiosity = brain.getCuriosity()) {
        std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
    }
    
    if (auto* novelty = brain.getNovelty()) {
        std::cout << "Novelty level: " << novelty->getLevel() << std::endl;
    }
    
    // Demonstrate development
    std::cout << std::endl;
    std::cout << "=== DEVELOPMENT STATUS ===" << std::endl;
    
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    std::cout << "Developmental stage: " << static_cast<int>(stage) << std::endl;
    
    if (auto* developmentSystem = brain.getDevelopmentSystem()) {
        std::cout << "Development system active: " << (developmentSystem->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    }
    
    // Demonstrate checkpoint functionality
    std::cout << std::endl;
    std::cout << "=== CHECKPOINT FUNCTIONALITY ===" << std::endl;
    
    if (config.enableCheckpointing) {
        std::string checkpointPath = "./example_checkpoint.bin";
        if (brain.save(checkpointPath)) {
            std::cout << "Checkpoint saved to: " << checkpointPath << std::endl;
            
            // Create a new brain and load the checkpoint
            auto newBrain = std::make_unique<nlm::Brain>(config);
            newBrain->initialize();
            
            if (newBrain->load(checkpointPath)) {
                std::cout << "Checkpoint loaded successfully!" << std::endl;
                std::cout << "Loaded brain has " << newBrain->getTotalNeuronCount() 
                          << " neurons and " << newBrain->getTotalSynapseCount() 
                          << " synapses." << std::endl;
            } else {
                std::cerr << "Warning: Failed to load checkpoint" << std::endl;
            }
        } else {
            std::cerr << "Warning: Failed to save checkpoint" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== EXAMPLE COMPLETE ===" << std::endl;
    std::cout << "NLM simulation completed successfully!" << std::endl;
    std::cout << "The brain demonstrated:" << std::endl;
    std::cout << "  - Complete integration of neural computation" << std::endl;
    std::cout << "  - Memory system functionality" << std::endl;
    std::cout << "  - Neuromodulation effects" << std::endl;
    std::cout << "  - Developmental processes" << std::endl;
    std::cout << "  - Checkpoint persistence" << std::endl;
    
    return 0;
}
