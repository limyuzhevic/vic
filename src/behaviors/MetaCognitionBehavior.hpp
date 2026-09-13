#pragma once

#include "../agent/AgentBrain.hpp"
#include "../behaviors/Behaviors.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// MetaCognitionBehavior: Implements self-monitoring and confidence estimation
class MetaCognitionBehavior : public Behavior {
public:
    MetaCognitionBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Update metacognitive awareness
        updateMetaCognition(timestep);
        
        // Select action based on metacognitive assessment
        return selectMetaCognitiveAction();
    }
    
    std::string getName() const override { return "MetaCognition"; }
    
    // Record prediction outcome for confidence calibration
    void recordPredictionOutcome(ActionType predictedAction,
                               ActionType actualAction,
                               float predictedReward,
                               float actualReward,
                               const std::vector<float>& state) {
        PredictionRecord record;
        record.predictedAction = predictedAction;
        record.actualAction = actualAction;
        record.predictedReward = predictedReward;
        record.actualReward = actualReward;
        record.state = state;
        record.timestamp = predictionRecordCount_++;
        
        // Compute prediction error
        record.rewardPredictionError = actualReward - predictedReward;
        
        // Compute action selection error
        record.actionPredictionError = (actualAction == predictedAction) ? 0.0f : 1.0f;
        
        predictionRecords_.push_back(record);
        
        // Update confidence based on prediction accuracy
        float accuracy = (actualAction == predictedAction) ? 1.0f : 0.0f;
        updateConfidence(accuracy * 0.2f);
        
        // Update metacognitive confidence for this action
        updateActionConfidence(predictedAction, accuracy * 0.5f);
    }
    
    // Get confidence in action selection for a given action
    float getActionConfidence(ActionType action) const {
        auto it = actionConfidence_.find(action);
        if (it != actionConfidence_.end()) {
            return it->second;
        }
        return 0.5f; // Default confidence
    }
    
    // Get overall metacognitive confidence
    float getMetaCognitiveConfidence() const {
        if (predictionRecords_.empty()) return 0.5f;
        
        float totalConfidence = 0.0f;
        for (const auto& record : predictionRecords_) {
            totalConfidence += getActionConfidence(record.predictedAction);
        }
        
        return totalConfidence / predictionRecords_.size();
    }
    
    // Get prediction calibration (how well confidence matches accuracy)
    float getPredictionCalibration() const {
        if (predictionRecords_.empty()) return 0.5f;
        
        float totalCalibration = 0.0f;
        for (const auto& record : predictionRecords_) {
            float confidence = getActionConfidence(record.predictedAction);
            float accuracy = (record.actualAction == record.predictedAction) ? 1.0f : 0.0f;
            
            // Calibration = accuracy - confidence (should be near 0 for well-calibrated)
            totalCalibration += accuracy - confidence;
        }
        
        return totalCalibration / predictionRecords_.size();
    }
    
    // Get number of prediction records
    size_t getPredictionRecordCount() const { return predictionRecords_.size(); }
    
    // Get self-monitoring score (awareness of one's own uncertainty)
    float getSelfMonitoringScore() const {
        float totalMonitoring = 0.0f;
        size_t count = 0;
        
        for (const auto& record : predictionRecords_) {
            float confidence = getActionConfidence(record.predictedAction);
            
            // Self-monitoring = 1 - confidence (higher when uncertain)
            totalMonitoring += 1.0f - confidence;
            count++;
        }
        
        return count > 0 ? totalMonitoring / count : 0.5f;
    }
    
    // Reset metacognitive state
    void reset() {
        predictionRecords_.clear();
        actionConfidence_.clear();
        predictionRecordCount_ = 0;
        confidenceCalibration_ = 0.5f;
        updateConfidence(-0.1f);
    }

