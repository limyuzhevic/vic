#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <random>
#include <functional>
#include <limits>
#include <unordered_map>
#include <queue>

namespace nlm {

// Hyperparameter optimization types
enum class HyperparameterType {
    Continuous,
    Integer,
    Categorical,
    Boolean,
    String,
    Vector,
    Range,
    Probability,
    Choice
};

enum class OptimizationAlgorithm {
    GridSearch,
    RandomSearch,
    BayesianOptimization,
    GeneticAlgorithm,
    ParticleSwarm,
    SimulatedAnnealing,
    CMAEvolutionStrategy,
    DifferentialEvolution,
    SHERPA,
    ASHA,
    POBO,
    MetaLearning,
    TransferLearning,
    MultiObjective
};

enum class ConvergenceCriterion {
    FunctionValue,
    ParameterChange,
    GradientNorm,
    PopulationDiversity,
    LearningCurve,
    CrossValidation,
    Custom
};

// Hyperparameter definition
struct Hyperparameter {
    std::string name;
    HyperparameterType type;
    std::string description;
    ConfigValue defaultValue;
    std::map<std::string, ConfigValue> bounds; // For continuous parameters
    std::vector<ConfigValue> choices; // For categorical parameters
    float probability; // For probability parameters
    bool adaptive; // Whether parameter should adapt during optimization
    float learningRate; // For adaptive parameters
    
    Hyperparameter(const std::string& n = "param", HyperparameterType t = HyperparameterType::Continuous) 
        : name(n), type(t), probability(1.0f), adaptive(false), learningRate(0.01f) {}
};

// Optimization configuration
struct OptimizationConfig {
    std::string name;
    std::string description;
    OptimizationAlgorithm algorithm;
    ConvergenceCriterion convergenceCriterion;
    size_t maxIterations;
    float convergenceThreshold;
    size_t populationSize;
    float mutationRate;
    float crossoverRate;
    float learningRate;
    bool parallel;
    size_t numWorkers;
    std::map<std::string, float> algorithmParameters;
    std::vector<Hyperparameter> hyperparameters;
    
    OptimizationConfig(const std::string& n = "Optimization", OptimizationAlgorithm algo = OptimizationAlgorithm::RandomSearch)
        : name(n), algorithm(algo), maxIterations(100), convergenceThreshold(0.01f), populationSize(20),
          mutationRate(0.1f), crossoverRate(0.8f), learningRate(0.01f), parallel(false), numWorkers(1) {}
};

// Optimization result
struct OptimizationResult {
    std::map<std::string, ConfigValue> bestParameters;
    float bestObjectiveValue;
    std::vector<std::map<std::string, ConfigValue>> history;
    std::vector<float> objectiveValues;
    std::vector<float> convergenceMetrics;
    size_t bestIteration;
    bool converged;
    float convergenceSpeed;
    
    OptimizationResult() : bestObjectiveValue(std::numeric_limits<float>::max()), bestIteration(0), converged(false), convergenceSpeed(0.0f) {}
};

// Hyperparameter optimization system
class HyperParameterOpt {
public:
    HyperParameterOpt();
    ~HyperParameterOpt();
    
    // Optimization setup
    void addHyperparameter(const Hyperparameter& hyperparameter);
    void removeHyperparameter(const std::string& name);
    Hyperparameter* getHyperparameter(const std::string& name);
    std::vector<Hyperparameter> getAllHyperparameters() const;
    void configureOptimization(const OptimizationConfig& config);
    OptimizationConfig getOptimizationConfig() const { return currentConfig; }
    
    // Parameter generation
    std::vector<ConfigValue> generateRandomParameter(const Hyperparameter& hyperparameter);
    std::vector<ConfigValue> generateParameterVector(const Hyperparameter& hyperparameter);
    std::vector<ConfigValue> generateParameterRange(const Hyperparameter& hyperparameter);
    Config createConfigFromParameters(const std::vector<ConfigValue>& parameters);
    std::vector<Config> generateParameterGrid(const std::vector<Hyperparameter>& hyperparameters);
    std::vector<Config> generateParameterSpace(const std::vector<Hyperparameter>& hyperparameters, size_t numSamples);
    
