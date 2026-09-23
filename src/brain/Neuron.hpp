#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Neuron state structure for efficient storage
// Contains all state variables needed to simulate a single neuron using the Leaky Integrate-and-Fire (LIF) model
// All fields are biologically realistic and include typical default values for cortical neurons
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV) - typically -70mV for cortical neurons
    MembranePotential threshold;           // Firing threshold (mV) - typically -55mV, must be > restingPotential
    MembranePotential resetPotential;       // Post-spike reset value (mV) - typically close to resting potential
    MembranePotential leakConductance;      // Leak conductance (nS) - determines membrane time constant
    MembranePotential synapseConductance;   // Synaptic conductance (nS) - for rate-based computation
    FiringRate firingRate;                 // Current firing rate (Hz) - for rate-based plasticity
    FiringState firingState;               // Current firing state (Resting, Active, Refractory)
    uint32_t refractoryRemaining;          // Steps remaining in refractory period (0 = not refractory)
    uint32_t refractoryPeriod;             // Total refractory period in simulation steps (typically 5-10ms)
    float adaptationVariable;              // For spike-frequency adaptation (increases with each spike)
    float lastSpikeTime;                   // Timestamp of last spike (-1.0f if none, ensures backward compatibility)
    
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
// Implements the Leaky Integrate-and-Fire (LIF) mathematical model for neural computation
// This is a biophysically plausible model that captures essential neural dynamics while remaining computationally efficient
// The model includes synaptic integration, spike generation, refractory periods, and spike-frequency adaptation
// Designed for large-scale network simulations with hundreds to thousands of neurons
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
    
    // Validation helper functions
    static bool isValidState(const NeuronState& state);
    static std::string neuronStateToString(const NeuronState& state);
    
    // Debug and logging functions
    void logState() const;
    void validateBeforeStep() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
