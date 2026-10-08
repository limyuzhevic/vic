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
//
// This class implements a biologically-inspired neuron model using the
// Leaky Integrate-and-Fire (LIF) dynamics framework. Each neuron maintains
// its own state including membrane potential, firing thresholds, and spike
// history. Neurons receive synaptic inputs, integrate them over time, and
// generate action potentials when threshold is reached.
//
// The neuron model supports multiple neuron types (excitatory, inhibitory,
// modulatory, sensory, motor, internal) and integrates with the plasticity
// system for learning and adaptation.
//
// @note This is a core component of the NLM brain architecture and forms the
//       basic processing unit of the neural network.
//
// @warning Memory management is handled via Pimpl idiom. Direct deletion
//          of pImpl is prohibited to prevent memory leaks.
//
// @see https://en.wikipedia.org/wiki/Integrate-and-fire
//
class Neuron {
public:
    // Create neuron with unique identifier
//
// @param id Unique identifier for this neuron (must be valid NeuronId)
//
// @note Neuron is constructed with default physiological parameters:
//       - Membrane potential near resting potential (-70mV)
//       - Firing threshold at typical cortical values (-55mV)
//       - Default refractory period of 5 simulation steps
Neuron(NeuronId id);
    
// Destructor
//
// @note Memory management is automatic due to Pimpl idiom
~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
// Get unique identifier of this neuron
//
// @return NeuronId Unique identifier for this neuron
//
// @note NeuronId is a strongly-typed identifier for safety and debugging
NeuronId getId() const;
    
// Get neuron type (functional classification)
//
// @return NeuronType Type of neuron (Excitatory, Inhibitory, Modulatory, etc.)
//
// @note Neuron type influences synaptic connections and computational role
NeuronType getType() const;

    // Set neuron type
    //
    // @param type New neuron type to assign
    //
    // @note Changing neuron type affects synaptic behavior and plasticity
    void setType(NeuronType type);
    
// Get complete neuron state snapshot
//
// @return const NeuronState& Reference to neuron state (read-only access)
//
// @note State includes membrane potential, firing rate, refractory status, etc.
const NeuronState& getState() const;

    // Get mutable reference to neuron state for direct modification
    //
    // @return NeuronState& Mutable reference to neuron state
    //
    // @warning Direct modification bypasses safety checks - use with caution
    NeuronState& getState();
    
// Get current membrane potential
//
// @return MembranePotential Current membrane potential in millivolts (mV)
//
// @note Typical resting potential is -70mV. Depolarization brings potential closer
//       to threshold (-55mV), hyperpolarization moves away from threshold.
MembranePotential getMembranePotential() const;

    // Set membrane potential directly
    //
    // @param potential New membrane potential in mV
    //
    // @warning Direct setting may bypass biophysical constraints - use setCurrent
    //          or synaptic inputs for normal operation
    void setMembranePotential(MembranePotential potential);

    // Inject current to change membrane potential
    //
    // @param delta Change in membrane potential in mV
    //
    // @note This method is used by synaptic inputs and external current sources
    void addToMembranePotential(MembranePotential delta);
    
// Get firing threshold
//
// @return MembranePotential Firing threshold in mV (typically -55mV)
//
// @note Spike is generated when membrane potential >= threshold
MembranePotential getThreshold() const;

    // Set firing threshold
    //
    // @param threshold New firing threshold in mV
    //
    // @note Threshold affects neuron excitability and firing dynamics
    void setThreshold(MembranePotential threshold);
    
// Check if neuron is currently firing (active)
//
// @return bool true if neuron is in Active state, false otherwise
//
// @note Neuron enters Active state when membrane potential reaches threshold
//       and remains until spike generation or reaching threshold again
bool isFiring() const;

    // Check if neuron is in refractory period after firing
    //
    // @return bool true if neuron is in Refractory state, false otherwise
    //
    // @note During refractory period, neuron cannot fire new spikes
    bool isRefractory() const;

    // Set firing state directly
    //
    // @param state New firing state (Resting, Active, Refractory, Inhibited)
    //
    // @warning Direct state changes may bypass spike generation logic
    //          - use stepLIF() for normal spike generation
    void setFiringState(FiringState state);

    // Set refractory period duration
    //
    // @param steps Number of simulation steps for refractory period
    //
    // @note Refractory period prevents excessive firing rates
    void setRefractoryPeriod(uint32_t steps);

    // Decrement refractory counter
    //
    // @note Called during simulation step to progress through refractory period
    //       automatically exits Refractory state when counter reaches zero
    void decrementRefractory();
    
// Get current firing rate
//
// @return FiringRate Current firing rate in Hertz (Hz)
//
// @note Firing rate is maintained separately from spike detection for rate-based
//       computation and monitoring purposes
FiringRate getFiringRate() const;

    // Set firing rate directly
    //
    // @param rate New firing rate in Hz
    //
    // @note Useful for rate-based plasticity models and control signals
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
