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
    // Legacy constructor for backward compatibility
    AgentBrain(std::shared_ptr<Brain> brain);
    
    // Modern constructor with dependency injection
    AgentBrain(std::shared_ptr<Brain> brain,
               std::unique_ptr<class MotorDecodingStrategy> motorStrategy = nullptr,
               std::unique_ptr<class CuriosityHandler> curiosityHandler = nullptr,
               std::shared_ptr<class Config> config = nullptr);
    
    ~AgentBrain();
    
    // Legacy method for backward compatibility
    void initialize(const SimpleWorld& world);
    
    // Modern configuration-driven method
    void configureFromConfig(const class Config& config);
    
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
    
    // Check if agent is in valid state
    bool isValid() const;
    
    // Get brain pointer (legacy)
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
    
    // Modern APIs
    void setMotorDecodingStrategy(std::unique_ptr<class MotorDecodingStrategy> strategy);
    void setCuriosityHandler(std::unique_ptr<class CuriosityHandler> handler);
    
    const class MotorDecodingStrategy* getMotorDecodingStrategy() const { return motorStrategy_.get(); }
    const class CuriosityHandler* getCuriosityHandler() const { return curiosityHandler_.get(); }
    
private:
    // Legacy implementation (deprecated)
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration (legacy)
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Dependency injection fields
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<class MotorDecodingStrategy> motorStrategy_;
    std::unique_ptr<class CuriosityHandler> curiosityHandler_;
    std::shared_ptr<class Config> config_;
    
    // Neuron group management
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
    
    // State
    std::vector<float> previousVision_;
    float noveltyLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    size_t sensoryInputSize_;
    size_t motorOutputSize_;
    
    // Legacy neuron groups for backward compatibility
    std::vector<Neuron*> legacyMotorForward_;
    std::vector<Neuron*> legacyMotorBackward_;
    std::vector<Neuron*> legacyMotorTurnLeft_;
    std::vector<Neuron*> legacyMotorTurnRight_;
    std::vector<Neuron*> legacyMotorInteract_;
    std::vector<Neuron*> legacyMotorWait_;
    
    std::vector<Neuron*> legacySensoryVision_;
    std::vector<Neuron*> legacySensoryTouch_;
    std::vector<Neuron*> legacySensoryInternal_;
    std::vector<Neuron*> legacySensoryProprioception_;
    
    // Helper methods
    void initializeNeuronGroups();
    void validatePercept(const SensoryPercept& percept) const;
    
    template<typename T>
    bool safeAccess(const std::vector<T*>& container, size_t index, T*& out);
    
    // Legacy initialization
    void legacyInitializeNeuronGroups();
    void legacyInitializeSensoryGroups();
    void legacyInitializeMotorGroups();
};

} // namespace nlm
