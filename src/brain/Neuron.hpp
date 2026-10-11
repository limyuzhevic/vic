#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron: A single spiking neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
//
// This is the fundamental computational unit of the NLM artificial brain.
// Each neuron integrates incoming synaptic inputs and generates action potentials
// (spikes) when its membrane potential reaches a threshold. Neurons communicate
// through synapses and are subject to various plasticity mechanisms.
//
// Key features:
// - Real biophysical dynamics with LIF neuron model
// - Event-driven spike propagation with synaptic delays
// - Multiple input types (excitatory, inhibitory, modulatory)
// - Synaptic connectivity management
// - Plasticity (STDP, Hebbian, reward-modulated)
// - Integration with brain regions and populations
// - Spike history recording for learning
//
// The neuron operates as part of a larger neural network, contributing to
// collective intelligence through coordinated activity and adaptive learning.
class Neuron {
public:
    /// @brief Constructs a new neuron with a unique identifier.
    ///
    /// @param id Unique identifier for this neuron (must be valid NeuronId)
    explicit Neuron(NeuronId id);
    
    /// @brief Destructor cleans up neuron resources and spike history.
    ~Neuron();
    
    /// @brief Disables copying to prevent accidental duplication.
    Neuron(const Neuron&) = delete;
    
    /// @brief Disables assignment to prevent accidental copying.
    Neuron& operator=(const Neuron&) = delete;
    
    /// @brief Enables move construction for efficient resource transfer.
    Neuron(Neuron&&) noexcept;
    
    /// @brief Enables move assignment for efficient resource transfer.
    Neuron& operator=(Neuron&&) noexcept;
    
    /// @brief Returns the neuron's unique identifier.
    ///
    /// @return The NeuronId that uniquely identifies this neuron
    NeuronId getId() const;
    
    /// @brief Returns the neuron's functional type.
    ///
    /// @return The NeuronType (Sensory, Motor, Internal, etc.)
    NeuronType getType() const;
    
    /// @brief Sets the neuron's functional type.
    ///
    /// @param type The new NeuronType for this neuron
    void setType(NeuronType type);
    
    /// @brief Returns a const reference to the neuron's current state.
    ///
    /// @return Const reference to the NeuronState structure
    const NeuronState& getState() const;
    
    /// @brief Returns a mutable reference to the neuron's current state.
    ///
    /// @return Mutable reference to the NeuronState structure
    NeuronState& getState();
    
    /// @brief Returns the current membrane potential.
    ///
    /// @return Current membrane potential in millivolts (mV)
    MembranePotential getMembranePotential() const;
    
    /// @brief Sets the membrane potential directly.
    ///
    /// @param potential New membrane potential in millivolts (mV)
    void setMembranePotential(MembranePotential potential);
    
    /// @brief Adds a value to the membrane potential.
    ///
    /// @param delta Value to add to membrane potential (in millivolts, mV)
    void addToMembranePotential(MembranePotential delta);
    
    /// @brief Returns the neuron's firing threshold.
    ///
    /// @return Threshold potential in millivolts (mV)
    MembranePotential getThreshold() const;
    
    /// @brief Sets the neuron's firing threshold.
    ///
    /// @param threshold New threshold potential in millivolts (mV)
    void setThreshold(MembranePotential threshold);
    
    /// @brief Checks if the neuron is currently firing.
    ///
    /// @return true if neuron is in active firing state, false otherwise
    bool isFiring() const;
    
    /// @brief Checks if the neuron is in refractory period.
    ///
    /// @return true if neuron cannot fire (refractory period > 0), false otherwise
    bool isRefractory() const;
    
    /// @brief Sets the neuron's firing state.
    ///
    /// @param state The new firing state (Resting, Active, Refractory)
    void setFiringState(FiringState state);
    
    /// @brief Sets the neuron's refractory period duration.
    ///
    /// @param steps Number of simulation steps the neuron should remain refractory
    void setRefractoryPeriod(uint32_t steps);
    
    /// @brief Decrements the remaining refractory period.
    ///
    /// @details This method is called during each simulation step to track
    /// the progress of the refractory period. When refractoryRemaining reaches 0,
    /// the neuron can fire again.
    void decrementRefractory();
    
    /// @brief Returns the neuron's current firing rate.
    ///
    /// @return Firing rate in Hertz (Hz)
    FiringRate getFiringRate() const;
    
    /// @brief Sets the neuron's firing rate.
    ///
    /// @param rate New firing rate in Hertz (Hz)
    void setFiringRate(FiringRate rate);
    
    /// @brief Sets the neuron's leak conductance.
    ///
    /// @param conductance New leak conductance in nanoSiemens (nS)
    void setLeakConductance(MembranePotential conductance);
    
    /// @brief Returns the neuron's leak conductance.
    ///
    /// @return Leak conductance in nanoSiemens (nS)
    MembranePotential getLeakConductance() const;
    
    /// @brief Returns the neuron's refractory period duration.
    ///
    /// @return Refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    
    /// @brief Sets the neuron's resting potential.
    ///
    /// @param potential New resting potential in millivolts (mV)
    void setRestingPotential(MembranePotential potential);
    
    /// @brief Returns the neuron's resting potential.
    ///
    /// @return Resting potential in millivolts (mV)
    MembranePotential getRestingPotential() const;
    
    /// @brief Sets the neuron's reset potential.
    ///
    /// @param potential New reset potential in millivolts (mV)
    void setResetPotential(MembranePotential potential);
    
    /// @brief Returns the neuron's reset potential.
    ///
    /// @return Reset potential in millivolts (mV)
    MembranePotential getResetPotential() const;
    
