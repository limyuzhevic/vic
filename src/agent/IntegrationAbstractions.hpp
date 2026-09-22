#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <span>
#include <expected>
#include <concepts>
#include <optional>
#include <vector>
#include <map>
#include <string>
#include <compare>
#include <coroutine>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept PointerToSensory = std::is_pointer_v<T> && 
                          std::is_convertible_v<T, const SensoryPercept*>;

// C++20: Type aliases for common patterns using std::span and std::expected
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: In-place new type for perfect forwarding
template<typename T>
using InPlace = std::in_place_t;

// Forward declarations for better encapsulation
class RandomGenerator;
class Neuron;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class Dopamine;
class Curiosity;
class Novelty;

// C++20: Three-way comparison for sorting
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

// Abstract base class for sensory processing and transduction
class ISensoryProcessor {
public:
    virtual ~ISensoryProcessor() = default;
    
    // Initialize the sensory processor
    virtual void initialize() = 0;
    
    // Process raw sensory input into percept
    virtual SensoryPercept processInput(const AgentBody& agent, const SimpleWorld& world) = 0;
    
    // Get expected sensory input size for brain integration
    virtual size_t getInputSize() const = 0;
    
    // Get sensory modality name (for debugging/logging)
    virtual const char* getModalityName() const = 0;
    
    // Check if this modality is enabled
    virtual bool isEnabled() const = 0;
    
    // Enable/disable modality
    virtual void enable(bool enabled) = 0;
    
    // Get contribution weights for different brain regions
    virtual const std::map<RegionId, float>& getRegionWeights() const = 0;
};

// Abstract base class for brain-motor interface
class IMotorController {
public:
    virtual ~IMotorController() = default;
    
    // Initialize motor controller
    virtual void initialize(const Brain& brain) = 0;
    
    // Decode motor commands from neural activity
    virtual MotorCommand decodeCommand(const std::vector<float>& neuralActivity) = 0;
    
    // Get expected motor output size
    virtual size_t getOutputSize() const = 0;
    
    // Apply neuromodulation to motor output
    virtual void applyNeuromodulation(MotorCommand& command, float neuromodulationLevel) = 0;
    
    // Check if motor control is enabled
    virtual bool isEnabled() const = 0;
    
    // Enable/disable motor control
    virtual void enable(bool enabled) = 0;
};

// Abstract base class for neural integration and processing
class INeuralIntegrator {
public:
    virtual ~INeuralIntegrator() = default;
    
    // Initialize integrator
    virtual void initialize(const Brain& brain) = 0;
    
    // Process neural state and update internal representation
    virtual void update(double timestep) = 0;
    
    // Get neural statistics for monitoring
    virtual void getStatistics(std::map<std::string, float>& stats) const = 0;
    
    // Check for developmental milestones
    virtual bool hasReachedDevelopmentalStage(DevelopmentalStage stage) const = 0;
    
    // Get current developmental stage
    virtual DevelopmentalStage getCurrentStage() const = 0;
};

// Concrete implementation of sensory processing for integrated systems
class IntegratedSensoryProcessor : public ISensoryProcessor {
public:
    IntegratedSensoryProcessor(std::shared_ptr<RandomGenerator> rng);
    ~IntegratedSensoryProcessor() override = default;
    
    void initialize() override;
    SensoryPercept processInput(const AgentBody& agent, const SimpleWorld& world) override;
    size_t getInputSize() const override;
    const char* getModalityName() const override { return "Integrated"; }
    bool isEnabled() const override;
    void enable(bool enabled) override;
    const std::map<RegionId, float>& getRegionWeights() const override;
    
    // Vision processing
    void enableVision(bool enabled) { visionEnabled_ = enabled; }
    
    // Touch/proximity processing
    void enableTouch(bool enabled) { touchEnabled_ = enabled; }
    
    // Internal state processing
    void enableInternal(bool enabled) { internalEnabled_ = enabled; }
    
    // Proprioception processing
    void enableProprioception(bool enabled) { proprioceptionEnabled_ = enabled; }
    
    // Audio processing
    void enableAudio(bool enabled) { audioEnabled_ = enabled; }
    
private:
    std::shared_ptr<RandomGenerator> rng_;
    
    // Modality enable flags
    bool visionEnabled_;
    bool touchEnabled_;
    bool internalEnabled_;
    bool proprioceptionEnabled_;
    bool audioEnabled_;
    
    // Region weight distributions
    std::map<RegionId, float> regionWeights_;
    
    // Processing state
    bool initialized_;
    bool enabled_;
    
    // Vision parameters
    static constexpr size_t VISION_WIDTH = 16;
    static constexpr size_t VISION_HEIGHT = 16;
    static constexpr size_t TOUCH_SIZE = 8;
    static constexpr size_t INTERNAL_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_SIZE = 6;
    static constexpr size_t AUDIO_SIZE = 0;  // Not implemented yet
};

// Concrete implementation of brain-motor interface
class IntegratedMotorController : public IMotorController {
public:
    IntegratedMotorController(std::shared_ptr<RandomGenerator> rng);
    ~IntegratedMotorController() override = default;
    
    void initialize(const Brain& brain) override;
    MotorCommand decodeCommand(const std::vector<float>& neuralActivity) override;
    size_t getOutputSize() const override;
    void applyNeuromodulation(MotorCommand& command, float neuromodulationLevel) override;
    bool isEnabled() const override;
    void enable(bool enabled) override;
    
    // Configure motor neuron groupings
    void configureMotorGroups(const std::vector<Neuron*>& motorNeurons);
    
    // Get motor activity statistics
    struct MotorStats {
        std::map<MotorCommand, float> activityLevels;
        MotorCommand dominantCommand;
        float explorationLevel;
    };
    
    MotorStats getMotorStats() const;
    
private:
    std::shared_ptr<RandomGenerator> rng_;
    
    // Motor neuron groupings
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    
    // Configuration flags
    bool enabled_;
    bool curiosityEnabled_;
    bool rewardModulationEnabled_;
    
    // Previous state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

// Concrete implementation of neural integration
class IntegratedNeuralIntegrator : public INeuralIntegrator {
public:
    IntegratedNeuralIntegrator(std::shared_ptr<RandomGenerator> rng);
    ~IntegratedNeuralIntegrator() override = default;
    
    void initialize(const Brain& brain) override;
    void update(double timestep) override;
    void getStatistics(std::map<std::string, float>& stats) const override;
    bool hasReachedDevelopmentalStage(DevelopmentalStage stage) const override;
    DevelopmentalStage getCurrentStage() const override;
    
    // Development tracking
    void advanceDevelopmentalStage();
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_; }
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_; }
    
    // Neuromodulator access
    Dopamine* getDopamine() const { return dopamine_; }
    Curiosity* getCuriosity() const { return curiosity_; }
    Novelty* getNovelty() const { return novelty_; }
    
private:
    std::shared_ptr<RandomGenerator> rng_;
    
    // Developmental state
    double developmentalAge_;
    DevelopmentalStage currentStage_;
    float plasticityModifier_;
    
    // Integrated systems
    NeuralWorkingMemory* workingMemory_;
    NeuralEpisodicMemory* episodicMemory_;
    
    // Neuromodulators
    Dopamine* dopamine_;
    Curiosity* curiosity_;
    Novelty* novelty_;
    
    // Learning state
    float predictionError_;
    float expectedReward_;
    
    // Processing state
    Timestamp lastUpdateTime_;
    SimulationStep stepCount_;
};

} // namespace nlm