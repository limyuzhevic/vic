#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <span>
#include <optional>
#include <string_view>
#include <chrono>
#include <concepts>
#include <expected>
#include <compare>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept PointerToNeuron = std::is_pointer_v<T> && 
                         std::is_convertible_v<T, Neuron*>;

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: In-place new type for perfect forwarding
template<typename T>
using InPlace = std::in_place_t;

// C++20: Union type for discriminated unions
template<typename... Types>
using Variant = std::variant<Types...>;

// Forward declarations for better encapsulation
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

// C++20: Enum class with underlying type for explicit control
enum class StorageBackend : uint8_t {
    InMemory = 0,
    File = 1,
    MemoryMapped = 2,
    Distributed = 3
};

// C++20: Optional checkpoint configuration using std::optional
struct CheckpointConfig {
    std::string directory;
    size_t maxCheckpoints;
    size_t retentionPeriod;
    bool compressionEnabled;
    
    CheckpointConfig(std::string dir = "./checkpoints", 
                     size_t max = 10000, 
                     size_t retention = 5,
                     bool compressed = true)
        : directory(std::move(dir))
        , maxCheckpoints(max)
        , retentionPeriod(retention)
        , compressionEnabled(compressed) {}
};

// C++20: Use span for better memory safety
struct BrainStatistics {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t totalSpikes;
    std::chrono::nanoseconds simulationTime;
    
