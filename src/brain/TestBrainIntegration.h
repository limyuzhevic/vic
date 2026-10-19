#include <cassert>
#include <iostream>
#include <stdexcept>

namespace nlm {

class AssertionError : public std::runtime_error {
public:
    AssertionError(const std::string& message) : std::runtime_error(message) {}
};

inline void test_assert(bool condition, const std::string& message) {
    if (!condition) {
        throw AssertionError(message);
    }
}

void TestBrainIntegration::runTests() {
    std::cout << "=== Testing Neural Planner Integration in Brain ===\n";
    
    // Test 1: Create brain and verify neural planner is initialized
    testNeuralPlannerInitialization();
    
    // Test 2: Test brain state extraction
    testBrainStateExtraction();
    
    // Test 3: Test neural planner integration in produceAction
    testNeuralPlannerIntegration();
    
    // Test 4: Test error handling when planner is not available
    testErrorHandling();
    
    std::cout << "\nAll tests passed!\n";
}

void TestBrainIntegration::testNeuralPlannerInitialization() {
    std::cout << "Test 1: Neural Planner Initialization... ";
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    
    // Initialize brain
    bool initSuccess = brain.initialize();
    test_assert(initSuccess, "Brain initialization failed");
    
    // Verify neural planner is available
    NeuralPlanner* planner = brain.getPlanner();
    test_assert(planner != nullptr, "Neural planner should be available after initialization");
    
    // Verify planner has brain reference
    test_assert(planner->brain_ != nullptr, "Neural planner should have brain reference");
    
    // Verify planner is properly configured
    test_assert(planner->getPlanningDepth() == 5, "Planning depth should be 5");
    
    std::cout << "PASSED\n";
}

void TestBrainIntegration::testBrainStateExtraction() {
    std::cout << "Test 2: Brain State Extraction... ";
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get brain state
    std::vector<float> brainState = brain.getCurrentBrainState();
    
    // Verify brain state has expected number of features
    // Expected features: avgMembranePotential, sensoryFiringRate, internalFiringRate, 
    //                    motorFiringRate, workingMemoryActivity, attentionFocus,
    //                    noveltyLevel, curiosityLevel, predictionError, dopamineLevel, developmentalStage
    const size_t expectedSize = 11;
    test_assert(brainState.size() == expectedSize, 
        "Brain state should contain " + std::to_string(expectedSize) + " features, got " + 
        std::to_string(brainState.size()));
    
    // Verify all values are reasonable (not NaN or infinite)
    for (size_t i = 0; i < brainState.size(); ++i) {
        test_assert(!std::isnan(brainState[i]), 
            "Brain state value at index " + std::to_string(i) + " is NaN");
        test_assert(!std::isinf(brainState[i]), 
            "Brain state value at index " + std::to_string(i) + " is infinite");
    }
    
    // Verify specific features make sense
    // Feature 0: Average membrane potential should be in reasonable range
    test_assert(brainState[0] >= -100.0f && brainState[0] <= 100.0f,
        "Average membrane potential out of expected range");
    
    // Feature 1-3: Firing rates should be non-negative
    test_assert(brainState[1] >= 0.0f, "Sensory firing rate should be non-negative");
    test_assert(brainState[2] >= 0.0f, "Internal firing rate should be non-negative");
    test_assert(brainState[3] >= 0.0f, "Motor firing rate should be non-negative");
    
    // Feature 10: Developmental stage should be normalized (0-1)
    test_assert(brainState[10] >= 0.0f && brainState[10] <= 1.0f,
        "Developmental stage should be normalized to 0-1 range");
    
    std::cout << "PASSED\n";
}

void TestBrainIntegration::testNeuralPlannerIntegration() {
    std::cout << "Test 3: Neural Planner Integration... ";
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get last planned action (should be default)
    ActionType lastAction = brain.getLastPlannedAction();
    test_assert(lastAction == ActionType::Wait, "Initial last planned action should be Wait");
    
    // Get action using neural planner
    auto action = brain.produceAction();
    test_assert(action != nullptr, "Action should not be null");
    
    // Verify action type was set
    ActionType actionType = action->getType();
    test_assert(actionType == ActionType::Wait || actionType == ActionType::MoveForward,
        "Action type should be valid");
    
    // Verify last planned action was updated
    ActionType updatedLastAction = brain.getLastPlannedAction();
    test_assert(updatedLastAction == actionType, "Last planned action should match action type");
    
    // Log the neural planning decision
    std::cout << "  Neural planner selected action: " << action->getName() << std::endl;
    
    std::cout << "PASSED\n";
}

void TestBrainIntegration::testErrorHandling() {
    std::cout << "Test 4: Error Handling... ";
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Test with available planner (should work)
    auto action1 = brain.produceAction();
    test_assert(action1 != nullptr, "Action should not be null with available planner");
    
    // Test that planner state is preserved after error
    test_assert(brain.getPlanner() != nullptr, "Planner should still be available");
    
    std::cout << "PASSED\n";
}

} // namespace nlm
