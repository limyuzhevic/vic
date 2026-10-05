// NLM Performance Monitoring and Profiling Framework
// Provides comprehensive performance monitoring, profiling, and analysis capabilities
// Includes real-time monitoring, performance metrics, and optimization insights

#include "performance/Performance.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

namespace nlm {

// Performance sample structure
struct PerformanceSample {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsagePercent;
    size_t memoryUsageKB;
    size_t memoryPeakKB;
    size_t threadCount;
    size_t activeNeuronCount;
    size_t activeSynapseCount;
    double spikeRate;
    double simulationTimeMs;
    std::map<std::string, double> customMetrics;
    
    PerformanceSample() : cpuUsagePercent(0.0), memoryUsageKB(0), memoryPeakKB(0),
                         threadCount(0), activeNeuronCount(0), activeSynapseCount(0),
                         spikeRate(0.0), simulationTimeMs(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Performance profiler for tracking metrics
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    // Start profiling
    void startProfile(const std::string& operationName);
    
    // Stop profiling
    double stopProfile(const std::string& operationName);
    
    // Get current profile
    double getCurrentProfile(const std::string& operationName) const;
    
    // Get all profiles
    std::map<std::string, double> getAllProfiles() const;
    
    // Clear all profiles
    void clearProfiles();
    
    // Get performance statistics
    struct ProfileStats {
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        size_t sampleCount;
        double cpuUsageAverage;
        size_t memoryUsageAverage;
    };
    
    ProfileStats getProfileStats(const std::string& operationName) const;
    
    // Get overall performance statistics
    std::map<std::string, ProfileStats> getAllProfileStats() const;
    
    // Add custom metric
    void addCustomMetric(const std::string& metricName, double value);
    
    // Get custom metrics
    double getCustomMetric(const std::string& metricName) const;
    
    // Get all custom metrics
    std::map<std::string, double> getAllCustomMetrics() const;
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
    // Export performance data
    bool exportPerformanceData(const std::string& filename) const;
    
    // Import performance data
    bool importPerformanceData(const std::string& filename);
    
    // Set sampling interval
    void setSamplingInterval(int intervalMs) { samplingIntervalMs_ = intervalMs; }
    
    // Get sampling interval
    int getSamplingIntervalMs() const { return samplingIntervalMs_; }
    
    // Enable/disable profiling
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Start/stop real-time monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Get current performance sample
    PerformanceSample getCurrentSample() const;
    
    // Add performance sample
    void addSample(const PerformanceSample& sample);
    
    // Get performance history
    std::vector<PerformanceSample> getPerformanceHistory() const { return performanceHistory_; }
    
    // Clear performance history
    void clearPerformanceHistory() { performanceHistory_.clear(); }
    
    // Set performance threshold
    void setThreshold(const std::string& metricName, double threshold);
    
    // Check if metric exceeds threshold
    bool checkThreshold(const std::string& metricName) const;
    
    // Get threshold
    double getThreshold(const std::string& metricName) const;
    
    // Get all thresholds
    std::map<std::string, double> getAllThresholds() const { return thresholds_; }
    
    // Performance alert
    struct PerformanceAlert {
        std::string metricName;
        double currentValue;
        double threshold;
        std::chrono::system_clock::time_point timestamp;
        std::string severity;
        std::string message;
    };
    
    // Get performance alerts
    std::vector<PerformanceAlert> getPerformanceAlerts() const { return alerts_; }
    
    // Clear alerts
    void clearAlerts() { alerts_.clear(); }
    
private:
    struct ProfileData {
        std::chrono::high_resolution_clock::time_point startTime;
        double totalTimeMs;
        size_t sampleCount;
        double cpuUsageSum;
        size_t memoryUsageSum;
        std::vector<double> timeSamples;
    };
    
    std::map<std::string, ProfileData> profiles_;
    std::map<std::string, double> customMetrics_;
    int samplingIntervalMs_ = 1000;
    bool enabled_ = true;
    bool monitoring_ = false;
    std::thread monitoringThread_;
    std::atomic<bool> monitoringStopFlag_ = false;
    
    std::vector<PerformanceSample> performanceHistory_;
    std::map<std::string, double> thresholds_;
    std::vector<PerformanceAlert> alerts_;
    
    // Current sample data
    mutable std::mutex mutex_;
    PerformanceSample currentSample_;
    
    // Helper functions
    double getCurrentCPUUsage() const;
    size_t getCurrentMemoryUsage() const;
    size_t getCurrentMemoryPeak() const;
    size_t getCurrentThreadCount() const;
    size_t getCurrentActiveNeuronCount() const;
    size_t getCurrentActiveSynapseCount() const;
    double getCurrentSpikeRate() const;
    double getCurrentSimulationTime() const;
    
    void updateSample();
    void checkThresholds();
    void generateAlert(const std::string& metricName, double currentValue, double threshold);
};

// System performance monitor
class SystemPerformanceMonitor {
public:
    SystemPerformanceMonitor() = default;
    ~SystemPerformanceMonitor() = default;
    
    // Start monitoring system performance
    void startMonitoring();
    
    // Stop monitoring system performance
    void stopMonitoring();
    
    // Check if monitoring
    bool isMonitoring() const { return monitoring_; }
    
    // Get current system metrics
    struct SystemMetrics {
        double cpuUsagePercent;
        size_t memoryTotalKB;
        size_t memoryUsedKB;
        size_t memoryAvailableKB;
        size_t memoryCachedKB;
        size_t swapTotalKB;
        size_t swapUsedKB;
        size_t threadCount;
        size_t processCount;
        double systemLoadAverage;
        std::chrono::system_clock::time_point timestamp;
    };
    
    SystemMetrics getSystemMetrics() const;
    
    // Get application performance metrics
    struct ApplicationMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t activeSynapseCount;
        double spikeRate;
        double simulationTimeMs;
        size_t processingSteps;
        double efficiencyScore;
        std::chrono::system_clock::time_point timestamp;
    };
    
    ApplicationMetrics getApplicationMetrics() const;
    
    // Get combined metrics
    struct CombinedMetrics {
        SystemMetrics system;
        ApplicationMetrics application;
        std::vector<PerformanceSample> performanceHistory;
        std::map<std::string, double> customMetrics;
    };
    
    CombinedMetrics getCombinedMetrics() const;
    
    // Performance analysis
    struct PerformanceAnalysis {
        double cpuUtilization;
        size_t memoryEfficiency;
        double spikeEfficiency;
        double synapticEfficiency;
        double computationalEfficiency;
        std::vector<std::string> recommendations;
    };
    
    PerformanceAnalysis analyzePerformance() const;
    
    // Performance comparison
    struct PerformanceComparison {
        std::string baselineMetricName;
        double baselineValue;
        double currentValue;
        double improvementPercent;
        std::string trend;
    };
    
    std::vector<PerformanceComparison> compareWithBaseline(const std::string& baselineFile) const;
    
    // Performance optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Performance benchmarking
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTimeMs;
        size_t memoryUsageKB;
        double spikesPerSecond;
        double synapsesPerMs;
        double efficiencyScore;
        std::chrono::system_clock::time_point timestamp;
    };
    
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                const std::shared_ptr<Config>& config) const;
    
    // Performance profiling
    bool profilePerformance(const std::string& profileName,
                           const std::function<void()>& workload,
                           int durationSeconds = 60) const;
    
    // Performance analysis
    bool analyzePerformanceProfile(const std::string& profileFile,
                                  const std::string& outputFile = "") const;
    
private:
    bool monitoring_ = false;
    std::thread monitorThread_;
    std::atomic<bool> monitorStopFlag_ = false;
    
    mutable std::mutex mutex_;
    
    SystemMetrics getSystemMetricsInternal() const;
    ApplicationMetrics getApplicationMetricsInternal() const;
    
    void updateSystemMetrics();
    void updateApplicationMetrics();
    void checkPerformanceThresholds();
    void generatePerformanceAlerts();
};