    /// @brief Checks if the neuron's membrane potential exceeds threshold.
    ///
    /// @return true if neuron should fire (membrane potential >= threshold), false otherwise
    bool checkThreshold() const;
    
    /// @brief Returns the timestamp of the neuron's last spike.
    ///
    /// @return Last spike time (0.0f if never fired)
    float getLastSpikeTime() const;
    
    /// @brief Performs one step of LIF dynamics integration.
    ///
    /// This is the core LIF neuron computation that updates membrane potential
    /// based on inputs, leak, and synaptic currents. Returns true if the neuron
    /// fired during this step.
    ///
    /// @param currentTime Current simulation time in seconds
    /// @param dt Time step duration in seconds
    /// @return true if neuron fired during this step, false otherwise
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /// @brief Receives excitatory synaptic input.
    ///
    /// This input increases the neuron's membrane potential, bringing it
    /// closer to firing threshold. Represents excitatory postsynaptic potentials (EPSPs).
    ///
    /// @param amplitude Amplitude of excitatory input (in millivolts, mV)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /// @brief Receives inhibitory synaptic input.
    ///
    /// This input decreases the neuron's membrane potential, moving it away
    /// from firing threshold. Represents inhibitory postsynaptic potentials (IPSPs).
    ///
    /// @param amplitude Magnitude of inhibitory input (in millivolts, mV)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /// @brief Receives modulatory synaptic input.
    ///
    /// This input affects neuronal properties but not immediately integrated
    /// into membrane potential. Used for neuromodulation (e.g., dopamine, acetylcholine).
    ///
    /// @param amplitude Amplitude of modulatory input (in millivolts, mV)
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /// @brief Injects external current directly into the neuron.
    ///
    /// This method simulates external input sources like sensory input
    /// or stimulation, adding current to the neuron's total input.
    ///
    /// @param current Amount of current to inject (in nanoAmperes, nA)
    void injectCurrent(MembranePotential current);
    
    /// @brief Returns the total current injected into the neuron this step.
    ///
    /// @return Total current in nanoAmperes (nA)
    MembranePotential getTotalCurrent() const;
    
    /// @brief Clears the total current accumulated for this simulation step.
    ///
    /// This is called at the end of each simulation step to prepare for
    /// the next step's input accumulation.
    void clearTotalCurrent();
    
    /// @brief Records a spike event in the neuron's history.
    ///
    /// Spike history is used for STDP learning rules and spike timing analysis.
    /// Only recent spikes are kept to save memory.
    ///
    /// @param timestamp Time at which the spike occurred
    void recordSpike(Timestamp timestamp);
    
    /// @brief Returns the neuron's recent spike history.
    ///
    /// @return Const reference to vector of spike timestamps
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /// @brief Clears all recorded spike history.
    void clearSpikeHistory();
    
    /// @brief Adds an incoming synapse to this neuron.
    ///
    /// This updates the neuron's synaptic connectivity, allowing it to
    /// receive signals from other neurons.
    ///
    /// @param handle Handle to the incoming synapse
    void addIncomingSynapse(SynapseHandle handle);
    
    /// @brief Adds an outgoing synapse from this neuron.
    ///
    /// This updates the neuron's synaptic connectivity, allowing it to
    /// send signals to other neurons.
    ///
    /// @param handle Handle to the outgoing synapse
    void addOutgoingSynapse(SynapseHandle handle);
    
    /// @brief Returns all incoming synapses to this neuron.
    ///
    /// @return Const reference to vector of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /// @brief Returns all outgoing synapses from this neuron.
    ///
    /// @return Const reference to vector of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /// @brief Returns the neuron's plasticity configuration flags.
    ///
    /// These flags determine which plasticity mechanisms apply to this neuron,
    /// such as STDP, Hebbian learning, or reward-modulated plasticity.
    ///
    /// @return Const reference to plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    
    /// @brief Returns the neuron's plasticity configuration flags (mutable version).
    ///
    /// @return Mutable reference to plasticity flags
    PlasticityFlags& getPlasticityFlags();
    
    /// @brief Enables or disables plasticity mechanisms for this neuron.
    ///
    /// @param hebbian Enable or disable Hebbian plasticity
    /// @param stdp Enable or disable STDP plasticity
    /// @param rewardModulated Enable or disable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /// @brief Sets the neuron's region identifier.
    ///
    /// Regions group neurons with similar functional properties.
    ///
    /// @param region Region identifier
    void setRegionId(RegionId region);
    
    /// @brief Returns the neuron's region identifier.
    ///
    /// @return Region identifier
    RegionId getRegionId() const;
    
    /// @brief Sets the neuron's population identifier.
    ///
    /// Populations are groups of neurons within regions that share properties.
    ///
    /// @param population Population identifier
    void setPopulationId(PopulationId population);
    
    /// @brief Returns the neuron's population identifier.
    ///
    /// @return Population identifier
    PopulationId getPopulationId() const;
    
    /// @brief Updates the neuron for one simulation step (placeholder implementation).
    ///
    /// TODO: Implement real integrate-and-fire dynamics using stepLIF method.
    ///
    /// @param currentTime Current simulation time in seconds
    void step(Timestamp currentTime);
    
    /// @brief Resets the neuron to its initial state.
    ///
    /// This method resets membrane potential, firing state, spike history,
    /// and other state variables to their initial values.
    void reset();
    
    /// @brief Initializes the neuron with random biological parameters.
    ///
    /// This method sets realistic random values for neuron properties
    /// based on biological constraints and distributions.
    ///
    /// @param rng Random number generator to use for parameter initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    /// @brief Private implementation details for the PImpl pattern.
    ///
    /// This separates the Neuron class interface from its implementation
    /// to reduce compilation dependencies and improve encapsulation.
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