private:
    struct PredictionRecord {
        ActionType predictedAction;
        ActionType actualAction;
        float predictedReward;
        float actualReward;
        float rewardPredictionError;
        float actionPredictionError;
        std::vector<float> state;
        size_t timestamp;
    };
    
    std::vector<PredictionRecord> predictionRecords_;
    std::unordered_map<ActionType, float> actionConfidence_;
    size_t predictionRecordCount_ = 0;
    float confidenceCalibration_ = 0.5f;
    
    void updateMetaCognition(double timestep) {
        // Update metacognitive awareness based on prediction history
        if (predictionRecords_.size() > 10) {
            // Analyze prediction accuracy over recent history
            size_t recentCount = std::min<size_t>(predictionRecords_.size(), 20);
            
            float totalAccuracy = 0.0f;
            for (size_t i = 0; i < recentCount; ++i) {
                size_t idx = predictionRecords_.size() - 1 - i;
                float accuracy = (predictionRecords_[idx].actualAction == predictionRecords_[idx].predictedAction) ? 1.0f : 0.0f;
                totalAccuracy += accuracy;
            }
            
            float recentAccuracy = totalAccuracy / recentCount;
            
            // Adjust overall confidence based on recent accuracy
            float delta = (recentAccuracy - 0.5f) * 0.1f;
            updateConfidence(delta);
            
            // Update calibration based on pattern of over/under-confidence
            float calibration = getPredictionCalibration();
            confidenceCalibration_ = confidenceCalibration_ * 0.9f + calibration * 0.1f;
        }
    }
    
    void updateActionConfidence(ActionType action, float delta) {
        actionConfidence_[action] += delta;
        actionConfidence_[action] = std::clamp(actionConfidence_[action], 0.0f, 1.0f);
    }
    
    MotorCommand selectMetaCognitiveAction() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // Check if we have good metacognitive confidence
        float metaCognitiveConfidence = getMetaCognitiveConfidence();
        
        if (metaCognitiveConfidence > 0.7f) {
            // High confidence - use default motor decoding
            return MotorCommand::Wait; // Would return from agentBrain_ in full implementation
        }
        else if (metaCognitiveConfidence < 0.3f) {
            // Low confidence - increase exploration
            float exploreChance = 0.5f - metaCognitiveConfidence * 0.3f;
            
            float r = (agentBrain_ && agentBrain_->getBrain()) ?
                agentBrain_->getBrain()->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
            
            if (r < exploreChance) {
                // Choose random action for exploration
                std::vector<ActionType> availableActions = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                
                if (!availableActions.empty()) {
                    int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                        agentBrain_->getBrain()->getRandomGenerator()->uniformInt(0, availableActions.size()) : 0;
                    
                    // Convert to MotorCommand
                    MotorCommand cmd;
                    switch (availableActions[choice]) {
                        case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                        case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                        case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                        case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                        case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                        case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                        case ActionType::Interact: cmd = MotorCommand::Interact; break;
                        case ActionType::Wait: cmd = MotorCommand::Wait; break;
                        default: cmd = MotorCommand::Wait;
                    }
                    return cmd;
                }
            }
        }
        
        // Medium confidence - use a more cautious approach
        // Check self-monitoring - if low, we're uncertain
        float selfMonitoring = getSelfMonitoringScore();
        
        if (selfMonitoring > 0.7f) {
            // High self-monitoring - we're aware of uncertainty, explore more
            return MotorCommand::Wait; // Would use curiosity in full implementation
        }
        
        // Default action
        return MotorCommand::Wait;
    }
};

// CuriosityDrivenBehavior: Enhanced exploration based on uncertainty
class CuriosityDrivenBehavior : public Behavior {
public:
    CuriosityDrivenBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Update curiosity based on prediction errors and novelty
        updateCuriosity(timestep);
        
        // Select action based on curiosity-driven exploration
        return selectCuriosityDrivenAction();
    }
    
    std::string getName() const override { return "CuriosityDriven"; }
    
    // Update curiosity level based on prediction errors and novelty
    void updateCuriosity(double timestep) {
        // Base curiosity on prediction error and novelty
        float predictionErrorCuriosity = std::abs(getPredictionError()) * 0.5f;
        
        // In a full implementation, this would access agent's curiosity state
        // For now, use a simple approximation
        
        // Add uncertainty-based curiosity
        float uncertainty = 1.0f - getConfidence();
        
        // Time-based exploration bonus
        float timeBonus = 0.0f;
        if (curiosityTimer_ > curiosityCooldown_) {
            timeBonus = 0.2f * std::min(1.0f, (curiosityTimer_ - curiosityCooldown_) / 10.0f);
            curiosityTimer_ = 0.0;
        }
        
        // Update curiosity level
        float newCuriosity = predictionErrorCuriosity * 0.4f + uncertainty * 0.4f + timeBonus * 0.2f;
        newCuriosity = std::clamp(newCuriosity, 0.0f, 1.0f);
        
        // Decay curiosity over time
        currentCuriosity_ = newCuriosity * curiosityDecay_;
        
        // Update timer
        curiosityTimer_ += timestep;
        
        // Update confidence based on exploration
        if (currentCuriosity_ > 0.7f) {
            updateConfidence(0.05f * timestep);
        }
    }
    
    // Get current curiosity level
    float getCuriosityLevel() const {
        return currentCuriosity_;
    }
    
    // Set curiosity parameters
    void setCuriosityParameters(float decayRate, float cooldownTime) {
        curiosityDecay_ = decayRate;
        curiosityCooldown_ = cooldownTime;
    }
    
    // Get exploration bonus for a given state-action pair
    float getExplorationBonus(const std::vector<float>& state, ActionType action) const {
        // Curiosity-driven exploration based on uncertainty
        float uncertainty = 1.0f - getConfidence();
        
        // Bonus for novel state-action combinations
        float noveltyBonus = 0.0f;
        
        // Check if we've seen this state-action combination before
        bool seenBefore = false;
        for (const auto& experienced : experiencedActions_) {
            if (experienced.state == state && experienced.action == action) {
                seenBefore = true;
                break;
            }
        }
        
        if (!seenBefore) {
            // Novel combination gets exploration bonus
            noveltyBonus = 0.5f * (1.0f - uncertainty); // Higher when less certain
        }
        
        // Bonus based on curiosity level
        float curiosityBonus = currentCuriosity_ * 0.3f * (1.0f - uncertainty);
        
        // Temporal difference bonus (explore different at different times)
        float temporalBonus = 0.0f;
        if (curiosityTimer_ > curiosityCooldown_) {
            temporalBonus = 0.1f * (curiosityTimer_ / (curiosityCooldown_ * 2.0f));
        }
        
        return noveltyBonus + curiosityBonus + temporalBonus;
    }
    
    // Record that we've experienced a state-action combination
    void recordExperience(const std::vector<float>& state, ActionType action) {
        ExperiencedAction exp;
        exp.state = state;
        exp.action = action;
        exp.timestamp = experienceCount_++;
        
        experiencedActions_.push_back(exp);
        
        // Keep memory bounded
        if (experiencedActions_.size() > 500) {
            experiencedActions_.erase(experiencedActions_.begin());
        }
    }
    
    // Get number of experienced actions
    size_t getExperienceCount() const { return experiencedActions_.size(); }

