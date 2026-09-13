#pragma once

#include "../agent/AgentBrain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Base behavior class - all advanced behaviors inherit from this
class Behavior {
public:
    virtual ~Behavior() = default;
    
    // Initialize behavior with agent context
    virtual void initialize(AgentBrain* agentBrain,
                           NeuralEpisodicMemory* memory,
                           NeuralPlanner* planner,
                           Brain* brain) {
        agentBrain_ = agentBrain;
        memory_ = memory;
        planner_ = planner;
        brain_ = brain;
        enabled_ = true;
    }
    
    // Update behavior based on current state
    virtual MotorCommand update(double timestep) {
        return MotorCommand::Wait;
    }
    
    // Get behavior name for identification
    virtual std::string getName() const { return "BaseBehavior"; }
    
    // Check if behavior should be active
    virtual bool shouldActivate() const { return enabled_; }
    
    // Enable/disable behavior
    virtual void setEnabled(bool enable) { enabled_ = enable; }
    
    // Get behavior confidence/score
    virtual float getConfidence() const { return confidence_; }
    
    // Update confidence based on experience
    virtual void updateConfidence(float delta) {
        confidence_ += delta;
        confidence_ = std::clamp(confidence_, 0.0f, 1.0f);
    }

protected:
    AgentBrain* agentBrain_ = nullptr;
    NeuralEpisodicMemory* memory_ = nullptr;
    NeuralPlanner* planner_ = nullptr;
    Brain* brain_ = nullptr;
    bool enabled_ = false;
    float confidence_ = 0.5f;
};

// RewardShapingBehavior: Enhances reward function design and shaping
class RewardShapingBehavior : public Behavior {
public:
    RewardShapingBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Apply shaped rewards to guide learning
        applyRewardShaping();
        
        // Select action based on shaped reward landscape
        return selectActionByShapedReward();
    }
    
    std::string getName() const override { return "RewardShaping"; }
    
    // Add custom reward shaping components
    void addShapingComponent(float (*component)(const std::vector<float>&, const ActionType&),
                             float weight = 1.0f) {
        shapingComponents_.push_back({component, weight});
    }
    
    // Set base reward function
    void setBaseRewardFunction(float (*rewardFunc)(const std::vector<float>&, const ActionType&)) {
        baseRewardFunc_ = rewardFunc;
    }

private:
    struct ShapingComponent {
        float (*func)(const std::vector<float>&, const ActionType&);
        float weight;
    };
    
    std::vector<ShapingComponent> shapingComponents_;
    float (*baseRewardFunc_)(const std::vector<float>&, const ActionType&) = nullptr;
    
    void applyRewardShaping() {
        if (agentBrain_) {
            // Modify expected rewards based on shaping components
            // In a full implementation, this would adjust the reward landscape
        }
    }
    
    MotorCommand selectActionByShapedReward() {
        if (!agentBrain_ || !planner_) return MotorCommand::Wait;
        
        // Get current state
        auto state = agentBrain_->getSensoryInputSize() > 0 ? 
            std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f) : 
            std::vector<float>();
            
        // Evaluate actions using shaped reward
        MotorCommand bestCmd = MotorCommand::Wait;
        float bestValue = -1000.0f;
        
        for (int i = 0; i < 7; ++i) { // MotorCommand enum values
            MotorCommand cmd = static_cast<MotorCommand>(i);
            
            // Compute shaped reward value
            float shapedValue = computeShapedReward(state, cmd);
            
            if (shapedValue > bestValue) {
                bestValue = shapedValue;
                bestCmd = cmd;
            }
        }
        
        return bestCmd;
    }
    
    float computeShapedReward(const std::vector<float>& state, MotorCommand action) {
        if (!baseRewardFunc_) return 0.0f;
        
        // Convert MotorCommand to ActionType for reward function
        ActionType actionType;
        switch (action) {
            case MotorCommand::MoveForward: actionType = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: actionType = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: actionType = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: actionType = ActionType::TurnRight; break;
            case MotorCommand::LookLeft: actionType = ActionType::LookLeft; break;
            case MotorCommand::LookRight: actionType = ActionType::LookRight; break;
            case MotorCommand::Interact: actionType = ActionType::Interact; break;
            case MotorCommand::Wait: actionType = ActionType::Wait; break;
            default: actionType = ActionType::Wait;
        }
        
        float baseReward = baseRewardFunc_(state, actionType);
        float shapingBonus = 0.0f;
        
        // Apply shaping components
        for (const auto& component : shapingComponents_) {
            shapingBonus += component.weight * component.func(state, actionType);
        }
        
        return baseReward + shapingBonus;
    }
};

