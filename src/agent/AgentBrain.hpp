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
    
    // Advanced agent control
    void setMaxExploration(float maxExploration) { maxExploration_ = maxExploration; }
    float getMaxExploration() const { return maxExploration_; }
    void setTemperature(float temperature) { temperature_ = temperature; }
    float getTemperature() const { return temperature_; }
    void setEpsilon(float epsilon) { epsilon_ = epsilon; }
    float getEpsilon() const { return epsilon_; }
    
    // State snapshot and comparison
    std::string createSnapshot() const;
    float compareSnapshots(const std::string& snapshot1, const std::string& snapshot2) const;
    void applySnapshot(const std::string& snapshot);
    
    // Advanced neuromodulation
    void setDopamineLevel(float level) { dopamineLevel_ = level; }
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriositySignal() const;
    float getNoveltySignal() const;
    float getPredictionErrorSignal() const;
    
    // Development control
    void setDevelopmentRate(float rate) { developmentRate_ = rate; }
    float getDevelopmentRate() const { return developmentRate_; }
    void setCriticalPeriodEnd(double time) { criticalPeriodEnd_ = time; }
    double getCriticalPeriodEnd() const { return criticalPeriodEnd_; }
    
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
    
    // Advanced control parameters
    float maxExploration_;
    float temperature_;
    float epsilon_;
    float developmentRate_;
    double criticalPeriodEnd_;
    float curiositySignal_;
    float noveltySignal_;
    float predictionErrorSignal_;
    size_t snapshotCounter_;

    // Move operations
    AgentBrain(AgentBrain&& other) noexcept;
    AgentBrain& operator=(AgentBrain&& other) noexcept;
};

} // namespace nlm

} // namespace nlm
