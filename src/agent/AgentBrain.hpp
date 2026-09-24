#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Enhanced with configurable parameters and extensibility
class AgentBrain {
public:
    // Enhanced constructor with configuration support
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config);
    
    // Default constructor for backward compatibility
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
    void enableCuriosityExploration(bool enable) { curiosityExplorationEnabled_ = enable; }
    void enableDetailedLogging(bool enable) { detailedLoggingEnabled_ = enable; }
    void enableStatisticalAnalysis(bool enable) { statisticalAnalysisEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isCuriosityExplorationEnabled() const { return curiosityExplorationEnabled_; }
    bool isDetailedLoggingEnabled() const { return detailedLoggingEnabled_; }
    bool isStatisticalAnalysisEnabled() const { return statisticalAnalysisEnabled_; }
    
    // Configuration access
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Statistics access
    const AgentBrainStatistics& getStatistics() const { return statistics_; }
    
    // Update statistics
    void updateStatistics();
    
    // Save configuration to file
    bool saveConfiguration(const std::string& filename) const;
    
    // Load configuration from file
    bool loadConfiguration(const std::string& filename);
    
    // Reset to default configuration
    void resetToDefaults();
    
    // Parameter sweep support
    bool enableParameterSweep(const std::string& sweepName);
    bool hasNextParameterSet() const;
    void nextParameterSet();
    
    // Research mode
    void enableResearchMode(bool enable) { researchModeEnabled_ = enable; }
    bool isResearchModeEnabled() const { return researchModeEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced neuron group initialization
    void initializeNeuronGroups(const AgentBrainConfig& config);
    
    // Enhanced sensory processing
    void processSensoryInputEnhanced(const SensoryPercept& percept);
    
    // Enhanced neuromodulation
    void applyNeuromodulationEnhanced(float reward, float predictedReward);
    
    // Enhanced development system
    void updateDevelopmentEnhanced(double timestep);
    
    // Configuration
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    
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
    bool curiosityExplorationEnabled_;
    bool detailedLoggingEnabled_;
    bool statisticalAnalysisEnabled_;
    bool researchModeEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Statistics
    AgentBrainStatistics statistics_;
    
    // Parameter sweep
    std::map<std::string, std::vector<float>> parameterSweepValues_;
    std::vector<std::string> parameterSweepKeys_;
    size_t currentParameterIndex_;
    
    // Timing for research
    double lastUpdateTime_;
    double accumulatedTime_;
    
    // Research mode data
    std::map<std::string, std::vector<double>> researchData_;
    
    // Helper methods
    void updateResearchData(const std::string& key, double value);
    std::vector<float> getCurrentParameterValues() const;
    void applyParameterSet(const std::vector<float>& values);
};

} // namespace nlm
