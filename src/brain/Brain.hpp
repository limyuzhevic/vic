#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../interfaces/IBrainInterface.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// BrainAdapter: Implementation of IBrainInterface
// Bridges the original Brain implementation to the interface-based architecture
class BrainAdapter : public IBrainInterface {
public:
    // Create brain with configuration
    explicit BrainAdapter(std::shared_ptr<Config> config);
    
    ~BrainAdapter() override;
    
    // Disable copying, enable moving
    BrainAdapter(const BrainAdapter&) = delete;
    BrainAdapter& operator=(const BrainAdapter&) = delete;
    BrainAdapter(BrainAdapter&&) noexcept = default;
    BrainAdapter& operator=(BrainAdapter&&) noexcept = default;
    
    // IBrainInterface implementations
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    void reset() override;
    
    RegionId addRegion(const std::string& name) override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    void receiveSensoryInput(const SensoryInput& input) override;
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    std::unique_ptr<Action> produceAction() override;
    
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    // Memory system access (via interfaces)
    IMemorySystem* getWorkingMemory() override;
    IMemorySystem* getEpisodicMemory() override;
    IMemorySystem* getAssociativeMemory() override;
    
    // Prediction system
    IPredictionSystem* getPredictionSystem() override;
    
    // Cognition systems
    ICognitionSystem* getPlanner() override;
    ICognitionSystem* getConceptFormation() override;
    ICognitionSystem* getAttention() override;
    
    // Development system
    IDevelopmentSystem* getDevelopmentSystem() override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    
    // Neuromodulation systems
    INeuromodulationSystem* getDopamine() override;
    INeuromodulationSystem* getCuriosity() override;
    INeuromodulationSystem* getNovelty() override;
    INeuromodulationSystem* getPredictionErrorSignal() override;
    
    // Plasticity and dynamics systems
    IDynamicsSystem* getSpikeSystem() override;
    IDynamicsSystem* getSTDP() override;
    IDynamicsSystem* getHebbian() override;
    IPlasticitySystem* getStructuralPlasticity() override;
    
    // Configuration and random number generation
    std::shared_ptr<const Config> getConfig() const override;
    RandomGenerator* getRandomGenerator() override;
    
    // State management
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
    // Logging and status
    void logStatus() const override;
    void applyNeuromodulation(const Neuromodulator& signal) override;
    void updatePlasticity() override;
    void develop() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Original Brain class - now delegates to adapter for interface compliance
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
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
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // Get IBrainInterface for external components
    IBrainInterface* getBrainInterface() { return brainInterface_.get(); }
    const IBrainInterface* getBrainInterface() const { return brainInterface_.get(); }

private:
    struct Impl;
    Impl* pImpl;
    std::shared_ptr<BrainAdapter> brainInterface_;
};

} // namespace nlm
