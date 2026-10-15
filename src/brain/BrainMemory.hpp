#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Brain memory system and utility operations declarations
// Refactored from Brain.cpp to improve maintainability

class BrainMemory {
public:
    // Logging and status
    static void logBrainStatus(const Brain& brain);
    
    // Memory system accessors
    static NeuralWorkingMemory* getWorkingMemory(Brain& brain);
    static NeuralEpisodicMemory* getEpisodicMemory(Brain& brain);
    static NeuralAssociativeMemory* getAssociativeMemory(Brain& brain);
    
    // Prediction system accessor
    static PredictionSystem* getPredictionSystem(Brain& brain);
    
    // Cognition system accessors
    static NeuralPlanner* getPlanner(Brain& brain);
    static ConceptFormation* getConceptFormation(Brain& brain);
    static AttentionalSelection* getAttention(Brain& brain);
    
    // Development system
    static DevelopmentSystem* getDevelopmentSystem(Brain& brain);
    static DevelopmentalStage getDevelopmentalStage(const Brain& brain);
    static void setDevelopmentalStage(Brain& brain, DevelopmentalStage stage);
    
    // Neuromodulation systems
    static Dopamine* getDopamine(Brain& brain);
    static Curiosity* getCuriosity(Brain& brain);
    static Novelty* getNovelty(Brain& brain);
    static PredictionError* getPredictionErrorSignal(Brain& brain);
    
    // Configuration and random access
    static std::shared_ptr<const Config> getConfig(const Brain& brain);
    static RandomGenerator* getRandomGenerator(Brain& brain);
    
    // Input/output operations
    static void receiveSensoryInput(Brain& brain, const class SensoryInput& input);
    static void injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current);
    
    // Statistics methods
    static float getExcitationInhibitionRatio(const Brain& brain);
    static size_t getTotalSpikeCount(const Brain& brain);
    static size_t getPendingSpikeEventCount(const Brain& brain);
    static size_t getTotalNeuronCount(const Brain& brain);
    static size_t getTotalSynapseCount(const Brain& brain);
    static size_t getActiveNeuronCount(const Brain& brain);
    static size_t getFiringNeuronCount(const Brain& brain);
    static float getAverageFiringRate(const Brain& brain);
    
    // Region management
    static RegionId addRegion(Brain& brain, const std::string& name = "");
    static NeuralRegion* getRegion(Brain& brain, RegionId id);
    static const NeuralRegion* getRegion(const Brain& brain, RegionId id);
    static size_t getRegionCount(const Brain& brain);
    static std::vector<RegionId> getRegionIds(const Brain& brain);
    static const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(const Brain& brain);
    
    // Inter-region connection management
    static void addInterRegionConnection(Brain& brain, RegionId source, RegionId target, 
                                        float weight = 0.0f, Delay delay = 1);
    static void removeInterRegionConnection(Brain& brain, RegionId source, RegionId target);
    
    // Spike system accessors
    static SpikeSystem* getSpikeSystem(Brain& brain);
    static const SpikeSystem* getSpikeSystem(const Brain& brain);
    
    // Plasticity system accessors
    static STDP* getSTDP(Brain& brain);
    static Hebbian* getHebbian(Brain& brain);
    static StructuralPlasticity* getStructuralPlasticity(Brain& brain);
    
    // Action production
    static std::unique_ptr<class Action> produceAction(Brain& brain);
    
    // Reset functionality
    static void resetBrain(Brain& brain);
};

}} // namespace nlm