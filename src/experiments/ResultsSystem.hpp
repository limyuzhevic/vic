#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Comprehensive experiment result structure
struct ComprehensiveExperimentResult {
    // Metadata
    std::string experimentName;
    uint64_t seed;
    std::chrono::system_clock::time_point timestamp;
    std::string experimentType;
    std::map<std::string, std::string> metadata;
    
    // Core metrics from Experiment base class
    SimulationStep startStep;
    SimulationStep endStep;
    std::vector<Experiment::StepData> stepData;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<std::string> notes;
    
    // Phase-specific results
    struct Phase3Results {
        int episodes = 0;
        float totalReward = 0.0f;
        float finalEnergy = 0.0f;
        float finalNovelty = 0.0f;
        float finalCuriosity = 0.0f;
        float predictionError = 0.0f;
        float developmentalAge = 0.0f;
        std::string developmentalStage = "Initial";
        size_t totalSynapses = 0;
        float averageWeight = 0.0f;
        float maxWeight = 0.0f;
        size_t totalSpikes = 0;
        float averageFiringRate = 0.0f;
        int forwardCount = 0;
        int backwardCount = 0;
        int turnLeftCount = 0;
        int turnRightCount = 0;
        int interactCount = 0;
        int waitCount = 0;
    } phase3Results;
    
    struct Phase4Results {
        float predictionErrorInitial = 0.0f;
        float predictionErrorFinal = 0.0f;
        float predictionAccuracyImprovement = 0.0f;
        float workingMemoryRetentionInitial = 0.0f;
        float workingMemoryRetentionFinal = 0.0f;
        float workingMemoryCapacity = 0.0f;
        float episodicRecallAccuracy = 0.0f;
        size_t episodesStored = 0;
        float episodicInfluence = 0.0f;
        float associationStrength = 0.0f;
        size_t conceptsFormed = 0;
        float conceptStability = 0.0f;
        float generalizationAbility = 0.0f;
        float attentionSelectivity = 0.0f;
        float distractionResistance = 0.0f;
        float planningAccuracy = 0.0f;
        float multiStepSuccess = 0.0f;
        float planningConfidence = 0.0f;
        float selfPredictionAccuracy = 0.0f;
        float bodyAwareness = 0.0f;
        float selfOtherDistinction = 0.0f;
        float imitationAccuracy = 0.0f;
        float socialPredictionAccuracy = 0.0f;
        size_t observationsFromOthers = 0;
        float totalReward = 0.0f;
        float behaviorImprovement = 0.0f;
        float transferPerformance = 0.0f;
    } phase4Results;
    
    struct Phase6Results {
        time_t startTime;
        time_t endTime;
        double totalWallClockTime;
        float totalReward;
        float avgFiringRate;
        float memoryEpisodesStored;
        float noveltyLevel;
        float curiosityLevel;
        float dopamineLevel;
        bool memoryWorkingMemoryIntegrated;
        bool memoryEpisodicMemoryIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool developmentIntegrated;
        bool checkpointingWorks;
    } phase6Results;
    
    // Analysis results
    struct AnalysisResults {
        // Performance metrics
        float meanFiringRate;
        float stdDevFiringRate;
        float minFiringRate;
        float maxFiringRate;
        float meanSynapseCount;
        float stdDevSynapseCount;
        float meanNeuronCount;
        
        // Reward statistics
        float meanReward;
        float stdDevReward;
        float minReward;
        float maxReward;
        float totalReward;
        
        // Behavioral metrics
        float convergenceRate;
        float explorationRate;
        float exploitationRate;
        
        // Learning metrics
        float learningCurveSlope;
        float learningCurveAUC;
        float retentionIndex;
        
        // Spike analysis
        float meanSpikesPerStep;
        float spikeRate;
        
        // Stability metrics
        float temporalStability;
        float representationalStability;
        
        // Efficiency metrics
        float energyEfficiency;
        float timeEfficiency;
    } analysis;
    
    // Comparison data
    struct ComparisonResults {
        std::string experimentName;
        float performanceScore;
        float learningRate;
        float efficiencyScore;
        std::map<std::string, float> normalizedMetrics;
    } comparison;
    
