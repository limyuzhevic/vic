#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <deque>
#include <atomic>
#include <mutex>
#include <functional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <optional>

namespace nlm {

// Metric types for comprehensive profiling
enum class MetricType {
    Counter,           // Incrementing counter
    Gauge,             // Current value
    Histogram,         // Distribution of values
    Timer,             // Time duration measurements
    Rate,              // Events per unit time
    Percentage,        // Percentage value
    String,            // String label/value
    Boolean,           // True/false flag
    Custom            // Custom metric type
};

// Individual metric sample
struct MetricSample {
    std::string name;
    MetricType type;
    std::chrono::steady_clock::time_point timestamp;
    std::variant<size_t, double, std::string, bool> value;
    std::unordered_map<std::string, std::string> tags;  // Key-value tags for categorization
    size_t sampleId{0};
    
    // Helper methods for value access
    size_t getAsSizeT() const { return std::get<size_t>(value); }
    double getAsDouble() const { return std::get<double>(value); }
    const std::string& getAsString() const { return std::get<std::string>(value); }
    bool getAsBool() const { return std::get<bool>(value); }
    
    // Check if value is within valid range
    bool isValid() const {
        if (std::holds_alternative<size_t>(value)) return true;
        if (std::holds_alternative<double>(value)) return !std::isnan(std::get<double>(value));
        if (std::holds_alternative<std::string>(value)) return !std::get<std::string>(value).empty();
        if (std::holds_alternative<bool>(value)) return true;
        return false;
    }
};

// Metric aggregation parameters
struct MetricAggregation {
    std::string name;
    MetricType type;
    std::vector<MetricSample> samples;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    // Aggregation results
    double sum{0.0};
    double mean{0.0};
    double variance{0.0};
    double stdDev{0.0};
    double min{std::numeric_limits<double>::max()};
    double max{std::numeric_limits<double>::lowest()};
    size_t count{0};
    size_t zeroCount{0};
    
    // Histogram-specific aggregation
    std::vector<size_t> histogramBuckets;
    size_t bucketCount{10};
    double bucketMin{0.0};
    double bucketMax{1.0};
    
    // Rate calculation
    double ratePerSecond{0.0};
    size_t eventsLastSecond{0};
    
    // Percentile calculations
    std::vector<double> percentiles;
    
    // Reset aggregation
    void reset() {
        samples.clear();
        sum = 0.0;
        mean = 0.0;
        variance = 0.0;
        stdDev = 0.0;
        min = std::numeric_limits<double>::max();
        max = std::numeric_limits<double>::lowest();
        count = 0;
        zeroCount = 0;
        histogramBuckets.clear();
        ratePerSecond = 0.0;
        eventsLastSecond = 0;
        percentiles.clear();
    }
    
    // Update aggregation with new sample
    void update(const MetricSample& sample) {
        if (!sample.isValid()) return;
        
        samples.push_back(sample);
        count++;
        
        double value = 0.0;
        if (std::holds_alternative<size_t>(sample.value)) {
            value = static_cast<double>(std::get<size_t>(sample.value));
        } else if (std::holds_alternative<double>(sample.value)) {
            value = std::get<double>(sample.value);
        }
        
        if (value == 0.0) {
            zeroCount++;
        }
        
        sum += value;
        mean = sum / count;
        
        if (count > 1) {
            double oldMean = mean - (value / count);
            variance += (value - oldMean) * (value - mean);
            stdDev = std::sqrt(variance / (count - 1));
        }
        
        if (value < min) min = value;
        if (value > max) max = value;
        
        // Update histogram
        if (type == MetricType::Histogram) {
            size_t bucketIndex = calculateBucketIndex(value);
            if (bucketIndex < histogramBuckets.size()) {
                histogramBuckets[bucketIndex]++;
            }
        }
    }
    
    // Calculate bucket index for histogram
    size_t calculateBucketIndex(double value) const {
        if (bucketCount == 0) return 0;
        
        if (value <= bucketMin) return 0;
        if (value >= bucketMax) return bucketCount - 1;
        
        double bucketSize = (bucketMax - bucketMin) / bucketCount;
        size_t index = static_cast<size_t>((value - bucketMin) / bucketSize);
        
        if (index >= bucketCount) index = bucketCount - 1;
        return index;
    }
    
