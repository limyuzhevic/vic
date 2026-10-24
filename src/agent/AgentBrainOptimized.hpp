#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "AgentCommand.hpp"
#include "AdvancedAgentFeatures.hpp"
#include "PerformanceOptimizations.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
private:
    AgentCommandHandler commandHandler_;
    AdvancedAgentCommandHandler advancedCommandHandler_;
    AgentFeatureFlags featureFlags_;
    PerformanceMonitor performanceMonitor_;
    bool performanceMonitoringEnabled_;
    
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
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
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
    
    // Process command line
    bool processCommand(const std::string& commandLine);
    
    // Command registration helper
    void registerCommand(const std::string& name, std::shared_ptr<AgentCommand> command);
    
    // Advanced agent methods
    bool processAdvancedCommand(const std::string& commandLine);
    void enableFeature(const std::string& featureName);
    void disableFeature(const std::string& featureName);
    bool isFeatureEnabled(const std::string& featureName) const;
    void registerAdvancedCommand(const std::string& name, std::shared_ptr<AgentCommand> command);
    void setFeatureFlag(const std::string& featureName, float value);
    float getFeatureFlag(const std::string& featureName) const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    std::string getPerformanceReport() const;
    bool executeOptimizedCommand(const std::string& commandLine);
    std::vector<bool> executeBatchCommands(const std::vector<std::string>& commandLines);
    void clearCommandCache();
    size_t getCommandCacheSize() const;
};

} // namespace nlm