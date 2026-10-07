#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include "../core/Config/Config.hpp"
#include "SimulationOptions.hpp"
#include "TrainingOptions.hpp"
#include "EvaluationOptions.hpp"
#include "BatchOptions.hpp"
#include "CheckpointOptions.hpp"
#include "MonitoringConfig.hpp"
#include "MonitoringCallback.hpp"
#include "CallbackType.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace nlm {

// Forward declarations
class VisualizationInterface;
class DebugSystem;
class EnhancedLogger;
class BrainIntegration;

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

// Error handling for Brain operations
enum class BrainErrorCode {
    Success = 0,
    InvalidConfig,
    InitializationFailed,
    SimulationError,
    SaveLoadError,
    MemoryError,
    Timeout,
    PluginError
};

struct BrainError {
    BrainErrorCode code;
    std::string message;
    std::vector<std::string> details;
    
    BrainError() : code(BrainErrorCode::Success), message(), details() {}
    BrainError(BrainErrorCode c, const std::string& msg, const std::vector<std::string>& det = {})
        : code(c), message(msg), details(det) {}
};

struct ValidationResult {
    bool valid;
    std::vector<BrainError> errors;
    std::vector<BrainError> warnings;
    
    ValidationResult() : valid(true), errors(), warnings() {}
};

// High-level configuration structure for Brain
struct BrainConfig {
    // Basic parameters
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Random seed and simulation parameters
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    
    // Plasticity parameters
    float stdpLtpWeight = 0.01f;
    float stdpLtdWeight = 0.012f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Memory system parameters
    size_t workingMemoryCapacity = 100;
    size_t maxEpisodicEpisodes = 1000;
    
    // Neuromodulation parameters
    float dopamineLevel = 0.5f;
    float curiosityStrength = 0.5f;
    float noveltyDecay = 0.99f;
    float predictionErrorWeight = 1.0f;
    
    // Development parameters
    bool enableDevelopment = true;
    double developmentalAge = 0.0;
    
    // Learning parameters
    bool enableLearning = true;
    float learningRate = 0.01f;
    bool enableCuriosity = true;
    bool enableRewardModulation = true;
    float rewardInfluence = 1.0f;
    
    // Runtime behavior
    size_t replayInterval = 100;
    size_t consolidationInterval = 1000;
    bool enableCheckpointing = false;
    std::string checkpointDir = "./checkpoints";
    
    // Simulation control
    size_t maxSteps = 10000;
    bool verboseLogging = false;
    
    // Predefined configuration templates
    static BrainConfig createDefault() {
        BrainConfig config;
        config.neuronCount = 1000;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableDevelopment = true;
        config.enableLearning = true;
        config.enableCuriosity = true;
        return config;
    }
    
    static BrainConfig createLearning() {
        BrainConfig config = createDefault();
        config.neuronCount = 500;
        config.stdpLtpWeight = 0.005f;
        config.stdpLtdWeight = 0.006f;
        config.curiosityStrength = 0.8f;
        config.rewardInfluence = 1.2f;
        return config;
    }
    
    static BrainConfig createDevelopment() {
        BrainConfig config = createDefault();
        config.neuronCount = 2000;
        config.enableDevelopment = true;
        config.developmentalAge = 0.0;
        config.synaptogenesisRate = 0.0002f;
        config.pruningRate = 0.00002f;
        return config;
    }
    
    static BrainConfig createOptimized(size_t neurons = 1000) {
        BrainConfig config = createDefault();
        config.neuronCount = neurons;
        config.connectionProbability = 0.05f;
        config.maxSteps = neurons * 10;
        return config;
    }
};

// Simulation result structure
struct SimulationResult {
    bool success;
    std::vector<std::unique_ptr<Action>> actions;
    float totalReward;
    size_t stepsCompleted;
    size_t totalSpikes;
    float averageFiringRate;
    std::vector<BrainError> errors;
    double wallClockTime;
    
    SimulationResult() : success(false), totalReward(0.0f), stepsCompleted(0), 
                         totalSpikes(0), averageFiringRate(0.0f), wallClockTime(0.0) {}
};

// Training result structure
struct TrainingResult {
    bool success;
    float finalPerformance;
    float bestPerformance;
    size_t epochsCompleted;
    size_t totalSamples;
    float learningCurve;
    std::vector<BrainError> errors;
    double wallClockTime;
    
    TrainingResult() : success(false), finalPerformance(0.0f), bestPerformance(0.0f), 
                       epochsCompleted(0), totalSamples(0), learningCurve(0.0f), wallClockTime(0.0) {}
};

// Evaluation result structure
struct EvaluationResult {
    bool success;
    float accuracy;
    float precision;
    float recall;
    float f1Score;
    size_t correctPredictions;
    size_t totalPredictions;
    std::vector<BrainError> errors;
    double wallClockTime;
    
