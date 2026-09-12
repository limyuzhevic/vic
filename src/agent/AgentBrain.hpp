#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class SensoryProcessor;
class MotorDecoder;
class NeuromodulationController;

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
    void enableRewardModulation(bool enable) { rewardModulationController_->enable(enable); }
    void enableStructuralPlasticity(bool enable) { rewardModulationController_->enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) { rewardModulationController_->enableDevelopment(enable); }
    void enableCuriosity(bool enable) { motorDecoder_->enableCuriosity(enable); }
    
    bool isRewardModulationEnabled() const { return rewardModulationController_->isEnabled(); }
    bool isStructuralPlasticityEnabled() const { return rewardModulationController_->isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return rewardModulationController_->isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return motorDecoder_->isCuriosityEnabled(); }
    
private:
    // Internal components
    std::shared_ptr<SensoryProcessor> sensoryProcessor_;
    std::shared_ptr<MotorDecoder> motorDecoder_;
    std::shared_ptr<NeuromodulationController> rewardModulationController_;
    
    std::shared_ptr<Brain> brain_;
    
    // Previous sensory state for novelty detection (kept for backward compatibility)
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
