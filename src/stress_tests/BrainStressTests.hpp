// NLM Brain Stress Tests Header
// Comprehensive stress testing for Brain system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace stress_tests {

// Memory stress test results
struct MemoryStressTestResults {
    size_t neuronCount;
    float stressLevel;
    double totalTime;
    double stepsPerSecond;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    double stressImpactScore;
};

struct MemoryStressResults {
    size_t totalTests;
    std::vector<MemoryStressTestResults> testResults;
};

// CPU memory stress test results
struct CPUMemoryTestResults {
    size_t neuronCount;
    size_t steps;
    double totalTime;
    double stepsPerSecond;
    double synapseInitTime;
    double neuronInitTime;
    double synapseConnectTime;
    double neuronConnectTime;
    double synapseActivationTime;
    double neuronActivationTime;
    double stdpTime;
    double hebbianTime;
    size_t totalSpikeCount;
};

struct CPUMemoryResults {
    size_t totalTests;
    std::vector<CPUMemoryTestResults> testResults;
};

// Network stress test results
struct NetworkStressTestResults {
    size_t neuronCount;
    size_t regionCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double networkUtilization;
    NetworkStats stats;
};

struct NetworkStressResults {
    size_t totalTests;
    std::vector<NetworkStressTestResults> testResults;
};

// Plasticity stress test results
struct PlasticityStressTestResults {
    size_t neuronCount;
    float learningRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double stdpUpdateTime;
    double hebbianUpdateTime;
    PlasticityStats stats;
};

struct PlasticityResults {
    size_t totalTests;
    std::vector<PlasticityStressTestResults> testResults;
};

// Neuromodulation stress test results
struct NeuromodulationStressTestResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double neuromodulationStressScore;
    size_t dopamineActivations;
    size_t curiosityActivations;
    size_t noveltyActivations;
    NeuromodulationStats stats;
};

struct NeuromodulationResults {
    size_t totalTests;
    std::vector<NeuromodulationStressTestResults> testResults;
};

// Development stress test results
struct DevelopmentStressTestResults {
    size_t neuronCount;
    float accelerationFactor;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double developmentStability;
    DevelopmentStats stats;
};

struct DevelopmentResults {
    size_t totalTests;
    std::vector<DevelopmentStressTestResults> testResults;
};

// Statistics structures (declared in respective headers)
struct NetworkStats {
    size_t neuronCount;
    size_t synapseCount;
    size_t spikeCount;
};

struct PlasticityStats {
    size_t totalSpikeCount;
    double plasticityConvergenceRate;
};

struct NeuromodulationStats {
    size_t totalSpikeCount;
    double activationsPerStep;
};

struct DevelopmentStats {
    size_t totalSpikeCount;
    double developmentRate;
};

class BrainStressTests {
public:
    static void runMemoryStressTests();
    static void runCPUSTressTests();
    static void runNetworkStressTests();
    static void runPlasticityStressTests();
    static void runNeuromodulationStressTests();
    static void runDevelopmentStressTests();
    
private:
    static double calculateStressImpactScore(const MemoryAllocationProfiler::MemoryStats& memStats,
                                             size_t neuronCount, size_t steps);
    static double calculateNetworkUtilization(const NetworkStats& brainStats, size_t neuronCount);
    static double calculatePlasticityConvergenceRate(size_t neuronCount, float learningRate);
    static double calculateNeuromodulationStressScore(const NeuromodulationStats& neuromodStats, size_t neuronCount);
    static double calculateDevelopmentRate(size_t neuronCount, float accelerationFactor);
    static double calculateDevelopmentStability(const DevelopmentStats& devStats, size_t neuronCount);
    
    static void printMemoryStressResults(const MemoryStressResults& results);
    static void printCPUMemoryStressResults(const CPUMemoryResults& results);
    static void printNetworkStressResults(const NetworkStressResults& results);
    static void printPlasticityStressResults(const PlasticityResults& results);
    static void printNeuromodulationStressResults(const NeuromodulationResults& results);
    static void printDevelopmentStressResults(const DevelopmentResults& results);
};

} // namespace stress_tests
} // namespace nlm