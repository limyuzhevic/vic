#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <functional>
#include <chrono>

namespace nlm {

// Configuration automation types
enum class AutomationStrategy {
    GridSearch,
    RandomSearch,
    BayesianOptimization,
    GeneticAlgorithm,
    ParticleSwarm,
    SimulatedAnnealing,
    GradientDescent,
    Hyperband,
    BOHB,
    Custom
};

enum class OptimizationObjective {
    MaximizeAccuracy,
    MinimizeError,
    MinimizeTrainingTime,
    MinimizeMemoryUsage,
    MaximizeSpeed,
    BalanceMultipleMetrics,
    CustomObjective
};

// Configuration template
struct ConfigurationTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> baseConfig;
    std::map<std::string, std::vector<ConfigValue>> parameterRanges;
    std::map<std::string, std::function<bool(const ConfigValue&)>> parameterConstraints;
    std::map<std::string, float> parameterWeights;
    
    ConfigurationTemplate(const std::string& n = "Template", const std::string& desc = "")
        : name(n), description(desc) {}
};

// Configuration macro
struct ConfigurationMacro {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> configOverrides;
    std::vector<std::string> dependencies;
    bool enabled;
    
    ConfigurationMacro(const std::string& n = "Macro", const std::string& desc = "")
        : name(n), description(desc), enabled(true) {}
};

// Simulation pipeline stage
struct PipelineStage {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> config;
    std::function<bool(const Config&)> validation;
    std::function<void(const Config&, std::vector<NeuralRegion*>&)> execution;
    std::vector<std::string> outputs;
    bool enabled;
    
    PipelineStage(const std::string& n = "Stage", const std::string& desc = "")
        : name(n), description(desc), enabled(true) {}
};

// Hyperparameter optimization result
struct OptimizationResult {
    std::map<std::string, ConfigValue> bestConfig;
    float bestScore;
    size_t iteration;
    float convergenceRate;
    std::vector<std::map<std::string, ConfigValue>> history;
    std::vector<float> scores;
    
    OptimizationResult() : bestScore(0.0f), iteration(0), convergenceRate(0.0f) {}
};

// Configuration automation system
class ConfigAutomation {
public:
    ConfigAutomation();
    ~ConfigAutomation();
    
    // Configuration templating
    void addTemplate(const ConfigurationTemplate& template);
    void removeTemplate(const std::string& name);
    ConfigurationTemplate* getTemplate(const std::string& name);
    std::vector<ConfigurationTemplate> getAllTemplates() const;
    bool generateConfigFromTemplate(const std::string& templateName, Config& config);
    Config createConfigFromTemplate(const std::string& templateName);
    std::vector<Config> generateConfigVariations(const std::string& templateName,
                                                 size_t numVariations);
    
    // Configuration macro management
    void addMacro(const ConfigurationMacro& macro);
    void removeMacro(const std::string& name);
    ConfigurationMacro* getMacro(const std::string& name);
    std::vector<ConfigurationMacro> getAllMacros() const;
    bool applyMacroToConfig(const std::string& macroName, Config& config);
    Config applyMacro(const std::string& macroName);
    bool resolveMacroDependencies(const std::string& macroName, Config& config);
    
    // Batch configuration generation
    std::vector<Config> generateBatchConfigurations(const std::vector<std::string>& templateNames,
                                                    size_t numPerTemplate = 1);
    std::vector<Config> generateRandomConfigurations(size_t numConfigs,
                                                     const std::map<std::string, std::vector<ConfigValue>>& parameterRanges);
    std::vector<Config> generateParameterGrid(const std::map<std::string, std::vector<ConfigValue>>& parameterGrid);
    
    // Configuration validation and quality control
    bool validateConfig(const Config& config) const;
    bool validateParameterRanges(const Config& config) const;
    bool checkParameterConstraints(const Config& config) const;
    std::vector<std::string> getConfigIssues(const Config& config) const;
    std::vector<std::string> getConfigWarnings(const Config& config) const;
    float calculateConfigQuality(const Config& config) const;
    
