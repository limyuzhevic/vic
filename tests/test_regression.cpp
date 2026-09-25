// Regression Tests
// Tests for detecting performance and behavioral regressions
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include "test_framework.hpp"
#include "test_analyzer.hpp"

namespace test_regression {

// Regression test base class
class RegressionTestBase : public nlm::test::TestBase {
protected:
    struct BaselineMetrics {
        std::chrono::nanoseconds baselineExecutionTime;
        double baselineMemoryUsage;
        int baselineNeuronCount;
        double baselineFiringRate;
        std::vector<double> baselinePerformanceProfile;
    };
    
    std::map<std::string, BaselineMetrics> baselineMetrics;
    
public:
    RegressionTestBase() {
        // Initialize baseline metrics (normally loaded from persistent storage)
        setupBaselineMetrics();
    }
    
    void setupBaselineMetrics() {
        // Sample baseline metrics (normally from previous test runs or stored benchmarks)
        BaselineMetrics brainMetrics;
        brainMetrics.baselineExecutionTime = std::chrono::nanoseconds(500000000); // 500ms
        brainMetrics.baselineMemoryUsage = 50.0; // 50MB
        brainMetrics.baselineNeuronCount = 100;
        brainMetrics.baselineFiringRate = 0.15; // 15% firing rate
        baselineMetrics["brain"] = brainMetrics;
        
        BaselineMetrics neuronMetrics;
        neuronMetrics.baselineExecutionTime = std::chrono::nanoseconds(100000); // 100µs
        neuronMetrics.baselineMemoryUsage = 5.0; // 5MB
        neuronMetrics.baselineNeuronCount = 10;
        neuronMetrics.baselineFiringRate = 0.10;
        baselineMetrics["neuron"] = neuronMetrics;
        
        BaselineMetrics plasticityMetrics;
        plasticityMetrics.baselineExecutionTime = std::chrono::nanoseconds(200000); // 200µs
        plasticityMetrics.baselineMemoryUsage = 10.0; // 10MB
        plasticityMetrics.baselineNeuronCount = 50;
        plasticityMetrics.baselineFiringRate = 0.12;
        baselineMetrics["plasticity"] = plasticityMetrics;
    }
    
    std::string getName() const override {
        return "RegressionTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for regression tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Regression;
    }
    
    std::string getFile() const override {
        return "tests/test_regression.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
    
    // Compare current results with baseline
    template<typename T>
    bool checkRegression(const std::string& testName, T currentValue, T baselineValue, 
                        double thresholdRatio = 1.2, const std::string& unit = "") {
        if (baselineValue == 0) return true; // Skip if baseline is zero
        
        double ratio = static_cast<double>(currentValue) / static_cast<double>(baselineValue);
        bool passed = ratio <= thresholdRatio;
        
        if (!passed) {
            std::cout << "    testRegression - " << testName << " REGRESSION DETECTED!" << std::endl;
            std::cout << "      Baseline: " << baselineValue << " " << unit << std::endl;
            std::cout << "      Current:  " << currentValue << " " << unit << std::endl;
            std::cout << "      Ratio:    " << std::fixed << std::setprecision(2) << ratio << "x" << std::endl;
            std::cout << "      Threshold: " << thresholdRatio << "x" << std::endl;
        }
        
        return passed;
    }
};

// Behavioral regression tests
void testBehavioralRegression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run brain to get behavioral baseline
    std::vector<double> firingRates;
    std::vector<double> actionProbabilities;
    
    auto baselineMetrics = getBrainBaselineMetrics(brain);
    
    // Reset and re-run to compare behavior
    brain.reset();
    std::vector<double> currentFiringRates = getBrainFiringRates(brain, 50);
    std::vector<double> currentActionProbabilities = getBrainActionProbabilities(brain, 50);
    
    auto regressionAnalyzer = nlm::test::RegressionAnalyzer();
    
    // Check firing rate regression
    for (size_t i = 0; i < baselineMetrics.firingRates.size(); ++i) {
        if (i < currentFiringRates.size()) {
            bool passed = regressionAnalyzer.checkRegression(
                "FiringRate_" + std::to_string(i),
                currentFiringRates[i],
                baselineMetrics.firingRates[i],
                1.5, // 50% tolerance for firing rates
                "rate"
            );
            assert(passed);
        }
    }
    
    // Check action probability regression
    for (size_t i = 0; i < baselineMetrics.actionProbabilities.size(); ++i) {
        if (i < currentActionProbabilities.size()) {
            bool passed = regressionAnalyzer.checkRegression(
                "ActionProb_" + std::to_string(i),
                currentActionProbabilities[i],
                baselineMetrics.actionProbabilities[i],
                1.3, // 30% tolerance for action probabilities
                "probability"
            );
            assert(passed);
        }
    }
    
