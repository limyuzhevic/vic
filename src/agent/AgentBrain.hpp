#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "SensoryTransduction.hpp"
#include "MotorDecoder.hpp"
#include "Neuromodulator.hpp"
#include "Developer.hpp"
#include "ExplorationAgent.hpp"
#include "../core/Config/Config.hpp"
#include <memory>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Uses composition to delegate concerns to specialized modules
class AgentBrain {
public:
    // New constructor with configuration for modular components
    AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Legacy constructor for backward compatibility
    // Creates a default configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    // Agent body reference
    std::shared_ptr<AgentBody> body_;
    
    // Modular components
    std::unique_ptr<SensoryTransduction> sensoryTransduction_;
    std::unique_ptr<MotorDecoder> motorDecoder_;
    std::unique_ptr<Neuromodulator> neuromodulator_;
    std::unique_ptr<Developer> developer_;
    std::unique_ptr<ExplorationAgent> explorationAgent_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
};

} // namespace nlm

