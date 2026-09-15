#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_set>
#include <eigen3/Eigen/Dense>
#include <chrono>
#include <random>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * Parameter sweep results
 */
struct ParameterSweepResult {
    std::string parameterName;
    std::vector<double> parameterValues;
    std::vector<double> performanceScores;
    double optimalValue;
    double optimalScore;
    std::vector<double> confidenceIntervals;
    std::map<std::string, std::vector<double>> correlations;
};

/**
 * Model selection results
 */
struct ModelSelectionResult {
    std::string modelName;
    std::string modelType;
    double performanceScore;
    double complexity;
    double trainingTime;
    double inferenceTime;
    double aic;  // Akaike Information Criterion
    double bic;  // Bayesian Information Criterion
    double rSquared;
    double adjustedRSquared;
    std::map<std::string, double> hyperparameters;
    bool isOverfitted;
    bool isUnderfitted;
};

/**
 * Cross-validation results
 */
struct CrossValidationResult {
    std::string cvMethod;  // k-fold, leave-one-out, bootstrap
    size_t k;  // For k-fold
    size_t numFolds;
    double meanAccuracy;
    double stdAccuracy;
    double meanPrecision;
    double stdPrecision;
    double meanRecall;
    double stdRecall;
    double meanF1Score;
    double stdF1Score;
    std::vector<double> foldAccuracies;
    std::vector<double> foldPrecisions;
    std::vector<double> foldRecalls;
    std::vector<double> foldF1Scores;
    double cvTime;
};

/**
 * Bootstrapping results
 */
struct BootstrapResult {
    std::string statisticType;  // mean, median, std, etc.
    size_t sampleSize;
    size_t numBootstrapSamples;
    double originalStatistic;
    double bootstrapStatistic;
    double standardError;
    double confidenceLevel;
    std::vector<double> bootstrapDistribution;
    std::vector<double> bias;
    std::vector<double> percentileIntervals;
    std::vector<double> biasCorrectedIntervals;
    double bootstrapTime;
};

/**
 * Experimental design parameters
 */
struct ExperimentalDesign {
    std::string designType;  // factorial, fractional_factorial, response_surface
    std::vector<std::string> factors;
    std::map<std::string, std::vector<double>> factorLevels;
    size_t numRuns;
    size_t numBlocks;
    std::string randomizationScheme;
    std::map<std::string, std::function<double(const std::map<std::string, double>&)>> modelFunction;
    std::map<std::string, std::function<void(std::map<std::string, double>&)>> noiseFunction;
    bool includeBlockingFactors;
    bool includeCovariates;
    std::string responseVariable;
};

/**
 * Statistical model comparison results
 */
struct ModelComparisonResult {
    std::string comparisonType;  // likelihood_ratio, information_criterion, cross_validation
    std::vector<ModelSelectionResult> models;
    std::map<std::pair<size_t, size_t>, double> pairwiseDistances;
    size_t bestModelIndex;
    std::string bestModelName;
    double bestPerformance;
    std::vector<double> ranks;
    std::string recommendation;
    double comparisonTime;
};

/**
 * Advanced research tools for neural network optimization and analysis
 */
class AdvancedResearchTools {
public:
    AdvancedResearchTools();
    ~AdvancedResearchTools();
    
    // Initialize with brain reference
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // Parameter sweep and optimization
    std::vector<ParameterSweepResult> performParameterSweep(
        const std::map<std::string, std::vector<double>>& parameterRanges,
        const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
        const std::string& optimizationType = "grid_search",
        size_t numThreads = 0);
    
    std::map<std::string, double> performOptimization(
        const std::map<std::string, double>& initialParameters,
        const std::map<std::string, std::pair<double, double>>& parameterBounds,
        const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
        const std::string& optimizerType = "gradient_descent",
        size_t maxIterations = 1000,
        double tolerance = 1e-6,
        size_t numThreads = 0);
    
    // Model selection
    std::vector<ModelSelectionResult> performModelSelection(
        const std::vector<std::function<double(const std::map<std::string, double>&)>>& models,
        const std::vector<std::map<std::string, double>>& trainingData,
        const std::vector<std::map<std::string, double>>& validationData,
        const std::map<std::string, std::vector<double>>& hyperparameterRanges,
        const std::string& selectionCriterion = "aic",
        size_t numThreads = 0);
    
    // Cross-validation
    CrossValidationResult performCrossValidation(
        const std::vector<std::map<std::string, double>>& data,
        const std::function<double(const std::map<std::string, double>&, 
                                   const std::function<double(const std::map<std::string, double>&)>&)>& predictFunction,
        const std::string& cvMethod = "k_fold",
        size_t k = 5,
        size_t numThreads = 0,
        bool stratification = false);
    
    // Bootstrapping
    BootstrapResult performBootstrapping(
        const std::vector<double>& data,
        const std::function<double(const std::vector<double>&)>& statisticFunction,
        size_t numBootstrapSamples = 1000,
        double confidenceLevel = 0.95,
        size_t numThreads = 0);
    
    BootstrapResult performParameterBootstrapping(
        std::shared_ptr<Brain> brain,
        const std::string& parameterName,
        size_t numBootstrapSamples = 1000,
        double confidenceLevel = 0.95,
        size_t numThreads = 0);
    
    // Experimental design
    ExperimentalDesign createFactorialDesign(
        const std::vector<std::string>& factors,
        const std::map<std::string, std::vector<double>>& factorLevels,
        size_t numBlocks = 0,
        bool includeInteractions = true);
    
    ExperimentalDesign createResponseSurfaceDesign(
        const std::vector<std::string>& factors,
        const std::map<std::string, std::pair<double, double>>& factorRanges,
        size_t numRuns = 50,
        const std::string& designType = "central_composite");
    
    std::vector<std::map<std::string, double>> generateExperimentalRuns(
        const ExperimentalDesign& design,
        std::shared_ptr<RandomGenerator> rng = nullptr);
    
    // Statistical model comparison
    ModelComparisonResult compareModels(
        const std::vector<ModelSelectionResult>& models,
        const std::string& comparisonType = "information_criterion",
        const std::string& rankingMethod = "performance_based",
        bool normalizePerformance = true);
    
    // Neural network specific research tools
    std::vector<ParameterSweepResult> sweepNetworkParameters(
        const std::vector<std::string>& parameterNames,
        const std::map<std::string, std::vector<double>>& parameterRanges,
        const std::string& performanceMetric = "firing_rate_stability",
        size_t simulationSteps = 1000,
        size_t numThreads = 0);
    
    std::map<std::string, double> optimizeNeuralArchitecture(
        const std::map<std::string, std::pair<double, double>>& architectureRanges,
        const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
        size_t numTrials = 100,
        size_t simulationSteps = 1000,
        size_t numThreads = 0);
    
    // Stability analysis
    struct StabilityAnalysisResult {
        double stabilityMetric;
        double convergenceRate;
        double oscillationLevel;
        double attractorCount;
        std::vector<std::complex<double>> eigenvalues;
        std::string stabilityClassification;  // stable, unstable, marginal, chaotic
        double analysisTime;
    };
    
    StabilityAnalysisResult analyzeStability(
        std::shared_ptr<Brain> brain,
        size_t simulationSteps = 1000,
        const std::string& stabilityMetric = "firing_rate_variance");
    
    // Learning curve analysis
    struct LearningCurveResult {
        std::vector<size_t> trainingSizes;
        std::vector<double> trainPerformance;
        std::vector<double> valPerformance;
        std::vector<double> testPerformance;
        double overfittingMeasure;
        double underfittingMeasure;
        size_t optimalTrainingSize;
        double learningCurveTime;
    };
    
    LearningCurveResult analyzeLearningCurves(
        const std::vector<std::map<std::string, double>>& data,
        const std::string& targetVariable,
        const std::vector<size_t>& trainingSizes,
        size_t numThreads = 0);
    
    // Configuration and utilities
    void configure(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getConfiguration() const;
    
    // Performance monitoring
    double getResearchTime() const { return researchTime; }
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Brain> brainRef;
    std::shared_ptr<Config> configRef;
    double researchTime;
    std::string lastError;
    
    // Parallel processing utilities
    std::mutex researchMutex;
    std::condition_variable researchCV;
    bool stopRequested;
    
    // Mathematical helper functions
    static double calculateMean(const std::vector<double>& data);
    static double calculateStandardDeviation(const std::vector<double>& data, double mean);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static double calculateKSStatistic(const std::vector<double>& data1, const std::vector<double>& data2);
    static double calculateAnovaPValue(const std::vector<std::vector<double>>& datasets);
    static double calculateAIC(size_t numParameters, double logLikelihood);
    static double calculateBIC(size_t numParameters, size_t numDataPoints, double logLikelihood);
    static Eigen::MatrixXd calculateCovarianceMatrix(const std::vector<std::vector<double>>& data);
    static std::vector<std::complex<double>> calculateEigenvalues(const Eigen::MatrixXd& matrix);
    static std::string classifyStability(double stabilityMetric);
};

} // namespace nlm