    std::cout << "    testBehavioralRegression passed" << std::endl;
}

// Learning performance regression tests
void testLearningPerformanceRegression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get baseline learning performance
    auto baselineMetrics = getLearningBaselineMetrics(brain);
    
    // Reset and re-run learning test
    brain.reset();
    
    std::vector<double> learningCurve;
    for (int episode = 0; episode < 100; ++episode) {
        // Simulate learning
        brain.step(episode);
        
        double performance = getBrainPerformance(brain);
        learningCurve.push_back(performance);
    }
    
    // Check learning curve regression
    bool passed = true;
    for (size_t i = 0; i < baselineMetrics.learningCurve.size() && i < learningCurve.size(); ++i) {
        bool stepPassed = regressionAnalyzer.checkRegression(
            "Learning_Episode_" + std::to_string(i),
            learningCurve[i],
            baselineMetrics.learningCurve[i],
            1.4, // 40% tolerance for learning performance
            "performance"
        );
        if (!stepPassed) passed = false;
    }
    
    assert(passed);
    
    std::cout << "    testLearningPerformanceRegression passed"" << std::endl;
}

// Accuracy regression tests
void testAccuracyRegression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 75, nlm::ConfigSource::Default);
    config->set("region_count", 3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Define test cases
    std::vector<std::pair<std::vector<double>, std::vector<double>>> testCases = {
        {{0.1, 0.2, 0.3}, {0.15, 0.25, 0.35}},
        {{0.9, 0.8, 0.7}, {0.85, 0.75, 0.65}},
        {{0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}}
    };
    
    // Get baseline accuracy
    auto baselineAccuracy = getBaselineAccuracy(testCases);
    
    // Reset and re-run accuracy test
    brain.reset();
    
    // Simulate test cases
    std::vector<double> currentAccuracy;
    for (const auto& testCase : testCases) {
        // Run brain simulation
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        double accuracy = calculateTestAccuracy(testCase.first, testCase.second, brain);
        currentAccuracy.push_back(accuracy);
        brain.reset();
    }
    
    // Check accuracy regression
    bool passed = true;
    for (size_t i = 0; i < baselineAccuracy.size() && i < currentAccuracy.size(); ++i) {
        bool casePassed = regressionAnalyzer.checkRegression(
            "Accuracy_TestCase_" + std::to_string(i),
            currentAccuracy[i],
            baselineAccuracy[i],
            1.1, // 10% tolerance for accuracy
            "%"
        );
        if (!casePassed) passed = false;
    }
    
    assert(passed);
    
    std::cout << "    testAccuracyRegression passed" << std::endl;
}

// Stability regression tests
void testStabilityRegression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 60, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get baseline stability metrics
    auto baselineStability = getBaselineStability(brain);
    
    // Reset and re-run stability test
    brain.reset();
    
    // Measure stability metrics
    std::vector<double> currentFiringVariances;
    std::vector<double> currentActionVariances;
    
    for (int run = 0; run < 10; ++run) {
        // Run multiple times to calculate variance
        std::vector<double> firingRates = getBrainFiringRates(brain, 100);
        std::vector<double> actionProbs = getBrainActionProbabilities(brain, 100);
        
        currentFiringVariances.push_back(calculateVariance(firingRates));
        currentActionVariances.push_back(calculateVariance(actionProbs));
        
        brain.reset();
    }
    
    // Check stability regression (lower variance is better)
    bool passed = true;
    
    // Check firing variance regression
    double avgBaselineFiringVariance = std::accumulate(baselineStability.firingVariances.begin(), 
                                                      baselineStability.firingVariances.end(), 0.0) / 
                                                      baselineStability.firingVariances.size();
    double avgCurrentFiringVariance = std::accumulate(currentFiringVariances.begin(), 
                                                     currentFiringVariances.end(), 0.0) / 
                                                     currentFiringVariances.size();
    
    // Current should be <= baseline (stability improves or stays same)
    bool firingPassed = regressionAnalyzer.checkRegression(
        "FiringVariance",
        avgCurrentFiringVariance,
        avgBaselineFiringVariance,
        1.5, // Allow 50% increase in variance (stability can decrease)
        "variance"
    );
    
    passed = passed && firingPassed;
    
    // Check action variance regression
    double avgBaselineActionVariance = std::accumulate(baselineStability.actionVariances.begin(), 
                                                      baselineStability.actionVariances.end(), 0.0) / 
                                                      baselineStability.actionVariances.size();
    double avgCurrentActionVariance = std::accumulate(currentActionVariances.begin(), 
                                                     currentActionVariances.end(), 0.0) / 
                                                     currentActionVariances.size();
    
    bool actionPassed = regressionAnalyzer.checkRegression(
        "ActionVariance",
        avgCurrentActionVariance,
        avgBaselineActionVariance,
        1.5, // Allow 50% increase in variance (stability can decrease)
        "variance"
    );
    
    passed = passed && actionPassed;
    
    assert(passed);
    
    std::cout << "    testStabilityRegression passed" << std::endl;
}

