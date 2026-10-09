#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace nlm {

// Forward declarations for all components
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralRegion;
class Synapse;
class Neuron;
class SensoryInput;
class Action;
class Neuromodulator;
class CheckpointManager;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection() : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
    
    bool operator==(const InterRegionConnection& other) const {
        return sourceRegion == other.sourceRegion && 
               targetRegion == other.targetRegion &&
               std::abs(weight - other.weight) < 1e-6f &&
               delay == other.delay;
    }
};

// Template utility functions for type-safe operations
template<typename T>
T* safeGet(void* objPtr) {
    return static_cast<T*>(objPtr);
}

// Configuration template for type-safe parameter retrieval
template<typename T>
T getConfigParameter(const Config& config, std::string_view key, T defaultValue) {
    if constexpr (std::is_integral_v<T>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else if constexpr (std::is_floating_point_v<T>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else {
        static_assert(!std::is_same_v<T, T>, "Type not supported for configuration retrieval");
        return defaultValue;
    }
}

// Concept for component initialization
class Initializable {
public:
    virtual ~Initializable() = default;
    virtual bool initialize() = 0;
    virtual void reset() = 0;
};

// Component specialization base
class BrainComponent : public Initializable {
protected:
    bool initialized = false;
    explicit BrainComponent() = default;
public:
    virtual ~BrainComponent() = default;
    bool isInitialized() const noexcept { return initialized; }
    virtual bool configure(const Config& config) { return true; }
};

// Brain: The central coordinator of the neural system
// Uses modern C++ with RAII, PIMPL pattern, and exception safety
class Brain {
public:
    // Constructor with configuration - uses std::unique_ptr internally
    explicit Brain(std::shared_ptr<Config> config);
    
    // Modern C++ destructor with noexcept
    ~Brain() noexcept;
    
    // Delete copy operations, enable move semantics
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Initialize brain with configuration - noexcept(false) for error handling
    [[nodiscard]] bool initialize() noexcept(false);
    
    // Main simulation step with different time representations
    void step(SimulationStep currentStep) noexcept;
    void step(SimulationStep currentStep, Timestamp currentTime) noexcept;
    
    // Sensory input handling
    void receiveSensoryInput(const SensoryInput& input) noexcept;
    void injectCurrent(NeuronId neuron, MembranePotential current) noexcept;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) noexcept;
    
    // Component accessors with null safety
    [[nodiscard]] SpikeSystem* getSpikeSystem() const noexcept;
    [[nodiscard]] STDP* getSTDP() const noexcept;
    [[nodiscard]] Hebbian* getHebbian() const noexcept;
    [[nodiscard]] StructuralPlasticity* getStructuralPlasticity() const noexcept;
    
    // Statistics
    [[nodiscard]] float getExcitationInhibitionRatio() const noexcept;
    [[nodiscard]] size_t getTotalSpikeCount() const noexcept;
    [[nodiscard]] size_t getPendingSpikeEventCount() const noexcept;
    
    // Action production
    [[nodiscard]] std::unique_ptr<Action> produceAction() noexcept;
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal) noexcept;
    
    // Plasticity and development control
    void updatePlasticity() noexcept;
    void develop() noexcept;
    
    // State management
    void reset() noexcept;
    
    // Persistence
    [[nodiscard]] bool save(const std::string& filepath) const noexcept(false);
    [[nodiscard]] bool load(const std::string& filepath) noexcept(false);
    
    // Region management
    [[nodiscard]] RegionId addRegion(std::string_view name = "") noexcept;
    [[nodiscard]] NeuralRegion* getRegion(RegionId id) const noexcept;
    [[nodiscard]] const NeuralRegion* getRegion(RegionId id) const noexcept;
    [[nodiscard]] size_t getRegionCount() const noexcept;
    [[nodiscard]] std::vector<RegionId> getRegionIds() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const noexcept;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) noexcept;
    void removeInterRegionConnection(RegionId source, RegionId target) noexcept;
    
    // Global statistics
    [[nodiscard]] size_t getTotalNeuronCount() const noexcept;
    [[nodiscard]] size_t getTotalSynapseCount() const noexcept;
    [[nodiscard]] size_t getActiveNeuronCount() const noexcept;
    [[nodiscard]] size_t getFiringNeuronCount() const noexcept;
    [[nodiscard]] float getAverageFiringRate() const noexcept;
    
    // Memory systems
    [[nodiscard]] NeuralWorkingMemory* getWorkingMemory() const noexcept;
    [[nodiscard]] NeuralEpisodicMemory* getEpisodicMemory() const noexcept;
    [[nodiscard]] NeuralAssociativeMemory* getAssociativeMemory() const noexcept;
    
    // Prediction system
    [[nodiscard]] PredictionSystem* getPredictionSystem() const noexcept;
    
    // Cognition systems
    [[nodiscard]] NeuralPlanner* getPlanner() const noexcept;
    [[nodiscard]] ConceptFormation* getConceptFormation() const noexcept;
    [[nodiscard]] AttentionalSelection* getAttention() const noexcept;
    
    // Development system
    [[nodiscard]] DevelopmentSystem* getDevelopmentSystem() const noexcept;
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const noexcept;
    void setDevelopmentalStage(DevelopmentalStage stage) noexcept;
    
    // Neuromodulation systems
    [[nodiscard]] Dopamine* getDopamine() const noexcept;
    [[nodiscard]] Curiosity* getCuriosity() const noexcept;
    [[nodiscard]] Novelty* getNovelty() const noexcept;
    [[nodiscard]] PredictionError* getPredictionErrorSignal() const noexcept;
    
    // Getters
    [[nodiscard]] std::shared_ptr<const Config> getConfig() const noexcept;
    [[nodiscard]] RandomGenerator* getRandomGenerator() const noexcept;
    
    // BrainFactory - Configuration-based brain creation helper class