    // Optimization algorithms
    OptimizationResult optimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult gridSearchOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult randomSearchOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult bayesianOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult geneticAlgorithmOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult particleSwarmOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult simulatedAnnealingOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult cmaEvolutionStrategyOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult differentialEvolutionOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult sherpaOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult ashaOptimize(const std::function<float(const Config&)>& objectiveFunction);
    OptimizationResult poboOptimize(const std::function<float(const Config&)>& objectiveFunction);
    
    // Multi-objective optimization
    struct MultiObjectiveResult {
        std::map<std::string, ConfigValue> bestParameters;
        std::vector<std::vector<float>> objectiveValues; // Pareto front
        std::vector<std::map<std::string, ConfigValue>> history;
        float convergenceSpeed;
        
        MultiObjectiveResult() : convergenceSpeed(0.0f) {}
    };
    
    MultiObjectiveResult multiObjectiveOptimize(const std::vector<std::function<float(const Config&)>>& objectiveFunctions);
    MultiObjectiveResult paretoFrontOptimization(const std::vector<std::function<float(const Config&)>>& objectiveFunctions);
    MultiObjectiveResult shpaOptimization(const std::vector<std::function<float(const Config&)>>& objectiveFunctions);
    MultiObjectiveResult nsgaIiiOptimization(const std::vector<std::function<float(const Config&)>>& objectiveFunctions);
    
    // Learning and adaptation
    void updateHyperparameters(const std::vector<Config>& population, const std::vector<float>& scores);
    void adaptLearningRates(const std::vector<Config>& population, const std::vector<float>& scores);
    void adjustMutationRates(const std::vector<Config>& population, const std::vector<float>& scores);
    void evolvePopulation(std::vector<Config>& population, const std::vector<float>& scores);
    void mutateConfig(Config& config, float mutationRate);
    void crossoverConfigs(Config& parent1, Config& parent2, Config& child1, Config& child2);
    
    // Advanced features
    bool enableTransferLearning(const std::string& sourceTask);
    bool enableMetaLearning(const std::vector<Config>& metaData);
    bool enableAutoML(const std::string& problemType, const std::string& dataset);
    bool enableBayesianOptimization(const std::map<std::string, std::vector<ConfigValue>>& searchSpace);
    bool enableSequentialModelBasedOptimization(const std::vector<Config>& initialConfigs);
    
    // Configuration evaluation
    float evaluateConfiguration(const Config& config, const std::function<float(const Config&)>& objectiveFunction);
    float evaluateConfigurationWithConstraints(const Config& config, const std::function<float(const Config&)>& objectiveFunction);
    bool validateConfiguration(const Config& config) const;
    float calculateConfigurationScore(const Config& config) const;
    
    // Parallel optimization
    void enableParallelOptimization(bool enable, size_t numWorkers = 4);
    size_t getNumWorkers() const { return numWorkers; }
    void setWorkerCount(size_t count) { numWorkers = count; }
    std::vector<OptimizationResult> parallelOptimize(const std::function<float(const Config&)>& objectiveFunction);
    
    // Hyperparameter analysis
    std::map<std::string, float> analyzeHyperparameterImportance(const std::vector<Config>& population, const std::vector<float>& scores);
    std::map<std::string, float> analyzeHyperparameterInteractions(const std::vector<Config>& population);
    std::vector<float> getHyperparameterSensitivity(const Hyperparameter& hyperparameter);
    std::map<std::string, std::vector<float>> getHyperparameterDistributions(const std::vector<Config>& population);
    
    // Optimization monitoring
    void startOptimizationMonitoring();
    void stopOptimizationMonitoring();
    bool isOptimizationMonitoringEnabled() const { return optimizationMonitoringEnabled; }
    void setOptimizationInterval(float interval) { optimizationInterval = interval; }
    float getOptimizationInterval() const { return optimizationInterval; }
    
    // Results and reporting
    OptimizationResult getOptimizationResult() const { return currentResult; }
    std::string getOptimizationReport() const;
    std::string exportOptimizationResults(const std::string& format = "json") const;
    bool saveOptimizationResults(const std::string& filepath);
    bool loadOptimizationResults(const std::string& filepath);
    
    // Advanced optimization
    bool runMultiStartOptimization(const std::function<float(const Config&)>& objectiveFunction);
    bool runRandomRestartOptimization(const std::function<float(const Config&)>& objectiveFunction);
    bool runIterativeOptimization(const std::function<float(const Config&)>& objectiveFunction);
    bool runFeatureSelectionOptimization(const std::function<float(const Config&)>& objectiveFunction);
    bool runRegularizationOptimization(const std::function<float(const Config&)>& objectiveFunction);
    
    // Hyperparameter tuning
    bool tuneHyperparameters(const std::vector<std::vector<Config>>& datasets,
                           const std::function<float(const Config&)>& objectiveFunction);
    bool tuneHyperparametersWithCrossValidation(const std::vector<std::vector<Config>>& datasets,
                                              const std::function<float(const Config&)>& objectiveFunction);
    bool tuneHyperparametersWithBayesianOptimization(const std::vector<std::vector<Config>>& datasets,
                                                   const std::function<float(const Config&)>& objectiveFunction);
    bool tuneHyperparametersWithGeneticAlgorithm(const std::vector<std::vector<Config>>& datasets,
                                               const std::function<float(const Config&)>& objectiveFunction);
    
protected:
    void initializeAlgorithmParameters();
    bool checkConvergence(const std::vector<float>& scores, float threshold);
    float calculateConvergenceMetric(const std::vector<float>& scores);
    void updateConvergenceMetrics(const std::vector<float>& scores);
    bool validateSearchSpace(const std::vector<Hyperparameter>& hyperparameters);
    bool validateObjectiveFunction(const std::function<float(const Config&)>& objectiveFunction);
    Config generateRandomConfig(const std::vector<Hyperparameter>& hyperparameters);
    float calculateObjectiveValue(const Config& config, const std::function<float(const Config&)>& objectiveFunction);
    
    bool optimizationMonitoringEnabled;
    float optimizationInterval;
    
    size_t numWorkers;
    OptimizationConfig currentConfig;
    OptimizationResult currentResult;
    
    std::vector<Hyperparameter> hyperparameters;
    std::vector<Config> population;
    std::vector<float> populationScores;
    
    std::map<std::string, std::vector<float>> hyperparameterImportance;
    std::map<std::string, std::vector<float>> hyperparameterDistributions;
};

// Advanced hyperparameter optimizer
class AdvancedHyperParameterOpt {
public:
    AdvancedHyperParameterOpt();
    ~AdvancedHyperParameterOpt();
    
    // Machine learning integration
    bool enableMachineLearningIntegration(const std::string& mlFramework);
    bool loadPre-trainedModels(const std::vector<std::string>& modelPaths);
    bool trainHyperparameterOptimizer(const std::vector<Config>& trainingData, const std::vector<float>& labels);
    bool predictHyperparameters(const std::vector<Config>& testData, std::vector<Config>& predictions);
    
