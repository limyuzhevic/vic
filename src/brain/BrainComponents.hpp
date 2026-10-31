// Brain system component headers
#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class StepManager;
class PlasticityManager;
class MemoryManager;
class NeuromodulationManager;
class DevelopmentManager;
class SensoryMotorManager;
class CheckpointManager;

// Brain system configuration
struct BrainSystemConfig {
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    std::string checkpointDir = "./checkpoints";
    size_t replayInterval = 100;
    size_t consolidationInterval = 1000;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    
    // Plasticity parameters
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Memory parameters
    size_t workingMemoryCapacity = 100;
    size_t maxEpisodes = 1000;
    
    // Neuromodulation parameters
    float noveltyThreshold = 0.5f;
    float curiosityLevel = 0.3f;
    float dopamineBaseline = 0.1f;
    
    // Development parameters
    size_t developmentInterval = 1000;
    float initialPlasticity = 1.0f;
    float criticalPeriodFactor = 0.8f;
};

// Component interface for Brain system
class IBrainComponent {
public:
    virtual ~IBrainComponent() = default;
    virtual bool initialize() = 0;
    virtual void reset() = 0;
    virtual void update() = 0;
    virtual void logStatus() const = 0;
};

// Step manager - coordinates simulation loop
class StepManager : public IBrainComponent {
public:
    StepManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Simulation control
    void advanceStep(SimulationStep step, Timestamp time);
    void processDelayedSpikes(SimulationStep step, Timestamp time);
    void processSpikes(SimulationStep step);
    
    // Statistics
    size_t getTotalSpikeCount() const { return totalSpikes_; }
    size_t getFiringNeuronCount() const { return firingNeurons_; }
    float getAverageFiringRate() const { return averageFiringRate_; }
    
    // Event handling
    void registerSpikeHandler(std::function<void(const DetailedSpikeEvent&)> handler);
    void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)> handler);
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    size_t totalSpikes_;
    size_t firingNeurons_;
    float averageFiringRate_;
    std::vector<std::function<void(const DetailedSpikeEvent&)>> spikeHandlers_;
    std::vector<std::function<void(const DelayedSpikeEvent&)>> delayedHandlers_;
};

// Plasticity manager - handles learning and synaptic changes
class PlasticityManager : public IBrainComponent {
public:
    PlasticityManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Plasticity control
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    void applySTDP(SimulationStep step, Timestamp time);
    void applyHebbianLearning(SimulationStep step, Timestamp time);
    void applyStructuralPlasticity(SimulationStep step, Timestamp time);
    
    // Plasticity parameters
    void setSTDPParameters(float ltpWeight, float ltdWeight, float tau);
    void setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate);
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    bool hebbianEnabled_;
    bool stdpEnabled_;
    bool rewardModulated_;
};

// Memory manager - coordinates memory systems
class MemoryManager : public IBrainComponent {
public:
    MemoryManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Memory operations
    void storeExperience(const ExperienceItem& experience);
    void updateWorkingMemory(SimulationStep step, Timestamp time);
    void updateEpisodicMemory(SimulationStep step, Timestamp time);
    void updateAssociativeMemory(SimulationStep step, Timestamp time);
    
    // Memory statistics
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemorySize() const;
    size_t getAssociativeMemorySize() const;
    
    // Replay and consolidation
    void replayMemory(SimulationStep step, Timestamp time);
    void consolidateMemory(SimulationStep step, Timestamp time);
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
};

// Neuromodulation manager - coordinates neuromodulation systems
class NeuromodulationManager : public IBrainComponent {
public:
    NeuromodulationManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Neuromodulation control
    void updateDopamine(SimulationStep step, Timestamp time);
    void updateCuriosity(SimulationStep step, Timestamp time);
    void updateNovelty(SimulationStep step, Timestamp time);
    void updatePredictionError(SimulationStep step, Timestamp time);
    
    // Neuromodulation effects
    void applyDopamineEffects(SimulationStep step, Timestamp time);
    void applyCuriosityEffects(SimulationStep step, Timestamp time);
    
    // Neuromodulation signals
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
};

// Development manager - coordinates developmental processes
class DevelopmentManager : public IBrainComponent {
public:
    DevelopmentManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Development control
    void applyDevelopment(SimulationStep step, Timestamp time);
    void updatePlasticityModulation(SimulationStep step, Timestamp time);
    
    // Development stages
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Development effects
    float getPlasticityModulation() const;
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    DevelopmentalStage currentStage_;
    float plasticityModulation_;
    SimulationStep developmentCounter_;
};

// Sensory-motor manager - handles input/output
class SensoryMotorManager : public IBrainComponent {
public:
    SensoryMotorManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Input handling
    void processSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Output generation
    std::unique_ptr<Action> generateAction() const;
    
    // Neuron access
    const std::vector<Neuron*>& getSensoryNeurons() const { return sensoryNeurons_; }
    const std::vector<Neuron*>& getMotorNeurons() const { return motorNeurons_; }
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
};

// Checkpoint manager - handles state persistence
class CheckpointManager : public IBrainComponent {
public:
    CheckpointManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config);
    bool initialize() override;
    void reset() override;
    void update() override;
    void logStatus() const override;
    
    // Checkpoint control
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Checkpoint management
    void update(SimulationStep step, Timestamp time);
    bool shouldCheckpoint(SimulationStep step) const;
    
private:
    std::shared_ptr<Brain> brain_;
    BrainSystemConfig config_;
    std::unique_ptr<CheckpointManager> checkpointSystem_;
    SimulationStep lastCheckpointStep_;
};

// Enhanced Brain class with component architecture
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Component access
    StepManager* getStepManager() { return stepManager_.get(); }
    PlasticityManager* getPlasticityManager() { return plasticityManager_.get(); }
    MemoryManager* getMemoryManager() { return memoryManager_.get(); }
    NeuromodulationManager* getNeuromodulationManager() { return neuromodulationManager_.get(); }
    DevelopmentManager* getDevelopmentManager() { return developmentManager_.get(); }
    SensoryMotorManager* getSensoryMotorManager() { return sensoryMotorManager_.get(); }
    CheckpointManager* getCheckpointManager() { return checkpointManager_.get(); }
    
    // Original Brain interface (backward compatibility)
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    void logStatus() const;
    
private:
    std::shared_ptr<Config> config_;
    BrainSystemConfig systemConfig_;
    
    // Component instances
    std::unique_ptr<StepManager> stepManager_;
    std::unique_ptr<PlasticityManager> plasticityManager_;
    std::unique_ptr<MemoryManager> memoryManager_;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager_;
    std::unique_ptr<DevelopmentManager> developmentManager_;
    std::unique_ptr<SensoryMotorManager> sensoryMotorManager_;
    std::unique_ptr<CheckpointManager> checkpointManager_;
    
    // Original brain components (for backward compatibility)
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm