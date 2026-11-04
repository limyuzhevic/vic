#pragma once

#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <vector>

namespace nlm {

class BrainIntegration {
public:
    BrainIntegration();
    ~BrainIntegration();
    
    // Initialize integrated systems
    bool initialize(BrainCore* core);
    
    // Update integrated systems
    void step(BrainCore* core, TimestepDuration timestep);
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Prediction system
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
    // Cognition systems
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Neuromodulation systems
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionErrorSignal() { return predictionError.get(); }
    
    // Apply neuromodulation
    void applyNeuromodulation(BrainCore* core);
    
    // Process memory systems
    void updateMemorySystems(BrainCore* core, TimestepDuration timestep);
    void updatePredictionSystem(BrainCore* core, TimestepDuration timestep);
    void updateCognitionSystems(BrainCore* core, TimestepDuration timestep);
    void updateDevelopment(BrainCore* core, TimestepDuration timestep);
    void applyDevelopmentEffects(BrainCore* core);
    
private:
    // Integrated memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    BrainCore* core = nullptr;
};

} // namespace nlm
