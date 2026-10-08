#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class MemorySystemManager {
public:
    MemorySystemManager(std::shared_ptr<Brain> brain);
    
    // Get working memory
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory.get(); }
    
    // Get episodic memory
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory.get(); }
    
    // Get associative memory
    NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory.get(); }
    
    // Get prediction system
    PredictionSystem* getPredictionSystem() const { return predictionSystem.get(); }
    
    // Get cognition systems
    NeuralPlanner* getPlanner() const { return planner.get(); }
    ConceptFormation* getConceptFormation() const { return conceptFormation.get(); }
    AttentionalSelection* getAttention() const { return attention.get(); }
    
    // Get neuromodulation systems
    Dopamine* getDopamine() const { return dopamine.get(); }
    Curiosity* getCuriosity() const { return curiosity.get(); }
    Novelty* getNovelty() const { return novelty.get(); }
    PredictionError* getPredictionError() const { return predictionError.get(); }
    
    // Get development system
    DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem.get(); }
    
    // Initialize all memory systems
    bool initializeAll();
    
    // Reset all memory systems
    void resetAll();
    
    // Update all systems for current time step
    void updateAll(double timestep);
    
private:
    std::shared_ptr<Brain> brain;
    
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
};

} // namespace nlm