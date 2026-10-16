#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include "experiments/Experiment.hpp"

namespace nlm {

/**
 * Phase result metrics for standardized testing
 */
struct PhaseTestResult {
    std::string phaseName;
    double overallScore;  // 0.0 to 1.0 scale
    std::unordered_map<std::string, double> systemScores;
    std::vector<std::string> passedTests;
    std::vector<std::string> failedTests;
    std::vector<std::string> notes;
    double executionTime;
    bool success;
    
    PhaseTestResult(const std::string& name) 
        : phaseName(name), overallScore(0.0), executionTime(0.0), success(false) {}
};

/**
 * Unified Phase Test Framework
 * Provides standardized testing for all NLM phases
 */
class UnifiedPhaseTestFramework {
public:
    using PhaseTestFunc = std::function<bool(Brain* brain, const std::string& testName, std::string& output)>;
    
    UnifiedPhaseTestFramework() = default;
    ~UnifiedPhaseTestFramework() = default;
    
    /**
     * Register a phase test function
     */
    void registerPhaseTest(const std::string& phaseName, PhaseTestFunc testFunc) {
        phaseTests[phaseName] = testFunc;
    }
    
    /**
     * Run tests for a specific phase
     */
    PhaseTestResult runPhaseTest(const std::string& phaseName, std::shared_ptr<Brain> brain, 
                                const std::string& configPath = "configs/default.cfg");
    
    /**
     * Run tests for all phases
     */
    std::vector<PhaseTestResult> runAllPhaseTests(std::shared_ptr<Config> config);
    
    /**
     * Get all registered phase names
     */
    std::vector<std::string> getRegisteredPhases() const {
        std::vector<std::string> names;
        for (const auto& pair : phaseTests) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * Generate summary report
     */
    std::string generateReport(const std::vector<PhaseTestResult>& results) const;
    
    /**
     * Save test results
     */
    bool saveResults(const std::vector<PhaseTestResult>& results, const std::string& filepath) const;
    
private:
    std::unordered_map<std::string, PhaseTestFunc> phaseTests;
    
    /**
     * Calculate overall score from system scores
     */
    double calculateOverallScore(const std::unordered_map<std::string, double>& systemScores) const;
    
    /**
     * Format system score for reporting
     */
    std::string formatSystemScore(const std::string& systemName, double score) const;
};

/**
 * Phase-specific test implementations
 */
class Phase2Tests {
public:
    static bool testLIFDynamics(Brain* brain, const std::string& testName, std::string& output);
    static bool testSTDPPlasticity(Brain* brain, const std::string& testName, std::string& output);
    static bool testStructuralPlasticity(Brain* brain, const std::string& testName, std::string& output);
    static bool testSpikePropagation(Brain* brain, const std::string& testName, std::string& output);
};

class Phase3Tests {
public:
    static bool testMemoryIntegration(Brain* brain, const std::string& testName, std::string& output);
    static bool testNeuromodulation(Brain* brain, const std::string& testName, std::string& output);
    static bool testDevelopment(Brain* brain, const std::string& testName, std::string& output);
    static bool testReplayConsolidation(Brain* brain, const std::string& testName, std::string& output);
};

class Phase4Tests {
public:
    static bool testPredictionSystem(Brain* brain, const std::string& testName, std::string& output);
    static bool testCognitiveSystems(Brain* brain, const std::string& testName, std::string& output);
    static bool testConceptFormation(Brain* brain, const std::string& testName, std::string& output);
    static bool testPlanning(Brain* brain, const std::string& testName, std::string& output);
};

class Phase5Tests {
public:
    static bool testPerformanceOptimizations(Brain* brain, const std::string& testName, std::string& output);
    static bool testMemoryPools(Brain* brain, const std::string& testName, std::string& output);
    static bool testParallelProcessing(Brain* brain, const std::string& testName, std::string& output);
    static bool testCheckpointSystem(Brain* brain, const std::string& testName, std::string& output);
};

class Phase6Tests {
public:
    static bool testIntegration(Brain* brain, const std::string& testName, std::string& output);
    static bool testMemoryNeuromodulation(Brain* brain, const std::string& testName, std::string& output);
    static bool testPredictionDevelopment(Brain* brain, const std::string& testName, std::string& output);
    static bool testCompleteBrainLoop(Brain* brain, const std::string& testName, std::string& output);
};

} // namespace nlm
