// run_behavioral_tests.cpp - Behavioral test runner for NLM
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "BehavioralTests.cpp"

namespace nlm {

// Behavioral test runner for validating learning and memory behavior
class BehavioralTestRunner {
public:
    void runTest() {
        TestRunner runner(true);  // Verbose mode for behavioral tests
        
        // Register behavioral tests
        register_memory_behavioral_tests(runner);
        register_learning_behavioral_tests(runner);
        register_continual_learning_tests(runner);
        register_generalization_tests(runner);
        register_prediction_behavioral_tests(runner);
        register_individual_difference_tests(runner);
        
        std::cout << "=== NLM Behavioral Test Suite ===" << std::endl;
        runner.runAll();
    }
private:
    void register_memory_behavioral_tests(TestRunner& runner) {
        TEST_CASE(BehavioralMemory, "Memory Retention Over Time");
        TEST_CASE(BehavioralMemory, "Memory Decay Pattern Tests");
        TEST_CASE(BehavioralMemory, "Memory Replay Effectiveness");
    }
    
    void register_learning_behavioral_tests(TestRunner& runner) {
        TEST_CASE(BehavioralLearning, "Learning Progress Over Development");
        TEST_CASE(BehavioralLearning, "Developmental Stage Testing");
        TEST_CASE(BehavioralLearning, "Critical Period Tests");
        TEST_CASE(BehavioralLearning, "Skill Acquisition Tests");
    }
    
    void register_continual_learning_tests(TestRunner& runner) {
        TEST_CASE(BehavioralContinual, "Continual Learning Without Catastrophic Forgetting");
        TEST_CASE(BehavioralContinual, "Task Interference Tests");
        TEST_CASE(BehavioralContinual, "Forgetting Curve Tests");
        TEST_CASE(BehavioralContinual, "Learning Consolidation Tests");
    }
    
    void register_generalization_tests(TestRunner& runner) {
        TEST_CASE(BehavioralGeneralization, "Generalization Across Contexts");
        TEST_CASE(BehavioralGeneralization, "Transfer Learning Tests");
        TEST_CASE(BehavioralGeneralization, "Abstract Concept Formation");
        TEST_CASE(BehavioralGeneralization, "Cross-Modal Generalization");
    }
    
    void register_prediction_behavioral_tests(TestRunner& runner) {
        TEST_CASE(BehavioralPrediction, "Prediction Accuracy Improvement");
        TEST_CASE(BehavioralPrediction, "Prediction Confidence Calibration");
        TEST_CASE(BehavioralPrediction, "Prediction Error Minimization");
        TEST_CASE(BehavioralPrediction, "Prediction Consistency Tests");
    }
    
    void register_individual_difference_tests(TestRunner& runner) {
        TEST_CASE(BehavioralIndividual, "Individual Differences from Different Histories");
        TEST_CASE(BehavioralIndividual, "Social Learning Effects");
        TEST_CASE(BehavioralIndividual, "Emergent Behaviors");
        TEST_CASE(BehavioralIndividual, "Developmental Trajectories");
    }
};

// Behavioral metrics validation
namespace BehavioralMetrics {
    bool validateLearningCurve(const std::vector<double>& curve, size_t expectedSteps, double expectedFinalPerformance = 0.9) {
        if (curve.size() != expectedSteps) {
            std::cout << "FAIL: Learning curve length mismatch. Expected " << expectedSteps 
                     << ", got " << curve.size() << std::endl;
            return false;
        }
        
        // Check for S-shaped learning curve
        double initial = curve[0];
        double final = curve.back();
        
        if (final < expectedFinalPerformance * 0.8) {
            std::cout << "FAIL: Final performance too low. Expected >=" << expectedFinalPerformance * 0.8 
                     << ", got " << final << std::endl;
            return false;
        }
        
        // Check for monotonic improvement (allow small fluctuations)
        bool improved = false;
        for (size_t i = 1; i < curve.size(); ++i) {
            if (curve[i] > curve[i-1] + 0.01) {  // Allow 0.01 fluctuations
                improved = true;
                break;
            }
        }
        
        if (!improved) {
            std::cout << "WARN: Learning curve shows minimal improvement" << std::endl;
        }
        
        std::cout << "PASS: Learning curve validation successful. "
                 << "Initial: " << initial << ", Final: " << final << std::endl;
        return true;
    }
    
    bool validateMemoryRetention(const std::vector<double>& retention, double expectedDecayRate = 0.05) {
        if (retention.size() < 3) {
            std::cout << "FAIL: Not enough time points for retention analysis" << std::endl;
            return false;
        }
        
        // Check exponential decay pattern
        for (size_t i = 2; i < retention.size(); ++i) {
            double expected = retention[i-1] * std::exp(-expectedDecayRate);
            double actual = retention[i];
            
            if (std::abs(actual - expected) > 0.1 * expected) {
                std::cout << "FAIL: Retention point " << i << " deviates from expected decay. "
                         << "Expected: " << expected << ", Actual: " << actual << std::endl;
                return false;
            }
        }
        
        std::cout << "PASS: Memory retention follows expected decay pattern" << std::endl;
        return true;
    }
    
    bool validateContinualLearning(const std::vector<double>& performance, size_t taskChangeFrequency) {
        if (performance.empty()) {
            std::cout << "FAIL: Empty performance data" << std::endl;
            return false;
        }
        
        // Check for catastrophic forgetting pattern
        bool catastrophicForgetting = false;
        for (size_t i = 1; i < performance.size(); ++i) {
            if (performance[i] < performance[i-1] * 0.5) {  // 50% drop indicates catastrophic forgetting
                catastrophicForgetting = true;
                break;
            }
        }
        
        if (catastrophicForgetting) {
            std::cout << "WARN: Potential catastrophic forgetting detected" << std::endl;
        }
        
        std::cout << "PASS: Continual learning validation completed" << std::endl;
        return true;
    }
    
    bool validateGeneralization(const std::vector<double>& performances, size_t trainingContexts = 3) {
        if (performances.size() < trainingContexts) {
            std::cout << "FAIL: Not enough contexts for generalization analysis" << std::endl;
            return false;
        }
        
        // Training contexts should have better performance
        double trainingPerformance = 0.0;
        for (size_t i = 0; i < std::min(trainingContexts, performances.size()); ++i) {
            trainingPerformance += performances[i];
        }
        trainingPerformance /= std::min(trainingContexts, performances.size());
        
        // Test contexts should have lower performance
        double testPerformance = 0.0;
        for (size_t i = trainingContexts; i < performances.size(); ++i) {
            testPerformance += performances[i];
        }
        testPerformance /= (performances.size() - trainingContexts);
        
        if (trainingPerformance < testPerformance * 0.8) {
            std::cout << "FAIL: Poor generalization. Training performance " << trainingPerformance 
                     << " < Test performance " << testPerformance * 0.8 << std::endl;
            return false;
        }
        
        std::cout << "PASS: Generalization validated. "
                 << "Training: " << trainingPerformance << ", Test: " << testPerformance << std::endl;
        return true;
    }}}

// Entry point for behavioral tests
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Behavioral Test Suite ===" << std::endl;
    
    // Parse command line arguments
    bool verbose = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
    }
    
    BehavioralTestRunner testSuite;
    testSuite.runTest();
    
    return 0;
}
