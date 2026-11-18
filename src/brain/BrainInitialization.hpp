#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
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

class BrainInitialization {
public:
    BrainInitialization(Brain::Impl* pImpl, std::shared_ptr<Config> config);
    
    static bool perform(Brain::Impl* pImpl, std::shared_ptr<Config> config);
    
private:
    Brain::Impl* pImpl;
    std::shared_ptr<Config> config;
};

} // namespace nlm