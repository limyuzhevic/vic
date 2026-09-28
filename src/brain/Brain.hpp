#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logging/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Config/Config.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../core/memory_management/EnhancedMemoryManagement.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>

namespace nlm {

// Forward declarations for all interconnected systems
namespace nlm {

// Core system classes - reduced redundancy by grouping related components

// Memory systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Prediction and cognition systems
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

// Development and neuromodulation systems
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Phase 2 systems
class SpikeSystem;
class Synapse;
class Neuron;

// Event-driven interfaces
class SensoryInput;
class Neuromodulator;
class Action;

// Configuration and runtime types
class Config;
class Logger;
class RandomGenerator;
class SimulationClock;

// Region management
class NeuralRegion;

// Checkpoint system
class CheckpointManager;

// Memory management utilities
class MemoryTracker;
class LeakDetector;
class GlobalMemoryManager;

// Enhanced memory pools
using NeuralResourcePool = MemoryManager::NeuralResourcePool<Neuron, 256>;
using SynapseResourcePool = MemoryManager::NeuralResourcePool<Synapse, 512>;
using PopulationResourcePool = MemoryManager::NeuralResourcePool<NeuralPopulation, 128>;

// Forward declarations for event-driven and observer pattern
class BrainObserver;
class BrainEvent;

// Type aliases for brevity
using ConfigParam = std::string;
using ObserverCallback = std::function<void(const BrainEvent&)>;

// =============================================================================
// Inter-Regional Connection Structure (Long-Range Connectivity)
// =============================================================================

/**
 * @brief Represents a connection between two brain regions
 * 
 * This structure models long-range connections that span across different neural regions,
 * enabling integrated information processing across distributed brain areas.
 * 
 * @note This structure has been optimized for performance and memory efficiency.
 *       The plasticityFlags field has been removed as it's not used in the current implementation.
 */
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    SynapticWeight weight;
    Delay delay;
    
    // Default constructor for convenience
    InterRegionConnection()
        : sourceRegion(RegionId()), targetRegion(RegionId()), 
          weight(0.0f), delay(1) {}
    
    // Parameterized constructor with default values
    InterRegionConnection(RegionId src, RegionId tgt, 
                         SynapticWeight w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d) {}
    
    /**
     * @brief Validate the connection's integrity
     * 
     * @return true if the connection is valid, false otherwise
     */
    bool isValid() const {
        return sourceRegion.value != targetRegion.value && 
               sourceRegion.value != 0 && targetRegion.value != 0;
    }
    
    /**
     * @brief Get the effective weight including potential modulation
     * 
     * @param modulationFactor Factor to modulate the weight by (default: 1.0)
     * @return Modulated weight
     */
    SynapticWeight getEffectiveWeight(float modulationFactor = 1.0f) const {
        return weight * modulationFactor;
    }
};

// =============================================================================
// Brain Event System (Observer Pattern Implementation)
// =============================================================================

/**
 * @brief Base class for all brain events
 * 
 * This forms the foundation of an observer pattern for notifying subsystems
 * about important state changes in the brain system.
 */
class BrainEvent {
public:
    virtual ~BrainEvent() = default;
    virtual const char* getEventType() const = 0;
    virtual double getTimestamp() const = 0;
};

/**
 * @brief Represents a state change event in the brain
 */
class BrainStateChange : public BrainEvent {
public:
    enum class Type {
        REGION_ADDED,
        REGION_REMOVED,
        NEURON_FIRED,
        WEIGHT_CHANGED,
        PLASTICITY_APPLIED,
        DEVELOPMENTAL_STAGE_CHANGED,
        MEMORY_STORED,
        EPISODE_REPLAYED
    };
    
    BrainStateChange(Type type, const BrainEvent* source)
        : eventType(type), sourceEvent(source) {}
    
    const char* getEventType() const override {
        return "StateChange";
    }
    
    double getTimestamp() const override {
        return static_cast<double>(eventType);
    }
    