    ComprehensiveExperimentResult() : 
        startStep(0), endStep(0), startTime(0), endTime(0), totalWallClockTime(0.0),
        totalReward(0.0f), avgFiringRate(0.0f), memoryEpisodesStored(0.0f),
        noveltyLevel(0.0f), curiosityLevel(0.0f), dopamineLevel(0.0f),
        memoryWorkingMemoryIntegrated(false), memoryEpisodicMemoryIntegrated(false),
        neuromodulationIntegrated(false), predictionIntegrated(false),
        developmentIntegrated(false), checkpointingWorks(false),
        meanFiringRate(0.0f), stdDevFiringRate(0.0f), minFiringRate(0.0f),
        maxFiringRate(0.0f), meanSynapseCount(0.0f), stdDevSynapseCount(0.0f),
        meanNeuronCount(0.0f), meanReward(0.0f), stdDevReward(0.0f),
        minReward(0.0f), maxReward(0.0f), convergenceRate(0.0f),
        explorationRate(0.0f), exploitationRate(0.0f), learningCurveSlope(0.0f),
        learningCurveAUC(0.0f), retentionIndex(0.0f), meanSpikesPerStep(0.0f),
        spikeRate(0.0f), temporalStability(0.0f), representationalStability(0.0f),
        energyEfficiency(0.0f), timeEfficiency(0.0f), performanceScore(0.0f),
        learningRate(0.0f), efficiencyScore(0.0f) {}
};

// Results analysis and visualization system
class ExperimentResultsSystem {
public:
    ExperimentResultsSystem();
    ~ExperimentResultsSystem();
    
    // Results collection and analysis
    static ComprehensiveExperimentResult collectResults(const std::shared_ptr<Experiment>& experiment);
    static void analyzeResults(ComprehensiveExperimentResult& result);
    static void calculatePerformanceMetrics(ComprehensiveExperimentResult& result);
    static void calculateStatisticalMetrics(ComprehensiveExperimentResult& result);
    static void calculateLearningMetrics(ComprehensiveExperimentResult& result);
    static void calculateEfficiencyMetrics(ComprehensiveExperimentResult& result);
    
    // Export functions
    static bool exportToJSON(const ComprehensiveExperimentResult& result, const std::string& filepath);
    static bool exportToCSV(const ComprehensiveExperimentResult& result, const std::string& filepath);
    static bool exportToText(const ComprehensiveExperimentResult& result, const std::string& filepath);
    static bool exportToBinary(const ComprehensiveExperimentResult& result, const std::string& filepath);
    
    // Comparison and aggregation
    static bool compareResults(const ComprehensiveExperimentResult& result1, 
                               const ComprehensiveExperimentResult& result2,
                               ComprehensiveExperimentResult::ComparisonResults& comparison);
    static std::vector<ComprehensiveExperimentResult> aggregateBatchResults(
        const std::vector<std::shared_ptr<Experiment>>& experiments);
    
    // Visualization
    static bool generateVisualization(const ComprehensiveExperimentResult& result, 
                                      const std::string& outputPath);
    static bool generateSummaryReport(const ComprehensiveExperimentResult& result,
                                     const std::string& reportPath);
    
    // File operations
    static std::vector<std::string> findResultFiles(const std::string& directory);
    static bool loadFromJSON(const std::string& filepath, ComprehensiveExperimentResult& result);
    static bool loadFromBinary(const std::string& filepath, ComprehensiveExperimentResult& result);
    
    // Statistics and metadata
    static void addMetadata(ComprehensiveExperimentResult& result, 
                           const std::string& key, const std::string& value);
    static void addPhase3Results(ComprehensiveExperimentResult& result, 
                                const Phase3Experiment& phase3Exp);
    static void addPhase4Results(ComprehensiveExperimentResult& result,
                                const Phase4Experiment& phase4Exp);
    static void addPhase6Results(ComprehensiveExperimentResult& result,
                                const Phase6IntegratedExperiment& phase6Exp);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm