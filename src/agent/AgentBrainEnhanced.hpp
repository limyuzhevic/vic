# Improved AgentBrain.hpp - Enhanced with better initialization, error handling, and extensibility

#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

// Advanced configuration for AgentBrain
struct AgentBrainConfig {
    // Neuron group distribution
    size_t motorForwardCount = 0;
    size_t motorBackwardCount = 0;
    size_t motorTurnLeftCount = 0;
    size_t motorTurnRightCount = 0;
    size_t motorInteractCount = 0;
    size_t motorWaitCount = 0;
    
    size_t sensoryVisionCount = 0;
    size_t sensoryTouchCount = 0;
    size_t sensoryInternalCount = 0;
    size_t sensoryProprioceptionCount = 0;
    
    // Behavior configuration
    float curiosityThreshold = 0.3f;
    float explorationThreshold = 0.5f;
    float maxExplorationChance = 0.3f;
    float noveltyDecayFactor = 0.99f;
    float plasticityModifierBase = 1.0f;
    
    // Development configuration
    double developmentStage1Duration = 60.0;  // Initial
    double developmentStage2Duration = 240.0; // Critical period  
    double developmentStage3Duration = 600.0; // Maturation
    
    // Reward modulation configuration
    float rewardPredictionDecay = 0.95f;
    float rewardMinChange = 0.001f;
    float dopamineMinClamp = -1.0f;
    float dopamineMaxClamp = 1.0f;
    
    // Error handling configuration
    bool logErrors = true;
    bool continueOnError = false;
    bool validateInputRanges = true;
    
    // Performance configuration
    bool enablePerformanceOptimizations = true;
    size_t maxSensoryNeurons = 256;
    size_t maxMotorNeurons = 64;
    
    AgentBrainConfig() = default;
};

// AgentBrain: Connects NLM brain to the world with enhanced functionality
// Handles sensory transduction and motor decoding with improved error handling,
// configuration support, and extensibility for advanced use cases
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
    ~AgentBrain();
    
    // Initialize with world and configuration
    bool initialize(const SimpleWorld& world, bool logInitialization = true);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain with error handling
    bool processSensoryInput(const SensoryPercept& percept, bool validateInput = true);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward with enhanced error handling
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
    
    // Reset agent for new episode with validation
    bool reset(bool logReset = true);
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration methods
    void setConfig(const AgentBrainConfig& config) { config_ = config; }
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Enhanced behavior control
    void enableCuriosity(bool enable, float threshold = 0.0f);
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    
    bool isCuriosityEnabled() const { return config_.curiosityThreshold > 0.0f; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Error handling
    bool hasErrors() const { return !errorMessages_.empty(); }
    std::vector<std::string> getErrorMessages() const { return errorMessages_; }
    void clearErrors() { errorMessages_.clear(); }
    
    // Statistics and metrics
    size_t getMotorGroupCount(MotorCommand cmd) const;
    size_t getSensoryGroupCount(NeuronType type) const;
    float getAverageFiringRate() const;
    float getTotalSpikesProcessed() const;
    
    // Advanced features
    void addCustomMotorAction(const std::string& name, const std::vector<Neuron*>& neurons);
    void addCustomSensoryAction(const std::string& name, const std::vector<Neuron*>& neurons);
    MotorCommand customMotorAction(const std::string& name);
    
    // Debug and diagnostic
    void debugNeuronActivity() const;
    void logStatistics() const;
    
    // Configuration file support
    static AgentBrainConfig loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
private:
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    
    // Enhanced motor neuron groups
    std::vector<std::vector<Neuron*>> motorGroups_;
    std::vector<std::vector<Neuron*>> sensoryGroups_;
    std::vector<std::string> motorGroupNames_;
    std::vector<std::string> sensoryGroupNames_;
    
    // Enhanced neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityModifier_;
    
    // Development state
    double developmentalAge_;
    
    // State tracking
    float totalSpikesProcessed_;
    size_t sensoryEventsProcessed_;
    size_t motorCommandsGenerated_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Error handling
    std::vector<std::string> errorMessages_;
    
    // Custom actions for extensibility
    std::vector<std::function<MotorCommand()>> customMotorActions_;
    std::vector<std::function<void(const SensoryPercept&)>> customSensoryActions_;
    
    // Private helper methods
    bool validateBrain() const;
    bool validateInputRanges(const SensoryPercept& percept) const;
    
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    void updateNeuromodulationState();
    void updateDevelopmentalParameters();
    void logError(const std::string& error, bool fatal = false);
    
    // Enhanced motor decoding with custom actions
    MotorCommand decodeFromMotorNeuronsEnhanced();
    MotorCommand selectWithAdvancedCuriosity(MotorCommand defaultCmd);
    
    // Statistical calculations
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    MotorCommand findDominantCommand(const std::vector<MotorCommand>& candidates) const;
};

} // namespace nlm
