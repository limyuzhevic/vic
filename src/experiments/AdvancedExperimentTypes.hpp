#pragma once

#include "../experiments/ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace nlm {

// Advanced experiment types for power users

// 1. Complex Continual Learning Experiment
class ComplexContinualLearningExperiment {
public:
    struct TaskDefinition {
        std::string name;
        std::string description;
        std::function<float(Brain*, std::chrono::system_clock::time_point)> evaluateFunction;
        uint64_t stepsBetweenTasks;
        float difficultyLevel;
        float rewardScaling;
        std::vector<std::string> prerequisites;
        
        TaskDefinition(const std::string& n, const std::string& desc) 
            : name(n), description(desc), difficultyLevel(0.5f), rewardScaling(1.0f) {}
    };
    
    struct LearningCurve {
        std::vector<std::chrono::system_clock::time_point> timestamps;
        std::vector<float> performanceScores;
        std::vector<float> rewardRates;
        std::vector<float> explorationRates;
        std::vector<size_t> taskCompletions;
        
        void addPoint(float performance, float reward, float exploration, size_t completions) {
            timestamps.push_back(std::chrono::system_clock::now());
            performanceScores.push_back(performance);
            rewardRates.push_back(reward);
            explorationRates.push_back(exploration);
            taskCompletions.push_back(completions);
        }
    };
    
    ComplexContinualLearningExperiment() = default;
    ~ComplexContinualLearningExperiment() = default;
    
    // Run complex continual learning with multiple task types
    LearningCurve run(
        std::shared_ptr<Brain> brain,
        const std::vector<TaskDefinition>& tasks,
        size_t cycles = 3,
        bool enableMetaLearning = false,
        bool enableTransferLearning = false,
        const std::string& transferTask = "");
    
    // Run task-specific learning with adaptive difficulty
    LearningCurve runAdaptiveLearning(
        std::shared_ptr<Brain> brain,
        const std::vector<TaskDefinition>& tasks,
        float initialDifficulty = 0.5f,
        float adaptationRate = 0.1f,
        bool enableFailureRecovery = true);
    
    // Run transfer learning experiment
    LearningCurve runTransferLearning(
        std::shared_ptr<Brain> brain,
        const std::string& sourceTask,
        const std::vector<std::string>& targetTasks,
        float transferStrength = 1.0f,
        bool enableFineTuning = true);
    
    // Analyze learning patterns
    struct LearningPatternAnalysis {
        float averageLearningRate;
        float learningStability;
        float transferEfficiency;
        float taskSwitchingCost;
        std::vector<float> taskSpecificLearningRates;
        float overallLearningCurveScore;
        
        LearningPatternAnalysis() : averageLearningRate(0), learningStability(0), 
                                   transferEfficiency(0), taskSwitchingCost(0), 
                                   overallLearningCurveScore(0) {}
    };
    
    LearningPatternAnalysis analyzeLearningPatterns(const LearningCurve& curve);
    
    // Generate detailed learning report
    std::string generateLearningReport(const LearningCurve& curve, const LearningPatternAnalysis& analysis);
};

// 2. Damage Recovery and Rehabilitation Experiment
class DamageRecoveryExperiment {
public:
    struct DamageProfile {
        std::string damageType;
        float damageSeverity; // 0.0 to 1.0
        std::string damageLocation;
        std::function<void(Brain*)> applyDamage;
        std::function<float(Brain*)> measureDamage;
        
        DamageProfile(const std::string& type, float severity) 
            : damageType(type), damageSeverity(severity) {}
    };
    
    struct RecoveryMetric {
        std::chrono::system_clock::time_point timestamp;
        float recoveryProgress; // 0.0 to 1.0
        float functionalCapacity;
        float synapticIntegrity;
        float behavioralPerformance;
        std::map<std::string, float> subsystemRecoveries;
        
        RecoveryMetric() : recoveryProgress(0), functionalCapacity(0), 
                          synapticIntegrity(0), behavioralPerformance(0) {}
    };
    
