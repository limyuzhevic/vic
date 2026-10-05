#include "MotorSystem.hpp"
#include "AgentBrain.hpp"
#include "../agent/AgentBody.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <queue>
#include <numeric>
#include "../agent/SensoryPercept.hpp"

namespace nlm {

struct MotorSystem::Impl {
    // Integration
    AgentBrain* agentBrain_;
    Brain* brain_;
    std::shared_ptr<RandomGenerator> rng_;
    
    // Enhanced action selection
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    std::unordered_map<ActionType, float> actionProficiencies;
    
    // Neuromodulation-based action selection
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionError_;
    
    // Working memory for action rehearsal
    std::vector<float> workingMemoryBuffer;
    std::vector<ActionType> activeActionSequence;
    std::vector<float> actionSequenceStrengths;
    
    // Episodic memory for action planning
    std::vector<EpisodicMemoryItem> episodeHistory;
    std::vector<ActionType> plannedActionSequences;
    
    // Procedural memory and motor skills
    struct MotorSkill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        float successRate;
        float averageExecutionTime;
        
        MotorSkill() : proficiency(0.0f), successRate(0.0f), averageExecutionTime(0.0f) {}
    };
    std::vector<MotorSkill> learnedSkills;
    
    // Motor planning and sequencing
    std::vector<ActionType> plannedSequence;
    std::vector<size_t> plannedSequencePosition;
    float planningHorizon_;
    
    // Sensory-motor integration
    std::vector<float> sensoryInputs;
    std::vector<float> motorOutputs;
    std::vector<float> predictionErrors;
    
    // Development and adaptation
    double developmentalAge_;
    float plasticityModifier_;
    float learningRate_;
    float explorationRate_;
    
    // Real-time execution with feedback
    std::queue<ActionType> pendingActions;
    std::vector<float> executionFeedback;
    std::vector<float> adaptationWeights;
    
    // Statistics and performance tracking
    std::unordered_map<ActionType, int> actionCounts;
    std::unordered_map<ActionType, float> actionRewards;
    std::unordered_map<ActionType, float> actionTimes;
    
    // State
    bool initialized_;
    size_t currentStep_;
    Timestamp currentTime_;
    
    Impl() : agentBrain_(nullptr)
        , brain_(nullptr)
        , planningHorizon_(0.5f)
        , developmentalAge_(0.0)
        , plasticityModifier_(1.0f)
        , learningRate_(0.01f)
        , explorationRate_(0.1f)
        , initialized_(false)
        , currentStep_(0)
        , currentTime_(0.0)
    {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::LookUp,
            ActionType::LookDown,
            ActionType::Interact,
            ActionType::Eat,
            ActionType::Drink,
            ActionType::Rest,
            ActionType::Wait
        };
        
        // Initialize random generator
        rng_ = std::make_shared<RandomGenerator>(42);
        
        // Initialize neuromodulation levels
        dopamineLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        predictionError_ = 0.0f;
        
        // Initialize working memory buffer
        workingMemoryBuffer.resize(128, 0.0f);
        
        // Initialize adaptation weights
        for (int i = 0; i < static_cast<int>(ActionType::Custom); ++i) {
            adaptationWeights.push_back(1.0f);
        }
        