    // AutoML integration
    bool runAutoMLPipeline(const std::string& problemType, const std::string& dataset, 
                         const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool runAutoMLWithNestedCV(const std::string& problemType, const std::string& dataset,
                            const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool runAutoMLWithBayesianOptimization(const std::string& problemType, const std::string& dataset,
                                         const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool runAutoMLWithGeneticAlgorithm(const std::string& problemType, const std::string& dataset,
                                     const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    
    // Transfer learning
    bool enableTransferLearning(const std::string& sourceTask, const std::string& targetTask);
    bool adaptTransferedModel(const Config& sourceConfig, Config& targetConfig);
    bool fineTuneTransferredModel(const std::vector<Config>& fineTuningData, Config& modelConfig);
    bool evaluateTransferLearning(const std::vector<Config>& evaluationData, float& transferAccuracy);
    
    // Meta-learning
    bool enableMetaLearning(const std::vector<Config>& metaData);
    bool learnMetaOptimizer(const std::vector<Config>& metaData, const std::vector<float>& metaLabels);
    bool applyMetaOptimization(Config& config, const std::map<std::string, float>& metaFeatures);
    bool evaluateMetaLearning(const std::vector<Config>& testData, float& metaLearningScore);
    
    // Hyperparameter space exploration
    bool exploreHyperparameterSpace(const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                   const std::function<float(const Config&)>& objectiveFunction);
    bool exploreHyperparameterSpaceWithUncertainty(const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                  const std::function<float(const Config&)>& objectiveFunction);
    bool exploreHyperparameterSpaceWithConstraints(const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                  const std::function<float(const Config&)>& objectiveFunction);
    bool exploreHyperparameterSpaceWithBayesianBands(const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                   const std::function<float(const Config&)>& objectiveFunction);
    
    // Advanced optimization algorithms
    bool runSHERPA(const std::function<float(const Config&)>& objectiveFunction);
    bool runASHA(const std::function<float(const Config&)>& objectiveFunction);
    bool runPOBO(const std::function<float(const Config&)>& objectiveFunction);
    bool runSequentialModelBasedOptimization(const std::function<float(const Config&)>& objectiveFunction);
    bool runMultiStartRandomSearch(const std::function<float(const Config&)>& objectiveFunction);
    bool runIterativeOptimization(const std::function<float(const Config&)>& objectiveFunction);
    
    // Configuration generation
    bool generateConfigFromHyperparameters(const std::map<std::string, ConfigValue>& hyperparameters, Config& config);
    bool generateConfigFromSearchSpace(const std::map<std::string, std::vector<ConfigValue>>& searchSpace, Config& config);
    bool generateConfigFromPrior(const std::map<std::string, float>& priorMeans, const std::map<std::string, float>& priorVariances, Config& config);
    bool generateConfigFromGaussianProcess(const std::map<std::string, float>& meanFunction, const std::map<std::string, float>& kernelFunction, Config& config);
    
    // Evaluation and testing
    bool evaluateHyperparameterQuality(const Config& config, const std::function<float(const Config&)>& objectiveFunction);
    bool performCrossValidation(const Config& config, const std::vector<std::vector<Config>>& datasets);
    bool performLeaveOneOutCrossValidation(const Config& config, const std::vector<std::vector<Config>>& datasets);
    bool performNestedCrossValidation(const Config& config, const std::vector<std::vector<Config>>& datasets);
    bool evaluateHyperparameterRobustness(const Config& config, const std::vector<std::vector<Config>>& datasets);
    
    // Advanced features
    bool runMultiObjectiveHyperparameterOptimization(const std::vector<std::function<float(const Config&)>>& objectiveFunctions);
    bool runHyperparameterSpacePartitioning(const std::map<std::string, std::vector<ConfigValue>>& searchSpace);
    bool runHyperparameterSensitivityAnalysis(const std::map<std::string, std::vector<ConfigValue>>& searchSpace);
    bool runHyperparameterInteractionAnalysis(const std::map<std::string, std::vector<ConfigValue>>& searchSpace);
    bool runHyperparameterFeatureImportance(const std::map<std::string, std::vector<ConfigValue>>& searchSpace);
    
    // Export and import
    bool saveHyperparameterOptimizationResults(const std::string& filepath);
    bool loadHyperparameterOptimizationResults(const std::string& filepath);
    std::string exportHyperparameterOptimizationResults(const std::string& format = "json") const;
    bool generateHyperparameterDocumentation(const std::string& filepath);
    bool generateHyperparameterTutorial(const std::string& filepath);
    
    // Monitoring and reporting
    std::string getAdvancedOptimizationReport() const;
    std::string exportAdvancedOptimizationResults(const std::string& format = "json") const;
    bool updateHyperparameterCache(const Config& config, float score);
    bool analyzeHyperparameterTrends(const std::vector<Config>& history);
    
    // Machine learning model integration
    bool integrateWithScikitlearn(const std::string& modelType, const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool integrateWithKeras(const std::string& modelType, const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool integrateWithPytorch(const std::string& modelType, const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    bool integrateWithXGBoost(const std::string& modelType, const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    
protected:
    void initializeAdvancedFeatures();
    bool validateAdvancedConfiguration(const Config& config);
    bool evaluateAdvancedPerformance(const Config& config, const std::function<float(const Config&)>& objectiveFunction);
    void updateAdvancedResults(const Config& config, float score);
    bool analyzeAdvancedTrends(const std::vector<Config>& history);
    
    bool machineLearningEnabled;
    std::string mlFramework;
    
    std::vector<Config> hyperparameterHistory;
    std::vector<float> hyperparameterScores;
    std::map<std::string, std::vector<float>> hyperparameterTrends;
};

} // namespace nlm
