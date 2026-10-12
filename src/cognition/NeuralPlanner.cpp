#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    PredictionSystem* predictionSystem;
    ConceptFormation* conceptFormation;
    
    // Action value distributions for Bayesian planning
    std::vector<float> actionMeans;
    std::vector<float> actionVariances;
    std::vector<size_t> actionObservationCounts;
    
    // Planning statistics
    std::deque<float> planningRewards;
    std::deque<float> planningErrors;
    
    // Neural planning dynamics
    struct PlanningNeuron {
        NeuronId id;
        std::vector<float> weights;
        std::vector<float> activationHistory;
        SimulationStep lastActive;
        
        PlanningNeuron() : id(INVALID_NEURON_ID), lastActive(-1) {}
    };
    
    std::vector<PlanningNeuron> planningNeurons;
    
    // Integration with prediction errors
    std::vector<float> predictionErrorHistory;
    
    Impl() : brain(nullptr), predictionSystem(nullptr), conceptFormation(nullptr) {}
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , predictionSystem_(nullptr)
    , conceptFormation_(nullptr)
    , errorSignal_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
    , hierarchicalPlanningEnabled_(false)
    , explorationRate_(0.1f)
    , epsilonGreedyThreshold_(0.3f)
    , lastPlanningUpdate_(-1.0f) {}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain, PredictionSystem* predictionSystem,
                               ConceptFormation* conceptFormation) {
    pImpl->brain = brain;
    pImpl->predictionSystem = predictionSystem;
    pImpl->conceptFormation = conceptFormation;
    brain_ = brain;
    predictionSystem_ = predictionSystem;
    conceptFormation_ = conceptFormation;
    
    // Initialize action value distributions
    actionValueDistributions_ = ActionValueDistribution(10);  // 10 action types
    
    // Initialize planning neurons using brain's neural substrate
    initializePlanningNeurons();
    
    NLM_LOG_INFO("NeuralPlanner initialized with Bayesian planning and neural integration");
}

void NeuralPlanner::initializePlanningNeurons() {
    if (!brain_) return;
    
    pImpl->planningNeurons.clear();
    
    // Create planning-specific region
    RegionId planningRegionId = brain_->addRegion("Planning_Dynamics");
    auto* region = brain_->getRegion(planningRegionId);
    
    if (region) {
        // Create planning neuron population with diverse initial states
        PopulationId planningPopId = region->addPopulation(40, NeuronType::Internal);
        auto* planningPop = region->getPopulation(planningPopId);
        
        if (planningPop) {
            for (size_t i = 0; i < planningPop->getNeurons().size(); ++i) {
                auto* neuron = planningPop->getNeurons()[i];
                
                // Set neuron properties for planning dynamics
                neuron->setThreshold(-30.0f);
                neuron->setRestingPotential(-65.0f);
                neuron->setResetPotential(-65.0f);
                neuron->setLeakConductance(4.0f);
                
                // Enable plasticity for planning
                neuron->enablePlasticity(true, true, true);
                
                // Initialize with diverse state for planning
                neuron->initializeRandom(*brain_->getRandomGenerator());
                
                // Store planning neuron
                pImpl->planningNeurons.push_back(PlanningNeuron{neuron->getId()});
            }
        }
    }
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                     float targetReward,
                                     SimulationStep currentTime) {
    
    // Update action value distributions with latest experience
    updateActionValueDistributions(currentTime);
    
    // Generate candidate action sequences using neural dynamics
    auto sequences = generateActionSequences(planningDepth_, currentState);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    // Evaluate each sequence using Bayesian planning
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState, currentTime);
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update planning confidence based on prediction accuracy
    if (!sequences.empty()) {
        planningConfidence_ = best.confidence;
    }
    
    // Return first action of best sequence
    if (!best.actions.empty()) {
        // Record success/failure based on whether we achieved target
        bool success = best.expectedReward >= targetReward;
        pImpl->planningRewards.push_back(success ? 1.0f : 0.0f);
        
        if (pImpl->planningRewards.size() > 100) {
            pImpl->planningRewards.pop_front();
        }
        
        return best.actions[0];
    }
    
    // Use epsilon-greedy with exploration bonus
    if (rand() / (RAND_MAX + 1.0) < explorationRate_) {
        // Explore: select random action
        std::vector<ActionType> allActions = {
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Interact, ActionType::Wait
        };
        return allActions[rand() % allActions.size()];
    }
    
    // Default: wait if no good option
    return ActionType::Wait;
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                   const std::vector<float>& startState,
                                                   SimulationStep currentTime) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = actions.size();
    
    std::vector<float> currentState = startState;
    float totalReward = 0.0f;
    float confidence = 1.0f;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType action = actions[i];
        
        // Evaluate this action using Bayesian planning
        float actionValue = evaluateAction(action, currentState, currentTime);
        
        // Predict next state using forward model
        auto predictedNext = predictNextState(action, currentState, currentTime);
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
            
            // Increase confidence if prediction matches state
            float predictionConfidence = computePredictionConfidence(currentState, predictedNext);
            confidence *= predictionConfidence;
        }
        
        // Get action value from Bayesian distribution
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionValueDistributions_.meanValues.size()) {
            float bayesianValue = actionValueDistributions_.meanValues[actionIdx];
            float variance = actionValueDistributions_.variances[actionIdx];
            
            // Use weighted average of experience and prediction
            float combinedValue = bayesianValue * 0.7f + actionValue * 0.3f;
            
            // Reduce variance based on more observations
            float confidenceFactor = std::min(1.0f, static_cast<float>(actionValueDistributions_.observationCounts[actionIdx]) / 10.0f);
            float effectiveValue = combinedValue + (1.0f - confidenceFactor) * (rand() / (RAND_MAX + 1.0f - 0.2f) - 0.1f);
            
            totalReward += effectiveValue;
        }
        
        // Reduce confidence if we're uncertain
        if (errorSignal_) {
            float modulation = errorSignal_->getModulationSignal();
            confidence *= (1.0f - modulation * 0.2f);
        }
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = std::max(0.0f, confidence);
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth,
                                                                const std::vector<float>& state) {
    std::vector<std::vector<ActionType>> result;
    
    // Get action preferences from neural dynamics
    std::vector<ActionType> preferredActions = getPreferredActions(state);
    
    // Generate sequences focused on preferred actions first
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            // If we have preferred actions, use them more frequently
            if (rand() % 3 == 0 && !preferredActions.empty()) {
                // Sometimes use preferred actions
                ActionType action = preferredActions[rand() % preferredActions.size()];
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
                
                // Also try random actions for exploration
                std::vector<ActionType> allActions = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                action = allActions[rand() % allActions.size()];
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
            } else {
                // Normal random generation
                std::vector<ActionType> allActions = {
                    ActionType::MoveForward, ActionType::MoveBackward,
                    ActionType::TurnLeft, ActionType::TurnRight,
                    ActionType::Interact, ActionType::Wait
                };
                for (ActionType action : allActions) {
                    current.push_back(action);
                    generate(current, remaining - 1);
                    current.pop_back();
                }
            }
        };
    
    std::vector<ActionType> current;
    generate(current, depth);
    
    // Limit number of sequences for efficiency
    if (result.size() > 200) {
        result.resize(200);
    }
    
    return result;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state,
                                    SimulationStep currentTime) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionValueDistributions_.meanValues.size()) return 0.0f;
    
    // Get baseline value from action value distribution
    float baselineValue = actionValueDistributions_.meanValues[actionIdx];
    
    // Get value from neural prediction system if available
    float neuralValue = 0.0f;
    if (predictionSystem_ && conceptFormation_) {
        // Use neural prediction to evaluate action consequences
        auto predictionResult = predictionSystem_->predictNextState(
            SensoryInput(state), currentTime);
        neuralValue = predictionResult.second;  // Error component
    }
    
    // Bonus for approaching goal (if goal is defined)
    float goalBonus = 0.0f;
    if (!currentGoal_.empty() && state.size() == currentGoal_.size()) {
        float stateGoalSim = 0.0f, stateNorm = 0.0f, goalNorm = 0.0f;
        for (size_t i = 0; i < state.size(); ++i) {
            stateGoalSim += state[i] * currentGoal_[i];
            stateNorm += state[i] * state[i];
            goalNorm += currentGoal_[i] * currentGoal_[i];
        }
        if (stateNorm > 0.0001f && goalNorm > 0.0001f) {
            goalBonus = stateGoalSim / (std::sqrt(stateNorm) * std::sqrt(goalNorm));
        }
    }
    
    // Get concept-based bonus if concepts are available
    float conceptBonus = 0.0f;
    if (conceptFormation_ && !currentGoal_.empty()) {
        std::vector<size_t> relevantConcepts = conceptFormation_->getRelevantConcepts(currentGoal_);
        if (!relevantConcepts.empty()) {
            conceptBonus = 0.5f;  // Bonus for using relevant concepts
        }
    }
    
    // Combine all factors with appropriate weights
    float totalValue = baselineValue * 0.4f + 
                      (1.0f - neuralValue) * 0.3f +  // Lower error = higher value
                      goalBonus * 0.2f + 
                      conceptBonus * 0.1f;
    
    return totalValue;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                   const std::vector<float>& currentState,
                                                   SimulationStep currentTime) {
    // Use forward model from prediction system if available
    if (predictionSystem_) {
        auto predictionResult = predictionSystem_->predictNextState(
            SensoryInput(currentState), currentTime);
        return predictionResult.first->getData();
    }
    
    // Fallback: simple action consequence model
    std::vector<float> nextState = currentState;
    
    // Apply action-specific dynamics
    switch (action) {
        case ActionType::MoveForward:
            // Simulate forward movement by shifting and smoothing
            for (size_t i = 0; i < nextState.size(); ++i) {
                float newValue = nextState[i] * 0.9f;
                if (i + 1 < nextState.size()) {
                    newValue += nextState[i + 1] * 0.1f;
                }
                nextState[i] = newValue;
            }
            break;
            
        case ActionType::MoveBackward:
            // Shift in opposite direction
            for (size_t i = nextState.size(); i > 0; --i) {
                nextState[i] = nextState[i - 1] * 0.9f;
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Apply rotation transformation
            float angle = (action == ActionType::TurnLeft) ? -0.1f : 0.1f;
            for (size_t i = 0; i < nextState.size(); ++i) {
                float original = nextState[i];
                nextState[i] = original * std::cos(angle) +
                            (i + 1 < nextState.size() ? nextState[i + 1] * 0.5f : 0.0f) * std::sin(angle);
            }
            break;
            
        case ActionType::Interact:
            // Significant state change for interaction
            for (size_t i = 0; i < nextState.size(); ++i) {
                nextState[i] = 0.5f + (rand() / (RAND_MAX + 1.0f)) * 0.5f;  // Random change
            }
            break;
            
        case ActionType::Wait:
            // Small decay for waiting
            for (size_t i = 0; i < nextState.size(); ++i) {
                nextState[i] *= 0.95f;
            }
            break;
    }
    
    return nextState;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                      const std::vector<ActionType>& actualActions,
                                      float actualReward,
                                      SimulationStep currentTime) {
    // Calculate plan success
    bool planSuccess = false;
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    for (size_t i = 0; i < minLen; ++i) {
        if (plannedActions[i] == actualActions[i]) {
            planSuccess = true;
            break;
        }
    }
    
    // Update action value distributions
    for (size_t i = 0; i < minLen; ++i) {
        ActionType action = actualActions[i];
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionValueDistributions_.meanValues.size()) {
            // Update action value based on outcome
            float oldValue = actionValueDistributions_.meanValues[actionIdx];
            float newValue = (oldValue * actionValueDistributions_.observationCounts[actionIdx] +
                             actualReward) / (actionValueDistributions_.observationCounts[actionIdx] + 1);
            
            actionValueDistributions_.meanValues[actionIdx] = newValue;
            
            // Reduce variance as we learn more
            actionValueDistributions_.variances[actionIdx] *= 0.9f;
            actionValueDistributions_.observationCounts[actionIdx]++;
            
            // Record planning error
            float error = std::abs(newValue - actualReward);
            pImpl->planningErrors.push_back(error);
            if (pImpl->planningErrors.size() > 100) {
                pImpl->planningErrors.pop_front();
            }
        }
    }
    
    // Update planning confidence based on prediction accuracy
    if (!pImpl->planningErrors.empty()) {
        float totalError = std::accumulate(pImpl->planningErrors.begin(), 
                                          pImpl->planningErrors.end(), 0.0f);
        float avgError = totalError / pImpl->planningErrors.size();
        planningConfidence_ = 1.0f - avgError;
    }
}

void NeuralPlanner::updateActionValueDistributions(SimulationStep currentTime) {
    // Update action value distributions based on recent experience
    if (currentTime - lastPlanningUpdate_ < 1.0f) return;  // Don't update too frequently
    
    // Decay action variances over time
    for (float& variance : actionValueDistributions_.variances) {
        variance *= 1.05f;  // Increase exploration over time
    }
    
    lastPlanningUpdate_ = currentTime;
}

void NeuralPlanner::clearCache() {
    // Clear planning statistics
    pImpl->planningRewards.clear();
    pImpl->planningErrors.clear();
    actionValueDistributions_ = ActionValueDistribution(actionValueDistributions_.meanValues.size());
    
    // Reset planning neurons
    for (auto& neuron : pImpl->planningNeurons) {
        neuron.activationHistory.clear();
        neuron.lastActive = -1;
    }
}

std::vector<ActionType> NeuralPlanner::getPreferredActions(const std::vector<float>& state) const {
    std::vector<ActionType> preferredActions;
    
    // Get action preferences from brain regions if available
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                // Use synaptic weights as action preferences
                if (syn->getWeight() > 0.1f) {
                    // In a real implementation, this would map synapses to actions
                    // For now, use a simple heuristic
                    preferredActions.push_back(ActionType::MoveForward);
                }
            }
        }
    }
    
    // Add some randomness for exploration
    if (preferredActions.size() < 3) {
        preferredActions.push_back(ActionType::TurnLeft);
        preferredActions.push_back(ActionType::Interact);
    }
    
    return preferredActions;
}

