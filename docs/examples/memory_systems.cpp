/**
 * NLM Memory Systems Example
 * 
 * This example demonstrates how NLM manages different memory systems:
 * working memory, episodic memory, and associative memory.
 * It shows how these memory systems integrate with neural processing
 * and enable more sophisticated cognition.
 */

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace nlm;

void printMemoryStats(const std::string& title, 
                      const WorkingMemoryStats& workingMem,
                      const EpisodicMemoryStats& episodicMem,
                      const AssociativeMemoryStats& assocMem) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "\nWorking Memory:\n";
    std::cout << "  Capacity: " << workingMem.capacity << " items\n";
    std::cout << "  Active items: " << workingMem.activeCount << "\n";
    std::cout << "  Fill ratio: " << std::fixed << std::setprecision(2) << workingMem.fillRatio * 100 << "%\n";
    std::cout << "  Average retention: " << workingMem.avgRetentionTime << " steps\n";
    
    std::cout << "\nEpisodic Memory:\n";
    std::cout << "  Episodes stored: " << episodicMem.episodesStored << "\n";
    std::cout << "  Average episode length: " << episodicMem.avgEpisodeLength << " steps\n";
    std::cout << "  Recall accuracy: " << std::setprecision(2) << episodicMem.recallAccuracy * 100 << "%\n";
    std::cout << "  Memory replay frequency: " << episodicMem.replayFrequency << " times/step\n";
    
    std::cout << "\nAssociative Memory:\n";
    std::cout << "  Associations formed: " << assocMem.associationsCount << "\n";
    std::cout << "  Average strength: " << std::setprecision(3) << assocMem.avgAssociationStrength << "\n";
    std::cout << "  Pattern diversity: " << std::setprecision(2) << assocMem.patternDiversity * 100 << "%\n";
    std::cout << "  Interference level: " << std::setprecision(2) << assocMem.interferenceLevel * 100 << "%\n";
}

int main() {
    std::cout << "NLM Memory Systems Example" << std::endl;
    std::cout << "===========================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Warning);
    
    printSection("Setup");
    std::cout << "Creating configuration for memory systems experiment...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1500));
    config->set("region_count", size_t(2));
    config->set("enable_working_memory", true);
    config->set("enable_episodic_memory", true);
    config->set("enable_associative_memory", true);
    
    printSection("Initialize Brain");
    std::cout << "Creating and initializing brain...\n";
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSection("Memory Integration");
    std::cout << "The brain already has memory systems integrated through configuration.\n";
    std::cout << "Memory systems will be activated during simulation.\n";
    
    printSection("Run Memory Learning Phase");
    std::cout << "Running simulation to train memory systems...\n";
    
    // Phase 1: Pattern Learning (20 steps)
    std::cout << "\nPhase 1: Pattern Learning (20 steps)\n";
    for (int step = 0; step < 20; ++step) {
        brain->step(step);
        
        if (step == 19) {
            // Add a pattern to working memory
            std::vector<float> pattern = {0.8f, 0.5f, 0.3f, 0.9f, 0.2f};
            brain->addPatternToWorkingMemory(pattern);
        }
        
        if (step % 5 == 0) {
            auto stats = brain->getWorkingMemoryStats();
            std::cout << "  Step " << step << ": Working memory load: " << stats.fillRatio * 100 << "%\n";
        }
    }
    
    // Phase 2: Episodic Experience (30 steps)
    std::cout << "\nPhase 2: Episodic Experience (30 steps)\n";
    for (int step = 20; step < 50; ++step) {
        brain->step(step);
        
        if (step == 49) {
            // Store an episodic experience
            Experience experience;
            experience.id = step;
            experience.sensoryInput = std::vector<float>(5, 0.5f);
            experience.actions = std::vector<ActionType> {ActionType::MoveForward, ActionType::Interact};
            experience.rewards = std::vector<float> {0.1f, 1.0f};
            experience.timestep = step;
            
            brain->storeEpisodicMemory(experience);
            std::cout << "  Stored episode at step " << step << "\n";
        }
    }
    
    // Phase 3: Association Formation (40 steps)
    std::cout << "\nPhase 3: Association Formation (40 steps)\n";
    for (int step = 50; step < 90; ++step) {
        brain->step(step);
        
        if (step == 89) {
            // Form an association
            std::vector<float> triggerPattern = {0.9f, 0.3f, 0.7f};
            std::vector<float> responsePattern = {0.2f, 0.8f, 0.4f};
            
            brain->formAssociation(triggerPattern, responsePattern);
            std::cout << "  Formed association between patterns\n";
        }
    }
    
    // Phase 4: Recall and Consolidation (30 steps)
    std::cout << "\nPhase 4: Recall and Consolidation (30 steps)\n";
    for (int step = 90; step < 120; ++step) {
        brain->step(step);
        
        // Attempt recall based on current state
        auto currentPattern = brain->getCurrentNeuralPattern();
        std::vector<float> recalledPattern;
        
        if (brain->recallFromEpisodicMemory(currentPattern, recalledPattern)) {
            std::cout << "  Recalled pattern at step " << step << "\n";
        }
        
        // Attempt associative recall
        brain->attemptAssociativeRecall(currentPattern);
        
        // Memory consolidation occurs automatically
        brain->consolidateMemory(step % 60); // Consolidate every 60 steps
    }
    
    printSection("Final Memory Statistics");
    std::cout << "Gathering comprehensive memory statistics...\n";
    
    auto workingStats = brain->getWorkingMemoryStats();
    auto episodicStats = brain->getEpisodicMemoryStats();
    auto assocStats = brain->getAssociativeMemoryStats();
    
    printMemoryStats("MEMORY SYSTEMS STATUS", workingStats, episodicStats, assocStats);
    
    printSection("Memory System Integration");
    std::cout << "\nMemory systems are integrated with the brain:\n";
    std::cout << "  • Working memory maintains information across delays\n";
    std::cout << "  • Episodic memory stores and recalls experiences\n";
    std::cout << "  • Associative memory forms pattern relationships\n";
    std::cout << "  • All systems affect neural dynamics and plasticity\n";
    std::cout << "  • Memory statistics are available through brain interface\n";
    
    return 0;
}
