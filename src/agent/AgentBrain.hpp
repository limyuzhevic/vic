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
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Enable/disable curiosity-based exploration
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get detailed brain statistics for debugging
    std::string getDebugInfo() const;
    
    // Advanced control methods
    void setExplorationWeight(float weight) { explorationWeight_ = std::clamp(weight, 0.0f, 1.0f); }
    void setLearningRate(float rate) { learningRate_ = std::clamp(rate, 0.001f, 1.0f); }
    void setNoveltyThreshold(float threshold) { noveltyThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
    // Memory management for advanced users
    void enableMemoryPersistence(bool enable) { memoryPersistenceEnabled_ = enable; }
    void setMemoryRetention(float retention) { memoryRetention_ = std::clamp(retention, 0.0f, 1.0f); }
    
    // Performance monitoring
    float getAverageActivityLevel() const;
    float getExplorationRatio() const;
    size_t getTotalMemoryUsage() const;
    
    // Advanced neuromodulation controls
    void setDopamineGain(float gain) { dopamineGain_ = std::clamp(gain, 0.0f, 10.0f); }
    void setCuriosityGain(float gain) { curiosityGain_ = std::clamp(gain, 0.0f, 10.0f); }
    void setNoveltyDecay(float decay) { noveltyDecay_ = std::clamp(decay, 0.1f, 1.0f); }
    
    // Development control for advanced users
    void forceDevelopmentStage(DevelopmentalStage stage) { 
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
    }
    void setDevelopmentSpeed(float speed) { developmentSpeed_ = std::clamp(speed, 0.1f, 10.0f); }
    
    // Sensory/motor tuning
    void setSensoryGain(float gain) { sensoryGain_ = std::clamp(gain, 0.1f, 10.0f); }
    void setMotorThreshold(float threshold) { motorThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
    // Debug and visualization controls
    void enableDebugVisualization(bool enable) { debugVisualizationEnabled_ = enable; }
    void setVisualizationScale(float scale) { visualizationScale_ = std::clamp(scale, 0.1f, 5.0f); }
    
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
