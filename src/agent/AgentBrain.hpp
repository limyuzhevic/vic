#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <unordered_map>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Enhanced with comprehensive error handling, advanced features, and power user commands
class AgentBrain {
public:
    // Constructor with optional configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, 
                      const std::unordered_map<std::string, float>& config = {});
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world and optional configuration
    bool initialize(const SimpleWorld& world, 
                   const std::unordered_map<std::string, float>& config = {});
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Get advanced statistics about current neural state
    struct NeuralStats {
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        float avgFiringRate;
        float excitationInhibitionRatio;
        float avgMembranePotential;
        float maxMembranePotential;
        float minMembranePotential;
        size_t totalSpikes;
        size_t pendingSpikes;
        std::unordered_map<NeuronType, size_t> neuronsByType;
        std::unordered_map<std::string, float> additionalMetrics;
    };
    
    NeuralStats getNeuralStats() const;
    
    // Process sensory percept and inject into brain
    bool processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command with advanced options
    MotorCommand decodeMotorCommand(float curiosityThreshold = 0.3f,
                                   float activityThreshold = 0.5f,
                                   bool enableExploration = true);
    
    // Apply neuromodulation based on reward with enhanced feedback
    bool applyRewardModulation(float reward, float predictedReward = 0.0f,
                              float learningRate = 0.05f,
                              bool enableErrorClamping = true);
    
    // Update development system with detailed feedback
    bool updateDevelopment(double timestep, bool forceStageChange = false);
    
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
    
    // Get expected reward (for prediction)
    float getExpectedReward() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Get previous vision data for analysis
    std::optional<std::vector<float>> getPreviousVision() const;
    
    // Advanced motor command generation with multiple strategies
    MotorCommand advancedMotorCommand(const std::string& strategy = "max_activity",
                                     float explorationIntensity = 0.5f,
                                     bool useHistory = false);
    
    // Enhanced curiosity-based exploration with multiple modes
    MotorCommand exploreWithStrategy(const std::string& strategy = "random",
                                    float intensity = 1.0f,
                                    const std::vector<MotorCommand>& forbiddenCommands = {});
    
    // Get statistics for power users
    struct AdvancedStats {
        std::unordered_map<MotorCommand, float> motorCommandActivity;
        std::unordered_map<std::string, float> neuromodulatorLevels;
        std::vector<float> sensorySignalStats;
        double lastUpdateTime;
        std::string currentStrategy;
        float explorationEfficiency;
        size_t successfulActions;
        size_t totalActions;
    };
    
    AdvancedStats getAdvancedStats() const;
    
    // Reset agent for new episode with detailed feedback
    bool reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Enhanced configuration methods
    void enableRewardModulation(bool enable, float learningRate = 0.05f);
    void enableStructuralPlasticity(bool enable, float growthRate = 0.0001, float pruneRate = 0.00001);
    void enableDevelopment(bool enable, const std::string& developmentStrategy = "standard");
    void enableCuriosity(bool enable, float noveltySensitivity = 2.0f, float errorWeight = 0.5f);
    void setCuriosityThreshold(float threshold);
    void setActivityThreshold(float threshold);
    void setExplorationStrategy(const std::string& strategy);
    void setMotorDecodingStrategy(const std::string& strategy);
    void setSensoryPreprocessing(bool enabled, float noiseReduction = 0.9f);
    
    // Configuration getters
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getActivityThreshold() const { return activityThreshold_; }
    const std::string& getExplorationStrategy() const { return explorationStrategy_; }
    const std::string& getMotorDecodingStrategy() const { return motorDecodingStrategy_; }
    bool isSensoryPreprocessingEnabled() const { return sensoryPreprocessingEnabled_; }
    
    // Advanced command processing for power users
    bool batchProcessSensoryInputs(const std::vector<const SensoryPercept*>& percepts);
    std::vector<MotorCommand> batchDecodeMotorCommands(size_t count);
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    
    // Performance and monitoring
    struct PerformanceMetrics {
        double cpuTimeUsed;
        double memoryUsage;
        size_t sensoryProcessingCount;
        size_t motorDecodingCount;
        size_t plasticityUpdates;
        double avgProcessingTime;
        size_t errorCount;
        bool isHealthy;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Debug and diagnostic tools
    bool verifySystemIntegrity() const;
    std::vector<std::string> getWarnings() const;
    std::vector<std::string> getErrors() const;
    std::string getSystemStatus() const;
    void clearDiagnosticLogs();
    
    // Configuration from initialization
    struct AgentConfig {
        float curiosityThreshold = 0.3f;
        float activityThreshold = 0.5f;
        float learningRate = 0.05f;
        float noveltySensitivity = 2.0f;
        float errorWeight = 0.5f;
        float noiseReduction = 0.9f;
        std::string explorationStrategy = "balanced";
        std::string motorDecodingStrategy = "max_activity";
        bool enableSensoryPreprocessing = false;
        size_t maxMotorNeuronsPerGroup = 100;
        size_t maxSensoryNeuronsPerGroup = 100;
        float noveltyDecay = 0.99f;
        float plasticityGrowthRate = 0.0001;
        float plasticityPruneRate = 0.00001;
        std::string developmentStrategy = "standard";
        bool enableErrorClamping = true;
        bool enablePerformanceMonitoring = false;
        size_t performanceSampleSize = 100;
    };
    
    const AgentConfig& getConfig() const { return config_; }
    void updateConfig(const AgentConfig& newConfig);
    
private:
    // Error handling and validation
    bool validateInputs() const;
    void logError(const std::string& error) const;
    void logWarning(const std::string& warning) const;
    
    // Motor decoding with multiple strategies
    MotorCommand decodeFromMotorNeuronsMaxActivity() const;
    MotorCommand decodeFromMotorNeuronsAverageActivity() const;
    MotorCommand decodeFromMotorNeuronsWeightedActivity() const;
    
    // Motor command selection with advanced exploration
    MotorCommand selectWithCuriosityAdvanced(MotorCommand defaultCmd, float intensity) const;
    MotorCommand selectWithEntropy(float entropy, MotorCommand defaultCmd) const;
    MotorCommand selectWithThompsonSampling(const std::vector<MotorCommand>& options) const;
    
    // Sensory processing with preprocessing
    void processVisionInput(const std::vector<float>& vision, size_t index);
    void processTouchInput(const std::vector<float>& touch, size_t index);
    void processInternalInput(const std::vector<float>& internal, size_t index);
    void processProprioceptionInput(const std::vector<float>& proprio, size_t index);
    
    // Statistical calculations
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    float calculateFiringRate(float current, float resting) const;
    float calculateNovelty(const std::vector<float>& current, const std::vector<float>& previous) const;
    
    // State management
    void updatePerformanceMetrics() const;
    void updateDiagnosticLogs() const;
    
    // Configuration
    std::shared_ptr<Brain> brain_;
    AgentConfig config_;
    
    // Motor neuron groups (with optional capacity limits)
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
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    std::string currentDevelopmentStage_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool sensoryPreprocessingEnabled_;
    
    // Thresholds and parameters
    float curiosityThreshold_;
    float activityThreshold_;
    float learningRate_;
    float noveltySensitivity_;
    float errorWeight_;
    float noiseReduction_;
    
    // Strategy selection
    std::string explorationStrategy_;
    std::string motorDecodingStrategy_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance monitoring
    mutable PerformanceMetrics perfMetrics_;
    mutable std::vector<std::string> errors_;
    mutable std::vector<std::string> warnings_;
    mutable double lastPerfUpdate_;
    
    // Statistical caches
    mutable std::unordered_map<std::string, float> statCache_;
    mutable bool statsDirty_;
    
    // State flags
    bool brainInitialized_;
    bool systemVerified_;
    
    // Helper methods for advanced features
    void initializeNeuronGroups();
    bool validateNeuronGroupCapacity() const;
    float calculateEntropy(const std::vector<float>& values) const;
    void updateStatCache() const;
    bool checkSystemHealth() const;
};

} // namespace nlm
