// AgentBrain.hpp - Improved version with advanced configuration and monitoring
#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <functional>

namespace nlm {

// Brain modes for advanced control
enum class BrainMode {
    Basic,
    Adaptive,
    Predictive,
    CuriosityDriven,
    Exploration,
    Exploitation,
    MetaLearning
};

// Motor neuron distribution strategies
enum class MotorDistributionStrategy {
    Balanced,           // Equal distribution (current behavior)
    Clustered,          // Cluster neurons by action type
    Adaptive,           // Dynamically redistribute based on usage
    Priority,           // Give priority to commonly used actions
    Random              // Random distribution
};

// Sensory processing modes
enum class SensoryProcessingMode {
    Basic,              // Current behavior
    Adaptive,           // Dynamically adjust sensitivity
    Predictive,         // Use predictions to enhance signals
    Sparse,             // Only process significant changes
    Hierarchical        // Process at multiple levels
};

// Configuration structure for AgentBrain
struct AgentConfig {
    // Basic configuration (backward compatible)
    bool rewardModulationEnabled = true;
    bool structuralPlasticityEnabled = true;
    bool developmentEnabled = true;
    bool curiosityEnabled = true;
    
    // Advanced configuration options
    BrainMode brainMode = BrainMode::Adaptive;
    MotorDistributionStrategy motorStrategy = MotorDistributionStrategy::Balanced;
    SensoryProcessingMode sensoryMode = SensoryProcessingMode::Adaptive;
    
    // Processing parameters
    float sensoryGain = 1.0f;              // Sensory signal amplification
    float motorThreshold = 0.5f;           // Activity threshold for motor actions
    float noveltySensitivity = 1.0f;       // How sensitive to novel inputs
    float curiosityThreshold = 0.3f;       // Minimum curiosity for exploration
    float explorationProbability = 0.1f;    // Base exploration rate
    
    // Development parameters
    double criticalPeriodDuration = 300.0;  // Time in seconds
    float plasticityDecayRate = 0.99f;     // How quickly plasticity decays
    
    // Monitoring parameters
    float monitoringInterval = 0.1f;       // Seconds between monitoring checks
    bool enableDebugLogging = true;        // Enable debug output
    bool enableMetricsCollection = true;   // Collect performance metrics
    
    // Sensory processing parameters
    float noveltyDecayRate = 0.95f;        // How quickly novelty decays
    float predictionAccuracyThreshold = 0.8f; // Threshold for successful prediction
    
    // Motor neuron distribution parameters
    float minNeuronPerGroup = 1.0f;        // Minimum neurons per action group
    float maxNeuronPerGroup = 10.0f;       // Maximum neurons per action group
    float redistributionThreshold = 0.7f;   // When to redistribute neurons
    
    // Advanced state management
    bool enableStatePersistence = false;    // Save/restore brain state
    std::string stateFilePath = "";         // Path for state file
    float stateCompressionRatio = 0.5f;     // Compression for state saving
    
    // Callbacks for events
    std::function<void(const std::string&)> onStateChanged = nullptr;
    std::function<void(float)> onPerformanceMetric = nullptr;
    std::function<void(BrainMode)> onBrainModeChanged = nullptr;
    
    // Validation methods
    bool validate() const {
        return sensoryGain > 0.0f && motorThreshold >= 0.0f && motorThreshold <= 1.0f &&
               noveltySensitivity > 0.0f && curiosityThreshold >= 0.0f && curiosityThreshold <= 1.0f &&
               explorationProbability >= 0.0f && explorationProbability <= 1.0f &&
               criticalPeriodDuration > 0.0 && plasticityDecayRate > 0.0f &&
               monitoringInterval > 0.0f && noveltyDecayRate > 0.0f &&
               predictionAccuracyThreshold >= 0.0f && predictionAccuracyThreshold <= 1.0f &&
               minNeuronPerGroup > 0.0f && maxNeuronPerGroup >= minNeuronPerGroup &&
               redistributionThreshold >= 0.0f && redistributionThreshold <= 1.0f &&
               stateCompressionRatio >= 0.0f && stateCompressionRatio <= 1.0f;
    }
    
    // Serialization support
    std::string toString() const;
    bool fromString(const std::string& data);
    
    // Equality comparison
    bool operator==(const AgentConfig& other) const {
        return std::tie(
            rewardModulationEnabled,
            structuralPlasticityEnabled,
            developmentEnabled,
            curiosityEnabled,
            brainMode,
            motorStrategy,
            sensoryMode,
            sensoryGain,
            motorThreshold,
            noveltySensitivity,
            curiosityThreshold,
            explorationProbability,
            criticalPeriodDuration,
            plasticityDecayRate,
            monitoringInterval,
            enableDebugLogging,
            enableMetricsCollection,
            noveltyDecayRate,
            predictionAccuracyThreshold,
            minNeuronPerGroup,
            maxNeuronPerGroup,
            redistributionThreshold,
            enableStatePersistence,
            stateFilePath,
            stateCompressionRatio
        ) == std::tie(
            other.rewardModulationEnabled,
            other.structuralPlasticityEnabled,
            other.developmentEnabled,
            other.curiosityEnabled,
            other.brainMode,
            other.motorStrategy,
            other.sensoryMode,
            other.sensoryGain,
            other.motorThreshold,
            other.noveltySensitivity,
            other.curiosityThreshold,
            other.explorationProbability,
            other.criticalPeriodDuration,
            other.plasticityDecayRate,
            other.monitoringInterval,
            other.enableDebugLogging,
            other.enableMetricsCollection,
            other.noveltyDecayRate,
            other.predictionAccuracyThreshold,
            other.minNeuronPerGroup,
            other.maxNeuronPerGroup,
            other.redistributionThreshold,
            other.enableStatePersistence,
            other.stateFilePath,
            other.stateCompressionRatio
        );
    }
};

// Debug information structure
struct DebugInfo {
    double currentTime;
    BrainMode currentBrainMode;
    float neuromodulationLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    DevelopmentalStage developmentalStage;
    float plasticityModifier;
    double developmentalAge;
    size_t motorNeuronsTotal;
    size_t sensoryNeuronsTotal;
    std::map<std::string, float> performanceMetrics;
    std::vector<std::string> activeEvents;
    std::string lastError;
    int debugLogLevel;
};

// AgentBrain: Connects NLM brain to the world with advanced features
// Handles sensory transduction, motor decoding, and advanced monitoring
class AgentBrain {
public:
    // Original constructor (backward compatible)
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    // Advanced constructor with configuration
    AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config);
    
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods (backward compatible)
    void enableRewardModulation(bool enable) { config_.rewardModulationEnabled = enable; }
    void enableStructuralPlasticity(bool enable) { config_.structuralPlasticityEnabled = enable; }
    void enableDevelopment(bool enable) { config_.developmentEnabled = enable; }
    void enableCuriosity(bool enable) { config_.curiosityEnabled = enable; }
    
    bool isRewardModulationEnabled() const { return config_.rewardModulationEnabled; }
    bool isStructuralPlasticityEnabled() const { return config_.structuralPlasticityEnabled; }
    bool isDevelopmentEnabled() const { return config_.developmentEnabled; }
    bool isCuriosityEnabled() const { return config_.curiosityEnabled; }
    
    // New advanced configuration methods
    void setBrainMode(BrainMode mode);
    void setMotorDistributionStrategy(MotorDistributionStrategy strategy);
    void setSensoryProcessingMode(SensoryProcessingMode mode);
    void setConfig(const AgentConfig& config);
    const AgentConfig& getConfig() const { return config_; }
    
    // Debug and monitoring methods
    void setDebugLogging(bool enabled) { config_.enableDebugLogging = enabled; }
    void setMetricsCollection(bool enabled) { config_.enableMetricsCollection = enabled; }
    DebugInfo getDebugInfo() const;
    void logDebugInfo(const std::string& context = "");
    
    // State management methods
    bool saveState(const std::string& filePath = "");
    bool loadState(const std::string& filePath = "");
    
    // Configuration serialization
    static std::string configToString(const AgentConfig& config);
    static AgentConfig configFromString(const std::string& data);
    std::string getCurrentConfigString() const { return config_.toString(); }
    bool updateConfigFromString(const std::string& data);
    
