// MemorySystem - Handles working memory, episodic memory, and attention
// Extracted from Brain::step() method
#pragma once

#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <memory>

namespace nlm {

class MemorySystem {
public:
    MemorySystem(std::shared_ptr<Brain> brain);
    ~MemorySystem();
    
    // Initialize the memory system
    bool initialize();
    
    // Process one timestep of memory operations
    void step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Update attention based on current neural activity
    void updateAttention();
    
    // Update concept formation
    void updateConceptFormation();
    
    // Update prediction system
    void updatePredictionSystem();
    
    // Get memory system pointers
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    NeuralPlanner* getPlanner() const { return planner_.get(); }
    ConceptFormation* getConceptFormation() const { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() const { return attention_.get(); }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // State tracking
    size_t stepsSinceLastEpisode_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    
    // Helper methods
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updateAttentionSystem();
    void updateWorkingMemory(TimestepDuration timestep);
    void updateMemoryReplay(SimulationStep currentStep);
    void consolidateMemories(SimulationStep currentStep);
};

} // namespace nlm
