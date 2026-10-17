#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Neuron.hpp"
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
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand getMotorCommandsFromNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectActionWithCuriosity(MotorCommand defaultCmd);
    
    // Helper method to calculate activity in a neuron group
    float calcNeuronGroupActivity(const std::vector<std::unique_ptr<Neuron>>& neurons) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - using unique_ptr for proper encapsulation
    std::vector<std::unique_ptr<Neuron>> motorForward_;
    std::vector<std::unique_ptr<Neuron>> motorBackward_;
    std::vector<std::unique_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::unique_ptr<Neuron>> motorTurnRight_;
    std::vector<std::unique_ptr<Neuron>> motorLookLeft_;
    std::vector<std::unique_ptr<Neuron>> motorLookRight_;
    std::vector<std::unique_ptr<Neuron>> motorInteract_;
    std::vector<std::unique_ptr<Neuron>> motorWait_;
    
    // Sensory neuron groups - using unique_ptr for proper encapsulation
    std::vector<std::unique_ptr<Neuron>> sensoryVision_;
    std::vector<std::unique_ptr<Neuron>> sensoryTouch_;
    std::vector<std::unique_ptr<Neuron>> sensoryInternal_;
    std::vector<std::unique_ptr<Neuron>> sensoryProprioception_;
    
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