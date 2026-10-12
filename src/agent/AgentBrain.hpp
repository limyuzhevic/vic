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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration for experienced users
    void setExplorationRate(float rate) { explorationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setAttentionWeight(float weight) { attentionWeight_ = std::clamp(weight, 0.0f, 2.0f); }
    float getExplorationRate() const { return explorationRate_; }
    float getAttentionWeight() const { return attentionWeight_; }
    
    // New advanced motor control methods
    void setMotorBias(const std::string& action, float bias) { motorBiases_[action] = bias; }
    float getMotorBias(const std::string& action) const { 
        auto it = motorBiases_.find(action);
        return it != motorBiases_.end() ? it->second : 0.0f;
    }
    
    // Enhanced sensory processing methods
    void setSensoryGain(const std::string& modality, float gain) { 
        sensoryGains_[modality] = gain;
    }
    float getSensoryGain(const std::string& modality) const {
        auto it = sensoryGains_.find(modality);
        return it != sensoryGains_.end() ? it->second : 1.0f;
    }
    
private:
    // Helper methods
    void initializeNeuronGroups();
    void organizeMotorNeurons(const std::shared_ptr<NeuralPopulation>& population, 
                             const std::vector<Neuron*>& neurons);
    void organizeSensoryNeurons(const std::shared_ptr<NeuralPopulation>& population,
                               const std::vector<Neuron*>& neurons);
    void setupAdvancedFeatures();
    void clearNeuronGroups();
    
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
    
    // Advanced configuration parameters
    float explorationRate_;
    float attentionWeight_;
    float curiosityThreshold_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // New features for experienced users
    std::unordered_map<std::string, float> motorBiases_;      // Action biases
    std::unordered_map<std::string, float> sensoryGains_;     // Sensory gains
    
    // Performance optimization
    mutable float cachedFiringRate_;
    mutable bool firingRateValid_;
    
    // Statistics and metrics
    size_t totalSteps_;
    float cumulativeReward_;
};

} // namespace nlm
