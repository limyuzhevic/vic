#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
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

// Neural components (needed for step function implementation)
class NeuralRegion;
class Neuron;
class Synapse;

// Action system
class Action;

// Sensory input
class SensoryInput;

// Neuromodulator
class Neuromodulator;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);

    // Helper functions for modular step processing
private:
    void processDelayedSpikeEvents();
    void updateNeuronLIFDynamics(Timestamp currentTime);
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime);
    bool isNeuronJustFired(const NeuronState& state, Timestamp currentTime) const;
    void handleNeuronFiring(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime, NeuralRegion* region);
    void scheduleDelayedSpikeEvent(Neuron* neuron, Synapse* syn, SimulationStep currentStep, Timestamp currentTime);
    void processSpikes(SimulationStep currentStep);
    void updateWorkingMemory();
    void applyNeuromodulation();
    void applyDopamineExcitabilityEffects();
    void applyPlasticityRules(Timestamp currentTime);
    float calculatePlasticityModulation() const;
    void applySTDP(Synapse* syn, float plasticityMod);
    void applyHebbianLearning(Synapse* syn);
    void updateEpisodicMemory();
    void captureAndStoreEpisodicMemory();
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayMemories(SimulationStep currentStep);
    void updateDevelopment(SimulationStep currentStep);
    void applyDevelopmentPlasticityModulation(StructuralPlasticity* sp);
    void consolidateMemories(SimulationStep currentStep);
    void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
