#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse class representing a connection between neurons
//
// This class implements real synaptic transmission with delays and short-term
// plasticity. A synapse connects a source neuron to a destination neuron and
// mediates the transmission of action potentials between them.
//
// Key biological mechanisms:
// - Synaptic delay: Spikes take time to travel along the connection
// - Short-term plasticity: Synaptic efficacy changes with activity
// - Synaptic weight: Determines signal strength
// - Spike-timing dependent plasticity (STDP): Weight changes based on spike order
// - Neuromodulation: External signals can modify synaptic properties
//
// Synaptic dynamics are essential for:
// - Information transmission between neural populations
// - Pattern formation and memory encoding
// - Computational capabilities of the brain model
//
// @note This is a fundamental component of the neural circuitry
//
// @see https://en.wikipedia.org/wiki/Synaptic_transmission
class Synapse {

class Synapse {
public:
// Create synapse with connection identifiers
//
// @param id Unique identifier for this synapse
// @param source Neuron that will emit spikes (presynaptic)
// @param destination Neuron that will receive spikes (postsynaptic)
//
// @note Synapse is initialized with default parameters:
//       - Weight: 0.0f (neutral)
//       - Delay: 1 simulation step (immediate transmission)
//       - Type: Excitatory (default for backward compatibility)
//       - Efficacy: 1.0f (maximum signal transmission)
//       - No short-term plasticity effects
Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
// Destructor
//
// @note Automatic memory management via Pimpl idiom
~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
// Get unique identifier of this synapse
//
// @return SynapseId Unique identifier for this synapse
//
// @note SynapseId is a strongly-typed identifier for safety and debugging
SynapseId getId() const;
    
// Get source neuron identifier
//
// @return NeuronId ID of the source (presynaptic) neuron
//
// @note This is the neuron that generates action potentials sent through this synapse
NeuronId getSourceNeuron() const;

    // Get destination neuron identifier
    //
    // @return NeuronId ID of the destination (postsynaptic) neuron
    //
    // @note This is the neuron that receives action potentials through this synapse
    NeuronId getDestinationNeuron() const;
    
// Get current synaptic weight
//
// @return SynapticWeight Current synaptic weight (-1.0 to 1.0)
//
// @note Positive weights indicate excitatory connections, negative indicate inhibitory
//       Weight magnitude determines signal strength
SynapticWeight getWeight() const;

    // Set synaptic weight directly
    //
    // @param weight New synaptic weight (-1.0 to 1.0)
    //
    // @warning Direct weight changes bypass plasticity rules
    //         Use step() or plasticity system for learning-based weight changes
    void setWeight(SynapticWeight weight);

    // Add delta to synaptic weight
    //
    // @param delta Weight change amount (-1.0 to 1.0)
    //
    // @note Clamped to range [-1.0, 1.0] to maintain stability
    //       Used by plasticity rules during learning
    void addToWeight(SynapticWeight delta);
    
// Get synaptic transmission delay
//
// @return Delay Transmission delay in simulation steps (typically 1-5)
//
// @note Delay affects spike timing and can create temporal dynamics
//       Critical for real-time neural processing and oscillatory activity
Delay getDelay() const;

    // Set synaptic transmission delay
    //
    // @param delay New delay in simulation steps
    //
    // @note Larger delays create temporal separation between pre- and post-synaptic
    //       activity, important for timing-dependent computations
    void setDelay(Delay delay);
    
// Get synapse type
//
// @return SynapseType Type classification (Excitatory, Inhibitory, Modulatory, etc.)
//
// @note Type determines synaptic polarity and computational role
//       Excitatory: increases post-synaptic firing probability
//       Inhibitory: decreases post-synaptic firing probability
//       Modulatory: affects plasticity and neuromodulation
//       Electrical: gap junction with immediate transmission
SynapseType getType() const;

    // Set synapse type
    //
    // @param type New synapse type
    //
    // @warning Changing synapse type affects signal transmission characteristics
    //         and may require corresponding updates to plasticity rules
    void setType(SynapseType type);
    
// Check if synapse is excitatory
//
// @return bool true if synapse type is Excitatory
//
// @note Excitatory synapses depolarize post-synaptic membrane potential
bool isExcitatory() const;

    // Check if synapse is inhibitory
    //
    // @return bool true if synapse type is Inhibitory
    //
    // @note Inhibitory synapses hyperpolarize post-synaptic membrane potential
    bool isInhibitory() const;
    
// Record presynaptic spike time
//
// @param timestamp Time of presynaptic spike
//
// @note Essential for spike-timing dependent plasticity (STDP)
//       Stores spike history for weight modulation based on timing
void recordPreSpike(Timestamp timestamp);

