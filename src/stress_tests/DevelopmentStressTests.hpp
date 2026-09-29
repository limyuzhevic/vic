// NLM Development System Stress Tests Header
// Comprehensive stress testing for Development system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace stress_tests {

// Critical period stress test results
struct CriticalPeriodStressTestResults {
    size_t neuronCount;
    float accelerationFactor;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double developmentAcceleration;
    double neuronInitTime;
    double synaptogenesisTime;
    CriticalPeriodStats stats;
};

struct CriticalPeriodResults {
    size_t totalTests;
    std::vector<CriticalPeriodStressTestResults> testResults;
};

// Sensitive period stress test results
struct SensitivePeriodStressTestResults {
    size_t neuronCount;
    float extensionFactor;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double extensionStability;
    double neuronExtensionTime;
    double synapticAdaptationTime;
    SensitivePeriodStats stats;
};

struct SensitivePeriodResults {
    size_t totalTests;
    std::vector<SensitivePeriodStressTestResults> testResults;
};

// Developmental trajectory stress test results
struct DevelopmentalTrajectoryStressTestResults {
    size_t neuronCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double trajectoryStability;
    double initialPhaseTime;
    double connectionPhaseTime;
    double maturationPhaseTime;
    double optimizationPhaseTime;
    TrajectoryStats stats;
};

struct TrajectoryResults {
    size_t totalTests;
    std::vector<DevelopmentalTrajectoryStressTestResults> testResults;
};

// Epigenetic stress test results
struct EpigeneticStressTestResults {
    size_t neuronCount;
    float epigeneticStress;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double epigeneticStability;
    double stressAdaptationTime;
    double geneExpressionTime;
    double networkReorganizationTime;
    EpigeneticStats stats;
};

struct EpigeneticResults {
    size_t totalTests;
    std::vector<EpigeneticStressTestResults> testResults;
};

// Statistics structures (declared in respective headers)
struct CriticalPeriodStats {
    double accelerationTime;
    double synaptogenesisTime;
    double criticalPeriodDevelopmentRate;
};

struct SensitivePeriodStats {
    double extensionTime;
    double adaptationTime;
    double sensitivePeriodStability;
};

struct TrajectoryStats {
    double totalDevelopmentTime;
    double initialDevelopmentPhase;
    double connectionEstablishmentPhase;
    double maturationPhase;
    double optimizationPhase;
    double trajectoryStability;
};

struct EpigeneticStats {
    double stressAdaptationTime;
    double geneExpressionTime;
    double networkReorganizationTime;
    double epigeneticStability;
};

class DevelopmentStressTests {
public:
    static void runCriticalPeriodAccelerationTests();
    static void runSensitivePeriodExtensionTests();
    static void runDevelopmentalTrajectoryTests();
    static void runEpigeneticStressTests();
    
private:
    static double calculateCriticalPeriodDevelopmentRate(size_t neuronCount, float accelerationFactor);
    static double calculateSensitivePeriodStability(size_t neuronCount, float extensionFactor);
    static double calculateTrajectoryStability(size_t neuronCount, size_t steps);
    static double calculateEpigeneticStability(size_t neuronCount, float epigeneticStress);
    
    static void printCriticalPeriodStressResults(const CriticalPeriodResults& results);
    static void printSensitivePeriodStressResults(const SensitivePeriodResults& results);
    static void printTrajectoryStressResults(const TrajectoryResults& results);
    static void printEpigeneticStressResults(const EpigeneticResults& results);
};

} // namespace stress_tests
} // namespace nlm