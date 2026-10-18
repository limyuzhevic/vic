#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>

namespace nlm {

// ExperimentResult: Result of an experiment execution
class ExperimentResult {
public:
    enum class Status {
        SUCCESS,
        FAILED,
        RUNNING,
        PAUSED,
        TIMEOUT,
        ERROR
    };

public:
    ExperimentResult();
    ~ExperimentResult();
    
    // Create result from experiment data
    void fromExperiment(const Experiment& experiment);
    
    // Get result status
    Status getStatus() const { return status_; }
    void setStatus(Status status) { status_ = status; }
    
    // Get experiment name
    const std::string& getExperimentName() const { return experimentName_; }
    void setExperimentName(const std::string& name) { experimentName_ = name; }
    
    // Get metrics
    const std::unordered_map<std::string, double>& getMetrics() const { return metrics_; }
    double getMetric(const std::string& name) const;
    void setMetric(const std::string& name, double value);
    
    // Get timing information
    double getDuration() const { return durationMs_; }
    void setDuration(double ms) { durationMs_ = ms; }
    
    // Get timestamp
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    void setTimestamp(const std::chrono::system_clock::time_point& tp) { timestamp_ = tp; }
    
    // Get step data
    const std::vector<StepData>& getStepData() const { return stepData_; }
    void addStepData(const StepData& stepData) { stepData_.push_back(stepData); }
    
    // Get notes
    const std::vector<std::string>& getNotes() const { return notes_; }
    void addNote(const std::string& note) { notes_.push_back(note); }
    
    // Check if result is valid
    bool isValid() const { return status_ == Status::SUCCESS && !experimentName_.empty(); }
    
    // Compare with another result
    bool compareWith(const ExperimentResult& other) const;
    
    // Clone experiment result
    std::unique_ptr<ExperimentResult> clone() const;
    
    // Serialize result
    std::string serialize() const;
    
    // Deserialize result
    static std::unique_ptr<ExperimentResult> deserialize(const std::string& serialized);
    
    // Calculate performance metrics
    struct PerformanceMetrics {
        double avgFiringRate;
        double totalReward;
        double efficiencyScore;
        double convergenceRate;
    };
    
    PerformanceMetrics calculatePerformanceMetrics() const;
    
    // Get result version
    static std::string getVersion() { return "1.0.0"; }

private:
    Status status_;
    std::string experimentName_;
    std::unordered_map<std::string, double> metrics_;
    std::vector<StepData> stepData_;
    std::vector<std::string> notes_;
    double durationMs_;
    std::chrono::system_clock::time_point timestamp_;
};

// Enhanced Experiment: Supports cloning and advanced features
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    ~Experiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Random seed
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Metrics
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    // Get all metrics
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Get metric value
    double getMetric(const std::string& name) const;
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Recording step data
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    // Get step data
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Duration
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    // Notes
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
    // ========== CLONING AND STATE MANAGEMENT ==========
    
    // Clone experiment with state preservation
    std::unique_ptr<Experiment> clone() const;
    
    // Save experiment checkpoint
    bool save(const std::string& filepath) const;
    
    // Load experiment checkpoint
    bool load(const std::string& filepath);
    
    // Compare with another experiment
    bool compareWith(const Experiment& other) const;
    
    // Get experiment hash for comparison
    size_t getHash() const;
    
    // Get creation timestamp
    const std::chrono::system_clock::time_point& getCreationTime() const { return creationTime_; }
    
    // Get last modified timestamp
    const std::chrono::system_clock::time_point& getLastModifiedTime() const { return lastModifiedTime_; }
    
    // ========== ADVANCED FEATURES ==========
    
    // Calculate experiment metrics
    struct MetricsSummary {
        double averageStepDuration;
        double maxFiringRate;
        double totalNeurons;
        double totalSynapses;
        double totalReward;
        double efficiencyScore;
    };
    
    MetricsSummary calculateMetricsSummary() const;
    
    // Filter steps by criteria
    std::vector<StepData> filterSteps(std::function<bool(const StepData&)> predicate) const;
    
    // Get step statistics
    struct StepStatistics {
        double avgNeurons;
        double avgSynapses;
        double avgFiringRate;
        double avgFiringCount;
    };
    StepStatistics getStepStatistics() const;
    
    // Check if experiment has data
    bool hasData() const;
    
    // Get experiment size (in memory)
    size_t getMemoryUsage() const;
    
    // ========== STATIC UTILITY FUNCTIONS ==========
    
    // Create experiment from configuration
    static std::unique_ptr<Experiment> createFromConfig(const std::string& configJson);
    
    // Create experiment with default settings
    static std::unique_ptr<Experiment> createDefault(const std::string& name);
    
    // Get experiment version
    static std::string getVersion() { return "1.0.0"; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::chrono::system_clock::time_point creationTime_;
    std::chrono::system_clock::time_point lastModifiedTime_;
};

} // namespace nlm
