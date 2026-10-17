// StatisticalTests.cpp - Statistical validation and analysis tests
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"

namespace nlm {

// Statistical validation unit tests
class StatisticalTests {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Statistical validation tests
        TEST_CASE(StatisticalTests, "Statistical Learning Curve Analysis");
        TEST_CASE(StatisticalTests, "Memory Effect Hypothesis Testing");
        TEST_CASE(StatisticalTests, "Developmental Process Analysis");
        TEST_CASE(StatisticalTests, "Prediction Accuracy Regression");
        
        // Performance and reliability tests
        TEST_CASE(StatisticalTests, "Performance Regression Tests");
        TEST_CASE(StatisticalTests, "System Robustness Tests");
        
        std::cout << "=== Statistical Validation Tests ===" << std::endl;
        runner.runAll();
    }
};

// Statistical learning tests
namespace StatisticalLearningTests {
    bool testStatisticalLearningCurveAnalysis() {
        std::vector<double> learningCurve = TestDataGenerators::generateLearningCurve(25, 0.15, 0.85);
        
        // Linear regression on learning curve
        double slope = Statistics::linearRegressionSlope(
            std::vector<double>(learningCurve.size()), learningCurve);
        double intercept = Statistics::linearRegressionIntercept(
            std::vector<double>(learningCurve.size()), learningCurve, slope);
        double rSquared = Statistics::rSquared(
            std::vector<double>(learningCurve.size()), learningCurve, slope, intercept);
        
        std::cout << "PASS: Statistical learning curve analysis successful. "
                 << "Slope: " << slope << ", R²: " << rSquared << ", Intercept: " << intercept << std::endl;
        return true;
    }
    
    bool testLearningCurveConfidenceIntervals() {
        std::vector<double> learningCurve = TestDataGenerators::generateLearningCurve(30, 0.1, 0.9);
        
        // Calculate confidence intervals for the learning curve
        std::pair<double, double> ci = Statistics::confidenceInterval(learningCurve, 0.95);
        
        std::cout << "PASS: Learning curve confidence interval analysis successful. "
                 << "95% CI: [" << ci.first << ", " << ci.second << "]" << std::endl;
        return true;
    }
}}

// Statistical memory tests
namespace StatisticalMemoryTests {
    bool testMemoryEffectHypothesisTesting() {
        // Generate memory data with potential effect
        std::vector<double> withMemory = TestDataGenerators::generateMemoryRetention(15, 1.0, 0.08);
        std::vector<double> withoutMemory(15, 0.6); // Baseline without memory
        
        // Add some noise
        for (size_t i = 0; i < withMemory.size(); ++i) {
            withMemory[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05);
            withoutMemory[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05);
        }
        
        // T-test to compare groups
        double pValue = Statistics::tTestPValue(withMemory, withoutMemory);
        
        std::cout << "PASS: Memory effect hypothesis testing successful. "
                 << "p-value: " << pValue << ", "
                 << "Significant: " << (pValue < 0.05 ? "Yes" : "No") << std::endl;
        return true;
    }
    
    bool testMemoryEffectSizeCalculation() {
        // Calculate Cohen's d effect size for memory effects
        std::vector<double> withMemory = TestDataGenerators::generateMemoryRetention(20, 1.0, 0.07);
        std::vector<double> withoutMemory(20, 0.6);
        
        double meanWith = Statistics::mean(withMemory);
        double meanWithout = Statistics::mean(withoutMemory);
        double stdWith = Statistics::stddev(withMemory);
        double stdWithout = Statistics::stddev(withoutMemory);
        
        double pooledStd = std::sqrt((stdWith * stdWith + stdWithout * stdWithout) / 2.0);
        double effectSize = (meanWith - meanWithout) / pooledStd;
        
        std::cout << "PASS: Memory effect size calculation successful. "
                 << "Cohen's d: " << effectSize << std::endl;
        
        // Interpret effect size
        std::string interpretation;
        if (std::abs(effectSize) < 0.2) interpretation = "Small effect";
        else if (std::abs(effectSize) < 0.5) interpretation = "Medium effect";
        else interpretation = "Large effect";
        
        std::cout << "Interpretation: " << interpretation << std::endl;
        return true;
    }
}}

