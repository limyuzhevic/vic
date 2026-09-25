#include "AgentStateManager.hpp"

namespace nlm {

AgentStateManager::AgentStateManager()
    : dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f),
      predictionError_(0.0f), expectedReward_(0.0f), developmentalAge_(0.0),
      plasticityModifier_(1.0f), rewardModulationEnabled_(true),
      structuralPlasticityEnabled_(true), developmentEnabled_(true),
      curiosityEnabled_(true), sensoryNoveltyDecay_(0.99f) {}

AgentStateManager::~AgentStateManager() {}

} // namespace nlm