    // Configuration comparison and merging
    std::map<std::string, ConfigDiff> compareConfigs(const Config& config1, const Config& config2) const;
    bool mergeConfigs(Config& target, const Config& source, ConfigSource sourceType = ConfigSource::Runtime);
    Config createInheritedConfig(const std::vector<Config>& configs) const;
    float calculateConfigSimilarity(const Config& config1, const Config& config2) const;
    
    // Configuration optimization
    OptimizationResult optimizeConfig(const OptimizationObjective& objective,
                                     const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                     const std::function<float(const Config&)>& objectiveFunction,
                                     AutomationStrategy strategy = AutomationStrategy::GridSearch,
                                     size_t maxIterations = 100,
                                     float convergenceThreshold = 0.01f);
    OptimizationResult bayesianOptimize(const OptimizationObjective& objective,
                                       const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                       const std::function<float(const Config&)>& objectiveFunction,
                                       size_t numInitialPoints = 10,
                                       size_t numIterations = 50);
    OptimizationResult geneticAlgorithmOptimize(const OptimizationObjective& objective,
                                               const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                               const std::function<float(const Config&)>& objectiveFunction,
                                               size_t populationSize = 20,
                                               size_t generations = 50,
                                               float mutationRate = 0.1f,
                                               float crossoverRate = 0.8f);
    OptimizationResult particleSwarmOptimize(const OptimizationObjective& objective,
                                            const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                            const std::function<float(const Config&)>& objectiveFunction,
                                            size_t swarmSize = 30,
                                            size_t maxIterations = 100,
                                            float inertiaWeight = 0.5f,
                                            float cognitiveWeight = 1.5f,
                                            float socialWeight = 1.5f);
    OptimizationResult simulatedAnnealingOptimize(const OptimizationObjective& objective,
                                                 const std::map<std::string, std::vector<ConfigValue>>& searchSpace,
                                                 const std::function<float(const Config&)>& objectiveFunction,
                                                 float initialTemperature = 100.0f,
                                                 float coolingRate = 0.95f,
                                                 size_t maxIterations = 1000);
    
    // Configuration sharing and reproducibility
    bool exportConfigToFile(const Config& config, const std::string& filepath) const;
    Config importConfigFromFile(const std::string& filepath);
    std::string exportConfigToJSON(const Config& config) const;
    Config importConfigFromJSON(const std::string& json);
    std::string exportConfigToDict(const Config& config) const;
    bool importConfigFromDict(Config& config, const std::string& dict);
    bool shareConfig(const Config& config, const std::string& shareName,
                    const std::vector<std::string>& recipients);
    bool loadSharedConfig(const std::string& shareName, Config& config);
    
    // Advanced automation features
    std::vector<Config> generateEnsembleConfigurations(const std::vector<std::string>& templateNames,
                                                       const std::map<std::string, float>& weights);
    Config createOptimalConfiguration(const OptimizationObjective& objective,
                                      const std::vector<Config>& candidateConfigs);
    float evaluateConfiguration(Config& config, const std::function<float(const Config&)>& evaluator);
    bool runConfigurationSweep(const std::map<std::string, std::vector<ConfigValue>>& parameterSpace,
                              const std::function<float(const Config&)>& objectiveFunction);
    bool runConfigurationPipeline(const std::vector<PipelineStage>& pipeline,
                                 std::vector<Config>& outputs);
    
    // Configuration history and versioning
    void saveConfigVersion(const Config& config, const std::string& versionName);
    Config loadConfigVersion(const std::string& versionName) const;
    std::vector<std::string> getConfigVersions() const;
    bool deleteConfigVersion(const std::string& versionName);
    bool compareConfigVersions(const std::string& version1, const std::string& version2);
    bool revertConfigToVersion(Config& config, const std::string& versionName);
    
    // Configuration analysis
    std::map<std::string, float> analyzeConfigComplexity(const Config& config) const;
    std::map<std::string, float> analyzeConfigSensitivity(const Config& config) const;
    std::map<std::string, float> analyzeConfigRobustness(const Config& config) const;
    float calculateConfigEntropy(const Config& config) const;
    std::vector<std::string> getConfigRecommendations(const Config& config) const;
    
    // Configuration export/import
    bool exportAllConfigs(const std::string& directory);
    bool importAllConfigs(const std::string& directory);
    bool exportConfigArchive(const std::string& filepath, const std::vector<std::string>& configNames);
    bool importConfigArchive(const std::string& filepath, std::vector<Config>& configs);
    
