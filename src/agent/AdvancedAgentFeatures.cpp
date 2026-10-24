#include "AdvancedAgentFeatures.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AdvancedAgentFeatures::AdvancedAgentFeatures(std::shared_ptr<AgentBrain> agentBrain)
    : agentBrain_(agentBrain), behavioralState_(BehavioralState::IDLE),
      curiosityWeight_(1.0f), learningRate_(0.01f), explorationBonus_(0.1f),
      lastUpdateTime_(0.0) {}

void AdvancedAgentFeatures::EnhancedCuriosity::update(double timestep, 
    const MultiModalNovelty& novelty, const std::vector<float>& predictionErrors) {
    
    // Update exploration pressure based on knowledge and novelty
    knowledgeValue_ += (0.5f - std::abs(knowledgeValue_)) * 0.01f * timestep;
    
    // Novelty increases exploration pressure
    float noveltyImpact = novelty.getCombinedNovelty() * 2.0f;
    explorationPressure_ = std::min(1.0f, explorationPressure_ + noveltyImpact * 0.05f * timestep);
    
    // Prediction errors modulate exploration
    if (!predictionErrors.empty()) {
        float avgPredictionError = std::accumulate(predictionErrors.begin(), 
                                                  predictionErrors.end(), 0.0f) / predictionErrors.size();
        explorationPressure_ = std::min(1.0f, explorationPressure_ + std::abs(avgPredictionError) * 0.1f * timestep);
    }
    
    // Intrinsic motivation based on curiosity and knowledge acquisition
    float curiosityDrive = novelty.getCombinedNovelty() * 0.7f + knowledgeValue_ * 0.3f;
    intrinsicMotivation_ = std::min(1.0f, intrinsicMotivation_ + curiosityDrive * 0.02f * timestep);
    
    // Extrinsic motivation based on rewards
    extrinsicMotivation_ = std::min(1.0f, extrinsicMotivation_ + 0.01f * timestep);
    
    // Decay over time
    explorationPressure_ = std::max(0.0f, explorationPressure_ - 0.005f * timestep);
    intrinsicMotivation_ = std::max(0.0f, intrinsicMotivation_ - 0.002f * timestep);
    extrinsicMotivation_ = std::max(0.0f, extrinsicMotivation_ - 0.001f * timestep);
}

MotorCommand AdvancedAgentFeatures::EnhancedCuriosity::selectActionWithCuriosity(
    const MotorCommand& defaultAction,
    const std::vector<MotorCommand>& availableActions) {
    
    if (explorationPressure_ < 0.3f) {
        // Exploitation - prefer the default action
        return defaultAction;
    }
    
    if (explorationPressure_ > 0.7f || explorationQueue_.empty()) {
        // High curiosity or no queue - explore with random action
        if (availableActions.empty()) return defaultAction;
        
        int index = rand() % availableActions.size();
        explorationQueue_.push(availableActions[index]);
    }
    
    MotorCommand queueAction = explorationQueue_.front();
    explorationQueue_.pop();
    
    // Occasionally fall back to default if queue action is not available
    bool actionAvailable = std::find(availableActions.begin(), availableActions.end(), queueAction) != availableActions.end();
    if (!actionAvailable) {
        return defaultAction;
    }
    
    return queueAction;
}

void AdvancedAgentFeatures::MemorySystem::storeExperience(const ExperienceRecord& experience) {
    experiences_.push_back(experience);
    
    // Update temporal index
    std::string timeKey = "t_" + std::to_string(experience.timestamp);
    temporalIndex_[timeKey].push_back(experiences_.size() - 1);
    
    // Update content index
    std::string contextKey = "c_" + experience.context;
    contentIndex_[contextKey].push_back(experiences_.size() - 1);
    
    // Trim if exceeding capacity
    if (experiences_.size() > experienceCapacity_) {
        // Remove oldest experiences (simple approach)
        size_t excess = experiences_.size() - experienceCapacity_;
        experiences_.erase(experiences_.begin(), experiences_.begin() + excess);
        
        // Rebuild indices
        temporalIndex_.clear();
        contentIndex_.clear();
        for (size_t i = 0; i < experiences_.size(); ++i) {
            std::string timeKey = "t_" + std::to_string(experiences_[i].timestamp);
            temporalIndex_[timeKey].push_back(i);
            std::string contextKey = "c_" + experiences_[i].context;
            contentIndex_[contextKey].push_back(i);
        }
    }
}

