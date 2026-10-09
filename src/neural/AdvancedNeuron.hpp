#pragma once

#include "../brain/Neuron.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace nlm {

/**
 * Izhikevich Neuron Model
 * Approximate model of neuronal electrophysiology
 */
class IzhikevichNeuron : public Neuron {
public:
    struct Parameters {
        float a;           // Recovery rate
        float b;           // Sensitivity of recovery to subthreshold fluctuations
        float c;           // Afterhyperpolarization value
        float d;           // Recovery variable increment
        float vPeak;       // Spike threshold
        float vRest;       // Resting potential
        float cm;          // Membrane capacitance
        float gL;          // Leak conductance
        float EL;          // Leak reversal potential
    };
    
    IzhikevichNeuron(NeuronId id, const Parameters& params = getDefaultParameters());
    ~IzhikevichNeuron() override = default;
    
    // Izhikevich dynamics update
    bool stepIzhikevich(float dt, std::function<void(const IzhikevichNeuron&, float)> spikeCallback = nullptr);
    
    // Parameter access
    const Parameters& getParameters() const { return params_; }
    void setParameters(const Parameters& params) { params_ = params; }
    
    // Custom neuron variables
    float getU() const { return u_; }  // Recovery variable
    void setU(float u) { u_ = u; }
    float getVPeak() const { return params_.vPeak; }
    
    // Static default parameters for different neuron types
    static Parameters getRegularSpikingParameters();
    static Parameters getIntrinsicBurstingParameters();
    static Parameters getChediumSpikingParameters();
    static Parameters getFastSpikingParameters();
    static Parameters getSlowSpikingParameters();
    static Parameters getDefaultParameters() {
        Parameters p;
        p.a = -0.02f;
        p.b = 0.2f;
        p.c = -65.0f;
        p.d = 8.0f;
        p.vPeak = 40.0f;
        p.vRest = -70.0f;
        p.cm = 1.0f;
        p.gL = 0.05f;
        p.EL = -70.0f;
        return p;
    }
    
private:
    Parameters params_;
    float u_;  // Recovery variable
};

/**
 * Hodgkin-Huxley Neuron Model
 * Detailed biophysical model of action potentials
 */
class HodgkinHuxleyNeuron : public Neuron {
public:
    struct Parameters {
        float gNa;         // Sodium conductance
        float gK;          // Potassium conductance
        float gL;          // Leak conductance
        float Cm;          // Membrane capacitance
        float ENa;         // Sodium reversal potential
        float EK;          // Potassium reversal potential
        float EL;          // Leak reversal potential
        float VhalfNa;     // Half-activation voltage for sodium
        float VhalfK;      // Half-activation voltage for potassium
        float kNa;         // Slope factor for sodium
        float kK;          // Slope factor for potassium
        float tauW;        // Time constant for slow variable
    };
    
    HodgkinHuxleyNeuron(NeuronId id, const Parameters& params = getDefaultParameters());
    ~HodgkinHuxleyNeuron() override = default;
    
    // Hodgkin-Huxley dynamics update
    bool stepHodgkinHuxley(float dt, std::function<void(const HodgkinHuxleyNeuron&, float)> spikeCallback = nullptr);
    
    // Gating variable access
    float getINa() const { return INa_; }
    float getIK() const { return IK_; }
    float getIL() const { return IL_; }
    void setINa(float val) { INa_ = val; }
    void setIK(float val) { IK_ = val; }
    void setIL(float val) { IL_ = val; }
    
    static Parameters getDefaultParameters() {
        Parameters p;
        p.gNa = 120.0f;
        p.gK = 36.0f;
        p.gL = 0.3f;
        p.Cm = 1.0f;
        p.ENa = 50.0f;
        p.EK = -77.0f;
        p.EL = -54.387f;
        p.VhalfNa = 0.1f;
        p.VhalfK = -12.0f;
        p.kNa = -3.1f;
        p.kK = 1.0f;
        p.tauW = 100.0f;
        return p;
    }
    
private:
    Parameters params_;
    float INa_, IK_, IL_;  // Ionic currents
};

/**
 * Custom Neural Topology
 * Defines custom network architectures with different connection patterns
 */
class CustomTopology {
public:
    enum class ConnectionPattern {
        Dense,              // All-to-all connections
        Sparse,             // Random sparse connections
        Clustered,          // Connections within clusters
        SmallWorld,         // Small-world topology
        Ring,               // Ring network
        Grid,               // 2D grid
        Hierarchical        // Hierarchical modular structure
    };
    
    struct TopologyConfig {
        size_t numRegions;
        size_t neuronsPerRegion;
        ConnectionPattern pattern;
        float connectionProbability;
        float clusterProbability;
        float rewireProbability;
        std::map<size_t, std::vector<size_t>> customConnections;
        bool directed;      // Whether connections are directed
    };
    
    CustomTopology(const TopologyConfig& config);
    ~CustomTopology() = default;
    
    // Create regions based on topology configuration
    void buildTopology(Brain* brain);
    
    // Add custom connections between neurons
    void addCustomConnections(Brain* brain, const std::map<NeuronId, std::vector<NeuronId>>& connections);
    
    // Get the topology configuration
    const TopologyConfig& getConfig() const { return config_; }
    
    // Set the topology configuration
    void setConfig(const TopologyConfig& config) { config_ = config; }
    
    // Utility functions for generating topologies
    static TopologyConfig generateSparseToplogy(size_t numRegions, float connectionProb);
    static TopologyConfig generateClusteredTopology(size_t numRegions, 
                                                   size_t clusterSize, 
                                                   float interClusterProb,
                                                   float intraClusterProb);
    static TopologyConfig generateSmallWorldTopology(size_t numRegions,
                                                   float connectionProb,
                                                   float rewireProb);
    
private:
    TopologyConfig config_;
    std::vector<RegionId> regionIds_;
};

/**
 * Custom Plasticity Rule
 * User-defined STDP and Hebbian learning rules
 */
class CustomPlasticityRule {
public:
    enum class RuleType {
        STDP,              // Spike-Timing Dependent Plasticity
        Hebbian,           // Hebbian learning
        AntiHebbian,       // Anti-Hebbian learning
        BCM,               // Bienenstock-Cooper-Munro
        Oja,               // Oja's learning rule
        HTD,               // Homeostatic Plasticity
        Neuromodulated,    // Neuromodulator-dependent plasticity
        TimeVarying        // Time-varying learning rates
    };
    
    struct PlasticityParams {
        float learningRate;
        float stdpTauPre;   // Pre-synaptic spike trace decay
        float stdpTauPost;  // Post-synaptic spike trace decay
        float hebbianAlpha; // Hebbian learning rate
        float homeostaticTarget;  // Target firing rate
        float homeostaticRate;    // Homeostatic learning rate
        float neuromodulatorWeight; // Weight for neuromodulator influence
        bool enableDepression;     // Enable LTD in STDP
        bool enableHebbian;        // Enable Hebbian component
        std::function<float(float)> timeVaryingFunction; // For time-varying rules
    };
    
    CustomPlasticityRule(RuleType type, const PlasticityParams& params);
    ~CustomPlasticityRule() = default;
    
    // Update synaptic weight based on pre and post spike times
    void update(Synapse* synapse, 
                const std::vector<float>& preSpikeTimes,
                const std::vector<float>& postSpikeTimes,
                float dt);
    
    // Get the plasticity type
    RuleType getType() const { return type_; }
    
    // Set the plasticity type
    void setType(RuleType type) { type_ = type; }
    
    // Get parameters
    const PlasticityParams& getParams() const { return params_; }
    
    // Set parameters
    void setParams(const PlasticityParams& params) { params_ = params; }
    