    // Calculate percentiles
    std::vector<double> calculatePercentiles(const std::vector<double>& values, const std::vector<double>& percentiles) const {
        if (values.empty()) return {};
        
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        
        std::vector<double> result;
        for (double p : percentiles) {
            double index = (p / 100.0) * (sorted.size() - 1);
            size_t lower = static_cast<size_t>(std::floor(index));
            size_t upper = static_cast<size_t>(std::ceil(index));
            
            if (lower == upper) {
                result.push_back(sorted[lower]);
            } else {
                double weight = index - lower;
                double percentileValue = sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
                result.push_back(percentileValue);
            }
        }
        
        return result;
    }
    
    // Calculate rate
    void calculateRate(size_t currentCount, double timeWindow) {
        if (timeWindow > 0) {
            ratePerSecond = static_cast<double>(currentCount - eventsLastSecond) / timeWindow;
            eventsLastSecond = currentCount;
        }
    }
    
    // Generate report for metric
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== Metric Report ===" << std::endl;
        oss << "Name: " << name << std::endl;
        oss << "Type: " << metricTypeToString(type) << std::endl;
        oss << "Samples: " << samples.size() << std::endl;
        oss << "Time Range: " << getFormattedTime(startTime) << " to " << getFormattedTime(endTime) << std::endl;
        oss << std::endl;
        
        if (type == MetricType::Histogram) {
            oss << "Histogram Buckets:" << std::endl;
            for (size_t i = 0; i < histogramBuckets.size(); ++i) {
                double bucketMin = this->bucketMin + (i * (bucketMax - bucketMin) / bucketCount);
                double bucketMax = bucketMin + (bucketMax - bucketMin) / bucketCount;
                oss << "  [" << bucketMin << ", " << bucketMax << "): " << histogramBuckets[i] << std::endl;
            }
        } else {
            oss << "Statistics:" << std::endl;
            oss << "  Count: " << count << std::endl;
            oss << "  Mean: " << mean << std::endl;
            oss << "  Standard Deviation: " << stdDev << std::endl;
            oss << "  Min: " << min << std::endl;
            oss << "  Max: " << max << std::endl;
            oss << "  Zero Count: " << zeroCount << std::endl;
            if (!std::isnan(ratePerSecond)) {
                oss << "  Rate: " << ratePerSecond << " per second" << std::endl;
            }
        }
        
        return oss.str();
    }
    
private:
    std::string metricTypeToString(MetricType type) const {
        switch (type) {
            case MetricType::Counter: return "Counter";
            case MetricType::Gauge: return "Gauge";
            case MetricType::Histogram: return "Histogram";
            case MetricType::Timer: return "Timer";
            case MetricType::Rate: return "Rate";
            case MetricType::Percentage: return "Percentage";
            case MetricType::String: return "String";
            case MetricType::Boolean: return "Boolean";
            case MetricType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    std::string getFormattedTime(const std::chrono::steady_clock::time_point& timestamp) const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
        return std::to_string(elapsed) + "s";
    }
};

// Metric collection configuration
struct MetricsCollectorConfig {
    bool enabled{true};                          // Enable metrics collection
    std::chrono::milliseconds collectionInterval{1000};  // Collection interval (ms)
    size_t maxMetricHistory{10000};               // Maximum metric history size
    bool enableAggregation{true};                 // Enable metric aggregation
    bool enableAlerting{true};                    // Enable metric alerting
    std::string outputDirectory{"."};             // Output directory for metrics
    std::string format{"json"};                   // Output format (json, csv, text)
    bool continuous{false};                      // Continuous metrics collection
    std::unordered_map<std::string, std::string> defaultTags;  // Default tags for all metrics
    std::chrono::steady_clock::time_point startTime;       // Collection start time
    
    // Alert thresholds
    std::unordered_map<std::string, double> alertThresholds;
    std::unordered_map<std::string, std::string> alertMessages;
    
