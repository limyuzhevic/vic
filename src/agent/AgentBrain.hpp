#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Distribution configuration structure
struct NeuronDistributionConfig {
    float motorForwardRatio_ = 0.16f;    // ~1/6
    float motorBackwardRatio_ = 0.16f;   // ~1/6
    float motorTurnLeftRatio_ = 0.16f;   // ~1/6
    float motorTurnRightRatio_ = 0.16f;  // ~1/6
    float motorInteractRatio_ = 0.16f;   // ~1/6
    float motorWaitRatio_ = 0.1f;       // 1/10
    
    float sensoryVisionRatio_ = 0.4f;    // 40%
    float sensoryTouchRatio_ = 0.25f;    // 25%
    float sensoryInternalRatio_ = 0.2f;  // 20%
    float sensoryProprioceptionRatio_ = 0.15f; // 15%
    
    float motorActivityThreshold_ = 0.5f;
    float curiosityExplorationThreshold_ = 0.5f;
    float curiosityExplorationChanceMultiplier_ = 0.3f;
};

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
        
        // Motor neuron distribution configuration
        void setMotorDistributionRatios(
            float forwardRatio, float backwardRatio, float leftRatio, float rightRatio,
            float interactRatio, float waitRatio);
        void setSensoryDistributionRatios(
            float visionRatio, float touchRatio, float internalRatio, float proprioceptionRatio);
        
        // Motor decoding configuration
        void setMotorActivityThreshold(float threshold);
        void setCuriosityExplorationThreshold(float threshold);
        void setCuriosityExplorationChanceMultiplier(float multiplier);
        
        // Feature enable/disable
        void enableRandomExploration(bool enable);
        void enableMotorDecoding(bool enable);
        
        // Parameter validation
        void validateParameters() const;
        
private:
    // Distribution and configuration
    NeuronDistributionConfig distributionConfig_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Distribution methods
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    void distributeMotorNeuronsByCounts(size_t forwardCount, size_t backwardCount,
                                        size_t leftCount, size_t rightCount,
                                        size_t interactCount, size_t waitCount);
    void distributeSensoryNeuronsByCounts(size_t visionCount, size_t touchCount,
                                          size_t internalCount, size_t proprioceptionCount);
    
    // Helper methods
    float calculateNeuronActivity(const std::vector<Neuron*>& neurons) const;
    
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