    // Callback management
    void setStateChangedCallback(std::function<void(const std::string&)> callback) {
        config_.onStateChanged = callback;
    }
    void setPerformanceMetricCallback(std::function<void(float)> callback) {
        config_.onPerformanceMetric = callback;
    }
    void setBrainModeChangedCallback(std::function<void(BrainMode)> callback) {
        config_.onBrainModeChanged = callback;
    }
    
    // Performance metrics
    float getAverageReward() const { return averageReward_; }
    float getExplorationRate() const { return explorationRate_; }
    float getLearningProgress() const { return learningProgress_; }
    
    // Event system
    void addEvent(const std::string& event) { activeEvents_.push_back(event); }
    void clearEvents() { activeEvents_.clear(); }
    const std::vector<std::string>& getActiveEvents() const { return activeEvents_; }
    
    // Neural group access for advanced users
    const std::vector<Neuron*>& getMotorForwardNeurons() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackwardNeurons() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeftNeurons() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRightNeurons() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteractNeurons() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWaitNeurons() const { return motorWait_; }
    
    const std::vector<Neuron*>& getSensoryVisionNeurons() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouchNeurons() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternalNeurons() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioceptionNeurons() const { return sensoryProprioception_; }
    
    // Monitoring and statistics
    void startMonitoring(double duration);
    void stopMonitoring();
    double getMonitoringDuration() const { return monitoringDuration_; }
    float getMonitoringFPS() const { return monitoringFPS_; }
    
    // Advanced sensory processing
    void setSensoryParameters(float gain, float threshold) {
        config_.sensoryGain = gain;
        config_.noveltySensitivity = threshold;
    }
    
    // Advanced motor processing
    void setMotorParameters(float threshold, float exploration) {
        config_.motorThreshold = threshold;
        config_.explorationProbability = exploration;
    }
    
private:
    // Original constructor implementation
    void initializeWithDefaults();
    
    // Advanced initialization
    void initializeAdvanced(std::shared_ptr<Brain> brain, const AgentConfig& config);
    
    // Motor neuron distribution strategies
    void distributeMotorNeuronsBalanced();
    void distributeMotorNeuronsClustered();
    void distributeMotorNeuronsAdaptive();
    void distributeMotorNeuronsPriority();
    void distributeMotorNeuronsRandom();
    
    // Sensory processing modes
    void processSensoryBasic(const SensoryPercept& percept);
    void processSensoryAdaptive(const SensoryPercept& percept);
    void processSensoryPredictive(const SensoryPercept& percept);
    void processSensorySparse(const SensoryPercept& percept);
    void processSensoryHierarchical(const SensoryPercept& percept);
    
    // Monitoring and metrics
    void updateMetrics(double timestep);
    void collectPerformanceMetrics();
    void updateLearningProgress();
    
    // State management
    bool saveStateToFile(const std::string& filePath);
    bool loadStateFromFile(const std::string& filePath);
    std::string compressState(const std::string& state) const;
    std::string decompressState(const std::string& compressed) const;
    
    // Debug logging
    void logDebug(const std::string& message, int level = 1) const;
    void debugLogInfo(const std::string& context = "") const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (advanced with dynamic sizing)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (dynamic sizing)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration (advanced)
    AgentConfig config_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Performance metrics
    float averageReward_;
    float explorationRate_;
    float learningProgress_;
    int rewardCount_;
    double lastUpdateTime_;
    
    // Monitoring
    bool monitoringActive_;
    double monitoringStartTime_;
    double monitoringDuration_;
    double monitoringFPS_;
    int frameCount_;
    
    // Events
    std::vector<std::string> activeEvents_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods
    float calculateNeuralActivity(const std::vector<Neuron*>& neurons) const;
    bool shouldExplore() const;
    BrainMode determineOptimalBrainMode() const;
    
    // Friend classes for serialization
    friend class AgentBrainSerializer;
};

// Serialization helper class
class AgentBrainSerializer {
public:
    static std::string serialize(const AgentBrain& agentBrain);
    static bool deserialize(std::string& data, AgentBrain& agentBrain);
};