    // Percentile settings
    std::vector<double> defaultPercentiles{50.0, 95.0, 99.0};
    
    // Histogram settings
    std::unordered_map<std::string, std::pair<double, double>> histogramRanges;
    
    // Validation
    bool validate() const {
        if (collectionInterval.count() < 10) return false;
        if (maxMetricHistory == 0) return false;
        return true;
    }
};

// Metrics alert
struct MetricsAlert {
    std::string metricName;
    double currentValue{0.0};
    double threshold{0.0};
    std::string message;
    std::string severity;  // INFO, WARNING, ERROR
    std::chrono::steady_clock::time_point timestamp;
    bool acknowledged{false};
    std::unordered_map<std::string, std::string> tags;
};

// Metrics collector interface
class IMetricsCollector {
public:
    virtual ~IMetricsCollector() = default;
    
    // Configuration
    virtual bool configure(const MetricsCollectorConfig& config) = 0;
    virtual MetricsCollectorConfig getConfig() const = 0;
    
    // Collection control
    virtual bool startCollecting() = 0;
    virtual bool stopCollecting() = 0;
    virtual bool pauseCollecting() = 0;
    virtual bool resumeCollecting() = 0;
    virtual bool isCollecting() const = 0;
    virtual bool isPaused() const = 0;
    
    // Metric recording
    virtual void recordMetric(const MetricSample& sample) = 0;
    virtual void recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value) = 0;
    virtual void recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value, 
                             const std::unordered_map<std::string, std::string>& tags) = 0;
    
    // Metric aggregation
    virtual std::vector<MetricAggregation> getAggregations(const std::string& name, const std::chrono::steady_clock::time_point& startTime,
                                                           const std::chrono::steady_clock::time_point& endTime) = 0;
    virtual MetricAggregation getCurrentAggregation(const std::string& name) = 0;
    
    // Metrics access
    virtual std::vector<MetricSample> getMetricHistory(const std::string& name, size_t limit = 0) = 0;
    virtual std::vector<std::string> getMetricNames() const = 0;
    
    // Alerting
    virtual std::vector<MetricsAlert> getAlerts() = 0;
    virtual void acknowledgeAlert(size_t index) = 0;
    virtual void clearAlerts() = 0;
    
    // Reporting
    virtual std::string generateReport(const std::string& name = "") = 0;
    virtual bool exportMetrics(const std::string& filename, const std::string& format = "text", 
                              const std::string& name = "") = 0;
    
    // Analysis
    virtual std::unordered_map<std::string, double> getAnalysis(const std::string& name, const std::string& analysisType = "statistics") = 0;
    virtual std::vector<std::string> getTrendAnalysis(const std::string& name, size_t windowSize = 10) = 0;
    
    // Clear data
    virtual void clearMetrics(const std::string& name = "") = 0;
    virtual void reset() = 0;
    
    // Special metric types
    virtual void recordTimer(const std::string& name, std::chrono::duration<double> duration, 
                            const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordCounter(const std::string& name, size_t count = 1, 
                              const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordGauge(const std::string& name, double value, 
                            const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordHistogram(const std::string& name, double value,
                                const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    
    // Performance monitoring
    virtual size_t getTotalMetricCount() const = 0;
    virtual double getCollectionEfficiency() const = 0;
    virtual std::chrono::steady_clock::time_point getLastCollectionTime() const = 0;
    
    // Health check
    virtual bool isHealthy() const = 0;
    virtual std::string getHealthStatus() const = 0;
};

// Main metrics collector implementation
class MetricsCollector : public IMetricsCollector {
public:
    MetricsCollector();
    ~MetricsCollector() override;
    
    // Disable copying, enable moving
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;
    MetricsCollector(MetricsCollector&&) noexcept;
    MetricsCollector& operator=(MetricsCollector&&) noexcept;
    
    // Configuration
    bool configure(const MetricsCollectorConfig& config) override;
    MetricsCollectorConfig getConfig() const override { return config; }
    
    // Collection control
    bool startCollecting() override;
    bool stopCollecting() override;
    bool pauseCollecting() override;
    bool resumeCollecting() override;
    bool isCollecting() const override { return collecting; }
    bool isPaused() const override { return paused; }
    
    // Metric recording
    void recordMetric(const MetricSample& sample) override;
    void recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value) override;
    void recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value,
                     const std::unordered_map<std::string, std::string>& tags) override;
    
    // Metric aggregation
    std::vector<MetricAggregation> getAggregations(const std::string& name, const std::chrono::steady_clock::time_point& startTime,
                                                   const std::chrono::steady_clock::time_point& endTime) override;
    MetricAggregation getCurrentAggregation(const std::string& name) override;
    
    // Metrics access
    std::vector<MetricSample> getMetricHistory(const std::string& name, size_t limit = 0) override;
    std::vector<std::string> getMetricNames() const override;
    
    // Alerting
    std::vector<MetricsAlert> getAlerts() override;
    void acknowledgeAlert(size_t index) override;
    void clearAlerts() override;
    
    // Reporting
    std::string generateReport(const std::string& name = "") override;
    bool exportMetrics(const std::string& filename, const std::string& format = "text",
                      const std::string& name = "") override;
    
    // Analysis
    std::unordered_map<std::string, double> getAnalysis(const std::string& name, const std::string& analysisType = "statistics") override;
    std::vector<std::string> getTrendAnalysis(const std::string& name, size_t windowSize = 10) override;
    
    // Clear data
    void clearMetrics(const std::string& name = "") override;
    void reset() override;
    
    // Special metric types
    void recordTimer(const std::string& name, std::chrono::duration<double> duration,
                     const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordCounter(const std::string& name, size_t count = 1,
                       const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordGauge(const std::string& name, double value,
                     const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordHistogram(const std::string& name, double value,
                         const std::unordered_map<std::string, std::string>& tags = {}) override;
    
    // Performance monitoring
    size_t getTotalMetricCount() const override;
    double getCollectionEfficiency() const override;
    std::chrono::steady_clock::time_point getLastCollectionTime() const override { return lastCollectionTime; }
    
    // Health check
    bool isHealthy() const override;
    std::string getHealthStatus() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MetricsCollectorConfig config;
    std::atomic<bool> collecting;
    std::atomic<bool> paused;
    std::chrono::steady_clock::time_point lastCollectionTime;
    
    // Collection thread
    std::thread collectionThread;
    std::mutex metricsMutex;
    std::mutex aggregationsMutex;
    std::mutex alertsMutex;
    
    // Data storage
    std::unordered_map<std::string, std::vector<MetricSample>> metricHistory;
    std::unordered_map<std::string, MetricAggregation> currentAggregations;
    std::vector<MetricsAlert> alerts;
    
    // Collection worker
    void collectionWorker();
    
    // Data management
    void cleanupOldMetrics();
    void cleanupOldAggregations();
    void checkForAlerts();
    
    // Metric categorization
    std::string getMetricCategory(const std::string& name) const;
    bool shouldAlert(const std::string& name, double value) const;
    std::string getAlertMessage(const std::string& name, double value, double threshold) const;
    
    // File I/O
    bool writeToFile(const std::string& filename, const std::string& content) const;
    bool ensureOutputDirectory(const std::string& filename) const;
    
    // Reporting
    std::string generateTextReport(const std::string& name = "") const;
    std::string generateJSONReport(const std::string& name = "") const;
    std::string generateCSVReport(const std::string& name = "") const;
    std::string generateHTMLReport(const std::string& name = "") const;
    
    // Analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateVariance(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values) const;
    std::vector<double> calculatePercentiles(const std::vector<double>& values, const std::vector<double>& percentiles) const;
    
    // Special metric helpers
    void updateAggregation(const std::string& name, const MetricSample& sample);
    void createHistogramAggregation(const std::string& name, double bucketMin, double bucketMax, size_t bucketCount);
    
    // Performance calculation
    double calculateCollectionEfficiency(size_t totalRecords, std::chrono::steady_clock::time_point startTime) const;
};