void AdvancedAgentFeatures::MemorySystem::consolidateMemories(double currentTime) {
    if (experiences_.empty()) return;
    
    // Find experiences that need consolidation (older than consolidation interval)
    auto cutoffTime = currentTime - consolidationInterval_;
    std::vector<size_t> toConsolidate;
    
    for (size_t i = 0; i < experiences_.size(); ++i) {
        if (experiences_[i].timestamp < cutoffTime) {
            toConsolidate.push_back(i);
        }
    }
    
    // Consolidate important memories (high novelty or reward)
    for (size_t i : toConsolidate) {
        float strength = getMemoryStrength(experiences_[i]);
        if (strength > 0.5f) {
            // Boost memory strength
            if (i < experiences_.size()) {
                // In a real implementation, this would update neural connections
                // For now, we just note the consolidation
                NLM_LOG_INFO("Consolidating experience at time " + std::to_string(experiences_[i].timestamp) +
                           " with strength " + std::to_string(strength));
            }
        }
    }
}

void AdvancedAgentFeatures::MemorySystem::replayImportantMemories() {
    if (experiences_.empty()) return;
    
    // Find high-strength memories for replay
    std::vector<size_t> highStrengthMemories;
    for (size_t i = 0; i < experiences_.size(); ++i) {
        if (getMemoryStrength(experiences_[i]) > replayStrength_) {
            highStrengthMemories.push_back(i);
        }
    }
    
    NLM_LOG_INFO("Replaying " + std::to_string(highStrengthMemories.size()) + 
               " important memories for consolidation");
    
    // In a real implementation, this would replay experiences to strengthen connections
}

const std::vector<AdvancedAgentFeatures::ExperienceRecord>& 
AdvancedAgentFeatures::MemorySystem::getRecentExperiences(size_t count) const {
    if (count > experiences_.size()) {
        return experiences_;
    }
    
    static std::vector<ExperienceRecord> recent;
    recent.clear();
    size_t startIdx = experiences_.size() - count;
    recent.insert(recent.end(), experiences_.begin() + startIdx, experiences_.end());
    return recent;
}

float AdvancedAgentFeatures::MemorySystem::getMemoryStrength(const ExperienceRecord& experience) const {
    float strength = 0.0f;
    
    // Base strength from novelty
    strength += experience.noveltyLevel * 0.3f;
    
    // Add reward magnitude
    strength += std::abs(experience.reward) * 0.4f;
    
    // Add prediction error
    strength += std::abs(experience.predictionError) * 0.2f;
    
    // Add temporal recency (more recent = stronger)
    float timeDecay = std::exp(-0.01f * experience.timestamp);
    strength *= timeDecay;
    
    return std::min(1.0f, strength);
}

AdvancedAgentFeatures::AdvancedMotorControl::AdvancedMotorControl(std::shared_ptr<AgentBrain> agentBrain)
    : agentBrain_(agentBrain), energyBudget_(100.0f), energyDecayRate_(0.01f) {}

void AdvancedAgentFeatures::AdvancedMotorControl::learnSkill(const Skill& skill) {
    skills_[skill.name] = skill;
    NLM_LOG_INFO("Learning new skill: " + skill.name + " (energy cost: " + std::to_string(skill.energyCost) + ")");
}

