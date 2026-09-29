// NLM Multi-Agent Benchmarks Header
// Comprehensive benchmarking for Multi-Agent system

#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace benchmarks {

// Inter-agent communication benchmark results
struct InterAgentCommunicationResults {
    size_t agentCount;
    size_t totalSteps;
    double totalTime;
    double stepsPerSecond;
    size_t totalMessages;
    double messagesPerSecond;
    double averageLatency;
    double throughput;
    CommunicationStats stats;
};

struct CommunicationBenchmarkResults {
    size_t totalTests;
    std::vector<InterAgentCommunicationResults> testResults;
};

// Coordination overhead benchmark results
struct CoordinationOverheadResults {
    struct TestResult {
        size_t agentCount;
        size_t integrationSteps;
        double totalTime;
        double stepsPerSecond;
        double totalCoordinationTime;
        double totalSynchronizationTime;
        double maxCoordinationOverhead;
        double averageCoordinationTime;
        double averageSynchronizationTime;
        double coordinationOverhead;
    };
    
    size_t totalTests;
    std::vector<TestResult> testResults;
};

// Social learning efficiency benchmark results
struct SocialLearningResults {
    struct TestResult {
        size_t agentCount;
        size_t learningSteps;
        double totalTime;
        double stepsPerSecond;
        double totalLearningEfficiency;
        double maxLearningGain;
        double averageLearningEfficiency;
        double maxLearningGain;
    };
    
    size_t totalTests;
    std::vector<TestResult> testResults;
};

// Competitive vs. cooperative learning benchmark results
struct CompetitiveCooperativeResults {
    struct TestResult {
        size_t agentCount;
        size_t learningSteps;
        double totalTime;
        double stepsPerSecond;
        double totalCooperativeEfficiency;
        double totalCompetitiveEfficiency;
        double maxCooperativeGain;
        double maxCompetitiveGain;
        double averageCooperativeEfficiency;
        double averageCompetitiveEfficiency;
    };
    
    size_t totalTests;
    std::vector<TestResult> testResults;
};

// Statistics structures (declared in respective headers)
struct CommunicationStats {
    size_t totalMessages;
    double averageLatency;
    double throughput;
};

struct CoordinationStats {
    double averageCoordinationTime;
    double synchronizationTime;
    double coordinationOverhead;
};

struct SocialLearningStats {
    double learningEfficiency;
    double learningGain;
};

struct CompetitiveCooperativeStats {
    double cooperativeEfficiency;
    double competitiveEfficiency;
    double learningGain;
};

class MultiAgentBenchmarks {
public:
    static void runInterAgentCommunicationBenchmarks();
    static void runCoordinationOverheadBenchmarks();
    static void runSocialLearningEfficiencyBenchmarks();
    static void runCompetitiveCooperativeLearningBenchmarks();
    
private:
    static double calculateCommunicationEfficiency(size_t agentCount, size_t steps);
    static double calculateCoordinationOverhead(size_t agentCount, size_t integrationSteps);
    static double calculateSocialLearningGain(size_t agentCount, size_t learningSteps);
    static double calculateLearningBalance(size_t agentCount, size_t steps);
    
    static void printInterAgentCommunicationResults(const CommunicationBenchmarkResults& results);
    static void printCoordinationOverheadResults(const CoordinationOverheadResults& results);
    static void printSocialLearningEfficiencyResults(const SocialLearningResults& results);
    static void printCompetitiveCooperativeLearningResults(const CompetitiveCooperativeResults& results);
};

} // namespace benchmarks
} // namespace nlm