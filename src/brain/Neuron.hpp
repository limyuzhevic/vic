#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with:
// - Membrane potential integration
// - Spike threshold detection
// - Refractory period management
// - Spike-frequency adaptation
// - Synaptic input processing
// - Plasticity state management

class Neuron {
public:
    // Create neuron with unique ID
    // @param id: Unique identifier for the neuron
    explicit Neuron(NeuronId id);
    
    // Destructor
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    // Move constructor with exception safety
    Neuron(Neuron&&) noexcept;
    // Move assignment operator with self-assignment protection
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity and identification
    // @return NeuronId: Unique identifier of this neuron
    NeuronId getId() const;
    
    // Neuron type and classification
    // @return NeuronType: Biological/functional type of neuron
    NeuronType getType() const;
    // Set neuron type (e.g., excitatory, inhibitory, sensory, motor)
    // @param type: Neuron type to set
    void setType(NeuronType type);
    
    // State access and modification
    // @return const reference to neuron state for read-only access
    const NeuronState& getState() const;
    // @return non-const reference to neuron state for modification
    NeuronState& getState();
    
    // Membrane potential management
    // @return Current membrane potential in mV
    MembranePotential getMembranePotential() const;
    // Set absolute membrane potential (use with caution)
    // @param potential: Desired membrane potential in mV
    void setMembranePotential(MembranePotential potential);
    // Add delta to membrane potential (use for synaptic inputs)
    // @param delta: Change in membrane potential in mV
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold parameters
    // @return Firing threshold in mV (typically -55mV)
    MembranePotential getThreshold() const;
    // Set firing threshold (spike occurs when V >= threshold)
    // @param threshold: New threshold value in mV
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    // @return true if neuron is in active firing state
    bool isFiring() const;
    // @return true if neuron is in refractory period
    bool isRefractory() const;
    // Set firing state (use enum values: Resting, Active, Refractory)
    // @param state: New firing state
    void setFiringState(FiringState state);
    // Configure refractory period length
    // @param steps: Number of simulation steps for refractory period
    void setRefractoryPeriod(uint32_t steps);
    // Decrement refractory counter (called automatically each step)
    void decrementRefractory();
    
    // Firing rate (for rate-based computation and analysis)
    // @return Current firing rate in Hz
    FiringRate getFiringRate() const;
    // Set firing rate (may be used for rate-based models)
    // @param rate: Desired firing rate in Hz
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    // Set membrane leak conductance (controls decay toward resting potential)
    // @param conductance: Leak conductance in nS (typically 10.0f)
    void setLeakConductance(MembranePotential conductance);
    // Get current leak conductance
    // @return Leak conductance in nS
    MembranePotential getLeakConductance() const;
    // Get refractory period in steps
    // @return Refractory period duration in simulation steps
    uint32_t getRefractoryPeriod() const;
    // Set resting potential (baseline membrane potential)
    // @param potential: Resting potential in mV (typically -70mV)
    void setRestingPotential(MembranePotential potential);
    // Get current resting potential
    // @return Resting potential in mV
    MembranePotential getRestingPotential() const;
    // Set reset potential (membrane potential after spike)
    // @param potential: Reset potential in mV (typically -70mV)
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and timing
    // Check if membrane potential exceeds firing threshold
    // @return true if neuron should fire spike this step
    bool checkThreshold() const;
    // Get timestamp of last spike
    // @return Last spike timestamp in ms (-1 if never spiked)
    float getLastSpikeTime() const;
    
    // LIF step function - advances neuron state by one timestep
    // Implements: dV/dt = (V_rest - V)/tau + I/C
    // @param currentTime: Current simulation time in ms
    // @param dt: Timestep duration in ms
    // @return true if neuron fired spike this step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    // Add excitatory synaptic input (depolarizes membrane)
    // @param amplitude: Synaptic input strength in mV (positive)
    void receiveExcitatoryInput(MembranePotential amplitude);
    // Add inhibitory synaptic input (hyperpolarizes membrane)
    // @param amplitude: Synaptic input strength in mV (positive, subtracts from V)
    void receiveInhibitoryInput(MembranePotential amplitude);
    // Add modulatory input (affects plasticity, not directly integrated)
    // @param amplitude: Modulatory input strength (typically smaller scale)
    void receiveModulatoryInput(MembranePotential amplitude);
    // Get total current input accumulated this step
    // @return Total current input in mV
    MembranePotential getTotalCurrent() const;
    // Clear accumulated current (called at end of simulation step)
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    // Record a spike event for plasticity calculations
    // @param timestamp: Time when spike occurred
    void recordSpike(Timestamp timestamp);
    // Get recent spike history for learning rules
    // @return Vector of spike timestamps (most recent first)
    const std::vector<Timestamp>& getSpikeHistory() const;
    // Clear spike history (used for reset)
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing connections)
    // Register incoming synapse for spike propagation
    // @param handle: Synapse identifier
    void addIncomingSynapse(SynapseHandle handle);
    // Register outgoing synapse for spike propagation
    // @param handle: Synapse identifier
    void addOutgoingSynapse(SynapseHandle handle);
    // Get list of incoming synapses
    // @return Vector of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    // Get list of outgoing synapses
    // @return Vector of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state management
    // Get plasticity flags for learning rules
    // @return Const reference to plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    // Get non-const reference to plasticity flags
    // @return Non-const reference to plasticity flags
    PlasticityFlags& getPlasticityFlags();
    // Enable specific plasticity rules
    // @param hebbian: Enable Hebbian learning
    // @param stdp: Enable Spike-Timing-Dependent Plasticity
    // @param rewardModulated: Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    // Set region identifier (anatomical grouping)
    // @param region: Region identifier
    void setRegionId(RegionId region);
    // Get region identifier
    // @return Region identifier
    RegionId getRegionId() const;
    // Set population identifier (functional grouping)
    // @param population: Population identifier
    void setPopulationId(PopulationId population);
    // Get population identifier
    // @return Population identifier
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // Default implementation uses standard LIF timestep
    // TODO PHASE 2: Implement optimized integrate-and-fire dynamics
    // @param currentTime: Current simulation time
    void step(Timestamp currentTime);
    
    // Reset to initial state
    // Resets membrane potential, firing state, and spike history
    void reset();
    
    // Initialize with random parameters
    // Sets biologically plausible initial conditions:
    // - Membrane potential near resting potential
    // - Random threshold variations
    // - Random refractory periods
    // @param rng: Random number generator for initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    // PIMPL idiom for implementation hiding
    // Neural implementation details are stored here
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
