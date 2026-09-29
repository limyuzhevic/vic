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
// Uses pimpl idiom for efficient memory management and implementation hiding
class Neuron {
public:
    // Create neuron with ID
    // @param id Unique identifier for this neuron
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving - neurons are lightweight objects
    // that can be moved but not copied to avoid double ownership of pImpl
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity access
    // @return Unique identifier for this neuron
    NeuronId getId() const;
    
    // Neuron type classification
    // @return Type of neuron (Internal, Sensory, Motor, etc.)
    NeuronType getType() const;
    // Set neuron type
    // @param type New neuron type classification
    void setType(NeuronType type);
    
    // State access
    // @return Constant reference to neuron state structure
    const NeuronState& getState() const;
    // @return Mutable reference to neuron state structure (for advanced usage)
    NeuronState& getState();
    
    // Membrane potential management
    // @return Current membrane potential in millivolts (mV)
    MembranePotential getMembranePotential() const;
    // Set absolute membrane potential
    // @param potential New membrane potential value (mV)
    void setMembranePotential(MembranePotential potential);
    // Add delta to current membrane potential
    // @param delta Change in membrane potential (mV)
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold voltage
    // @return Current firing threshold in millivolts (mV)
    MembranePotential getThreshold() const;
    // Set firing threshold
    // @param threshold New threshold voltage (mV)
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    // @return true if neuron is currently in firing state
    bool isFiring() const;
    // @return true if neuron is in refractory period (cannot fire)
    bool isRefractory() const;
    // Set firing state manually (advanced usage)
    // @param state New firing state
    void setFiringState(FiringState state);
    // Set refractory period length
    // @param steps Number of simulation steps neuron cannot fire
    void setRefractoryPeriod(uint32_t steps);
    // Decrement refractory counter (called automatically by stepLIF)
    void decrementRefractory();
    
    // Firing rate (for rate-based computation and analysis)
    // @return Current firing rate in Hertz (spikes per second)
    FiringRate getFiringRate() const;
    // Set firing rate (advanced usage)
    // @param rate New firing rate (Hz)
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    // Set leak conductance ( Controls passive membrane current flow)
    // @param conductance Leak conductance in nanoSiemens (nS)
    void setLeakConductance(MembranePotential conductance);
    // @return Current leak conductance in nanoSiemens (nS)
    MembranePotential getLeakConductance() const;
    // @return Current refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    // Set resting potential (baseline membrane voltage)
    // @param potential Resting potential in millivolts (mV)
    void setRestingPotential(MembranePotential potential);
    // @return Resting potential in millivolts (mV)
    MembranePotential getRestingPotential() const;
    // Set reset potential (post-spike reset value)
    // @param potential Reset potential in millivolts (mV)
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and timing
    // @return true if neuron has crossed firing threshold
    bool checkThreshold() const;
    // @return Time of last spike in seconds (-1 if no spike yet)
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    // Performs complete Leaky Integrate-and-Fire computation for one timestep
    // @param currentTime Simulation time in seconds
    // @param dt Timestep duration in seconds
    // @return true if neuron crossed threshold and fired during this step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    // Add excitatory synaptic input
    // @param amplitude Synaptic conductance * reversal potential contribution (mV)
    void receiveExcitatoryInput(MembranePotential amplitude);
    // Add inhibitory synaptic input
    // @param amplitude Synaptic conductance * reversal potential contribution (mV)
    void receiveInhibitoryInput(MembranePotential amplitude);
    // Add modulatory input (affects plasticity, not directly integrated)
    // @param amplitude Neuromodulatory signal strength (e.g., dopamine, acetylcholine)
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    // @return Current membrane potential contribution (mV)
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Direct current injection for external sources
    // @param current External current input in millivolts (mV)
    void injectCurrent(MembranePotential current);
    
    // Spike history (recent spikes for STDP and analysis)
    // Record a spike occurrence with timestamp
    // @param timestamp Simulation time when spike occurred
    void recordSpike(Timestamp timestamp);
    // @return Constant reference to recent spike timestamps for plasticity calculations
    const std::vector<Timestamp>& getSpikeHistory() const;
    // Clear all spike history
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing connections)
    // Add incoming synapse connection (from other neuron)
    // @param handle Handle to the synaptic connection
    void addIncomingSynapse(SynapseHandle handle);
    // Add outgoing synapse connection (to other neuron)
    // @param handle Handle to the synaptic connection
    void addOutgoingSynapse(SynapseHandle handle);
    // @return List of incoming synapse handles for this neuron
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    // @return List of outgoing synapse handles for this neuron
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state management
    // @return Constant reference to plasticity flags for read-only access
    const PlasticityFlags& getPlasticityFlags() const;
    // @return Mutable reference to plasticity flags for advanced plasticity control
    PlasticityFlags& getPlasticityFlags();
    // Enable/disable specific plasticity rules
    // @param hebbian Enable Hebbian potentiation
    // @param stdp Enable spike-timing-dependent plasticity
    // @param rewardModulated Enable reward-modulated learning
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    // @param region Region identifier for this neuron
    void setRegionId(RegionId region);
    // @return Region identifier for this neuron
    RegionId getRegionId() const;
    // @param population Population identifier for this neuron
    void setPopulationId(PopulationId population);
    // @return Population identifier for this neuron
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // Performs Leaky Integrate-and-Fire computation for standard timestep (1ms)
    // @param currentTime Simulation time in seconds
    // @return true if neuron crossed threshold and fired during this step
    bool step(Timestamp currentTime);
    
    // Reset to initial state (resting state)
    void reset();
    
    // Initialize neuron with biologically realistic random parameters
    // Uses Gaussian distributions around typical biological values
    // @param rng Random number generator for parameter sampling
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
