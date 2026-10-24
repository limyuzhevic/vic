#pragma once

#include "Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainMemoryManager {
public:
    BrainMemoryManager();
    ~BrainMemoryManager();
    
    // Disable copying
    BrainMemoryManager(const BrainMemoryManager&) = delete;
    BrainMemoryManager& operator=(const BrainMemoryManager&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update working memory
    void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    
    // Update episodic memory
    void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Apply neuromodulation to memory
    void applyNeuromodulationToMemory(Brain* brain, float neuromodulationLevel);
    
    // Get memory system pointers
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