    Type getChangeType() const { return eventType; }
    const BrainEvent* getSourceEvent() const { return sourceEvent; }
    
private:
    Type eventType;
    const BrainEvent* sourceEvent;
};

/**
 * @brief Observer interface for brain system events
 * 
 * Classes implementing this interface can receive notifications about brain events,
 * enabling the observer pattern for decoupled system communication.
 */
class BrainObserver {
public:
    virtual ~BrainObserver() = default;
    virtual void onBrainEvent(const BrainEvent& event) = 0;
    virtual void onBrainStateChange(const BrainStateChange& change) = 0;
};

// =============================================================================
// BRAIN CLASS DECLARATION
// =============================================================================

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The implementation uses the Pimpl idiom for efficient compilation and resource management.
 * The class provides a clean interface while encapsulating complex implementation details.
 * 
 * @note This is Phase 6 of the NLM implementation - Integrated Artificial Brain
 * @warning This class is not thread-safe. External synchronization is required
 *          for concurrent access from multiple threads.
 */
class Brain {
public:
    // =========================================================================
    // FACTORY METHODS AND CONSTRUCTION
    // =========================================================================
    
    /**
     * @brief Create a brain with default configuration
     * 
     * @return A shared pointer to a fully initialized Brain instance
     */
    static std::shared_ptr<Brain> createDefaultBrain();
    
    /**
     * @brief Create a brain with specified configuration
     * 
     * @param config Shared pointer to configuration object
     * @return A shared pointer to a Brain instance
     */
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    /**
     * @brief Create a brain with configuration from file
     * 
     * @param configFilePath Path to configuration file
     * @return A shared pointer to a Brain instance, or nullptr on failure
     */
    static std::shared_ptr<Brain> createBrain(const std::string& configFilePath);
    
    /**
     * @brief Create a brain builder for complex initialization
     * 
     * @return A BrainBuilder instance for fluent configuration
     */
    static BrainBuilder createBrainBuilder();
    
    // =========================================================================
    // CONSTRUCTION AND DESTRUCTION
    // =========================================================================
    
    /**
     * @brief Construct brain with configuration
     * 
     * @param config Shared pointer to configuration object
     * @throws std::invalid_argument if config is nullptr
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Properly cleans up all brain resources and systems.
     */
    ~Brain();
    
    // =========================================================================
    // COPY AND MOVE SEMANTICS
    // =========================================================================
    
    // Disable copying (Pimpl pattern)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other Brain to move from (will be left in valid but unspecified state)
     */
    Brain(Brain&& other) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other Brain to move from
     * @return Reference to this brain
     */
    Brain& operator=(Brain&& other) noexcept;
    
    // =========================================================================
    // INITIALIZATION AND CONFIGURATION
    // =========================================================================
    
    /**
     * @brief Initialize brain with configuration
     * 
     * @param config Shared pointer to configuration object
     * @return true on success, false on initialization failure
     * @throws std::runtime_error if brain is already initialized
     */
    bool initialize(std::shared_ptr<Config> config);
    
    /**
     * @brief Get the current configuration
     * 
     * @return Shared pointer to configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    // =========================================================================
    // SIMULATION CONTROL
    // =========================================================================
    
    /**
     * @brief Execute a simulation step
     * 
     * @param currentStep Current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute a simulation step with time information
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Get the current simulation time
     * 
     * @return Current simulation time
     */
    Timestamp getCurrentTime() const;
    
    /**
     * @brief Get the current simulation step
     * 
     * @return Current simulation step
     */
    SimulationStep getCurrentStep() const;
    
    // =========================================================================
    // INPUT AND OUTPUT HANDLING
    // =========================================================================
    
    /**
     * @brief Receive sensory input from environment
     * 
     * @param input Sensory input data from environment
     * @throws std::invalid_argument if input is nullptr
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Current value to inject
     * @throws std::invalid_argument if neuron ID is invalid
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type to target
     * @param current Current value to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Produce motor action based on motor neuron activity
     * 
     * @return Unique pointer to generated action
     * @return nullptr if no action was generated
     */
    std::unique_ptr<Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signal
     * 
     * @param signal Neuromodulation signal
     * @throws std::invalid_argument if signal is nullptr
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // =========================================================================
    // PLASTICITY AND DEVELOPMENT
    // =========================================================================
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * @param plasticityModulator Optional factor to modulate plasticity (default: 1.0)
     */
    void updatePlasticity(float plasticityModulator = 1.0f);
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * @param force Force developmental update even if not time
     */
    void develop(bool force = false);
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // =========================================================================
    // STATE MANAGEMENT
    // =========================================================================
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * @param preserveConfig Whether to preserve configuration settings
     */
    void reset(bool preserveConfig = true);
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @param filepath Path to checkpoint file
     * @param compress Whether to compress the checkpoint
     * @return true on success, false on failure
     * @throws std::invalid_argument if filepath is empty
     */
    bool save(const std::string& filepath, bool compress = true) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Path to checkpoint file
     * @param verifySignature Whether to verify checkpoint signature
     * @return true on success, false on failure
     * @throws std::invalid_argument if filepath is empty
     */
    bool load(const std::string& filepath, bool verifySignature = true);
    
