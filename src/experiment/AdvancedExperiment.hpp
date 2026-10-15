#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>
#include "../core/Config/Config.hpp"
#include "../core/ExtensibleBrain.hpp"
#include "../arch/NeuralArchitecture.hpp"

namespace nlm {

class AdvancedExperiment {
public:
    AdvancedExperiment();
    ~AdvancedExperiment();
    
    // Disable copying, enable moving
    AdvancedExperiment(const AdvancedExperiment&) = delete;
    AdvancedExperiment& operator=(const AdvancedExperiment&) = delete;
    AdvancedExperiment(AdvancedExperiment&&) noexcept;
    AdvancedExperiment& operator=(AdvancedExperiment&&) noexcept;
    
    // Experiment lifecycle
    bool initialize(const Config& config);
    void update(double timestep);
    void shutdown();
    
    // Experiment management
    bool addProtocol(std::unique_ptr<CustomProtocol> protocol);
    bool removeProtocol(const std::string& protocolName);
    CustomProtocol* getProtocol(const std::string& protocolName) const;
    std::vector<std::string> getAvailableProtocols() const;
    
    // Experiment execution
    bool run(const std::string& protocolName);
    bool runProtocol(const std::string& protocolName, const Config& parameters);
    bool pause();
    bool resume();
    bool stop();
    bool reset();
    
    // Hyperparameter optimization
    bool addHyperparameter(const std::string& name, const std::vector<double>& values);
    bool removeHyperparameter(const std::string& name);
    bool addOptimizationObjective(const std::string& objective,
                                 std::function<double(const Config&)> objectiveFunction);
    bool removeOptimizationObjective(const std::string& objective);
    std::string runHyperparameterOptimization(const HyperparameterOptimizer& optimizer);
    Config getOptimalParameters() const;
    
    // Experiment monitoring
    bool enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::string getExperimentStatus() const;
    std::chrono::system_clock::time_point getExperimentStartTime() const;
    double getExperimentDuration() const;
    
    // Data management
    bool saveResults(const std::string& filepath) const;
    bool loadResults(const std::string& filepath);
    bool exportResultsToCSV(const std::string& filepath) const;
    bool exportResultsToJSON(const std::string& filepath) const;
    
    // Protocol statistics
    std::string getProtocolStats(const std::string& protocolName) const;
    std::string getOptimizationStats() const;
    std::string getExperimentAnalytics() const;
    
    // Experiment configuration
    void setExperimentConfig(const Config& config);
    Config getExperimentConfig() const;
    void addExperimentParameter(const std::string& key, const ConfigValue& value);
    ConfigValue getExperimentParameter(const std::string& key) const;
    
    // Custom metric systems
    bool addCustomMetric(const std::string& name,
                        std::function<double(const class ExperimentData&)> metricCalculator);
    bool removeCustomMetric(const std::string& name);
    std::vector<std::string> getCustomMetrics() const;
    std::map<std::string, double> calculateAllMetrics(const class ExperimentData& data) const;
    
    // Experiment events
    using ExperimentCallback = std::function<void(const std::string& eventName, const class ExperimentEvent& event)>;
    void registerExperimentCallback(const std::string& eventType, ExperimentCallback callback);
    void triggerExperimentEvent(const std::string& eventName, const class ExperimentEvent& event);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment data structure
struct ExperimentData {
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::map<std::string, ConfigValue> parameters;
    std::string protocolName;
    double progress;
    bool completed;
    
    ExperimentData()
        : progress(0.0), completed(false), timestamp(std::chrono::system_clock::now()) {}
};

// Experiment event structure
struct ExperimentEvent {
    std::string eventType;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::map<std::string, std::string> data;
    bool isImportant;
    
    ExperimentEvent() : isImportant(false), timestamp(std::chrono::system_clock::now()) {}
    ExperimentEvent(const std::string& type, const std::string& src)
        : eventType(type), source(src), isImportant(false), timestamp(std::chrono::system_clock::now()) {}
};

// Custom protocol interface
class CustomProtocol {
public:
    virtual ~CustomProtocol() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    
    // Protocol execution
    virtual bool start() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool stop() = 0;
    virtual bool reset() = 0;
    
    // Data collection
    virtual bool collectData(const class ExperimentData& data) = 0;
    virtual bool validateData(const class ExperimentData& data) = 0;
    virtual std::map<std::string, double> calculateMetrics(const class ExperimentData& data) = 0;
    
    // Configuration management
    virtual void setParameters(const Config& parameters) = 0;
    virtual Config getParameters() const = 0;
    virtual bool validateParameters(const Config& parameters) const = 0;
    
    // Protocol-specific methods
    virtual bool configureBrain(class ExtensibleBrain& brain) = 0;
    virtual bool configureArchitecture(class NeuralArchitecture& arch) = 0;
    virtual bool setupEnvironment(class Environment& env) = 0;
};

// Built-in protocol implementations
class ScalingBenchmarkProtocol : public CustomProtocol {
public:
    ScalingBenchmarkProtocol() = default;
    ~ScalingBenchmarkProtocol() override = default;
    
    std::string getName() const override { return "ScalingBenchmark"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool start() override;
    bool pause() override;
    bool resume() override;
    bool stop() override;
    bool reset() override;
    
    bool collectData(const class ExperimentData& data) override;
    bool validateData(const class ExperimentData& data) override;
    std::map<std::string, double> calculateMetrics(const class ExperimentData& data) override;
    
    void setParameters(const Config& parameters) override;
    Config getParameters() const override;
    bool validateParameters(const Config& parameters) const override;
    
    bool configureBrain(class ExtensibleBrain& brain) override;
    bool configureArchitecture(class NeuralArchitecture& arch) override;
    bool setupEnvironment(class Environment& env) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PhaseTrainingProtocol : public CustomProtocol {
public:
    PhaseTrainingProtocol() = default;
    ~PhaseTrainingProtocol() override = default;
    
    std::string getName() const override { return "PhaseTraining"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool start() override;
    bool pause() override;
    bool resume() override;
    bool stop() override;
    bool reset() override;
    
    bool collectData(const class ExperimentData& data) override;
    bool validateData(const class ExperimentData& data) override;
    std::map<std::string, double> calculateMetrics(const class ExperimentData& data) override;
    
    void setParameters(const Config& parameters) override;
    Config getParameters() const override;
    bool validateParameters(const Config& parameters) const override;
    
    bool configureBrain(class ExtensibleBrain& brain) override;
    bool configureArchitecture(class NeuralArchitecture& arch) override;
    bool setupEnvironment(class Environment& env) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hyperparameter optimizer interface
class HyperparameterOptimizer {
public:
    virtual ~HyperparameterOptimizer() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    
    // Optimization methods
    virtual bool addSearchSpace(const std::string& name, const std::vector<double>& values) = 0;
    virtual bool removeSearchSpace(const std::string& name) = 0;
    virtual bool addObjectiveFunction(const std::string& name,
                                     std::function<double(const Config&)> objective) = 0;
    virtual bool removeObjectiveFunction(const std::string& name) = 0;
    
    virtual Config optimize(const std::string& objectiveName) = 0;
    virtual std::map<std::string, double> optimize(const std::string& objectiveName,
                                                   const std::vector<std::string>& parameterNames) = 0;
    
    // Optimization algorithms
    virtual bool setAlgorithm(const std::string& algorithm) = 0;
    virtual std::string getAlgorithm() const = 0;
    virtual bool setOptimizationParameters(const std::map<std::string, double>& parameters) = 0;
    virtual std::map<std::string, double> getOptimizationParameters() const = 0;
    
    // Results
    virtual Config getOptimalParameters() const = 0;
    virtual double getOptimalObjectiveValue() const = 0;
    virtual size_t getIterationCount() const = 0;
    virtual double getConvergenceRate() const = 0;
    virtual std::string getOptimizationStats() const = 0;
    
    // Optimization algorithms
    virtual bool runGradientDescent(const std::string& objectiveName) = 0;
    virtual bool runRandomSearch(const std::string& objectiveName) = 0;
    virtual bool runBayesianOptimization(const std::string& objectiveName) = 0;
    virtual bool runGeneticAlgorithm(const std::string& objectiveName) = 0;
    virtual bool runSimulatedAnnealing(const std::string& objectiveName) = 0;
};

// Built-in optimizer implementations
class RandomSearchOptimizer : public HyperparameterOptimizer {
public:
    RandomSearchOptimizer() = default;
    ~RandomSearchOptimizer() override = default;
    
    std::string getName() const override { return "RandomSearch"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool addSearchSpace(const std::string& name, const std::vector<double>& values) override;
    bool removeSearchSpace(const std::string& name) override;
    bool addObjectiveFunction(const std::string& name,
                             std::function<double(const Config&)> objective) override;
    bool removeObjectiveFunction(const std::string& name) override;
    
    Config optimize(const std::string& objectiveName) override;
    std::map<std::string, double> optimize(const std::string& objectiveName,
                                           const std::vector<std::string>& parameterNames) override;
    
    bool setAlgorithm(const std::string& algorithm) override;
    std::string getAlgorithm() const override;
    bool setOptimizationParameters(const std::map<std::string, double>& parameters) override;
    std::map<std::string, double> getOptimizationParameters() const override;
    
    Config getOptimalParameters() const override;
    double getOptimalObjectiveValue() const override;
    size_t getIterationCount() const override;
    double getConvergenceRate() const override;
    std::string getOptimizationStats() const override;
    
    bool runGradientDescent(const std::string& objectiveName) override;
    bool runRandomSearch(const std::string& objectiveName) override;
    bool runBayesianOptimization(const std::string& objectiveName) override;
    bool runGeneticAlgorithm(const std::string& objectiveName) override;
    bool runSimulatedAnnealing(const std::string& objectiveName) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class BayesianOptimizer : public HyperparameterOptimizer {
public:
    BayesianOptimizer() = default;
    ~BayesianOptimizer() override = default;
    
    std::string getName() const override { return "BayesianOptimizer"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool addSearchSpace(const std::string& name, const std::vector<double>& values) override;
    bool removeSearchSpace(const std::string& name) override;
    bool addObjectiveFunction(const std::string& name,
                             std::function<double(const Config&)> objective) override;
    bool removeObjectiveFunction(const std::string& name) override;
    
    Config optimize(const std::string& objectiveName) override;
    std::map<std::string, double> optimize(const std::string& objectiveName,
                                           const std::vector<std::string>& parameterNames) override;
    
    bool setAlgorithm(const std::string& algorithm) override;
    std::string getAlgorithm() const override;
    bool setOptimizationParameters(const std::map<std::string, double>& parameters) override;
    std::map<std::string, double> getOptimizationParameters() const override;
    
    Config getOptimalParameters() const override;
    double getOptimalObjectiveValue() const override;
    size_t getIterationCount() const override;
    double getConvergenceRate() const override;
    std::string getOptimizationStats() const override;
    
    bool runGradientDescent(const std::string& objectiveName) override;
    bool runRandomSearch(const std::string& objectiveName) override;
    bool runBayesianOptimization(const std::string& objectiveName) override;
    bool runGeneticAlgorithm(const std::string& objectiveName) override;
    bool runSimulatedAnnealing(const std::string& objectiveName) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class GeneticAlgorithmOptimizer : public HyperparameterOptimizer {
public:
    GeneticAlgorithmOptimizer() = default;
    ~GeneticAlgorithmOptimizer() override = default;
    
    std::string getName() const override { return "GeneticAlgorithm"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool addSearchSpace(const std::string& name, const std::vector<double>& values) override;
    bool removeSearchSpace(const std::string& name) override;
    bool addObjectiveFunction(const std::string& name,
                             std::function<double(const Config&)> objective) override;
    bool removeObjectiveFunction(const std::string& name) override;
    
    Config optimize(const std::string& objectiveName) override;
    std::map<std::string, double> optimize(const std::string& objectiveName,
                                           const std::vector<std::string>& parameterNames) override;
    
    bool setAlgorithm(const std::string& algorithm) override;
    std::string getAlgorithm() const override;
    bool setOptimizationParameters(const std::map<std::string, double>& parameters) override;
    std::map<std::string, double> getOptimizationParameters() const override;
    
    Config getOptimalParameters() const override;
    double getOptimalObjectiveValue() const override;
    size_t getIterationCount() const override;
    double getConvergenceRate() const override;
    std::string getOptimizationStats() const override;
    
    bool runGradientDescent(const std::string& objectiveName) override;
    bool runRandomSearch(const std::string& objectiveName) override;
    bool runBayesianOptimization(const std::string& objectiveName) override;
    bool runGeneticAlgorithm(const std::string& objectiveName) override;
    bool runSimulatedAnnealing(const std::string& objectiveName) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
