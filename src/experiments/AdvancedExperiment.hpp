#pragma once

#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

/**
 * Advanced neuromodulation implementation
 * Multiple neuromodulators with complex interactions
 */
class AdvancedNeuromodulationImpl {
public:
    struct Neuromodulator {
        float level;                          // Current level (0.0 to 1.0)
        float baseline;                       // Baseline level
        float peak;                           // Peak level
        float decayRate;                      // Decay rate
        float releaseRate;                    // Release rate
        float timeConstant;                   // Time constant for dynamics
        bool enabled;                         // Whether this neuromodulator is active
        float homeostaticTarget;              // Homeostatic target level
        float adaptationRate;                 // Rate of adaptation to target
        float burstThreshold;                 // Threshold for burst firing
        float burstDuration;                  // Duration of burst firing
        std::string modulationFunction;       // Function for level modulation
        std::map<std::string, float> interactionWeights; // Weights for interactions with other modulators
        
        Neuromodulator() : level(0.0f), baseline(0.0f), peak(1.0f), 
                          decayRate(0.1f), releaseRate(1.0f), timeConstant(0.1f),
                          enabled(true), homeostaticTarget(0.5f), adaptationRate(0.01f),
                          burstThreshold(0.8f), burstDuration(0.1f) {}
    };
    
    enum class NeuromodulatorType {
        Dopamine,            // Reward prediction error
        Acetylcholine,       // Attention, arousal
        Norepinephrine,      // Vigilance, arousal
        Serotonin,          // Mood, anxiety, punishment
        GABA,               // Inhibition
        Glutamate,          // Excitation
        Endorphin,          // Pain modulation
        Histamine           // Wake-sleep regulation
    };
    
    AdvancedNeuromodulationImpl();
    ~AdvancedNeuromodulationImpl() = default;
    
    // Add a neuromodulator
    void addNeuromodulator(NeuromodulatorType type, const Neuromodulator& config);
    
    // Remove a neuromodulator
    void removeNeuromodulator(NeuromodulatorType type);
    
    // Get neuromodulator level
    float getLevel(NeuromodulatorType type) const;
    
    // Set neuromodulator level
    void setLevel(NeuromodulatorType type, float level);
    
    // Update all neuromodulators
    void update(float dt);
    
    // Process stimulus to influence neuromodulators
    void processStimulus(float stimulus, NeuromodulatorType primaryType,
                        const std::vector<NeuromodulatorType>& interactingTypes = {});
    
    // Get combined effect of multiple neuromodulators on plasticity
    float getPlasticityModulation(NeuromodulatorType type = NeuromodulatorType::Dopamine) const;
    
    // Get combined effect on excitability
    float getExcitabilityModulation(NeuromodulatorType type = NeuromodulatorType::Acetylcholine) const;
    
    // Get neuromodulator state
    const Neuromodulator& getState(NeuromodulatorType type) const;
    
    // Set homeostatic target
    void setHomeostaticTarget(NeuromodulatorType type, float target);
    
    // Get homeostatic deviation
    float getHomeostaticDeviation(NeuromodulatorType type) const;
    
    // Burst detection and modulation
    bool detectBurst(NeuromodulatorType type) const;
    void modulateBurst(NeuromodulatorType type, float burstStrength);
    
    // Interaction matrix for neuromodulator cross-talk
    float getInteractionWeight(NeuromodulatorType source, NeuromodulatorType target) const;
    void setInteractionWeight(NeuromodulatorType source, NeuromodulatorType target, float weight);
    
    // Static factory methods for common neuromodulation configurations
    static Neuromodulator getDopamineConfig();
    static Neuromodulator getAChConfig();
    static Neuromodulator getNEConfig();
    static Neuromodulator get5HTConfig();
    static Neuromodulator getGABAConfig();
    
    // Get all neuromodulator types
    static std::vector<NeuromodulatorType> getAllTypes();
    
    // Get neuromodulator name
    static std::string getTypeName(NeuromodulatorType type);
    
private:
    std::map<NeuromodulatorType, Neuromodulator> neuromodulators_;
    
