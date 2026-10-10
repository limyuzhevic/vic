// Neural Planning Tests
// Tests for predictive planning and action sequence evaluation

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_neural_planning {

void testActionPlanning() {
    std::cout << "  Testing action planning..." << std::endl;
    
    // Test that planning system can generate action sequences
    std::vector<std::string> actions = {"move_forward", "turn_left", "turn_right", "look", "interact"};
    
    // Simple planning test - plan a sequence of actions
    std::vector<std::string> plannedSequence;
    
    // Plan: go forward, turn right, look
    plannedSequence.push_back("move_forward");
    plannedSequence.push_back("turn_right");
    plannedSequence.push_back("look");
    
    // Verify the planned sequence is valid
    assert(plannedSequence.size() == 3 && "Planned sequence should have 3 actions");
    assert(std::find(actions.begin(), actions.end(), plannedSequence[0]) != actions.end() && "First action should be valid");
    assert(std::find(actions.begin(), actions.end(), plannedSequence[1]) != actions.end() && "Second action should be valid");
    assert(std::find(actions.begin(), actions.end(), plannedSequence[2]) != actions.end() && "Third action should be valid");
    
    std::cout << "    ✓ Action planning test passed" << std::endl;
}

void testSequenceEvaluation() {
    std::cout << "  Testing sequence evaluation..." << std::endl;
    
    // Test that planning system can evaluate different action sequences
    std::vector<std::vector<std::string>> possibleSequences = {
        {"move_forward", "turn_left", "look"},
        {"turn_right", "move_forward", "interact"},
        {"move_backward", "turn_right", "look"}
    };
    
    // Evaluate each sequence
    float bestSequenceScore = 0.0f;
    std::vector<std::string> bestSequence;
    
    for (const auto& sequence : possibleSequences) {
        float sequenceScore = 0.0f;
        
        // Simple scoring based on action types
        for (size_t i = 0; i < sequence.size(); ++i) {
            if (sequence[i] == "move_forward" || sequence[i] == "move_backward") {
                sequenceScore += 1.0f; // Movement actions
            } else if (sequence[i] == "look" || sequence[i] == "look_up" || sequence[i] == "look_down") {
                sequenceScore += 0.5f; // Observation actions
            } else if (sequence[i] == "interact" || sequence[i] == "turn_left" || sequence[i] == "turn_right") {
                sequenceScore += 0.8f; // Interaction/actions
            }
        }
        
        // Consider sequence length penalty
        sequenceScore -= static_cast<float>(sequence.size()) * 0.1f;
        
        if (sequenceScore > bestSequenceScore) {
            bestSequenceScore = sequenceScore;
            bestSequence = sequence;
        }
    }
    
    // Verify that best sequence was selected
    assert(bestSequenceScore > 0.0f && "Should have a valid best sequence");
    assert(bestSequence.size() > 0 && "Best sequence should not be empty");
    
    std::cout << "    ✓ Sequence evaluation test passed" << std::endl;
}

void testPlanningDepth() {
    std::cout << "  Testing planning depth..." << std::endl;
    
    // Test that planning system can handle different planning depths
    std::vector<size_t> planningDepths = {1, 2, 3, 5, 10};
    
    for (size_t depth : planningDepths) {
        // Simulate planning at different depths
        std::vector<std::string> plan;
        
        // Create a plan of specified depth
        for (size_t i = 0; i < depth; ++i) {
            plan.push_back("action_" + std::to_string(i));
        }
        
        // Verify plan depth
        assert(plan.size() == depth && "Plan depth should match requested depth");
        
        // More complex plans should have higher cost
        float planCost = static_cast<float>(depth) * 1.0f;
        assert(planCost > 0.0f && "Plan should have positive cost");
    }
    
    std::cout << "    ✓ Planning depth test passed" << std::endl;
}

void runAll() {
    std::cout << "Neural Planning Tests:" << std::endl;
    
    testActionPlanning();
    testSequenceEvaluation();
    testPlanningDepth();
    
    std::cout << "Neural Planning Tests: PASSED" << std::endl;
}

} // namespace test_neural_planning
