// NLM Dopamine Tests
// Test the enhanced Dopamine neuromodulation system

#include "../src/neuromodulation/Dopamine.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_dopamine {
    void testInitialization() {
        nlm::Dopamine dopamine;
        
        // Test basic initialization
        assert(dopamine.getName() == "DA");
        assert(dopamine.getLevel() >= 0.0f);
        assert(dopamine.getPhasicComponent() >= 0.0f);
        assert(dopamine.getTonicComponent() >= 0.0f);
        
        // Test value function initialization
        assert(dopamine.getValueFunction() >= 0.0f);
        
        // Test plasticity factor calculation
        float plasticity = dopamine.getPlasticityFactor();
        assert(plasticity >= 0.5f && plasticity <= 1.5f);
        
        std::cout << "  Dopamine initialization test PASSED" << std::endl;
    }
    
    void testRewardSignaling() {
        nlm::Dopamine dopamine;
        
        // Signal reward with prediction
        dopamine.signalReward(1.0f, 0.5f);
        
        // Check that prediction error was computed
        float rpe = dopamine.getPredictionError();
        assert(std::abs(rpe - 0.5f) < 0.01f);
        
        // Check value function update
        float valueFunc = dopamine.getValueFunction();
        assert(valueFunc > 0.0f);
        
        // Test immediate reward
        dopamine.signalImmediateReward(0.8f);
        assert(dopamine.getPhasicComponent() > 0.0f);
        
        std::cout << "  Dopamine reward signaling test PASSED" << std::endl;
    }
    
    void testTemporalDynamics() {
        nlm::Dopamine dopamine;
        
        // Set custom time constants
        dopamine.setTimeConstants(0.5f, 100.0f);
        
        // Test phasic gain
        dopamine.setPhasicGain(2.0f);
        
        // Test tonic baseline
        dopamine.setTonicBaseline(0.1f);
        
        // Update for some time
        float level1 = dopamine.getLevel();
        dopamine.update(0.1f);
        float level2 = dopamine.getLevel();
        
        // Levels should be similar (no strong phasic input)
        assert(std::abs(level1 - level2) < 0.1f);
        
        std::cout << "  Dopamine temporal dynamics test PASSED" << std::endl;
    }
    
    void testPlasticityModulation() {
        nlm::Dopamine dopamine;
        
        // Set up STDP modulation parameters
        dopamine.setSTDPModulationParameters(1.2f, 0.8f, 0.01f);
        
        // Test plasticity factor with different prediction errors
        float rpe1 = 0.5f;
        dopamine.signalPredictionError(rpe1);
        float plasticity1 = dopamine.getPlasticityFactor();
        
        float rpe2 = -0.5f;
        dopamine.signalPredictionError(rpe2);
        float plasticity2 = dopamine.getPlasticityFactor();
        
        // Both should have enhanced plasticity
        assert(plasticity1 > 1.0f);
        assert(plasticity2 > 1.0f);
        
        std::cout << "  Dopamine plasticity modulation test PASSED" << std::endl;
    }
    
    void testValueLearning() {
        nlm::Dopamine dopamine;
        
        // Initial value function
        float initialValue = dopamine.getValueFunction();
        
        // Learn from outcomes
        dopamine.updateValuePrediction(1.0f);
        dopamine.learnFromOutcome(0.8f);
        
        // Value function should have updated
        float finalValue = dopamine.getValueFunction();
        assert(finalValue != initialValue);
        assert(finalValue > 0.0f);
        
        std::cout << "  Dopamine value learning test PASSED" << std::endl;
    }
    
    void testParameterConfiguration() {
        nlm::Dopamine dopamine;
        
        // Get default parameters
        auto params = dopamine.getParameters();
        assert(!params.empty());
        
        // Set custom parameter
        dopamine.setParameter("phasic_gain", 3.0f);
        
        // Verify parameter was updated
        assert(dopamine.getParameters()["phasic_gain"] == 3.0f);
        
        // Verify internal state was updated
        dopamine.setPhasicGain(2.0f); // This should update the parameter
        assert(dopamine.getParameters()["phasic_gain"] == 2.0f);
        
        std::cout << "  Dopamine parameter configuration test PASSED" << std::endl;
    }
    
    void testLearningStatistics() {
        nlm::Dopamine dopamine;
        
        // Initial statistics
        auto stats = dopamine.getLearningStats();
        assert(stats.totalRewards >= 0.0f);
        assert(stats.totalPredictionErrors >= 0.0f);
        assert(stats.learningRate > 0.0f);
        assert(stats.predictionAccuracy >= 0.0f);
        assert(stats.explorationBonus >= 0.0f);
        assert(stats.exploitationValue >= 0.0f);
        
        // Signal some rewards to update statistics
        dopamine.signalReward(1.0f, 0.5f);
        dopamine.signalReward(0.5f, 0.3f);
        
        // Check that statistics were updated
        auto updatedStats = dopamine.getLearningStats();
        assert(updatedStats.totalRewards > stats.totalRewards);
        assert(updatedStats.totalPredictionErrors > stats.totalPredictionErrors);
        
        std::cout << "  Dopamine learning statistics test PASSED" << std::endl;
    }
    
    void testTemporalAdaptation() {
        nlm::Dopamine dopamine;
        
        // Test adaptation with different complexity levels
        dopamine.temporalAdaptation(1.0f); // Simple environment
        float learningRate1 = dopamine.getLearningStats().learningRate;
        
        dopamine.temporalAdaptation(5.0f); // Complex environment
        float learningRate2 = dopamine.getLearningStats().learningRate;
        
        // Complex environment should have higher learning rate
        assert(learningRate2 > learningRate1);
        
        std::cout << "  Dopamine temporal adaptation test PASSED" << std::endl;
    }
    
    void testIntegrationWithPrediction() {
        nlm::Dopamine dopamine;
        
        // Integrate with prediction system
        dopamine.integrateWithPredictionSystem(1.0f, 0.8f);
        
        // Should have updated value function and prediction error
        assert(dopamine.getValueFunction() > 0.0f);
        assert(dopamine.getPredictionError() != 0.0f);
        
        std::cout << "  Dopamine integration with prediction test PASSED" << std::endl;
    }
    
    void testResetFunctionality() {
        nlm::Dopamine dopamine;
        
        // Modify some state
        dopamine.signalReward(1.0f, 0.5f);
        dopamine.setPhasicGain(2.0f);
        
        // Reset
        dopamine.reset();
        
        // Check that state was reset
        assert(dopamine.getPhasicComponent() == 0.0f);
        assert(dopamine.getTonicComponent() == 0.05f); // Default tonic baseline
        assert(dopamine.getPredictionError() == 0.0f);
        assert(dopamine.getValueFunction() == 0.0f);
        
        std::cout << "  Dopamine reset functionality test PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Running Dopamine tests..." << std::endl;
        
        testInitialization();
        testRewardSignaling();
        testTemporalDynamics();
        testPlasticityModulation();
        testValueLearning();
        testParameterConfiguration();
        testLearningStatistics();
        testTemporalAdaptation();
        testIntegrationWithPrediction();
        testResetFunctionality();
        
        std::cout << "All Dopamine tests PASSED" << std::endl;
    }
}

// Alias for backward compatibility with test_main.cpp
namespace test_reward {
    void runAll() {
        test_dopamine::runAll();
    }
}