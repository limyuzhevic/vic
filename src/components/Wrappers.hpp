#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../interfaces/IComponents.hpp"
#include <memory>
#include <vector>

namespace nlm {

// MemorySystemWrapper: Wraps existing memory systems to implement IMemorySystem
class MemorySystemWrapper : public IMemorySystem {
public:
    explicit MemorySystemWrapper(std::shared_ptr<Brain> brain);
    ~MemorySystemWrapper() override;
    
    // IMemorySystem interface
    void initialize(IBrainInterface* brain) override;
    void store(NeuronId neuron, float value) override;
    float retrieve(NeuronId neuron) const override;
    bool contains(NeuronId neuron) const override;
    void clear() override;
    size_t getCapacity() const override;
    size_t getCurrentSize() const override;
    void decay(float decayRate) override;
    void update(double timestep) override;
    std::vector<NeuronId> getActiveTraces() const override;
    std::vector<NeuronId> getMemoryNeurons() const override;
    
    // Memory system access methods
    NeuralWorkingMemory* getWorkingMemory() const;
    NeuralEpisodicMemory* getEpisodicMemory() const;
    NeuralAssociativeMemory* getAssociativeMemory() const;

private:
    std::shared_ptr<Brain> brain_;
};

// NeuromodulationSystemWrapper: Wraps existing neuromodulation systems
class NeuromodulationSystemWrapper : public INeuromodulationSystem {
public:
    explicit NeuromodulationSystemWrapper(std::shared_ptr<Brain> brain);
    ~NeuromodulationSystemWrapper() override;
    
    // INeuromodulationSystem interface
    float getLevel() const override;
    void setLevel(float level) override;
    const char* getName() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalReward(float reward) override;
    void signalError(float error) override;
    
    // Neuromodulation system access methods
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionError() const;

private:
    std::shared_ptr<Brain> brain_;
};

// PredictionSystemWrapper: Wraps existing prediction system
class PredictionSystemWrapper : public IPredictionSystem {
public:
    explicit PredictionSystemWrapper(std::shared_ptr<Brain> brain);
    ~PredictionSystemWrapper() override;
    
    // IPredictionSystem interface
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState) override;
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) override;
    float getPredictionError() const override;
    float getConfidence() const override;
    const std::vector<float>& getErrorHistory() const override;
    void clearHistory() override;
    void train(const SensoryInput& observation) override;
    void update(IBrainInterface* brain, double timestep) override;
    
    // Prediction system access methods
    PredictionSystem* getPredictionSystem() const;

private:
    std::shared_ptr<Brain> brain_;
};

// CognitionSystemWrapper: Wraps existing cognition systems
class CognitionSystemWrapper : public ICognitionSystem {
public:
    explicit CognitionSystemWrapper(std::shared_ptr<Brain> brain);
    ~CognitionSystemWrapper() override;
    
    // ICognitionSystem interface
    ActionType evaluateAction(const std::vector<float>& state, ActionType action) override;
    std::vector<ActionType> generateActionSequences(size_t depth) override;
    float evaluateSequence(const std::vector<ActionType>& actions, const std::vector<float>& state) override;
    void updatePlans(const std::vector<ActionType>& actualActions, float reward) override;
    void update(double timestep) override;
    void clearCache() override;
    void initialize(IBrainInterface* brain) override;
    size_t getPlanningDepth() const override;
    void setPlanningDepth(size_t depth) override;
    float getPlanningConfidence() const override;
    std::vector<float> getCurrentGoal() const override;
    void setCurrentGoal(const std::vector<float>& goal) override;
    
    // Cognition system access methods
    NeuralPlanner* getPlanner() const;
    ConceptFormation* getConceptFormation() const;
    AttentionalSelection* getAttention() const;

private:
    std::shared_ptr<Brain> brain_;
};

// DevelopmentSystemWrapper: Wraps existing development system
class DevelopmentSystemWrapper : public IDevelopmentSystem {
public:
    explicit DevelopmentSystemWrapper(std::shared_ptr<Brain> brain);
    ~DevelopmentSystemWrapper() override;
    
    // IDevelopmentSystem interface
    void update(IBrainInterface* brain, class RandomGenerator& rng, TimestepDuration dt) override;
    void update(IBrainInterface* brain, SimulationStep currentStep) override;
    DevelopmentalStage getStage() const override;
    void setStage(DevelopmentalStage stage) override;
    float getPlasticityModifier() const override;
    double getDevelopmentalAge() const override;
    void setDevelopmentalAge(double age) override;
    bool isCriticalPeriod() const override;
    float getCriticalPeriodProgress() const override;
    const char* getStageName() const override;
    
    // Development system access methods
    DevelopmentSystem* getDevelopmentSystem() const;

private:
    std::shared_ptr<Brain> brain_;
};

// PlasticitySystemWrapper: Wraps existing plasticity systems
class PlasticitySystemWrapper : public IPlasticitySystem {
public:
    explicit PlasticitySystemWrapper(std::shared_ptr<Brain> brain);
    ~PlasticitySystemWrapper() override;
    
    // IPlasticitySystem interface
    void update(IBrainInterface* brain, class RandomGenerator& rng, TimestepDuration dt) override;
    SynapseId createSynapse(IBrainInterface* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight) override;
    bool removeSynapse(IBrainInterface* brain, SynapseId synapse) override;
    NeuronId createNeuron(IBrainInterface* brain, NeuronType type) override;
    bool removeNeuron(IBrainInterface* brain, NeuronId neuron) override;
    
    // Plasticity system access methods
    float getSynaptogenesisRate() const override;
    void setSynaptogenesisRate(float rate) override;
    float getPruningRate() const override;
    void setPruningRate(float rate) override;
    void configure(float ltpWeight, float ltdWeight, float tau) override;
    
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity() const;

private:
    std::shared_ptr<Brain> brain_;
};

// DynamicsSystemWrapper: Wraps existing dynamics systems
class DynamicsSystemWrapper : public IDynamicsSystem {
public:
    explicit DynamicsSystemWrapper(std::shared_ptr<Brain> brain);
    ~DynamicsSystemWrapper() override;
    
    // IDynamicsSystem interface
    void processSpikes(SimulationStep currentStep) override;
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) override;
    void queueSpike(const SpikeEvent& event) override;
    void queueDelayedSpike(const DelayedSpikeEvent& event) override;
    void registerHandler(std::function<void(const DetailedSpikeEvent&)> handler) override;
    void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)> handler) override;
    size_t getPendingSpikeCount() const override;
    size_t getPendingDelayedCount() const override;
    void reset() override;
    void setParameters(float threshold, float reset, float leak) override;
    void getParameters(float& threshold, float& reset, float& leak) const override;
    
    // Dynamics system access methods
    SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;

private:
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
