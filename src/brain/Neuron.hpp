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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with:
// - Configurable membrane parameters (capacitance, time constant, leak)
// - Spike-frequency adaptation
// - Synaptic input integration with conductance-based modeling
// - Biophysical constraints and regularization
class Neuron {
public:
    // Create neuron with ID and type
    Neuron(NeuronId id, NeuronType type = NeuronType::Internal);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity and metadata
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // ===== LIF DYNAMICS =====
    // Configure neuron biophysical parameters
    void setMembraneCapacitance(float capacitance);  // nF, typically 0.1-1.0
    float getMembraneCapacitance() const;
    
    void setTimeConstant(float timeConstant);  // ms, typically 10-30
    float getTimeConstant() const;
    
    void setLeakConductance(float conductance);  // nS, typical -70 to -50
    float getLeakConductance() const;
    
    void setRestingPotential(float potential);  // mV, typically -65 to -75
    float getRestingPotential() const;
    
    void setThreshold(float threshold);  // mV, typically -50 to -40
    float getThreshold() const;
    
    void setResetPotential(float potential);  // mV, typically -70 to -65
    float getResetPotential() const;
    
    void setRefractoryPeriod(uint32_t steps);  // ms, typically 2-10
    uint32_t getRefractoryPeriod() const;
    
    // ===== STATE MANAGEMENT =====
    const NeuronState& getState() const;
    NeuronState& getState();
    
    MembranePotential getMembranePotential() const;
    void setMembranePotential(float potential);
    void addToMembranePotential(float delta);
    
    // ===== Firing and Spiking =====
    bool checkThreshold() const;
    bool isFiring() const;
    bool isRefractory() const;
    float getLastSpikeTime() const;
    
    // Process one LIF integration step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // ===== INPUTS AND MODULATION =====
    // Synaptic inputs
    void receiveExcitatoryInput(float amplitude);  // Positive conductance change
    void receiveInhibitoryInput(float amplitude);  // Negative conductance change
    void receiveModulatoryInput(float amplitude);   // Neuromodulatory input
    
    // External current injection
    void injectCurrent(float current);  // Direct current (μA)
    float getTotalCurrent() const;
    void clearTotalCurrent();
    
    // ===== ADAPTATION AND REGULATION =====
    void setAdaptationStrength(float strength);  // Spike-frequency adaptation
    float getAdaptationStrength() const;
    
    void setAdaptationTimeConstant(float tau);  // Adaptation decay time
    float getAdaptationTimeConstant() const;
    
    // ===== SPIKE HISTORY =====
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    size_t getRecentSpikeCount(float timeWindow) const;  // Count spikes within time window
    
    // ===== SYNAPTIC CONNECTIVITY =====
    // For STDP and synaptic plasticity
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // ===== PLASTICITY =====
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // ===== DEVELOPMENT =====
    // Initialize neuron with biologically plausible random parameters
    void initializeRandom(RandomGenerator& rng, bool developmental = true);
    
    // Growth and structural changes
    void growDendriteBranch();
    void addReceptor(float probability = 1.0f);
    
    // Reset to initial state
    void reset();
    
    // ===== STATISTICS =====
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // ===== DEBUG AND MONITORING =====
    void logState() const;
    bool validateParameters() const;
    
    // ===== PLASTICITY =====
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // ===== DEVELOPMENT =====
    // Initialize neuron with biologically plausible random parameters
    void initializeRandom(RandomGenerator& rng, bool developmental = true);
    
    // Growth and structural changes
    void growDendriteBranch();
    void addReceptor(float probability = 1.0f);
    
    // Reset to initial state
    void reset();
    
    // ===== STATISTICS =====
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // ===== DEBUG AND MONITORING =====
    void logState() const;
    bool validateParameters() const;
    
    // ===== TEMPORARY =====
    // Update neuron for one simulation step (convenience method)
    void step(Timestamp currentTime);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
