#ifndef NLM_AGENT_BRAIN_HPP
#define NLM_AGENT_BRAIN_HPP

#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../motor/Action.hpp"
#include "../agent/AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <memory>
#include <stdexcept>
#include <sstream>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Refactored facade that delegates to modular components
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world - returns error result for failure cases
    AgentBrainResult<void> initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain - returns error if brain is null
    AgentBrainResult<size_t> getSensoryInputSize() const;
    
    // Get motor output size expected - returns error if brain is null
    AgentBrainResult<size_t> getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    AgentBrainResult<void> processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    AgentBrainResult<MotorCommand> decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    AgentBrainResult<void> applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    AgentBrainResult<void> updateDevelopment(double timestep);
    
    // Get current developmental stage - returns error if brain is null
    AgentBrainResult<DevelopmentalStage> getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    AgentBrainResult<void> reset();
    
    // Get brain pointer - returns error if brain is null
    AgentBrainResult<Brain*> getBrain();
    
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
    std::shared_ptr<AgentBrainConfig> config_;
    
    std::shared_ptr<Brain> brain_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Error handling state
    bool errorRecoveryMode_;
    
    // Component classes for modularity
    class AgentBrainMotor;
    class AgentBrainSensory;
    class AgentBrainNeuromodulation;
    
    std::unique_ptr<AgentBrainMotor> motorComponent_;
    std::unique_ptr<AgentBrainSensory> sensoryComponent_;
    std::unique_ptr<AgentBrainNeuromodulation> neuromodulationComponent_;
    
    // Helper methods for error handling
    bool validateNotNull(const void* ptr, const std::string& context) const;
    bool validateIndex(size_t idx, size_t maxSize, const std::string& context) const;
    bool validateRange(double value, double min, double max, const std::string& context) const;
    bool validateRange(float value, float min, float max, const std::string& context) const;
    bool validateBrainState(const std::string& operation) const;
    void setErrorRecoveryMode(bool enable) { errorRecoveryMode_ = enable; }
    bool isErrorRecoveryMode() const { return errorRecoveryMode_; }
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_HPP