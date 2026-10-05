#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Error.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace nlm {

// Forward declarations for circular dependencies
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
class CheckpointManager;

// Brain initialization exception
class BrainInitializationError : public NLMError {
public:
    BrainInitializationError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::System, context, errorCode) {}
};

class BrainStepError : public NLMError {
public:
    BrainStepError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Simulation, context, errorCode) {}
};

class BrainMemoryError : public NLMError {
public:
    BrainMemoryError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Memory, context, errorCode) {}
};

class BrainCheckpointError : public NLMError {
public:
    BrainCheckpointError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::System, context, errorCode) {}
};

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

// Exception-safe Brain interface
class Brain {
public:
    // Create brain with configuration - may throw BrainInitializationError
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration - may throw BrainInitializationError
    bool initialize();
    
    // Main simulation step - may throw BrainStepError
    void step(SimulationStep currentStep);
    
    // Simulation time step - may throw BrainStepError
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment - may throw BrainMemoryError
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem() { return pImpl->spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return pImpl->spikeSystem.get(); }
    
    // Plasticity system access
    STDP* getSTDP() { return pImpl->stdp.get(); }
    Hebbian* getHebbian() { return pImpl->hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }
    
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
    
    // Save brain state to file (checkpointing) - may throw BrainCheckpointError
    bool save(const std::string& filepath) const;
    
    // Load brain state from file - may throw BrainCheckpointError
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
    
    // Exception-safe validation methods
    bool isHealthy() const;
    bool hasMemorySystems() const;
    bool hasNeuromodulation() const;
    bool hasPrediction() const;
    bool hasCognition() const;
    bool hasDevelopment() const;
    
    // Get error information for debugging
    std::string getErrorSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Exception-safe initialization helper
    bool initializeImpl();
    
    // Exception-safe step helper
    void stepImpl(SimulationStep currentStep, Timestamp currentTime);
    
    // Exception-safe resource cleanup
    void cleanup() noexcept;
    
    // Validate internal state
    bool validateState() const;
    
    // Ensure required components exist
    void ensureComponentsExist() const;
};

} // namespace nlm
