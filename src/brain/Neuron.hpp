#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <memory>
#include <string>

namespace nlm {

// Abstract base class for neuron dynamics models
class NeuronDynamics {
public:
    virtual ~NeuronDynamics() = default;
    
    // Initialize dynamics with neuron parameters
    virtual void initialize(const NeuronState& state) = 0;
    
    // Update dynamics for time step
    virtual bool update(MembranePotential& membranePotential, 
                       FiringRate& firingRate,
                       Timestamp currentTime,
                       TimestepDuration dt) = 0;
    
    // Handle spike event
    virtual void onSpike(Timestamp currentTime) = 0;
    
    // Reset dynamics to initial state
    virtual void reset() = 0;
    
    // Get dynamics name
    virtual const std::string& getName() const = 0;
};

// Abstract base class for plasticity rules
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Initialize plasticity rule
    virtual void initialize(const PlasticityFlags& flags) = 0;
    
    // Update weights based on neural activity
    virtual void updateWeights(float& weight, 
                              float preSpikeTime,
                              float postSpikeTime,
                              float eligibilityTrace) = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
    
    // Get learning rate
    virtual float getLearningRate() const = 0;
    
    // Get rule name
    virtual const std::string& getName() const = 0;
    
    // Check if rule is enabled
    virtual bool isEnabled() const = 0;
};

// Abstract base class for sensory processing
class SensoryProcessor {
public:
    virtual ~SensoryProcessor() = default;
    
    // Process sensory input
    virtual MembranePotential processInput(const class SensoryInput& input,
                                          Timestamp timestamp) = 0;
    
    // Set sensory gain
    virtual void setGain(float gain) = 0;
    
    // Get sensory gain
    virtual float getGain() const = 0;
    
    // Get processor name
    virtual const std::string& getName() const = 0;
    
    // Check if processor is active
    virtual bool isActive() const = 0;
};

// Factory interface for creating dynamics components
class DynamicsFactory {
public:
    virtual ~DynamicsFactory() = default;
    
    // Create neuron dynamics instance
    virtual std::unique_ptr<NeuronDynamics> createNeuronDynamics(
        const std::string& type, 
        const std::vector<float>& parameters) = 0;
    
    // Create plasticity rule instance
    virtual std::unique_ptr<PlasticityRule> createPlasticityRule(
        const std::string& type,
        const std::vector<float>& parameters) = 0;
    
    // Create sensory processor instance
    virtual std::unique_ptr<SensoryProcessor> createSensoryProcessor(
        const std::string& type,
        const std::vector<float>& parameters) = 0;
    
    // Get list of available dynamics types
    virtual std::vector<std::string> getAvailableNeuronDynamicsTypes() const = 0;
    
    // Get list of available plasticity types
    virtual std::vector<std::string> getAvailablePlasticityTypes() const = 0;
    
    // Get list of available sensory processor types
    virtual std::vector<std::string> getAvailableSensoryProcessorTypes() const = 0;
};
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

    // EXTENSIBILITY METHODS
    // 
    // Set custom dynamics implementation
    void setDynamics(std::unique_ptr<NeuronDynamics> dynamics);
    
    // Set custom plasticity rule implementation
    void setPlasticityRule(std::unique_ptr<PlasticityRule> plasticity);
    
    // Set custom sensory processor implementation
    void setSensoryProcessor(std::unique_ptr<SensoryProcessor> processor);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