    // =========================================================================
    // REGION MANAGEMENT
    // =========================================================================
    
    /**
     * @brief Add a new brain region
     * 
     * @param name Optional name for the region (auto-generated if empty)
     * @return Region ID of the newly created region
     * @throws std::runtime_error if region creation fails
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by ID
     * 
     * @param id Region ID to look up
     * @return Pointer to region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a region by ID (const version)
     * 
     * @param id Region ID to look up
     * @return Const pointer to region if found, nullptr otherwise
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions
     * 
     * @return Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Const reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // =========================================================================
    // INTER-REGIONAL CONNECTIONS
    // =========================================================================
    
    /**
     * @brief Add an inter-regional connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (default: 0.0)
     * @param delay Connection delay in steps (default: 1)
     * @throws std::invalid_argument if source or target are invalid
     */
    void addInterRegionConnection(RegionId source, RegionId target,
                                  SynapticWeight weight = 0.0f,
                                  Delay delay = 1);
    
    /**
     * @brief Remove an inter-regional connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // =========================================================================
    // SYSTEM ACCESSORS
    // =========================================================================
    
    /**
     * @brief Get spike system accessor
     * 
     * @return Pointer to spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system accessor (const version)
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system accessor
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system accessor
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system accessor
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // =========================================================================
    // STATISTICAL ACCESSORS
    // =========================================================================
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * @return E/I ratio (excitatory / inhibitory weights)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes since initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return Number of currently active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count for current step
     * 
     * @return Number of neurons firing in current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all regions
     * 
     * @return Average firing rate (Hz)
     */
    float getAverageFiringRate() const;
    
    // =========================================================================
    // MEMORY SYSTEMS
    // =========================================================================
    
    /**
     * @brief Get working memory system accessor
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system accessor
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system accessor
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // =========================================================================
    // PREDICTION SYSTEM
    // =========================================================================
    
    /**
     * @brief Get prediction system accessor
     */
    PredictionSystem* getPredictionSystem();
    
    // =========================================================================
    // COGNITION SYSTEMS
    // =========================================================================
    
    /**
     * @brief Get neural planner accessor
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation accessor
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection accessor
     */
    AttentionalSelection* getAttention();
    
    // =========================================================================
    // DEVELOPMENT SYSTEM
    // =========================================================================
    
    /**
     * @brief Get development system accessor
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    // =========================================================================
    // NEUROMODULATION SYSTEMS
    // =========================================================================
    
    /**
     * @brief Get dopamine system accessor
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system accessor
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection accessor
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal accessor
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get random number generator accessor
     */
    RandomGenerator* getRandomGenerator();
    
    // =========================================================================
    // OBSERVER PATTERN SUPPORT
    // =========================================================================
    
    /**
     * @brief Add an observer for brain events
     * 
     * @param observer Pointer to observer implementation
     * @throws std::invalid_argument if observer is nullptr
     */
    void addObserver(std::shared_ptr<BrainObserver> observer);
    
    /**
     * @brief Remove an observer from event notifications
     * 
     * @param observerId Unique ID of observer to remove
     */
    void removeObserver(size_t observerId);
    
    /**
     * @brief Check if an observer exists
     * 
     * @param observerId Unique ID of observer to check
     * @return true if observer exists, false otherwise
     */
    bool hasObserver(size_t observerId) const;
    
    // =========================================================================
    // DEBUGGING AND MONITORING
    // =========================================================================
    
