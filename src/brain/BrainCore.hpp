#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class RandomGenerator;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class Synapse;
class Neuron;
class NeuralWorkingMemory;
class Dopamine;

class BrainCore {
public:
    // ===========================================
    // NEURAL DYNAMICS AND SPIKE SYSTEM CORE
    // ===========================================
    
    static void initializeNeuronDynamics();
    static void updateNeuronDynamics(const std::vector<Neuron*>& neurons,
                                     Timestamp currentTime, TimestepDuration dt);
    static void processSpikes(std::vector<Neuron*>& neurons, SimulationStep step,
                              SpikeSystem* spikeSystem);
    static void deliverSynapticInput(std::vector<Neuron*>& neurons,
                                     const DelayedSpikeEvent& event,
                                     SpikeSystem* spikeSystem);
    
    // ===========================================
    // PLASTICITY RULES (STDP AND HEBBIAN)
    // ===========================================
    
    static void applySTDP(std::vector<Synapse*>& synapses, SpikeSystem* spikeSystem,
                          float plasticityMod);
    static void applyHebbianLearning(std::vector<Synapse*>& synapses, SpikeSystem* spikeSystem);
    static void updateSynapticState(std::vector<Synapse*>& synapses, Timestamp currentTime);
    
    // ===========================================
    // STRUCTURAL PLASTICITY
    // ===========================================
    
    static void applyStructuralPlasticity(Brain* brain, RandomGenerator* rng);
    
    // ===========================================
    // NEUROMODULATION EFFECTS ON NEURONS
    // ===========================================
    
    static void applyDopamineEffects(std::vector<Neuron*>& neurons, Dopamine* dopamine);
    
    // ===========================================
    // UTILITY FUNCTIONS
    // ===========================================
    
    static void storeActiveNeuronsToWorkingMemory(NeuralWorkingMemory* workingMemory,
                                                  const std::vector<Neuron*>& neurons);
    static float getExcitationInhibitionRatio(const std::vector<Synapse*>& synapses);
};

} // namespace nlm