float NeuralPlanner::computePredictionConfidence(const std::vector<float>& state,
                                                const std::vector<float>& predicted) const {
    if (state.size() != predicted.size() || state.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < state.size(); ++i) {
        totalDiff += std::abs(state[i] - predicted[i]);
    }
    
    float avgDiff = totalDiff / state.size();
    return std::max(0.0f, 1.0f - avgDiff);
}

float NeuralPlanner::getSelfModelConfidence(ActionType action) const {
    // Use action value distribution to estimate confidence
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionValueDistributions_.meanValues.size()) return 0.5f;
    
    float variance = actionValueDistributions_.variances[actionIdx];
    float count = actionValueDistributions_.observationCounts[actionIdx];
    
    // Higher variance or fewer observations = lower confidence
    float confidence = 1.0f / (1.0f + variance * (1.0f + 1.0f / (count + 1.0f)));
    
    return confidence;
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (pImpl->planningRewards.empty()) return true;  // No data
    
    size_t successCount = 0;
    for (float reward : pImpl->planningRewards) {
        if (reward > 0.5f) {
            successCount++;
        }
    }
    
    return successCount > pImpl->planningRewards.size() / 2;
}

NeuralPlanner::PlanningStats NeuralPlanner::getStats() const {
    PlanningStats stats;
    stats.totalPlansEvaluated = pImpl->planningRewards.size();
    stats.successfulPlans = 0;
    
    float totalReward = 0.0f;
    float totalConfidence = 0.0f;
    float totalError = 0.0f;
    
    for (float reward : pImpl->planningRewards) {
        totalReward += reward;
        if (reward > 0.5f) {
            stats.successfulPlans++;
        }
    }
    
    for (float error : pImpl->planningErrors) {
        totalError += error;
    }
    
    if (!pImpl->planningRewards.empty()) {
        stats.meanExpectedReward = totalReward / pImpl->planningRewards.size();
    }
    
    if (!pImpl->planningErrors.empty()) {
        stats.meanPlanningConfidence = 1.0f - (totalError / pImpl->planningErrors.size());
        stats.predictionError = totalError / pImpl->planningErrors.size();
    }
    
    stats.explorationRate = explorationRate_;
    
    return stats;
}

void NeuralPlanner::updateFromPredictionErrors(const std::vector<float>& predictionErrors,
                                               SimulationStep currentTime) {
    pImpl->predictionErrorHistory = predictionErrors;
    
    // Update planning confidence based on prediction errors
    if (!predictionErrors.empty()) {
        float totalError = std::accumulate(predictionErrors.begin(), predictionErrors.end(), 0.0f);
        float avgError = totalError / predictionErrors.size();
        planningConfidence_ = 1.0f - avgError;
    }
}

} // namespace nlm