#include "MotorPlanning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct MotorPlanning::Impl {
    class Brain* brain_;
    std::vector<MotorPrimitive*> currentPlan_;
    size_t planningHorizon_;
    size_t maxPrimitivesPerAction_;
    float replanningThreshold_;
    float lastExecutionError_;
    float currentExecutionTime_;
    float totalExecutionTime_;
    
    Impl() : brain_(nullptr), planningHorizon_(5), maxPrimitivesPerAction_(3),
             replanningThreshold_(0.5f), lastExecutionError_(0.0f),
             currentExecutionTime_(0.0f), totalExecutionTime_(0.0f) {}
};

MotorPlanning::MotorPlanning() : pImpl(new Impl) {}

MotorPlanning::~MotorPlanning() = default;

void MotorPlanning::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("MotorPlanning initialized");
}

std::vector<MotorPrimitive*> MotorPlanning::planSequence(const std::vector<float>& startState,
                                                         const std::vector<float>& goalState,
                                                         const std::vector<ActionType>& preferredActions) {
    // Generate simple action sequences
    std::vector<std::vector<ActionType>> sequences = generateActionSequences(pImpl->planningHorizon_);
    
    // Evaluate sequences
    std::vector<std::pair<float, std::vector<MotorPrimitive*>>> evaluatedSequences;
    
    for (const auto& actionSequence : sequences) {
        // Create primitives for each action
        std::vector<MotorPrimitive*> primitives;
        for (size_t i = 0; i < std::min(actionSequence.size(), preferredActions.size()); ++i) {
            auto* primitive = new MotorPrimitive();
            primitive->initialize(pImpl->brain_);
            primitive->setAction(actionSequence[i]);
            primitive->setTargetPosition(goalState);
            primitive->setStrength(0.8f);
            primitive->setDuration(1.0f + i * 0.5f);
            primitive->setSmoothness(0.7f);
            primitive->setActive(true);
            primitives.push_back(primitive);
        }
        
        // Evaluate plan quality
        float score = 0.0f;
        for (size_t i = 0; i < primitives.size(); ++i) {
            score += primitives[i]->getStrength() / static_cast<float>(primitives.size());
        }
        
        evaluatedSequences.push_back({score, primitives});
    }
    
    // Sort by score (descending)
    std::sort(evaluatedSequences.begin(), evaluatedSequences.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Clear current plan and set new plan
    clearPlan();
    pImpl->currentPlan_ = evaluatedSequences.empty() ? std::vector<MotorPrimitive*>() : evaluatedSequences[0].second;
    
    return pImpl->currentPlan_;
}

void MotorPlanning::setCurrentPlan(const std::vector<MotorPrimitive*>& plan) {
    clearPlan();
    pImpl->currentPlan_ = plan;
}

void MotorPlanning::executePlan(TimestepDuration dt) {
    for (auto* primitive : pImpl->currentPlan_) {
        if (primitive->isActive()) {
            primitive->update(dt);
        }
    }
}

std::vector<float> MotorPlanning::getCombinedMotorCommand() const {
    std::vector<float> combinedCommand;
    if (!pImpl->currentPlan_.empty()) {
        // Get command from first active primitive
        for (const auto* primitive : pImpl->currentPlan_) {
            if (primitive->isActive()) {
                std::vector<float> command = primitive->getMotorCommand(
                    primitive->getElapsedTime() / primitive->getDuration());
                
                // Combine commands (average)
                if (combinedCommand.empty()) {
                    combinedCommand = command;
                } else {
                    for (size_t i = 0; i < command.size() && i < combinedCommand.size(); ++i) {
                        combinedCommand[i] = (combinedCommand[i] + command[i]) / 2.0f;
                    }
                }
                break; // Use only first primitive for now
            }
        }
    }
    return combinedCommand;
}

void MotorPlanning::updatePlan(const std::vector<float>& sensoryInput, TimestepDuration dt) {
    // Simple replanning based on error
    float error = 0.0f;
    for (float value : sensoryInput) {
        error += std::abs(value);
    }
    error /= sensoryInput.empty() ? 1.0f : static_cast<float>(sensoryInput.size());
    
    pImpl->lastExecutionError_ = error;
    
    // Replan if error is high
    if (needsReplanning(sensoryInput)) {
        NLM_LOG_INFO("Replanning needed due to high error: " + std::to_string(error));
        // Would get goal state from brain here
        std::vector<float> goalState(5, 0.0f);
        planSequence(std::vector<float>(5, 0.0f), goalState, std::vector<ActionType>());
    }
}

void MotorPlanning::clearPlan() {
    for (auto* primitive : pImpl->currentPlan_) {
        delete primitive;
    }
    pImpl->currentPlan_.clear();
}

bool MotorPlanning::needsReplanning(const std::vector<float>& currentError) const {
    float error = 0.0f;
    for (float value : currentError) {
        error += value * value;
    }
    error = std::sqrt(error / currentError.size());
    return error > pImpl->replanningThreshold_;
}

std::vector<std::vector<ActionType>> MotorPlanning::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> sequences;
    
    // Simple sequences for testing
    std::vector<ActionType> simpleSeq = {ActionType::MoveForward, ActionType::TurnRight, ActionType::Look};
    sequences.push_back(simpleSeq);
    
    return sequences;
}

} // namespace nlm
