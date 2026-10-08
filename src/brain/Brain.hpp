#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <variant>

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
class Neuromodulator;
class SensoryInput;
class Action;
class SensoryPercept;

// Custom exceptions for better error handling
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class FileNotFoundError : public std::runtime_error {
public:
    explicit FileNotFoundError(const std::string& message) : std::runtime_error(message) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& message) : std::runtime_error(message) {}
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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

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
    void receiveSensoryInput(const SensoryInput& input);
    
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
    
    // Produce motor/action output based on neural activity
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
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
    
    // Prediction system for temporal sequence learning
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Cognitive processing systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development and plasticity management
    DevelopmentSystem* getDevelopmentSystem();
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Neuromodulator access methods
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    PredictionError* getPredictionError();
    Novelty* getNovelty();
    
    // ========== EVENT HANDLERS ==========
    
    // Event callback registration
    void registerSpikeHandler(std::function<void(NeuronId, Timestamp)> handler);
    void registerLearningEventHandler(std::function<void(const std::string&)> handler);
    void registerDevelopmentEventHandler(std::function<void(DevelopmentalStage)> handler);
    
    // ========== UTILITY METHODS ==========
    
    // Statistical methods
    float getStandardDeviation() const;
    float getMaximumFiringRate() const;
    float getMinimumFiringRate() const;
    
    // Configuration helpers
    void configureDefault();
    void configureForLearning();
    void configureForDevelopment();
    void configureForMemory();
    
    // State validation
    bool validateState() const;
    std::string getValidationErrors() const;
    
    // Performance monitoring
    double getExecutionTime() const;
    size_t getMemoryUsage() const;
    
    // ========== OBSERVATION AND DEBUGGING ==========
    
    // Get neural activity patterns
    std::vector<float> getNeuralActivityPattern() const;
    std::vector<float> getRegionActivity(RegionId regionId) const;
    
    // Get population statistics
    float getPopulationAverageFiringRate(PopulationId populationId) const;
    float getPopulationStandardDeviation(PopulationId populationId) const;
    
    // ========== CLONING AND COPYING ==========
    
    // Create a copy of the brain
    std::unique_ptr<Brain> clone() const;
    
    // Create a snapshot for later restoration
    std::unique_ptr<Brain> createSnapshot() const;
    
    // Restore from snapshot
    void restoreSnapshot(const Brain& snapshot);
    
    // ========== INTERNAL STATE ==========
    
    // Get current simulation state
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    
    // Check if brain is initialized
    bool isInitialized() const;
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Check if brain is in resting state
    bool isResting() const;
    
    // Get time since last activity
    double getTimeSinceLastEpisode() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
