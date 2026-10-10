// PerformanceMetrics - Enhanced Python bindings - Part 1: Core classes
#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>

namespace nlm {

// PerformanceMetrics - Enhanced version for Python bindings
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Core performance metrics
    struct MetricData {
        double spikeCount = 0.0;
        double firingRate = 0.0;
        double memoryUsage = 0.0;
        double simulationTime = 0.0;
        double learningProgress = 0.0;
        double cpuUsage = 0.0;
        double gpuMemory = 0.0;
        double energyConsumption = 0.0;
        double networkLatency = 0.0;
    };
    
    // Update metrics with new data
    void update(double step, double time);
    
    // Get current metrics
    const MetricData& getCurrentMetrics() const { return currentMetrics_; }
    
    // Historical data access
    const std::vector<double>& getSpikeCountHistory() const { return spikeCountHistory_; }
    const std::vector<double>& getFiringRateHistory() const { return firingRateHistory_; }
    const std::vector<double>& getMemoryUsageHistory() const { return memoryUsageHistory_; }
    
    // Statistics
    double getAverageSpikeCount() const;
    double getAverageFiringRate() const;
    double getPeakMemoryUsage() const;
    double getTotalSimulationTime() const;
    
    // Formatting
    std::string getFormattedSummary() const;
    std::string getFormattedMetrics() const;
    
    // Reset metrics
    void reset();
    
    // Export data
    std::string exportCSV() const;
    std::string exportJSON() const;
    
private:
    void recordMetric(const std::string& name, double value);
    
    MetricData currentMetrics_;
    std::vector<double> spikeCountHistory_;
    std::vector<double> firingRateHistory_;
    std::vector<double> memoryUsageHistory_;
    std::vector<double> simulationTimeHistory_;
    std::vector<double> learningProgressHistory_;
    
    double totalSpikeCount_ = 0.0;
    double totalFiringRate_ = 0.0;
    double peakMemoryUsage_ = 0.0;
    double totalSimulationTime_ = 0.0;
    std::chrono::steady_clock::time_point startTime_;
};

// LearningRule - Advanced learning configuration
class LearningRule {
public:
    enum class RuleType {
        STDP,
        Hebbian,
        RewardModulated,
        CuriosityDriven,
        Custom
    };
    
    LearningRule();
    ~LearningRule();
    
    // Rule configuration
    void setType(RuleType type) { type_ = type; }
    RuleType getType() const { return type_; }
    
    // Parameters
    void setParameter(const std::string& name, double value);
    double getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    
    // Learning rate
    void setLearningRate(double rate) { learningRate_ = rate; }
    double getLearningRate() const { return learningRate_; }
    
    // Rule-specific parameters
    void setLTPWeight(double weight) { parameters_["ltp_weight"] = weight; }
    void setLTDWeight(double weight) { parameters_["ltd_weight"] = weight; }
    void setSTDPWindow(double window) { parameters_["stdp_window"] = window; }
    void setHebbianStrength(double strength) { parameters_["hebbian_strength"] = strength; }
    
    // Validation
    bool validate() const;
    std::string getValidationError() const;
    
    // Export/import
    std::string exportToString() const;
    bool importFromString(const std::string& data);
    
    // Preset configurations
    static LearningRule createSTDP();
    static LearningRule createHebbian();
    static LearningRule createRewardModulated();
    static LearningRule createCuriosityDriven();
    static LearningRule createCustom(const std::map<std::string, double>& params);
    
private:
    RuleType type_;
    double learningRate_;
    std::map<std::string, double> parameters_;
    std::vector<std::string> validationErrors_;
};

// ArchitectureConfig - Brain architecture configuration
class ArchitectureConfig {
public:
    ArchitectureConfig();
    ~ArchitectureConfig();
    
    // Core parameters
    void setNeuronCount(size_t count) { neuronCount_ = count; }
    size_t getNeuronCount() const { return neuronCount_; }
    
    void setRegionCount(size_t count) { regionCount_ = count; }
    size_t getRegionCount() const { return regionCount_; }
    
    void setConnectionProbability(double prob) { connectionProbability_ = prob; }
    double getConnectionProbability() const { return connectionProbability_; }
    
    // Region configuration
    struct RegionInfo {
        std::string type;
        size_t size;
        double excitability;
        double plasticity;
    };
    
    void addRegion(const std::string& type, size_t size, double excitability = 1.0, double plasticity = 1.0);
    const std::vector<RegionInfo>& getRegions() const { return regions_; }
    
    // Validation
    bool validate() const;
    std::string getValidationError() const;
    
    // Export/import
    std::string exportToString() const;
    bool importFromString(const std::string& data);
    
private:
    size_t neuronCount_;
    size_t regionCount_;
    double connectionProbability_;
    std::vector<RegionInfo> regions_;
    std::vector<std::string> validationErrors_;
};

// ExperimentConfig - Experiment configuration
class ExperimentConfig {
public:
    ExperimentConfig();
    ~ExperimentConfig();
    
    // Basic info
    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    
    void setDescription(const std::string& desc) { description_ = desc; }
    const std::string& getDescription() const { return description_; }
    
    // Parameters
    void setParameter(const std::string& name, double value);
    double getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    
    // Experiment settings
    void setNumSteps(int steps) { numSteps_ = steps; }
    int getNumSteps() const { return numSteps_; }
    
    void setTimestep(double dt) { timestep_ = dt; }
    double getTimestep() const { return timestep_; }
    
    // Validation
    bool validate() const;
    std::string getValidationError() const;
    
    // Export/import
    std::string exportToString() const;
    bool importFromString(const std::string& data);
    
    // Preset configurations
    static ExperimentConfig createSimpleLearning();
    static ExperimentConfig createComplexLearning();
    static ExperimentConfig createDevelopmentExperiment();
    static ExperimentConfig createCustom(const std::map<std::string, double>& params);
    
private:
    std::string name_;
    std::string description_;
    std::map<std::string, double> parameters_;
    int numSteps_;
    double timestep_;
    std::vector<std::string> validationErrors_;
};

// EnvironmentConfig - Environment-based configuration
class EnvironmentConfig {
public:
    EnvironmentConfig();
    ~EnvironmentConfig();
    
    // Environment variables
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // Load from environment
    bool loadFromEnvironment();
    bool validate() const;
    std::string getValidationError() const;
    
    // Configuration files
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Variable types
    enum class VariableType {
        String,
        Integer,
        Float,
        Boolean,
        Path
    };
    
    void setVariableType(const std::string& name, VariableType type);
    VariableType getVariableType(const std::string& name) const;
    
private:
    std::map<std::string, std::string> variables_;
    std::map<std::string, VariableType> variableTypes_;
    std::vector<std::string> validationErrors_;
};

// System utilities
namespace utils {
    // System information
    std::string getSystemInfo();
    
    // Memory statistics
    struct MemoryStats {
        size_t totalMemory;
        size_t usedMemory;
        size_t availableMemory;
        double memoryUsagePercent;
    };
    MemoryStats getMemoryStats();
    
    // Performance benchmark
    struct BenchmarkResult {
        double initializationTime;
        double simulationTime;
        double memoryEfficiency;
        double spikeRateEfficiency;
        std::string hardwareInfo;
    };
    BenchmarkResult getPerformanceBenchmark();
    
    // Configuration utilities
    bool validateConfiguration(const std::string& configPath);
    std::string getDefaultConfig();
}

} // namespace nlm