    // Interaction matrix for neuromodulator cross-talk
    std::map<NeuromodulatorType, std::map<NeuromodulatorType, float>> interactionMatrix_;
    
    // Temporal dynamics
    std::map<NeuromodulatorType, float> lastUpdateTime_;
    std::map<NeuromodulatorType, float> adaptationAccumulator_;
    
    // Burst detection
    std::map<NeuromodulatorType, std::vector<float>> levelHistory_;
    
    // Initialize interaction matrix
    void initializeInteractionMatrix();
    
    // Update neuromodulator level
    void updateNeuromodulatorLevel(NeuromodulatorType type, float dt);
    
    // Apply homeostatic regulation
    void applyHomeostaticRegulation(NeuromodulatorType type, float dt);
    
    // Update interaction effects
    void updateInteractionEffects(NeuromodulatorType type, float dt);
    
    // Time function implementations
    float exponentialTimeFunction(float t, float tau) const;
    float gaussianTimeFunction(float t, float sigma) const;
    float linearTimeFunction(float t) const;
    float sigmoidTimeFunction(float t, float k, float x0) const;
};

/**
 * Advanced experiment control implementation
 * Complex experimental manipulation and control
 */
class AdvancedExperimentImpl {
public:
    struct RewardFunction {
        std::string name;                    // Name of the reward function
        std::function<float(const std::vector<float>&)> compute; // Reward computation
        std::function<float(const std::vector<float>&)> gradient; // Reward gradient computation
        float weight;                        // Weight in combined reward
        bool enabled;                        // Whether this reward function is active
        bool useStateFeatures;               // Whether to use state features
        std::vector<size_t> featureIndices;  // Indices of features to use
        std::string temporalAbstraction;     // Temporal abstraction method
        float temporalDiscount;              // Temporal discount factor
        std::string explorationBonus;        // Exploration bonus method
        bool enableIntrinsicMotivation;      // Whether to include intrinsic motivation
        std::string intrinsicFunction;       // Intrinsic motivation function
        float intrinsicWeight;               // Weight for intrinsic motivation
        bool enableExtrinsicMotivation;      // Whether to include extrinsic motivation
        std::map<std::string, float> extrinsicWeights; // Weights for extrinsic factors
        bool enableMetadata;                 // Whether to include metadata
        std::vector<std::string> metadataKeys; // Metadata keys to include
        
        RewardFunction(const std::string& n = "default",
                      std::function<float(const std::vector<float>&)> comp = nullptr,
                      std::function<float(const std::vector<float>&)> grad = nullptr,
                      float w = 1.0f, bool en = true)
            : name(n), compute(comp), gradient(grad), weight(w), enabled(en),
              useStateFeatures(false), temporalDiscount(0.9f),
              enableIntrinsicMotivation(false), intrinsicWeight(0.1f),
              enableExtrinsicMotivation(true), temporalVariation(0.01f),
              enableMetadata(false) {}
    };
    
    struct ExperimentalCondition {
        std::string name;                    // Name of the experimental condition
        std::map<std::string, float> parameters; // Experimental parameters
        std::vector<RewardFunction> rewardFunctions; // Reward functions
        bool useDynamicRewards;              // Whether to use time-varying rewards
        bool enableAdvancedLogging;          // Whether to enable detailed logging
        std::map<std::string, bool> manipulations; // Experimental manipulations
        bool useTemporalAbstraction;         // Whether to use temporal abstraction
        bool enableCrossModalIntegration;    // Whether to enable cross-modal memory integration
        bool enableMemoryInterferenceManagement; // Whether to enable memory interference management
        std::string interferenceManagementStrategy; // Strategy for managing interference
    };
    
    AdvancedExperimentImpl();
    ~AdvancedExperimentImpl() = default;
    
    // Set the experimental condition
    void setCondition(const ExperimentalCondition& condition);
    
    // Get current experimental condition
    const ExperimentalCondition& getCondition() const { return currentCondition_; }
    
    // Compute reward based on state
    float computeReward(const std::vector<float>& state);
    
    // Compute gradient of reward w.r.t. state
    std::vector<float> computeRewardGradient(const std::vector<float>& state);
    
    // Enable/disable specific experimental manipulations
    void setManipulation(const std::string& name, bool enabled);
    
    // Get manipulation state
    bool getManipulation(const std::string& name) const;
    
    // Enable advanced logging
    void enableAdvancedLogging(bool enable);
    
    // Get detailed log of neural activity
    std::string getNeuralActivityLog() const;
    
    // Get detailed log of memory traces
    std::string getMemoryTracesLog() const;
    
    // Get detailed log of prediction errors
    std::string getPredictionErrorsLog() const;
    
    // Reset experiment state
    void reset();
    
    // Add to neural activity log
    void addToNeuralActivityLog(const NeuralActivityLog& log);
    
    // Add to memory trace log
    void addToMemoryTraceLog(const MemoryTraceLog& log);
    
    // Set memory interference management strategy
    void setInterferenceManagementStrategy(const std::string& strategy);
    
    // Get interference management statistics
    std::map<std::string, float> getInterferenceManagementStats() const;
    
    // Static factory methods for common experimental conditions
    static ExperimentalCondition createExplorationCondition();
    static ExperimentalCondition createExploitationCondition();
    static ExperimentalCondition createBalancedCondition();
    static ExperimentalCondition createChallengeCondition();
    static ExperimentalCondition createRestCondition();
    static ExperimentalCondition createSleepCondition();
    
private:
    ExperimentalCondition currentCondition_;
    
    // Advanced logging structures
    struct NeuralActivityLog {
        SimulationStep step;
        float totalFiringRate;
        float maxFiringRate;
        std::vector<std::pair<NeuronId, float>> neuronActivations;
        float predictionError;
        float neuromodulationLevel;
        std::map<std::string, float> additionalMetrics;
    };
    
    struct MemoryTraceLog {
        SimulationStep step;
        size_t workingMemorySize;
        size_t episodicMemorySize;
        std::vector<std::pair<NeuronId, float>> activeMemoryNeurons;
        float consolidationStrength;
        std::map<std::string, float> memoryMetrics;
    };
    
    std::vector<NeuralActivityLog> neuralActivityLog_;
    std::vector<MemoryTraceLog> memoryTraceLog_;
    bool loggingEnabled_;
    
    // Memory interference management
    struct MemoryInterference {
        std::vector<NeuronId> interferingNeurons;
        float interferenceStrength;
        SimulationStep lastInterference;
        std::string interferenceType;
    };
    
    std::vector<MemoryInterference> memoryInterferences_;
    std::string interferenceManagementStrategy_;
    
    // Helper functions
    float computeCombinedReward(const std::vector<float>& state) const;
    std::vector<float> computeCombinedRewardGradient(const std::vector<float>& state) const;
    
    // Reward function utilities
    float computeTemporalAbstractionReward(const std::vector<float>& state) const;
    float computeExplorationBonusReward(const std::vector<float>& state) const;
    float computeIntrinsicMotivationReward(const std::vector<float>& state) const;
    float computeExtrinsicMotivationReward(const std::vector<float>& state) const;
    
    // Gradient utilities
    std::vector<float> computeTemporalAbstractionRewardGradient(const std::vector<float>& state) const;
    std::vector<float> computeExplorationBonusRewardGradient(const std::vector<float>& state) const;
    std::vector<float> computeIntrinsicMotivationRewardGradient(const std::vector<float>& state) const;
    std::vector<float> computeExtrinsicMotivationRewardGradient(const std::vector<float>& state) const;
    
    // Memory interference management
    void detectMemoryInterference(const std::vector<float>& state);
    void applyInterferenceManagement(SimulationStep step);
    void consolidateMemoriesWithInterferenceManagement();
};

/**
 * Parallel processing manager implementation
 * Multi-core support for large-scale simulations
 */
class ParallelManagerImpl {
public:
    struct ParallelConfig {
        size_t numCores;                     // Number of CPU cores to use
        size_t neuronsPerCore;               // Approximate neurons per core
        bool enableSharedMemory;             // Whether to use shared memory
        bool enableTaskQueue;                // Whether to use task-based parallelism
        bool enableSpikingQueue;             // Whether to use event-driven spiking
        size_t spikeQueueSize;               // Size of spike event queue
        float loadBalanceThreshold;          // Threshold for load balancing
        bool enableDynamicScheduling;        // Whether to enable dynamic scheduling
        float schedulingInterval;            // Scheduling interval
        bool enableMemoryPool;               // Whether to enable memory pool
        size_t memoryPoolSize;                // Size of memory pool
        bool enableSIMD;                     // Whether to enable SIMD optimizations
    };
    
    ParallelManagerImpl(const ParallelConfig& config = getDefaultConfig());
    ~ParallelManagerImpl() = default;
    
    // Add a computational task
    void addTask(const std::function<void()>& task, size_t priority = 0);
    
    // Process all pending tasks
    void processTasks();
    
    // Get the next spike event
    std::optional<SpikeEvent> getNextSpikeEvent();
    
    // Submit a spike event for processing
    void submitSpikeEvent(const SpikeEvent& event);
    
    // Get current load balancing information
    std::map<size_t, float> getLoadBalance() const;
    
    // Set the number of active cores
    void setNumCores(size_t numCores);
    
    // Get performance statistics
    struct PerformanceStats {
        float cpuUtilization;                // CPU utilization percentage
        float memoryUsage;                   // Memory usage in MB
        float simulationTime;                // Time for one simulation step
        float spikeProcessingTime;           // Average spike processing time
        float plasticityTime;                // Time for plasticity updates
        float memoryAccessTime;              // Time for memory operations
        float neuronUpdateTime;              // Time for neuron updates
        size_t tasksCompleted;               // Number of completed tasks
        size_t totalSpikesProcessed;         // Total spikes processed
        size_t cacheHitRate;                 // Cache hit rate
        size_t memoryPoolUsage;              // Memory pool usage
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Set parallel configuration
    void setConfig(const ParallelConfig& config);
    
    // Get current configuration
    const ParallelConfig& getConfig() const { return config_; }
    
    // Enable/disable memory pool
    void enableMemoryPool(bool enable);
    
    // Allocate memory from pool
    void* allocateMemory(size_t size);
    
    // Free memory from pool
    void freeMemory(void* ptr, size_t size);
    
    // Static factory methods for common parallel configurations
    static ParallelConfig getDefaultConfig() {
        ParallelConfig config;
        config.numCores = std::thread::hardware_concurrency();
        config.neuronsPerCore = 10000;
        config.enableSharedMemory = true;
        config.enableTaskQueue = true;
        config.enableSpikingQueue = true;
        config.spikeQueueSize = 10000;
        config.loadBalanceThreshold = 0.7f;
        config.enableDynamicScheduling = true;
        config.schedulingInterval = 0.1f;
        config.enableMemoryPool = true;
        config.memoryPoolSize = 100 * 1024 * 1024; // 100 MB
        config.enableSIMD = true;
        return config;
    }
    
    static ParallelConfig getSmallScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = 2;
        config.neuronsPerCore = 1000;
        config.memoryPoolSize = 10 * 1024 * 1024; // 10 MB
        return config;
    }
    
    static ParallelConfig getLargeScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = std::max<size_t>(4, std::thread::hardware_concurrency() / 2);
        config.neuronsPerCore = 50000;
        config.memoryPoolSize = 500 * 1024 * 1024; // 500 MB
        return config;
    }
    
