#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration structure for agent behavior
    struct AgentConfig {
        // Sensory processing parameters
        float visionScaleFactor = 5.0f;
        float touchScaleFactor = 8.0f;
        float internalScaleFactor = 5.0f;
        float proprioceptionScaleFactor = 3.0f;
        
        // Novelty detection
        float noveltyDecayRate = 0.99f;
        
        // Motor decoding thresholds
        float minActivityThreshold = 0.5f;
        
        // Curiosity and exploration
        float curiosityThreshold = 0.3f;
        float explorationProbability = 0.3f;
        
        // Development
        float developmentStartAge = 60.0f;
        float developmentCriticalStart = 300.0f;
        float developmentMaturationStart = 900.0f;
        
        // Neuromodulation
        float dopamineClampMin = -1.0f;
        float dopamineClampMax = 1.0f;
        
        // Constructor with sensible defaults
        AgentConfig() = default;
    };

public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config = AgentConfig());
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
    
    // Configuration management
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Utility methods for better testing and debugging
    std::string getDebugInfo() const;
    void logState() const;
    
    // Configuration flags
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Core components
    std::shared_ptr<Brain> brain_;
    AgentConfig config_;
    
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
    
    // Helper methods
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    
    float calculateMotorGroupActivity(const std::vector<Neuron*>& neurons) const;
    float computeSensoryNovelty(const std::vector<float>& vision) const;
    
    // Initialize default configuration
    void initializeDefaultConfig();
};

} // namespace nlm
