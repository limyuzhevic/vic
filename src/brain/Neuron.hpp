#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;
// Neuron state structure for efficient storage
// 
// This structure stores all state variables for a single neuron in a compact
// form to minimize memory usage and maximize cache efficiency. The structure
// is designed for high-performance neural simulation with real-time constraints.
// 
// The neuron implements Leaky Integrate-and-Fire (LIF) dynamics, which is a
// biologically plausible model for neural computation. This approach balances
// computational efficiency with biological realism, making it suitable for
// large-scale neural network simulations.

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
    
    // Default constructor initializes neuron to resting state
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
// 
// This class implements a biologically-inspired Leaky Integrate-and-Fire (LIF)
// neuron model that is fundamental to spiking neural networks. The neuron
// receives synaptic inputs, integrates them over time, and generates spikes
// when the membrane potential reaches a threshold.
// 
// The neuron uses the Pimpl idiom to hide implementation details and improve
// compilation times while maintaining high performance through efficient
// data structures and memory access patterns.
// 
// Key features:
// - LIF dynamics with leak conductance and adaptive threshold
// - Spike-frequency adaptation for realistic firing patterns
// - Refractory period to prevent unrealistic firing rates
// - Synaptic input integration and spike generation
// - Random initialization with biological constraints
// - Event-driven spike recording for plasticity computation
// 
// This implementation is designed for large-scale neural simulations with
// emphasis on computational efficiency and biological plausibility.

class Neuron {
public:
    // Create neuron with ID
    // 
    // Args:
    //   id: Unique identifier for this neuron
    // 
    // Note: The neuron is initialized in a resting state with default parameters.
    // Use initializeRandom() to set biologically realistic parameters.
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving for performance and memory safety
    // 
    // Copy operations are disabled because the Pimpl pattern requires
    // deep copy semantics. Move operations enable efficient transfer of ownership
    // while maintaining memory safety.
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity and type management
    // 
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access and modification
    // 
    // Provides access to the complete neuron state for inspection and modification.
    // The state structure includes membrane potential, firing properties, and
    // adaptation variables that govern neural dynamics.
    
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential management
    // 
    // The membrane potential represents the electrical state of the neuron and
    // is the primary variable that determines neural activity. It integrates
    // synaptic inputs and determines when spikes are generated.
    
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Firing threshold and dynamics
    // 
    // The threshold determines when the neuron will fire a spike. When the
    // membrane potential reaches or exceeds the threshold, the neuron enters
    // an active state and generates a spike.
    
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    // 
    // The firing state tracks the neuron's activity state (Resting, Active,
    // Refractory) and is used for spike timing and plasticity computations.
    
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    // 
    // Firing rate provides a continuous measure of neural activity that can be
    // used for downstream computations or rate-based neural models.
    
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    // 
    // These parameters control the biophysical properties of the LIF neuron
    // and determine its computational characteristics.
    
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection and timing
    // 
    // These methods manage spike generation and timing, which are fundamental
    // to neural communication and plasticity.
    
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    // 
    // Advances the neuron state for one simulation time step using the
    // Leaky Integrate-and-Fire dynamics. This is the core computational
    // method that determines neural activity and spike generation.
    // 
    // Args:
    //   currentTime: Current simulation time in milliseconds
    //   dt: Time step duration in milliseconds
    // 
    // Returns:
    //   true if the neuron fired during this time step, false otherwise
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    // 
    // These methods receive synaptic inputs that affect the neuron's membrane
    // potential and determine its firing behavior.
    
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    // 
    // These methods inject current directly into the neuron from external
    // sources such as sensory input or experimental manipulation.
    
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    // 
    // Spike history records recent spikes for spike-timing-dependent plasticity
    // computations and for analyzing neural activity patterns.
    
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    // 
    // These methods manage synaptic connections that determine how neurons
    // communicate and influence each other's activity.
    
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    // 
    // These methods manage the neuron's plasticity properties, determining
    // how its connections will change in response to activity.
    
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    // 
    // Neurons belong to specific regions and populations, which organize
    // neural computation into functional modules.
    
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // 
    // This method advances the neuron state for one time step. For LIF neurons,
    // this delegates to stepLIF() with standard parameters.
    // 
    // TODO PHASE 6: Implement more sophisticated update rules for different
    // neuron models and adaptive dynamics.
    
    void step(Timestamp currentTime);
    
    // Reset to initial state
    // 
    // Resets the neuron to its initial resting state, clearing all activity,
    // adaptation, and spike history. This is useful for starting fresh
    // simulations or experimental trials.
    
    void reset();
    
    // Initialize with random parameters
    // 
    // Initializes the neuron with biologically realistic parameters using
    // random number generation. This sets the neuron's intrinsic properties
    // such as threshold, resting potential, and refractory period according
    // to typical biological ranges with small variations.
    // 
    // Args:
    //   rng: Random number generator for stochastic parameter initialization
    
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};
} // namespace nlm
