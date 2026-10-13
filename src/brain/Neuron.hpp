#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
// Enhanced version with HH-style dynamics and additional parameters
struct NeuronState {
    // Basic LIF parameters (maintained for backward compatibility)
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
    
    // Enhanced HH-style dynamics parameters
    // HH gating variables
    float m;                               // Sodium activation gate
    float h;                               // Sodium inactivation gate
    float n;                               // Potassium activation gate
    
    // HH conductances and reversal potentials
    float sodiumConductance;               // Sodium conductance (nS)
    float potassiumConductance;             // Potassium conductance (nS)
    float sodiumReversal;                  // Sodium reversal potential (mV)
    float potassiumReversal;                // Potassium reversal potential (mV)
    
    // Advanced plasticity and neuromodulation parameters
    float thresholdAdaptation;             // Adaptive threshold modulation
    float calciumConcentration;            // Calcium dynamics (for plasticity)
    float nmdaConductance;                  // NMDA synaptic conductance
    float eligibilityTrace;                 // For reward-modulated learning
    float metaplasticWeight;               // Weight-dependent plasticity
    float voltageModulation;                // Voltage-dependent plasticity factor
    
    // Refractory period management
    uint32_t refractoryPhase;               // Current refractory phase (0=absolute, 1=relative)
    float adaptationTimeConstant;           // Adaptation decay time constant
    float v_min;                           // Minimum voltage clamp (nS)
    float v_max;                           // Maximum voltage clamp (nS)
    
    // Synaptic integration state (conductance-based)
    float ampaConductance;                 // AMPA synaptic conductance (nS)
    float nmdaConductance;                 // NMDA synaptic conductance (nS)
    float gabaConductance;                 // GABA_A synaptic conductance (nS)
    float modulatoryConductance;           // Modulatory synaptic conductance (nS)
    
    // Short-term plasticity parameters
    float resourcePool;                    // Available synaptic resources (0-1)
    float utilization;                      // Fraction of resources used
    float facilitationRate;                // Resource facilitation rate
    float depressionRate;                   // Resource depression rate
    float recoveryRate;                     // Resource recovery rate
    
    // Neuromodulation state
    float dopamineLevel;                   // Dopamine concentration (0-1)
    float acetylcholineLevel;              // Acetylcholine concentration (0-1)
    float serotoninLevel;                  // Serotonin concentration (0-1)
    float norepinephrineLevel;             // Norepinephrine concentration (0-1)
    
    // Synaptic activity history
    float recentAMPA;                       // Recent AMPA activity
    float recentNMDA;                       // Recent NMDA activity
    
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
        , lastSpikeTime(-1.0f)
        , m(0.05f)
        , h(0.6f)
        , n(0.32f)
        , sodiumConductance(0.0f)
        , potassiumConductance(0.0f)
        , sodiumReversal(50.0f)
        , potassiumReversal(-77.0f)
        , thresholdAdaptation(0.0f)
        , calciumConcentration(0.0f)
        , nmdaConductance(0.0f)
        , eligibilityTrace(0.0f)
        , metaplasticWeight(1.0f)
        , voltageModulation(1.0f)
        , refractoryPhase(0)
        , adaptationTimeConstant(50.0f)
        , v_min(-100.0f)
        , v_max(50.0f)
        , ampaConductance(0.0f)
        , nmdaConductance(0.0f)
        , gabaConductance(0.0f)
        , modulatoryConductance(0.0f)
        , resourcePool(1.0f)
        , utilization(0.0f)
        , facilitationRate(0.5f)
        , depressionRate(0.15f)
        , recoveryRate(0.01f)
        , dopamineLevel(0.0f)
        , acetylcholineLevel(0.0f)
        , serotoninLevel(0.0f)
        , norepinephrineLevel(0.0f)
        , recentAMPA(0.0f)
        , recentNMDA(0.0f) {}
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