    // Configuration monitoring
    void startConfigMonitoring();
    void stopConfigMonitoring();
    bool isConfigMonitoringEnabled() const { return configMonitoringEnabled; }
    void setMonitoringInterval(float interval) { monitoringInterval = interval; }
    float getMonitoringInterval() const { return monitoringInterval; }
    void setMonitoringCallback(std::function<void(const Config&)> callback) { monitoringCallback = callback; }
    
    // Configuration statistics
    size_t getConfigCount() const { return configCount; }
    float getAverageConfigQuality() const { return averageConfigQuality; }
    size_t getSuccessfulConfigurations() const { return successfulConfigurations; }
    size_t getFailedConfigurations() const { return failedConfigurations; }
    std::map<std::string, size_t> getTemplateUsageStats() const;
    std::map<std::string, size_t> getMacroUsageStats() const;
    
    // Configuration reporting
    std::string getAutomationReport() const;
    std::string exportConfigStats(const std::string& format = "json") const;
    bool generateConfigDocumentation(const std::string& filepath);
    bool generateUserGuide(const std::string& filepath);
    
    // Advanced automation
    std::vector<Config> performAutoML(const std::vector<std::string>& taskTypes,
                                     const std::map<std::string, std::vector<ConfigValue>>& searchSpaces);
    bool runAutoTuning(const std::string& modelType, const std::string& dataset,
                      const std::map<std::string, std::vector<ConfigValue>>& hyperparameterSpace);
    Config createConfigFromDomainKnowledge(const std::string& domain, const std::string& task);
    bool validateDomainSpecificConfig(const Config& config, const std::string& domain);
    
protected:
    void updateStats(const Config& config, bool success);
    void cleanupOldVersions();
    bool validateTemplate(const ConfigurationTemplate& template) const;
    bool validateMacro(const ConfigurationMacro& macro) const;
    
    bool configMonitoringEnabled;
    float monitoringInterval;
    std::function<void(const Config&)> monitoringCallback;
    
    size_t configCount;
    float totalConfigQuality;
    size_t successfulConfigurations;
    size_t failedConfigurations;
    
    std::vector<ConfigurationTemplate> templates;
    std::vector<ConfigurationMacro> macros;
    std::vector<PipelineStage> pipeline;
    
    std::map<std::string, std::chrono::system_clock::time_point> configVersions;
    std::map<std::string, Config> versionData;
};

// Simulation pipeline system
class SimulationPipeline {
public:
    SimulationPipeline();
    ~SimulationPipeline();
    
    // Pipeline management
    void addStage(const PipelineStage& stage);
    void removeStage(const std::string& name);
    PipelineStage* getStage(const std::string& name);
    std::vector<PipelineStage> getAllStages() const;
    bool reorderStage(const std::string& name, size_t newPosition);
    bool enableStage(const std::string& name, bool enable);
    bool isStageEnabled(const std::string& name) const;
    
    // Pipeline execution
    bool executePipeline(std::vector<NeuralRegion*>& regions);
    bool executeStage(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    bool executeStageSequentially(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    bool executeStageInParallel(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    bool executeStageWithDependencies(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    
    // Pipeline configuration
    void setPipelineConfig(const std::map<std::string, ConfigValue>& config);
    std::map<std::string, ConfigValue> getPipelineConfig() const;
    void setStageConfig(const std::string& stageName, const std::map<std::string, ConfigValue>& config);
    std::map<std::string, ConfigValue> getStageConfig(const std::string& stageName) const;
    
    // Pipeline validation
    bool validatePipeline() const;
    bool validateStageDependencies() const;
    bool validateStageInputs(const std::string& stageName) const;
    bool validateStageOutputs(const std::string& stageName) const;
    std::vector<std::string> getPipelineIssues() const;
    
    // Pipeline monitoring
    void startPipelineMonitoring();
    void stopPipelineMonitoring();
    bool isPipelineMonitoringEnabled() const { return pipelineMonitoringEnabled; }
    void setMonitoringInterval(float interval) { pipelineMonitoringInterval = interval; }
    float getMonitoringInterval() const { return pipelineMonitoringInterval; }
    void setExecutionCallback(std::function<void(const PipelineStage&, const Config&)> callback) { executionCallback = callback; }
    
    // Pipeline statistics
    size_t getTotalExecutions() const { return totalExecutions; }
    float getAverageExecutionTime() const { return averageExecutionTime; }
    size_t getSuccessfulExecutions() const { return successfulExecutions; }
    size_t getFailedExecutions() const { return failedExecutions; }
    std::map<std::string, size_t> getStageExecutionStats() const;
    std::map<std::string, float> getStagePerformanceStats() const;
    
    // Pipeline reporting
    std::string getPipelineReport() const;
    std::string exportPipelineData(const std::string& format = "json") const;
    bool generatePipelineDocumentation(const std::string& filepath);
    bool generatePipelineStatusReport(const std::string& filepath);
    
    // Advanced pipeline features
    bool createDynamicPipeline(const std::vector<PipelineStage>& stages,
                               const std::map<std::string, std::vector<ConfigValue>>& parameterSpace);
    bool createConditionalPipeline(const std::vector<PipelineStage>& stages,
                                  const std::map<std::string, std::function<bool(const Config&)>> conditions);
    bool createAdaptivePipeline(const std::vector<PipelineStage>& stages,
                               const std::function<float(const Config&)>& performanceEvaluator);
    bool createHierarchicalPipeline(const std::vector<PipelineStage>& stages,
                                   const std::map<std::string, std::vector<std::string>>& hierarchy);
    
    // Pipeline optimization
    bool optimizePipelineExecution(std::vector<NeuralRegion*>& regions);
    bool reorderPipelineForEfficiency(std::vector<NeuralRegion*>& regions);
    bool parallelizePipelineStages(std::vector<NeuralRegion*>& regions);
    bool pipelineLoadBalancing(std::vector<NeuralRegion*>& regions);
    bool pipelineResourceAllocation(std::vector<NeuralRegion*>& regions);
    
    // Pipeline export/import
    bool savePipelineToFile(const std::string& filepath);
    bool loadPipelineFromFile(const std::string& filepath);
    std::string exportPipelineToJSON() const;
    bool importPipelineFromJSON(const std::string& json);
    bool exportPipelineToXML(const std::string& filepath) const;
    bool importPipelineFromXML(const std::string& filepath);
    
    // Pipeline analysis
    std::map<std::string, float> analyzePipelinePerformance(std::vector<NeuralRegion*>& regions);
    std::vector<std::string> identifyPipelineBottlenecks(std::vector<NeuralRegion*>& regions);
    std::vector<std::string> optimizePipelineComponents(std::vector<NeuralRegion*>& regions);
    float calculatePipelineEfficiency(std::vector<NeuralRegion*>& regions);
    float calculatePipelineThroughput(std::vector<NeuralRegion*>& regions);
    float calculatePipelineScalability(std::vector<NeuralRegion*>& regions);
    
    // Pipeline simulation
    bool simulatePipelineExecution(std::vector<NeuralRegion*>& regions);
    bool predictPipelinePerformance(std::vector<NeuralRegion*>& regions);
    bool benchmarkPipelineStages(std::vector<NeuralRegion*>& regions);
    bool profilePipelineResourceUsage(std::vector<NeuralRegion*>& regions);
    bool analyzePipelineDependencies(std::vector<NeuralRegion*>& regions);
    
protected:
    void updatePipelineStats(const PipelineStage& stage, const Config& config, bool success, float executionTime);
    bool executeStageWithRetry(const std::string& stageName, std::vector<NeuralRegion*>& regions, size_t maxRetries = 3);
    bool handleStageFailure(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    bool handleStageSuccess(const std::string& stageName, std::vector<NeuralRegion*>& regions);
    
    bool pipelineMonitoringEnabled;
    float pipelineMonitoringInterval;
    std::function<void(const PipelineStage&, const Config&)> executionCallback;
    
    size_t totalExecutions;
    float totalExecutionTime;
    size_t successfulExecutions;
    size_t failedExecutions;
    
    std::vector<PipelineStage> stages;
    std::map<std::string, ConfigValue> pipelineConfig;
};

} // namespace nlm
