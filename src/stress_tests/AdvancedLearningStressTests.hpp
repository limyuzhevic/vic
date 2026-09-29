// NLM Advanced Learning Stress Tests Header
// Comprehensive stress testing for Advanced Learning system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace stress_tests {

// Transfer learning stress test results
struct TransferLearningStressTestResults {
    size_t baseTaskSize;
    float transferStrength;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double totalTransferEfficiency;
    double maxTransferGain;
    double averageTransferTime;
    double averageTransferEfficiency;
    TransferLearningStats stats;
};

struct TransferLearningResults {
    size_t totalTests;
    std::vector<TransferLearningStressTestResults> testResults;
};

// Meta-learning stress test results
struct MetaLearningStressTestResults {
    size_t taskVariety;
    float metaLearningRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double totalMetaLearningEfficiency;
    double maxMetaLearningGain;
    double averageMetaLearningTime;
    double averageMetaLearningEfficiency;
    double maxMetaLearningGain;
    MetaLearningStats stats;
};

struct MetaLearningResults {
    size_t totalTests;
    std::vector<MetaLearningStressTestResults> testResults;
};

// Fast synaptic plasticity stress test results
struct FastPlasticityStressTestResults {
    size_t neuronCount;
    float plasticityRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double totalPlasticityEffect;
    double maxPlasticityChange;
    double averagePlasticityTime;
    double averagePlasticityEffect;
    double maxPlasticityChange;
    double averagePlasticityTime;
    FastPlasticityStats stats;
};

struct FastPlasticityResults {
    size_t totalTests;
    std::vector<FastPlasticityStressTestResults> testResults;
};

// Bayesian learning stress test results
struct BayesianLearningStressTestResults {
    size_t beliefSystemSize;
    float learningPrior;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double totalPosteriorUpdate;
    double maxPosteriorChange;
    double averageBeliefConvergence;
    double averagePosteriorUpdate;
    double maxPosteriorChange;
    double averageBeliefConvergence;
    BayesianLearningStats stats;
};

struct BayesianLearningResults {
    size_t totalTests;
    std::vector<BayesianLearningStressTestResults> testResults;
};

// Statistics structures (declared in respective headers)
struct TransferLearningStats {
    size_t totalBaseTasks;
    size_t totalTransferTasks;
    double averageTransferTime;
    double transferEfficiency;
};

struct MetaLearningStats {
    size_t totalTasksLearned;
    double averageMetaLearningTime;
    double metaLearningEfficiency;
};

struct FastPlasticityStats {
    size_t totalPlasticityEvents;
    double averagePlasticityTime;
    double plasticityEfficiency;
};

struct BayesianLearningStats {
    size_t totalBeliefs;
    double averageBeliefConvergenceRate;
    double bayesianEfficiency;
};

class AdvancedLearningStressTests {
public:
    static void runTransferLearningStressTests();
    static void runMetaLearningStressTests();
    static void runFastSynapticPlasticityStressTests();
    static void runBayesianLearningStressTests();
    
private:
    static double calculateTransferLearningEfficiency(size_t baseTaskSize, float transferStrength);
    static double calculateMetaLearningEfficiency(size_t taskVariety, float metaLearningRate);
    static double calculateFastPlasticityEfficiency(size_t neuronCount, float plasticityRate);
    static double calculateBayesianLearningEfficiency(size_t beliefSystem, float learningPrior);
    
    static void printTransferLearningStressResults(const TransferLearningResults& results);
    static void printMetaLearningStressResults(const MetaLearningResults& results);
    static void printFastPlasticityStressResults(const FastPlasticityResults& results);
    static void printBayesianLearningStressResults(const BayesianLearningResults& results);
};

} // namespace stress_tests
} // namespace nlm