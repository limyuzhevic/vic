#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
// This is the fundamental computational unit of the NLM brain
// Each neuron integrates incoming synaptic inputs and fires when threshold is reached
// 
// Key biological properties modeled:
// - Membrane potential integration with exponential decay
// - Refractory period after each spike
// - Synaptic input integration (excitatory/inhibitory/modulatory)
// - Spike-frequency adaptation to prevent excessive firing
// - Plasticity mechanisms (STDP, Hebbian, reward-modulated)
class Neuron {
public:
    // Create neuron with ID
    // @param id: Unique identifier for this neuron
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity - unique identifier for this neuron
    // @return NeuronId: Unique ID of this neuron
    NeuronId getId() const;
    
    // Type - determines neuron's role in network
    // @return NeuronType: Type of this neuron (Excitatory, Inhibitory, Sensory, etc.)
    NeuronType getType() const;
    
    // Set neuron type
    // @param type: New neuron type
    void setType(NeuronType type);
    
    // State access - complete neuron state including membrane potential,
    // firing state, refractory period, etc.
    // @return const NeuronState&: Complete state information (read-only)
    const NeuronState& getState() const;
    
    // State access - mutable access to neuron state
    // @return NeuronState&: Complete state information (mutable)
    NeuronState& getState();
    
    // Membrane potential - the electrical potential across neuron membrane
    // @return MembranePotential: Current membrane potential in millivolts (mV)
    // @param potential: New membrane potential in millivolts (mV)
    // @param delta: Change to add to current potential in millivolts (mV)
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold - membrane potential required to trigger action potential
    // @return MembranePotential: Firing threshold in millivolts (mV)
    // @param threshold: New firing threshold in millivolts (mV)
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state - current physiological state of the neuron
    // @return bool: True if neuron is currently in refractory period
    bool isFiring() const;
    bool isRefractory() const;
    
    // Set firing state - for testing and debugging
    // @param state: New firing state
    void setFiringState(FiringState state);
    
    // Refractory period control
    // @param steps: Number of simulation steps for refractory period
    void setRefractoryPeriod(uint32_t steps);
    
    // Internal refractory period management - called automatically during simulation
    void decrementRefractory();
    
    // Firing rate - average number of spikes per second
    // @return FiringRate: Current firing rate in Hz
    // @param rate: New firing rate in Hz
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters - membrane time constant and conductance
    // @return MembranePotential: Leak conductance in nanoSiemens (nS)
    // @param conductance: New leak conductance in nanoSiemens (nS)
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    
    // Refractory period - how long neuron cannot fire after spike
    // @return uint32_t: Refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    
    // Membrane potentials - resting and reset values
    // @param potential: New resting potential in millivolts (mV)
    // @param potential: New reset potential in millivolts (mV)
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    
    // Spike detection - check if neuron should fire in next step
    // @return bool: True if neuron has reached firing threshold
    bool checkThreshold() const;
    
    // Last spike time - when neuron last fired
    // @return float: Timestamp of last spike, or -1 if never fired
    float getLastSpikeTime() const;
    
    // LIF step function - compute one integration step
    // This is the core simulation function that implements:
    // 1. Refractory period handling
    // 2. Membrane potential integration (dV/dt = (V_rest - V)/tau + I/C)
    // 3. Spike detection and reset
    // 4. Adaptation mechanism
    // @param currentTime: Current simulation timestamp
    // @param dt: Time step duration in milliseconds
    // @return bool: True if neuron fired during this step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials from synapses)
    // @param amplitude: Input current in millivolts (mV)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources like sensory input)
    // @param current: Injected current in millivolts (mV)
    // @return MembranePotential: Total current accumulated this step
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history - recent spikes for STDP and learning
    // @param timestamp: Time when spike occurred
    // @return const std::vector<Timestamp>&: History of spike times
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management - connections to other neurons
    // @param handle: Handle to incoming or outgoing synapse
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state - controls which plasticity rules apply
    // @return const PlasticityFlags&: Flags for Hebbian, STDP, reward modulation
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable specific plasticity mechanisms
    // @param hebbian: Enable Hebbian plasticity
    // @param stdp: Enable spike-timing dependent plasticity
    // @param rewardModulated: Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership - for network organization
    // @param region: ID of neural region
    // @param population: ID of neuron population
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    // @param currentTime: Current simulation timestamp
    void step(Timestamp currentTime);
    
    // Reset to initial state
    // Clears all state and prepares neuron for new simulation
    void reset();
    
    // Initialize with random parameters
    // Sets biologically realistic parameters for new neuron
    // @param rng: Random number generator for stochastic initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuron factory function - creates properly initialized neuron
// @param id: Neuron ID for the new neuron
// @param type: Neuron type (default: Internal)
Neuron* createNeuron(NeuronId id, NeuronType type = NeuronType::Internal);
