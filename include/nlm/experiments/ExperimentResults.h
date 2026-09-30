#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <limits>

namespace nlm {

/**
 * Advanced experiment results storage and statistical analysis
 * for comprehensive neural network experiment data collection and analysis.
 */
class ExperimentResults {
public:
    // Result metrics container
    struct MetricData {
        std::string name;
        std::vector<double> values;
        double mean;
        double stdDev;
        double min;
        double max;
        double median;
        double percentile95;
        
        MetricData() : mean(0.0), stdDev(0.0), min(std::numeric_limits<double>::max()), 
                      max(std::numeric_limits<double>::lowest()), median(0.0), percentile95(0.0) {}
        
        void calculateStatistics();
        bool isValid() const;
    };
    
    // Time series data
    struct TimeSeriesData {
        std::chrono::steady_clock::time_point timestamp;
        double value;
        std::string label;
        
        TimeSeriesData() : value(0.0) {}
        TimeSeriesData(const std::chrono::steady_clock::time_point& t, double v, const std::string& l = "")
            : timestamp(t), value(v), label(l) {}
    };
    
    // Experiment summary statistics
    struct ExperimentSummary {
        std::string experimentName;
        double totalReward;
        double avgFiringRate;
        double avgLearningRate;
        double convergenceSpeed;
        double finalAccuracy;
        size_t totalSteps;
        size_t totalTrials;
        double totalTimeSeconds;
        bool converged;
        
        ExperimentSummary() : totalReward(0.0), avgFiringRate(0.0), avgLearningRate(0.0),
                             convergenceSpeed(0.0), finalAccuracy(0.0), totalSteps(0), 
                             totalTrials(0), totalTimeSeconds(0.0), converged(false) {}
    };
    
    ExperimentResults();
    ~ExperimentResults() = default;
    
    // Results collection
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, double value, size_t trial);
    void recordTimeSeries(const std::chrono::steady_clock::time_point& timestamp, 
                         double value, const std::string& label = "");
    
    // Metric management
    bool hasMetric(const std::string& name) const;
    const MetricData& getMetric(const std::string& name) const;
    const std::vector<std::string>& getMetricNames() const;
    
    // Time series access
    const std::vector<TimeSeriesData>& getTimeSeries() const;
    std::vector<double> getTimeSeriesValues(const std::string& label = "") const;
    
    // Statistical analysis
    double calculateCorrelation(const std::string& metric1, const std::string& metric2) const;
    double calculateRegression(const std::string& dependent, const std::string& independent) const;
    std::vector<double> calculateMovingAverage(const std::string& metric, size_t window) const;
    
    // Result validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Result export
    std::string exportToCSV() const;
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    std::string exportToText() const;
    
    // File I/O
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Result comparison
    double compareWith(const ExperimentResults& other) const;
    
    // Result aggregation
    static ExperimentResults aggregate(const std::vector<ExperimentResults>& results);
    
    // Visualization support
    std::string generatePlotData(const std::string& metric) const;
    
    // Result clearing and statistics
    void clear();
    size_t getMetricCount() const;
    size_t getTimeSeriesCount() const;
    
    // Get results
    const std::map<std::string, MetricData>& getAllMetrics() const;
    const ExperimentSummary& getSummary() const;
    void setSummary(const ExperimentSummary& summary);
    
    // Time series filtering
    std::vector<TimeSeriesData> filterTimeSeries(const std::string& label = "",
                                                 const std::chrono::steady_clock::time_point& start = {}, 
                                                 const std::chrono::steady_clock::time_point& end = {}) const;
    
private:
    // Results storage
    std::map<std::string, MetricData> metrics;
    std::vector<TimeSeriesData> timeSeries;
    ExperimentSummary summary;
    
    // Validation
    mutable std::vector<std::string> validationErrors;
    bool validateMetric(const MetricData& metric) const;
    
    // Statistics calculation helpers
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values, double mean);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
};

} // namespace nlm