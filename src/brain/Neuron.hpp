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
class Neuron {
public:
    /**
     * @brief Construct a new Neuron object with a unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron object and clean up resources
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type (excitatory, inhibitory, sensory, motor, etc.)
     * 
     * @return NeuronType Neuron type enumeration
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state
     * 
     * @return const NeuronState& Constant reference to neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the complete neuron state (mutable version)
     * 
     * @return NeuronState& Mutable reference to neuron state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a current-based change to the membrane potential
     * 
     * @param delta Change in membrane potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold potential
     * 
     * @return MembranePotential Threshold potential (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold potential
     * 
     * @param threshold New threshold potential (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing
     * 
     * @return bool True if neuron is firing, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return bool True if neuron is refractory, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period length
     * 
     * @param steps Number of steps for refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the remaining refractory period by one step
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate (for rate-based computation)
     * 
     * @param rate New firing rate (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance for LIF dynamics
     * 
     * @param conductance Leak conductance (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period length
     * 
     * @return uint32_t Refractory period (steps)
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting membrane potential
     * 
     * @param potential Resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting membrane potential
     * 
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (value after spike)
     * 
     * @param potential Reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * 
     * @return MembranePotential Reset potential (mV)
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if membrane potential exceeds firing threshold
     * 
     * @return bool True if neuron should fire, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Last spike time (-1.0 if no spikes yet)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step
     * 
     * @param currentTime Current simulation time
     * @param dt Time step duration
     * @return bool True if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current into neuron
     * 
     * @param current External current (mV)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total injected current
     * 
     * @return MembranePotential Total injected current (mV)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear all injected current
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike at the given timestamp for plasticity
     * 
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the spike history timestamps
     * 
     * @return const std::vector<Timestamp>& Spike history
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear all spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse connection
     * 
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * 
     * @return const PlasticityFlags& Plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get the plasticity flags for this neuron (mutable)
     * 
     * @return PlasticityFlags& Mutable plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID this neuron belongs to
     * 
     * @param region Region ID
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID this neuron belongs to
     * 
     * @return RegionId Region ID
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID this neuron belongs to
     * 
     * @param population Population ID
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID this neuron belongs to
     * 
     * @return PopulationId Population ID
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step (legacy LIF implementation)
     * 
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * 
     * @param rng Random number generator for initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
    // Neural adaptation configuration
    /**
     * @brief Set the adaptation rate for spike-frequency adaptation
     * 
     * @param rate Adaptation rate (0.0 = no adaptation, higher = stronger adaptation)
     */
    void setAdaptationRate(float rate);
    
    /**
     * @brief Get the current adaptation rate
     * 
     * @return float Adaptation rate
     */
    float getAdaptationRate() const;
    
    // Homeostatic regulation
    /**
     * @brief Set the homeostatic target membrane potential
     * 
     * @param target Target membrane potential for homeostasis (mV)
     */
    void setHomeostaticTarget(float target);
    
    /**
     * @brief Get the homeostatic target membrane potential
     * 
     * @return float Homeostatic target (mV)
     */
    float getHomeostaticTarget() const;
    
    // Neural excitability modulation
    /**
     * @brief Set the excitability modulation factor
     * 
     * @param mod Excitability modulation (0.0 = no modulation, 1.0 = normal, >1.0 = increased)
     */
    void setExcitabilityModulation(float mod);
    
    /**
     * @brief Get the current excitability modulation factor
     * 
     * @return float Excitability modulation
     */
    float getExcitabilityModulation() const;
    
    // Learning readiness assessment
    /**
     * @brief Check if the neuron is ready for synaptic learning
     * 
     * @return bool True if neuron is ready for learning
     */
    bool isReadyForLearning() const;
    
    // Synaptic efficacy calculation
    /**
     * @brief Calculate the current synaptic efficacy based on membrane potential and state
     * 
     * @return float Synaptic efficacy (0.0 to 1.0)
     */
    float calculateSynapticEfficacy() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
