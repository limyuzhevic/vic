#include "MotorAction.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

// Advanced action selector using working memory, attention, and planning
class AdvancedActionSelector {
public:
    AdvancedActionSelector() = default;
    ~AdvancedActionSelector() = default;
    
    void initialize(Brain* brain) {
        brain_ = brain;
        workingMemory_ = brain ? brain->getWorkingMemory() : nullptr;
        attention_ = brain ? brain->getAttention() : nullptr;
        planner_ = brain ? brain->getPlanner() : nullptr;
        dopamine_ = brain ? brain->getDopamine() : nullptr;
        curiosity_ = brain ? brain->getCuriosity() : nullptr;
        predictionError_ = brain ? brain->getPredictionErrorSignal() : nullptr;
    }
    
    // Select best action based on multiple factors
    std::unique_ptr<MotorAction> selectAction() {
        std::vector<std::unique_ptr<MotorAction>> candidateActions;
        
        // Create basic actions
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::MoveForward));
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::MoveBackward));
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::TurnLeft));
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::TurnRight));
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::Interact));
        candidateActions.push_back(ActionFactory::createAction(MotorCommand::Wait));
        
        // Calculate action scores based on various factors
        std::vector<std::pair<std::unique_ptr<MotorAction>, float>> scoredActions;
        
        for (auto& action : candidateActions) {
            float score = calculateActionScore(action.get());
            scoredActions.emplace_back(std::move(action), score);
        }
        
        // Sort by score (highest first)
        std::sort(scoredActions.begin(), scoredActions.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Apply attention mechanism to narrow down options
        std::vector<std::pair<std::unique_ptr<MotorAction>, float>> attendedActions;
        if (attention_) {
            // Get attention focus
            auto winners = attention_->getWinners();
            
            // Filter actions based on attention
            for (auto& scored : scoredActions) {
                bool isAttended = false;
                auto& action = scored.first;
                
                // Check if action matches attended neurons
                if (auto motorAction = dynamic_cast<MoveForwardAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::MoveForward, winners);
                } else if (auto motorAction = dynamic_cast<MoveBackwardAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::MoveBackward, winners);
                } else if (auto motorAction = dynamic_cast<TurnLeftAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::TurnLeft, winners);
                } else if (auto motorAction = dynamic_cast<TurnRightAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::TurnRight, winners);
                } else if (auto motorAction = dynamic_cast<InteractAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::Interact, winners);
                } else if (auto motorAction = dynamic_cast<WaitAction*>(action.get())) {
                    isAttended = isAttended || checkAttentionForCommand(MotorCommand::Wait, winners);
                }
                
                if (isAttended) {
                    attendedActions.push_back(std::move(scored));
                }
            }
            
            // Use attended actions if any were found
            if (!attendedActions.empty()) {
                scoredActions = std::move(attendedActions);
            }
        }
        
        // Apply neuromodulation to action selection
        if (dopamine_ || curiosity_ || predictionError_) {
            scoredActions = applyNeuromodulationToScores(scoredActions);
        }
        
        // Apply planning if available
        if (planner_) {
            // Get planned actions from neural planner
            auto plannedActions = planner_->getPlan();
            if (!plannedActions.empty()) {
                // Filter scored actions to include only planned ones
                std::vector<std::pair<std::unique_ptr<MotorAction>, float>> filteredActions;
                for (auto& scored : scoredActions) {
                    for (const auto& planned : plannedActions) {
                        if (actionMatchesCommand(scored.first.get(), planned.command)) {
                            filteredActions.push_back(std::move(scored));
                            break;
                        }
                    }
                }
                if (!filteredActions.empty()) {
                    scoredActions = std::move(filteredActions);
                }
            }
        }
        
        // Select the best action
        if (!scoredActions.empty()) {
            return std::move(scoredActions[0].first);
        }
        
        // Fallback to wait action
        return ActionFactory::createAction(MotorCommand::Wait);
    }
    