    DamageRecoveryExperiment() = default;
    ~DamageRecoveryExperiment() = default;
    
    // Run comprehensive damage recovery experiment
    std::vector<RecoveryMetric> run(
        std::shared_ptr<Brain> brain,
        const DamageProfile& damage,
        const std::vector<std::string>& rehabilitationStrategies,
        uint64_t recoverySteps = 10000,
        bool enableNeurogenesis = false,
        bool enablePlasticityBoost = false);
    
    // Compare different rehabilitation strategies
    struct StrategyComparison {
        std::string strategyName;
        float finalRecoveryRate;
        float recoveryTime;
        float resourceEfficiency;
        float longTermStability;
        std::vector<RecoveryMetric> recoveryCurve;
        
        StrategyComparison() : finalRecoveryRate(0), recoveryTime(0), 
                             resourceEfficiency(0), longTermStability(0) {}
    };
    
    std::vector<StrategyComparison> compareRehabilitationStrategies(
        std::shared_ptr<Brain> brain,
        const DamageProfile& damage,
        const std::vector<std::string>& strategies,
        uint64_t stepsPerStrategy = 5000);
    
    // Predict recovery trajectory
    struct RecoveryPrediction {
        std::vector<std::chrono::system_clock::time_point> predictedTimepoints;
        std::vector<float> predictedRecoveryProgress;
        std::vector<float> confidenceIntervals;
        float predictedFullRecoveryTime;
        bool isCompleteRecoveryPredicted;
        
        RecoveryPrediction() : predictedFullRecoveryTime(0), isCompleteRecoveryPredicted(false) {}
    };
    
    RecoveryPrediction predictRecoveryTrajectory(
        const std::vector<RecoveryMetric>& recoveryData,
        const DamageProfile& damage);
};

// 3. Large-Scale Scaling Experiment
class ScalingExperiment {
public:
    struct ScaleLevel {
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        float theoreticalCapacity;
        float empiricalPerformance;
        float resourceUtilization;
        float timeToConvergence;
        
        ScaleLevel(size_t neurons) : neuronCount(neurons), regionCount(1), 
                                    connectionProbability(0.1f), theoreticalCapacity(0), 
                                    empiricalPerformance(0), resourceUtilization(0), 
                                    timeToConvergence(0) {}
    };
    
    struct ScalingAnalysis {
        float scalingLawExponent;
        float saturationPoint;
        float efficiencyPeak;
        std::vector<ScaleLevel> scaleLevels;
        std::vector<std::pair<size_t, double>> performanceScaling;
        std::vector<std::pair<size_t, double>> resourceScaling;
        
        ScalingAnalysis() : scalingLawExponent(0), saturationPoint(0), efficiencyPeak(0) {}
    };
    
    ScalingExperiment() = default;
    ~ScalingExperiment() = default;
    
    // Run comprehensive scaling experiment
    ScalingAnalysis run(
        const std::vector<ScaleLevel>& scaleLevels,
        uint64_t stepsPerScale,
        bool enableMultithreading = true,
        bool enableSIMD = true,
        bool enableSparseConnectivity = true,
        const std::string& hardwareProfile = "standard");
    
    // Analyze scaling laws
    ScalingAnalysis analyzeScalingLaws(
        const std::vector<ScaleLevel>& scaleLevels,
        const std::string& scalingModel = "power_law"); // power_law, exponential, logarithmic
    
    // Predict performance at arbitrary scales
    struct PerformancePrediction {
        std::vector<size_t> targetScales;
        std::vector<double> predictedPerformance;
        std::vector<double> confidenceIntervals;
        std::vector<std::string> scalingRegimes;
        
        PerformancePrediction() {}
    };
    
    PerformancePrediction predictPerformanceAtScale(
        const std::vector<ScaleLevel>& scaleLevels,
        const std::vector<size_t>& targetScales,
        const std::string& predictionModel = "interpolation");
};

// 4. Advanced Ablation Study
class AdvancedAblationStudy {
public:
    struct AblationTarget {
        std::string targetName;
        std::string targetType; // "region", "connection_type", "plasticity_rule", "neuromodulator"
        float ablationStrength; // 0.0 to 1.0
        std::function<void(Brain*)> applyAblation;
        bool reversible;
        float recoveryRate;
        
