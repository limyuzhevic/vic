#pragma once

#include "../core/Logger/Logger.hpp"

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
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Organize neurons by function for better performance
    void organizeNeurons();
    
    // Get total spike counts for debugging
    size_t getTotalMotorSpikeCount() const;
    size_t getTotalSensorySpikeCount() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - organized by action type for better performance
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups - organized by modality for efficient processing
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Advanced neuromodulation state with history tracking
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development and plasticity state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Advanced monitoring and debugging capabilities
    void enablePerformanceMonitoring(bool enable) { performanceMonitoringEnabled_ = enable; }
    void enableEventLogging(bool enable) { eventLoggingEnabled_ = enable; }
    void setLogLevel(LogLevel level) { logLevel_ = level; }
    
    bool isPerformanceMonitoringEnabled() const { return performanceMonitoringEnabled_; }
    bool isEventLoggingEnabled() const { return eventLoggingEnabled_; }
    LogLevel getLogLevel() const { return logLevel_; }
    
    // Debugging and diagnostic methods
    std::string getSystemStatus() const;
    std::string getPerformanceReport() const;
    std::vector<std::string> getEventLog() const;
    std::vector<float> getSensorStatistics() const;
    std::vector<float> getMotorStatistics() const;
    
    // Checkpoint and save/load functionality
    void checkpoint(const std::string& filename) const;
    bool loadCheckpoint(const std::string& filename);
    
    // Real-time monitoring
    double getUptime() const;
    size_t getTotalStepCount() const;
    float getSystemHealth() const;
    
    // Advanced configuration and tuning
    void setLearningRate(float rate) { learningRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setExplorationDecay(float decay) { explorationDecay_ = std::clamp(decay, 0.0f, 1.0f); }
    void setExploitationDecay(float decay) { exploitationDecay_ = std::clamp(decay, 0.0f, 1.0f); }
    
    float getLearningRate() const { return learningRate_; }
    float getExplorationDecay() const { return explorationDecay_; }
    float getExploitationDecay() const { return exploitationDecay_; }
    
    // System calibration and health monitoring
    bool calibrateSystem();
    std::vector<std::string> getCalibrationErrors() const;
    
private:
    // Performance monitoring state
    bool performanceMonitoringEnabled_;
    bool eventLoggingEnabled_;
    LogLevel logLevel_;
    
    // Debugging and tracking state
    double sessionStartTime_;
    size_t totalStepCount_;
    float systemHealth_;
    std::vector<std::string> eventLog_;
    
    // Advanced configuration
    float learningRate_;
    float explorationDecay_;
    float exploitationDecay_;
    
    // Calibration and health
    std::vector<std::string> calibrationErrors_;
    mutable std::mutex debugMutex_;
};

} // namespace nlm