class BrainFactory {
public:
    // Configuration validation
    class ConfigurationValidator {
    public:
        static bool validate(const Config& config);
    };
    
    // Create brain from configuration
    static std::shared_ptr<Brain> create(std::shared_ptr<Config> config);
    
    // Create default configuration
    static std::shared_ptr<Config> createDefaultConfig();
    
    // Create configuration for specific use case
    template<typename... Args>
    static std::shared_ptr<Config> createConfigForUseCase(
        std::string_view useCase,
        Args&&... args);
};

// Forward declarations for all components
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralRegion;
class Synapse;
class Neuron;
class SensoryInput;
class Action;
class Neuromodulator;
class CheckpointManager;

// BrainFactory - Configuration-based brain creation helper class
class BrainFactory {
public:
    // Configuration validation
    class ConfigurationValidator {
    public:
        static bool validate(const Config& config);
    };
    
    // Create brain from configuration
    static std::shared_ptr<Brain> create(std::shared_ptr<Config> config);
    
    // Create default configuration
    static std::shared_ptr<Config> createDefaultConfig();
    
    // Create configuration for specific use case
    template<typename... Args>
    static std::shared_ptr<Config> createConfigForUseCase(
        std::string_view useCase,
        Args&&... args);
};

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection() : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
    
    bool operator==(const InterRegionConnection& other) const {
        return sourceRegion == other.sourceRegion && 
               targetRegion == other.targetRegion &&
               std::abs(weight - other.weight) < 1e-6f &&
               delay == other.delay;
    }
};

// Template utility functions for type-safe operations
template<typename T>
T* safeGet(void* objPtr) {
    return static_cast<T*>(objPtr);
}

