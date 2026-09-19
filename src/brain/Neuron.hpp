#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <cmath>
#include <random>

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
    MembranePotential channelConductance;   // Channel conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    float noiseLevel;                      // Intrinsic channel noise
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , channelConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f)
        , noiseLevel(0.01f) {}
};

// Hodgkin-Huxley ion channel gating variables
struct HHChannelState {
    float m;  // Sodium activation gate (0-1)
    float h;  // Sodium inactivation gate (0-1)
    float n;  // Potassium activation gate (0-1)
    
    HHChannelState() : m(0.0f), h(0.0f), n(0.0f) {}
};

// NMDA receptor state
struct NMDAState {
    float activation;           // NMDA receptor activation (0-1)
    float magnesiumBlock;        // Mg2+ block factor (0-1)
    float desensitization;      // Desensitization factor (0-1)
    
    NMDAState() : activation(0.0f), magnesiumBlock(1.0f), desensitization(1.0f) {}
};

// AMPA receptor state
struct AMPAState {
    float activation;           // AMPA receptor activation (0-1)
    float desensitization;      // Desensitization factor (0-1)
    
    AMPAState() : activation(0.0f), desensitization(1.0f) {}
};

// Neuron class representing a single neuron
// Implements biologically realistic dynamics including HH model, channel noise, and receptor dynamics
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
    
    // Channel noise
    void setChannelNoiseLevel(float level);
    float getChannelNoiseLevel() const;
    
    // NMDA/AMPA receptor state access
    const NMDAState& getNMDAState() const;
    NMDAState& getNMDAState();
    const AMPAState& getAMPAState() const;
    AMPAState& getAMPAState();
    void updateReceptorDynamics(float dt, const std::vector<float>& synapticInput);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // Hodgkin-Huxley dynamics (realistic model)
    void updateHHChannels(float V, float dt, float noise = 0.0f);
    float computeSodiumCurrent(float V, const HHChannelState& channels);
    float computePotassiumCurrent(float V, const HHChannelState& channels);
    float computeLeakCurrent(float V, float gL);
    float alpha_m(float V);
    float beta_m(float V);
    float alpha_h(float V);
    float beta_h(float V);
    float alpha_n(float V);
    float beta_n(float V);
    
    // LIF dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt, bool enableNoise = false);
    
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
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Stochastic dynamics
    void applyChannelNoise(float noiseLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