bool AdvancedAgentFeatures::AdvancedMotorControl::executeSkill(const std::string& skillName, float energyBudget) {
    auto it = skills_.find(skillName);
    if (it == skills_.end()) {
        NLM_LOG_INFO("Skill not found: " + skillName);
        return false;
    }
    
    Skill& skill = it->second;
    
    if (energyBudget < skill.energyCost) {
        NLM_LOG_INFO("Not enough energy for skill " + skillName + ": need " + std::to_string(skill.energyCost) +
                    ", have " + std::to_string(energyBudget));
        return false;
    }
    
    // Execute skill
    NLM_LOG_INFO("Executing skill: " + skillName);
    skill.useCount++;
    skill.lastUsedTime = agentBrain_->getBrain()->getSimulationTime();
    
    // Improve skill proficiency based on success
    skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
    skill.successRate = (skill.successRate * skill.useCount + 1.0f) / (skill.useCount + 1);
    
    return true;
}

float AdvancedAgentFeatures::AdvancedMotorControl::evaluateActionOutcome(const MotorCommand& action,
    const ActionResult& result) {
    // Base action value from action history
    float baseValue = 0.0f;
    auto it = actionValues_.find(action);
    if (it != actionValues_.end()) {
        baseValue = it->second;
    }
    
    // Adjust based on result
    float resultValue = result.reward * 2.0f;  // Scale reward
    if (result.success) {
        resultValue += 1.0f;  // Bonus for successful actions
    }
    
    // Update action value (simple reinforcement learning)
    actionValues_[action] = baseValue * 0.9f + resultValue * 0.1f;
    
    return baseValue + resultValue;
}

std::vector<MotorCommand> AdvancedAgentFeatures::AdvancedMotorControl::getAvailableActions() const {
    std::vector<MotorCommand> actions;
    
    // Add basic motor commands
    actions.push_back(MotorCommand::MoveForward);
    actions.push_back(MotorCommand::MoveBackward);
    actions.push_back(MotorCommand::TurnLeft);
    actions.push_back(MotorCommand::TurnRight);
    actions.push_back(MotorCommand::LookLeft);
    actions.push_back(MotorCommand::LookRight);
    actions.push_back(MotorCommand::Interact);
    actions.push_back(MotorCommand::Wait);
    
    // Add skills that can be executed
    for (const auto& pair : skills_) {
        Skill skill = pair.second;
        if (skill.proficiency > 0.5f && energyBudget_ >= skill.energyCost) {
            // Map skills to motor commands (simplified)
            if (skill.name.find("move") != std::string::npos) {
                actions.push_back(MotorCommand::MoveForward);
            } else if (skill.name.find("turn") != std::string::npos) {
                actions.push_back(MotorCommand::TurnLeft);
            } else if (skill.name.find("look") != std::string::npos) {
                actions.push_back(MotorCommand::LookLeft);
            }
        }
    }
    
    // Remove duplicates
    std::sort(actions.begin(), actions.end());
    actions.erase(std::unique(actions.begin(), actions.end()), actions.end());
    
    return actions;
}

MotorCommand AdvancedAgentFeatures::AdvancedMotorControl::selectActionWithSkills(
    const MotorCommand& defaultAction) {
    
    std::vector<MotorCommand> availableActions = getAvailableActions();
    if (availableActions.empty()) {
        return defaultAction;
    }
    
    // Choose action with highest value
    MotorCommand bestAction = defaultAction;
    float bestValue = evaluateActionOutcome(defaultAction, ActionResult(0.0f, false, "default"));
    
    for (const MotorCommand& action : availableActions) {
        if (action == defaultAction) continue;  // Already evaluated
        
        float actionValue = evaluateActionOutcome(action, ActionResult(0.0f, false, "skill"));
        if (actionValue > bestValue) {
            bestValue = actionValue;
            bestAction = action;
        }
    }
    
    return bestAction;
}

AdvancedAgentFeatures::PredictivePlanning::PredictivePlanning(std::shared_ptr<AgentBrain> agentBrain)
    : agentBrain_(agentBrain), planningHorizon_(5), explorationRate_(0.3f) {}

