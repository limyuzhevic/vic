#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace nlm {

// Forward declarations
class RandomGenerator;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced neuromodulation
class AgentBrain {
public:
    // Configuration structure for advanced AgentBrain settings
    struct Config {
        // Neuromodulation settings
        bool rewardModulationEnabled = true;
        bool curiosityEnabled = true;
        bool noveltyDetectionEnabled = true;
        bool predictionErrorEnabled = true;
        bool structuralPlasticityEnabled = true;
        bool developmentEnabled = true;
        
        // Advanced curiosity settings
        float curiosityThreshold = 0.3f;
        float explorationMultiplier = 1.5f;
        float noveltySensitivity = 2.0f;
        float predictionErrorWeight = 0.5f;
        
        // Development settings
        double developmentalStartAge = 0.0;
        double criticalPeriodStart = 60.0;  // seconds
        double criticalPeriodEnd = 300.0;   // seconds
        double maturationEnd = 900.0;       // seconds
        
        // Plasticity modulation
        bool adaptivePlasticity = true;
        float plasticityMaxFactor = 2.0f;
        float plasticityMinFactor = 0.1f;
        
        // State management
        bool enableStateCheckpointing = true;
        bool enableEpisodeManagement = true;
        size_t maxEpisodeLength = 1000;
        
        // Performance optimization
        bool enableSensoryCompression = true;
        bool enableMotorSelectionOptimization = true;
        
        // Callbacks and hooks
        std::function<void(const AgentBrain&)> episodeStartCallback;
        std::function<void(const AgentBrain&)> episodeEndCallback;
        std::function<float(const SensoryPercept&)> customCuriosityCallback;
        std::function<void(float, float)> rewardModulationHook;
        
        // Configuration loading
        bool loadFromFile(const std::string& filepath);
        bool saveToFile(const std::string& filepath) const;
        
        // Configuration validation
        bool validate() const;
        std::string getValidationErrors() const;
        
        // Reset to defaults
        void resetToDefaults();
        
        // Get default configuration
        static Config getDefaultConfig();
    };
    
    // Statistical tracking for performance monitoring
    struct Statistics {
        uint64_t totalSteps = 0;
        uint64_t totalEpisodes = 0;
        uint64_t totalSpikes = 0;
        double averageFiringRate = 0.0;
        double averageCuriosityLevel = 0.0;
        double averageNoveltyLevel = 0.0;
        double averagePredictionError = 0.0;
        double developmentalProgress = 0.0;
        float memoryUsage = 0.0f;
        
        void reset();
        void update(const AgentBrain& agent);
        std::string toString() const;
    };
    
    // Advanced AgentBrain constructor with configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config::getDefaultConfig());
    
    ~AgentBrain();
    
