#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
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
    
    // Configuration getters with validation
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration methods
    void setNeuronGroupDistribution(bool optimizeForSpeed = false);
    void setSensoryMapping(const std::vector<float>& mapping);
    void setMotorCommandScaling(float scale = 1.0f);
    void setNoveltyThreshold(float threshold);
    void setCuriosityFactor(float factor);
    void setDevelopmentRate(double rate);
    
    // Debug and profiling methods
    void enableDebugMode(bool enable);
    void enableProfiling(bool enable);
    void enablePerformanceMonitoring(bool enable);
    
    // State management methods
    void saveAgentState(const std::string& filepath) const;
    void loadAgentState(const std::string& filepath);
    void exportBrainState(const std::string& filepath) const;
    void importBrainState(const std::string& filepath);
    
    // Performance and debugging methods
    void recordPerformanceMetrics(double timestep);
    void logPerformanceMetrics();
    double recordMemoryUsage() const;
    void startPerformanceTimer();
    double stopPerformanceTimer();
    void exportDebugInfo(const std::string& filepath) const;
    std::string getCurrentTimestamp() const;
    
private:
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool debugModeEnabled_;
    bool profilingEnabled_;
    bool performanceMonitoringEnabled_;
    bool performanceMetricsEnabled_;
    
    // Performance metrics
    struct PerformanceMetrics {
        double stepsPerSecond = 0.0;
        double avgStepTime = 0.0;
        double memoryUsage = 0.0;
        size_t neuronActivity = 0;
        size_t spikeCount = 0;
    };
    
    PerformanceMetrics performanceMetrics_;
    std::vector<double> recordedTimesteps_;
    std::chrono::steady_clock::time_point timerStart_;
    
    // State management
    struct AgentState {
        float dopamineLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float predictionError = 0.0f;
        float expectedReward = 0.0f;
        double developmentalAge = 0.0;
        float plasticityModifier = 1.0f;
        std::vector<float> previousVision;
        std::vector<Neuron*> motorForward;
        std::vector<Neuron*> motorBackward;
        std::vector<Neuron*> motorTurnLeft;
        std::vector<Neuron*> motorTurnRight;
        std::vector<Neuron*> motorInteract;
        std::vector<Neuron*> motorWait;
        std::vector<Neuron*> sensoryVision;
        std::vector<Neuron*> sensoryTouch;
        std::vector<Neuron*> sensoryInternal;
        std::vector<Neuron*> sensoryProprioception;
    };
    
    // Configuration parameters
    std::vector<float> sensoryMapping_;
    float motorCommandScaling_ = 1.0f;
    float noveltyThreshold_ = 0.5f;
    float curiosityFactor_ = 0.5f;
    double developmentRate_ = 1.0;
    
    // State
    AgentState currentState_;
    
    // Performance monitoring
    bool isPerformanceMetricsEnabled() const { return performanceMetricsEnabled_; }
}
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
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
};

} // namespace nlm
