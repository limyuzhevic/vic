#pragma once

/**
 * Advanced Experiment Framework for NLM
 * 
 * This framework provides sophisticated experiment capabilities for the NLM neural simulation,
 * going beyond basic testing to include comprehensive experimental analysis, parameter sweeps,
 * and advanced neural computation experiments.
 */

#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

/**
 * Advanced experiment configuration with extensive parameter options
 */
struct AdvancedExperimentConfig {
    // Core simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Learning parameters
    float learningRate;
    float plasticityModulation;
    bool enableStructuralPlasticity;
    bool enableRewardModulation;
    bool enableDevelopment;
    
    // Memory system parameters
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enableAssociativeMemory;
    bool enableMemoryReplay;
    float memoryConsolidationRate;
    
    // Neuromodulation parameters
    float dopamineBaseline;
    float curiosityLevel;
    float noveltyDetection;
    
    // Experiment parameters
    bool enableCheckpointing;
    bool enableProgressLogging;
    uint64_t checkpointInterval;
    std::string checkpointPath;
    
    // Analysis parameters
    bool enableStatisticalAnalysis;
    bool enableVisualization;
    bool enableComparativeAnalysis;
    
    // Advanced learning parameters
    float attentionStrength;
    float predictionAccuracyThreshold;
    float conceptFormationStability;
    
    // Experimental design parameters
    enum ExperimentType {
        Baseline,
        ParameterSweep,
        Comparative,
        LifelongLearning,
        Developmental,
        SocialLearning,
        MemoryFormation,
        CognitiveDevelopment,
        Optimization
    };
    
    ExperimentType experimentType;
    
    // Parameter sweep configuration
    std::map<std::string, std::vector<float>> parameterRanges;
    
    // Analysis configuration
    struct AnalysisConfig {
        bool computeFiringRateStatistics;
        bool computeSynapticWeightDynamics;
        bool computeMemoryRetention;
        bool computeLearningTrajectories;
        bool computeNetworkTopology;
        bool computeInformationProcessing;
        
        // Statistical analysis parameters
        bool computeCentralTendency;
        bool computeVariability;
        bool computeCorrelation;
        bool computeSignificanceTesting;
        float confidenceLevel;
        
        // Visualization parameters
        bool generateNeuralActivityPlots;
        bool generateConnectivityPlots;
        bool generateLearningCurves;
        bool generateMemoryRetentionPlots;
        std::string outputFormat;
    } analysis;
    
    AdvancedExperimentConfig() : maxSteps(10000), neuronCount(1000), regionCount(1),
                                 connectionProbability(0.1f), learningRate(0.01f),
                                 plasticityModulation(1.0f), enableStructuralPlasticity(true),
                                 enableRewardModulation(false), enableDevelopment(true),
                                 enableWorkingMemory(true), enableEpisodicMemory(true),
                                 enableAssociativeMemory(true), enableMemoryReplay(true),
                                 memoryConsolidationRate(0.3f), dopamineBaseline(0.5f),
                                 curiosityLevel(0.5f), noveltyDetection(0.5f),
                                 enableCheckpointing(true), enableProgressLogging(true),
                                 checkpointInterval(1000), enableStatisticalAnalysis(true),
                                 enableVisualization(true), enableComparativeAnalysis(true),
                                 attentionStrength(1.0f), predictionAccuracyThreshold(0.8f),
                                 conceptFormationStability(0.7f),
                                 experimentType(AdvancedExperimentConfig::Baseline) {}
    }
};

/**
 * Advanced experiment metrics with comprehensive statistical analysis
 */
struct AdvancedExperimentMetrics {
    // Basic performance metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEfficiency;
    float learningSpeed;
    
    // Memory system metrics
    float workingMemoryUtilization;
    float episodicMemoryRetention;
    float associativeMemoryStrength;
    float memoryReplayFrequency;
    
    // Neuromodulation metrics
    float neuromodulationEffectiveness;
    float rewardPredictionAccuracy;
    float curiosityDrivenExploration;
    float noveltyResponse;
    
    // Cognitive metrics
    float attentionalFocus;
    float planningAccuracy;
    float conceptClarity;
    float selfModelStability;
    
    // Development metrics
    float developmentalProgress;
    float plasticityMaturation;
    float experienceIntegration;
    float adaptationRate;
    
