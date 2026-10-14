#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

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
    explicit Neuron(NeuronId id) 
        : id(id), type(NeuronType::Internal), regionId(INVALID_REGION_ID), 
          populationId(INVALID_POPULATION_ID), totalCurrent(0.0f),
          synapticInput(0.0f) {}
    
    ~Neuron() = default;
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&& other) noexcept : id(other.id), type(other.type), 
        state(std::move(other.state)), regionId(other.regionId), 
        populationId(other.populationId), totalCurrent(other.totalCurrent),
        synapticInput(other.synapticInput),
        spikeHistory(std::move(other.spikeHistory)),
        incomingSynapses(std::move(other.incomingSynapses)),
        outgoingSynapses(std::move(other.outgoingSynapses)),
        plasticityFlags(std::move(other.plasticityFlags)) {
        other.id = INVALID_NEURON_ID;
        other.type = NeuronType::Internal;
    }
    Neuron& operator=(Neuron&& other) noexcept {
        if (this != &other) {
            id = other.id;
            type = other.type;
            state = std::move(other.state);
            regionId = other.regionId;
            populationId = other.populationId;
            totalCurrent = other.totalCurrent;
            synapticInput = other.synapticInput;
            spikeHistory = std::move(other.spikeHistory);
            incomingSynapses = std::move(other.incomingSynapses);
            outgoingSynapses = std::move(other.outgoingSynapses);
            plasticityFlags = std::move(other.plasticityFlags);
            other.id = INVALID_NEURON_ID;
            other.type = NeuronType::Internal;
        }
        return *this;
    }
    
    // Identity
    NeuronId getId() const { return id; }
    
    // Type
    NeuronType getType() const { return type; }
    void setType(NeuronType type) { this->type = type; }
    
    // State access
    const NeuronState& getState() const { return state; }
    NeuronState& getState() { return state; }
    
    // Membrane potential
    MembranePotential getMembranePotential() const { return state.membranePotential; }
    void setMembranePotential(MembranePotential potential) { state.membranePotential = potential; }
    void addToMembranePotential(MembranePotential delta) { state.membranePotential += delta; }
    
    // Threshold
    MembranePotential getThreshold() const { return state.threshold; }
    void setThreshold(MembranePotential threshold) { state.threshold = threshold; }
    
    // Firing state
    bool isFiring() const { return state.firingState == FiringState::Active; }
    bool isRefractory() const { return state.refractoryRemaining > 0; }
    void setFiringState(FiringState state) { this->state.firingState = state; }
    void setRefractoryPeriod(uint32_t steps) { state.refractoryPeriod = steps; }
    void decrementRefractory() {
        if (state.refractoryRemaining > 0) {
            --state.refractoryRemaining;
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
        }
    }
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const { return state.firingRate; }
    void setFiringRate(FiringRate rate) { state.firingRate = rate; }
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance) { state.leakConductance = conductance; }
    MembranePotential getLeakConductance() const { return state.leakConductance; }
    uint32_t getRefractoryPeriod() const { return state.refractoryPeriod; }
    void setRestingPotential(MembranePotential potential) { state.restingPotential = potential; }
    MembranePotential getRestingPotential() const { return state.restingPotential; }
    void setResetPotential(MembranePotential potential) { state.resetPotential = potential; }
    
    // Spike detection
    bool checkThreshold() const { return state.membranePotential >= state.threshold; }
    float getLastSpikeTime() const { return state.lastSpikeTime; }
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) {
        bool fired = false;
        
        // Handle refractory period
        if (state.refractoryRemaining > 0) {
            --state.refractoryRemaining;
            // During refractory period, clear synaptic input but don't integrate
            synapticInput = 0.0f;
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // LIF dynamics: Leaky Integrate-and-Fire (exponential Euler formula)
        // v(t) = v_reset + (v(t-1) - v_reset) * exp(-dt/tau) + I_syn * tau
        
        MembranePotential& V = state.membranePotential;
        MembranePotential V_reset = state.resetPotential;
        MembranePotential threshold = state.threshold;
        const float tau = TIME_CONSTANT;  // ms
        
        float leakFactor = std::exp(-static_cast<float>(dt) / tau);
        
        // Apply leak dynamics: V(t) = V_reset + (V(t-1) - V_reset) * exp(-dt/tau)
        V = V_reset + (V - V_reset) * leakFactor;
        
        // Apply synaptic input: I_syn * tau
        // Synaptic input is converted from current to voltage using membrane resistance
        float synapticInputContribution = synapticInput * tau;
        
        // Combine leak and synaptic contributions
        V += synapticInputContribution;
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (state.adaptationVariable > 0.0f) {
            V -= state.adaptationVariable * 0.01f;
            state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            state.firingState = FiringState::Active;
            state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike
            recordSpike(currentTime);
            
            // Reset membrane potential
            V = V_reset;
            
            // Enter refractory period
            state.refractoryRemaining = state.refractoryPeriod;
            state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            state.adaptationVariable += 1.0f;
        } else {
            state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input for next step
        synapticInput = 0.0f;
        
        return fired;
    }
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude) {
        // Real synaptic input: excitatory currents add to total current
        // amplitude represents synaptic conductance * reversal potential contribution
        synapticInput += amplitude;
    }
    
    void receiveInhibitoryInput(MembranePotential amplitude) {
        // Real inhibitory input: subtract from total current
        // Inhibitory synaptic currents hyperpolarize the neuron
        synapticInput -= amplitude;
    }
    
    void receiveModulatoryInput(MembranePotential amplitude) {
        // Modulatory input affects plasticity but not directly integrated
        // Used for neuromodulation (e.g., dopamine, acetylcholine)
        state.adaptationVariable += amplitude * 0.1f;
    }
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current) {
        // Direct current injection (e.g., from sensory input or external source)
        // Add to synaptic input for LIF integration
        synapticInput += current;
    }
    
    MembranePotential getTotalCurrent() const { return synapticInput; }
    void clearTotalCurrent() { synapticInput = 0.0f; }
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp) {
        spikeHistory.push_back(timestamp);
        if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
            spikeHistory.erase(spikeHistory.begin());
        }
    }
    
    const std::vector<Timestamp>& getSpikeHistory() const { return spikeHistory; }
    void clearSpikeHistory() { spikeHistory.clear(); }
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle) {
        incomingSynapses.push_back(handle);
    }
    
    void addOutgoingSynapse(SynapseHandle handle) {
        outgoingSynapses.push_back(handle);
    }
    
    const std::vector<SynapseHandle>& getIncomingSynapses() const { return incomingSynapses; }
    const std::vector<SynapseHandle>& getOutgoingSynapses() const { return outgoingSynapses; }
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const { return plasticityFlags; }
    PlasticityFlags& getPlasticityFlags() { return plasticityFlags; }
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
        plasticityFlags.hebbian = hebbian;
        plasticityFlags.stdp = stdp;
        plasticityFlags.reward_modulated = rewardModulated;
    }
    
    // Region/population membership
    void setRegionId(RegionId region) { regionId = region; }
    RegionId getRegionId() const { return regionId; }
    void setPopulationId(PopulationId population) { populationId = population; }
    PopulationId getPopulationId() const { return populationId; }
    
    // Update neuron for one simulation step
    // Uses LIF dynamics via stepLIF() implementation
    void step(Timestamp currentTime) {
        // Default LIF step with standard timestep (1ms)
        TimestepDuration dt = 0.001;  // 1ms default
        stepLIF(currentTime, dt);
    }
    
    // Reset to initial state
    void reset() {
        state = NeuronState();
        synapticInput = 0.0f;
        spikeHistory.clear();
    }
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng) {
        // Real random initialization with biological constraints
        // Membrane potential starts near resting potential
        state.membranePotential = state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        
        // Threshold is typically -55mV with small variation
        state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
        
        // Resting potential typically -70mV
        state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        
        // Reset potential is usually close to resting
        state.resetPotential = state.restingPotential + rng.uniformReal(0.0f, 5.0f);
        
        // Refractory period: 2-10ms typical
        state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(static_cast<uint32_t>(2), static_cast<uint32_t>(10)));
        
        // Initial state
        state.firingState = FiringState::Resting;
        state.refractoryRemaining = 0;
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
        
        // Clear any residual state
        synapticInput = 0.0f;
        spikeHistory.clear();
    }
    
private:
    // Neuron implementation details (moved from pImpl)
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
};

} // namespace nlm