    /**
     * @brief Log brain status and statistics
     */
    void logStatus() const;
    
    /**
     * @brief Get brain version information
     * 
     * @return String containing version information
     */
    static std::string getVersion();
    
// =========================================================================
    // PIMPL IMPLEMENTATION
    // =========================================================================
    
    struct ImplEnhanced;
    ImplEnhanced* pImpl;
    
    // =========================================================================
    // FACTORY CLASS DECLARATION (forward declaration)
    // =========================================================================
    
    class BrainBuilder;
};

// =============================================================================
// BRAIN BUILDER PATTERN CLASS
// =============================================================================

/**
 * @brief Fluent builder for complex brain initialization
 * 
 * This class provides a fluent interface for configuring brain systems
 * before initialization, allowing for complex initialization scenarios
 * that cannot be expressed through simple configuration files.
 * 
 * Example usage:
 * @code
 * BrainBuilder builder = Brain::createBrainBuilder();
 * builder.withNeuronCount(1000)
 *       .withRegionCount(5)
 *       .withPlasticityMode(PlasticityMode::STDP_HEBBIAN)
 *       .withMemoryCapacity(10000)
 *       .enableDevelopment(true);
 * auto brain = builder.build();
 * @endcode
 */
class BrainBuilder {
public:
    BrainBuilder();
    ~BrainBuilder();
    
    /**
     * @brief Set the number of neurons
     * 
     * @param count Number of neurons
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withNeuronCount(size_t count);
    
    /**
     * @brief Set the number of brain regions
     * 
     * @param count Number of regions
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withRegionCount(size_t count);
    
    /**
     * @brief Set the connection probability
     * 
     * @param probability Probability (0.0 to 1.0)
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withConnectionProbability(float probability);
    
    /**
     * @brief Enable or disable development system
     * 
     * @param enable Whether to enable development
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withDevelopment(bool enable);
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Enable or disable memory systems
     * 
     * @param enable Whether to enable memory systems
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withMemorySystems(bool enable);
    
    /**
     * @brief Set memory capacity
     * 
     * @param capacity Memory capacity
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withMemoryCapacity(size_t capacity);
    
    /**
     * @brief Enable or disable prediction system
     * 
     * @param enable Whether to enable prediction
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withPredictionSystem(bool enable);
    
    /**
     * @brief Enable or disable cognition systems
     * 
     * @param enable Whether to enable cognition
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withCognitionSystems(bool enable);
    
    /**
     * @brief Enable or disable neuromodulation systems
     * 
     * @param enable Whether to enable neuromodulation
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withNeuromodulationSystems(bool enable);
    
    /**
     * @brief Configure plasticity rule
     * 
     * @param ruleType Type of plasticity rule
     * @param parameters Rule-specific parameters
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withPlasticityRule(PlasticityRule::Type ruleType,
                                    const std::unordered_map<std::string, float>& parameters);
    
    /**
     * @brief Add a custom brain observer
     * 
     * @param observer Observer implementation
     * @return Reference to this builder for chaining
     */
    BrainBuilder& withObserver(std::shared_ptr<BrainObserver> observer);
    
    /**
     * @brief Build the configured brain
     * 
     * @return Shared pointer to configured brain
     */
    std::shared_ptr<Brain> build();
    
    /**
     * @brief Build and immediately initialize the brain
     * 
     * @return Shared pointer to initialized brain
     */
    std::shared_ptr<Brain> buildAndInitialize();
    
private:
    struct Impl;
    Impl* pImpl;
};

// =============================================================================
// INLINE IMPLEMENTATION AND UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Get the current version of the NLM Brain system
 * 
 * @return Version string
 */
inline std::string Brain::getVersion() {
    return "NLM Brain v6.0 - Integrated Artificial Brain";
}

// =============================================================================
// END OF BRAIN.HPP
// =============================================================================

} // namespace nlm

// Remove duplicate namespace declaration
namespace nlm {

// BrainBuilder implementations

struct BrainBuilder::Impl {
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool developmentEnabled;
    DevelopmentalStage developmentalStage;
    bool memoryEnabled;
    size_t memoryCapacity;
    bool predictionEnabled;
    bool cognitionEnabled;
    bool neuromodulationEnabled;
    PlasticityRule::Type plasticityRuleType;
    std::unordered_map<std::string, float> plasticityParameters;
    std::vector<std::shared_ptr<BrainObserver>> observers;
    