    // === Core Functionality ===
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world, const Config& config = Config::getDefaultConfig());
    
    // Process sensory percept and inject into brain with advanced features
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command with curiosity/exploration
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation with advanced prediction and learning
    void applyRewardModulation(float reward, float predictedReward, float uncertainty = 1.0f);
    
    // Update development system with enhanced modeling
    void updateDevelopment(double timestep);
    
    // Get current developmental stage with enhanced tracking
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Enhanced neuromodulation accessors
    float getDopamineLevel() const { return dopamineLevel_; }
    float getSerotoninLevel() const { return serotoninLevel_; }
    float getNorepinephrineLevel() const { return norepinephrineLevel_; }
    float getAcetylcholineLevel() const { return acetylcholineLevel_; }
    
    // Enhanced curiosity and novelty detection
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Enhanced state management
    void reset();
    void resetEpisode();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // === Configuration and Management ===
    
    // Get current configuration
    const Config& getConfig() const { return config_; }
    void updateConfig(const Config& config);
    
    // Enable/disable subsystems
    void enableRewardModulation(bool enable) { config_.rewardModulationEnabled = enable; }
    void enableStructuralPlasticity(bool enable) { config_.structuralPlasticityEnabled = enable; }
    void enableDevelopment(bool enable) { config_.developmentEnabled = enable; }
    void enableCuriosity(bool enable) { config_.curiosityEnabled = enable; }
    
    bool isRewardModulationEnabled() const { return config_.rewardModulationEnabled; }
    bool isStructuralPlasticityEnabled() const { return config_.structuralPlasticityEnabled; }
    bool isDevelopmentEnabled() const { return config_.developmentEnabled; }
    bool isCuriosityEnabled() const { return config_.curiosityEnabled; }
    
    // === Advanced Neuromodulation ===
    
    // Get advanced neuromodulation states
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getUncertainty() const { return uncertainty_; }
    float getAttentionLevel() const { return attentionLevel_; }
    
    // === Statistics and Monitoring ===
    
    // Get performance statistics
    const Statistics& getStatistics() const { return stats_; }
    std::string getStatisticsString() const;
    
    // === State Management ===
    
    // Checkpoint and restore functionality
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    std::string getStateFingerprint() const;
    
    // === Event System ===
    
    // Register callbacks for events
    void setEpisodeStartCallback(std::function<void(const AgentBrain&)> callback) {
        config_.episodeStartCallback = callback;
    }
    void setEpisodeEndCallback(std::function<void(const AgentBrain&)> callback) {
        config_.episodeEndCallback = callback;
    }
    void setCustomCuriosityCallback(std::function<float(const SensoryPercept&)> callback) {
        config_.customCuriosityCallback = callback;
    }
    
    // === Utility Methods ===
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Get current simulation time
    double getSimulationTime() const { return simulationTime_; }
    
    // Get developmental progress (0.0 to 1.0)
    double getDevelopmentalProgress() const;
    
    // Check if agent is in critical developmental period
    bool isInCriticalPeriod() const;
    
    // Get active neuron count
    size_t getActiveNeuronCount() const;
    
    // Get learning rate
    float getLearningRate() const;
    
    // === Deprecated Methods (for backward compatibility) ===
    
    // Backward compatibility methods
    MotorCommand decodeMotorCommand() { return decodeMotorCommand(); }
    void applyRewardModulation(float reward, float predictedReward) {
        applyRewardModulation(reward, predictedReward, 1.0f);
    }
    void updateDevelopment(double timestep) { updateDevelopment(timestep); }
    DevelopmentalStage getDevelopmentalStage() const { return getDevelopmentalStage(); }
    float getNeuromodulationLevel() const { return getOverallModulationLevel(); }
    float getCuriosityLevel() const { return getCuriosityLevel(); }
    float getNoveltyLevel() const { return getNoveltyLevel(); }
    float getPredictionError() const { return getPredictionError(); }
    void reset() { reset(); }
    
    // === Internal Processing ===
    
    // Process and enhance sensory input
    void processEnhancedSensoryInput(const SensoryPercept& percept);
    
    // Enhanced motor decoding with multiple strategies
    MotorCommand decodeAdvancedMotorCommand();
    
    // Enhanced curiosity-driven exploration
    MotorCommand selectWithAdvancedCuriosity(MotorCommand defaultCmd);
    
    // Compute enhanced neuromodulation signals
    void computeEnhancedNeuromodulation(const SensoryPercept& percept, float reward, float predictedReward);
    
    // Update advanced development model
    void updateAdvancedDevelopment(double timestep);
    
    // Update statistics
    void updateStatistics();
    
private:
    // === Internal State ===
    
    std::shared_ptr<Brain> brain_;
    Config config_;
    Statistics stats_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Advanced neuromodulation state
    float dopamineLevel_;
    float serotoninLevel_;
    float norepinephrineLevel_;
    float acetylcholineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float uncertainty_;
    float attentionLevel_;
    
    // Development state
    double developmentalAge_;
    double simulationTime_;
    float plasticityModifier_;
    float memoryStrength_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool noveltyDetectionEnabled_;
    bool predictionErrorEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Episode management
    size_t currentEpisodeLength_;
    uint64_t episodeStartStep_;
    
    // State management
    std::unordered_map<std::string, std::string> stateCache_;
    std::string lastStateFingerprint_;
    
    // === Internal Processing Methods ===
    
    // Initialize neuron groups
    void initializeNeuronGroups();
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Compute enhanced novelty
    void computeEnhancedNovelty(const SensoryPercept& percept);
    
    // Compute curiosity
    void computeCuriosity(const SensoryPercept& percept);
    
    // Compute prediction error
    void computePredictionError(float reward, float predictedReward);
    
    // Update neuromodulation state
    void updateNeuromodulationState();
    
    // Update development state
    void updateDevelopmentState(double timestep);
    
    // Compress sensory input
    std::vector<float> compressSensoryInput(const SensoryPercept& percept) const;
    
    // Optimize motor selection
    MotorCommand optimizeMotorSelection(const std::vector<MotorCommand>& candidates) const;
    
    // Validate configuration
    bool validateConfig() const;
    
    // Update state cache
    void updateStateCache();
    
    // Call episode callbacks
    void callEpisodeStartCallback();
    void callEpisodeEndCallback();
    
    // Get overall neuromodulation level
    float getOverallModulationLevel() const;
};

} // namespace nlm
