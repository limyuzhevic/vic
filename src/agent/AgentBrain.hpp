#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "neuromodulation/NeuromodulationConfig.hpp"
#include "neuromodulation/NeuromodulationController.hpp"
#include <memory>
#include <vector>
#include "EnhancedAgentBrainLogger.hpp"

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding, and integrates with neuromodulation systems
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const NeuromodulationConfig& config);
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
    
    // Configuration through neuromodulation config
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Access neuromodulation controller
    NeuromodulationController* getNeuromodulationController() { return neuromodulationController_.get(); }
    
    // Update neuromodulation controller
    void update(double timestep);
    
    // Signal reward to neuromodulation
    void signalReward(float reward);
    
    // Signal prediction error to neuromodulation
    void signalPredictionError(float error);
    
    // Enable/disable debug mode with enhanced logging
    void enableDebugMode();
    void disableDebugMode();
    bool isDebugModeEnabled() const;
    
    // Get performance metrics
    AgentBrainMetrics getPerformanceMetrics() const;
    
    // Log current state
    void logState(const std::string& snapshotName) const;
    
    // Set logging configuration
    void configureLogging(const AgentBrainLoggingConfig& config);
    
    // Enable/disable specific logging features
    void enablePerformanceMonitoring(bool enable);
    void enableVerboseLogging(bool enable);
    void enableStateDumping(bool enable);
    void enableEventLogging(bool enable);
    void enableMemoryTracking(bool enable);
    void enableDecisionLogging(bool enable);
    
    // Enable/disable specific logging features
    void enablePerformanceMonitoring(bool enable);
    void enableVerboseLogging(bool enable);
    void enableStateDumping(bool enable);
    void enableEventLogging(bool enable);
    void enableMemoryTracking(bool enable);
    void enableDecisionLogging(bool enable);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation controller
    std::unique_ptr<NeuromodulationController> neuromodulationController_;
    
    // AgentBrain-specific state variables (duplicates from original AgentBrain.cpp for backward compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (for backward compatibility)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Previous sensory state for novelty detection (in controller now)
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Enhanced logging and performance tracking
    std::unique_ptr<AgentBrainLogger> logger_;
    mutable AgentBrainMetrics metrics_;
    double lastUpdateTime_;
    
    // Configuration for logging
    AgentBrainLoggingConfig loggingConfig_;
    
    // Feature flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
};

} // namespace nlm
