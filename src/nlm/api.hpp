// nlm/api.hpp - High-level programmatic API for NLM brain control
#pragma once

#include "brain/Brain.hpp"
#include "brain/BrainFactory.hpp"
#include "plugins/integration_hooks.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace nlm {

// Forward declarations for convenience
class BrainController;
class ExperimentRunner;
class VisualizationInterface;
class MetricsCollector;

// Brain control API types and classes
class Pattern {
public:
    std::vector<float> neurons;
    std::vector<std::pair<size_t, size_t>> connections;
    float weight = 1.0f;
    
    Pattern(size_t neuronCount) : neurons(neuronCount, 0.0f) {}
    Pattern(const std::vector<float>& patternData) : neurons(patternData) {}
};

class BehavioralRule {
public:
    enum class Type {
        Reflex,
        Habit,
        Exploration,
        SocialImitation,
        RewardBased,
        NoveltyBased,
        Predictive
    };
    
    std::string name;
    Type type;
    float threshold;
    float strength;
    std::map<std::string, float> parameters;
    
    BehavioralRule(const std::string& ruleName, Type ruleType) 
        : name(ruleName), type(ruleType), threshold(0.5f), strength(1.0f) {}
};

class LearningScenario {
public:
    std::string scenarioId;
    std::vector<Pattern> inputPatterns;
    std::vector<float> expectedOutputs;
    float rewardSignal;
    float learningRate;
    size_t duration;
    
    LearningScenario(const std::string& id, size_t patternCount)
        : scenarioId(id), rewardSignal(1.0f), learningRate(0.01f), duration(100) {
        inputPatterns.reserve(patternCount);
        expectedOutputs.reserve(patternCount);
    }
};

class BrainController {
public:
    // Controller construction
    BrainController();
    explicit BrainController(const BrainFactoryConfig& config);
    explicit BrainController(std::shared_ptr<Brain> brain);
    ~BrainController();
    
    // Brain control methods
    bool initialize();
    void reset();
    void step(double timestep = 0.001);
    void runSteps(size_t steps, double timestep = 0.001);
    
    // Input/output control
    void injectPattern(const Pattern& pattern);
    void injectCurrent(NeuronId neuronId, float current);
    void injectCurrentToType(NeuronType type, float current);
    std::vector<float> getInternalState() const;
    MotorCommand getLastAction() const;
    
    // Learning control
    void applyLearningScenario(const LearningScenario& scenario);
    void trainPattern(const Pattern& input, float targetOutput);
    void enablePlasticity(bool enable);
    void setLearningRate(float rate);
    
    // Behavioral control
    void addBehavioralRule(const BehavioralRule& rule);
    void removeBehavioralRule(const std::string& ruleName);
    void modifyBehavioralRule(const std::string& ruleName, const BehavioralRule& newRule);
    std::vector<MotorCommand> getAvailableActions() const;
    
    // State and metrics
    float getCurrentFiringRate() const;
    size_t getTotalSpikeCount() const;
    size_t getActiveNeuronCount() const;
    float getLearningProgress() const;
    std::map<std::string, float> getPerformanceMetrics() const;
    
    // Checkpointing and persistence
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    bool exportBrain(const std::string& filepath) const;
    bool importBrain(const std::string& filepath);
    
    // Plugin integration
    bool registerPlugin(std::unique_ptr<plugins::IntegrationPlugin> plugin);
    bool unregisterPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Advanced control
    void setTargetBehavior(const std::string& behavior);
    void setEnvironmentalConditions(const std::string& conditions);
    void scheduleEvent(double time, const std::function<void()>& event);
    
    // Utility methods
    std::string getBrainInfo() const;
    std::vector<std::string> getPluginStatus() const;
    bool isHealthy() const;
    
    // Brain access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<const Brain> getBrain() const { return brain_; }
    bool hasBrain() const { return brain_ != nullptr; }
    
    // Configuration
    void updateConfig(const BrainFactoryConfig& config);
    BrainFactoryConfig getConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment runner for controlled experiments
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Experiment setup
    void addExperiment(const LearningScenario& experiment);
    void removeExperiment(const std::string& experimentId);
    void clearExperiments();
    
    // Experiment execution
    bool runExperiment(const std::string& experimentId, BrainController& controller);
    bool runAllExperiments(BrainController& controller);
    bool runSequentialExperiments(BrainController& controller);
    
    // Results collection and analysis
    std::vector<float> getExperimentResults(const std::string& experimentId) const;
    std::map<std::string, std::vector<float>> getAllResults() const;
    float computeExperimentAccuracy(const std::string& experimentId) const;
    float computeOverallPerformance() const;
    
    // Experiment management
    std::vector<std::string> getExperimentIds() const;
    bool isExperimentValid(const std::string& experimentId) const;
    void setExperimentParameters(const std::string& experimentId, const std::map<std::string, float>& parameters);
    
    // Statistics and reporting
    size_t getTotalExperimentsRun() const;
    size_t getSuccessfulExperiments() const;
    double getAverageExecutionTime() const;
    std::string generateReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization interface for brain state monitoring
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Visualization setup
    void setOutputFormat(const std::string& format); // json, xml, csv, etc.
    void setUpdateInterval(double interval); // seconds
    void enableRealTimeUpdates(bool enable);
    
