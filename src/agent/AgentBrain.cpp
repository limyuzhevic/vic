#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <deque>
#include <limits>

namespace nlm {

// Helper function for softmax selection
float softmax(float x) { return std::exp(x) / (1.0f + std::exp(x)); }

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , developmentModulation_(1.0f)
    , predictionDrivenEnabled_(true)
    , metaExplorationActive_(false)
    , metaExplorationStartTime_(0.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , metaExplorationWeight_(0.5f)
    , metaExplorationBaseThreshold_(0.3f)
    , metaExplorationBaseDuration_(60.0f)
    , metaExplorationActivationThreshold_(0.5f)
    , bayesianExplorationBonus_(0.2f)
    , uncertaintyScaling_(1.0f)
    , actionSelectionBaseThreshold_(0.5f)
    , sensoryNoveltyDecay_(0.99f)
    , rng_(nullptr)
    , cachedDevelopmentalStage_(DevelopmentalStage::Initial)
    , totalPredictionsMade_(0)
    , successfulPredictions_(0)
{
    // Initialize random generator reference
    if (brain_) {
        rng_ = brain_->getRandomGenerator();
    }
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    developmentModulation_ = getDevelopmentalModulationFactor();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Prediction-driven action selection methods
void AgentBrain::updateActionSelectionStrategy() {
    if (!brain_) return;
    
    // Update prediction error based on prediction system
    if (brain_->getPredictionSystem()) {
        predictionError_ = brain_->getPredictionSystem()->getPredictionError();
    }
    
    // Update developmental modulation of action selection
    float developmentModulation = getDevelopmentalModulationFactor();
    developmentModulation_ = developmentModulation;
    
    // Update meta-exploration based on prediction error and curiosity
    updateMetaExploration();
}

float AgentBrain::getDevelopmentalModulationFactor() const {
    DevelopmentalStage stage = getDevelopmentalStage();
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity, more exploration
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;  // Still high exploration
        case DevelopmentalStage::Maturation:
            return 0.5f;  // Moderate exploration
        case DevelopmentalStage::Adult:
            return 0.3f;  // Lower exploration, more exploitation
        case DevelopmentalStage::Aging:
            return 0.2f;  // Even lower exploration
        default:
            return 0.5f;
    }
}

MotorCommand AgentBrain::selectActionWithPrediction(const std::vector<float>& currentState) {
    if (!brain_) return MotorCommand::Wait;
    
    // Get predictions from brain's prediction system
    MotorCommand predictedAction = getPredictedAction(currentState);
    
    // Get Bayesian action selection considering uncertainty
    MotorCommand bayesianAction = selectActionBayesian(currentState);
    
    // Get goal-directed action from neural planner
    MotorCommand goalDirectedAction = getGoalDirectedAction(currentState);
    
    // Get concept-based action selection
    MotorCommand conceptAction = getConceptBasedAction(currentState);
    
    // Combine using weighted decision based on prediction confidence
    MotorCommand finalAction = combineActionSelections(
        predictedAction, bayesianAction, goalDirectedAction, conceptAction, currentState
    );
    
    return finalAction;
}

MotorCommand AgentBrain::getPredictedAction(const std::vector<float>& currentState) {
    if (!brain_ || !brain_->getPredictionSystem()) return MotorCommand::Wait;
    
    // Use neural planner's action prediction
    if (brain_->getPlanner()) {
        ActionType actionType = brain_->getPlanner()->planAction(
            currentState, expectedReward_
        );
        return actionTypeToMotorCommand(actionType);
    }
    
    // Fallback to concept-based prediction
    if (brain_->getConceptFormation()) {
        size_t conceptId = brain_->getConceptFormation()->getMatchingConcept(currentState);
        if (conceptId > 0) {
            const DiscoveredConcept* concept = brain_->getConceptFormation()->getConcept(conceptId);
            if (concept && !concept->associatedConceptIds.empty()) {
                // Use associated concepts to predict action
                return predictActionFromConcept(conceptId, currentState);
            }
        }
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectActionBayesian(const std::vector<float>& currentState) {
    if (!brain_) return MotorCommand::Wait;
    
    // Compute action values with uncertainty estimation
    std::vector<std::pair<MotorCommand, float>> actionValues;
    
    for (size_t i = 0; i < 6; ++i) {
        MotorCommand cmd = static_cast<MotorCommand>(i);
        float value = computeActionValue(cmd, currentState);
        float uncertainty = computeActionUncertainty(cmd, currentState);
        
        // Concept-based exploration bonus
        float explorationBonus = getConceptExplorationBonus(cmd, currentState);
        
        // Bayesian adjustment: high uncertainty increases exploration bonus
        float totalBonus = uncertainty * bayesianExplorationBonus_ + explorationBonus;
        
        actionValues.push_back({cmd, value + totalBonus});
    }
    
    // Apply softmax to convert to probabilities
    float totalScore = 0.0f;
    for (const auto& pair : actionValues) {
        totalScore += softmax(pair.second);
    }
    
    // Select action based on probabilities
    float threshold = getRandomGenerator()->uniformReal(0.0f, totalScore);
    float cumulative = 0.0f;
    
    for (const auto& pair : actionValues) {
        cumulative += softmax(pair.second);
        if (threshold <= cumulative) {
            return pair.first;
        }
    }
    
    // Fallback to highest value action
    std::sort(actionValues.begin(), actionValues.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return actionValues.empty() ? MotorCommand::Wait : actionValues[0].first;
}

MotorCommand AgentBrain::getConceptBasedAction(const std::vector<float>& currentState) {
    if (!brain_ || !brain_->getConceptFormation()) return MotorCommand::Wait;
    
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    
    // Get active concepts based on current state
    std::vector<size_t> activeConcepts;
    for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
        size_t conceptId = conceptFormation->getConcept(i)->id;
        float activation = conceptFormation->getConceptActivation(conceptId);
        
        if (activation > 0.5f) {  // Concept is sufficiently active
            activeConcepts.push_back(conceptId);
        }
    }
    
    if (activeConcepts.empty()) return MotorCommand::Wait;
    
    // Evaluate actions based on active concepts
    std::vector<std::pair<MotorCommand, float>> actionScores;
    
    for (size_t i = 0; i < 6; ++i) {
        MotorCommand action = static_cast<MotorCommand>(i);
        float score = 0.0f;
        
        // Sum scores from active concepts
        for (size_t conceptId : activeConcepts) {
            float conceptScore = getConceptActionScore(conceptId, action, currentState);
            score += conceptScore * conceptFormation->getConceptActivation(conceptId);
        }
        
        if (activeConcepts.size() > 0) {
            score /= activeConcepts.size();
        }
        
        actionScores.push_back({action, score});
    }
    
    // Select highest scoring action
    std::sort(actionScores.begin(), actionScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (!actionScores.empty() && actionScores[0].second > 0.5f) {
        return actionScores[0].first;
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::getGoalDirectedAction(const std::vector<float>& currentState) {
    if (!brain_ || !brain_->getPlanner()) return MotorCommand::Wait;
    
    // Get current goal from planner if set
    std::vector<float> goal = brain_->getPlanner()->getCurrentGoal();
    
    if (goal.empty()) {
        return MotorCommand::Wait;
    }
    
    // Use concept-based planning to get goal-directed action
    if (brain_->getConceptFormation()) {
        std::vector<size_t> relevantConcepts;
        
        // Find concepts relevant to current goal
        for (size_t i = 0; i < brain_->getConceptFormation()->getConceptCount(); ++i) {
            const DiscoveredConcept* concept = brain_->getConceptFormation()->getConcept(
                brain_->getConceptFormation()->getConcept(i)->id);
            if (concept && concept->categoryHint == "rewarding") {
                relevantConcepts.push_back(concept->id);
            }
        }
        
        if (!relevantConcepts.empty()) {
            // Return action that maximizes reward for relevant concepts
            std::vector<std::pair<MotorCommand, float>> actionValues;
            for (size_t i = 0; i < 6; ++i) {
                MotorCommand action = static_cast<MotorCommand>(i);
                if (action == MotorCommand::Wait) continue;
                
                float value = 0.0f;
                for (size_t conceptId : relevantConcepts) {
                    float conceptValue = getConceptActionScore(conceptId, action, currentState);
                    value += conceptValue;
                }
                
                actionValues.push_back({action, value / relevantConcepts.size()});
            }
            
            if (!actionValues.empty()) {
                std::sort(actionValues.begin(), actionValues.end(),
                          [](const auto& a, const auto& b) { return a.second > b.second; });
                return actionValues[0].first;
            }
        }
    }
    
    // Fallback to traditional goal-directed action selection
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::combineActionSelections(
    MotorCommand predAction, MotorCommand bayesianAction, MotorCommand goalAction,
    MotorCommand conceptAction, const std::vector<float>& currentState) {
    
    // Collect all valid non-Wait actions
    std::vector<MotorCommand> candidateActions;
    std::vector<float> actionWeights;
    
    if (predAction != MotorCommand::Wait) {
        candidateActions.push_back(predAction);
        actionWeights.push_back(0.4f);  // Prediction weight
    }
    if (bayesianAction != MotorCommand::Wait) {
        candidateActions.push_back(bayesianAction);
        actionWeights.push_back(0.3f);  // Bayesian weight
    }
    if (goalAction != MotorCommand::Wait) {
        candidateActions.push_back(goalAction);
        actionWeights.push_back(0.2f);  // Goal-directed weight
    }
    if (conceptAction != MotorCommand::Wait) {
        candidateActions.push_back(conceptAction);
        actionWeights.push_back(0.1f);  // Concept-based weight
    }
    
    if (candidateActions.empty()) return MotorCommand::Wait;
    
    // If only one candidate, use it
    if (candidateActions.size() == 1) return candidateActions[0];
    
    // Choose based on prediction confidence and development stage
    float predConfidence = (brain_ && brain_->getPredictionSystem()) ? 
        brain_->getPredictionSystem()->getConfidence() : 0.5f;
    
    // Normalize weights
    float totalWeight = std::accumulate(actionWeights.begin(), actionWeights.end(), 0.0f);
    if (totalWeight > 0.0f) {
        for (float& weight : actionWeights) {
            weight /= totalWeight;
        }
    }
    
    // Compute scores
    float predScore = predConfidence;
    float bayesianScore = 1.0f - getPredictionError(); // Lower error = higher score
    float goalScore = computeActionValue(goalAction, currentState);
    float conceptScore = computeConceptActionScore(conceptAction, currentState);
    
    // Weighted average
    float finalScore = predScore * actionWeights[0] + 
                      bayesianScore * actionWeights[1] + 
                      goalScore * actionWeights[2] + 
                      conceptScore * actionWeights[3];
    
    // Threshold for action selection
    float actionThreshold = getActionSelectionThreshold();
    
    if (finalScore < actionThreshold && metaExplorationActive_) {
        // Use meta-exploration: select least certain action
        return selectLeastCertainAction(candidateActions, currentState);
    }
    
    // Select highest scoring action
    MotorCommand bestAction = candidateActions[0];
    float bestScore = 0.0f;
    
    for (size_t i = 0; i < candidateActions.size(); ++i) {
        MotorCommand action = candidateActions[i];
        float score = (i == 0) ? predScore : 
                     (i == 1) ? bayesianScore : 
                     (i == 2) ? goalScore : conceptScore;
        
        score *= actionWeights[i];
        
        if (score > bestScore) {
            bestScore = score;
            bestAction = action;
        }
    }
    
    return bestAction;
}

float AgentBrain::computeActionValue(MotorCommand action, const std::vector<float>& state) {
    if (!brain_) return 0.0f;
    
    // Get action quality from neural planner
    if (brain_->getPlanner()) {
        ActionType actionType = actionTypeToAction(action);
        size_t actionIdx = static_cast<size_t>(actionType);
        if (actionIdx < brain_->getPlanner()->getActionQualitySize()) {
            return brain_->getPlanner()->getActionQuality(actionIdx);
        }
    }
    
    // Fallback to motor neuron activity
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    std::vector<Neuron*> neurons;
    switch (action) {
        case MotorCommand::MoveForward: neurons = motorForward_; break;
        case MotorCommand::MoveBackward: neurons = motorBackward_; break;
        case MotorCommand::TurnLeft: neurons = motorTurnLeft_; break;
        case MotorCommand::TurnRight: neurons = motorTurnRight_; break;
        case MotorCommand::Interact: neurons = motorInteract_; break;
        case MotorCommand::Wait: neurons = motorWait_; break;
        default: return 0.0f;
    }
    
    return calcActivity(neurons);
}

float AgentBrain::computeActionUncertainty(MotorCommand action, const std::vector<float>& state) {
    if (!brain_ || !brain_->getEpisodicMemory()) return 0.0f;
    
    // Get prediction error for this action from episodic memory
    // Simplified: use overall prediction error as proxy
    float uncertainty = std::abs(predictionError_) * uncertaintyScaling_;
    
    // Check development stage - young agents have higher uncertainty
    float developmentUncertainty = 1.0f - developmentModulation_;
    
    return std::min(1.0f, uncertainty + developmentUncertainty);
}

float AgentBrain::computeConceptExplorationBonus(MotorCommand action, const std::vector<float>& state) {
    if (!brain_ || !brain_->getConceptFormation()) return 0.0f;
    
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    
    // Calculate exploration bonus based on novel concept activation
    float explorationBonus = 0.0f;
    
    for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
        size_t conceptId = conceptFormation->getConcept(i)->id;
        float activation = conceptFormation->getConceptActivation(conceptId);
        
        if (activation > 0.3f && activation < 0.7f) {  // Concept is moderately active
            // Moderate activation suggests exploration opportunity
            explorationBonus += (0.7f - activation) * 0.2f;
        }
    }
    
    return explorationBonus;
}

float AgentBrain::getConceptActionScore(size_t conceptId, MotorCommand action, const std::vector<float>& state) {
    if (!brain_ || !brain_->getConceptFormation()) return 0.0f;
    
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    const DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
    
    if (!concept) return 0.0f;
    
    // Score based on concept category and action compatibility
    float score = 0.0f;
    
    if (concept->categoryHint == "rewarding") {
        score = 0.8f;  // Rewarding concepts promote approach actions
    } else if (concept->categoryHint == "harmful") {
        score = 0.2f;  // Harmful concepts discourage approach actions
    } else {
        score = 0.5f;  // Neutral concepts are neutral
    }
    
    // Adjust score based on action type
    switch (action) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            score *= 1.0f;  // Movement actions
            break;
        case MotorCommand::Interact:
            score *= 0.8f;  // Interaction actions
            break;
        case MotorCommand::Wait:
            score *= 0.3f;  // Wait actions
            break;
    }
    
    return score;
}

float AgentBrain::computeConceptActionScore(MotorCommand action, const std::vector<float>& state) {
    if (!brain_ || !brain_->getConceptFormation()) return 0.0f;
    
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    
    // Find most active concept
    size_t mostActiveConcept = 0;
    float maxActivation = 0.0f;
    
    for (size_t i = 0; i < conceptFormation->getConceptCount(); ++i) {
        size_t conceptId = conceptFormation->getConcept(i)->id;
        float activation = conceptFormation->getConceptActivation(conceptId);
        
        if (activation > maxActivation) {
            maxActivation = activation;
            mostActiveConcept = conceptId;
        }
    }
    
    if (mostActiveConcept == 0) return 0.0f;
    
    return getConceptActionScore(mostActiveConcept, action, state);
}

MotorCommand AgentBrain::predictActionFromConcept(size_t conceptId, const std::vector<float>& currentState) {
    if (!brain_ || !brain_->getConceptFormation()) return MotorCommand::Wait;
    
    ConceptFormation* conceptFormation = brain_->getConceptFormation();
    const DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
    
    if (!concept) return MotorCommand::Wait;
    
    // Predict action based on concept properties
    // If concept is "rewarding", predict approach actions
    // If concept is "harmful", predict avoidance actions
    
    if (concept->categoryHint == "rewarding") {
        // Randomly choose between approach actions
        int choice = rng_ ? rng_->uniformInt(0, 3) : 0;
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::TurnLeft;
            case 2: return MotorCommand::TurnRight;
            default: return MotorCommand::MoveForward;
        }
    } else if (concept->categoryHint == "harmful") {
        // Randomly choose between avoidance actions
        int choice = rng_ ? rng_->uniformInt(0, 3) : 0;
        switch (choice) {
            case 0: return MotorCommand::MoveBackward;
            case 1: return MotorCommand::TurnLeft;
            case 2: return MotorCommand::TurnRight;
            default: return MotorCommand::MoveBackward;
        }
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectLeastCertainAction(
    const std::vector<MotorCommand>& actions, const std::vector<float>& state) {
    
    MotorCommand leastCertain = actions[0];
    float leastCertainValue = std::numeric_limits<float>::max();
    
    for (MotorCommand action : actions) {
        float uncertainty = computeActionUncertainty(action, state);
        if (uncertainty < leastCertainValue) {
            leastCertainValue = uncertainty;
            leastCertain = action;
        }
    }
    
    return leastCertain;
}

float AgentBrain::getActionSelectionThreshold() const {
    // Threshold adapts with development and prediction error
    float errorFactor = std::abs(predictionError_);
    float devFactor = developmentModulation_;
    
    return actionSelectionBaseThreshold_ * (1.0f + errorFactor) * (1.0f - devFactor);
}

ActionType AgentBrain::actionTypeToAction(MotorCommand motorCmd) const {
    switch (motorCmd) {
        case MotorCommand::MoveForward: return ActionType::MoveForward;
        case MotorCommand::MoveBackward: return ActionType::MoveBackward;
        case MotorCommand::TurnLeft: return ActionType::TurnLeft;
        case MotorCommand::TurnRight: return ActionType::TurnRight;
        case MotorCommand::Interact: return ActionType::Interact;
        case MotorCommand::Wait: return ActionType::Wait;
        default: return ActionType::Wait;
    }
}

MotorCommand AgentBrain::actionTypeToMotorCommand(ActionType actionType) const {
    switch (actionType) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::MoveLeft: return MotorCommand::TurnLeft; // Simplified mapping
        case ActionType::MoveRight: return MotorCommand::TurnRight; // Simplified mapping
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::Look: return MotorCommand::Wait; // Placeholder
        case ActionType::LookUp: return MotorCommand::Wait; // Placeholder
        case ActionType::LookDown: return MotorCommand::Wait; // Placeholder
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Eat: return MotorCommand::Wait; // Placeholder
        case ActionType::Drink: return MotorCommand::Wait; // Placeholder
        case ActionType::Rest: return MotorCommand::Wait; // Placeholder
        case ActionType::Wait: return MotorCommand::Wait;
        case ActionType::Custom: return MotorCommand::Wait; // Placeholder
        default: return MotorCommand::Wait;
    }
}

float AgentBrain::getRandomFloat() const {
    return rng_ ? rng_->uniformReal(0.0f, 1.0f) : 0.5f;
}

std::vector<float> AgentBrain::getCurrentState() const {
    if (!brain_ || !brain_->getWorkingMemory()) {
        return std::vector<float>(256, 0.0f); // Default vision size
    }
    
    return brain_->getWorkingMemory()->getCurrentPattern();
}

void AgentBrain::updateMetaExploration() {
    if (!brain_ || !curiosityEnabled_) return;
    
    // Meta-exploration uses prediction error to guide exploration
    // High prediction error = uncertain -> more exploratory
    float predictionErrorMagnitude = std::abs(predictionError_);
    
    // Combine prediction error with curiosity for meta-exploration
    float explorationDrive = predictionErrorMagnitude * curiosityLevel_ * metaExplorationWeight_;
    
    // Adaptive meta-exploration threshold based on development
    float explorationThreshold = metaExplorationBaseThreshold_ * (1.0f - developmentModulation_);
    
    if (explorationDrive > explorationThreshold && !metaExplorationActive_) {
        // Activate meta-exploration mode
        metaExplorationActive_ = true;
        metaExplorationStartTime_ = developmentalAge_;
        NLM_LOG_INFO("Meta-exploration activated");
    }
    
    if (metaExplorationActive_) {
        // In meta-exploration mode, use Bayesian action selection
        // Duration depends on developmental stage
        float metaExplorationDuration = metaExplorationBaseDuration_ * (1.0f + developmentModulation_);
        if (developmentalAge_ - metaExplorationStartTime_ > metaExplorationDuration) {
            metaExplorationActive_ = false;
            NLM_LOG_INFO("Meta-exploration deactivated");
        }
    }
}

float AgentBrain::getDevelopmentalModulationFactor() const {
    DevelopmentalStage stage = getDevelopmentalStage();
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity, more exploration
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;  // Still high exploration
        case DevelopmentalStage::Maturation:
            return 0.5f;  // Moderate exploration
        case DevelopmentalStage::Adult:
            return 0.3f;  // Lower exploration, more exploitation
        case DevelopmentalStage::Aging:
            return 0.2f;  // Even lower exploration
        default:
            return 0.5f;
    }
}

void AgentBrain::updateSelfModel(MotorCommand action, const std::vector<float>& beforeState,
                               const std::vector<float>& afterState) {
    // Update self-model (forward model) with new experience
    // This helps predict consequences of actions
    
    // In a full implementation, this would update the brain's SelfModel
    // For now, just log
    NLM_LOG_INFO("Self-model updated for action: " + std::to_string(static_cast<int>(action)));
}

void AgentBrain::recordActionOutcome(MotorCommand action, const std::vector<float>& beforeState,
                                     const std::vector<float>& afterState, float reward) {
    // Record in episodic memory for future prediction
    if (brain_ && brain_->getEpisodicMemory()) {
        // This would record the action-outcome pair
        // For now, just log
        NLM_LOG_INFO("Recorded action outcome: " + std::to_string(static_cast<int>(action)) +
                     " reward: " + std::to_string(reward));
    }
    
    // Update neural planner with outcome
    if (brain_ && brain_->getPlanner()) {
        brain_->getPlanner()->updatePlanQuality(
            std::vector<ActionType>(), // Would need planned actions
            std::vector<ActionType>(), // Would need actual actions  
            reward
        );
    }
    
    // Update concept formation with outcome
    if (brain_ && brain_->getConceptFormation() && !beforeState.empty()) {
        brain_->getConceptFormation()->updateConcept(
            brain_->getConceptFormation()->getMatchingConcept(beforeState),
            afterState, std::vector<float>(afterState.size(), 1.0f), reward);
    }
}

void AgentBrain::enablePredictionDrivenActionSelection(bool enable) {
    predictionDrivenEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Prediction-driven action selection enabled");
    }
}

bool AgentBrain::isPredictionDrivenActionSelectionEnabled() const {
    return predictionDrivenEnabled_;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Process through concept formation system
    if (brain_->getConceptFormation()) {
        std::vector<float> pattern = vision;
        std::vector<float> features;
        if (!touch.empty()) {
            features.insert(features.end(), touch.begin(), touch.end());
        }
        if (!intern.empty()) {
            features.insert(features.end(), intern.begin(), intern.end());
        }
        if (!proprio.empty()) {
            features.insert(features.end(), proprio.begin(), proprio.end());
        }
        
        size_t conceptId = brain_->getConceptFormation()->presentExperience(
            pattern, features, curiosityLevel_, developmentalAge_);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Update action selection strategy based on current state
    updateActionSelectionStrategy();
    
    std::vector<float> currentState = getCurrentState();
    
    MotorCommand predictedAction;
    
    // Use prediction-driven action selection if enabled
    if (predictionDrivenEnabled_) {
        predictedAction = selectActionWithPrediction(currentState);
    } else {
        // Fallback to traditional motor decoding
        predictedAction = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            predictedAction = selectWithCuriosity(predictedAction);
        }
    }
    
    // Apply concept-based action selection
    if (brain_->getConceptFormation() && curiosityLevel_ > 0.5f) {
        // Use concept-based action selection when curiosity is high
        MotorCommand conceptAction = getConceptBasedAction(currentState);
        if (conceptAction != MotorCommand::Wait) {
            return conceptAction;
        }
    }
    
    return predictedAction;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    }
    
    // === ADVANCED SLEEP/REST CYCLE INTEGRATION ===
    
    // Check if agent should enter rest state based on activity levels
    updateRestState();
    
    // If in rest state, integrate with brain sleep mechanisms
    if (isResting_) {
        // Send rest signals to brain for sleep-based replay
        sendRestSignals();
        
        // Perform offline consolidation during rest
        performOfflineConsolidation();
        
        // Update metaplasticity during rest
        if (brain_ && brain_->getMetaplasticity()) {
            brain_->getMetaplasticity()->integrateNeuromodulation(
                createSleepNeuromodulatorSignal()
            );
        }
    }
    
    // Reset rest timer if agent is active
    if (brain_ && brain_->getWorkingMemory() && 
        !brain_->getWorkingMemory()->getMemoryNeurons().empty()) {
        resetRestTimer();
        isResting_ = false;
    } else if (!isResting_ && restTimer_ > 5000) {  // Enter rest after 5000 steps
        enterRestState();
        isResting_ = true;
    }
    
    // Apply sleep-induced metaplasticity for adaptation
    applySleepMetaplasticity();
}

// ========== SLEEP/REST CYCLE MANAGEMENT ==========

void AgentBrain::updateRestState() {
    // Check if agent is currently active (has working memory)
    bool isActive = false;
    if (brain_ && brain_->getWorkingMemory()) {
        isActive = !brain_->getWorkingMemory()->getMemoryNeurons().empty();
    }
    
    // Update rest timer based on activity
    if (isActive) {
        resetRestTimer();
        isResting_ = false;
    } else {
        restTimer_++;
        if (restTimer_ >= REST_ENTRY_THRESHOLD && !isResting_) {
            enterRestState();
            isResting_ = true;
        }
    }
}

void AgentBrain::enterRestState() {
    NLM_LOG_INFO("Agent entering rest state for offline consolidation");
    isResting_ = true;
    restStartTime_ = std::chrono::steady_clock::now();
    
    // Notify brain about rest state entry
    sendRestSignals();
    
    // Perform immediate consolidation
    performOfflineConsolidation();
}

void AgentBrain::exitRestState() {
    NLM_LOG_INFO("Agent exiting rest state");
    isResting_ = false;
    
    // Reset rest-related timers
    restTimer_ = 0;
    
    // Notify brain about rest state exit
    sendWakeSignals();
}

void AgentBrain::sendRestSignals() {
    if (!brain_) return;
    
    // Create and send rest neuromodulator signal
    Neuromodulator restSignal;
    restSignal.type = NeuromodulatorType::Acetylcholine;
    restSignal.level = 0.8f;  // High acetylcholine during rest/sleep
    restSignal.plasticityFactor = 1.5f;  // Enhanced plasticity during rest
    
    // Apply to brain
    brain_->applyNeuromodulation(restSignal);
}

void AgentBrain::sendWakeSignals() {
    if (!brain_) return;
    
    // Create and send wake neuromodulator signal
    Neuromodulator wakeSignal;
    wakeSignal.type = NeuromodulatorType::Norepinephrine;
    wakeSignal.level = 0.6f;  // Moderate norepinephrine during wake
    wakeSignal.plasticityFactor = 0.8f;  // Reduced plasticity during wake
    
    // Apply to brain
    brain_->applyNeuromodulation(wakeSignal);
}

void AgentBrain::performOfflineConsolidation() {
    if (!brain_) return;
    
    // Perform offline consolidation of working memory
    if (brain_->getWorkingMemory()) {
        // Strengthen recent memory traces
        brain_->getWorkingMemory()->strengthenMemory(1.2f);
    }
    
    // Consolidate episodic memory
    if (brain_->getEpisodicMemory()) {
        // Get recent episodes for consolidation
        auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(10);
        
        // Apply consolidation to recent episodes
        for (const auto* episode : recentEpisodes) {
            brain_->getEpisodicMemory()->updateRelevance(
                episode->timestamp, 0.1f  // Small relevance boost
            );
        }
    }
    
    // Update metaplasticity based on offline learning
    if (brain_->getMetaplasticity()) {
        // Simulate offline metaplastic changes
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Apply metaplastic threshold adjustments
                    neuron->adjustMetaplasticThreshold(0.05f);
                }
            }
        }
    }
}