// TransferLearningBehavior: Applies learned skills across different contexts
class TransferLearningBehavior : public Behavior {
public:
    TransferLearningBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Detect if we're in a new context
        if (detectNewContext()) {
            // Attempt transfer from similar past experiences
            return attemptTransferLearning();
        }
        
        return MotorCommand::Wait;
    }
    
    std::string getName() const override { return "TransferLearning"; }
    
    // Register a skill learned in one domain for transfer
    void registerSkill(const std::string& skillName,
                      const std::vector<float>& pattern,
                      MotorCommand action) {
        Skill skill;
        skill.name = skillName;
        skill.pattern = pattern;
        skill.action = action;
        skill.confidence = 0.5f;
        skill.usageCount = 0;
        skill.lastUsed = 0.0;
        
        skills_.push_back(skill);
    }
    
    // Retrieve best transferable skill for a given pattern
    MotorCommand findTransferSkill(const std::vector<float>& pattern,
                                  float& outConfidence) const {
        MotorCommand bestAction = MotorCommand::Wait;
        float bestConf = 0.0f;
        
        for (const auto& skill : skills_) {
            float similarity = computePatternSimilarity(pattern, skill.pattern);
            
            // Only consider skills with reasonable confidence
            if (similarity > 0.7f && skill.confidence > 0.3f) {
                float transferValue = similarity * skill.confidence * 
                                    (1.0f + skill.usageCount * 0.1f); // More usage = better transfer
                
                if (transferValue > bestConf) {
                    bestConf = transferValue;
                    bestAction = skill.action;
                }
            }
        }
        
        outConfidence = bestConf;
        return bestAction;
    }

private:
    struct Skill {
        std::string name;
        std::vector<float> pattern;
        MotorCommand action;
        float confidence;
        size_t usageCount;
        double lastUsed;
    };
    
    std::vector<Skill> skills_;
    
    bool detectNewContext() {
        if (!memory_ || !agentBrain_) return false;
        
        // Get recent episodes
        auto recent = memory_->getRecentEpisodes(10);
        
        // If we have little experience, we're in a new context
        return recent.size() < 5;
    }
    
    MotorCommand attemptTransferLearning() {
        if (!agentBrain_) return MotorCommand::Wait;
        
        // Get current sensory state as pattern
        auto state = agentBrain_->getSensoryInputSize() > 0 ?
            std::vector<float>(agentBrain_->getSensoryInputSize(), 0.0f) : 
            std::vector<float>();
        
        float confidence;
        MotorCommand action = findTransferSkill(state, confidence);
        
        // If we found a good transferable skill, use it
        if (confidence > 0.5f) {
            // Update skill usage
            for (auto& skill : skills_) {
                if (skill.action == action) {
                    skill.usageCount++;
                    skill.lastUsed = agentBrain_->getNeuromodulationLevel();
                    break;
                }
            }
        }
        
        return action;
    }
    
    float computePatternSimilarity(const std::vector<float>& pattern1,
                                  const std::vector<float>& pattern2) const {
        if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
        
        float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dot += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        
        if (norm1 < 0.0001f || norm2 < 0.0001f) return 0.0f;
        
        return dot / (std::sqrt(norm1) * std::sqrt(norm2));
    }
};

// HierarchicalPlanningBehavior: Multi-level action planning
class HierarchicalPlanningBehavior : public Behavior {
public:
    HierarchicalPlanningBehavior() = default;
    
    MotorCommand update(double timestep) override {
        if (!shouldActivate()) return MotorCommand::Wait;
        
        // Update hierarchical plans
        updateHierarchicalPlans(timestep);
        
        // Select appropriate action based on current level
        return selectActionFromPlan();
    }
    
    std::string getName() const override { return "HierarchicalPlanning"; }
    
    // Add a high-level goal
    void addHighLevelGoal(const std::vector<float>& goal,
                         float priority = 1.0f) {
        HighLevelGoal hlGoal;
        hlGoal.goal = goal;
        hlGoal.priority = priority;
        hlGoal.createdTime = agentBrain_ ? agentBrain_->getNeuromodulationLevel() : 0.0f;
        hlGoal.isActive = true;
        
        highLevelGoals_.push_back(hlGoal);
    }
    