    Impl() : neuronCount(1000), regionCount(1), connectionProbability(0.1f),
             developmentEnabled(true), developmentalStage(DevelopmentalStage::Initial),
             memoryEnabled(true), memoryCapacity(10000), predictionEnabled(true),
             cognitionEnabled(true), neuromodulationEnabled(true),
             plasticityRuleType(PlasticityRule::Type::STDP) {}
};

BrainBuilder::BrainBuilder() : pImpl(new Impl()) {}

BrainBuilder::~BrainBuilder() {
    delete pImpl;
}

BrainBuilder& BrainBuilder::withNeuronCount(size_t count) {
    pImpl->neuronCount = count;
    return *this;
}

BrainBuilder& BrainBuilder::withRegionCount(size_t count) {
    pImpl->regionCount = count;
    return *this;
}

BrainBuilder& BrainBuilder::withConnectionProbability(float probability) {
    pImpl->connectionProbability = std::max(0.0f, std::min(1.0f, probability));
    return *this;
}

BrainBuilder& BrainBuilder::withDevelopment(bool enable) {
    pImpl->developmentEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
    return *this;
}

BrainBuilder& BrainBuilder::withMemorySystems(bool enable) {
    pImpl->memoryEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withMemoryCapacity(size_t capacity) {
    pImpl->memoryCapacity = capacity;
    return *this;
}

BrainBuilder& BrainBuilder::withPredictionSystem(bool enable) {
    pImpl->predictionEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withCognitionSystems(bool enable) {
    pImpl->cognitionEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withNeuromodulationSystems(bool enable) {
    pImpl->neuromodulationEnabled = enable;
    return *this;
}

BrainBuilder& BrainBuilder::withPlasticityRule(PlasticityRule::Type ruleType,
                                              const std::unordered_map<std::string, float>& parameters) {
    pImpl->plasticityRuleType = ruleType;
    pImpl->plasticityParameters = parameters;
    return *this;
}

BrainBuilder& BrainBuilder::withObserver(std::shared_ptr<BrainObserver> observer) {
    if (observer) {
        pImpl->observers.push_back(observer);
    }
    return *this;
}

std::shared_ptr<Brain> BrainBuilder::build() {
    auto config = std::make_shared<Config>();
    
    config->set("neuron_count", pImpl->neuronCount);
    config->set("region_count", pImpl->regionCount);
    config->set("connection_probability", pImpl->connectionProbability);
    config->set("simulation_timestep", 0.001);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    
    if (pImpl->developmentEnabled) {
        config->set("developmental_stage", static_cast<int>(pImpl->developmentalStage));
    }
    
    if (pImpl->memoryEnabled) {
        config->set("memory_capacity", pImpl->memoryCapacity);
    }
    
    auto brain = std::shared_ptr<Brain>(new Brain(config));
    
    for (const auto& observer : pImpl->observers) {
        brain->addObserver(observer);
    }
    
    return brain;
}

std::shared_ptr<Brain> BrainBuilder::buildAndInitialize() {
    auto brain = build();
    if (brain) {
        brain->initialize(brain->getConfig());
    }
    return brain;
}

std::shared_ptr<Brain> Brain::createDefaultBrain() {
    BrainBuilder builder = createBrainBuilder();
    return builder.buildAndInitialize();
}

std::shared_ptr<Brain> Brain::createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Configuration cannot be null");
    }
    
    auto brain = std::shared_ptr<Brain>(new Brain(config));
    if (brain) {
        brain->initialize(config);
    }
    return brain;
}

std::shared_ptr<Brain> Brain::createBrain(const std::string& configFilePath) {
    if (configFilePath.empty()) {
        throw std::invalid_argument("Configuration file path cannot be empty");
    }
    
    auto config = std::make_shared<Config>();
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + configFilePath);
        return nullptr;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config->set(key, value);
        }
    }
    
    return createBrain(config);
}

BrainBuilder Brain::createBrainBuilder() {
    return BrainBuilder();
}

} // namespace nlm