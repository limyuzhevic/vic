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
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const { return id_; }
    
    // Type
    NeuronType getType() const { return type_; }
    void setType(NeuronType type) { type_ = type; }
    
    // State access
    const NeuronState& getState() const { return state_; }
    NeuronState& getState() { return state_; }
    
    // Membrane potential
    MembranePotential getMembranePotential() const { return state_.membranePotential; }
    void setMembranePotential(MembranePotential potential) { state_.membranePotential = potential; }
    void addToMembranePotential(MembranePotential delta) { state_.membranePotential += delta; }
    
    // Threshold
    MembranePotential getThreshold() const { return state_.threshold; }
    void setThreshold(MembranePotential threshold) { state_.threshold = threshold; }
    
    // Firing state
    bool isFiring() const { return state_.firingState == FiringState::Firing; }
    bool isRefractory() const { return state_.refractoryRemaining > 0; }
    void setFiringState(FiringState state) { state_.firingState = state; }
    void setRefractoryPeriod(uint32_t steps) { state_.refractoryPeriod = steps; }
    void decrementRefractory() { 
        if (state_.refractoryRemaining > 0) {
            --state_.refractoryRemaining; 
            if (state_.refractoryRemaining == 0) {
                state_.firingState = FiringState::Resting;
            }
        }
    }
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const { return state_.firingRate; }
    void setFiringRate(FiringRate rate) { state_.firingRate = rate; }
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance) { state_.leakConductance = conductance; }
    MembranePotential getLeakConductance() const { return state_.leakConductance; }
    uint32_t getRefractoryPeriod() const { return state_.refractoryPeriod; }
    void setRestingPotential(MembranePotential potential) { state_.restingPotential = potential; }
    MembranePotential getRestingPotential() const { return state_.restingPotential; }
    void setResetPotential(MembranePotential potential) { state_.resetPotential = potential; }
    
    // Spike detection
    bool checkThreshold() const { return state_.membranePotential >= state_.threshold; }
    float getLastSpikeTime() const { return state_.lastSpikeTime; }
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    float getTotalCurrent() const { return synapticInput_; }
    void clearTotalCurrent() { synapticInput_ = 0.0f; }
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const { return spikeHistory_; }
    void clearSpikeHistory() { spikeHistory_.clear(); }
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle) { incomingSynapses_.push_back(handle); }
    void addOutgoingSynapse(SynapseHandle handle) { outgoingSynapses_.push_back(handle); }
    const std::vector<SynapseHandle>& getIncomingSynapses() const { return incomingSynapses_; }
    const std::vector<SynapseHandle>& getOutgoingSynapses() const { return outgoingSynapses_; }
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const { return plasticityFlags_; }
    PlasticityFlags& getPlasticityFlags() { return plasticityFlags_; }
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region) { regionId_ = region; }
    RegionId getRegionId() const { return regionId_; }
    void setPopulationId(PopulationId population) { populationId_ = population; }
    PopulationId getPopulationId() const { return populationId_; }
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    // Member variables (eliminated PIMPL for performance)
    NeuronId id_;
    NeuronType type_;
    NeuronState state_;
    RegionId regionId_;
    PopulationId populationId_;
    float synapticInput_;  // Accumulated synaptic input for this step
    std::vector<Timestamp> spikeHistory_;
    std::vector<SynapseHandle> incomingSynapses_;
    std::vector<SynapseHandle> outgoingSynapses_;
    PlasticityFlags plasticityFlags_;
    
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
};

} // namespace nlm
