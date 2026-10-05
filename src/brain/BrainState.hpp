#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
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
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "BrainCore.hpp"
#include <string>

namespace nlm {

class Brain;

// Forward declarations for checkpointing
class CheckpointManager;
class CheckpointWriter;
class CheckpointReader;

// Forward declarations for neural components
class NeuralRegion;

// BrainState implementation - Phase 6: Integrated Artificial Brain
// Contains state management and serialization
struct BrainState {
    // ========== STATE MANAGEMENT ==========
    
    // Reset brain state
    void reset(Brain* parent);
    
    // Save brain state to file (checkpointing)
    bool save(const Brain* parent, const std::string& filepath) const;
    
    // Load brain state from file
    bool load(Brain* parent, const std::string& filepath);
    
    // Utility methods for brain state
    size_t getTotalNeuronCount(const Brain* parent) const;
    size_t getTotalSynapseCount(const Brain* parent) const;
    size_t getRegionCount(const Brain* parent) const;
    size_t getActiveNeuronCount(const Brain* parent) const;
    size_t getFiringNeuronCount(const Brain* parent) const;
    float getAverageFiringRate(const Brain* parent) const;
    float getExcitationInhibitionRatio(const Brain* parent) const;
    size_t getTotalSpikeCount(const Brain* parent) const;
    size_t getPendingSpikeEventCount(const Brain* parent) const;
    
    // ========== UTILITY AND ACCESSOR METHODS ==========
    
    // Logging
    void logStatus(const Brain* parent) const;
    
    // Region management utilities
    std::vector<RegionId> getRegionIds(const Brain* parent) const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(const Brain* parent) const;
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory(Brain* parent);
    NeuralEpisodicMemory* getEpisodicMemory(Brain* parent);
    NeuralAssociativeMemory* getAssociativeMemory(Brain* parent);
    
    // Prediction system accessor
    PredictionSystem* getPredictionSystem(Brain* parent);
    
    // Cognition system accessors
    NeuralPlanner* getPlanner(Brain* parent);
    ConceptFormation* getConceptFormation(Brain* parent);
    AttentionalSelection* getAttention(Brain* parent);
    
    // Development system accessor
    DevelopmentSystem* getDevelopmentSystem(Brain* parent);
    DevelopmentalStage getDevelopmentalStage(const Brain* parent) const;
    
    // Neuromodulation system accessors
    Dopamine* getDopamine(Brain* parent);
    Curiosity* getCuriosity(Brain* parent);
    Novelty* getNovelty(Brain* parent);
    PredictionError* getPredictionErrorSignal(Brain* parent);
    
    // Configuration accessors
    std::shared_ptr<const Config> getConfig(const Brain* parent) const;
    RandomGenerator* getRandomGenerator(Brain* parent);
    
    // Spike system accessors
    SpikeSystem* getSpikeSystem(Brain* parent);
    const SpikeSystem* getSpikeSystem(const Brain* parent) const;
    
    // Plasticity system accessors
    STDP* getSTDP(Brain* parent);
    Hebbian* getHebbian(Brain* parent);
    StructuralPlasticity* getStructuralPlasticity(Brain* parent);
    
    // Integration utilities
    void collectStatistics(Brain* parent);
    void updateIntegrationState(Brain* parent);
};

} // namespace nlm