        NLM_LOG_INFO("Enhanced MotorSystem with full neural integration initialized");
    }
    
    // Initialize with agent brain reference
    void initialize(AgentBrain* agentBrain) {
        if (!agentBrain) {
            NLM_LOG_ERROR("MotorSystem::initialize - Invalid AgentBrain reference");
            return;
        }
        
        agentBrain_ = agentBrain;
        brain_ = agentBrain->getBrain();
        
        if (!brain_) {
            NLM_LOG_ERROR("MotorSystem::initialize - Invalid Brain reference");
            return;
        }
        
        // Initialize enhanced integration systems
        initializeWorkingMemoryIntegration();
        initializeEpisodicMemoryIntegration();
        initializeNeuromodulationIntegration();
        initializeProceduralMemoryIntegration();
        initializeMotorPlanning();
        
        initialized_ = true;
        
        NLM_LOG_INFO("Enhanced MotorSystem fully integrated with AgentBrain");
    }
    
    // Enhanced action selection using neuromodulation
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion) {
        if (!initialized_ || !agentBrain_ || !brain_) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Update neuromodulation levels from AgentBrain
        updateNeuromodulationLevels();
        
        // Get enhanced action selection based on multiple factors
        ActionType selectedAction = enhancedActionSelection();
        
        // Create action with parameters based on neural activity
        std::unique_ptr<Action> action = createEnhancedAction(selectedAction, motorRegion);
        
        // Store in action history
        actionHistory.push_back(action.get());
        
        // Update statistics
        actionCounts[selectedAction]++;
        
        return action;
    }
    
    // Enhanced action selection with neuromodulation, curiosity, and reward
    ActionType enhancedActionSelection() {
        if (!agentBrain_ || !brain_) return ActionType::Wait;
        
        // Get neuromodulation levels
        float dopamine = agentBrain_->getNeuromodulationLevel();
        float curiosity = agentBrain_->getCuriosityLevel();
        float novelty = agentBrain_->getNoveltyLevel();
        
        // Compute action values using multiple factors
        std::vector<std::pair<ActionType, float>> actionValues;
        
        for (ActionType action : availableActions) {
            float value = computeActionValue(action, dopamine, curiosity, novelty);
            actionValues.push_back({action, value});
        }
        
        // Apply exploration vs exploitation based on neuromodulation
        applyExploration(actionValues, curiosity, dopamine);
        
        // Select action based on values (softmax or epsilon-greedy)
        ActionType selected = selectWithPolicy(actionValues);
        
        return selected;
    }
    
    // Compute action value using neural and learned factors
    float computeActionValue(ActionType action, float dopamine, float curiosity, float novelty) {
        float value = 0.0f;
        
        // Base preference from action preferences
        value += actionPreferences[action] * 0.3f;
        
        // Neuromodulation contribution
        if (action == ActionType::Interact || action == ActionType::Eat || action == ActionType::Drink) {
            value += dopamine * 0.4f;  // Reward-based actions
        }
        
        // Curiosity-based exploration
        if (curiosity > 0.5f) {
            // Random exploration for high curiosity
            value += curiosity * rng_->uniformReal(-0.5f, 0.5f);
        }
        
        // Novelty-based exploration
        if (novelty > 0.3f) {
            value += novelty * 0.2f;
        }
        
        // Learned proficiency
        value += actionProficiencies[action] * 0.5f;
        
        return value;
    }
    
    // Apply exploration policy
    void applyExploration(std::vector<std::pair<ActionType, float>>& actionValues, float curiosity, float dopamine) {
        if (curiosity < 0.5f) return; // No exploration needed
        
        // Higher curiosity = more random exploration
        float exploreChance = curiosity * 0.3f;
        
        float r = rng_->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Completely random action for exploration
            int randomIdx = rng_->uniformInt(0, actionValues.size() - 1);
            // Force this action to have maximum value
            actionValues[randomIdx].second = 10.0f;
        } else if (dopamine < -0.2f) {
            // Negative dopamine encourages exploration of different actions
            // Reduce the advantage of previously successful actions
            for (auto& pair : actionValues) {
                pair.second *= 0.7f;
            }
        }
    }
    
    // Select action using softmax or epsilon-greedy policy
    ActionType selectWithPolicy(std::vector<std::pair<ActionType, float>>& actionValues) {
        if (actionValues.empty()) return ActionType::Wait;
        
        // Find maximum value
        float maxValue = actionValues[0].second;
        for (const auto& pair : actionValues) {
            maxValue = std::max(maxValue, pair.second);
        }
        
        // Apply softmax for probabilistic selection
        float sumExp = 0.0f;
        std::vector<float> expValues;
        
        for (const auto& pair : actionValues) {
            float expVal = std::exp((pair.second - maxValue) / 0.1f); // Temperature = 0.1
            expValues.push_back(expVal);
            sumExp += expVal;
        }
        
        // Normalize to probabilities
        std::vector<float> probabilities;
        for (float expVal : expValues) {
            probabilities.push_back(expVal / sumExp);
        }
        
        // Select based on probabilities
        float r = rng_->uniformReal(0.0f, 1.0f);
        float cumulative = 0.0f;
        
        for (size_t i = 0; i < actionValues.size(); ++i) {
            cumulative += probabilities[i];
            if (r <= cumulative) {
                return actionValues[i].first;
            }
        }
        
        return actionValues.back().first; // Fallback
    }
    
    // Create enhanced action with parameters based on neural activity
    std::unique_ptr<Action> createEnhancedAction(ActionType type, const NeuralRegion& motorRegion) {
        std::unique_ptr<Action> action = std::make_unique<Action>(type);
        
        // Get neural activity for parameter generation
        std::vector<float> parameters;
        
        // Generate parameters based on action type and neural activity
        switch (type) {
            case ActionType::MoveForward:
            case ActionType::MoveBackward:
                parameters.push_back(1.0f); // Speed
                parameters.push_back(agentBrain_->getBrain()->getAverageFiringRate()); // Neural intensity
                break;
            case ActionType::TurnLeft:
            case ActionType::TurnRight:
                parameters.push_back(rng_->uniformReal(0.5f, 2.0f)); // Turn angle
                parameters.push_back(agentBrain_->getBrain()->getFiringNeuronCount() * 0.001f); // Intensity
                break;
            case ActionType::Look:
            case ActionType::LookUp:
            case ActionType::LookDown:
                parameters.push_back(rng_->uniformReal(0.0f, 360.0f)); // Direction
                parameters.push_back(agentBrain_->getBrain()->getAverageFiringRate() * 0.5f);
                break;
            case ActionType::Interact:
                parameters.push_back(agentBrain_->getBrain()->getActiveNeuronCount() * 0.001f); // Force
                parameters.push_back(1.0f); // Duration
                break;
            default:
                parameters.push_back(0.0f); // Default
                parameters.push_back(0.0f);
        }
        
        action->setParameters(parameters);
        return action;
    }
    
    // Update neuromodulation levels from AgentBrain
    void updateNeuromodulationLevels() {
        if (!agentBrain_) return;
        
        dopamineLevel_ = agentBrain_->getNeuromodulationLevel();
        curiosityLevel_ = agentBrain_->getCuriosityLevel();
        noveltyLevel_ = agentBrain_->getNoveltyLevel();
        predictionError_ = agentBrain_->getPredictionError();
    }
    
    // Initialize working memory integration
    void initializeWorkingMemoryIntegration() {
        if (!brain_) return;
        
        NeuralWorkingMemory* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            workingMemory->setCapacity(128);
            workingMemory->setDecayRate(0.95f);
        }
    }
    
    // Initialize episodic memory integration
    void initializeEpisodicMemoryIntegration() {
        if (!brain_) return;
        
        NeuralEpisodicMemory* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            // Episodic memory is ready for use
        }
    }
    
    // Initialize neuromodulation integration
    void initializeNeuromodulationIntegration() {
        // Neuromodulation is already integrated through AgentBrain
    }
    
    // Initialize procedural memory integration
    void initializeProceduralMemoryIntegration() {
        // Procedural memory (MotorSystem::Impl::MotorSkill) is ready for use
    }
    
    // Initialize motor planning
    void initializeMotorPlanning() {
        plannedActionSequences.clear();
        plannedSequencePosition.clear();
        plannedSequence.clear();
        
        // Initialize with some basic action sequences
        plannedActionSequences = {
            ActionType::MoveForward, ActionType::TurnLeft, ActionType::MoveForward,
            ActionType::TurnRight, ActionType::MoveForward
        };
        
        for (size_t i = 0; i < plannedActionSequences.size(); ++i) {
            plannedSequencePosition.push_back(0);
        }
    }
    
    // Apply reward-based learning and motor skill acquisition
    void applyRewardLearning(ActionType action, float reward, double timestep) {
        if (!agentBrain_) return;
        
        // Update action reward statistics
        float currentReward = actionRewards[action];
        actionRewards[action] = 0.95f * currentReward + 0.05f * reward;
        
        // Update proficiency based on reward
        actionProficiencies[action] = std::clamp(
            actionProficiencies[action] + learningRate_ * reward * plasticityModifier_,
            0.0f, 1.0f);
        
        // Learn new motor skills based on successful actions
        if (reward > 0.5f && actionProficiencies[action] > 0.7f) {
            learnMotorSkill(action, reward);
        }
        
        // Apply neuromodulation from reward prediction error
        float predictedReward = expectedActionReward(action);
        agentBrain_->applyRewardModulation(reward, predictedReward);
        
        // Update development
        updateDevelopment(timestep);
    }
    
    // Learn new motor skill
    void learnMotorSkill(ActionType action, float reward) {
        MotorSkill skill;
        skill.name = actionToString(action);
        skill.proficiency = actionProficiencies[action];
        skill.successRate = reward;
        skill.averageExecutionTime = 1.0f; // Could be tracked
        
        learnedSkills.push_back(skill);
        
        NLM_LOG_INFO("MotorSystem: Learned new skill " + skill.name + " with proficiency " + 
                    std::to_string(skill.proficiency) + " and success rate " + 
                    std::to_string(skill.successRate));
    }
    
    // Get expected reward for an action
    float expectedActionReward(ActionType action) const {
        if (actionRewards.find(action) != actionRewards.end()) {
            return actionRewards.at(action);
        }
        return 0.0f;
    }
    
    // Update development system
    void updateDevelopment(double timestep) {
        developmentalAge_ += timestep;
        
        // Adjust learning rate based on development stage
        if (developmentalAge_ < 60.0) {
            // Early development - high plasticity
            learningRate_ = 0.05f;
            explorationRate_ = 0.3f;
            plasticityModifier_ = 1.0f;
        } else if (developmentalAge_ < 300.0) {
            // Critical period
            learningRate_ = 0.02f;
            explorationRate_ = 0.2f;
            plasticityModifier_ = 0.8f;
        } else {
            // Adult
            learningRate_ = 0.01f;
            explorationRate_ = 0.1f;
            plasticityModifier_ = 0.5f;
        }
        
        // Update AgentBrain development system if available
        if (agentBrain_) {
            agentBrain_->updateDevelopment(timestep);
        }
    }
    
    // Convert action type to string
    std::string actionToString(ActionType action) const {
        switch (action) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const {
        return availableActions;
    }
    
    // Set action preference (for exploration)
    void setActionPreference(ActionType action, float preference) {
        actionPreferences[action] = std::clamp(preference, 0.0f, 1.0f);
    }
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const {
        return actionHistory;
    }
    
    // Clear history
    void clearHistory() {
        for (auto* action : actionHistory) {
            delete action;
        }
        actionHistory.clear();
    }
    
    // Execute pending actions with feedback
    void executePendingActions(const AgentBody& agentBody, const SimpleWorld& world, 
                               double timestep, Timestamp currentTime) {
        currentStep_++;
        currentTime_ = currentTime;
        
        while (!pendingActions.empty()) {
            ActionType action = pendingActions.front();
            pendingActions.pop();
            
            // Execute action and get feedback
            ActionResult result = executeActionWithFeedback(action, agentBody, world, timestep);
            
            // Store execution feedback
            executionFeedback.push_back(result.reward);
            
            // Apply reward learning
            applyRewardLearning(action, result.reward, timestep);
            
            // Keep only recent feedback
            if (executionFeedback.size() > 100) {
                executionFeedback.erase(executionFeedback.begin());
            }
        }
    }
    
    // Execute actions with full feedback loop
    ActionResult executeActionWithFeedback(ActionType action, const AgentBody& agentBody, 
                                           const SimpleWorld& world, double timestep) {
        // In a real implementation, this would apply the motor command to the agent
        // and world, returning reward, success status, and message
        
        // For now, return a simple result based on action type
        float reward = 0.0f;
        bool success = true;
        std::string message = "Action executed";
        
        switch (action) {
            case ActionType::MoveForward:
                reward = 0.1f;
                success = agentBody.isMoving;
                break;
            case ActionType::MoveBackward:
                reward = 0.1f;
                break;
            case ActionType::TurnLeft:
            case ActionType::TurnRight:
                reward = 0.05f;
                break;
            case ActionType::Interact:
                reward = 0.5f;
                break;
            case ActionType::Eat:
                reward = 1.0f;
                break;
            case ActionType::Drink:
                reward = 0.8f;
                break;
            case ActionType::Rest:
                reward = 0.2f;
                break;
            case ActionType::Wait:
                reward = 0.0f;
                break;
            default:
                reward = 0.0f;
        }
        
        return ActionResult(reward, success, message);
    }
    
    // Update sensory-motor integration
    void updateSensoryMotorIntegration(const SensoryPercept& percept) {
        // Store sensory inputs for integration
        sensoryInputs.clear();
        for (float val : percept.getVision()) {
            sensoryInputs.push_back(val);
        }
        
        // Add other sensory modalities
        for (float val : percept.getTouch()) {
            sensoryInputs.push_back(val);
        }
        
        // Update motor outputs based on sensory integration
        updateMotorOutputsFromSensory();
    }
    
    // Update motor outputs based on sensory inputs
    void updateMotorOutputsFromSensory() {
        motorOutputs.clear();
        
        // Simple integration: convert sensory to motor modulation
        for (size_t i = 0; i < sensoryInputs.size() && i < adaptationWeights.size(); ++i) {
            float adaptedOutput = sensoryInputs[i] * adaptationWeights[i % adaptationWeights.size()];
            motorOutputs.push_back(adaptedOutput);
        }
        
        // Apply to action preferences
        for (size_t i = 0; i < std::min(motorOutputs.size(), availableActions.size()); ++i) {
            ActionType action = availableActions[i];
            float feedback = motorOutputs[i];
            
            // Adjust action preferences based on sensory feedback
            if (feedback > 0.5f) {
                setActionPreference(action, actionPreferences[action] * 1.1f);
            } else if (feedback < -0.5f) {
                setActionPreference(action, actionPreferences[action] * 0.9f);
            }
        }
    }
    
    // Get enhanced action selection statistics
    struct MotorSystemStats {
        size_t totalActionsExecuted;
        float averageReward;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        float explorationRate;
        float learningRate;
        float proficiencyLevel;
        std::unordered_map<ActionType, float> actionRewards;
        std::unordered_map<ActionType, float> actionProficiencies;
        
        MotorSystemStats() : totalActionsExecuted(0), averageReward(0.0f), 
                           curiosityLevel(0.0f), noveltyLevel(0.0f), dopamineLevel(0.0f),
                           explorationRate(0.0f), learningRate(0.0f), proficiencyLevel(0.0f) {}
    };
    
    // Get motor system statistics
    MotorSystemStats getStats() const {
        MotorSystemStats stats;
        stats.totalActionsExecuted = actionHistory.size();
        
        // Calculate average reward
        float totalReward = 0.0f;
        size_t rewardCount = 0;
        for (const auto& pair : actionRewards) {
            totalReward += pair.second;
            rewardCount++;
        }
        if (rewardCount > 0) {
            stats.averageReward = totalReward / static_cast<float>(rewardCount);
        }
        
        stats.curiosityLevel = curiosityLevel_;
        stats.noveltyLevel = noveltyLevel_;
        stats.dopamineLevel = dopamineLevel_;
        stats.explorationRate = explorationRate_;
        stats.learningRate = learningRate_;
        
        // Calculate average proficiency
        float totalProficiency = 0.0f;
        size_t proficiencyCount = 0;
        for (const auto& pair : actionProficiencies) {
            totalProficiency += pair.second;
            proficiencyCount++;
        }
        if (proficiencyCount > 0) {
            stats.proficiencyLevel = totalProficiency / static_cast<float>(proficiencyCount);
        }
        
        stats.actionRewards = actionRewards;
        stats.actionProficiencies = actionProficiencies;
        
        return stats;
    }
}; // struct MotorSystem::Impl

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    return pImpl->selectAction(motorRegion);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->getAvailableActions();
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    pImpl->setActionPreference(action, preference);
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->getActionHistory();
}

void MotorSystem::clearHistory() {
    pImpl->clearHistory();
}

// Additional helper methods for enhanced integration
void MotorSystem::integrateWithAgentBrain(AgentBrain* agentBrain) {
    if (pImpl) {
        pImpl->initialize(agentBrain);
    }
}

void MotorSystem::updateSensoryInputs(const SensoryPercept& percept) {
    if (pImpl) {
        pImpl->updateSensoryMotorIntegration(percpt);
    }
}

void MotorSystem::executeActions(const AgentBody& agentBody, const SimpleWorld& world, 
                                double timestep, Timestamp currentTime) {
    if (pImpl) {
        pImpl->executePendingActions(agentBody, world, timestep, currentTime);
    }
}

MotorSystem::MotorSystemStats MotorSystem::getSystemStats() const {
    if (pImpl) {
        return pImpl->getStats();
    }
    return MotorSystemStats();
}

} // namespace nlm