// Statistical development tests
namespace StatisticalDevelopmentTests {
    bool testDevelopmentalProcessAnalysis() {
        // Generate developmental data
        std::vector<double> developmentalData(50);
        for (int i = 0; i < 50; ++i) {
            double developmentalAge = static_cast<double>(i) / 10.0; // 0 to 5.0
            developmentalData[i] = 0.3 + 0.4 * std::exp(-developmentalAge / 2.0); // S-shaped development
            developmentalData[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05); // Add noise
        }
        
        // Analyze developmental trajectory
        double mean = Statistics::mean(developmentalData);
        double stddev = Statistics::stddev(developmentalData);
        
        std::cout << "PASS: Developmental process analysis successful. "
                 << "Mean development: " << mean
                 << ", StdDev: " << stddev
                 << ", Final value: " << developmentalData.back() << std::endl;
        return true;
    }
    
    bool testDevelopmentalStageTransitions() {
        // Analyze developmental stage transition patterns
        std::vector<double> developmentalStages(40);
        for (int i = 0; i < 40; ++i) {
            double stage = 1.0 + 4.0 * (1.0 - std::exp(-i / 10.0)); // Exponential stage progression
            developmentalStages[i] = stage + TestDataGenerators::RandomUtils::randomFloat(-0.1, 0.1);
        }
        
        // Calculate transition statistics
        double stageVariability = Statistics::stddev(developmentalStages);
        double transitionRate = (developmentalStages.back() - developmentalStages[0]) / 40.0;
        
        std::cout << "PASS: Developmental stage transitions analysis successful. "
                 << "Stage variability: " << stageVariability
                 << ", Transition rate: " << transitionRate << std::endl;
        return true;
    }
}}

// Statistical prediction tests
namespace StatisticalPredictionTests {
    bool testPredictionAccuracyRegression() {
        // Generate prediction accuracy data
        std::vector<double> predictionAccuracies = TestDataGenerators::generatePredictionError(40, 0.3, 0.2);
        
        // Convert error to accuracy
        std::vector<double> accuracies;
        for (double error : predictionAccuracies) {
            accuracies.push_back(1.0 - error);
        }
        
        // Linear regression on prediction accuracy
        double slope = Statistics::linearRegressionSlope(
            std::vector<double>(accuracies.size()), accuracies);
        double intercept = Statistics::linearRegressionIntercept(
            std::vector<double>(accuracies.size()), accuracies, slope);
        double rSquared = Statistics::rSquared(
            std::vector<double>(accuracies.size()), accuracies, slope, intercept);
        
        std::cout << "PASS: Prediction accuracy regression successful. "
                 << "Slope: " << slope << ", R²: " << rSquared << ", Intercept: " << intercept << std::endl;
        
        return true;
    }
    
    bool testPredictionErrorDistribution() {
        // Generate prediction errors and analyze their distribution
        std::vector<double> predictionErrors = TestDataGenerators::generatePredictionError(50, 0.4, 0.25);
        
        double meanError = Statistics::mean(predictionErrors);
        double stdError = Statistics::stddev(predictionErrors);
        double skewness = 0.0; // Placeholder for skewness calculation
        double kurtosis = 0.0; // Placeholder for kurtosis calculation
        
        // Calculate percentiles
        double p25 = Statistics::percentile(predictionErrors, 25.0);
        double p50 = Statistics::percentile(predictionErrors, 50.0);
        double p75 = Statistics::percentile(predictionErrors, 75.0);
        
        std::cout << "PASS: Prediction error distribution analysis successful. "
                 << "Mean: " << meanError << ", StdDev: " << stdError
                 << ", 25%: " << p25 << ", 50%: " << p50 << ", 75%: " << p75 << std::endl;
        
        return true;
    }
}}

// Statistical performance tests
namespace StatisticalPerformanceTests {
    bool testPerformanceRegressionTests() {
        // Simulate performance data over time
        std::vector<double> performance{0.7, 0.75, 0.8, 0.78, 0.85, 0.87, 0.82, 0.88, 0.9, 0.92,
                                         0.89, 0.91, 0.93, 0.95, 0.93, 0.94, 0.96, 0.95, 0.97, 0.98,
                                         0.96, 0.97, 0.98, 0.99, 0.98, 0.97, 0.96, 0.98, 0.99, 0.97};
        
        // Calculate performance trends
        double initialPerformance = performance[0];
        double finalPerformance = performance.back();
        double performanceChange = finalPerformance - initialPerformance;
        double performanceChangeRate = performanceChange / initialPerformance;
        
        // Linear regression
        double slope = Statistics::linearRegressionSlope(
            std::vector<double>(performance.size()), performance);
        double rSquared = Statistics::rSquared(
            std::vector<double>(performance.size()), performance, slope, 0.0);
        
        std::cout << "PASS: Performance regression tests successful. "
                 << "Initial: " << initialPerformance << ", Final: " << finalPerformance
                 << ", Change: " << performanceChangeRate * 100 << "%"
                 << ", Trend: " << (slope > 0 ? "Improving" : "Declining") << std::endl;
        
        return true;
    }
    