private:
    struct ExperiencedAction {
        std::vector<float> state;
        ActionType action;
        size_t timestamp;
    };
    
    std::vector<ExperiencedAction> experiencedActions_;
    size_t experienceCount_ = 0;
    
    // Curiosity dynamics
    float currentCuriosity_ = 0.0f;
    float curiosityDecay_ = 0.95f;
    float curiosityCooldown_ = 10.0f;
    double curiosityTimer_ = 0.0;
    
    float getPredictionError() const {
        if (agentBrain_) {
            return agentBrain_->getPredictionError();
        }
        return 0.0f;
    }
    
    MotorCommand selectCuriosityDrivenAction() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // High curiosity -> more exploration
        if (currentCuriosity_ > 0.8f) {
            // Max exploration - choose random action
            std::vector<ActionType> availableActions = {
                ActionType::MoveForward, ActionType::MoveBackward,
                ActionType::TurnLeft, ActionType::TurnRight,
                ActionType::Interact, ActionType::Wait
            };
            
            if (!availableActions.empty()) {
                int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                    agentBrain_->getBrain()->getRandomGenerator()->uniformInt(0, availableActions.size()) : 0;
                
                // Convert to MotorCommand
                MotorCommand cmd;
                switch (availableActions[choice]) {
                    case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                    case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                    case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                    case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                    case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                    case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                    case ActionType::Interact: cmd = MotorCommand::Interact; break;
                    case ActionType::Wait: cmd = MotorCommand::Wait; break;
                    default: cmd = MotorCommand::Wait;
                }
                return cmd;
            }
        }
        
        else if (currentCuriosity_ > 0.5f) {
            // Medium curiosity - explore options that have exploration bonuses
            std::vector<std::pair<ActionType, float>> scoredActions;
            
            std::vector<ActionType> availableActions = {
                ActionType::MoveForward, ActionType::MoveBackward,
                ActionType::TurnLeft, ActionType::TurnRight,
                ActionType::Interact, ActionType::Wait
            };
            
            // Get current state
            auto state = agentBrain_->getSensoryInputSize() > 0 ?
                std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f) : 
                std::vector<float>();
            
            // Score each action by exploration bonus
            for (const auto& action : availableActions) {
                float bonus = getExplorationBonus(state, action);
                scoredActions.push_back({action, bonus});
            }
            
            // Select action with highest exploration bonus
            if (!scoredActions.empty()) {
                std::sort(scoredActions.begin(), scoredActions.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
                
                ActionType bestAction = scoredActions[0].first;
                
                // With some probability, choose lower-scoring action to ensure exploration
                float r = (agentBrain_ && agentBrain_->getBrain()) ?
                    agentBrain_->getBrain()->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
                
                if (r < 0.2f && scoredActions.size() > 1) {
                    // Choose from bottom 20% of actions
                    int startIdx = std::max(0, static_cast<int>(scoredActions.size() * 0.8f));
                    int choice = (agentBrain_ && agentBrain_->getBrain()) ?
                        agentBrain_->getBrain()->getRandomGenerator()->uniformInt(startIdx, scoredActions.size()) : startIdx;
                    bestAction = scoredActions[choice].first;
                }
                
                // Convert to MotorCommand
                MotorCommand cmd;
                switch (bestAction) {
                    case ActionType::MoveForward: cmd = MotorCommand::MoveForward; break;
                    case ActionType::MoveBackward: cmd = MotorCommand::MoveBackward; break;
                    case ActionType::TurnLeft: cmd = MotorCommand::TurnLeft; break;
                    case ActionType::TurnRight: cmd = MotorCommand::TurnRight; break;
                    case ActionType::LookLeft: cmd = MotorCommand::LookLeft; break;
                    case ActionType::LookRight: cmd = MotorCommand::LookRight; break;
                    case ActionType::Interact: cmd = MotorCommand::Interact; break;
                    case ActionType::Wait: cmd = MotorCommand::Wait; break;
                    default: cmd = MotorCommand::Wait;
                }
                return cmd;
            }
        }
        
        // Low curiosity - mostly follow standard policy
        return MotorCommand::Wait;
    }
};

} // namespace nlm