    // Convert high-level goal to sequence of actions
    std::vector<ActionType> decomposeGoal(const std::vector<float>& goal) {
        std::vector<ActionType> plan;
        
        // Simple decomposition - in full implementation would use learned hierarchies
        if (goal.size() > 0 && goal[0] > 0.7f) {
            // Goal is about approaching something - move forward
            plan.push_back(ActionType::MoveForward);
        }
        
        if (goal.size() > 1 && goal[1] > 0.5f) {
            // Goal has second component - interact
            plan.push_back(ActionType::Interact);
        }
        
        return plan;
    }
    
    // Get current top-level plan
    std::vector<ActionType> getCurrentPlan() const {
        if (currentPlan_.empty()) return {};
        
        // Return subplan based on execution progress
        size_t executed = executedSteps_;
        if (executed >= currentPlan_.size()) return {};
        
        std::vector<ActionType> remaining;
        for (size_t i = executed; i < currentPlan_.size(); ++i) {
            remaining.push_back(currentPlan_[i]);
        }
        
        return remaining;
    }
    
    // Get planning confidence for current goal
    float getPlanningConfidence() const {
        if (highLevelGoals_.empty()) return 0.0f;
        
        // Compute average confidence across all active goals
        float totalConf = 0.0f;
        size_t count = 0;
        
        for (const auto& goal : highLevelGoals_) {
            if (goal.isActive) {
                totalConf += goal.confidence;
                count++;
            }
        }
        
        return count > 0 ? totalConf / count : 0.0f;
    }

private:
    struct HighLevelGoal {
        std::vector<float> goal;
        float priority;
        float confidence;
        double createdTime;
        bool isActive;
    };
    
    std::vector<HighLevelGoal> highLevelGoals_;
    std::vector<ActionType> currentPlan_;
    size_t executedSteps_ = 0;
    float planningHorizon_ = 5.0f; // Time steps for planning
    
    void updateHierarchicalPlans(double timestep) {
        // Update high-level goal confidence based on progress
        for (auto& goal : highLevelGoals_) {
            if (goal.isActive) {
                // Simple confidence update - in full implementation would use predictive models
                goal.confidence = std::min(1.0f, goal.confidence + 0.01f * static_cast<float>(timestep));
            }
        }
        
        // Check if we need to replan
        if (shouldReplan()) {
            replan();
        }
    }
    
    bool shouldReplan() {
        // Replan if we're far from goal or confidence is low
        if (highLevelGoals_.empty()) return false;
        
        bool needReplan = false;
        for (const auto& goal : highLevelGoals_) {
            if (goal.isActive && goal.confidence < 0.3f) {
                needReplan = true;
                break;
            }
        }
        
        // Also replan periodically
        if (agentBrain_ && agentBrain_->getNeuromodulationLevel() > 0.5f) {
            needReplan = true;
        }
        
        return needReplan;
    }
    
    void replan() {
        if (highLevelGoals_.empty()) return;
        
        // Pick highest priority active goal
        HighLevelGoal* bestGoal = nullptr;
        float bestPriority = 0.0f;
        
        for (auto& goal : highLevelGoals_) {
            if (goal.isActive && goal.priority > bestPriority) {
                bestPriority = goal.priority;
                bestGoal = &goal;
            }
        }
        
        if (bestGoal) {
            // Decompose goal into action sequence
            currentPlan_ = decomposeGoal(bestGoal->goal);
            executedSteps_ = 0;
            
            // Set planning horizon based on goal complexity
            planningHorizon_ = std::max(1.0f, static_cast<float>(currentPlan_.size()) * 0.5f);
        }
    }
    
    MotorCommand selectActionFromPlan() {
        if (currentPlan_.empty() || executedSteps_ >= currentPlan_.size()) {
            return MotorCommand::Wait;
        }
        
        // Get next action in plan
        ActionType nextAction = currentPlan_[executedSteps_];
        
        // Convert to MotorCommand
        MotorCommand cmd;
        switch (nextAction) {
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
        
        // Mark this action as executed
        executedSteps_++;
        
        return cmd;
    }
};

} // namespace nlm