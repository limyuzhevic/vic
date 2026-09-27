#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// NeuronState: Complete state representation for a single neuron
// Efficient struct for storing all LIF neuron state variables to minimize memory footprint

/**
 * @struct NeuronState
 * @brief Complete state representation for a single neuron implementing LIF dynamics
 * 
 * This structure contains all state variables needed for Leaky Integrate-and-Fire (LIF) neuron
 * dynamics. It is designed for efficient memory access patterns and cache locality.
 * 
 * State Components:
 * - Membrane potentials: representing electrical state of neuron
 * - Firing properties: threshold, reset, refractory period
 * - Dynamics variables: adaptation, spike timing
 * - Network connections: synaptic inputs
 * 
 * Default Values:
 * - Resting potential: -70 mV (typical for cortical neurons)
 * - Threshold: -55 mV (typical for action potential initiation)
 * - Refractory period: 5 simulation steps
 * - Leak conductance: 10 nS (determines membrane time constant)
 * 
 * @note This structure is used internally by Neuron class and can be exposed for optimization
 * @see Neuron
 * @see NeuronType
 * @ingroup core
 */

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
    
    /**
     * @brief Default constructor initializes all state variables to physiological defaults
     * 
     * Sets initial state consistent with typical cortical neuron properties:
     * - Resting near -70 mV
     * - Ready to fire at -55 mV
     * - Short refractory period to limit firing rate
     * - Moderate leak for appropriate time constant
     */
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

// Neuron: Single computational neuron implementing Leaky Integrate-and-Fire dynamics
// Core processing unit for the NLM brain, capable of complex spiking patterns and learning

/**
 * @class Neuron
 * @brief Single computational neuron implementing Leaky Integrate-and-Fire dynamics
 * 
 * The Neuron class represents the fundamental processing unit of the NLM brain. It implements
 * LIF dynamics where membrane potential integrates incoming inputs with exponential decay.
 * When threshold is reached, the neuron emits a spike, resetting membrane potential.
 * 
 * Key Features:
 * - Biophysical LIF dynamics with realistic membrane properties
 * - Support for multiple input types (excitatory, inhibitory, modulatory)
 * - Plasticity support for synaptic strengthening/weakening
 * - Population and region membership for large-scale network organization
 * - Spike history tracking for STDP learning rules
 * - Refractory period to limit firing rates and ensure biological realism
 * 
 * Biological Plausibility:
 * - Uses membrane potentials measured in mV (consistent with biology)
 * - Implements spike-frequency adaptation via adaptation variable
 * - Includes refractory period to prevent unrealistic firing
 * - Supports ion channel-like properties (leak conductance)
 * 
 * Performance Characteristics:
 * - Computationally lightweight: O(1) per step with constant memory
 * - Event-driven: Only updates on relevant inputs or time steps
 * - Memory efficient: Uses struct for state and handles for connections
 * 
 * @note Phase 2 TODO: Implement complete integrate-and-fire dynamics in step() method
 * @see NeuronState
 * @see Synapse
 * @see PlasticityFlags
 * @ingroup core
 */

class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
