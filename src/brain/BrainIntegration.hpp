// Brain integration system - memory, prediction, cognition, development, neuromodulation
// This file contains integration points for all brain subsystems

#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

class Brain;

// Integration interface for memory systems
class BrainMemoryIntegration {
public:
    virtual ~BrainMemoryIntegration() = default;
    
    // Working memory - transient active information
    virtual NeuralWorkingMemory* getWorkingMemory() = 0;
    
    // Episodic memory - experience storage
    virtual NeuralEpisodicMemory* getEpisodicMemory() = 0;
    
    // Associative memory - pattern associations
    virtual NeuralAssociativeMemory* getAssociativeMemory() = 0;
    
protected:
    BrainMemoryIntegration() = default;
};

// Integration interface for prediction systems
class BrainPredictionIntegration {
public:
    virtual ~BrainPredictionIntegration() = default;
    
    // Prediction system for sensory prediction and error computation
    virtual PredictionSystem* getPredictionSystem() = 0;
    
protected:
    BrainPredictionIntegration() = default;
};

// Integration interface for cognition systems
class BrainCognitionIntegration {
public:
    virtual ~BrainCognitionIntegration() = default;
    
    // Neural planner for action planning
    virtual NeuralPlanner* getPlanner() = 0;
    
    // Concept formation for pattern discovery
    virtual ConceptFormation* getConceptFormation() = 0;
    
    // Attentional selection for focus
    virtual AttentionalSelection* getAttention() = 0;
    
protected:
    BrainCognitionIntegration() = default;
};

// Integration interface for development system
class BrainDevelopmentIntegration {
public:
    virtual ~BrainDevelopmentIntegration() = default;
    
    // Development system for plasticity rate modulation
    virtual DevelopmentSystem* getDevelopmentSystem() = 0;
    
    // Developmental stage
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    
protected:
    BrainDevelopmentIntegration() = default;
};

// Integration interface for neuromodulation systems
class BrainNeuromodulationIntegration {
public:
    virtual ~BrainNeuromodulationIntegration() = default;
    
    // Dopamine - reward and reinforcement
    virtual Dopamine* getDopamine() = 0;
    
    // Curiosity - exploration motivation
    virtual Curiosity* getCuriosity() = 0;
    
    // Novelty - novelty detection
    virtual Novelty* getNovelty() = 0;
    
    // Prediction error signal
    virtual PredictionError* getPredictionErrorSignal() = 0;
    
protected:
    BrainNeuromodulationIntegration() = default;
};

} // namespace nlm