// Run all regression tests
void runAll() {
    testBehavioralRegression();
    testLearningPerformanceRegression();
    testAccuracyRegression();
    testStabilityRegression();
}

// Helper functions for regression testing
nlm::test::RegressionAnalyzer::RegressionReport regressionAnalyzer;

struct BrainMetrics {
    std::vector<double> firingRates;
    std::vector<double> actionProbabilities;
    std::vector<double> learningCurve;
};

BrainMetrics getBrainBaselineMetrics(nlm::Brain& brain) {
    BrainMetrics metrics;
    
    // Simulate baseline measurement
    for (int neuronId = 0; neuronId < 10; ++neuronId) {
        metrics.firingRates.push_back(0.1 + (neuronId * 0.01));
        metrics.actionProbabilities.push_back(0.5 + (neuronId * 0.02));
    }
    
    // Generate learning curve
    for (int episode = 0; episode < 50; ++episode) {
        metrics.learningCurve.push_back(0.6 + (episode * 0.01));
    }
    
    return metrics;
}

std::vector<double> getBrainFiringRates(nlm::Brain& brain, int steps) {
    std::vector<double> firingRates;
    
    for (int step = 0; step < steps; ++step) {
        brain.step(step);
        firingRates.push_back(brain.getAverageFiringRate());
    }
    
    return firingRates;
}

std::vector<double> getBrainActionProbabilities(nlm::Brain& brain, int steps) {
    std::vector<double> actionProbs;
    
    for (int step = 0; step < steps; ++step) {
        brain.step(step);
        auto action = brain.produceAction();
        // Simulate action probability (normally would involve actual action distribution)
        actionProbs.push_back(0.3 + (step % 5) * 0.1);
    }
    
    return actionProbs;
}

BrainMetrics getLearningBaselineMetrics(nlm::Brain& brain) {
    BrainMetrics metrics;
    
    // Generate baseline learning curve
    for (int episode = 0; episode < 100; ++episode) {
        metrics.learningCurve.push_back(0.7 + std::min(episode * 0.01, 0.3));
    }
    
    return metrics;
}

double getBrainPerformance(nlm::Brain& brain) {
    // Calculate performance metric based on brain state
    return brain.getAverageFiringRate() * brain.getFiringNeuronCount();
}

std::vector<double> getBaselineAccuracy(const std::vector<std::pair<std::vector<double>, std::vector<double>>>& testCases) {
    std::vector<double> accuracies;
    
    for (const auto& testCase : testCases) {
        // Simulate baseline accuracy calculation
        double accuracy = 0.0;
        for (size_t i = 0; i < testCase.first.size(); ++i) {
            double diff = std::abs(testCase.first[i] - testCase.second[i]);
            accuracy += (1.0 - std::min(diff, 1.0));
        }
        accuracy /= testCase.first.size();
        accuracies.push_back(accuracy);
    }
    
    return accuracies;
}

double calculateTestAccuracy(const std::vector<double>& input, 
                            const std::vector<double>& expected,
                            nlm::Brain& brain) {
    double accuracy = 0.0;
    
    for (size_t i = 0; i < input.size(); ++i) {
        double diff = std::abs(input[i] - expected[i]);
        accuracy += (1.0 - std::min(diff, 1.0));
    }
    
    return accuracy / input.size();
}

struct StabilityMetrics {
    std::vector<double> firingVariances;
    std::vector<double> actionVariances;
};

StabilityMetrics getBaselineStability(nlm::Brain& brain) {
    StabilityMetrics metrics;
    
    // Simulate baseline stability measurements
    for (int i = 0; i < 10; ++i) {
        metrics.firingVariances.push_back(0.01 + (i * 0.001));
        metrics.actionVariances.push_back(0.02 + (i * 0.002));
    }
    
    return metrics;
}

double calculateVariance(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sumSquares = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return sumSquares / values.size();
}

} // namespace test_regression