        AblationTarget(const std::string& name, const std::string& type) 
            : targetName(name), targetType(type), ablationStrength(1.0f), 
              reversible(false), recoveryRate(0.0f) {}
    };
    
    struct AblationResult {
        std::string ablationName;
        std::vector<AblationTarget> targets;
        float baselinePerformance;
        float ablatedPerformance;
        float performanceDrop;
        float recoveryPotential;
        std::vector<std::string> criticalComponents;
        std::vector<std::string> redundantComponents;
        
        AblationResult() : baselinePerformance(0), ablatedPerformance(0), 
                         performanceDrop(0), recoveryPotential(0) {}
    };
    
    AdvancedAblationStudy() = default;
    ~AdvancedAblationStudy() = default;
    
    // Run systematic ablative study
    std::vector<AblationResult> runSystematicAblation(
        std::shared_ptr<Brain> brainTemplate,
        const std::vector<AblationTarget>& targets,
        const std::string& experimentalDesign = "full_factorial", // full_factorial, fractional, random
        size_t numReplicates = 5);
    
    // Run targeted ablative study for hypothesis testing
    std::vector<AblationResult> runTargetedAblation(
        std::shared_ptr<Brain> brain,
        const std::vector<AblationTarget>& hypotheses,
        const std::string& statisticalTest = "t_test"); // t_test, mann_whitney, kruskal_wallis
    
    // Identify critical components
    std::vector<std::string> identifyCriticalComponents(
        const std::vector<AblationResult>& results,
        float performanceThreshold = 0.8f);
    
    // Identify redundant components
    std::vector<std::string> identifyRedundantComponents(
        const std::vector<AblationResult>& results,
        float performanceThreshold = 0.95f);
};

// 5. Multi-Seed Statistical Experiment
class MultiSeedStatisticalExperiment {
public:
    struct SeedConfiguration {
        uint64_t baseSeed;
        std::vector<uint64_t> specificSeeds;
        size_t numSeeds;
        bool enableSeedBalancedDesign;
        std::string randomizationMethod; // "random", "systematic", "block"
        
        SeedConfiguration(size_t seeds) : baseSeed(42), numSeeds(seeds), 
                                         enableSeedBalancedDesign(true), 
                                         randomizationMethod("random") {}
    };
    
    struct SeedResult {
        uint64_t seed;
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Environment> environment;
        float finalPerformance;
        float convergenceRate;
        std::chrono::duration<double> wallClockTime;
        bool success;
        std::string errorMessage;
        std::map<std::string, double> detailedMetrics;
        
        SeedResult() : finalPerformance(0), convergenceRate(0), success(false) {}
    };
    
    MultiSeedStatisticalExperiment() = default;
    ~MultiSeedStatisticalExperiment() = default;
    
    // Run multi-seed experiment with statistical analysis
    std::vector<SeedResult> runMultiSeedExperiment(
        const SeedConfiguration& config,
        const std::function<std::shared_ptr<Brain>(uint64_t)>& brainFactory,
        const std::function<bool(Brain*, uint64_t)>& simulationFunction,
        const std::function<void(Brain*, uint64_t, std::map<std::string, double>&)>& metricsCollector);
    
    // Compute advanced statistics across seeds
    struct AdvancedSeedStatistics {
        double meanPerformance;
        double stdDevPerformance;
        double coefficientOfVariation;
        double skew;
        double kurtosis;
        double medianAbsoluteDeviation;
        double interquartileRange;
        size_t successCount;
        size_t failureCount;
        std::vector<double> normalizedPerformance;
        std::vector<double> performanceZScores;
        
        AdvancedSeedStatistics() : meanPerformance(0), stdDevPerformance(0), 
                                  coefficientOfVariation(0), skew(0), kurtosis(0),
                                  medianAbsoluteDeviation(0), interquartileRange(0),
                                  successCount(0), failureCount(0) {}
    };
    
    AdvancedSeedStatistics computeAdvancedStatistics(const std::vector<SeedResult>& results);
    
    // Detect seed effects and outliers
    struct SeedEffectAnalysis {
        bool hasSeedEffect;
        double seedEffectSize;
        double pValue;
        std::vector<size_t> outlierSeeds;
        std::string effectInterpretation;
        
        SeedEffectAnalysis() : hasSeedEffect(false), seedEffectSize(0), pValue(0) {}
    };
    
    SeedEffectAnalysis analyzeSeedEffects(const std::vector<SeedResult>& results);
    
    // Generate power analysis
    struct PowerAnalysis {
        size_t currentSampleSize;
        double observedEffectSize;
        double achievedPower;
        size_t requiredSampleSize;
        double alphaLevel;
        bool isUnderpowered;
        bool isOverpowered;
        
        PowerAnalysis() : currentSampleSize(0), observedEffectSize(0), achievedPower(0), 
                         requiredSampleSize(0), alphaLevel(0.05), 
                         isUnderpowered(false), isOverpowered(false) {}
    };
    
    PowerAnalysis performPowerAnalysis(const std::vector<SeedResult>& results, double alpha = 0.05);
};

// 6. Adaptive Experiment Runner
class AdaptiveExperimentRunner {
public:
    struct AdaptationPolicy {
        std::string policyName;
        float learningRate;
        float explorationRate;
        float exploitationRate;
        std::function<float(const std::vector<SeedResult>&)> rewardFunction;
        std::function<bool(const std::vector<SeedResult>&)> stoppingCondition;
        bool enableOnlineLearning;
        
        AdaptationPolicy(const std::string& name) : policyName(name), learningRate(0.1f), 
                                                   explorationRate(0.3f), exploitationRate(0.6f),
                                                   enableOnlineLearning(true) {}
    };
    
    struct AdaptiveExperimentResult {
        uint64_t iteration;
        float reward;
        std::vector<float> stateParameters;
        std::shared_ptr<Brain> learnedBrain;
        std::chrono::system_clock::time_point timestamp;
        
        AdaptiveExperimentResult() : iteration(0), reward(0) {}
    };
    
    AdaptiveExperimentRunner() = default;
    ~AdaptiveExperimentRunner() = default;
    
    // Run adaptive experiment with policy optimization
    std::vector<AdaptiveExperimentResult> runAdaptiveExperiment(
        const AdaptationPolicy& policy,
        const std::function<std::shared_ptr<Brain>(uint64_t)>& brainFactory,
        const std::function<bool(Brain*, uint64_t)>& simulationFunction,
        uint64_t maxIterations = 1000,
        bool enablePolicyOptimization = true);
    
    // Run multi-armed bandit experiment
    std::vector<AdaptiveExperimentResult> runMultiArmedBanditExperiment(
        const std::vector<AdaptationPolicy>& policies,
        uint64_t maxRounds = 1000,
        const std::string& selectionAlgorithm = "epsilon_greedy"); // epsilon_greedy, UCB, Thompson
    
    // Run reinforcement learning experiment
    std::vector<AdaptiveExperimentResult> runReinforcementLearningExperiment(
        std::shared_ptr<Brain> brain,
        const std::function<float(Brain*, uint64_t)>& rewardFunction,
        uint64_t maxSteps = 5000,
        float discountFactor = 0.99f,
        float explorationRate = 0.1f);
};

// 7. Cross-Experiment Comparison Tool
class CrossExperimentComparison {
public:
    struct ExperimentMetadata {
        std::string experimentName;
        std::string experimentalDesign;
        std::string hypothesis;
        float baselinePerformance;
        std::chrono::duration<double> duration;
        std::vector<std::string> keyVariables;
        std::map<std::string, double> parameterSettings;
        
        ExperimentMetadata(const std::string& name) : experimentName(name), baselinePerformance(0) {}
    };
    
    struct ComparisonMetric {
        std::string metricName;
        std::vector<double> experimentValues;
        std::vector<double> normalizedValues;
        double overallMean;
        double overallStdDev;
        std::vector<size_t> ranking;
        std::vector<std::string> performanceLabels;
        
        ComparisonMetric() : overallMean(0), overallStdDev(0) {}
    };
    
    CrossExperimentComparison() = default;
    ~CrossExperimentComparison() = default;
    
    // Compare multiple experiment results
    std::map<std::string, ComparisonMetric> compareExperiments(
        const std::vector<std::vector<ExperimentResult>>& experimentResults,
        const std::vector<ExperimentMetadata>& metadata);
    
    // Perform meta-analysis
    struct MetaAnalysisResult {
        std::string analysisType; // fixed_effect, random_effects, mixed_effects
        double overallEffectSize;
        double confidenceInterval;
        double heterogeneityMeasure; // I^2
        std::vector<std::string> significantFindings;
        std::vector<std::string> limitations;
        
        MetaAnalysisResult() : overallEffectSize(0), confidenceInterval(0), heterogeneityMeasure(0) {}
    };
    
    MetaAnalysisResult performMetaAnalysis(
        const std::vector<std::vector<ExperimentResult>>& experimentResults,
        const std::string& analysisType = "random_effects");
    
    // Generate comparison report
    std::string generateComparisonReport(
        const std::map<std::string, ComparisonMetric>& metrics,
        const MetaAnalysisResult& metaAnalysis,
        const std::vector<ExperimentMetadata>& metadata,
        const std::string& reportType = "comprehensive"); // comprehensive, executive, technical
};

// 8. Advanced Experiment Orchestrator
class AdvancedExperimentOrchestrator {
public:
    struct ExperimentWorkflow {
        std::string workflowName;
        std::vector<std::string> experimentSequence;
        std::map<std::string, std::string> dependencies;
        std::map<std::string, double> resourceRequirements;
        std::chrono::duration<double> estimatedDuration;
        bool enableParallelExecution;
        size_t maxConcurrentExperiments;
        
        ExperimentWorkflow(const std::string& name) : workflowName(name), 
                                                   enableParallelExecution(false), 
                                                   maxConcurrentExperiments(1) {}
    };
    
    struct OrchestrationResult {
        std::string workflowName;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double totalDuration;
        size_t totalExperimentsRun;
        size_t successfulExperiments;
        std::vector<ExperimentResult> allResults;
        std::map<std::string, double> resourceUtilization;
        std::string status; // "completed", "in_progress", "failed", "cancelled"
        std::string errorMessage;
        
        OrchestrationResult() : totalDuration(0), totalExperimentsRun(0), 
                              successfulExperiments(0), status("completed") {}
    };
    
    AdvancedExperimentOrchestrator() = default;
    ~AdvancedExperimentOrchestrator() = default;
    
    // Execute experiment workflow
    OrchestrationResult executeWorkflow(
        const ExperimentWorkflow& workflow,
        const std::map<std::string, std::function<std::vector<ExperimentResult>(const std::vector<ExperimentResult>&)>>& experimentRunners,
        bool enableMonitoring = true,
        const std::string& outputDirectory = "experiment_results");
    
    // Schedule experiments based on dependencies and resources
    std::vector<ExperimentWorkflow> scheduleExperiments(
        const std::vector<ExperimentWorkflow>& workflows,
        const std::map<std::string, double>& resourceConstraints,
        const std::string& schedulingAlgorithm = "gantt"); // gantt, critical_path, resource_leveled
    
    // Monitor and optimize experiment execution
    struct MonitoringData {
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> resourceUsage;
        std::map<std::string, double> performanceMetrics;
        size_t completedExperiments;
        size_t runningExperiments;
        double averageProgress;
        
        MonitoringData() : completedExperiments(0), runningExperiments(0), averageProgress(0) {}
    };
    
    std::vector<MonitoringData> monitorExecution(const OrchestrationResult& orchestration);
};

// 9. Interactive Experiment Explorer
class InteractiveExperimentExplorer {
public:
    struct ExplorationResult {
        std::string experimentName;
        std::vector<std::string> suggestedModifications;
        std::vector<std::string> identifiedBiases;
        std::vector<std::string> alternativeHypotheses;
        std::vector<std::string> recommendedExtensions;
        std::map<std::string, double> sensitivityAnalysis;
        std::vector<std::string> potentialConfounds;
        
        ExplorationResult() {}
    };
    
    InteractiveExperimentExplorer() = default;
    ~InteractiveExperimentExplorer() = default;
    
    // Analyze experiment results for improvements
    ExplorationResult analyzeForImprovements(
        const std::vector<ExperimentResult>& results,
        const std::vector<std::string>& experimentNames,
        const std::string& analysisFocus = "comprehensive"); // comprehensive, statistical, methodological
    
    // Generate experiment recommendations
    std::vector<std::string> generateRecommendations(
        const std::vector<ExperimentResult>& results,
        const std::string& userExpertiseLevel = "intermediate"); // beginner, intermediate, advanced
    
    // Create experiment optimization suggestions
    struct OptimizationSuggestion {
        std::string suggestionType; // "parameter_tuning", "experimental_design", "data_collection"
        std::string description;
        std::string expectedImpact;
        std::string implementationComplexity;
        double confidenceLevel;
        
        OptimizationSuggestion() : confidenceLevel(0) {}
    };
    
    std::vector<OptimizationSuggestion> generateOptimizationSuggestions(
        const std::vector<ExperimentResult>& results,
        const std::string& targetMetric = "performance");
    
    // Create experiment replication guide
    std::string createReplicationGuide(
        const std::vector<ExperimentResult>& results,
        const std::string& targetAudience = "researchers"); // researchers, students, general_public
};

// 10. Expert System for Experiment Design
class ExpertExperimentDesigner {
public:
    struct DesignRecommendation {
        std::string recommendationType; // "optimal_design", "sample_size", "power_analysis"
        std::string description;
        std::string rationale;
        std::vector<std::string> implementationSteps;
        std::vector<std::string> expectedOutcomes;
        double confidenceScore;
        
        DesignRecommendation() : confidenceScore(0) {}
    };
    
    ExpertExperimentDesigner() = default;
    ~ExpertExperimentDesigner() = default;
    
    // Design optimal experiment based on research goals
    std::vector<DesignRecommendation> designOptimalExperiment(
        const std::string& researchQuestion,
        const std::string& theoreticalFramework,
        const std::string& practicalConstraints,
        const std::string& targetPopulation,
        const std::string& outcomeMeasures);
    
    // Perform experimental design sensitivity analysis
    std::vector<DesignRecommendation> performDesignSensitivityAnalysis(
        const std::vector<ExperimentResult>& results,
        const std::vector<std::string>& designFactors,
        const std::string& sensitivityMetric = "power");
    
    // Create experimental protocol
    std::string createExperimentalProtocol(
        const std::vector<DesignRecommendation>& recommendations,
        const std::string& protocolType = "standard"); // standard, rigorous, exploratory
    
    // Validate experimental design
    struct DesignValidation {
        bool isValid;
        std::vector<std::string> validationChecks;
        std::vector<std::string> requiredModifications;
        std::vector<std::string> statisticalIssues;
        double overallQualityScore;
        
        DesignValidation() : isValid(true), overallQualityScore(0) {}
    };
    
    DesignValidation validateExperimentalDesign(
        const std::vector<ExperimentResult>& results,
        const std::vector<std::string>& qualityCriteria);
};

// Convenience functions for advanced experiment setup
namespace AdvancedExperimentFactory {
    // Create comprehensive experiment suite
    std::vector<std::shared_ptr<Experiment>> createAdvancedExperimentSuite(
        const std::string& suiteName,
        const std::vector<std::string>& experimentTypes = {"continual_learning", "damage_recovery", 
                                                            "scaling", "ablation", "multi_seed"});
    
    // Create experiment configuration template
    AdvancedExperimentConfig createAdvancedExperimentConfig(
        const std::string& mode = "balanced",
        size_t customNeurons = 0,
        bool enableAdvancedAnalytics = true,
        const std::string& outputFormat = "json");
    
    // Create brain template for advanced experiments
    std::shared_ptr<Brain> createAdvancedBrainTemplate(
        const std::shared_ptr<Config>& config,
        const std::string& brainType = "standard",
        bool enableAdvancedPlasticity = true,
        bool enableNeuromodulation = true);
    
    // Create environment template for advanced experiments
    std::shared_ptr<Environment> createAdvancedEnvironmentTemplate(
        const std::shared_ptr<Config>& config,
        const std::string& environmentType = "standard",
        bool enableSensoryInputs = true,
        bool enableMotorOutputs = true);
}

} // namespace nlm

// Convenience function for creating advanced experiment runner
inline nlm::ExperimentRunner createAdvancedExperimentRunner() {
    nlm::ExperimentRunner runner;
    
    // Set up advanced configuration
    nlm::AdvancedExperimentConfig config;
    config.experimentMode = "advanced";
    config.maxSteps = 50000;
    config.enableParallelExecution = true;
    config.threadCount = 4;
    
    runner.setGlobalConfig(config);
    
    // Add advanced experiment types
    return runner;
}

// Convenience function for running advanced experiment suite
inline std::vector<nlm::ExperimentResult> runAdvancedExperimentSuite(
    nlm::ExperimentRunner& runner,
    const std::vector<std::shared_ptr<nlm::Experiment>>& experiments,
    std::shared_ptr<nlm::Brain> brain,
    std::shared_ptr<nlm::Environment> environment,
    const nlm::AdvancedExperimentConfig& config = nlm::AdvancedExperimentConfig()) {
    
    return runner.runBatch(experiments, brain, environment, config);
}

// Convenience function for analyzing advanced experiment results
inline nlm::ExperimentAnalysis analyzeAdvancedExperimentResults(
    const std::vector<nlm::ExperimentResult>& results) {
    return nlm::AdvancedVisualizationReporter::analyzeExperimentResults(results);
}

// Convenience function for exporting advanced experiment results
inline bool exportAdvancedExperimentResults(
    const std::vector<nlm::ExperimentResult>& results,
    const std::string& outputPath,
    nlm::ReportFormat format = nlm::ReportFormat::JSON) {
    
    std::string report;
    switch (format) {
        case nlm::ReportFormat::JSON:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", format);
            break;
        case nlm::ReportFormat::CSV:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", format);
            break;
        case nlm::ReportFormat::HTML:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", format);
            break;
        case nlm::ReportFormat::LaTeX:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", format);
            break;
        case nlm::ReportFormat::Markdown:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", format);
            break;
        default:
            report = nlm::AdvancedVisualizationReporter::generateExperimentReport(results, "Advanced Experiment Results", nlm::ReportFormat::Text);
            break;
    }
    
    // Write report to file
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << report;
    file.close();
    
    return true;
}

// Convenience function for creating advanced visualization
inline std::string createAdvancedVisualization(
    const nlm::VisualizationData& data,
    nlm::ChartType chartType = nlm::ChartType::LineChart,
    nlm::ReportFormat format = nlm::ReportFormat::PNG) {
    return nlm::AdvancedVisualizationReporter::generateProgressChart(data, chartType, format);
}

// Convenience function for generating advanced statistical report
inline std::string generateAdvancedStatisticalReport(
    const std::vector<nlm::ExperimentResult>& results,
    const std::string& title = "Advanced Statistical Analysis Report") {
    return nlm::AdvancedVisualizationReporter::generateStatisticalReport(results, title);
}

// Convenience function for creating advanced comparison report
inline std::string createAdvancedComparisonReport(
    const std::vector<nlm::ExperimentAnalysis>& analyses,
    const std::vector<std::string>& labels,
    const std::string& title = "Advanced Comparison Report") {
    return nlm::AdvancedVisualizationReporter::generateComparativeReport(analyses, labels, title);
}

} // namespace nlm

#endif // ADVANCED_EXPERIMENT_TYPES_HPP