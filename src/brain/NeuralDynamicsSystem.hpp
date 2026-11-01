#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

class NeuralDynamicsSystem {
public:
    NeuralDynamicsSystem();
    ~NeuralDynamicsSystem();
    
    // Initialize the neural dynamics system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Process delayed spike events
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Update neuron dynamics (LIF computation)
    void updateNeuronDynamics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                              TimestepDuration timestep, Timestamp currentTime);
    
    // Detect and queue spikes
    void detectAndQueueSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             SimulationStep currentStep, Timestamp currentTime,
                             std::unique_ptr<SpikeSystem>& spikeSystem);
    
    // Apply dopamine effects on neural excitability
    void applyNeuromodulationEffects(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                    const Dopamine* dopamine);
    
    // Apply plasticity rules (STDP and Hebbian)
    void applyPlasticityRules(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             STDP* stdp, Hebbian* hebbian, float plasticityMod,
                             TimestepDuration timestep, Timestamp currentTime);
    
    // Apply homeostatic synaptic scaling
    void applySynapticScaling(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             SimulationStep currentStep);
    
    // Update working memory with neuron activations
    void updateWorkingMemory(const std::vector<Neuron*>& sensoryNeurons,
                            std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                            SimulationStep currentStep);
    
    // Get statistics
    size_t getTotalSpikeCount() const { return totalSpikes_; }
    size_t getPendingSpikeEventCount() const { return spikeSystem_->getPendingSpikeCount() + spikeSystem_->getPendingDelayedCount(); }
    float getExcitationInhibitionRatio() const;
    
    // Reset state
    void reset(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Inject current for testing
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t totalSpikes_;
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
};

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();
    
    // Initialize the memory system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Update working memory
    void updateWorkingMemory(TimestepDuration dt, SimulationStep currentStep);
    
    // Update episodic memory
    void updateEpisodicMemory(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                              SimulationStep currentStep, Timestamp currentTime,
                              const Dopamine* dopamine);
    
    // Handle memory replay
    void handleMemoryReplay(SimulationStep currentStep, SimulationStep replayInterval,
                       std::unique_ptr<NeuralEpisodicMemory>& episodicMemory);
    
    // Handle memory consolidation
    void handleMemoryConsolidation(SimulationStep currentStep,
                                  SimulationStep consolidationInterval,
                                  std::unique_ptr<NeuralEpisodicMemory>& episodicMemory);
    
    // Get statistics
    size_t getEpisodicMemoryCount() const;
    const std::vector<NeuronId>& getWorkingMemoryNeurons() const;
    
    // Reset state
    void reset();
    
    // Store to working memory
    void storeToWorkingMemory(NeuronId neuron, float activation, SimulationStep timestamp);
    
    // Get working memory state for integration
    float getWorkingMemoryState(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    SimulationStep stepsSinceLastEpisode_;
};

class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Initialize the neuromodulation system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Update neuromodulators
    void update(TimestepDuration dt, float noveltyLevel, float predictionError);
    
    // Apply neuromodulatory effects
    void applyEffects(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                     Timestamp currentTime);
    
    // Get neuromodulator levels
    float getNoveltyLevel() const;
    float getCuriosityLevel() const;
    float getDopamineLevel() const;
    float getPredictionErrorLevel() const;
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PlasticitySystem {
public:
    PlasticitySystem();
    ~PlasticitySystem();
    
    // Initialize the plasticity system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Update plasticity rules
    void update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                STDP* stdp, Hebbian* hebbian,
                float plasticityMod, TimestepDuration timestep,
                Timestamp currentTime);
    
    // Apply developmental effects on plasticity
    void applyDevelopmentEffects(StructuralPlasticity* structuralPlasticity,
                                DevelopmentalStage developmentalStage);
    
    // Handle structural plasticity
    void handleStructuralPlasticity(Brain* brain, RandomGenerator& rng,
                                   SimulationStep currentStep);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Initialize the development system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Update development
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class IntegrationSystem {
public:
    IntegrationSystem();
    ~IntegrationSystem();
    
    // Initialize the integration system
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Coordinate attention system
    void updateAttention(AttentionalSelection* attention,
                        NeuralWorkingMemory* workingMemory,
                        TimestepDuration dt);
    
    // Coordinate concept formation
    void updateConceptFormation(ConceptFormation* conceptFormation,
                                Timestamp currentTime);
    
    // Coordinate prediction system
    void updatePredictionSystem(PredictionSystem* predictionSystem);
    
    // Coordinate cognition systems
    void updateCognitionSystems(NeuralPlanner* planner,
                               ConceptFormation* conceptFormation,
                               AttentionalSelection* attention,
                               Timestamp currentTime);
    
    // Update all integrated systems
    void updateAllSystems(Brain* brain, Timestamp currentTime, TimestepDuration dt);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
