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
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    [[nodiscard]] size_t getSensoryInputSize() const noexcept;
    
    // Get motor output size expected
    [[nodiscard]] size_t getMotorOutputSize() const noexcept;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    [[nodiscard]] MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    [[nodiscard]] float getNeuromodulationLevel() const noexcept;
    
    // Get curiosity level
    [[nodiscard]] float getCuriosityLevel() const noexcept;
    
    // Get novelty level
    [[nodiscard]] float getNoveltyLevel() const noexcept;
    
    // Get prediction error
    [[nodiscard]] float getPredictionError() const noexcept;
    
    // Reset agent for new episode
    void reset() noexcept;
    
    // Get brain pointer
    [[nodiscard]] Brain* getBrain() noexcept { return brain_.get(); }
    
    // Configuration methods
    void enableRewardModulation(bool enable) noexcept { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) noexcept { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) noexcept { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) noexcept { curiosityEnabled_ = enable; }
    
    // Configuration getters
    [[nodiscard]] bool isRewardModulationEnabled() const noexcept { return rewardModulationEnabled_; }
    [[nodiscard]] bool isStructuralPlasticityEnabled() const noexcept { return structuralPlasticityEnabled_; }
    [[nodiscard]] bool isDevelopmentEnabled() const noexcept { return developmentEnabled_; }
    [[nodiscard]] bool isCuriosityEnabled() const noexcept { return curiosityEnabled_; }
    
    // Statistical getters
    [[nodiscard]] float getExpectedReward() const noexcept { return expectedReward_; }
    [[nodiscard]] float getPlasticityModifier() const noexcept { return plasticityModifier_; }
    [[nodiscard]] double getDevelopmentalAge() const noexcept { return developmentalAge_; }
    
    // Utility methods
    [[nodiscard]] size_t getVisionNeuronsCount() const noexcept { return sensoryVision_.size(); }
    [[nodiscard]] size_t getTouchNeuronsCount() const noexcept { return sensoryTouch_.size(); }
    [[nodiscard]] size_t getInternalNeuronsCount() const noexcept { return sensoryInternal_.size(); }
    [[nodiscard]] size_t getProprioceptiveNeuronsCount() const noexcept { return sensoryProprioception_.size(); }
    [[nodiscard]] size_t getMotorNeuronsCount() const noexcept { return getTotalMotorNeurons(); }
    
    // Advanced configuration
    void setSensoryNoveltyDecay(float decay) noexcept { sensoryNoveltyDecay_ = decay; }
    void setDevelopmentEnabled(bool enabled) noexcept { developmentEnabled_ = enabled; }
    
    // State update
    void updateNeuromodulation(double timestep);
    
    // Validate agent brain is properly initialized
    [[nodiscard]] bool isValid() const noexcept;
    
private:
    // Helper methods
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    
    // Motor decoding: convert neural activity to motor command
    [[nodiscard]] MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    [[nodiscard]] MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Statistics helpers
    [[nodiscard]] size_t getTotalMotorNeurons() const noexcept;
    [[nodiscard]] size_t getTotalSensoryNeurons() const noexcept;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (organized by action type)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (organized by modality)
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
