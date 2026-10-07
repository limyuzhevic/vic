//=============================================================
// BRAIN MANAGER HEADER - Brain.cpp Refactoring (Phase 7)
// Single Responsibility: Integrated Systems Management
//=============================================================

#pragma once

#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

class BrainCore; // Forward declaration

class BrainManager {
public:
    BrainManager();
    ~BrainManager();
    
    // Initialize with reference to core brain
    void initialize(BrainCore* coreBrain);
    
    // Execute a complete step (all integrated systems)
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Getters for integrated systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    DevelopmentSystem* getDevelopmentSystem();
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Action and neuromodulation
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // Reset
    void reset();
    
    // Logging
    void logStatus() const;
    
private:
    // Core brain reference
    BrainCore* m_coreBrain;
    
    // Integrated systems
    NeuralWorkingMemory* m_workingMemory;
    NeuralEpisodicMemory* m_episodicMemory;
    NeuralAssociativeMemory* m_associativeMemory;
    
    PredictionSystem* m_predictionSystem;
    NeuralPlanner* m_planner;
    ConceptFormation* m_conceptFormation;
    AttentionalSelection* m_attention;
    
    DevelopmentSystem* m_developmentSystem;
    
    Dopamine* m_dopamine;
    Curiosity* m_curiosity;
    Novelty* m_novelty;
    PredictionError* m_predictionError;
    
    SpikeSystem* m_spikeSystem;
    STDP* m_stdp;
    Hebbian* m_hebbian;
    StructuralPlasticity* m_structuralPlasticity;
    
    // Helper methods
    void updateWorkingMemory();
    void updateNeuromodulation();
    void updatePlasticity();
    void updateEpisodicMemory();
    void updatePredictionSystem();
    void updateAttention();
    void updateConceptFormation();
    void applyStructuralPlasticity();
    void replayMemories();
    void applyDevelopment();
    void consolidateMemories();
    void updateCheckpoints();
    
    // State tracking
    SimulationStep currentStep;
    Timestamp currentTime;
};

} // namespace nlm
