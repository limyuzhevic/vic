// nlm/metrics.hpp - Comprehensive metrics collection for NLM
#pragma once

#include "nlm/api.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace nlm {

// Metric types for different measurements
class Metric {
public:
    enum class Type {
        COUNTER,
        GAUGE,
        HISTOGRAM,
        SUMMARY,
        TIMER,
        PERCENTAGE
    };
    
    std::string name;
    Type type;
    float value;
    float minValue;
    float maxValue;
    std::chrono::steady_clock::time_point timestamp;
    std::string context;
    std::map<std::string, std::string> tags;
    
    Metric(const std::string& metricName, Type metricType, float metricValue)
        : name(metricName), type(metricType), value(metricValue),
          minValue(metricValue), maxValue(metricValue),
          timestamp(std::chrono::steady_clock::now()) {}
    
    Metric(const std::string& metricName, Type metricType, float metricValue,
           float min, float max)
        : name(metricName), type(metricType), value(metricValue),
          minValue(min), maxValue(max),
          timestamp(std::chrono::steady_clock::now()) {}
    
    void update(float newValue);
    void setContext(const std::string& ctx) { context = ctx; }
    void addTag(const std::string& key, const std::string& value) { tags[key] = value; }
    double getAgeSeconds() const;
    bool isStale(double maxAgeSeconds) const;
};

// Time series data for metrics
class TimeSeries {
public:
    std::string name;
    std::deque<std::pair<double, float>> dataPoints; // (timestamp, value)
    size_t maxPoints;
    double samplingInterval; // seconds
    
    TimeSeries(const std::string& seriesName, size_t maxPoints = 1000, 
               double interval = 0.1) : name(seriesName), maxPoints(maxPoints), 
                                         samplingInterval(interval) {}
    
    void addPoint(double timestamp, float value);
    float getLatestValue() const;
    std::vector<std::pair<double, float>> getDataPoints(double fromTime = 0.0) const;
    size_t getPointCount() const { return dataPoints.size(); }
    double getDuration() const;
    float getAverage() const;
    float getMax() const;
    float getMin() const;
    float getStandardDeviation() const;
    std::vector<float> getRecentValues(size_t count) const;
};

// Experiment results tracking
class ExperimentResult {
public:
    std::string experimentId;
    std::string controllerId;
    double startTime;
    double endTime;
    std::map<std::string, float> metrics;
    std::vector<float> performanceData;
    float accuracy;
    float successRate;
    float learningProgress;
    std::string status; // success, failed, running, cancelled
    
    ExperimentResult(const std::string& expId, const std::string& ctrlId)
        : experimentId(expId), controllerId(ctrlId), accuracy(0.0f), 
          successRate(0.0f), learningProgress(0.0f) {}
    
    void recordMetric(const std::string& name, float value);
    double getDuration() const { return endTime - startTime; }
    bool isCompleted() const { return !status.empty() && status != "running" && status != "cancelled"; }
};

// Learning curve analysis
class LearningCurve {
public:
    std::string experimentId;
    std::vector<double> timePoints;
    std::vector<float> performanceValues;
    std::vector<float> rewardValues;
    
    LearningCurve(const std::string& expId) : experimentId(expId) {}
    
    void addDataPoint(double time, float performance, float reward);
    float computeSlope(double startTime, double endTime) const;
    float computeAreaUnderCurve() const;
    double findInflectionPoint() const;
    float getMaximumPerformance() const;
    double getTimeToConvergence(float threshold = 0.95f) const;
    float computeStability() const;
    std::string getSummary() const;
};

// Anomaly detector for metrics
class AnomalyDetector {
public:
    enum class DetectionMethod {
        STATIC_THRESHOLD,
        MOVING_AVERAGE,
        STANDARD_DEVIATION,
        ISOLATION_FOREST,
        DBSCAN
    };
    
    DetectionMethod method;
    float threshold;
    size_t windowSize;
    float zScoreThreshold;
    
    AnomalyDetector() : method(DetectionMethod::STANDARD_DEVIATION), 
                       threshold(3.0f), windowSize(10), zScoreThreshold(2.0f) {}
    
    bool isAnomaly(const std::vector<float>& dataPoints, size_t index) const;
    std::vector<bool> detectAnomalies(const std::vector<float>& dataPoints) const;
    float computeZScore(const std::vector<float>& dataPoints, size_t index) const;
    std::vector<size_t> getAnomalyIndices(const std::vector<float>& dataPoints) const;
};

// Metrics collector implementation
class MetricsCollector {
public:
    MetricsCollector();
    ~MetricsCollector();
    
    // Metric collection
    void startCollecting(const std::string& controllerId);
    void stopCollecting(const std::string& controllerId);
    bool isCollecting(const std::string& controllerId) const;
    
    // Basic metrics
    void recordMetric(const std::string& controllerId, const Metric& metric);
    void recordCounter(const std::string& controllerId, const std::string& name, 
                       float value, const std::string& context = "");
    void recordGauge(const std::string& controllerId, const std::string& name, 
                    float value, const std::string& context = "");
    void recordHistogram(const std::string& controllerId, const std::string& name,
                         float value, float min, float max,
                         const std::string& context = "");
    void recordTimer(const std::string& controllerId, const std::string& name,
                     float duration, const std::string& context = "");
    void recordPercentage(const std::string& controllerId, const std::string& name,
                          float percentage, const std::string& context = "");
    
    // Time series metrics
    void recordTimeSeries(const std::string& controllerId, const TimeSeries& series);
    void updateTimeSeries(const std::string& controllerId, const std::string& name,
                          double timestamp, float value);
    
    // Brain controller metrics
    void recordBrainControllerMetrics(const std::string& controllerId, 
                                      const BrainController& controller);
    void recordExperimentStart(const std::string& controllerId, 
                              const std::string& experimentId);
    void recordExperimentEnd(const std::string& controllerId,
                            const std::string& experimentId,
                            const std::string& status, float accuracy);
    
    // Analysis methods
    Metric getLatestMetric(const std::string& controllerId, const std::string& name) const;
    std::vector<Metric> getMetricsHistory(const std::string& controllerId, 
                                          const std::string& name,
                                          double fromTime = 0.0) const;
    
    TimeSeries getTimeSeries(const std::string& controllerId, const std::string& name) const;
    std::vector<TimeSeries> getTimeSeriesList(const std::string& controllerId) const;
    
    ExperimentResult getExperimentResult(const std::string& experimentId) const;
    std::vector<ExperimentResult> getExperimentResults(const std::string& controllerId) const;
    
    LearningCurve getLearningCurve(const std::string& experimentId) const;
    std::vector<LearningCurve> getLearningCurves(const std::string& controllerId) const;
    
    // Statistical analysis
    float computeAverage(const std::string& controllerId, const std::string& metricName) const;
    float computeMax(const std::string& controllerId, const std::string& metricName) const;
    float computeMin(const std::string& controllerId, const std::string& metricName) const;
    float computeStandardDeviation(const std::string& controllerId, const std::string& metricName) const;
    float computeMedian(const std::string& controllerId, const std::string& metricName) const;
    
    // Comparison methods
    float compareControllers(const std::string& controllerId1, const std::string& controllerId2,
                            const std::string& metricName) const;
    std::map<std::string, float> compareExperiments(const std::string& controllerId,
                                                    const std::vector<std::string>& experimentIds) const;
    
    // Export and reporting
    std::string generateMetricsReport(const std::string& controllerId) const;
    std::string generateComparativeReport(const std::vector<std::string>& controllerIds) const;
    std::string generateLearningReport(const std::string& experimentId) const;
    
    bool exportMetricsToCSV(const std::string& filepath, const std::string& controllerId = "") const;
    bool exportMetricsToJSON(const std::string& filepath, const std::string& controllerId = "") const;
    bool exportLearningCurvesToJSON(const std::string& filepath,
                                    const std::vector<std::string>& experimentIds) const;
    
    // Configuration
    void setSamplingRate(const std::string& controllerId, float samplesPerSecond);
    void setMetricRetentionTime(const std::string& controllerId, double retentionTime); // seconds
    void setMaxDataPoints(const std::string& controllerId, size_t maxPoints);
    void enableAnomalyDetection(const std::string& controllerId, bool enable);
    void setAnomalyDetector(const AnomalyDetector& detector);
    
    // Control methods
    void clearControllerData(const std::string& controllerId);
    void clearAllData();
    void pauseCollecting(const std::string& controllerId);
    void resumeCollecting(const std::string& controllerId);
    
    // Status and monitoring
    std::vector<std::string> getCollectingControllerIds() const;
    size_t getMetricCount(const std::string& controllerId, const std::string& metricName) const;
    double getControllerDataAge(const std::string& controllerId) const;
    size_t getTotalMetricCount() const;
    size_t getTotalAnomalyCount() const;
    
    void printMetricsSummary(const std::string& controllerId) const;
    void printAnomalyReport(const std::string& controllerId) const;
    void generateSystemReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions for metrics
namespace metrics {
    // Global access to the metrics collector
    MetricsCollector& getCollector();
    
    // Quick metric recording
    void record(const std::string& name, float value);
    void record(const std::string& controllerId, const std::string& name, float value);
    void recordCounter(const std::string& name, float value);
    void recordGauge(const std::string& name, float value);
    void recordTimer(const std::string& name, float duration);
    
    // Brain controller metrics
    void recordBrainMetrics(const std::string& controllerId, const BrainController& controller);
    void recordExperimentMetrics(const std::string& experimentId, const std::map<std::string, float>& metrics);
    
    // Analysis functions
    std::string getReport(const std::string& controllerId);
    bool exportToCSV(const std::string& filepath, const std::string& controllerId = "");
    bool exportToJSON(const std::string& filepath, const std::string& controllerId = "");
    
    // Configuration
    void setSamplingRate(float samplesPerSecond);
    void setRetentionTime(double retentionTime);
    void clearAll();
    
    // Plugin integration
    bool registerMetricsPlugin(std::unique_ptr<plugins::IntegrationPlugin> plugin);
    bool unregisterMetricsPlugin(const std::string& name);
}

// Global metrics instances
extern MetricsCollector& getGlobalMetricsCollector();

// Utility functions for metrics
std::string generateMetricName(const std::string& prefix, const std::string& suffix = "");
std::string formatMetricValue(float value, const std::string& unit = "");
double calculateMean(const std::vector<float>& values);
float calculateStdDev(const std::vector<float>& values, float mean);
std::vector<size_t> findOutliers(const std::vector<float>& values, float threshold = 2.0f);
std::string getTimeString(double timestamp);

} // namespace nlm