    // Record postsynaptic spike time
    //
    // @param timestamp Time of postsynaptic spike
    //
    // @note Used in STDP to determine synaptic weight changes
    //       Postsynaptic spikes preceding presynaptic spikes strengthen synapse
    void recordPostSpike(Timestamp timestamp);

    // Get presynaptic spike history
    //
    // @return const std::vector<Timestamp>& Reference to spike history vector
    //
    // @note Spike times are stored in chronological order for plasticity computations
    const std::vector<Timestamp>& getPreSpikeHistory() const;

    // Get postsynaptic spike history
    //
    // @return const std::vector<Timestamp>& Reference to postsynaptic spike history
    const std::vector<Timestamp>& getPostSpikeHistory() const;

    // Clear spike history
    //
    // @note Used during reset or when synapse is disabled
    //       Preserves plasticity flags and other state
    void clearHistory();
    
// Get reference to plasticity flags for this synapse
//
// @return const PlasticityFlags& Read-only reference to plasticity flags
//
// @note Plasticity flags enable/disable specific learning rules
//       (Hebbian, STDP, reward-modulated, structural)
const PlasticityFlags& getPlasticityFlags() const;

    // Get mutable reference to plasticity flags
    //
    // @return PlasticityFlags& Mutable reference to plasticity flags
    //
    // @warning Direct flag modification may bypass safety checks
    //         Use enablePlasticity() for safe rule activation
    PlasticityFlags& getPlasticityFlags();

    // Enable or disable plasticity rules for this synapse
    //
    // @param hebbian Enable/disable Hebbian learning
    // @param stdp Enable/disable spike-timing dependent plasticity
    // @param rewardModulated Enable/disable reward-modulated learning
    //
    // @note Only enabled rules will modify synaptic weight during learning
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
// Get current eligibility trace value
//
// @return float Current eligibility trace (-1.0 to 1.0)
//
// @note Eligibility trace tracks synaptic eligibility for reward-modulated
//       learning. Decay occurs automatically during reward modulation
float getEligibilityTrace() const;

    // Set eligibility trace directly
    //
    // @param trace New eligibility trace value
    //
    // @warning Typically modified by plasticity rules, not directly by user code
    void setEligibilityTrace(float trace);

    // Decay eligibility trace
    //
    // @param decayRate Decay factor (0.0 to 1.0, higher = faster decay)
    //
    // @note Used in reward-modulated learning to forget unused synaptic changes
    //       Called during reward modulation to prevent indefinite trace accumulation
    void decayEligibilityTrace(float decayRate);
    
// Get current synaptic efficacy
//
// @return float Synaptic efficacy (0.0 to 2.0)
//
// @note Efficacy represents use-dependent modulation of synaptic strength
//       Can be modified by short-term plasticity based on activity history
float getEfficacy() const;

    // Set synaptic efficacy directly
    //
    // @param efficacy New efficacy value (clamped to [0.0, 2.0])
    //
    // @note Efficacy affects synaptic weight scaling during transmission
    //       Higher values increase signal transmission strength
    void setEfficacy(float efficacy);
    
// Update synapse for one simulation step
//
// @param currentTime Current simulation time in seconds
//
// @note Processes synaptic dynamics including:
//       - Short-term plasticity decay
//       - Eligibility trace decay
//       - Efficacy modulation
//       - Weight clamping to maintain stability
//
// @todo PHASE 2: Implement real synaptic dynamics including
//              - Realistic synaptic conductance models
//              - Short-term depression/facilitation
//              - Neuromodulatory effects
//              - Stochastic spike transmission
void step(Timestamp currentTime);
    
// Reset synapse to initial state
//
// @note Resets all state variables to default values:
//       - Weight to 0.0f
//       - Spike histories cleared
//       - Eligibility trace to 0.0f
//       - Efficacy to 1.0f
//       - Last spike times set to -1.0f
//
// @note Preserves synapse identity (id, source, destination) and type
void reset();

    // Initialize synapse with random biological parameters
    //
    // @param rng Random number generator for stochastic initialization
    //
    // @note Creates biologically realistic synapse parameters based on type:
    //       - Excitatory: positive weights (0.1-0.4)
    //       - Inhibitory: negative weights (-0.1 to -0.4)
    //       - Other types: small random weights (-0.1 to 0.1)
    //
    // Also initializes:
    // - Random delay (1-5 steps)
    // - Short-term plasticity state
    // - Spike history
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