void AdvancedAgentFeatures::PredictivePlanning::updateForwardModel(
    const std::vector<float>& state,
    const MotorCommand& action,
    float outcome) {
    
    // Predict expected outcome for this action
    float expectedOutcome = predictActionOutcome(action);
    
    // Update prediction confidence
    size_t actionIndex = static_cast<size_t>(action);
    if (actionIndex < predictionConfidence.size()) {
        predictionConfidence[actionIndex] = (predictionConfidence[actionIndex] * 0.9f + 
                                           std::abs(expectedOutcome - outcome) * 0.1f);
    }
    
    // Update expected outcomes
    expectedOutcomes[actionIndex] = outcome;
    lastPredictionTime = agentBrain_->getBrain()->getSimulationTime();
}

std::vector<float> AdvancedAgentFeatures::PredictivePlanning::predictFutureStates(
    const std::vector<float>& currentState,
    size_t steps) const {
    
    std::vector<float> predictedState = currentState;
    std::vector<float> futurePredictions;
    
    for (size_t i = 0; i < steps; ++i) {
        // Simple forward prediction (in real implementation, would use actual forward model)
        for (size_t j = 0; j < predictedState.size(); ++j) {
            predictedState[j] *= 0.95f + 0.05f * (rand() % 100) / 100.0f;
        }
        
        futurePredictions.push_back(predictedState[0]); // Just track first state element
    }
    
    return futurePredictions;
}

float AdvancedAgentFeatures::PredictivePlanning::evaluateActionPolicy(
    const MotorCommand& action,
    const std::vector<float>& currentState) const {
    
    // Use forward model to predict outcome
    float predictedOutcome = predictActionOutcome(action);
    
    // Get actual expected outcome
    size_t actionIndex = static_cast<size_t>(action);
    float expectedOutcome = 0.0f;
    if (actionIndex < expectedOutcomes.size()) {
        expectedOutcome = expectedOutcomes[actionIndex];
    }
    
    // Calculate prediction error
    float predictionError = std::abs(predictedOutcome - expectedOutcome);
    
    // Value is negative of prediction error (lower error = higher value)
    return 1.0f / (1.0f + predictionError);
}

MotorCommand AdvancedAgentFeatures::PredictivePlanning::selectActionWithPlanning(
    const std::vector<MotorCommand>& availableActions,
    const std::vector<float>& currentState) {
    
    if (availableActions.empty()) {
        return MotorCommand::Wait;
    }
    
    // Evaluate all actions using value function
    MotorCommand bestAction = availableActions[0];
    float bestValue = -1000.0f;
    
    for (const MotorCommand& action : availableActions) {
        float actionValue = evaluateActionPolicy(action, currentState);
        
        // Add exploration bonus for some actions
        if (rand() % 100 < explorationRate_ * 100) {
            actionValue += explorationBonus_;
        }
        
        if (actionValue > bestValue) {
            bestValue = actionValue;
            bestAction = action;
        }
    }
    
    return bestAction;
}

// AdvancedAgentFeatures implementation functions

AdvancedAgentFeatures::MultiModalNovelty AdvancedAgentFeatures::getEnhancedNovelty(
    const SensoryPercept& percept,
    const std::vector<float>& predictionErrors,
    double currentTime) const {
    
    MultiModalNovelty novelty;
    
    // Sensory novelty
    if (!percept.getVision().empty()) {
        std::vector<float> previousVision = agentBrain_->getPreviousVision();
        if (previousVision.size() == percept.getVision().size()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < percept.getVision().size(); ++i) {
                totalDiff += std::abs(percept.getVision()[i] - previousVision[i]);
            }
            novelty.sensoryNovelty = totalDiff / percept.getVision().size();
        }
    }
    
    // Cognitive novelty based on prediction errors
    if (!predictionErrors.empty()) {
        float avgPredictionError = std::accumulate(predictionErrors.begin(), 
                                                  predictionErrors.end(), 0.0f) / predictionErrors.size();
        novelty.predictionError = std::abs(avgPredictionError);
    }
    
    // Temporal novelty (simplified - just based on time since last action)
    novelty.temporalNovelty = std::min(1.0f, static_cast<float>(currentTime - lastUpdateTime_) / 100.0f);
    
    // Other dimensions would be filled in based on actual cognitive state
    
    return novelty;
}

