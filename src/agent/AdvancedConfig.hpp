#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>

namespace nlm {

// Advanced configuration options for power users
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Neural network configuration
    struct NeuralNetworkConfig {
        // Neuron parameters
        double neuronThreshold;        // Membrane potential threshold (mV)
        double neuronRestPotential;    // Resting potential (mV)
        double neuronResetPotential;   // Reset potential after spike (mV)
        double membraneTimeConstant;   // Membrane time constant (ms)
        double refractoryPeriod;       // Refractory period (ms)
        
        // Synaptic parameters
        double initialWeightMean;      // Mean initial synaptic weight
        double initialWeightStd;       // Standard deviation of initial weights
        double maxWeight;              // Maximum synaptic weight
        double minWeight;              // Minimum synaptic weight
        double weightDecayRate;        // Weight decay per timestep
        
        // Connectivity
        double connectionProbability;  // Probability of connection between neurons
        double connectionDistance;     // Maximum distance for connections
        size_t maxConnectionsPerNeuron; // Maximum incoming/outgoing connections per neuron
        
        // Activity thresholds
        double firingThreshold;        // Firing threshold for rate-based output
        double maxFiringRate;          // Maximum firing rate (Hz)
    };
    
    // Plasticity configuration
    struct PlasticityConfig {
        // STDP parameters
        double stdpLtpWeight;          // LTP weight change magnitude
        double stdpLtdWeight;          // LTD weight change magnitude
        double stdpTau;                // STDP time constant (ms)
        double stdpMaxTime;            // Maximum STDP time window (ms)
        
        // Hebbian parameters
        double hebbianLearningRate;    // Hebbian learning rate
        double hebbianTimeConstant;    // Hebbian time constant (ms)
        
        // Structural plasticity
        double synaptogenesisRate;     // Rate of new synapse formation
        double pruningRate;           // Rate of synapse elimination
        double minSynapseWeight;      // Minimum weight for pruning
        double structuralPlasticityTimeConstant; // Time constant for structural plasticity
        
        // Reward-modulated plasticity
        double rewardLtpModulator;     // Reward modulation for LTP
        double rewardLtdModulator;     // Reward modulation for LTD
    };
    
    // Neuromodulation configuration
    struct NeuromodulationConfig {
        // Dopamine
        double dopamineBaseline;       // Baseline dopamine level
        double dopamineLearningRate;   // Dopamine learning rate
        double dopamineTimeConstant;   // Dopamine dynamics time constant (ms)
        double dopamineSensitivity;    // Dopamine sensitivity to reward prediction error
        
        // Novelty detection
        double noveltyThreshold;       // Novelty detection threshold
        double noveltyDecayRate;       // Novelty decay rate
        double noveltyLearningRate;    // Novelty learning rate
        
        // Curiosity
        double curiosityBaseline;      // Baseline curiosity level
        double curiosityLearningRate;  // Curiosity learning rate
        double curiosityTimeConstant;  // Curiosity dynamics time constant (ms)
        double curiosityActionSelectionWeight; // Weight for curiosity in action selection
        
        // Prediction error
        double predictionErrorDecayRate; // Prediction error decay rate
        double predictionErrorLearningRate; // Prediction error learning rate
    };
    
    // Memory system configuration
    struct MemoryConfig {
        // Working memory
        double workingMemoryCapacity;  // Number of items working memory can hold
        double workingMemoryDecayRate;  // Decay rate of working memory items
        double workingMemoryCompetitionStrength; // Competition strength in working memory
        
        // Episodic memory
        double episodicMemoryCapacity; // Maximum number of episodic memories
        double episodicMemoryReplayRate; // Rate of memory replay
        double episodicMemoryConsolidationThreshold; // Threshold for memory consolidation
        
        // Associative memory
        double associativeMemoryCapacity; // Capacity of associative memory
        double associativeMemoryDecayRate; // Decay rate of associative memories
        double associativeMemoryStrength;  // Strength of associative memory connections
        
        // Semantic memory
        double semanticMemoryCapacity;  // Capacity of semantic memory
        double semanticMemoryLearningRate; // Learning rate for semantic memory
    };
    
    // Developmental configuration
    struct DevelopmentConfig {
        // Developmental stages
        double initialStageAge;        // Age at which initial stage ends (ms)
        double criticalPeriodAge;      // Age at which critical period ends (ms)
        double maturationAge;          // Age at which maturation ends (ms)
        double adultAge;               // Age at which adult stage begins (ms)
        
        // Plasticity modulation by development
        double initialPlasticityLevel;   // Plasticity level in initial stage
        double criticalPeriodPlasticityLevel; // Plasticity level in critical period
        double maturationPlasticityLevel; // Plasticity level in maturation
        double adultPlasticityLevel;     // Plasticity level in adult stage
        
        // Structural development
        double synaptogenesisBias;    // Bias towards synaptogenesis in development
        double pruningBias;           // Bias towards pruning in development
        double maturationBias;        // Bias towards maturation in development
    };
    
    // Performance and optimization configuration
    struct PerformanceConfig {
        // Simulation parameters
        double timeStep;               // Simulation time step (ms)
        double maxSimTime;             // Maximum simulation time (ms)
        size_t maxNeuronCount;         // Maximum number of neurons to simulate
        size_t maxSynapseCount;        // Maximum number of synapses
        
        // Memory optimization
        size_t memoryPoolSize;         // Size of memory pools
        size_t eventQueueSize;         // Size of event queues
        bool enableSIMD;               // Enable SIMD optimizations
        bool enableVectorization;      // Enable vectorization optimizations
        
        // Parallel processing
        size_t maxThreads;             // Maximum number of threads for parallel processing
        double loadBalancingThreshold; // Load balancing threshold
        
        // Checkpointing
        double checkpointInterval;     // Checkpoint saving interval (ms)
        std::string checkpointFormat;  // Checkpoint file format
        bool enableCompression;        // Enable checkpoint compression
    };
    
    // Agent configuration
    struct AgentConfig {
        // Sensory processing
        size_t visionWidth;            // Vision width in pixels
        size_t visionHeight;           // Vision height in pixels
        size_t touchSensorCount;       // Number of touch sensors
        size_t internalSignalCount;    // Number of internal signals
        size_t proprioceptionCount;   // Number of proprioception signals
        
        // Motor control
        size_t motorActionCount;       // Number of motor actions
        double actionSelectionThreshold; // Threshold for action selection
        double actionSelectionNoise;   // Noise level for action selection
        
        // Agent behavior
        double curiosityThreshold;     // Threshold for curiosity-driven exploration
        double noveltyThreshold;       // Threshold for novelty detection
        double rewardThreshold;        // Threshold for reward detection
        
        // Learning parameters
        double learningRate;           // General learning rate
        double explorationRate;        // Exploration rate
        double exploitationRate;       // Exploitation rate
        double balanceRatio;           // Balance between exploration and exploitation
    };
    
    // Environment configuration
    struct EnvironmentConfig {
        // World parameters
        double worldWidth;             // World width
        double worldHeight;            // World height
        double worldDepth;             // World depth (for 3D environments)
        
        // Physics
        double gravity;                // Gravity constant
        double friction;               // Friction coefficient
        double restitution;            // Restitution coefficient
        
        // Resources and hazards
        double resourceValue;          // Value of resources
        double hazardDamage;           // Damage from hazards
        double visibilityRange;        // Vision range
        
        // Dynamics
        double dynamicsTimeStep;       // Dynamics integration time step
        double maxVelocity;            // Maximum velocity
        double maxAngularVelocity;     // Maximum angular velocity
    };
    
    // Get configuration values by section
    NeuralNetworkConfig getNeuralNetworkConfig() const { return neuralNetworkConfig_; }
    PlasticityConfig getPlasticityConfig() const { return plasticityConfig_; }
    NeuromodulationConfig getNeuromodulationConfig() const { return neuromodulationConfig_; }
    MemoryConfig getMemoryConfig() const { return memoryConfig_; }
    DevelopmentConfig getDevelopmentConfig() const { return developmentConfig_; }
    PerformanceConfig getPerformanceConfig() const { return performanceConfig_; }
    AgentConfig getAgentConfig() const { return agentConfig_; }
    EnvironmentConfig getEnvironmentConfig() const { return environmentConfig_; }
    
    // Set configuration values
    void setNeuralNetworkConfig(const NeuralNetworkConfig& config) { neuralNetworkConfig_ = config; }
    void setPlasticityConfig(const PlasticityConfig& config) { plasticityConfig_ = config; }
    void setNeuromodulationConfig(const NeuromodulationConfig& config) { neuromodulationConfig_ = config; }
    void setMemoryConfig(const MemoryConfig& config) { memoryConfig_ = config; }
    void setDevelopmentConfig(const DevelopmentConfig& config) { developmentConfig_ = config; }
    void setPerformanceConfig(const PerformanceConfig& config) { performanceConfig_ = config; }
    void setAgentConfig(const AgentConfig& config) { agentConfig_ = config; }
    void setEnvironmentConfig(const EnvironmentConfig& config) { environmentConfig_ = config; }
    
    // Get individual configuration values
    template<typename T>
    T getValue(const std::string& section, const std::string& key) const;
    
    // Set individual configuration values
    template<typename T>
    void setValue(const std::string& section, const std::string& key, const T& value);
    
    // Check if configuration exists
    bool hasValue(const std::string& section, const std::string& key) const;
    
    // Get all configuration keys for a section
    std::vector<std::string> getKeys(const std::string& section) const;
    
    // Get all section names
    std::vector<std::string> getSections() const;
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Reset to default configuration
    void resetToDefaults();
    
    // Get configuration summary
    std::string summary() const;
    
    // Apply configuration to existing Config object
    void applyTo(Config& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Default configuration instances
    NeuralNetworkConfig neuralNetworkConfig_;
    PlasticityConfig plasticityConfig_;
    NeuromodulationConfig neuromodulationConfig_;
    MemoryConfig memoryConfig_;
    DevelopmentConfig developmentConfig_;
    PerformanceConfig performanceConfig_;
    AgentConfig agentConfig_;
    EnvironmentConfig environmentConfig_;
    
    // Internal helpers
    static std::string getSectionPrefix(const std::string& section) {
        return section + ".";
    }
    
    static std::pair<std::string, std::string> splitKey(const std::string& fullKey) {
        size_t pos = fullKey.find('.');
        if (pos == std::string::npos) {
            return {fullKey, ""};
        }
        return {fullKey.substr(0, pos), fullKey.substr(pos + 1)};
    }
};

// Helper function to create a default advanced configuration
AdvancedConfig createDefaultAdvancedConfig();

} // namespace nlm