    // Network metrics
    float networkConnectivity;
    float informationFlow;
    float computationalEfficiency;
    float robustness;
    
    // Statistical metrics
    float meanReward;
    float rewardVariance;
    float rewardSkewness;
    float rewardKurtosis;
    float learningRateVariance;
    float explorationExploitationBalance;
    
    // Time series metrics
    std::vector<float> rewardHistory;
    std::vector<float> firingRateHistory;
    std::vector<float> synapticWeightHistory;
    std::vector<float> memoryStrengthHistory;
    std::vector<float> neuromodulationHistory;
    
    // Comparative metrics (for comparative experiments)
    std::map<std::string, float> comparativeScores;
    std::map<std::string, float> baselineScores;
    
    // Temporal metrics
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    size_t totalStepsExecuted;
    
    AdvancedExperimentMetrics() : totalReward(0.0f), avgFiringRate(0.0f), avgSynapticWeight(0.0f),
                                  memoryEfficiency(0.0f), learningSpeed(0.0f),
                                  workingMemoryUtilization(0.0f), episodicMemoryRetention(0.0f),
                                  associativeMemoryStrength(0.0f), memoryReplayFrequency(0.0f),
                                  neuromodulationEffectiveness(0.0f), rewardPredictionAccuracy(0.0f),
                                  curiosityDrivenExploration(0.0f), noveltyResponse(0.0f),
                                  attentionalFocus(0.0f), planningAccuracy(0.0f),
                                  conceptClarity(0.0f), selfModelStability(0.0f),
                                  developmentalProgress(0.0f), plasticityMaturation(0.0f),
                                  experienceIntegration(0.0f), adaptationRate(0.0f),
                                  networkConnectivity(0.0f), informationFlow(0.0f),
                                  computationalEfficiency(0.0f), robustness(0.0f),
                                  meanReward(0.0f), rewardVariance(0.0f), rewardSkewness(0.0f),
                                  rewardKurtosis(0.0f), learningRateVariance(0.0f),
                                  explorationExploitationBalance(0.0f), startTime(0),
                                  endTime(0), totalWallClockTime(0.0), totalStepsExecuted(0) {}
    }
};

/**
 * Advanced experiment runner with sophisticated experimental capabilities
 */
class AdvancedExperiment {
public:
    AdvancedExperiment();
    ~AdvancedExperiment();
    
    /**
     * Run an advanced experiment with comprehensive analysis
     */
    AdvancedExperimentMetrics run(const AdvancedExperimentConfig& config);
    
    /**
     * Run parameter sweep experiments
     */
    std::map<std::string, AdvancedExperimentMetrics> runParameterSweep(
        const AdvancedExperimentConfig& baseConfig,
        const std::map<std::string, std::vector<float>>& parameterRanges);
    
    /**
     * Run comparative experiments between different configurations
     */
    std::map<std::string, AdvancedExperimentMetrics> runComparative(
        const std::map<std::string, AdvancedExperimentConfig>& configs);
    
    /**
     * Run lifelong learning experiment with continuous adaptation
     */
    AdvancedExperimentMetrics runLifelongLearning(
        const AdvancedExperimentConfig& config,
        uint64_t totalLifespan);
    
    /**
     * Run developmental experiment with stage progression
     */
    AdvancedExperimentMetrics runDevelopmental(
        const AdvancedExperimentConfig& config,
        uint64_t developmentalStages);
    
    /**
     * Run memory formation and consolidation experiment
     */
    AdvancedExperimentMetrics runMemoryFormation(
        const AdvancedExperimentConfig& config,
        bool enableReplay,
        uint64_t consolidationInterval);
    
    /**
     * Run cognitive development experiment with attention and planning
     */
    AdvancedExperimentMetrics runCognitiveDevelopment(
        const AdvancedExperimentConfig& config,
        uint64_t cognitiveStages);
    
    /**
     * Run optimization experiment using evolutionary algorithms
     */
    AdvancedExperimentMetrics runOptimization(
        const AdvancedExperimentConfig& baseConfig,
        const std::map<std::string, std::vector<float>>& searchSpace,
        uint64_t generations);
    
    /**
     * Run integration verification for advanced systems
     */
    bool verifyAdvancedIntegration();
    
