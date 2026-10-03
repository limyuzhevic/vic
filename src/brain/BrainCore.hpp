#pragma once

#include "Brain.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <memory>

namespace nlm {

// Forward declarations for system classes
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class RandomGenerator;
class Config;

// Brain PImpl implementation details
struct Brain::Impl {
    std::shared_ptr<Config> config;
    
    // Storage for brain regions
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    
    // Neural populations for I/O
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // System pointers
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // Random number generator
    std::unique_ptr<RandomGenerator> rng;
    
    // Simulation state
    SimulationStep currentStep = 0;
    Timestamp currentTime = 0.0;
    size_t totalSpikesThisStep = 0;
    size_t totalSpikesTotal = 0;
    bool isResting = false;
    size_t stepsSinceLastEpisode = 0;
    DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
};

} // namespace nlm
