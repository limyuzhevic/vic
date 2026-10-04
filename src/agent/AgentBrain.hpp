#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "MotorNeuronManager.hpp"
#include "SensoryNeuronManager.hpp"
#include "SensoryProcessor.hpp"
#include "MotorCommandDecoder.hpp"
#include "NeuromodulationManager.hpp"
#include "DevelopmentManager.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Uses manager classes to separate concerns
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
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const { return neuromodulationManager_ && neuromodulationManager_->isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return developmentManager_ && developmentManager_->isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const { return developmentManager_ && developmentManager_->isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const { return sensoryProcessor_ && sensoryProcessor_->isCuriosityEnabled(); }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Manager instances for separation of concerns
    std::unique_ptr<MotorNeuronManager> motorNeuronManager_;
    std::unique_ptr<SensoryNeuronManager> sensoryNeuronManager_;
    std::unique_ptr<SensoryProcessor> sensoryProcessor_;
    std::unique_ptr<MotorCommandDecoder> motorCommandDecoder_;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager_;
    std::unique_ptr<DevelopmentManager> developmentManager_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
};

} // namespace nlm
