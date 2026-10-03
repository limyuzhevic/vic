#ifndef NLM_AGENT_BRAIN_NEUROMODULATION_HPP
#define NLM_AGENT_BRAIN_NEUROMODULATION_HPP

#include "../brain/Brain.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace nlm {

class AgentBrainNeuromodulation {
public:
    AgentBrainNeuromodulation(std::shared_ptr<AgentBrainConfig> config = nullptr);
    ~AgentBrainNeuromodulation() = default;
    
    // Apply reward-based neuromodulation - returns error if brain or reward values invalid
    AgentBrainResult<void> applyReward(const std::shared_ptr<Brain>& brain, float reward, float predictedReward, float plasticityModifier);
    
    // Update development system - returns error if brain or timestep invalid
    AgentBrainResult<void> updateDevelopment(const std::shared_ptr<Brain>& brain, double timestep, double& developmentalAge, float& plasticityModifier);
    
    // Get developmental stage - returns error if brain is null
    AgentBrainResult<DevelopmentalStage> getDevelopmentalStage(const std::shared_ptr<Brain>& brain);
    
    // Get neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    // Reset neuromodulation state
    AgentBrainResult<void> reset();
    
    // Update neuromodulation components
    AgentBrainResult<void> updateCuriosity(float curiosityLevel);
    AgentBrainResult<void> updateNovelty(float noveltyLevel);
    AgentBrainResult<void> updatePredictionError(float reward, float predictedReward);
    
private:
    // Configuration
    std::shared_ptr<AgentBrainConfig> config_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    
    // Neuromodulation state (dopamine, reward, prediction error, curiosity, novelty)
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_NEUROMODULATION_HPP