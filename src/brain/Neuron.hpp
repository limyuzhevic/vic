#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <memory>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;
class HodgkinHuxleyNeuron;
class AEIFNeuron;

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
    
    // HH neuron state variables
    float sodiumActivation;                // m gate (HH)
    float sodiumInactivation;              // h gate (HH)
    float potassiumActivation;              // n gate (HH)
    float adaptationCurrent;               // w variable (AEIF)
    float adaptationVariable2;             // u variable (AEIF)
    
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
        , sodiumActivation(0.0f)
        , sodiumInactivation(0.0f)
        , potassiumActivation(0.0f)
        , adaptationCurrent(0.0f)
        , adaptationVariable2(0.0f) {}
};

// Base neuron interface - abstract class for all neuron types
class Neuron {
public:
    virtual ~Neuron() = default;
    
    // Create neuron with ID and type
    static std::unique_ptr<Neuron> createNeuron(NeuronId id, NeuronType type = NeuronType::Internal);
    
    // Identity
    virtual NeuronId getId() const = 0;
    
    // Type
    virtual NeuronType getType() const = 0;
    virtual void setType(NeuronType type) = 0;
    
    // State access
    virtual const NeuronState& getState() const = 0;
    virtual NeuronState& getState() = 0;
    
    // Membrane potential
    virtual MembranePotential getMembranePotential() const = 0;
    virtual void setMembranePotential(MembranePotential potential) = 0;
    virtual void addToMembranePotential(MembranePotential delta) = 0;
    
    // Threshold
    virtual MembranePotential getThreshold() const = 0;
    virtual void setThreshold(MembranePotential threshold) = 0;
    
    // Firing state
    virtual bool isFiring() const = 0;
    virtual bool isRefractory() const = 0;
    virtual void setFiringState(FiringState state) = 0;
    virtual void setRefractoryPeriod(uint32_t steps) = 0;
    virtual void decrementRefractory() = 0;
    
    // Firing rate (for rate-based computation)
    virtual FiringRate getFiringRate() const = 0;
    virtual void setFiringRate(FiringRate rate) = 0;
    
    // LIF neuron parameters
    virtual void setLeakConductance(MembranePotential conductance) = 0;
    virtual MembranePotential getLeakConductance() const = 0;
    virtual uint32_t getRefractoryPeriod() const = 0;
    virtual void setRestingPotential(MembranePotential potential) = 0;
    virtual MembranePotential getRestingPotential() const = 0;
    virtual void setResetPotential(MembranePotential potential) = 0;
    
    // Spike detection
    virtual bool checkThreshold() const = 0;
    virtual float getLastSpikeTime() const = 0;
    
    // LIF step function - returns true if neuron fired
    virtual bool stepLIF(Timestamp currentTime, TimestepDuration dt) = 0;
    
    // HH step function - Hodgkin-Huxley dynamics
    virtual bool stepHH(Timestamp currentTime, TimestepDuration dt) = 0;
    
    // AEIF step function - Adaptive Exponential Integrate-and-Fire
    virtual bool stepAEIF(Timestamp currentTime, TimestepDuration dt) = 0;
    
    // Advanced step function - automatically selects appropriate model
    virtual bool step(Timestamp currentTime) = 0;
    
    // Incoming signals (post-synaptic potentials)
    virtual void receiveExcitatoryInput(MembranePotential amplitude) = 0;
    virtual void receiveInhibitoryInput(MembranePotential amplitude) = 0;
    virtual void receiveModulatoryInput(MembranePotential amplitude) = 0;
    
    // Current injection (from external sources)
    virtual void injectCurrent(MembranePotential current) = 0;
    virtual MembranePotential getTotalCurrent() const = 0;
    virtual void clearTotalCurrent() = 0;
    
    // Spike history (recent spikes for STDP)
    virtual void recordSpike(Timestamp timestamp) = 0;
    virtual const std::vector<Timestamp>& getSpikeHistory() const = 0;
    virtual void clearSpikeHistory() = 0;
    
    // Synapse management (incoming and outgoing)
    virtual void addIncomingSynapse(SynapseHandle handle) = 0;
    virtual void addOutgoingSynapse(SynapseHandle handle) = 0;
    virtual const std::vector<SynapseHandle>& getIncomingSynapses() const = 0;
    virtual const std::vector<SynapseHandle>& getOutgoingSynapses() const = 0;
    
    // Plasticity state
    virtual const PlasticityFlags& getPlasticityFlags() const = 0;
    virtual PlasticityFlags& getPlasticityFlags() = 0;
    virtual void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) = 0;
    
    // Region/population membership
    virtual void setRegionId(RegionId region) = 0;
    virtual RegionId getRegionId() const = 0;
    virtual void setPopulationId(PopulationId population) = 0;
    virtual PopulationId getPopulationId() const = 0;
    
    // Reset to initial state
    virtual void reset() = 0;
    
    // Initialize with random parameters
    virtual void initializeRandom(RandomGenerator& rng) = 0;
    
    // Get neuron model name
    virtual const char* getModelName() const = 0;
};
