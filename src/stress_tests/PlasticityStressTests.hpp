// NLM Plasticity System Stress Tests Header
// Comprehensive stress testing for Plasticity system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace stress_tests {

// STDP stress test results
struct STDPStressTestResults {
    size_t neuronCount;
    size_t spikePairCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double avgSTDPUpdateTime;
    double convergenceRate;
    STDPStats stats;
};

struct STDPResults {
    size_t totalTests;
    std::vector<STDPStressTestResults> testResults;
};

// Hebbian stress test results
struct HebbianStressTestResults {
    size_t neuronCount;
    float learningRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double avgHebbianUpdateTime;
    double learningSpeed;
    HebbianStats stats;
};

struct HebbianResults {
    size_t totalTests;
    std::vector<HebbianStressTestResults> testResults;
};

// Structural plasticity stress test results
struct StructuralPlasticityStressTestResults {
    size_t neuronCount;
    float synaptogenesisRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double synaptogenesisEfficiency;
    double pruningEfficiency;
    double structuralScore;
    StructuralPlasticityStats stats;
};

struct StructuralPlasticityResults {
    size_t totalTests;
    std::vector<StructuralPlasticityStressTestResults> testResults;
};

// Metaplasticity stress test results
struct MetaplasticityStressTestResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double parameterStability;
    double avgSTDPTime;
    double avgHebbianTime;
    MetaplasticityStats stats;
};

struct MetaplasticityResults {
    size_t totalTests;
    std::vector<MetaplasticityStressTestResults> testResults;
};

// Statistics structures (declared in respective headers)
struct STDPStats {
    size_t totalSpikePairs;
    double stdpUpdateTime;
    double plasticityConvergenceRate;
};

struct HebbianStats {
    size_t totalConnections;
    double hebbianUpdateTime;
    double learningSpeed;
};

struct StructuralPlasticityStats {
    size_t totalSynapticEvents;
    double synaptogenesisTime;
    double pruningTime;
    double structuralPlasticityScore;
};

struct MetaplasticityStats {
    size_t parameterChangeFrequency;
    double stdpAdaptationRate;
    double hebbianAdaptationRate;
    double metaplasticityStability;
};

class PlasticityStressTests {
public:
    static void runSTDPStressTests();
    static void runHebbianStressTests();
    static void runStructuralPlasticityStressTests();
    static void runMetaplasticityStressTests();
    
private:
    static double calculateSTDPConvergenceRate(size_t neuronCount, size_t spikePairs);
    static double calculateHebbianLearningSpeed(size_t neuronCount, float learningRate);
    static double calculateStructuralPlasticityScore(size_t neuronCount, float synRate);
    static double calculateMetaplasticityStability(size_t neuronCount, size_t steps);
    
    static void printSTDPStressResults(const STDPResults& results);
    static void printHebbianStressResults(const HebbianResults& results);
    static void printStructuralPlasticityStressResults(const StructuralPlasticityResults& results);
    static void printMetaplasticityStressResults(const MetaplasticityResults& results);
};

} // namespace stress_tests
} // namespace nlm