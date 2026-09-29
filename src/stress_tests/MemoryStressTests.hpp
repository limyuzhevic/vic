// NLM Memory System Stress Tests Header
// Comprehensive stress testing for Memory system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace stress_tests {

// Working memory stress test results
struct WorkingMemoryStressTestResults {
    size_t memorySize;
    float pressureLevel;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    double memoryPressureScore;
};

struct WorkingMemoryResults {
    size_t totalTests;
    std::vector<WorkingMemoryStressTestResults> testResults;
};

// Episodic memory stress test results
struct EpisodicMemoryStressTestResults {
    size_t episodeCount;
    float consolidationRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    double consolidationStressScore;
};

struct EpisodicMemoryResults {
    size_t totalTests;
    std::vector<EpisodicMemoryStressTestResults> testResults;
};

// Associative memory stress test results
struct AssociativeMemoryStressTestResults {
    size_t patternSet;
    float learningRate;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    double associationStressScore;
};

struct AssociativeMemoryResults {
    size_t totalTests;
    std::vector<AssociativeMemoryStressTestResults> testResults;
};

// Memory leak test results
struct LeakTestResult {
    size_t testSize;
    size_t iteration;
    double allocTime;
    double accessTime;
    double deallocTime;
    double peakMemoryKB;
    double totalMemoryKB;
    size_t allocationCount;
    size_t deallocationCount;
    double leakRate;
};

struct LeakDetectionResults {
    size_t totalTests;
    std::vector<LeakTestResult> testResults;
};

class MemoryStressTests {
public:
    static void runWorkingMemoryStressTests();
    static void runEpisodicMemoryStressTests();
    static void runAssociativeMemoryStressTests();
    static void runMemoryLeakTests();
    
private:
    static double calculateWorkingMemoryPressureScore(
        const std::shared_ptr<NeuralWorkingMemory>& workingMemory,
        size_t memorySize, float pressureLevel);
    static double calculateEpisodicMemoryConsolidationStress(
        const std::shared_ptr<NeuralEpisodicMemory>& episodicMemory,
        size_t episodeCount, float consolidationRate);
    static double calculateAssociativeMemoryStress(
        const std::shared_ptr<NeuralAssociativeMemory>& associativeMemory,
        size_t patternSet, float learningRate);
    static double calculateLeakRate(const MemoryAllocationProfiler::MemoryStats& memStats,
                                    size_t expectedSize, size_t iteration);
    
    static void printWorkingMemoryStressResults(const WorkingMemoryResults& results);
    static void printEpisodicMemoryStressResults(const EpisodicMemoryResults& results);
    static void printAssociativeMemoryStressResults(const AssociativeMemoryResults& results);
    static void printLeakDetectionResults(const LeakDetectionResults& results);
};

} // namespace stress_tests
} // namespace nlm