// Configuration template for type-safe parameter retrieval
template<typename T>
T getConfigParameter(const Config& config, std::string_view key, T defaultValue) {
    if constexpr (std::is_integral_v<T>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else if constexpr (std::is_floating_point_v<T>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return config.getOr<T>(std::string(key), defaultValue);
    } else {
        static_assert(!std::is_same_v<T, T>, "Type not supported for configuration retrieval");
        return defaultValue;
    }
}

// Concept for component initialization
class Initializable {
public:
    virtual ~Initializable() = default;
    virtual bool initialize() = 0;
    virtual void reset() = 0;
};

// Component specialization base
class BrainComponent : public Initializable {
protected:
    bool initialized = false;
    explicit BrainComponent() = default;
public:
    virtual ~BrainComponent() = default;
    bool isInitialized() const noexcept { return initialized; }
    virtual bool configure(const Config& config) { return true; }
};

// Brain: The central coordinator of the neural system
// Uses modern C++ with RAII, PIMPL pattern, and exception safety
class Brain {
public:
    // Constructor with configuration - uses std::unique_ptr internally
    explicit Brain(std::shared_ptr<Config> config);
    
    // Modern C++ destructor with noexcept
    ~Brain() noexcept;
    
    // Delete copy operations, enable move semantics
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Initialize brain with configuration - noexcept(false) for error handling
    [[nodiscard]] bool initialize() noexcept(false);
    
    // Main simulation step with different time representations
    void step(SimulationStep currentStep) noexcept;
    void step(SimulationStep currentStep, Timestamp currentTime) noexcept;
    
    // Sensory input handling
    void receiveSensoryInput(const SensoryInput& input) noexcept;
    void injectCurrent(NeuronId neuron, MembranePotential current) noexcept;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) noexcept;
    
    // Component accessors with null safety
    [[nodiscard]] SpikeSystem* getSpikeSystem() const noexcept;
    [[nodiscard]] STDP* getSTDP() const noexcept;
    [[nodiscard]] Hebbian* getHebbian() const noexcept;
    [[nodiscard]] StructuralPlasticity* getStructuralPlasticity() const noexcept;
    
    // Statistics
    [[nodiscard]] float getExcitationInhibitionRatio() const noexcept;
    [[nodiscard]] size_t getTotalSpikeCount() const noexcept;
    [[nodiscard]] size_t getPendingSpikeEventCount() const noexcept;
    
    // Action production
    [[nodiscard]] std::unique_ptr<Action> produceAction() noexcept;
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal) noexcept;
    
    // Plasticity and development control
    void updatePlasticity() noexcept;
    void develop() noexcept;
    
    // State management
    void reset() noexcept;
    
    // Persistence
    [[nodiscard]] bool save(const std::string& filepath) const noexcept(false);
    [[nodiscard]] bool load(const std::string& filepath) noexcept(false);
    
    // Region management
    [[nodiscard]] RegionId addRegion(std::string_view name = "") noexcept;
    [[nodiscard]] NeuralRegion* getRegion(RegionId id) const noexcept;
    [[nodiscard]] const NeuralRegion* getRegion(RegionId id) const noexcept;
    [[nodiscard]] size_t getRegionCount() const noexcept;
    [[nodiscard]] std::vector<RegionId> getRegionIds() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const noexcept;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) noexcept;
    void removeInterRegionConnection(RegionId source, RegionId target) noexcept;
    
    // Global statistics
    [[nodiscard]] size_t getTotalNeuronCount() const noexcept;
    [[nodiscard]] size_t getTotalSynapseCount() const noexcept;
    [[nodiscard]] size_t getActiveNeuronCount() const noexcept;
    [[nodiscard]] size_t getFiringNeuronCount() const noexcept;
    [[nodiscard]] float getAverageFiringRate() const noexcept;
    
    // Memory systems
    [[nodiscard]] NeuralWorkingMemory* getWorkingMemory() const noexcept;
    [[nodiscard]] NeuralEpisodicMemory* getEpisodicMemory() const noexcept;
    [[nodiscard]] NeuralAssociativeMemory* getAssociativeMemory() const noexcept;
    
    // Prediction system
    [[nodiscard]] PredictionSystem* getPredictionSystem() const noexcept;
    
    // Cognition systems
    [[nodiscard]] NeuralPlanner* getPlanner() const noexcept;
    [[nodiscard]] ConceptFormation* getConceptFormation() const noexcept;
    [[nodiscard]] AttentionalSelection* getAttention() const noexcept;
    
    // Development system
    [[nodiscard]] DevelopmentSystem* getDevelopmentSystem() const noexcept;
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const noexcept;
    void setDevelopmentalStage(DevelopmentalStage stage) noexcept;
    
    // Neuromodulation systems
    [[nodiscard]] Dopamine* getDopamine() const noexcept;
    [[nodiscard]] Curiosity* getCuriosity() const noexcept;
    [[nodiscard]] Novelty* getNovelty() const noexcept;
    [[nodiscard]] PredictionError* getPredictionErrorSignal() const noexcept;
    
    // Getters
    [[nodiscard]] std::shared_ptr<const Config> getConfig() const noexcept;
    [[nodiscard]] RandomGenerator* getRandomGenerator() const noexcept;
    
    // Status and logging
    void logStatus() const noexcept;

private:
    // PIMPL implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Factory method for implementation creation
    static std::unique_ptr<Impl> createImpl(std::shared_ptr<Config> config);
};