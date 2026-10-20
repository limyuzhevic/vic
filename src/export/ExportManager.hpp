#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <memory>
#include <filesystem>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Export configuration
struct ExportConfig {
    std::string outputDirectory = "./exports";
    bool autoCreateDirectory = true;
    bool overwriteExisting = false;
    std::chrono::milliseconds exportInterval = std::chrono::milliseconds(60000); // 1 minute
    bool enableCompression = false;
    bool enableEncryption = false;
    std::string encryptionKey;
    std::vector<std::string> preferredFormats = {"csv", "json", "png"};
    bool enableProgressTracking = true;
    std::function<void(double)> progressCallback;
};

// Export task information
struct ExportTask {
    std::string name;
    std::string source;
    std::vector<std::string> dataSources;
    std::string format;
    std::string filename;
    std::chrono::high_resolution_clock::time_point scheduledTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double progress = 0.0;
    bool completed = false;
    std::string errorMessage;
    size_t estimatedSize = 0;
    
    ExportTask(const std::string& taskName, const std::string& src, const std::string& fmt, 
               const std::string& filename = "")
        : name(taskName), source(src), format(fmt), filename(filename), 
          scheduledTime(std::chrono::high_resolution_clock::now()), 
          progress(0.0), completed(false) {}
};

// Export format information
struct ExportFormatInfo {
    std::string name;
    std::string extension;
    std::string mimeType;
    bool supportsCompression;
    bool supportsEncryption;
    bool supportsMetadata;
    double averageSizeFactor; // Compression ratio
    std::string description;
};

// Export result
struct ExportResult {
    bool success;
    std::string message;
    std::string outputPath;
    std::chrono::milliseconds duration;
    size_t bytesWritten;
    ExportTask task;
    
    ExportResult(bool s, const std::string& msg, const std::string& path = "", 
                 const ExportTask& t = ExportTask("", "", ""))
        : success(s), message(msg), outputPath(path), duration(0), bytesWritten(0), task(t) {}
};

// Export filters and transformations
struct ExportFilter {
    std::string name;
    std::function<bool(const ExportTask&)> condition;
    std::function<void(ExportTask&)> action;
    bool enabled = true;
};

// Export manager for data file output
class ExportManager {
public:
    ExportManager();
    ~ExportManager();
    
    // Disable copying
    ExportManager(const ExportManager&) = delete;
    ExportManager& operator=(const ExportManager&) = delete;
    
    // Initialize export manager
    bool initialize(const ExportConfig& config = ExportConfig());
    
    // Start/stop export manager
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Configure export
    void configure(const ExportConfig& config);
    ExportConfig getConfig() const { return config_; }
    
    // Export data
    ExportResult exportMetrics(const std::string& name, const std::string& format,
                               const std::string& filename = "");
    ExportResult exportVisualization(const std::string& name, const std::string& format,
                                      const std::string& filename = "");
    ExportResult exportData(const std::string& name, const std::string& format,
                            const std::vector<std::string>& dataSources = {});
    
    // Batch export
    std::vector<ExportResult> exportBatch(const std::vector<std::string>& taskNames);
    ExportResult exportAll(const std::string& prefix = "export");
    
    // Export formats
    bool exportToCSV(const std::string& filename, const std::string& name = "metrics");
    bool exportToJSON(const std::string& filename, const std::string& name = "metrics");
    bool exportToXML(const std::string& filename, const std::string& name = "metrics");
    bool exportToSQLite(const std::string& filename, const std::string& name = "metrics");
    
    // Visualization exports
    bool exportToPNG(const std::string& filename, const std::string& view = "activity");
    bool exportToJPG(const std::string& filename, const std::string& view = "activity");
    bool exportToSVG(const std::string& filename, const std::string& view = "activity");
    bool exportToPDF(const std::string& filename, const std::string& view = "activity");
    bool exportToHTML(const std::string& filename, const std::string& view = "activity");
    
    // History exports
    bool exportHistory(const std::string& filename, const std::string& format,
                      const std::chrono::milliseconds& age = std::chrono::milliseconds(0));
    bool exportMetricsHistory(const std::string& filename, const std::string& format,
                             const std::chrono::milliseconds& age = std::chrono::milliseconds(0));
    
    // Configuration management
    void setOutputDirectory(const std::string& directory);
    std::string getOutputDirectory() const;
    
    // Format management
    void registerFormat(const ExportFormatInfo& formatInfo);
    ExportFormatInfo getFormatInfo(const std::string& format) const;
    std::vector<std::string> getAvailableFormats() const;
    
    // Filter management
    void addFilter(const ExportFilter& filter);
    void removeFilter(const std::string& name);
    void clearFilters();
    
    // Status and monitoring
    std::vector<ExportTask> getActiveTasks() const;
    ExportTask getTask(const std::string& name) const;
    size_t getCompletedTasksCount() const;
    double getCompletionRate() const;
    
    // Export control
    void cancelExport(const std::string& taskName);
    void pauseExport(const std::string& taskName);
    void resumeExport(const std::string& taskName);
    
    // History management
    void clearHistory();
    void setMaxHistorySize(size_t maxSize);
    
    // Configuration getters/setters
    void enableCompression(bool enabled) { config_.enableCompression = enabled; }
    bool isCompressionEnabled() const { return config_.enableCompression; }
    
    void enableEncryption(bool enabled, const std::string& key = "") {
        config_.enableEncryption = enabled;
        config_.encryptionKey = key;
    }
    bool isEncryptionEnabled() const { return config_.enableEncryption; }
    
    void enableProgressTracking(bool enabled) { config_.enableProgressTracking = enabled; }
    bool isProgressTrackingEnabled() const { return config_.enableProgressTracking; }
    
    // Get singleton instance
    static ExportManager& getInstance() {
        static ExportManager instance;
        return instance;
    }
    
    // Global export functions
    static ExportResult exportGlobal(const std::string& name, const std::string& format,
                                     const std::string& filename = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::shared_mutex mutex_;
    std::atomic<bool> running_;
    ExportConfig config_;
    std::string outputDirectory_;
    
    // Internal helper methods
    std::string generateFilename(const std::string& name, const std::string& format) const;
    bool ensureOutputDirectory() const;
    bool filterTask(ExportTask& task) const;
    void updateTaskProgress(ExportTask& task, double progress) const;
    void executeExportTask(ExportTask& task);
    
    // Export implementation details
    bool exportToCSVInternal(const std::string& filename, const std::string& name);
    bool exportToJSONInternal(const std::string& filename, const std::string& name);
    bool exportToXMLInternal(const std::string& filename, const std::string& name);
    bool exportToSQLiteInternal(const std::string& filename, const std::string& name);
    
    bool exportToPNGInternal(const std::string& filename, const std::string& view);
    bool exportToJPGInternal(const std::string& filename, const std::string& view);
    bool exportToSVGInternal(const std::string& filename, const std::string& view);
    bool exportToPDFInternal(const std::string& filename, const std::string& view);
    bool exportToHTMLInternal(const std::string& filename, const std::string& view);
    
    // Compression and encryption helpers
    std::string compressData(const std::string& data) const;
    std::string encryptData(const std::string& data) const;
    std::string decryptData(const std::string& data) const;
    
    // File management
    void cleanOldExports(const std::chrono::milliseconds& age) const;
    size_t getDirectorySize(const std::string& directory) const;
    std::vector<std::string> getFilesInDirectory(const std::string& directory) const;
};

} // namespace nlm