    // Static factory methods for common plasticity rules
    static CustomPlasticityRule createSTDP(float ltpRate = 0.01f, float ltdRate = 0.012f);
    static CustomPlasticityRule createHebbian(float learningRate = 0.001f);
    static CustomPlasticityRule createBCM(float targetRate = 5.0f);
    static CustomPlasticityRule createNeuromodulated(float neuromodulatorWeight = 0.5f);
    
private:
    RuleType type_;
    PlasticityParams params_;
    
    // Helper functions
    float computeSTDPWeightChange(float preTime, float postTime, bool preBeforePost) const;
    float computeHebbianWeightChange(float preSpike, float postSpike) const;
    float computeBCMWeightChange(float postActivity, float bValue) const;
};

/**
 * Advanced Neuromodulation
 * Multiple neuromodulators with complex interactions
 */
class AdvancedNeuromodulation {
public:
    struct Neuromodulator {
        float level;          // Current level (0.0 to 1.0)
        float baseline;       // Baseline level
        float peak;           // Peak level
        float decayRate;      // Decay rate
        float releaseRate;    // Release rate
        float timeConstant;   // Time constant for dynamics
        bool enabled;         // Whether this neuromodulator is active
        
        Neuromodulator() : level(0.0f), baseline(0.0f), peak(1.0f), 
                          decayRate(0.1f), releaseRate(1.0f), 
                          timeConstant(0.1f), enabled(true) {}
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
    
    AdvancedNeuromodulation();
    ~AdvancedNeuromodulation() = default;
    
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
    
    // Static factory methods for common neuromodulation configurations
    static Neuromodulator getDopamineConfig();
    static Neuromodulator getAChConfig();
    static Neuromodulator getNEConfig();
    static Neuromodulator get5HTConfig();
    static Neuromodulator getGABAConfig();
    
private:
    std::map<NeuromodulatorType, Neuromodulator> neuromodulators_;
    
    // Interaction matrix for neuromodulator cross-talk
    std::map<NeuromodulatorType, std::map<NeuromodulatorType, float>> interactionMatrix_;
    
    // Initialize interaction matrix
    void initializeInteractionMatrix();
};

/**
 * Advanced Experiment Control
 * Complex experimental manipulation and control
 */
class AdvancedExperiment {
public:
    struct RewardFunction {
        std::string name;
        std::function<float(const std::vector<float>&)> compute;
        std::function<float(const std::vector<float>&)> gradient; // For optimization
        float weight;                    // Weight in combined reward
        bool enabled;                    // Whether this reward function is active
        
        RewardFunction(const std::string& n = "default",
                      std::function<float(const std::vector<float>&)> comp = nullptr,
                      std::function<float(const std::vector<float>&)> grad = nullptr,
                      float w = 1.0f, bool en = true)
            : name(n), compute(comp), gradient(grad), weight(w), enabled(en) {}
    };
    
    struct ExperimentalCondition {
        std::string name;
        std::map<std::string, float> parameters;
        std::vector<RewardFunction> rewardFunctions;
        bool useDynamicRewards;           // Whether to use time-varying rewards
        bool enableAdvancedLogging;       // Whether to enable detailed logging
        std::map<std::string, bool> manipulations; // Experimental manipulations
    };
    
    AdvancedExperiment();
    ~AdvancedExperiment() = default;
    
    // Set the experimental condition
    void setCondition(const ExperimentalCondition& condition);
    
    // Get current experimental condition
    const ExperimentalCondition& getCondition() const { return currentCondition_; }
    
    // Compute reward based on state
    float computeReward(const std::vector<float>& state);
    
    // Compute gradient of reward w.r.t. state (for policy optimization)
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
    
    // Static factory methods for common experimental conditions
    static ExperimentalCondition createExplorationCondition();
    static ExperimentalCondition createExploitationCondition();
    static ExperimentalCondition createBalancedCondition();
    static ExperimentalCondition createChallengeCondition();
    
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
    };
    
    struct MemoryTraceLog {
        SimulationStep step;
        size_t workingMemorySize;
        size_t episodicMemorySize;
        std::vector<std::pair<NeuronId, float>> activeMemoryNeurons;
        float consolidationStrength;
    };
    
