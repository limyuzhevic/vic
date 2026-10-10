#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

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
    
    // NEW: Advanced neuron state variables
    float thresholdAdaptation;              // mV per spike (adaptive threshold)
    float adaptationCurrent;                // Current adaptation level
    float refractoryAdaptation;            // ms per spike (adapted refractory)
    float inputResistance;                 // MOhm (input resistance)
    float energyConsumption;               // arbitrary units
    float lastMaintenanceTime;             // Timestamp of last maintenance
    float repairCooldownRemaining;         // Remaining cooldown for repair
    float neuralNoiseSigma;                // mV (noise level)
    float synapticInputFilter;             // time constant for filtering (ms)
    float eligibilityTrace;                // for reward-modulated learning
    float spikeCount;                      // recent spike count for adaptation
    float timeSinceLastSpike;              // ms since last spike
    uint32_t consecutiveSpikes;            // count of consecutive spikes
    
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
        , thresholdAdaptation(0.1f)
        , adaptationCurrent(0.0f)
        , refractoryAdaptation(1.0f)
        , inputResistance(100.0f)
        , energyConsumption(0.0f)
        , lastMaintenanceTime(0.0f)
        , repairCooldownRemaining(0.0f)
        , neuralNoiseSigma(0.5f)
        , synapticInputFilter(5.0f)
        , eligibilityTrace(0.0f)
        , spikeCount(0.0f)
        , timeSinceLastSpike(0.0f)
        , consecutiveSpikes(0)
        {}
};

// Helper functions
inline float computeMembraneResistance(float timeConstant, float capacitance) {
    return (capacitance > 0.0f) ? (timeConstant / capacitance) : 100.0f;
}

inline float computeTimeConstant(float resistance, float capacitance) {
    return (capacitance > 0.0f) ? (resistance * capacitance) : 20.0f;
}

inline float exponentialDecay(float value, float decayRate, TimestepDuration dt) {
    return value * std::exp(-decayRate * dt * 1000.0f);  // Convert s to ms
}

inline float synapticFilter(float input, float previous, float filterTimeConstant, TimestepDuration dt) {
    float alpha = std::exp(-dt * 1000.0f / filterTimeConstant);  // Convert s to ms
    return input * (1.0f - alpha) + previous * alpha;
}

inline float computeAdaptiveThreshold(float baseThreshold, float adaptation, float consecutiveSpikes, float adaptationStrength) {
    return baseThreshold + adaptation * adaptationStrength * consecutiveSpikes;
}

inline float updateEnergyEfficiency(float efficiency, float activity, float dt) {
    // Activity-dependent efficiency change
    float efficiencyChange = activity * dt * 0.01f;
    return std::clamp(efficiency + efficiencyChange, 0.5f, 1.0f);
}

inline float stdpUpdate(float weight, float preSpikeTime, float postSpikeTime, float tauPre, float tauPost, float learningRate) {
    float delta = 0.0f;
    if (preSpikeTime >= 0.0f && postSpikeTime >= 0.0f) {
        delta = learningRate * std::exp(-std::abs(postSpikeTime - preSpikeTime) / tauPre) - 
                learningRate * std::exp(-std::abs(preSpikeTime - postSpikeTime) / tauPost);
    }
    return weight + delta;
}

inline float hebbianUpdate(float weight, float preActivity, float postActivity, float learningRate) {
    return weight + learningRate * preActivity * postActivity;
}

inline float rewardModulatedUpdate(float weight, float eligibilityTrace, float reward, float learningRate) {
    return weight + learningRate * eligibilityTrace * reward;
}

inline MembranePotential computeConstrainedMembranePotential(
    float V, 
    float V_rest, 
    float I_syn, 
    float Cm, 
    float tau, 
    TimestepDuration dt,
    float noiseSigma = 0.0f
) {
    // Add noise
    if (noiseSigma > 0.0f) {
        float noise = noiseSigma * (rand() / (RAND_MAX + 1.0f) * 2.0f - 1.0f);
        I_syn += noise;
    }
    
    // Compute new potential
    float dV = dt * 1000.0f * ((V_rest - V) / tau + I_syn / Cm);
    float V_new = V + dV;
    
    return V_new;
}

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with advanced features
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
    
    // Advanced neuron parameters
    void setMembraneCapacitance(float capacitance);  // nF
    float getMembraneCapacitance() const;
    void setTimeConstant(float tau);  // ms
    float getTimeConstant() const;
    void setInputResistance(float resistance);  // MOhm
    float getInputResistance() const;
    void setThresholdAdaptation(float alpha);  // mV per spike
    float getThresholdAdaptation() const;
    void setAdaptationTimeConstant(float tau_adapt);  // ms
    float getAdaptationTimeConstant() const;
    void setNoiseLevel(float sigma);  // mV
    float getNoiseLevel() const;
    void setRefractoryAdaptation(float alpha_refrac);  // ms per spike
    float getRefractoryAdaptation() const;
    void setEnergyEfficiency(float efficiency);  // 0.0-1.0
    float getEnergyEfficiency() const;
    
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
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Safety and validation
    bool validateState() const;
    void clampMembranePotential();
    
    // Population management
    void setMinPopulationSize(uint32_t size);
    uint32_t getMinPopulationSize() const;
    bool isPopulationHealthy() const;
    
    // Energy and maintenance
    void updateEnergyConsumption();
    float getEnergyConsumption() const;
    void applyMaintenance(Timestamp currentTime);
    bool needsRepair(Timestamp currentTime) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm