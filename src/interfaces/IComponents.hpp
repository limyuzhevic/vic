#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Forward declarations
class IBrainInterface;
class Config;
class RandomGenerator;

// IMemorySystem: Abstract interface for memory operations
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;

    // Initialize with brain reference
    virtual void initialize(IBrainInterface* brain) = 0;

    // Core memory operations
    virtual void store(NeuronId neuron, float value) = 0;
    virtual float retrieve(NeuronId neuron) const = 0;
    virtual bool contains(NeuronId neuron) const = 0;
    virtual void clear() = 0;

    // System-specific operations
    virtual size_t getCapacity() const = 0;
    virtual size_t getCurrentSize() const = 0;
    virtual void decay(float decayRate) = 0;
    virtual void update(double timestep) = 0;

    // Get memory traces
    virtual std::vector<NeuronId> getActiveTraces() const = 0;
    virtual std::vector<NeuronId> getMemoryNeurons() const = 0;

protected:
    IMemorySystem() = default;
};

// INeuromodulationSystem: Interface for neuromodulation
class INeuromodulationSystem {
public:
    virtual ~INeuromodulationSystem() = default;

    // Core neuromodulation operations
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual const char* getName() const = 0;
    virtual float getPlasticityFactor() const = 0;

    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;

    // Modulatory effects
    virtual void signalReward(float reward) = 0;
    virtual void signalError(float error) = 0;

protected:
    INeuromodulationSystem() = default;
};

// IPredictionSystem: Interface for prediction system
class IPredictionSystem {
public:
    virtual ~IPredictionSystem() = default;

    // Prediction operations
    virtual std::unique_ptr<class SensoryInput> predictNextState(const SensoryInput& currentState) = 0;
    virtual void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) = 0;
    virtual float getPredictionError() const = 0;
    virtual float getConfidence() const = 0;
    virtual const std::vector<float>& getErrorHistory() const = 0;
    virtual void clearHistory() = 0;
    virtual void train(const SensoryInput& observation) = 0;

    // Update with brain state
    virtual void update(IBrainInterface* brain, double timestep) = 0;

protected:
    IPredictionSystem() = default;
};

// ICognitionSystem: Interface for cognition system
class ICognitionSystem {
public:
    virtual ~ICognitionSystem() = default;

    // Cognition operations
    virtual ActionType evaluateAction(const std::vector<float>& state, ActionType action) = 0;
    virtual std::vector<ActionType> generateActionSequences(size_t depth) = 0;
    virtual float evaluateSequence(const std::vector<ActionType>& actions, const std::vector<float>& state) = 0;
    virtual void updatePlans(const std::vector<ActionType>& actualActions, float reward) = 0;
    virtual void update(double timestep) = 0;
    virtual void clearCache() = 0;

    // System-specific operations
    virtual void initialize(IBrainInterface* brain) = 0;
    virtual size_t getPlanningDepth() const = 0;
    virtual void setPlanningDepth(size_t depth) = 0;
    virtual float getPlanningConfidence() const = 0;

    // State access
    virtual std::vector<float> getCurrentGoal() const = 0;
    virtual void setCurrentGoal(const std::vector<float>& goal) = 0;

protected:
    ICognitionSystem() = default;
};

// IDevelopmentSystem: Interface for development system
class IDevelopmentSystem {
public:
    virtual ~IDevelopmentSystem() = default;

    // Development operations
    virtual void update(IBrainInterface* brain, RandomGenerator& rng, TimestepDuration dt) = 0;
    virtual void update(IBrainInterface* brain, SimulationStep currentStep) = 0;
    virtual DevelopmentalStage getStage() const = 0;
    virtual void setStage(DevelopmentalStage stage) = 0;
    virtual float getPlasticityModifier() const = 0;
    virtual double getDevelopmentalAge() const = 0;
    virtual void setDevelopmentalAge(double age) = 0;
    virtual bool isCriticalPeriod() const = 0;
    virtual float getCriticalPeriodProgress() const = 0;
    virtual const char* getStageName() const = 0;

protected:
    IDevelopmentSystem() = default;
};

// IPlasticitySystem: Interface for plasticity system
class IPlasticitySystem {
public:
    virtual ~IPlasticitySystem() = default;

    // Plasticity operations
    virtual void update(IBrainInterface* brain, RandomGenerator& rng, TimestepDuration dt) = 0;
    virtual SynapseId createSynapse(IBrainInterface* brain, NeuronId source, NeuronId destination,
                                   SynapticWeight weight) = 0;
    virtual bool removeSynapse(IBrainInterface* brain, SynapseId synapse) = 0;
    virtual NeuronId createNeuron(IBrainInterface* brain, NeuronType type) = 0;
    virtual bool removeNeuron(IBrainInterface* brain, NeuronId neuron) = 0;

    // Configuration
    virtual float getSynaptogenesisRate() const = 0;
    virtual void setSynaptogenesisRate(float rate) = 0;
    virtual float getPruningRate() const = 0;
    virtual void setPruningRate(float rate) = 0;

    // STDP configuration
    virtual void configure(float ltpWeight, float ltdWeight, float tau) = 0;

protected:
    IPlasticitySystem() = default;
};

// IDynamicsSystem: Interface for dynamics system (spike generation)
class IDynamicsSystem {
public:
    virtual ~IDynamicsSystem() = default;

    // Dynamics operations
    virtual void processSpikes(SimulationStep currentStep) = 0;
    virtual void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void queueSpike(const SpikeEvent& event) = 0;
    virtual void queueDelayedSpike(const DelayedSpikeEvent& event) = 0;
    virtual void registerHandler(std::function<void(const DetailedSpikeEvent&)> handler) = 0;
    virtual void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)> handler) = 0;
    virtual size_t getPendingSpikeCount() const = 0;
    virtual size_t getPendingDelayedCount() const = 0;
    virtual void reset() = 0;

    // Configuration and parameters
    virtual void setParameters(float threshold, float reset, float leak) = 0;
    virtual void getParameters(float& threshold, float& reset, float& leak) const = 0;

protected:
    IDynamicsSystem() = default;
};

} // namespace nlm
