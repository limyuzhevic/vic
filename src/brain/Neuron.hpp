#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Configuration parameters for neuron behavior
struct NeuronConfig {
    // LIF dynamics parameters
    float membraneCapacitance;     // nF
    float timeConstant;            // ms
    float maxMembranePotential;    // mV
    float minMembranePotential;    // mV
    
    // Default biological values
    static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;
    static constexpr float DEFAULT_MAX_POTENTIAL = 50.0f;
    static constexpr float DEFAULT_MIN_POTENTIAL = -100.0f;
    
    // Adaptation parameters
    float adaptationTimeConstant;  // ms
    float maxAdaptation;           // mV
    
    // Default adaptation values
    static constexpr float DEFAULT_ADAPTATION_TC = 100.0f;
    static constexpr float DEFAULT_MAX_ADAPTATION = 20.0f;
    
    // Input scaling parameters
    float excitatoryWeight;        // Scaling factor for excitatory inputs
    float inhibitoryWeight;         // Scaling factor for inhibitory inputs
    
    // Default input weights
    static constexpr float DEFAULT_EXCITATORY_WEIGHT = 1.0f;
    static constexpr float DEFAULT_INHIBITORY_WEIGHT = 0.8f;
    
    // Firing rate parameters
    float maxFiringRate;           // Hz
    float minFiringRate;           // Hz
    
    // Default firing rate limits
    static constexpr float DEFAULT_MAX_FIRING_RATE = 1000.0f;
    static constexpr float DEFAULT_MIN_FIRING_RATE = 0.0f;
    
    NeuronConfig()
        : membraneCapacitance(DEFAULT_MEMBRANE_CAPACITANCE)
        , timeConstant(DEFAULT_TIME_CONSTANT)
        , maxMembranePotential(DEFAULT_MAX_POTENTIAL)
        , minMembranePotential(DEFAULT_MIN_POTENTIAL)
        , adaptationTimeConstant(DEFAULT_ADAPTATION_TC)
        , maxAdaptation(DEFAULT_MAX_ADAPTATION)
        , excitatoryWeight(DEFAULT_EXCITATORY_WEIGHT)
        , inhibitoryWeight(DEFAULT_INHIBITORY_WEIGHT)
        , maxFiringRate(DEFAULT_MAX_FIRING_RATE)
        , minFiringRate(DEFAULT_MIN_FIRING_RATE) {}
};

// Constants for biologically plausible ranges
struct NeuronConstants {
    // Membrane potential ranges (mV)
    static constexpr float MIN_MEMBRANE_POTENTIAL = -120.0f;
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float DEFAULT_MEMBRANE_POTENTIAL = -70.0f;
    
    // Threshold ranges (mV)
    static constexpr float MIN_THRESHOLD = -80.0f;
    static constexpr float MAX_THRESHOLD = -40.0f;
    static constexpr float DEFAULT_THRESHOLD = -55.0f;
    
    // Resting potential ranges (mV)
    static constexpr float MIN_RESTING_POTENTIAL = -80.0f;
    static constexpr float MAX_RESTING_POTENTIAL = -50.0f;
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;
    
    // Leak conductance ranges (nS)
    static constexpr float MIN_LEAK_CONDUCTANCE = 1.0f;
    static constexpr float MAX_LEAK_CONDUCTANCE = 100.0f;
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;
    
    // Refractory period ranges (ms)
    static constexpr uint32_t MIN_REFLECTORY_PERIOD = 1;
    static constexpr uint32_t MAX_REFLECTORY_PERIOD = 100;
    static constexpr uint32_t DEFAULT_REFLECTORY_PERIOD = 5;
    
    // Adaptation ranges (mV)
    static constexpr float MIN_ADAPTATION = 0.0f;
    static constexpr float MAX_ADAPTATION = 50.0f;
    static constexpr float DEFAULT_ADAPTATION = 0.0f;
    
    // Firing rate ranges (Hz)
    static constexpr float MIN_FIRING_RATE = 0.0f;
    static constexpr float MAX_FIRING_RATE = 1000.0f;
    static constexpr float DEFAULT_FIRING_RATE = 0.0f;
};

// Factory methods for creating specialized neurons
class NeuronFactory {
public:
    // Standard biological neuron
    static Neuron createStandardNeuron(NeuronId id);
    
    // Fast-spiking neuron
    static Neuron createFastSpikingNeuron(NeuronId id);
    
    // Regular spiking neuron
    static Neuron createRegularSpikingNeuron(NeuronId id);
    
    // Intrinsic burster
    static Neuron createIntrinsicBursterNeuron(NeuronId id);
    
    // Chattering neuron
    static Neuron createChatteringNeuron(NeuronId id);
    
    // Adapting neuron
    static Neuron createAdaptingNeuron(NeuronId id);
    
    // Customized neuron with specific configuration
    static Neuron createCustomizedNeuron(NeuronId id, const NeuronConfig& config);
    
    // Initialize neuron with random parameters
    static void initializeRandom(Neuron& neuron, RandomGenerator& rng, const NeuronConfig& config = NeuronConfig());
    
    // Validation helpers
    static bool validateMembranePotential(MembranePotential potential);
    static bool validateThreshold(MembranePotential threshold);
    static bool validateLeakConductance(MembranePotential conductance);
    static bool validateRefractoryPeriod(uint32_t period);
    static bool validateAdaptation(float adaptation);
    static bool validateFiringRate(FiringRate rate);
    
    // Configuration helpers
    static NeuronConfig createDefaultConfig();
    static NeuronConfig createFastConfig();
    static NeuronConfig createBurstingConfig();
    static NeuronConfig createAdaptingConfig();
};

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
    MembranePotential getSynapticInput() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Advanced spike history management
    void resetSpikeHistory();
    
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