    // Data visualization
    void addMetric(const std::string& name, float value);
    void addTimeSeries(const std::string& name, const std::vector<float>& data);
    void addBrainStateSnapshot(const BrainController& controller);
    void addExperimentResult(const std::string& experimentId, float result);
    
    // Export functionality
    std::string generateJSON() const;
    std::string generateXML() const;
    std::string generateCSV() const;
    bool saveToFile(const std::string& filepath) const;
    bool saveVisualizationData(const std::string& filepath) const;
    
    // Display and interaction
    void display() const;
    void showBrainState(const BrainController& controller) const;
    void showExperimentResults() const;
    void showMetrics() const;
    
    // Configuration
    void setMaxDataPoints(size_t maxPoints);
    void setDataRetentionTime(double retentionTime); // seconds
    void clearData();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metrics collection and analysis
class MetricsCollector {
public:
    MetricsCollector();
    ~MetricsCollector();
    
    // Metric collection
    void startCollecting(const std::string& controllerId);
    void stopCollecting(const std::string& controllerId);
    void collectSample(const BrainController& controller);
    void addCustomMetric(const std::string& name, float value, const std::string& context = "");
    
    // Metric analysis
    float getAverageMetric(const std::string& metricName) const;
    float getMaxMetric(const std::string& metricName) const;
    float getMinMetric(const std::string& metricName) const;
    float getStandardDeviation(const std::string& metricName) const;
    std::vector<float> getTimeSeries(const std::string& metricName) const;
    std::map<std::string, float> getMetricsSummary(const std::string& controllerId) const;
    
    // Experiment analysis
    float computeLearningCurve(const std::string& experimentId) const;
    float computeConvergenceTime(const std::string& experimentId) const;
    float computePerformanceRatio(const std::string& experimentId) const;
    std::vector<float> compareExperiments(const std::vector<std::string>& experimentIds) const;
    
    // Export and reporting
    std::string generateMetricsReport(const std::string& controllerId) const;
    bool exportMetricsToCSV(const std::string& filepath, const std::string& controllerId = "") const;
    bool exportMetricsToJSON(const std::string& filepath, const std::string& controllerId = "") const;
    
    // Configuration
    void setSamplingRate(float samplesPerSecond);
    void setMetricThreshold(const std::string& metricName, float threshold);
    void enableAnomalyDetection(bool enable);
    void setAnalysisWindow(double windowSize); // seconds
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for common tasks
namespace api {
    // Brain creation and setup
    std::shared_ptr<BrainController> createBrainController(const std::string& profile = "standard");
    std::shared_ptr<BrainController> createMinimalBrainController();
    std::shared_ptr<BrainController> createResearchBrainController();
    std::shared_ptr<BrainController> createDemoBrainController(const std::string& demoType = "learning");
    
    // Brain loading and saving
    std::shared_ptr<BrainController> loadBrainFromFile(const std::string& filepath);
    bool saveBrainToFile(const BrainController& controller, const std::string& filepath);
    
    // Quick experiment setup
    LearningScenario createPatternLearningExperiment(const std::string& id, 
                                                      const std::vector<Pattern>& patterns,
                                                      const std::vector<float>& targets);
    LearningScenario createBehaviorLearningExperiment(const std::string& id,
                                                      const std::vector<BehavioralRule>& rules);
    
    // Brain state analysis
    std::map<std::string, float> analyzeBrainState(const BrainController& controller);
    std::vector<NeuronId> getActiveNeurons(const BrainController& controller);
    std::vector<std::pair<NeuronId, float>> getStrongestConnections(const BrainController& controller);
    
    // Utility functions
    std::string generatePatternString(const Pattern& pattern);
    Pattern parsePatternString(const std::string& patternStr);
    std::string generateBehaviorString(const BehavioralRule& rule);
    BehavioralRule parseBehaviorString(const std::string& behaviorStr);
    
    // Plugin management
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& name);
    bool loadPluginsFromDirectory(const std::string& directory);
    std::vector<std::string> getAvailablePlugins();
    
    // Global instances and management
    BrainController& getDefaultController();
    ExperimentRunner& getDefaultExperimentRunner();
    VisualizationInterface& getDefaultVisualizationInterface();
    MetricsCollector& getDefaultMetricsCollector();
    
    void shutdownAll();
    
    // Configuration presets
    BrainFactoryConfig getQuickTestConfig();
    BrainFactoryConfig getLearningDemoConfig();
    BrainFactoryConfig getPerformanceTestConfig();
    BrainFactoryConfig getIntegrationTestConfig();
    BrainFactoryConfig getResearchConfig();
    
    // Validation and setup
    bool validateBrainConfig(const BrainFactoryConfig& config);
    bool validateExperiment(const LearningScenario& experiment);
    bool setupDevelopmentEnvironment();
    
    // Command-line interface helpers
    int runCLI(int argc, char** argv);
    void printHelp(const std::string& programName);
}

// Global API instances (singleton pattern)
extern BrainController& getGlobalBrainController();
extern ExperimentRunner& getGlobalExperimentRunner();
extern VisualizationInterface& getGlobalVisualizationInterface();
extern MetricsCollector& getGlobalMetricsCollector();

// High-level API functions
void initializeNLM();
void shutdownNLM();
bool isNLMInitialized();
std::string getNLMVersion();
void printSystemInfo();

} // namespace nlm
