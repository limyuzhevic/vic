#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

class ExperimentResult {
public:
    // Result status
    enum class Status {
        SUCCESS,
        FAILED,
        RUNNING,
        PAUSED,
        TIMEOUT,
        ERROR
    };
    
    // Aggregation mode
    enum class AggregationMode {
        MEAN,
        SUM,
        MAX,
        MIN,
        WEIGHTED_MEAN,
        MEDIAN
    };

public:
    ExperimentResult() = default;
    ~ExperimentResult() = default;
    
    // Create result from experiment
    static ExperimentResult fromExperiment(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment
    );
    
    // Add individual experiment result
    void addExperiment(std::shared_ptr<Experiment> experiment);
    
    // Calculate aggregated metrics
    double calculateMetric(const std::string& name, AggregationMode mode = AggregationMode::MEAN) const;
    
    // Compare two results
    bool compareWith(const ExperimentResult& other) const;
    
    // Get result status
    Status getStatus() const { return status_; }
    void setStatus(Status status) { status_ = status; }
    
    // Get experiment ID/name
    const std::string& getExperimentName() const { return experimentName_; }
    void setExperimentName(const std::string& name) { experimentName_ = name; }
    
    // Get metrics
    const std::unordered_map<std::string, double>& getMetrics() const { return metrics_; }
    double getMetric(const std::string& name) const;
    void setMetric(const std::string& name, double value);
    
    // Get step data
    const std::vector<Experiment::StepData>& getStepData() const { return stepData_; }
    void addStepData(const Experiment::StepData& stepData) { stepData_.push_back(stepData); }
    
    // Get experiment data
    const std::vector<std::string>& getNotes() const { return notes_; }
    void addNote(const std::string& note) { notes_.push_back(note); }
    
    // Get timing information
    double getDuration() const { return durationMs_; }
    void setDuration(double ms) { durationMs_ = ms; }
    
    // Get timestamp
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    void setTimestamp(const std::chrono::system_clock::time_point& tp) { timestamp_ = tp; }
    
    // Check if result is valid
    bool isValid() const { return status_ == Status::SUCCESS && !experimentName_.empty(); }
    
    // Serialize result to string
    std::string serialize() const;
    
    // Deserialize result from string
    static ExperimentResult deserialize(const std::string& serialized);
    
    // Get result summary
    std::string getSummary() const;
    
    // Calculate performance metrics
    struct PerformanceMetrics {
        double avgFiringRate;
        double totalReward;
        double efficiencyScore;
        double convergenceRate;
    };
    
    PerformanceMetrics calculatePerformanceMetrics() const;
    
    // Compare two results with tolerance
    bool approximatelyEqual(const ExperimentResult& other, double tolerance = 1e-6) const;
    
    // Get result hash for comparison
    size_t getHash() const;
    
    // Filter results by criteria
    std::vector<ExperimentResult> filterByMetric(const std::string& metricName, double threshold, bool greaterThan = true) const;
    
    // Get best result from collection
    static ExperimentResult getBestResult(const std::vector<ExperimentResult>& results, const std::string& metricName = "total_reward");
    
    // Get result version
    static std::string getVersion() { return "1.0.0"; }

private:
    Status status_;
    std::string experimentName_;
    std::unordered_map<std::string, double> metrics_;
    std::vector<Experiment::StepData> stepData_;
    std::vector<std::string> notes_;
    double durationMs_;
    std::chrono::system_clock::time_point timestamp_;
    
    // For result aggregation
    std::vector<std::shared_ptr<Experiment>> experimentPtrs_;
};

} // namespace nlm