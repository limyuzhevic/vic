#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Brain::Impl;
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;
class NeuralRegion;
class Neuron;
class Synapse;

class BrainMemory {
public:
    BrainMemory(Brain::Impl* pImpl);
    
    void performWorkingMemoryUpdate(TimestepDuration timestep);
    void performEpisodicMemoryUpdate(SimulationStep currentStep);
    void performPredictionSystemUpdate();
    void performAttentionSystemUpdate();
    void performConceptFormationUpdate();
    void performNeuromodulationUpdate();
    void performPlasticityUpdate(SimulationStep currentStep);
    void performStructuralPlasticityUpdate(SimulationStep currentStep);
    void performMemoryReplay(SimulationStep currentStep);
    void performDevelopmentUpdate(SimulationStep currentStep);
    void performMemoryConsolidation(SimulationStep currentStep);
    void performCheckpointUpdate(SimulationStep currentStep);
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm