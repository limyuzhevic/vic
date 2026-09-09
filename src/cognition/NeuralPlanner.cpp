#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct NeuralPlanner::Impl {
    // Planning memory: store past experiences and outcomes
    struct PlanMemoryEntry {
        std::vector<float> state;
        ActionType action;
        std::vector<float> predictedOutcome;
        float actualReward;
        size_t stepsTaken;
        float confidence;
    };
    
    std::vector<PlanMemoryEntry> planMemory;
    
    // Neural planning state
    std::vector<float> currentPlanningState;
    
    // Simulation for potential futures
    struct SimulatedFuture {
        std::vector<ActionType> actionSequence;
        std::vector<float> predictedFinalState;
        float totalReward;
        float confidence;
        size_t depth;
    };
    
    Impl() : planningDepth(5), planningConfidence(0.5f) {}
    
    // Neural simulation for planning
    SimulatedFuture simulateFuture(const std::vector<float>& currentState,
                                   const std::vector<ActionType>& actions,
                                   size_t steps);
    
    // Generate action sequences using neural sampling
    std::vector<std::vector<ActionType>> generateActionSequencesNeural(size_t depth);
    
    // Apply planning to neural substrate
    void applyPlanToBrain(const std::vector<ActionType>& actions);
};

NeuralPlanner::NeuralPlanner()
    : brain(nullptr)
    , planningDepth(5)
    , planningConfidence(0.5f)
    , pImpl(new Impl)
{
    // Initialize action quality vector (8 motor commands + Wait)
    actionQuality_.resize(8, 0.5f);
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    this->brain = brain;
    pImpl->planMemory.clear();
    
    NLM_LOG_INFO("NeuralPlanner initialized with brain reference");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                   float targetReward) {
    // Get planning depth from brain config if available
    if (brain_ && brain_->getConfig()) {
        planningDepth_ = brain_->getConfig()->getOr<size_t>("planning.depth", planningDepth_);
    }
    
    // Generate possible action sequences
    auto sequences = pImpl->generateActionSequencesNeural(planningDepth_);
    
    if (sequences.empty()) {
        return ActionType::Wait;
    }
    
    // Evaluate all sequences
    std::vector<std::pair<ActionType, float>> actionScores;
    
    for (const auto& sequence : sequences) {
        // Evaluate first action in sequence for immediate selection
        ActionType action = sequence[0];
        
        // Get action quality from experience
        float quality = actionQuality_[static_cast<size_t>(action)];
        
        // Adjust based on current state and target reward
        float stateValue = 0.0f;
        if (!currentGoal_.empty() && currentState.size() == currentGoal_.size()) {
            for (size_t i = 0; i < currentState.size(); ++i) {
                stateValue += currentState[i] * currentGoal_[i];
            }
        }
        
        // Combine factors
        float score = quality * 0.6f + stateValue * 0.3f + 
                     (targetReward - 0.5f) * 0.1f;  // Prefer actions toward target
        
        actionScores.emplace_back(action, score);
    }
    
    // Select best action
    ActionType bestAction = ActionType::Wait;
    float bestScore = -1.0f;
    
    for (const auto& pair : actionScores) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestAction = pair.first;
        }
    }
    
    // Store planning decision for learning
    pImpl->currentPlanningState = currentState;
    
    return bestAction;
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                 const std::vector<float>& startState) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = std::min(actions.size(), planningDepth_);
    
    // Use neural simulation for evaluation
    SimulatedFuture future = pImpl->simulateFuture(startState, actions, candidate.depth);
    
    candidate.predictedStates = future.predictedFinalState;
    candidate.expectedReward = future.totalReward;
    candidate.confidence = future.confidence;
    
    return candidate;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                     const std::vector<ActionType>& actualActions,
                                     float actualReward) {
    // Update action quality based on outcome
    float qualityChange = (actualReward - 0.5f) * 0.1f;
    
    for (ActionType action : plannedActions) {
        size_t idx = static_cast<size_t>(action);
        if (idx < actionQuality_.size()) {
            actionQuality_[idx] = std::clamp(actionQuality_[idx] + qualityChange, 0.0f, 1.0f);
        }
    }
    
    // Store experience for future planning
    if (!plannedActions.empty()) {
        PlanMemoryEntry entry;
        entry.action = plannedActions[0]; // Focus on first action for learning
        entry.actualReward = actualReward;
        entry.stepsTaken = actualActions.size();
        entry.confidence = actualReward > 0.5f ? 0.8f : 0.2f;
        
        pImpl->planMemory.push_back(entry);
        
        // Limit memory size
        if (pImpl->planMemory.size() > 100) {
            pImpl->planMemory.erase(pImpl->planMemory.begin());
        }
    }
    
    // Update planning confidence based on success
    float recentSuccessRate = 0.5f;
    if (!pImpl->planMemory.empty()) {
        size_t successCount = 0;
        for (const auto& entry : pImpl->planMemory) {
            if (entry.actualReward > 0.5f) {
                successCount++;
            }
        }
        recentSuccessRate = static_cast<float>(successCount) / pImpl->planMemory.size();
    }
    
    planningConfidence_ = std::clamp(recentSuccessRate, 0.1f, 1.0f);
}