bool AdvancedAgentFeatures::useSkill(const std::string& skillName, float energyBudget) {
    return advancedMotorControl_.executeSkill(skillName, energyBudget);
}

float AdvancedAgentFeatures::getSkillProficiency(const std::string& skillName) const {
    auto it = skills_.find(skillName);
    if (it != skills_.end()) {
        return it->second.proficiency;
    }
    return 0.0f;
}

std::vector<std::string> AdvancedAgentFeatures::getAvailableSkills() const {
    std::vector<std::string> skills;
    for (const auto& pair : skills_) {
        if (pair.second.proficiency > 0.3f) {
            skills.push_back(pair.first);
        }
    }
    return skills;
}

void AdvancedAgentFeatures::recordExperience(const ExperienceRecord& experience) {
    memorySystem_.storeExperience(experience);
}

void AdvancedAgentFeatures::addSocialContact(const std::string& agentId) {
    socialLearningSystem_.addSocialContact(agentId);
}

void AdvancedAgentFeatures::updateSocialKnowledge(const std::string& otherAgent,
    const SocialKnowledge& knowledge) {
    socialLearningSystem_.updateSocialKnowledge(otherAgent, knowledge);
}

float AdvancedAgentFeatures::getSocialTrust(const std::string& otherAgent) const {
    return socialLearningSystem_.getRelationshipStrength(otherAgent);
}

MotorCommand AdvancedAgentFeatures::selectActionWithAdvancedFeatures(
    const MotorCommand& defaultAction,
    const std::vector<MotorCommand>& availableActions,
    const std::vector<float>& currentState) {
    
    // Enhanced curiosity-based action selection
    MotorCommand curiosityAction = enhancedCuriosity_.selectActionWithCuriosity(
        defaultAction, availableActions);
    
    // Predictive planning
    MotorCommand plannedAction = predictivePlanning_.selectActionWithPlanning(
        availableActions, currentState);
    
    // Advanced motor control with skills
    MotorCommand motorAction = advancedMotorControl_.selectActionWithSkills(defaultAction);
    
    // Combine based on behavioral state
    switch (behavioralState_) {
        case BehavioralState::EXPLORATION:
            return curiosityAction;
        case BehavioralState::EXPLOITATION:
            return plannedAction;
        case BehavioralState::LEARNING:
            return motorAction;
        case BehavioralState::IDLE:
        default:
            return defaultAction;
    }
}

AdvancedAgentFeatures::AdvancedAgentStats AdvancedAgentFeatures::getStatistics() const {
    AdvancedAgentStats stats;
    
    stats.totalSkills = skills_.size();
    stats.experiencesRecorded = memorySystem_.getRecentExperiences(1000).size();
    
    // Calculate average novelty
    if (!noveltyHistory_.empty()) {
        float totalNovelty = std::accumulate(noveltyHistory_.begin(), noveltyHistory_.end(), 0.0f);
        stats.averageNovelty = totalNovelty / noveltyHistory_.size();
    }
    
    // Calculate average success rate
    if (!successHistory_.empty()) {
        float totalSuccess = std::accumulate(successHistory_.begin(), successHistory_.end(), 0.0f);
        stats.averageSuccessRate = totalSuccess / successHistory_.size();
    }
    
    stats.totalEnergyExpended = energyHistory_.empty() ? 0.0f : 
        *std::max_element(energyHistory_.begin(), energyHistory_.end());
    
    stats.socialInteractions = 0; // Would need to track in real implementation
    stats.planningStepsExecuted = 0; // Would need to track in real implementation
    
    return stats;
}

} // namespace nlm
