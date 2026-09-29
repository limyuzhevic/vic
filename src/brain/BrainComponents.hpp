#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainComponents {
public:
    // Component access methods
    static SpikeSystem* getSpikeSystem(Brain& brain);
    static const SpikeSystem* getSpikeSystem(const Brain& brain);
    
    static STDP* getSTDP(Brain& brain);
    static Hebbian* getHebbian(Brain& brain);
    static StructuralPlasticity* getStructuralPlasticity(Brain& brain);
    
    // Statistics methods
    static float getExcitationInhibitionRatio(const Brain& brain);
    static size_t getTotalSpikeCount(const Brain& brain);
    static size_t getPendingSpikeEventCount(const Brain& brain);
    
    static size_t getTotalNeuronCount(const Brain& brain);
    static size_t getTotalSynapseCount(const Brain& brain);
    static size_t getActiveNeuronCount(const Brain& brain);
    static size_t getFiringNeuronCount(const Brain& brain);
    static float getAverageFiringRate(const Brain& brain);
    
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
    
    // Config and random generator
    static std::shared_ptr<const Config> getConfig(const Brain& brain);
    static RandomGenerator* getRandomGenerator(Brain& brain);
};

} // namespace nlm