    std::vector<NeuralActivityLog> neuralActivityLog_;
    std::vector<MemoryTraceLog> memoryTraceLog_;
    bool loggingEnabled_;
    
    // Helper functions
    float computeCombinedReward(const std::vector<float>& state) const;
    std::vector<float> computeCombinedRewardGradient(const std::vector<float>& state) const;
};

/**
 * Parallel Processing Manager
 * Multi-core support for large-scale simulations
 */
class ParallelManager {
public:
    struct ParallelConfig {
        size_t numCores;                     // Number of CPU cores to use
        size_t neuronsPerCore;               // Approximate neurons per core
        bool enableSharedMemory;             // Whether to use shared memory
        bool enableTaskQueue;                // Whether to use task-based parallelism
        bool enableSpikingQueue;             // Whether to use event-driven spiking
        size_t spikeQueueSize;               // Size of spike event queue
        float loadBalanceThreshold;          // Threshold for load balancing
    };
    
    ParallelManager(const ParallelConfig& config = getDefaultConfig());
    ~ParallelManager() = default;
    
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
        float spikeProcessingTime;           // Average spike processing time
        size_t tasksCompleted;               // Number of completed tasks
        size_t totalSpikesProcessed;         // Total spikes processed
    };
    
    PerformanceStats getPerformanceStats() const;
    
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
        return config;
    }
    
    static ParallelConfig getSmallScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = 2;
        config.neuronsPerCore = 1000;
        return config;
    }
    
    static ParallelConfig getLargeScaleConfig() {
        ParallelConfig config = getDefaultConfig();
        config.numCores = std::max<size_t>(4, std::thread::hardware_concurrency() / 2);
        config.neuronsPerCore = 50000;
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
    
    // Performance monitoring
    mutable std::mutex statsMutex_;
    PerformanceStats stats_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    
    // Helper functions
    void workerThreadFunction();
    void balanceLoad();
};

/**
 * Configuration Templates
 * Preset configurations for different experimental scenarios
 */
class ConfigurationTemplates {
public:
    struct Template {
        std::string name;
        std::string description;
        std::map<std::string, std::any> parameters;
        bool advancedFeatures;  // Whether this template enables advanced features
        std::vector<std::string> requiredPlugins;  // Plugins required for this template
    };
    
    ConfigurationTemplates();
    ~ConfigurationTemplates() = default;
    
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
        t.parameters = {
            {"neuron_count", 1000},
            {"region_count", 1},
            {"simulation_timestep", 0.001},
            {"random_seed", 42}
        };
        return t;
    }
    
    static Template getAdvancedTemplate() {
        Template t;
        t.name = "Advanced";
        t.description = "Advanced configuration with enhanced features";
        t.advancedFeatures = true;
        t.parameters = {
            {"neuron_count", 5000},
            {"region_count", 4},
            {"simulation_timestep", 0.0001},
            {"random_seed", 42},
            {"enable_advanced_plasticity", true},
            {"enable_multiple_modulators", true},
            {"enable_memory_replay", true},
            {"enable_structural_plasticity", true}
        };
        return t;
    }
    
    static Template getResearchTemplate() {
        Template t;
        t.name = "Research";
        t.description = "Research configuration with maximum flexibility";
        t.advancedFeatures = true;
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
            {"enable_cross_modal_memory", true}
        };
        return t;
    }
    
private:
    std::map<std::string, Template> templates_;
};

/**
 * Performance Profiler
 * Monitor and optimize simulation performance
 */
class PerformanceProfiler {
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
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
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
    
    // Enable/disable performance profiling
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Export performance data
    std::string exportData() const;
    
    // Clear performance data
    void clear();
    
    // Get performance recommendations
    std::vector<std::string> getRecommendations() const;
    
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
    
    // Helper functions
    float getCpuUsage() const;
    float getMemoryUsage() const;
    ProfileData computeCurrentStats() const;
};

} // namespace nlm