// Performance analyzer for deep analysis
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    ~PerformanceAnalyzer() = default;
    
    // Analyze performance samples
    struct PerformanceStatistics {
        double mean;
        double median;
        double mode;
        double stddev;
        double variance;
        double min;
        double max;
        double range;
        size_t count;
        std::vector<double> percentiles;
    };
    
    PerformanceStatistics analyzeSamples(const std::vector<PerformanceSample>& samples) const;
    
    // Analyze performance trends
    struct PerformanceTrend {
        std::string metricName;
        std::vector<double> values;
        std::vector<double> timestamps;
        double slope;
        double rSquared;
        std::string trendType;
        std::string prediction;
    };
    
    std::vector<PerformanceTrend> analyzeTrends(const std::vector<PerformanceSample>& samples,
                                               const std::vector<std::string>& metricNames) const;
    
    // Performance correlation analysis
    struct CorrelationResult {
        std::string metric1;
        std::string metric2;
        double correlation;
        std::string strength;
        std::string direction;
        bool statisticallySignificant;
    };
    
    std::vector<CorrelationResult> analyzeCorrelation(const std::vector<PerformanceSample>& samples,
                                                      const std::vector<std::string>& metricPairs) const;
    
    // Performance clustering
    struct ClusterResult {
        std::vector<size_t> sampleIndices;
        double centroid;
        double radius;
        std::string clusterType;
    };
    
    std::vector<ClusterResult> clusterSamples(const std::vector<PerformanceSample>& samples,
                                             size_t clusterCount = 3) const;
    
    // Performance anomaly detection
    struct AnomalyResult {
        size_t sampleIndex;
        std::string metricName;
        double expectedValue;
        double actualValue;
        double deviation;
        double zScore;
        std::string anomalyType;
        double confidence;
    };
    
    std::vector<AnomalyResult> detectAnomalies(const std::vector<PerformanceSample>& samples,
                                               double threshold = 2.0) const;
    
    // Performance forecasting
    struct ForecastResult {
        std::string metricName;
        std::vector<double> predictedValues;
        std::vector<double> confidenceIntervals;
        std::string forecastType;
        int forecastHorizon;
    };
    
    std::vector<ForecastResult> forecastPerformance(const std::vector<PerformanceSample>& samples,
                                                     const std::vector<std::string>& metricNames,
                                                     int horizon = 10) const;
    
    // Performance optimization
    struct OptimizationRecommendation {
        std::string recommendation;
        std::string category;
        double potentialImprovement;
        double implementationCost;
        std::string priority;
        std::string expectedImpact;
    };
    
    std::vector<OptimizationRecommendation> generateOptimizationRecommendations(
        const std::vector<PerformanceSample>& samples) const;
    
    // Performance validation
    struct ValidationResult {
        bool passed;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double validationScore;
    };
    
    ValidationResult validatePerformance(const std::vector<PerformanceSample>& samples,
                                         const std::map<std::string, double>& targets) const;
    
    // Performance report generation
    std::string generatePerformanceReport(const std::vector<PerformanceSample>& samples,
                                          const std::map<std::string, double>& targets = {}) const;
    
    std::string generateDetailedPerformanceReport(
        const std::vector<PerformanceSample>& samples,
        const std::map<std::string, double>& targets = {}) const;
    
    // Performance visualization
    std::string generatePerformanceChart(const std::vector<PerformanceSample>& samples,
                                         const std::string& chartType = "line") const;
    
    // Performance comparison
    std::string comparePerformance(const std::vector<PerformanceSample>& baseline,
                                  const std::vector<PerformanceSample>& current) const;
    
    // Performance export
    bool exportPerformanceAnalysis(const std::string& filename,
                                  const std::vector<PerformanceSample>& samples,
                                  const std::map<std::string, double>& targets = {}) const;
    
    // Performance import
    bool importPerformanceAnalysis(std::vector<PerformanceSample>& samples,
                                  const std::string& filename) const;
    
    // Performance statistics
    struct PerformanceStatistics {
        size_t totalSamples;
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        double cpuUsageAverage;
        double memoryUsageAverage;
        double spikeRateAverage;
        double efficiencyScore;
    };
    
    PerformanceStatistics calculateStatistics(const std::vector<PerformanceSample>& samples) const;
    
    // Performance comparison
    struct PerformanceComparison {
        std::string metricName;
        double baselineValue;
        double currentValue;
        double difference;
        double percentChange;
        std::string trend;
    };
    
    std::vector<PerformanceComparison> compareMetrics(
        const std::vector<PerformanceSample>& baseline,
        const std::vector<PerformanceSample>& current) const;
};

// Global performance profiler instance
inline PerformanceProfiler performanceProfiler;

// Global system performance monitor instance
inline SystemPerformanceMonitor systemPerformanceMonitor;

// Global performance analyzer instance
inline PerformanceAnalyzer performanceAnalyzer;

// Convenience functions
inline void startPerformanceProfiling(const std::string& operationName) {
    performanceProfiler.startProfile(operationName);
}

inline double stopPerformanceProfiling(const std::string& operationName) {
    return performanceProfiler.stopProfile(operationName);
}

inline void addPerformanceMetric(const std::string& metricName, double value) {
    performanceProfiler.addCustomMetric(metricName, value);
}

inline double getPerformanceMetric(const std::string& metricName) {
    return performanceProfiler.getCustomMetric(metricName);
}

inline std::string generatePerformanceReport() {
    return performanceProfiler.generatePerformanceReport();
}

inline bool exportPerformanceData(const std::string& filename) {
    return performanceProfiler.exportPerformanceData(filename);
}

inline void startSystemMonitoring() {
    systemPerformanceMonitor.startMonitoring();
}

inline void stopSystemMonitoring() {
    systemPerformanceMonitor.stopMonitoring();
}

inline SystemPerformanceMonitor::SystemMetrics getSystemMetrics() {
    return systemPerformanceMonitor.getSystemMetrics();
}

inline SystemPerformanceMonitor::ApplicationMetrics getApplicationMetrics() {
    return systemPerformanceMonitor.getApplicationMetrics();
}

inline SystemPerformanceMonitor::CombinedMetrics getCombinedMetrics() {
    return systemPerformanceMonitor.getCombinedMetrics();
}

inline SystemPerformanceMonitor::PerformanceAnalysis analyzePerformance() {
    return systemPerformanceMonitor.analyzePerformance();
}

inline SystemPerformanceMonitor::BenchmarkResult runBenchmark(const std::string& name,
                                                             const std::shared_ptr<Config>& config) {
    return systemPerformanceMonitor.runBenchmark(name, config);
}

inline bool profilePerformance(const std::string& name, const std::function<void()>& workload,
                               int duration) {
    return systemPerformanceMonitor.profilePerformance(name, workload, duration);
}

} // namespace nlm

// Implementation details
namespace nlm {
    
    // PerformanceProfiler implementation
    namespace PerformanceProfiler {
        void PerformanceProfiler::startProfile(const std::string& operationName) {
            if (!enabled_) return;
            
            std::lock_guard<std::mutex> lock(mutex_);
            
            if (profiles_.find(operationName) == profiles_.end()) {
                ProfileData data;
                data.startTime = std::chrono::high_resolution_clock::now();
                profiles_[operationName] = data;
            }
        }
        
        double PerformanceProfiler::stopProfile(const std::string& operationName) {
            if (!enabled_ || profiles_.find(operationName) == profiles_.end()) {
                return 0.0;
            }
            
            std::lock_guard<std::mutex> lock(mutex_);
            
            auto& data = profiles_[operationName];
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - data.startTime);
            
            data.totalTimeMs += duration.count();
            data.sampleCount++;
            data.timeSamples.push_back(duration.count());
            
            return duration.count();
        }
        
        double PerformanceProfiler::getCurrentProfile(const std::string& operationName) const {
            if (!enabled_ || profiles_.find(operationName) == profiles_.end()) {
                return 0.0;
            }
            
            std::lock_guard<std::mutex> lock(mutex_);
            return profiles_[operationName].totalTimeMs;
        }
        
        std::map<std::string, double> PerformanceProfiler::getAllProfiles() const {
            std::lock_guard<std::mutex> lock(mutex_);
            std::map<std::string, double> result;
            for (const auto& pair : profiles_) {
                result[pair.first] = pair.second.totalTimeMs;
            }
            return result;
        }
        
        void PerformanceProfiler::clearProfiles() {
            std::lock_guard<std::mutex> lock(mutex_);
            profiles_.clear();
        }
        
        ProfileStats PerformanceProfiler::getProfileStats(const std::string& operationName) const {
            ProfileStats stats;
            
            if (!enabled_ || profiles_.find(operationName) == profiles_.end()) {
                return stats;
            }
            
            std::lock_guard<std::mutex> lock(mutex_);
            const auto& data = profiles_[operationName];
            
            if (data.timeSamples.empty()) {
                return stats;
            }
            
            stats.totalTimeMs = data.totalTimeMs;
            stats.sampleCount = data.sampleCount;
            
            if (data.sampleCount > 0) {
                stats.averageTimeMs = data.totalTimeMs / data.sampleCount;
            }
            
            if (!data.timeSamples.empty()) {
                stats.minTimeMs = *std::min_element(data.timeSamples.begin(), data.timeSamples.end());
                stats.maxTimeMs = *std::max_element(data.timeSamples.begin(), data.timeSamples.end());
            }
            
            return stats;
        }
        
        std::map<std::string, ProfileStats> PerformanceProfiler::getAllProfileStats() const {
            std::lock_guard<std::mutex> lock(mutex_);
            std::map<std::string, ProfileStats> result;
            for (const auto& pair : profiles_) {
                result[pair.first] = getProfileStats(pair.first);
            }
            return result;
        }
        
        void PerformanceProfiler::addCustomMetric(const std::string& metricName, double value) {
            std::lock_guard<std::mutex> lock(mutex_);
            customMetrics_[metricName] = value;
        }
        
        double PerformanceProfiler::getCustomMetric(const std::string& metricName) const {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = customMetrics_.find(metricName);
            if (it != customMetrics_.end()) {
                return it->second;
            }
            return 0.0;
        }
        
        std::map<std::string, double> PerformanceProfiler::getAllCustomMetrics() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return customMetrics_;
        }
        
        std::string PerformanceProfiler::generatePerformanceReport() const {
            std::stringstream ss;
            
            ss << "NLM Performance Report" << std::endl;
            ss << "=======================" << std::endl;
            ss << std::endl;
            
            ss << "Enabled: " << (enabled_ ? "Yes" : "No") << std::endl;
            ss << "Sampling Interval: " << samplingIntervalMs_ << "ms" << std::endl;
            ss << "Monitoring: " << (monitoring_ ? "Yes" : "No") << std::endl;
            ss << std::endl;
            
            ss << "Profile Statistics:" << std::endl;
            auto profileStats = getAllProfileStats();
            for (const auto& pair : profileStats) {
                const auto& stats = pair.second;
                ss << "  " << pair.first << ":" << std::endl;
                ss << "    Total Time: " << stats.totalTimeMs << "ms" << std::endl;
                ss << "    Average Time: " << stats.averageTimeMs << "ms" << std::endl;
                ss << "    Samples: " << stats.sampleCount << std::endl;
            }
            
            ss << std::endl;
            
            ss << "Custom Metrics:" << std::endl;
            auto customMetrics = getAllCustomMetrics();
            for (const auto& pair : customMetrics) {
                ss << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            ss << std::endl;
            
            ss << "Thresholds:" << std::endl;
            for (const auto& pair : thresholds_) {
                ss << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            ss << std::endl;
            
            ss << "Alerts:" << std::endl;
            for (const auto& alert : alerts_) {
                ss << "  " << alert.metricName << ": " << alert.currentValue << 
                   " (threshold: " << alert.threshold << ") - " << alert.message << std::endl;
            }
            
            return ss.str();
        }
        
        bool PerformanceProfiler::exportPerformanceData(const std::string& filename) const {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open performance data file: " + filename);
                return false;
            }
            
            auto profiles = getAllProfiles();
            auto customMetrics = getAllCustomMetrics();
            
            file << "NLM Performance Data" << std::endl;
            file << "=====================" << std::endl;
            file << std::endl;
            
            file << "Profiles:" << std::endl;
            for (const auto& pair : profiles) {
                file << "  " << pair.first << ": " << pair.second << "ms" << std::endl;
            }
            
            file << std::endl;
            
            file << "Custom Metrics:" << std::endl;
            for (const auto& pair : customMetrics) {
                file << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            file << std::endl;
            
            file << "Thresholds:" << std::endl;
            for (const auto& pair : thresholds_) {
                file << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            file.close();
            NLM_LOG_INFO("Performance data exported to: " + filename);
            return true;
        }
        
        bool PerformanceProfiler::importPerformanceData(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open performance data file: " + filename);
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("Profiles:") != std::string::npos) {
                    // Parse profiles (simplified)
                    while (std::getline(file, line) && !line.empty()) {
                        if (line.find("  ") == 0 && line.find(":") != std::string::npos) {
                            size_t colonPos = line.find(":");
                            std::string name = line.substr(2, colonPos - 2);
                            std::string valueStr = line.substr(colonPos + 1);
                            valueStr = valueStr.substr(0, valueStr.find("ms"));
                            try {
                                double value = std::stod(valueStr);
                                // Add to profiles
                                startProfile(name);
                                stopProfile(name);
                            } catch (...) {
                                // Ignore parsing errors
                            }
                        }
                    }
                } else if (line.find("Custom Metrics:") != std::string::npos) {
                    // Parse custom metrics
                    while (std::getline(file, line) && !line.empty()) {
                        if (line.find("  ") == 0 && line.find(":") != std::string::npos) {
                            size_t colonPos = line.find(":");
                            std::string name = line.substr(2, colonPos - 2);
                            std::string valueStr = line.substr(colonPos + 1);
                            try {
                                double value = std::stod(valueStr);
                                customMetrics_[name] = value;
                            } catch (...) {
                                // Ignore parsing errors
                            }
                        }
                    }
                }
            }
            
            file.close();
            NLM_LOG_INFO("Performance data imported from: " + filename);
            return true;
        }
        
        void PerformanceProfiler::startMonitoring() {
            if (monitoringStopFlag_) {
                return;
            }
            
            monitoring_ = true;
            monitoringStopFlag_ = false;
            
            monitoringThread_ = std::thread([this]() {
                while (!monitoringStopFlag_) {
                    updateSample();
                    checkThresholds();
                    
                    std::this_thread::sleep_for(std::chrono::milliseconds(samplingIntervalMs_));
                }
            });
        }
        
        void PerformanceProfiler::stopMonitoring() {
            monitoringStopFlag_ = true;
            
            if (monitoringThread_.joinable()) {
                monitoringThread_.join();
            }
            
            monitoring_ = false;
        }
        
        PerformanceSample PerformanceProfiler::getCurrentSample() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return currentSample_;
        }
        
        void PerformanceProfiler::addSample(const PerformanceSample& sample) {
            std::lock_guard<std::mutex> lock(mutex_);
            performanceHistory_.push_back(sample);
            
            if (performanceHistory_.size() > 1000) {
                performanceHistory_.erase(performanceHistory_.begin());
            }
        }
        
        void PerformanceProfiler::setThreshold(const std::string& metricName, double threshold) {
            std::lock_guard<std::mutex> lock(mutex_);
            thresholds_[metricName] = threshold;
        }
        
        bool PerformanceProfiler::checkThreshold(const std::string& metricName) const {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = thresholds_.find(metricName);
            if (it == thresholds_.end()) {
                return false;
            }
            
            double currentValue = 0.0;
            if (metricName == "cpu_usage") {
                currentValue = currentSample_.cpuUsagePercent;
            } else if (metricName == "memory_usage") {
                currentValue = currentSample_.memoryUsageKB;
            } else if (metricName == "spike_rate") {
                currentValue = currentSample_.spikeRate;
            } else if (metricName == "simulation_time") {
                currentValue = currentSample_.simulationTimeMs;
            }
            
            bool exceedsThreshold = currentValue > it->second;
            
            if (exceedsThreshold) {
                generateAlert(metricName, currentValue, it->second);
            }
            
            return exceedsThreshold;
        }
        
        double PerformanceProfiler::getThreshold(const std::string& metricName) const {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = thresholds_.find(metricName);
            if (it != thresholds_.end()) {
                return it->second;
            }
            return 0.0;
        }
        
        void PerformanceProfiler::generateAlert(const std::string& metricName, double currentValue,
                                                double threshold) {
            PerformanceAlert alert;
            alert.metricName = metricName;
            alert.currentValue = currentValue;
            alert.threshold = threshold;
            alert.timestamp = std::chrono::system_clock::now();
            alert.severity = currentValue > threshold * 2.0 ? "HIGH" : 
                           currentValue > threshold * 1.5 ? "MEDIUM" : "LOW";
            alert.message = metricName + " is above threshold: " + 
                           std::to_string(currentValue) + " > " + std::to_string(threshold);
            
            alerts_.push_back(alert);
            
            if (alerts_.size() > 100) {
                alerts_.erase(alerts_.begin());
            }
        }
        
        // Implementation of helper functions would continue...
        
    } // namespace PerformanceProfiler
    
    // Additional implementations would continue for SystemPerformanceMonitor and PerformanceAnalyzer
    
} // namespace nlm
