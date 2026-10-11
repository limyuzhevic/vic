// Brain.hpp - Enhanced version with error handling
// This header defines the Brain class with comprehensive error handling,
// validation, and robustness features for Phase 6 integrated brain systems

#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../plasticity/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/Error.hpp"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace nlm {

// Forward declarations for integrated systems
class RandomGenerator;
class SimulationClock;
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Inter-regional connection (long-range connectivity) with error handling
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
    
    // Validation for inter-region connections
    bool isValid() const {
        return sourceRegion.index() != 0 && targetRegion.index() != 0 && weight >= 0.0f && delay > 0;
    }
};

// Exception hierarchy for Brain-specific errors
class BrainInitializationError : public BrainError {
public:
    enum class Code {
        SystemNotInitialized = 3000,
        ConfigurationInvalid = 3001,
        MemoryAllocationFailed =3002,
        IntegrationFailed = 3003,
        PlasticityError = 3004
    };
    
    BrainInitializationError(const std::string& message, const std::string& context = "", Code code = Code::SystemNotInitialized)
        : BrainError(message, context, static_cast<int>(code)) {}
};

class BrainStepError : public BrainError {
public:
    enum class Code {
        SimulationFailed = 3100,
        NeuronError =3101,
        PlasticityError = 3102,
        MemoryError =3103,
        ResourceExhausted = 3104
    };
    
    BrainStepError(const std::string& message, const std::string& context = "", Code code = Code::SimulationFailed)
        : BrainError(message, context, static_cast<int>(code)) {}
};

class BrainMemoryError : public MemoryError {
public:
    enum class Code {
        WorkingMemoryFull = 2100,
        EpisodicMemoryFull = 2101,
        AssociativeMemoryError =2102,
        MemorySystemUnavailable =2103
    };
    
    BrainMemoryError(const std::string& message, const std::string& context = "", Code code = Code::WorkingMemoryFull)
        : MemoryError(message, context, static_cast<int>(code)) {}
};

class BrainConfigError : public ConfigError {
public:
    enum class Code {
        InvalidParameter = 1100,
        MissingRequiredParameter = 1101,
        TypeMismatch = 1102,
        FileLoadFailed = 1103
    };
    
    BrainConfigError(const std::string& message, const std::string& context = "", Code code = Code::InvalidParameter)
        : ConfigError(message, context, static_cast<int>(code)) {}
};

// Thread-safe error context for Brain operations
class BrainErrorContext {
public:
    static BrainErrorContext& getInstance() {
        static BrainErrorContext instance;
        return instance;
    }
    
    void pushContext(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        contextStack_.push_back(operation);
    }
    
    void popContext() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!contextStack_.empty()) {
            contextStack_.pop_back();
        }
    }
    
    std::string getCurrentContext() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (contextStack_.empty()) {
            return "";
        }
        std::ostringstream oss;
        for (size_t i = 0; i < contextStack_.size(); ++i) {
            if (i > 0) oss << ">";
            oss << contextStack_[i];
        }
        return oss.str();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        contextStack_.clear();
    }

private:
    BrainErrorContext() = default;
    ~BrainErrorContext() = default;
    BrainErrorContext(const BrainErrorContext&) = delete;
    BrainErrorContext& operator=(const BrainErrorContext&) = delete;
    
    std::vector<std::string> contextStack_;
    mutable std::mutex mutex_;
};

// Safe pointer wrapper for Brain components
template<typename T>
class BrainSafePtr {
public:
    BrainSafePtr(T* ptr = nullptr, const std::string& operation = "") : ptr_(ptr), operation_(operation) {}
    ~BrainSafePtr() = default;
    
    T* get() const { return ptr_; }
    T* operator->() const { 
        if (!ptr_) {
            throw BrainError("Null pointer access", operation_ + " -> BrainSafePtr dereference");
        }
        return ptr_; 
    }
    T& operator*() const { 
        if (!ptr_) {
            throw BrainError("Null pointer dereference", operation_ + " -> BrainSafePtr dereference");
        }
        return *ptr_; 
    }
    operator bool() const { return ptr_ != nullptr; }
    bool operator!() const { return ptr_ == nullptr; }

private:
    T* ptr_;
    std::string operation_;
};

// Configuration validation utilities for Brain
class BrainConfigValidator {
public:
    static bool validateNeuronCount(size_t neuronCount);
    static bool validateRegionCount(size_t regionCount);
    static bool validateConnectionProbability(float prob);
    static bool validateTimestep(float timestep);
    static bool validateMemoryCapacity(size_t capacity);
    static bool validateDevelopmentStage(const DevelopmentalStage& stage);
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
private:
    static constexpr size_t MIN_NEURON_COUNT = 1;
    static constexpr size_t MAX_NEURON_COUNT = 1000000;
    static constexpr size_t MIN_REGION_COUNT = 1;
    static constexpr size_t MAX_REGION_COUNT = 100;
    static constexpr float MIN_CONNECTION_PROBABILITY = 0.0f;
    static constexpr float MAX_CONNECTION_PROBABILITY = 1.0f;
    static constexpr float MIN_TIMESTEP = 0.0001f;
    static constexpr float MAX_TIMESTEP = 0.1f;
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// with comprehensive error handling and robustness features

class Brain {
public:
    // Create brain with configuration and error handling
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving with error handling
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration and validation
    bool initialize();
    
    // Main simulation step with error handling
    void step(SimulationStep currentStep);
    
    // Simulation time step with full error handling
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment with validation
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron with validation
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type with validation
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access with null checking
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access with null checking
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics with bounds checking
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity with error handling
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals with validation
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step) with error handling
    void develop();
    
    // Reset brain state with comprehensive cleanup
    void reset();
    
    // Save brain state to file (checkpointing) with error handling
    bool save(const std::string& filepath) const;
    
    // Load brain state from file with error handling
    bool load(const std::string& filepath);
    
    // Region management with validation
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management with validation
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics with error checking
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
    
    // Get current configuration with null checking
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator with null checking
    RandomGenerator* getRandomGenerator();
    
    // Logging with error handling
    void logStatus() const;
    
    // Error reporting and recovery methods
    static bool isBrainHealthy(const Brain& brain);
    static std::string getBrainHealthReport(const Brain& brain);
    static void setGracefulDegradationEnabled(bool enabled);
    static bool isGracefulDegradationEnabled();

protected:
    // Protected methods for subclasses
    bool initializeSystem(void* system, const std::string& systemName);
    void cleanupSystem(void* system, const std::string& systemName);
    bool validateConfiguration(const Config& config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety for Brain operations
    static std::mutex globalBrainMutex;
    
    // Global error reporter
    static std::unique_ptr<ErrorReporter> globalErrorReporter;
    
    // Configuration validator
    static BrainConfigValidator configValidator;
    
    // Static initializers/destructors
    static void initializeStaticComponents();
    static void cleanupStaticComponents();
    
    // Global state
    static bool gracefulDegradationEnabled;
    static bool globalErrorReportingEnabled;
    
    // Error reporter management
    static void setGlobalErrorReporter(std::unique_ptr<ErrorReporter> reporter);
    static ErrorReporter& getGlobalErrorReporter();
    
    // Thread-safe error context management
    static std::mutex errorContextMutex;
};