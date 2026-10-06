#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace nlm {

class HistoricalMetricsStore {
public:
    // Storage configuration
    struct StorageConfig {
        std::string directory;           // Directory for storage files
        std::string fileFormat;          // JSON, CSV, binary, or mixed
        size_t maxFileSize;              // Maximum file size in bytes
        size_t maxHistoryDays;            // Maximum history to keep
        bool enableCompression;           // Enable compression
        bool enableArchival;              // Enable archival to slower storage
        std::string archivePath;          // Path for archival storage
        
        StorageConfig()
            : directory("./metrics_history"), fileFormat("json"), 
              maxFileSize(100 * 1024 * 1024), maxHistoryDays(30),  // 100MB, 30 days
              enableCompression(true), enableArchival(true),
              archivePath("./metrics_archive") {}
    };
    
    HistoricalMetricsStore(const StorageConfig& config = StorageConfig());
    ~HistoricalMetricsStore();
    
    // Store metrics data
    bool storeMetrics(const std::vector<MetricData>& metrics);
    
    // Store metric sample
    bool storeSample(const MetricSample& sample);
    
    // Retrieve metrics for time range
    std::vector<MetricData> retrieveMetrics(const std::string& metricName,
                                            std::chrono::system_clock::time_point start,
                                            std::chrono::system_clock::time_point end) const;
    
    // Get all metrics samples
    std::vector<MetricSample> getAllSamples() const;
    
    // Get metrics by category
    std::vector<MetricData> getMetricsByCategory(const std::string& category) const;
    
    // Export metrics to different formats
    std::string exportToJSON(const std::string& metricName = "",
                            std::chrono::system_clock::time_point start = std::chrono::system_clock::time_point::min(),
                            std::chrono::system_clock::time_point end = std::chrono::system_clock::now()) const;
    
    std::string exportToCSV(const std::string& metricName = "",
                            std::chrono::system_clock::time_point start = std::chrono::system_clock::time_point::min(),
                            std::chrono::system_clock::time_point end = std::chrono::system_clock::now()) const;
    
    // Import metrics from different formats
    bool importFromJSON(const std::string& data);
    bool importFromCSV(const std::string& data);
    
    // Clean old data
    void cleanupOldData();
    
    // Get storage statistics
    struct StorageStats {
        size_t totalSamples;
        size_t totalFiles;
        size_t diskUsage;  // bytes
        std::chrono::system_clock::time_point oldestData;
        std::chrono::system_clock::time_point newestData;
        
        StorageStats()
            : totalSamples(0), totalFiles(0), diskUsage(0) {}
    };
    
    StorageStats getStats() const;
    
    // Configure storage
    void configure(const StorageConfig& config) {
        config_ = config;
        ensureDirectoryExists();
    }
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    bool ensureDirectoryExists() const;
    std::string generateFilename(const std::string& prefix = "metrics", 
                                 const std::string& extension = "json") const;
    bool writeToFile(const std::string& filename, const std::string& data) const;
    std::string readFromFile(const std::string& filename) const;
    bool compressFile(const std::string& filename) const;
    bool archiveOldFiles();
    
    StorageConfig config_;
    mutable std::mutex mutex_;
    
    // In-memory cache for frequently accessed data
    mutable std::vector<MetricSample> memoryCache_;
    size_t cacheSize_;
    
    // File management
    std::unordered_map<std::string, std::chrono::system_clock::time_point> fileTimestamps_;
};

} // namespace nlm
