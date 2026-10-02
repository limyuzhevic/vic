#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Enhanced with advanced agent control, learning, and monitoring capabilities
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
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
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced Agent Control Methods
    void start() { isPaused_ = false; isStopped_ = false; startTime_ = std::chrono::steady_clock::now(); }
    void stop() { isStopped_ = true; isPaused_ = false; stopTime_ = std::chrono::steady_clock::now(); }
    void pause() { isPaused_ = true; }
    void resume() { isPaused_ = false; }
    bool isRunning() const { return !isStopped_ && !isPaused_; }
    bool isPaused() const { return isPaused_; }
    bool isStopped() const { return isStopped_; }
    
    // Advanced Configuration and Behavior Tuning
    void setLearningEnabled(bool enabled) { learningEnabled_ = enabled; }
    void setLearningRate(float rate) { learningRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setExplorationRate(float rate) { explorationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setMaxMemorySize(size_t size) { maxMemorySize_ = size; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    void setAdaptationSpeed(float speed) { adaptationSpeed_ = std::clamp(speed, 0.0f, 1.0f); }
    
    bool isLearningEnabled() const { return learningEnabled_; }
    float getLearningRate() const { return learningRate_; }
    float getExplorationRate() const { return explorationRate_; }
    size_t getMaxMemorySize() const { return maxMemorySize_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getAdaptationSpeed() const { return adaptationSpeed_; }
    
    // Agent Introspection and Statistics
    double getUptime() const;
    double getTotalTime() const;
    size_t getStepCount() const { return stepCount_; }
    float getAverageFiringRate() const;
    float getSynapticStability() const;
    std::vector<float> getWeightStatistics() const;
    std::string getPerformanceMetrics() const;
    
    // Agent Learning and Adaptation Control
    void enableLearning(bool enable) { learningEnabled_ = enable; }
    void adjustLearningRate(float factor) { learningRate_ *= factor; learningRate_ = std::clamp(learningRate_, 0.0f, 1.0f); }
    void setExplorationExploitation(float exploration) { explorationRate_ = std::clamp(exploration, 0.0f, 1.0f); }
    void resetAdaptation() { adaptationAccumulator_ = 0.0f; }
    
    // Agent State Management
    void checkpoint(const std::string& filename) const;
    void restore(const std::string& filename);
    void saveState(const std::string& filename) const;
    void loadState(const std::string& filename);
    
    // Performance Monitoring and Optimization
    void enablePerformanceMonitoring(bool enable) { performanceMonitoringEnabled_ = enable; }
    void setPerformanceThreshold(float threshold) { performanceThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    bool checkPerformance() const;
    float getPerformanceScore() const;
    
    // Command Line Integration Helpers
    static std::string getAgentCommandHelp();
    bool processCommand(const std::string& command, const std::string& arg = "");
    
    // Validation and Error Handling
    bool validateConfiguration() const;
    std::string getValidationErrors() const;
    
    // Statistics and Monitoring
    struct AgentStats {
        double elapsedTime;
        size_t stepsProcessed;
        float averageFiringRate;
        float synapticStability;
        float learningProgress;
        float performanceScore;
        size_t memoryUsage;
        int64_t totalSpikes;
    };
    
    AgentStats collectStats() const;
    
    // Callbacks and Event Handling
    void setStateChangeCallback(std::function<void(const std::string&)> callback) { stateChangeCallback_ = callback; }
    void setPerformanceAlertCallback(std::function<void(float)> callback) { performanceAlertCallback_ = callback; }
    
private:
    // Advanced Agent Control State
    bool isPaused_;
    bool isStopped_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point stopTime_;
    
    // Advanced Learning and Adaptation State
    bool learningEnabled_;
    float learningRate_;
    float explorationRate_;
    size_t maxMemorySize_;
    float curiosityThreshold_;
    float adaptationSpeed_;
    float adaptationAccumulator_;
    
    // Performance Monitoring
    bool performanceMonitoringEnabled_;
    float performanceThreshold_;
    mutable float peakPerformanceScore_;
    mutable std::vector<float> recentPerformance_;
    
    // Event Callbacks
    std::function<void(const std::string&)> stateChangeCallback_;
    std::function<void(float)> performanceAlertCallback_;
    
    // State Management
    std::string currentState_;
    std::vector<std::string> stateHistory_;
    
    // Legacy state (for backward compatibility)
    std::shared_ptr<Brain> brain_;
    
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
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Legacy statistics
    size_t stepCount_;
    mutable float totalFiringRate_;
    mutable size_t firingCount_;
};

} // namespace nlm