    /**
     * Run comprehensive analysis on experiment results
     */
    AdvancedExperimentMetrics analyzeResults(
        const std::vector<AdvancedExperimentMetrics>& metricsList);
    
    /**
     * Generate visualization data for experiment results
     */
    std::string generateVisualizationData(
        const AdvancedExperimentMetrics& metrics,
        const std::string& outputFormat);
    
    /**
     * Export experiment results to various formats
     */
    bool exportResults(
        const AdvancedExperimentMetrics& metrics,
        const std::string& filepath,
        const std::string& format);
    
    /**
     * Load previous experiment results for analysis
     */
    bool loadResults(std::vector<AdvancedExperimentMetrics>& metricsList,
                   const std::string& filepath);
    
    /**
     * Calculate similarity between two experiment results
     */
    float calculateSimilarity(const AdvancedExperimentMetrics& metrics1,
                             const AdvancedExperimentMetrics& metrics2);
    
    /**
     * Perform statistical analysis on experiment results
     */
    std::map<std::string, float> performStatisticalAnalysis(
        const std::vector<AdvancedExperimentMetrics>& metricsList);
    
    /**
     * Generate experiment summary report
     */
    std::string generateSummaryReport(
        const AdvancedExperimentMetrics& metrics,
        const std::string& experimentName);
    
    /**
     * Create visualization of learning curves
     */
    std::string createLearningCurveVisualization(
        const std::vector<float>& xData,
        const std::vector<float>& yData,
        const std::string& title,
        const std::string& xLabel,
        const std::string& yLabel);
    
    /**
     * Create visualization of network connectivity
     */
    std::string createConnectivityVisualization(
        const AdvancedExperimentMetrics& metrics,
        const std::string& visualizationType);
    
    /**
     * Create visualization of memory dynamics
     */
    std::string createMemoryDynamicsVisualization(
        const std::vector<float>& memoryStrengthHistory,
        const std::vector<float>& replayEvents);
    
    /**
     * Create visualization of neuromodulation effects
     */
    std::string createNeuromodulationVisualization(
        const std::vector<float>& neuromodulationHistory,
        const std::vector<float>& rewardHistory);
    
    /**
     * Create visualization of cognitive development
     */
    std::string createCognitiveDevelopmentVisualization(
        const std::vector<float>& cognitiveMetricsHistory,
        uint64_t developmentalStages);
    
    /**
     * Create visualization of comparative experiment results
     */
    std::string createComparativeResultsVisualization(
        const std::map<std::string, AdvancedExperimentMetrics>& results);

private:
    // Internal helper functions
    std::shared_ptr<Brain> createBrainWithConfig(const AdvancedExperimentConfig& config);
    void runSingleStep(std::shared_ptr<Brain> brain, uint64_t step, 
                      const AdvancedExperimentConfig& config);
    void applyNeuromodulation(std::shared_ptr<Brain> brain, 
                             const AdvancedExperimentConfig& config);
    void updateMemorySystems(std::shared_ptr<Brain> brain,
                           const AdvancedExperimentConfig& config,
                           uint64_t step);
    void updateCognitiveSystems(std::shared_ptr<Brain> brain,
                              const AdvancedExperimentConfig& config,
                              uint64_t step);
    void updateDevelopmentSystem(std::shared_ptr<Brain> brain,
                               const AdvancedExperimentConfig& config,
                               uint64_t step);
    void recordMetrics(std::shared_ptr<Brain> brain,
                      AdvancedExperimentMetrics& metrics,
                      const AdvancedExperimentConfig& config,
                      uint64_t step);
    
    // Statistical analysis functions
    float calculateMean(const std::vector<float>& data);
    float calculateVariance(const std::vector<float>& data, float mean);
    float calculateStandardDeviation(const std::vector<float>& data, float mean);
    float calculateSkewness(const std::vector<float>& data, float mean, float stddev);
    float calculateKurtosis(const std::vector<float>& data, float mean, float stddev);
    float calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y);
    float calculatePearsonCorrelation(const std::vector<float>& x, const std::vector<float>& y);
    
    // Random number generation
    float randomFloat(float min, float max);
    int randomInt(int min, int max);
    
    // File operations
    std::string generateTimestamp();
    std::string sanitizeFilename(const std::string& filename);
};