private:
    ParallelConfig config_;
    
    // Task queue and spike event queue
    std::priority_queue<std::pair<size_t, std::function<void()>>> taskQueue_;
    std::priority_queue<SpikeEvent, std::vector<SpikeEvent>, SpikeEventComparator> spikeQueue_;
    
    // Thread pool
    std::vector<std::thread> workerThreads_;
    std::mutex taskMutex_;
    std::condition_variable taskCV_;
    std::atomic<bool> running_;
    std::atomic<size_t> currentLoad_;
    
    // Memory pool
    struct MemoryBlock {
        void* ptr;
        size_t size;
        bool used;
    };
    std::vector<MemoryBlock> memoryPool_;
    std::mutex memoryPoolMutex_;
    
    // Performance monitoring
    mutable std::mutex statsMutex_;
    PerformanceStats stats_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    std::chrono::steady_clock::time_point lastLoadBalanceTime_;
    
    // Load balancing
    std::map<size_t, float> coreLoads_;
    std::map<size_t, size_t> taskCounts_;
    
    // Worker function
    void workerThreadFunction(size_t workerId);
    
    // Task scheduling
    void scheduleTasks();
    void balanceLoad();
    
    // Memory pool management
    void initializeMemoryPool();
    void* allocateFromPool(size_t size);
    void freeToPool(void* ptr, size_t size);
    
    // Performance monitoring
    void updatePerformanceStats();
    float calculateCpuUsage() const;
    float calculateMemoryUsage() const;
    
    // SIMD optimizations
    void processSpikesSIMD(std::vector<SpikeEvent>& events);
    void updateNeuronsSIMD(std::vector<Neuron*>& neurons, float dt);
};

/**
 * Configuration templates implementation
 * Preset configurations for different experimental scenarios
 */
class ConfigurationTemplatesImpl {
public:
    struct Template {
        std::string name;                    // Template name
        std::string description;             // Template description
        std::map<std::string, std::any> parameters; // Template parameters
        bool advancedFeatures;               // Whether template enables advanced features
        std::vector<std::string> requiredPlugins; // Required plugins
        std::map<std::string, std::any> defaultParameters; // Default parameters
        std::map<std::string, std::function<void(std::map<std::string, std::any>&)>> applyFunction; // Apply function
        bool validated;                      // Whether template is validated
    };
    
    ConfigurationTemplatesImpl();
    ~ConfigurationTemplatesImpl() = default;
    
    // Register a template
    void registerTemplate(const Template& template);
    
    // Get a template by name
    const Template* getTemplate(const std::string& name) const;
    
    // Get all available templates
    std::vector<std::string> getAvailableTemplates() const;
    
    // Create a configuration from a template
    std::shared_ptr<Config> createConfig(const std::string& templateName,
                                         const std::map<std::string, std::any>& overrides = {}) const;
    
    // Validate a configuration against a template
    bool validateConfig(const std::shared_ptr<Config>& config,
                       const Template& template) const;
    
    // Get default templates
    static Template getDefaultTemplate() {
        Template t;
        t.name = "Default";
        t.description = "Standard NLM configuration with basic features";
        t.advancedFeatures = false;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 1000},
            {"region_count", 1},
            {"simulation_timestep", 0.001},
            {"random_seed", 42}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    static Template getAdvancedTemplate() {
        Template t;
        t.name = "Advanced";
        t.description = "Advanced configuration with enhanced features";
        t.advancedFeatures = true;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 5000},
            {"region_count", 4},
            {"simulation_timestep", 0.0001},
            {"random_seed", 42},
            {"enable_advanced_plasticity", true},
            {"enable_multiple_modulators", true},
            {"enable_memory_replay", true},
            {"enable_structural_plasticity", true},
            {"enable_custom_neuron_models", true},
            {"enable_dynamic_stdp", true}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    static Template getResearchTemplate() {
        Template t;
        t.name = "Research";
        t.description = "Research configuration with maximum flexibility";
        t.advancedFeatures = true;
        t.validated = true;
        t.parameters = {
            {"neuron_count", 20000},
            {"region_count", 8},
            {"simulation_timestep", 0.00001},
            {"random_seed", 42},
            {"enable_advanced_plasticity", true},
            {"enable_multiple_modulators", true},
            {"enable_memory_replay", true},
            {"enable_structural_plasticity", true},
            {"enable_parallel_processing", true},
            {"enable_custom_neuron_models", true},
            {"enable_dynamic_stdp", true},
            {"enable_cross_modal_memory", true},
            {"enable_advanced_reward_functions", true},
            {"enable_performance_monitoring", true}
        };
        t.defaultParameters = t.parameters;
        return t;
    }
    
    // Template validation
    bool validateTemplate(const Template& template) const;
    
    // Parameter validation
    bool validateParameters(const std::map<std::string, std::any>& parameters,
                           const Template& template) const;
    
    // Apply template parameters
    void applyTemplateParameters(std::shared_ptr<Config>& config,
                                 const Template& template) const;
    
private:
    std::map<std::string, Template> templates_;
    
    // Parameter validation
    bool validateParameter(const std::string& name,
                          const std::any& value,
                          const Template& template) const;
    
    // Type checking utilities
    template<typename T>
    bool checkType(const std::any& value) const {
        return value.type() == typeid(T);
    }
};

/**
 * Performance profiler implementation
 * Monitor and optimize simulation performance
 */
class PerformanceProfilerImpl {
public:
    struct ProfileData {
        float cpuUsage;                      // CPU usage percentage
        float memoryUsage;                   // Memory usage in MB
        float simulationTime;                // Time for one simulation step
        float spikeProcessingTime;           // Time for spike processing
        float plasticityTime;                // Time for plasticity updates
        float memoryAccessTime;              // Time for memory operations
        float neuronUpdateTime;              // Time for neuron updates
        size_t spikesProcessed;              // Number of spikes processed
        size_t neuronsUpdated;               // Number of neurons updated
        float cacheHitRate;                  // Cache hit rate
        float memoryPoolUsage;               // Memory pool usage
        std::map<std::string, float> customMetrics; // Custom metrics
    };
    
    PerformanceProfilerImpl();
    ~PerformanceProfilerImpl() = default;
    
    // Start profiling
    void start();
    
    // Stop profiling
    void stop();
    
    // Record a performance sample
    void recordSample();
    
    // Get current performance statistics
    ProfileData getCurrentStats() const;
    
    // Get average performance statistics
    ProfileData getAverageStats(size_t samples = 100) const;
    
    // Get maximum performance statistics
    ProfileData getMaxStats() const;
    
    // Get minimum performance statistics
    ProfileData getMinStats() const;
    
    // Enable/disable performance profiling
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Export performance data
    std::string exportData() const;
    
    // Import performance data
    bool importData(const std::string& data);
    
    // Clear performance data
    void clear();
    
    // Get performance recommendations
    std::vector<std::string> getRecommendations() const;
    
    // Set custom metric
    void setCustomMetric(const std::string& name,
                        std::function<float()> getter);
    
    // Get all custom metrics
    std::map<std::string, float> getCustomMetrics() const;
    
private:
    bool enabled_;
    
    mutable std::mutex dataMutex_;
    
    // Performance data
    std::vector<ProfileData> profileData_;
    
    // Timing utilities
    std::chrono::steady_clock::time_point startTime_;
    size_t sampleCount_;
    
    // System resources monitoring
    size_t totalSpikesProcessed_;
    size_t totalNeuronsUpdated_;
    float totalCpuUsage_;
    float totalMemoryUsage_;
    float totalSimulationTime_;
    float totalSpikeProcessingTime_;
    float totalPlasticityTime_;
    float totalMemoryAccessTime_;
    float totalNeuronUpdateTime_;
    
    // Custom metrics
    std::map<std::string, std::function<float()>> customMetrics_;
    
    // Helper functions
    float getCpuUsage() const;
    float getMemoryUsage() const;
    ProfileData computeCurrentStats() const;
    
    // Data processing
    void processSample(const ProfileData& sample);
    ProfileData computeWeightedAverage(size_t samples) const;
};

} // namespace nlm
