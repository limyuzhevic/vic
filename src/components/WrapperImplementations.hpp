#pragma once

#include "../core/Types/Types.hpp"
#include "../interfaces/IComponents.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// MemorySystemImpl: Wraps existing NeuralWorkingMemory, NeuralEpisodicMemory, 
// and NeuralAssociativeMemory classes to implement IMemorySystem interface
class MemorySystemImpl : public IMemorySystem {
public:
    MemorySystemImpl(std::shared_ptr<Brain> brain);
    ~MemorySystemImpl() override;
    
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

    // Memory-specific interface methods
    NeuralWorkingMemory* getWorkingMemory() const;
    NeuralEpisodicMemory* getEpisodicMemory() const;
    NeuralAssociativeMemory* getAssociativeMemory() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<IWorkingMemorySystem> workingMemory_;
    std::shared_ptr<IEpisodicMemorySystem> episodicMemory_;
    std::shared_ptr<IAssociativeMemorySystem> associativeMemory_;
};

// NeuromodulationSystemImpl: Wraps existing neuromodulation classes
// to implement INeuromodulationSystem interface
class NeuromodulationSystemImpl : public INeuromodulationSystem {
public:
    NeuromodulationSystemImpl(std::shared_ptr<Brain> brain);
    ~NeuromodulationSystemImpl() override;
    
    // INeuromodulationSystem interface
    float getLevel() const override;
    void setLevel(float level) override;
    const char* getName() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalReward(float reward) override;
    void signalError(float error) override;
    
    // Neuromodulation-specific interface methods
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionError() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<INeuromodulationSystem> dopamine_;
    std::shared_ptr<INeuromodulationSystem> curiosity_;
    std::shared_ptr<INeuromodulationSystem> novelty_;
    std::shared_ptr<INeuromodulationSystem> predictionError_;
};

// PredictionSystemImpl: Wraps existing PredictionSystem
// to implement IPredictionSystem interface
class PredictionSystemImpl : public IPredictionSystem {
public:
    PredictionSystemImpl(std::shared_ptr<Brain> brain);
    ~PredictionSystemImpl() override;
    
    // IPredictionSystem interface
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState) override;
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) override;
    float getPredictionError() const override;
    float getConfidence() const override;
    const std::vector<float>& getErrorHistory() const override;
    void clearHistory() override;
    void train(const SensoryInput& observation) override;
    void update(IBrainInterface* brain, double timestep) override;
    
    // Prediction system specific methods
    PredictionSystem* getPredictionSystem() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<IPredictionSystem> predictionSystem_;
};

// CognitionSystemImpl: Wraps existing cognition systems
// to implement ICognitionSystem interface
class CognitionSystemImpl : public ICognitionSystem {
public:
    CognitionSystemImpl(std::shared_ptr<Brain> brain);
    ~CognitionSystemImpl() override;
    
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
    
    // Cognition-specific interface methods
    NeuralPlanner* getPlanner() const;
    ConceptFormation* getConceptFormation() const;
    AttentionalSelection* getAttention() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<ICognitionSystem> planner_;
    std::shared_ptr<ICognitionSystem> conceptFormation_;
    std::shared_ptr<ICognitionSystem> attention_;
};

// DevelopmentSystemImpl: Wraps existing DevelopmentSystem
// to implement IDevelopmentSystem interface
class DevelopmentSystemImpl : public IDevelopmentSystem {
public:
    DevelopmentSystemImpl(std::shared_ptr<Brain> brain);
    ~DevelopmentSystemImpl() override;
    
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
    
    // Development-specific interface methods
    DevelopmentSystem* getDevelopmentSystem() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<IDevelopmentSystem> developmentSystem_;
};

// PlasticitySystemImpl: Wraps existing plasticity systems
// to implement IPlasticitySystem interface
class PlasticitySystemImpl : public IPlasticitySystem {
public:
    PlasticitySystemImpl(std::shared_ptr<Brain> brain);
    ~PlasticitySystemImpl() override;
    
    // IPlasticitySystem interface
    void update(IBrainInterface* brain, class RandomGenerator& rng, TimestepDuration dt) override;
    SynapseId createSynapse(IBrainInterface* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight) override;
    bool removeSynapse(IBrainInterface* brain, SynapseId synapse) override;
    NeuronId createNeuron(IBrainInterface* brain, NeuronType type) override;
    bool removeNeuron(IBrainInterface* brain, NeuronId neuron) override;
    
    // Plasticity-specific interface methods
    float getSynaptogenesisRate() const override;
    void setSynaptogenesisRate(float rate) override;
    float getPruningRate() const override;
    void setPruningRate(float rate) override;
    void configure(float ltpWeight, float ltdWeight, float tau) override;
    
    // Plasticity system access
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<IPlasticitySystem> stdp_;
    std::shared_ptr<IPlasticitySystem> hebbian_;
    std::shared_ptr<IPlasticitySystem> structuralPlasticity_;
};

// DynamicsSystemImpl: Wraps existing dynamics systems
// to implement IDynamicsSystem interface
class DynamicsSystemImpl : public IDynamicsSystem {
public:
    DynamicsSystemImpl(std::shared_ptr<Brain> brain);
    ~DynamicsSystemImpl() override;
    
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
    
    // Dynamics-specific interface methods
    SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<IDynamicsSystem> spikeSystem_;
    std::shared_ptr<IDynamicsSystem> stdp_;
    std::shared_ptr<IDynamicsSystem> hebbian_;
};

} // namespace nlm
