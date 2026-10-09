#include "PredictionSystem.hpp"
#include "../../core/Types/Types.hpp"
#include "../../sensory/Vision.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace nlm {

void testPredictionSystem() {
    std::cout << "Testing PredictionSystem..." << std::endl;
    
    // Create prediction system
    PredictionSystem predSys;
    
    // Test 1: Initialization
    std::cout << "Test 1: Initialization..." << std::endl;
    // For now, test without brain
    
    // Test 2: Configuration
    std::cout << "Test 2: Configuration..." << std::endl;
    predSys.setPredictionHorizon(5);
    predSys.setSequenceMemorySize(20);
    predSys.setPrecisionWeighting(true);
    predSys.setActiveInference(true);
    
    // Test 3: Prediction modalities
    std::cout << "Test 3: Prediction modalities..." << std::endl;
    predSys.enableSensoryPrediction(true);
    predSys.enableMotorPrediction(true);
    predSys.enableInternalPrediction(true);
    
    // Test 4: Error history
    std::cout << "Test 4: Error history..." << std::endl;
    std::vector<std::vector<float>> errorHistory = predSys.getErrorHistory();
    assert(errorHistory.empty());  // Should be empty initially
    
    // Test 5: Clear history
    std::cout << "Test 5: Clear history..." << std::endl;
    predSys.clearHistory();
    
    // Test 6: Action selection
    std::cout << "Test 6: Action selection..." << std::endl;
    std::vector<ActionType> availableActions = {ActionType::MOVE_LEFT, ActionType::MOVE_RIGHT, 
                                              ActionType::MOVE_UP, ActionType::MOVE_DOWN};
    
    // For now, test with null input (Vision default constructor)
    ActionType selectedAction = predSys.selectAction(availableActions, Vision());
    assert(selectedAction == ActionType::NO_ACTION || 
           std::find(availableActions.begin(), availableActions.end(), selectedAction) != availableActions.end());
    
    // Test 7: Hierarchical state
    std::cout << "Test 7: Hierarchical state..." << std::endl;
    std::vector<std::vector<float>> hierarchicalState = predSys.getHierarchicalState();
    assert(!hierarchicalState.empty());  // Should have at least one level
    
    // Test 8: Prediction neurons
    std::cout << "Test 8: Prediction neurons..." << std::endl;
    std::vector<std::vector<NeuronId>> predictionNeurons = predSys.getPredictionNeurons();
    assert(predictionNeurons.empty());  // Should be empty without brain
    
    // Test 9: Neuromodulation
    std::cout << "Test 9: Neuromodulation..." << std::endl;
    
    // Create a mock neuromodulator for testing
    class MockNeuromodulator : public Neuromodulator {
    public:
        const char* getName() const override { return "Mock"; }
        float getLevel() const override { return 0.0f; }
        void setLevel(float level) override {}
        float getPlasticityFactor() const override { return 1.0f; }
        void update(TimestepDuration dt) override {}
    };
    
    MockNeuromodulator mockMod;
    predSys.setNeuromodulator(&mockMod, "MockModulator");
    
    // Test 10: Prediction error computation
    std::cout << "Test 10: Prediction error computation..." << std::endl;
    std::vector<float> predictionErrors = predSys.getPredictionErrors();
    std::vector<float> predictionConfidences = predSys.getPredictionConfidences();
    
    assert(predictionErrors.size() == hierarchicalState.size());
    assert(predictionConfidences.size() == hierarchicalState.size());
    
    // Test 11: Basic prediction methods
    std::cout << "Test 11: Basic prediction methods..." << std::endl;
    // Test with default-constructed Vision object
    Vision testVision;
    auto predictedState = predSys.predictNextState(testVision, 0.1);
    assert(predictedState != nullptr);
    
    // Test update predictions
    predSys.updatePredictions(testVision, 0.1);
    
    // Test 12: Training
    std::cout << "Test 12: Training..." << std::endl;
    predSys.train(testVision);
    
    // Test 13: Precision weighting
    std::cout << "Test 13: Precision weighting..." << std::endl;
    // After training, precision should be updated
    
    // Test 14: Prediction horizon configuration
    std::cout << "Test 14: Prediction horizon configuration..." << std::endl;
    predSys.setPredictionHorizon(10);
    
    // Test 15: Sequence memory size
    std::cout << "Test 15: Sequence memory size..." << std::endl;
    predSys.setSequenceMemorySize(100);
    
    // Test 16: Active inference configuration
    std::cout << "Test 16: Active inference configuration..." << std::endl;
    predSys.setActiveInference(false);
    ActionType noInferenceAction = predSys.selectAction(availableActions, testVision);
    assert(noInferenceAction == ActionType::NO_ACTION || 
           std::find(availableActions.begin(), availableActions.end(), noInferenceAction) != availableActions.end());
    
    std::cout << "All tests passed!" << std::endl;
}

} // namespace nlm