private:
    float calculateActionScore(MotorAction* action) {
        float baseScore = action->getPriority();
        
        if (!brain_ || !workingMemory_) return baseScore;
        
        // Modify score based on working memory
        auto memoryNeurons = workingMemory_->getMemoryNeurons();
        float memoryScore = 0.0f;
        
        // Check if action's neural targets are in working memory
        if (dynamic_cast<MoveForwardAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::MoveForward, memoryNeurons);
        } else if (dynamic_cast<MoveBackwardAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::MoveBackward, memoryNeurons);
        } else if (dynamic_cast<TurnLeftAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::TurnLeft, memoryNeurons);
        } else if (dynamic_cast<TurnRightAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::TurnRight, memoryNeurons);
        } else if (dynamic_cast<InteractAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::Interact, memoryNeurons);
        } else if (dynamic_cast<WaitAction*>(action)) {
            memoryScore = checkWorkingMemoryForCommand(MotorCommand::Wait, memoryNeurons);
        }
        
        baseScore += memoryScore * 0.5f; // Working memory can boost score significantly
        
        // Modify score based on neuromodulation
        if (dopamine_) {
            float dopamine = dopamine_->getLevel();
            baseScore += dopamine * 0.3f; // Positive dopamine boosts action selection
        }
        
        if (curiosity_) {
            float curiosity = curiosity_->getLevel();
            if (curiosity > 0.5f) {
                baseScore += 0.2f; // High curiosity encourages exploration
            }
        }
        
        if (predictionError_) {
            float error = predictionError_->getLevel();
            baseScore += std::abs(error) * 0.2f; // Prediction error can drive learning actions
        }
        
        return baseScore;
    }
    
    bool checkAttentionForCommand(MotorCommand cmd, const std::vector<NeuronId>& winners) {
        // Simplified implementation: check if any winner neuron corresponds to this command
        for (const auto& winner : winners) {
            // In a real implementation, we'd have a mapping from neurons to commands
            // For now, just return true for some winners
            if (winner.index() % 3 == 0) return true;
        }
        return false;
    }
    
    bool checkWorkingMemoryForCommand(MotorCommand cmd, const std::vector<NeuronId>& memoryNeurons) {
        // Simplified implementation: check if any memory neuron corresponds to this command
        for (const auto& neuron : memoryNeurons) {
            // In a real implementation, we'd have a mapping from neurons to commands
            // For now, just return true for some neurons
            if (neuron.index() % 4 == 0) return true;
        }
        return false;
    }
    
    bool actionMatchesCommand(MotorAction* action, MotorCommand cmd) {
        // Simplified implementation
        switch (cmd) {
            case MotorCommand::MoveForward:
                return dynamic_cast<MoveForwardAction*>(action) != nullptr;
            case MotorCommand::MoveBackward:
                return dynamic_cast<MoveBackwardAction*>(action) != nullptr;
            case MotorCommand::TurnLeft:
                return dynamic_cast<TurnLeftAction*>(action) != nullptr;
            case MotorCommand::TurnRight:
                return dynamic_cast<TurnRightAction*>(action) != nullptr;
            case MotorCommand::Interact:
                return dynamic_cast<InteractAction*>(action) != nullptr;
            case MotorCommand::LookLeft:
            case MotorCommand::LookRight:
            case MotorCommand::Wait:
            default:
                return dynamic_cast<WaitAction*>(action) != nullptr;
        }
    }
    
    std::vector<std::pair<std::unique_ptr<MotorAction>, float>> applyNeuromodulationToScores(
        std::vector<std::pair<std::unique_ptr<MotorAction>, float>>& scoredActions) {
        
        // Dopamine affects action selection strength
        if (dopamine_) {
            float dopamine = dopamine_->getLevel();
            if (dopamine > 0.5f) {
                // High dopamine increases scores of all actions
                for (auto& scored : scoredActions) {
                    scored.second *= (1.0f + dopamine * 0.5f);
                }
            } else if (dopamine < -0.5f) {
                // Low dopamine decreases scores, may cause action paralysis
                for (auto& scored : scoredActions) {
                    scored.second *= (0.5f + dopamine * 0.5f);
                }
            }
        }
        
        // Curiosity increases exploration (adds randomness to scores)
        if (curiosity_) {
            float curiosity = curiosity_->getLevel();
            if (curiosity > 0.7f) {
                // High curiosity adds significant randomness
                for (auto& scored : scoredActions) {
                    float randomFactor = brain_->getRandomGenerator()->uniformReal(0.8f, 1.2f);
                    scored.second *= randomFactor;
                }
            }
        }
        
        return scoredActions;
    }
    
    Brain* brain_ = nullptr;
    NeuralWorkingMemory* workingMemory_ = nullptr;
    AttentionalSelection* attention_ = nullptr;
    NeuralPlanner* planner_ = nullptr;
    Dopamine* dopamine_ = nullptr;
    Curiosity* curiosity_ = nullptr;
    PredictionError* predictionError_ = nullptr;
};

} // namespace nlm
