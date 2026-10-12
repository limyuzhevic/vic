#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Forward declaration for recursive action sequence generator
class NeuralPlanner;

// Action sequence generator callback type
// This allows customizing sequence generation
using ActionSequenceGenerator = std::function<void(std::vector<ActionType>&, size_t, const std::vector<ActionType>&)>;

// Utility functions for action sequence generation
namespace ActionSequenceUtils {

// Recursive generator for action sequences
// Extracted from NeuralPlanner for better modularity
inline void generateActionSequencesRecursive(
    std::vector<ActionType>& current,
    size_t remaining,
    const std::vector<ActionType>& baseActions,
    std::vector<std::vector<ActionType>>& result,
    size_t maxSequences = 100) {
    if (remaining == 0) {
        result.push_back(current);
        return;
    }
    
    for (ActionType action : baseActions) {
        current.push_back(action);
        generateActionSequencesRecursive(current, remaining - 1, baseActions, result, maxSequences);
        current.pop_back();
    }
}

// Optimized iterative generator for large depth
// Avoids recursion stack overflow for deep planning
inline std::vector<std::vector<ActionType>> generateActionSequencesIterative(
    size_t depth,
    const std::vector<ActionType>& baseActions,
    size_t maxSequences = 100) {
    std::vector<std::vector<ActionType>> result;
    
    if (depth == 0) {
        result.push_back(std::vector<ActionType>());
        return result;
    }
    
    // Use iterative approach with stack simulation
    std::vector<std::vector<ActionType>> currentLevel;
    currentLevel.push_back(std::vector<ActionType>());
    
    for (size_t step = 0; step < depth; ++step) {
        std::vector<std::vector<ActionType>> nextLevel;
        
        for (const auto& sequence : currentLevel) {
            for (ActionType action : baseActions) {
                std::vector<ActionType> newSequence = sequence;
                newSequence.push_back(action);
                nextLevel.push_back(newSequence);
            }
        }
        
        currentLevel = std::move(nextLevel);
    }
    
    result = std::move(currentLevel);
    
    // Apply limit
    if (result.size() > maxSequences) {
        result.resize(maxSequences);
    }
    
    return result;
}

// Validate action sequence for planning feasibility
inline bool validateActionSequence(const std::vector<ActionType>& sequence,
                                   const std::vector<float>& currentState) {
    // Basic validation - check for contradictory actions
    // Could be extended with neural plausibility checks
    
    if (sequence.empty()) return true;
    
    // Check for impossible sequences
    for (size_t i = 0; i < sequence.size() - 1; ++i) {
        // Simple check for immediate contradictions
        // In a full implementation, this would use learned constraints
        if (sequence[i] == ActionType::MoveForward && sequence[i + 1] == ActionType::MoveBackward &&
            i + 1 < sequence.size()) {
            return false; // Can't move forward then immediately backward
        }
    }
    
    return true;
}

// Generate base action set for planning
inline std::vector<ActionType> getBaseActionSet() {
    return {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
}

// Calculate sequence complexity score
inline float calculateSequenceComplexity(const std::vector<ActionType>& sequence) {
    // Simple heuristic: more actions = higher complexity
    // Could be enhanced with action dependency analysis
    return static_cast<float>(sequence.size()) / 10.0f;
}

// Generate action sequences using multiple strategies
// Provides flexible planning for different scenarios
inline std::vector<std::vector<ActionType>> generateActionSequences(
    size_t depth,
    const std::vector<ActionType>& baseActions = getBaseActionSet(),
    size_t maxSequences = 100,
    bool useIterativeForDeep = true) {
    
    // Choose generation strategy based on depth
    if (useIterativeForDeep && depth > 4) {
        // Use iterative for deep sequences to avoid stack overflow
        return generateActionSequencesIterative(depth, baseActions, maxSequences);
    } else {
        // Use recursive for shallow sequences (simpler to understand)
        std::vector<std::vector<ActionType>> result;
        std::vector<ActionType> current;
        
        std::function<void()> generate = [&](std::vector<ActionType>& currentSeq, size_t remaining) {
            if (remaining == 0) {
                result.push_back(currentSeq);
                return;
            }
            
            for (ActionType action : baseActions) {
                currentSeq.push_back(action);
                generate(currentSeq, remaining - 1);
                currentSeq.pop_back();
            }
        };
        
        generate(current, depth);
        
        // Apply limit
        if (result.size() > maxSequences) {
            result.resize(maxSequences);
        }
        
        return result;
    }
}

} // namespace ActionSequenceUtils

} // namespace nlm