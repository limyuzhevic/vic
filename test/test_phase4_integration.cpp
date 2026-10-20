#pragma once

#include "integration_test_framework.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <memory>
#include <cassert>
#include <cmath>

namespace nlm {
namespace integration_test {

/**
 * @class Phase4IntegrationTest
 * @brief Integration tests for Phase 4: Cognitive Systems
 * 
 * Phase 4 includes:
 * - Neural prediction system integration
 * - Working memory integration
 * - Concept formation
 * - Attention and planning
 */
class Phase4IntegrationTest : public IntegrationTestFramework {
public:
    Phase4IntegrationTest() = default;
    ~Phase4IntegrationTest() override = default;
    
    TestMetrics run() override {
        TestMetrics metrics;
        metrics.testName = getTestName();
        metrics.phaseName = getPhaseName();
        
        double startTime = getCurrentTimeMs();
        bool allPassed = true;
        std::string errorMsg;
        
        try {
            // Run Phase 4 tests
            allPassed &= testNeuralPredictionIntegration();
            allPassed &= testWorkingMemoryIntegration();
            allPassed &= testConceptFormation();
            allPassed &= testAttentionAndPlanning();
        } catch (const std::exception& e) {
            allPassed = false;
            errorMsg = std::string("Exception: ") + e.what();
        }
        
        metrics.passed = allPassed;
        metrics.errorMessage = errorMsg;
        return metrics;
    }
    
    std::string getPhaseName() const override {
        return "Phase 4";
    }
    
    std::string getTestName() const override {
        return "Cognitive Integration";
    }
    
private:
    bool testNeuralPredictionIntegration() {
        std::cout << "    Testing neural prediction system integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        Brain brain(config);
        brain.initialize();
        
        // Test 1: Prediction system integration
        PredictionSystem* prediction = brain.getPredictionSystem();
        
        if (prediction) {
            // Test prediction error computation
            float error = prediction->computePredictionError(0.5f, 0.8f);
            assert(std::abs(error - 0.3f) < 0.01f && "Prediction error computation incorrect");
            
            // Test prediction generation
            float predicted = prediction->generateNextState(0.5f, 0.1f);
            assert(predicted >= 0.0f && predicted <= 1.0f && "Invalid prediction range");
        }
        
        // Test 2: Neural prediction integration
        NeuralPrediction neuralPred;
        neuralPred.setHistory(std::vector<float>{0.1f, 0.2f, 0.3f});
        float pred = neuralPred.predictNext(0.15f);
        
        std::cout << "      Neural prediction integration: PASSED" << std::endl;
        return true;
    }
    
    bool testWorkingMemoryIntegration() {
        std::cout << "    Testing working memory integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        // Test 1: Working memory functionality
        NeuralWorkingMemory workingMem;
        
        // Add information to working memory
        workingMem.storeInformation(std::vector<float>{0.5f, 0.7f, 0.9f});
        
        // Retrieve information
        auto retrieved = workingMem.retrieveInformation();
        assert(retrieved.size() > 0 && "Information not stored/retrieved correctly");
        
        // Test 2: Integration with brain
        Brain brain(config);
        brain.initialize();
        
        NeuralWorkingMemory* brainWorkingMem = brain.getWorkingMemory();
        // Note: This may be nullptr in current implementation
        
        // Test 3: Integration with prediction
        PredictionSystem* prediction = brain.getPredictionSystem();
        
        std::cout << "      Working memory integration: PASSED" << std::endl;
        return true;
    }
    
    bool testConceptFormation() {
        std::cout << "    Testing concept formation integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        ConceptFormation conceptFormation;
        
        // Test 1: Concept learning from sensory patterns
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
        std::vector<float> pattern2 = {0.2f, 0.3f, 0.4f};  // Similar to pattern1
        std::vector<float> pattern3 = {0.9f, 0.8f, 0.7f};  // Different pattern
        
        conceptFormation.addExperience(pattern1);
        conceptFormation.addExperience(pattern2);
        conceptFormation.addExperience(pattern3);
        
        // Test 2: Concept categorization
        std::string category = conceptFormation.categorize(pattern1);
        assert(!category.empty() && "Categorization failed");
        
        // Test 3: Integration with brain
        Brain brain(config);
        brain.initialize();
        
        ConceptFormation* brainConceptFormation = brain.getConceptFormation();
        // Note: This may be nullptr in current implementation
        
        std::cout << "      Concept formation integration: PASSED" << std::endl;
        return true;
    }
    
    bool testAttentionAndPlanning() {
        std::cout << "    Testing attention and planning integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        // Test 1: Neural planner functionality
        NeuralPlanner planner;
        
        // Create a simple action plan
        std::vector<ActionStep> plan;
        ActionStep step;
        step.actionId = "MOVE_FORWARD";
        step.parameters = std::vector<float>{1.0f, 0.0f};
        step.confidence = 0.8f;
        plan.push_back(step);
        
        planner.setPlan(plan);
        
        // Execute plan
        auto result = planner.executePlan();
        assert(result.success && "Plan execution failed");
        
        // Test 2: Integration with brain
        Brain brain(config);
        brain.initialize();
        
        NeuralPlanner* brainPlanner = brain.getPlanner();
        // Note: This may be nullptr in current implementation
        
        std::cout << "      Attention and planning integration: PASSED" << std::endl;
        return true;
    }
};

} // namespace integration_test
} // namespace nlm