void AgentBrain::applySleepMetaplasticity() {
    if (!brain_ || !isResting_) return;
    
    // Apply sleep-induced metaplastic effects for learning adaptation
    auto* metaplasticity = brain_->getMetaplasticity();
    if (!metaplasticity) return;
    
    // Get plasticity budget from metaplasticity manager
    float remainingBudget = metaplasticity->getRemainingPlasticityBudget();
    
    if (remainingBudget > 0.0f) {
        // Allocate budget for sleep-based metaplastic changes
        float metaplasticBudget = std::min(remainingBudget, 0.5f);
        
        // Apply metaplastic threshold changes to enhance learning adaptation
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Increase plasticity thresholds for better adaptation
                    neuron->setMetaplasticThreshold(
                        neuron->getMetaplasticThreshold() * 1.1f
                    );
                    
                    // Track metaplastic changes
                    metaplasticThresholdHistory_.push_back(
                        neuron->getMetaplasticThreshold()
                    );
                }
            }
        }
        
        // Release plasticity budget
        metaplasticity->releasePlasticityBudget(metaplasticBudget, INVALID_SYNAPSE_ID);
    }
}

Neuromodulator AgentBrain::createSleepNeuromodulatorSignal() {
    Neuromodulator sleepSignal;
    sleepSignal.type = NeuromodulatorType::Acetylcholine;
    sleepSignal.level = 0.8f;
    sleepSignal.plasticityFactor = 1.5f;
    return sleepSignal;
}

void AgentBrain::resetRestTimer() {
    if (restTimer_ > REST_TIMER_RESET) {
        restTimer_ = 0;
        isResting_ = false;
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    developmentModulation_ = 1.0f;
    metaExplorationActive_ = false;
    metaExplorationStartTime_ = 0.0f;
    
    // Clear prediction history
    predictionErrorHistory_.clear();
    rewardHistory_.clear();
    actionOutcomePredictions_.clear();
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