void NeuralPlanner::clearCache() {
    pImpl->planMemory.clear();
    pImpl->currentPlanningState.clear();
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (pImpl->planMemory.empty()) {
        return false;
    }
    
    size_t recentSuccessful = 0;
    size_t recentTotal = std::min(size_t(10), pImpl->planMemory.size());
    
    for (size_t i = 0; i < recentTotal; ++i) {
        if (pImpl->planMemory[pImpl->planMemory.size() - 1 - i].actualReward > 0.5f) {
            recentSuccessful++;
        }
    }
    
    return recentSuccessful >= recentTotal / 2; // Success if half or more recent plans were successful
}

// NeuralPlanner Implementation Details

SimulatedFuture NeuralPlanner::Impl::simulateFuture(const std::vector<float>& currentState,
                                                   const std::vector<ActionType>& actions,
                                                   size_t steps) {
    SimulatedFuture future;
    future.actionSequence = actions;
    future.depth = steps;
    
    // Simple neural simulation
    std::vector<float> current = currentState;
    float totalReward = 0.0f;
    
    for (size_t i = 0; i < steps; ++i) {
        // Each action has a baseline effect
        float actionEffect = 0.1f;
        
        // Apply action effect to state
        if (!current.empty()) {
            for (size_t j = 0; j < std::min(current.size(), size_t(4)); ++j) {
                current[j] += actionEffect * (i + 1) * 0.1f;
            }
        }
        
        totalReward += actionEffect * 0.5f;
        
        // Store predicted state
        future.predictedFinalState.push_back(current);
    }
    
    // Compute confidence based on action similarity to past successes
    float similarity = 0.5f;
    if (!planMemory.empty()) {
        for (const auto& entry : planMemory) {
            if (entry.action == actions[0]) {
                similarity = (similarity + entry.confidence) * 0.5f;
            }
        }
    }
    
    future.confidence = similarity;
    
    return future;
}

std::vector<std::vector<ActionType>> NeuralPlanner::Impl::generateActionSequencesNeural(size_t depth) {
    std::vector<std::vector<ActionType>> sequences;
    
    // Sample actions based on action quality from brain reference
    if (!brain_) {
        return sequences;
    }
    
    // Convert to float weights for sampling
    std::vector<float> weights;
    for (float quality : brain_->getPlanner() ? brain_->getPlanner()->actionQuality_ : 
                               std::vector<float>(8, 0.5f)) {
        weights.push_back(quality);
    }
    
    // Generate sequences
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < 20; ++i) { // Generate 20 sequences
        std::vector<ActionType> sequence;
        
        for (size_t step = 0; step < depth; ++step) {
            // Choose action based on weights
            float r = dist(gen);
            float cumulative = 0.0f;
            size_t chosen = 0;
            
            for (size_t j = 0; j < weights.size(); ++j) {
                cumulative += weights[j];nlm {
    NeuralPlanner::Impl::generateActionSequencesNeural(size_t depth) {
        std::vector<std::vector<ActionType>> sequences;
        
        // Sample actions based on action quality from brain reference
        if (!brain_) {
            return sequences;
        }
        
        // Convert to float weights for sampling
        std::vector<float> weights;
        for (float quality : brain_->getPlanner() ? brain_->getPlanner()->actionQuality_ : 
                               std::vector<float>(8, 0.5f)) {
            weights.push_back(quality);
        }
        
        // Generate sequences
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        for (size_t i = 0; i < 20; ++i) { // Generate 20 sequences
            std::vector<ActionType> sequence;
            
            for (size_t step = 0; step < depth; ++step) {
                // Choose action based on weights
                float r = dist(gen);
                float cumulative = 0.0f;
                size_t chosen = 0;
                
                for (size_t j = 0; j < weights.size(); ++j) {
                    cumulative += weights[j];
                    if (r <= cumulative) {
                        chosen = j;
                        break;
                    }
                }
                
                // Map to ActionType (excluding LookLeft/LookRight for now)
                ActionType action;
                if (chosen < 4) {
                    action = static_cast<ActionType>(chosen);
                } else if (chosen == 4) {
                    action = ActionType::LookLeft;
                } else if (chosen == 5) {
                    action = ActionType::LookRight;
                } else if (chosen == 6) {
                    action = ActionType::Interact;
                } else {
                    action = ActionType::Wait;
                }
                
                sequence.push_back(action);
            }
            
            sequences.push_back(sequence);
        }
        
        return sequences;
    }
    
    // Apply planning to neural substrate
    void NeuralPlanner::Impl::applyPlanToBrain(const std::vector<ActionType>& actions) {
        if (!brain_) {
            return;
        }
        
        // Enhance neural activity for planned actions
        for (ActionType action : actions) {
            size_t idx = static_cast<size_t>(action);
            if (idx < 8) {
                brain_->injectCurrentToNeurons(NeuronType::Motor, 20.0f * (1.0f + actionQuality_[idx]));
            }
        }
    }
}

} // namespace nlm