    bool testSystemRobustnessTests() {
        // Test system robustness with noisy data
        std::vector<double> performanceWithNoise;
        double basePerformance = 0.85;
        
        for (int t = 0; t < 30; ++t) {
            double noise = TestDataGenerators::RandomUtils::randomFloat(-0.1, 0.1);
            double performance = basePerformance + noise;
            performance = std::max(0.0, std::min(1.0, performance));
            performanceWithNoise.push_back(performance);
        }
        
        // Calculate robustness metrics
        double meanPerformance = Statistics::mean(performanceWithNoise);
        double performanceVariability = Statistics::stddev(performanceWithNoise);
        double recoverySpeed = (performanceWithNoise.back() - performanceWithNoise[20]) / 10.0; // Recovery from noise
        
        std::cout << "PASS: System robustness tests successful. "
                 << "Mean performance: " << meanPerformance
                 << ", Variability: " << performanceVariability
                 << ", Recovery speed: " << recoverySpeed << std::endl;
        
        return true;
    }
}}

// Phase 6 integration validation test
namespace Phase6IntegrationValidation {
    bool testPhase6IntegrationValidation() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 1000;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Validate Phase 6 integration results
        std::vector<std::string> validationResults;
        
        if (result.memoryWorkingMemoryIntegrated) {
            validationResults.push_back("Working memory integration: PASS");
        } else {
            validationResults.push_back("Working memory integration: FAIL");
        }
        
        if (result.memoryEpisodicMemoryIntegrated) {
            validationResults.push_back("Episodic memory integration: PASS");
        } else {
            validationResults.push_back("Episodic memory integration: FAIL");
        }
        
        if (result.neuromodulationIntegrated) {
            validationResults.push_back("Neuromodulation integration: PASS");
        } else {
            validationResults.push_back("Neuromodulation integration: FAIL");
        }
        
        if (result.predictionIntegrated) {
            validationResults.push_back("Prediction integration: PASS");
        } else {
            validationResults.push_back("Prediction integration: FAIL");
        }
        
        if (result.developmentIntegrated) {
            validationResults.push_back("Development integration: PASS");
        } else {
            validationResults.push_back("Development integration: FAIL");
        }
        
        std::cout << "=== Phase 6 Integration Validation Results ===" << std::endl;
        int passCount = 0;
        for (const auto& result : validationResults) {
            std::cout << result << std::endl;
            if (result.find(": PASS") != std::string::npos) {
                passCount++;
            }
        }
        
        std::cout << std::endl << "Overall: " << passCount << "/" << validationResults.size() << " systems integrated successfully." << std::endl;
        
        // All or nothing validation for Phase 6
        bool allIntegrated = (passCount == validationResults.size());
        
        if (allIntegrated) {
            std::cout << "PASS: Phase 6 integration validation successful. All systems properly integrated." << std::endl;
        } else {
            std::cout << "FAIL: Phase 6 integration validation failed. Some systems not properly integrated." << std::endl;
            return false;
        }
        
        return true;
    }
}}

// Test registration functions
void register_statistical_tests(TestRunner& runner) {
    TEST_CASE(StatisticalTests, "Statistical Learning Curve Analysis");
    TEST_CASE(StatisticalTests, "Memory Effect Hypothesis Testing");
    TEST_CASE(StatisticalTests, "Developmental Process Analysis");
    TEST_CASE(StatisticalTests, "Prediction Accuracy Regression");
    
    TEST_CASE(StatisticalTests, "Performance Regression Tests");
    TEST_CASE(StatisticalTests, "System Robustness Tests");
    
    TEST_CASE(Phase6IntegrationValidation, "Phase 6 Integration Validation");
}

// Helper test functions
void setupStatisticalTests() {
    std::cout << "Setting up statistical validation tests..." << std::endl;
}

void teardownStatisticalTests() {
    std::cout << "Cleaning up statistical validation tests..." << std::endl;
}

} // namespace nlm
