#include "AgentBrainConfig.hpp"
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>

namespace nlm {
namespace config {

void AgentBrainConfig::initializeConfig(AgentBrain& agentBrain, const SimpleWorld& world) {
    agentBrain.previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    agentBrain.developmentalAge_ = 0.0;
    agentBrain.plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(agentBrain.sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(agentBrain.sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(agentBrain.sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrainConfig::enableRewardModulation(AgentBrain& agentBrain, bool enable) {
    agentBrain.rewardModulationEnabled_ = enable;
}

void AgentBrainConfig::enableStructuralPlasticity(AgentBrain& agentBrain, bool enable) {
    agentBrain.structuralPlasticityEnabled_ = enable;
}

void AgentBrainConfig::enableDevelopment(AgentBrain& agentBrain, bool enable) {
    agentBrain.developmentEnabled_ = enable;
}

void AgentBrainConfig::enableCuriosity(AgentBrain& agentBrain, bool enable) {
    agentBrain.curiosityEnabled_ = enable;
}

bool AgentBrainConfig::isRewardModulationEnabled(const AgentBrain& agentBrain) {
    return agentBrain.rewardModulationEnabled_;
}

bool AgentBrainConfig::isStructuralPlasticityEnabled(const AgentBrain& agentBrain) {
    return agentBrain.structuralPlasticityEnabled_;
}

bool AgentBrainConfig::isDevelopmentEnabled(const AgentBrain& agentBrain) {
    return agentBrain.developmentEnabled_;
}

bool AgentBrainConfig::isCuriosityEnabled(const AgentBrain& agentBrain) {
    return agentBrain.curiosityEnabled_;
}

} // namespace config
} // namespace nlm