    EvaluationResult() : success(false), accuracy(0.0f), precision(0.0f), 
                         recall(0.0f), f1Score(0.0f), correctPredictions(0), 
                         totalPredictions(0), wallClockTime(0.0) {}
};

// Batch processing result
struct BatchResult {
    bool success;
    std::vector<SimulationResult> results;
    float averageReward;
    size_t totalSteps;
    double wallClockTime;
    
    BatchResult() : success(false), averageReward(0.0f), totalSteps(0), wallClockTime(0.0) {}
};

// Callback types for extensibility
using SimulationCallback = std::function<void(const SimulationStep&)>;
using ErrorCallback = std::function<void(const BrainError&)>;
using ProgressCallback = std::function<void(float)>;
using MetricCallback = std::function<void(const std::string&, double)>;

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// Enhanced with user-friendly API and extensibility features

class Brain {
public:
    // Original constructor (backward compatibility)
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // New simplified constructors for easier use
    Brain();  // Default brain with default config
    Brain(const std::unordered_map<std::string, ConfigValue>& configMap);  // Map-based config
    Brain(const std::string& configFile);  // File-based config
    Brain(const BrainConfig& config);  // High-level config object
    
    // Factory methods for common brain types
    static std::unique_ptr<Brain> createBasicBrain();
    static std::unique_ptr<Brain> createLearningBrain();
    static std::unique_ptr<Brain> createDevelopmentBrain();
    static std::unique_ptr<Brain> createOptimizedBrain(size_t neuronCount = 1000);
    static std::unique_ptr<Brain> createCustomBrain(const BrainConfig& config);
    
    // Convenience initialization methods
    bool initialize(const BrainConfig& config = BrainConfig::createDefault());
    bool initializeWithValidation(const BrainConfig& config, ValidationResult& validationResult);
    
    // High-level simulation control methods
    SimulationResult runSimulation(const std::vector<SensoryInput>& inputs, 
                                 const SimulationOptions& options = {});
    SimulationResult runTraining(const std::vector<TrainingData>& data,
                                const TrainingOptions& options = {});
    SimulationResult runEvaluation(const std::vector<SensoryInput>& inputs,
                                  const EvaluationOptions& options = {});
    
    // Simplified lifecycle management
    void resetBrain();
    bool saveBrain(const std::string& filepath, const CheckpointOptions& options = {});
    bool loadBrain(const std::string& filepath);
    
    // Configuration management
    static BrainConfig createDefaultConfig();
    static BrainConfig createLearningConfig(float learningRate = 0.01f);
    static BrainConfig createDevelopmentConfig();
    static BrainConfig createOptimizedConfig(size_t neuronCount = 1000);
    
    ValidationResult validateConfig(const BrainConfig& config) const;
    bool applyConfig(const BrainConfig& config);
    
    // Runtime configuration modification
    void setLearningRate(float rate);
    void setNeuronCount(size_t count);
    void enableLearning(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    void enableRewardModulation(bool enable);
    void setVerboseLogging(bool verbose);
    
    // Advanced features
    BatchResult runBatch(const std::vector<std::vector<SensoryInput>>& inputSequences,
                       const BatchOptions& options = {});
    
    // Plugin and extension system
    void registerPlugin(std::unique_ptr<BrainPlugin> plugin);
    void unregisterPlugin(const std::string& name);
    std::vector<std::string> getPluginNames() const;
    
    // Callback system
    void registerCallback(CallbackType type, const std::function<void()>& callback);
    void unregisterCallback(CallbackType type);
    void clearCallbacks();
    
    // Real-time monitoring and control
    void startMonitoring(std::unique_ptr<MonitoringCallback> monitor,
                        const MonitoringConfig& config = {});
    void stopMonitoring();
    bool isMonitoring() const;
    
    void pauseSimulation();
    void resumeSimulation();
    void stepSimulation();
    bool isSimulationRunning() const;
    
    // Error handling improvements
    BrainError initializeWithError(const BrainConfig& config = BrainConfig::createDefault());
    BrainError resetWithError();
    BrainError saveWithError(const std::string& filepath, const CheckpointOptions& options = {});
    
    // Parameter validation
    bool validateParameters(const BrainConfig& config) const;
    BrainError validateRuntimeState() const;
    
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
    std::unique_ptr<Action> produceAction();
    
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
    
    // NeuromodulationManager - integrates all neuromodulation systems
    NeuromodulationManager* getNeuromodulationManager();

    // ========== REWARD SYSTEM ==========
    
    // Reward signal for reinforcement learning
    Reward* getReward();
    
    // Get current total reward
    float getTotalReward() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm