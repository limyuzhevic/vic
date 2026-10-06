#pragma once

#include "MetricRegistry.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <condition_variable>

namespace nlm {

class HistoricalMetricsStore;
class AlertSystem;
class PerformanceAnalyzer;

// Metrics collection configuration
struct MetricsCollectionConfig {
    double collectionInterval;  // seconds
    double storageInterval;     // seconds
    size_t maxBufferSize;       // Maximum metrics in memory buffer
    bool enableRealTimeAlerts;   // Enable real-time alerting
    bool enableHistoricalStorage; // Enable historical storage
    std::string outputFormat;    // JSON, CSV, binary
    std::string storagePath;     // Path for historical storage
    
    MetricsCollectionConfig()
        : collectionInterval(0.1), storageInterval(1.0), maxBufferSize(10000),
          enableRealTimeAlerts(true), enableHistoricalStorage(true),
          outputFormat("json"), storagePath("./metrics_history") {}
};

// Thread-safe metrics buffer
struct MetricSample {
    std::chrono::system_clock::time_point timestamp;
    std::string metricName;
    double value;
    std::unordered_map<std::string, std::string> tags;
    
    MetricSample(const std::string& name, double value, 
                 const std::unordered_map<std::string, std::string>& tags = {})
        : metricName(name), value(value), tags(tags) {
        timestamp = std::chrono::system_clock::now();
    }
};

class RealTimeMetricsCollector {
public:
    RealTimeMetricsCollector(std::shared_ptr<MetricRegistry> registry,
                            const MetricsCollectionConfig& config = MetricsCollectionConfig());
    ~RealTimeMetricsCollector();
    
    // Start continuous metrics collection
    bool start();
    
    // Stop metrics collection
    void stop();
    
    // Check if collection is running
    bool isRunning() const { return running_.load(); }
    
    // Set the historical metrics store
    void setHistoricalStore(std::shared_ptr<HistoricalMetricsStore> store) {
        historicalStore_ = store;
    }
    
    // Set the alert system
    void setAlertSystem(std::shared_ptr<AlertSystem> alertSystem) {
        alertSystem_ = alertSystem;
    }
    
    // Set the performance analyzer
    void setPerformanceAnalyzer(std::shared_ptr<PerformanceAnalyzer> analyzer) {
        performanceAnalyzer_ = analyzer;
    }
    
    // Manually trigger metrics collection
    void collectMetrics();
    
    // Get current metrics as MetricData
    std::vector<MetricData> getCurrentMetrics() const;
    
    // Get recent metric samples
    std::vector<MetricSample> getRecentSamples(size_t count = 100) const;
    
    // Configure collection parameters
    void setCollectionInterval(double interval) { config_.collectionInterval = interval; }
    void setStorageInterval(double interval) { config_.storageInterval = interval; }
    void setMaxBufferSize(size_t size) { config_.maxBufferSize = size; }
    
    // Add custom metric collector
    bool addCustomMetric(const std::string& name,
                        const std::string& category,
                        const std::string& description,
                        MetricCollector collector,
                        const std::unordered_map<std::string, std::string>& tags = {});
    
    // Remove metric
    bool removeMetric(const std::string& name);
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Get registry reference
    std::shared_ptr<MetricRegistry> getRegistry() const { return registry_; }
    
    // Export metrics
    std::string exportMetrics(const std::string& format = "auto") const;
    
    // Import metrics
    bool importMetrics(const std::string& data, const std::string& format = "auto");
    
    // Get collection statistics
    struct CollectionStats {
        size_t metricsCollected;
        size_t bufferSize;
        size_t samplesStored;
        double collectionRate;
        std::chrono::system_clock::time_point lastCollectionTime;
        
        CollectionStats()
            : metricsCollected(0), bufferSize(0), samplesStored(0), collectionRate(0.0) {}
    };
    
    CollectionStats getStats() const;
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    // Collection worker thread function
    void collectionWorker();
    
    // Storage worker thread function
    void storageWorker();
    
    // Alert checking worker thread function
    void alertWorker();
    
    // Add sample to buffer
    void addSample(const MetricSample& sample);
    
    // Check for anomalies and trigger alerts
    void checkForAlerts(const std::vector<MetricData>& metrics);
    
    // Compress old samples if buffer is full
    void compressIfNeeded();
    
    // Calculate collection rate
    double calculateCollectionRate() const;
    
    std::shared_ptr<MetricRegistry> registry_;
    MetricsCollectionConfig config_;
    
    // Integration components
    std::shared_ptr<HistoricalMetricsStore> historicalStore_;
    std::shared_ptr<AlertSystem> alertSystem_;
    std::shared_ptr<PerformanceAnalyzer> performanceAnalyzer_;
    
    // Threading
    std::atomic<bool> running_;
    std::thread collectionThread_;
    std::thread storageThread_;
    std::thread alertThread_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    
    // Metrics buffer
    std::deque<MetricSample> metricsBuffer_;
    
    // Statistics
    CollectionStats stats_;
    mutable std::mutex statsMutex_;
    
    // Timing
    std::chrono::steady_clock::time_point lastCollectionTime_;
    size_t totalCollections_;
};

} // namespace nlm