    // Calculate derived metrics
    [[nodiscard]] float getSpikeRatePerSecond() const noexcept {
        return static_cast<float>(totalSpikes) / 
               (simulationTime.count() / 1'000'000'000.0f);
    }
    
    // C++20: Structured binding for clarity
    void print() const {
        printf("Brain Statistics:\n");
        printf("  Total Neurons: %zu\n", totalNeurons);
        printf("  Total Synapses: %zu\n", totalSynapses);
        printf("  Active Neurons: %zu\n", activeNeurons);
        printf("  Firing Neurons: %zu\n", firingNeurons);
        printf("  Average Firing Rate: %.3f Hz\n", averageFiringRate);
        printf("  E/I Ratio: %.3f\n", excitationInhibitionRatio);
        printf("  Total Spikes: %zu\n", totalSpikes);
        printf("  Spike Rate: %.3f per second\n", getSpikeRatePerSecond());
    }
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
    
    // C++20: Explicitly defaulted copy/move operations
    InterRegionConnection(const InterRegionConnection&) = default;
    InterRegionConnection& operator=(const InterRegionConnection&) = default;
    InterRegionConnection(InterRegionConnection&&) noexcept = default;
    InterRegionConnection& operator=(InterRegionConnection&&) noexcept = default;
    
    // C++20: Three-way comparison for sorting
    auto operator<=>(const InterRegionConnection&) const = default;
    bool operator==(const InterRegionConnection&) const = default;
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // C++20: Use in-class member initializer lists for better readability
    explicit Brain(std::shared_ptr<Config> config)
        : pImpl(std::make_unique<Impl>(std::move(config))) {};
    
    ~Brain() = default;
    
    // C++20: Delete copy, enable move for efficiency
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept = default;
    Brain& operator=(Brain&&) noexcept = default;
    
    // C++20: Explicit boolean conversion
    explicit operator bool() const noexcept { return pImpl != nullptr; }
    
    // C++20: Attributes for better documentation
    [[nodiscard]] bool initialize() &; // Add& to indicate non-const reference
    [[nodiscard]] bool initialize() const& = delete; // Delete const version
    
    // Main simulation step
    void step(SimulationStep currentStep) & noexcept; // Add& to indicate non-const reference
    void step(SimulationStep currentStep, Timestamp currentTime) & noexcept;
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input) & noexcept;
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current) & noexcept;
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) & noexcept;
    
    // Spike system access
    [[nodiscard]] SpikeSystem* getSpikeSystem() noexcept { return pImpl->spikeSystem.get(); }
    [[nodiscard]] const SpikeSystem* getSpikeSystem() const noexcept { return pImpl->spikeSystem.get(); }
    
    // Plasticity system access
    [[nodiscard]] STDP* getSTDP() noexcept { return pImpl->stdp.get(); }
    [[nodiscard]] const STDP* getSTDP() const noexcept { return pImpl->stdp.get(); }
    [[nodiscard]] Hebbian* getHebbian() noexcept { return pImpl->hebbian.get(); }
    [[nodiscard]] const Hebbian* getHebbian() const noexcept { return pImpl->hebbian.get(); }
    [[nodiscard]] StructuralPlasticity* getStructuralPlasticity() noexcept { return pImpl->structuralPlasticity.get(); }
    [[nodiscard]] const StructuralPlasticity* getStructuralPlasticity() const noexcept { return pImpl->structuralPlasticity.get(); }
    
    // Statistics
    [[nodiscard]] float getExcitationInhibitionRatio() const noexcept;
    [[nodiscard]] size_t getTotalSpikeCount() const noexcept;
    [[nodiscard]] size_t getPendingSpikeEventCount() const noexcept;
    
    // Produce motor/action output based on motor neuron activity
    [[nodiscard]] std::unique_ptr<class Action> produceAction() & noexcept;
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal) & noexcept;
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity() & noexcept;
    
    // Apply developmental changes (called automatically in step)
    void develop() & noexcept;
    
    // Reset brain state
    void reset() & noexcept;
    
    // Save brain state to file (checkpointing)
    [[nodiscard]] bool save(const std::string& filepath) const &;
    [[nodiscard]] bool save(const std::string& filepath) && = delete; // Move-only for saving
    
    // Load brain state from file
    [[nodiscard]] bool load(const std::string& filepath) & noexcept;
    
    // Region management
    [[nodiscard]] RegionId addRegion(const std::string& name = "");
    [[nodiscard]] NeuralRegion* getRegion(RegionId id) noexcept { return pImpl->getRegion(id); }
    [[nodiscard]] const NeuralRegion* getRegion(RegionId id) const noexcept { return pImpl->getRegion(id); }
    [[nodiscard]] size_t getRegionCount() const noexcept;
    [[nodiscard]] std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    [[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const noexcept;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) & noexcept;
    void removeInterRegionConnection(RegionId source, RegionId target) & noexcept;
    
    // Global statistics
    [[nodiscard]] size_t getTotalNeuronCount() const noexcept;
    [[nodiscard]] size_t getTotalSynapseCount() const noexcept;
    [[nodiscard]] size_t getActiveNeuronCount() const noexcept;
    [[nodiscard]] size_t getFiringNeuronCount() const noexcept;
    [[nodiscard]] float getAverageFiringRate() const noexcept;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    [[nodiscard]] NeuralWorkingMemory* getWorkingMemory() noexcept { return pImpl->workingMemory.get(); }
    [[nodiscard]] const NeuralWorkingMemory* getWorkingMemory() const noexcept { return pImpl->workingMemory.get(); }
    
    // Episodic memory - experience storage
    [[nodiscard]] NeuralEpisodicMemory* getEpisodicMemory() noexcept { return pImpl->episodicMemory.get(); }
    [[nodiscard]] const NeuralEpisodicMemory* getEpisodicMemory() const noexcept { return pImpl->episodicMemory.get(); }
    
    // Associative memory - pattern associations
    [[nodiscard]] NeuralAssociativeMemory* getAssociativeMemory() noexcept { return pImpl->associativeMemory.get(); }
    [[nodiscard]] const NeuralAssociativeMemory* getAssociativeMemory() const noexcept { return pImpl->associativeMemory.get(); }
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    [[nodiscard]] PredictionSystem* getPredictionSystem() noexcept { return pImpl->predictionSystem.get(); }
    [[nodiscard]] const PredictionSystem* getPredictionSystem() const noexcept { return pImpl->predictionSystem.get(); }
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    [[nodiscard]] NeuralPlanner* getPlanner() noexcept { return pImpl->planner.get(); }
    [[nodiscard]] const NeuralPlanner* getPlanner() const noexcept { return pImpl->planner.get(); }
    
    // Concept formation for pattern discovery
    [[nodiscard]] ConceptFormation* getConceptFormation() noexcept { return pImpl->conceptFormation.get(); }
    [[nodiscard]] const ConceptFormation* getConceptFormation() const noexcept { return pImpl->conceptFormation.get(); }
    
    // Attentional selection for focus
    [[nodiscard]] AttentionalSelection* getAttention() noexcept { return pImpl->attention.get(); }
    [[nodiscard]] const AttentionalSelection* getAttention() const noexcept { return pImpl->attention.get(); }
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system
    [[nodiscard]] DevelopmentSystem* getDevelopmentSystem() noexcept { return pImpl->developmentSystem.get(); }
    [[nodiscard]] const DevelopmentSystem* getDevelopmentSystem() const noexcept { return pImpl->developmentSystem.get(); }
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const noexcept;
    void setDevelopmentalStage(DevelopmentalStage stage) noexcept;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    [[nodiscard]] Dopamine* getDopamine() noexcept { return pImpl->dopamine.get(); }
    [[nodiscard]] const Dopamine* getDopamine() const noexcept { return pImpl->dopamine.get(); }
    
    // Curiosity - exploration motivation
    [[nodiscard]] Curiosity* getCuriosity() noexcept { return pImpl->curiosity.get(); }
    [[nodiscard]] const Curiosity* getCuriosity() const noexcept { return pImpl->curiosity.get(); }
    
    // Novelty - novelty detection
    [[nodiscard]] Novelty* getNovelty() noexcept { return pImpl->novelty.get(); }
    [[nodiscard]] const Novelty* getNovelty() const noexcept { return pImpl->novelty.get(); }
    
    // Prediction error signal
    [[nodiscard]] PredictionError* getPredictionErrorSignal() noexcept { return pImpl->predictionError.get(); }
    [[nodiscard]] const PredictionError* getPredictionErrorSignal() const noexcept { return pImpl->predictionError.get(); }
    
    // Get current configuration
    [[nodiscard]] std::shared_ptr<const Config> getConfig() const noexcept;
    
    // Get random generator
    [[nodiscard]] RandomGenerator* getRandomGenerator() noexcept { return pImpl->rng.get(); }
    [[nodiscard]] const RandomGenerator* getRandomGenerator() const noexcept { return pImpl->rng.get(); }
    
    // Logging
    void logStatus() const noexcept;
    
    // Statistics helper
    [[nodiscard]] BrainStatistics getStatistics() const noexcept;
    
    // Configuration for checkpoint system
    [[nodiscard]] CheckpointConfig getCheckpointConfig() const noexcept;
    void configureCheckpoint(const CheckpointConfig& config) & noexcept;
    
    // C++20: Factory method for creating brain with common configurations
    static Result<std::unique_ptr<Brain>> createStandardBrain(const std::string& configFile = "");
    
    // C++20: Visitor pattern for brain operations
    template<typename Visitor>
    void visit(Visitor&& visitor) & {
        visitor.visitRegions(*this);
        visitor.visitMemorySystems(*this);
        visitor.visitNeuromodulation(*this);
        visitor.visitDevelopment(*this);
    }
    
private:
    // PIMPL implementation
    struct Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        // ========== INTEGRATED MEMORY SYSTEMS ==========
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        
        // ========== INTEGRATED PREDICTION SYSTEM ==========
        std::unique_ptr<PredictionSystem> predictionSystem;
        
        // ========== INTEGRATED COGNITION SYSTEMS ==========
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attention;
        
        // ========== DEVELOPMENT SYSTEM ==========
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        
        // ========== NEUROMODULATION SYSTEMS ==========
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<PredictionError> predictionError;
        std::unique_ptr<Novelty> novelty;
        
        // Phase 2: Real neural computation components
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hebbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        // Simulation parameters
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        
        // Statistics
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        
        // Sensory neurons for input injection
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        // Integration state
        bool isResting;  // For sleep/rest cycle
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Checkpoint system
        std::unique_ptr<CheckpointManager> checkpointManager;
        CheckpointConfig checkpointConfig;
        
        Impl(std::shared_ptr<Config> cfg);
        
        // Helper methods for accessing regions
        NeuralRegion* getRegion(RegionId id) noexcept;
        const NeuralRegion* getRegion(RegionId id) const noexcept;
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
