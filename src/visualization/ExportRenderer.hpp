#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <fstream>
#include <chrono>

namespace nlm {

class ExportRenderer {
public:
    enum class ExportFormat {
        ASCII,
        CSV,
        JSON,
        HTML,
        PDF,
        XML,
        TXT,
        PNG,
        SVG
    };
    
    enum class ExportScope {
        CurrentView,
        FullSession,
        TimeRange,
        SelectedData,
        MemoryOnly,
        ActivityOnly,
        StructureOnly
    };
    
    struct ExportOptions {
        ExportFormat format;
        ExportScope scope;
        std::string outputPath;
        bool includeMetadata;
        bool compress;
        std::string title;
        std::string description;
        std::string author;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> customFields;
        
        ExportOptions() : format(ExportFormat::ASCII), scope(ExportScope::CurrentView),
                         includeMetadata(true), compress(false), title("NLM Visualization Export") {}
    };
    
    ExportRenderer();
    ~ExportRenderer() = default;
    
    // Export functionality
    bool exportData(const ExportOptions& options, const std::string& sessionId = "");
    bool exportBrainStructure(const ExportOptions& options, const Brain& brain);
    bool exportActivityData(const ExportOptions& options, const std::vector<ActivityData>& activityData);
    bool exportMemoryData(const ExportOptions& options, const std::vector<MemoryEntry>& memoryData);
    bool exportProgressData(const ExportOptions& options, const std::vector<ProgressData>& progressData);
    
    // Format-specific export methods
    std::string exportToASCII(const std::vector<ActivityData>& activityData) const;
    std::string exportToCSV(const std::vector<ActivityData>& activityData) const;
    std::string exportToJSON(const std::vector<ActivityData>& activityData) const;
    std::string exportToHTML(const std::vector<ActivityData>& activityData) const;
    std::string exportToXML(const std::vector<ActivityData>& activityData) const;
    std::string exportToTXT(const std::vector<ActivityData>& activityData) const;
    
    // File operations
    bool writeToFile(const std::string& content, const std::string& filePath) const;
    bool createDirectory(const std::string& path) const;
    std::string generateFilename(const ExportOptions& options) const;
    std::string getFileExtension(ExportFormat format) const;
    
    // Data collection for export
    std::vector<ActivityData> collectActivityData() const;
    std::vector<MemoryEntry> collectMemoryData() const;
    std::vector<ProgressData> collectProgressData() const;
    std::vector<ConnectionInfo> collectConnectionData() const;
    
    // Configuration and options
    void setDefaultOptions(const ExportOptions& options) { defaultOptions_ = options; }
    void setOutputDirectory(const std::string& directory) { outputDirectory_ = directory; }
    void setCompressionLevel(int level) { compressionLevel_ = level; }
    void enableExportFormat(ExportFormat format, bool enable) {
        enabledFormats_[format] = enable;
    }
    
    // Validation and validation
    bool validateExportOptions(const ExportOptions& options) const;
    bool checkFileWritePermissions(const std::string& filePath) const;
    bool validateDataConsistency() const;
    
    // Statistics and reporting
    void generateExportReport(const ExportOptions& options) const;
    std::string getExportStatistics() const;
    double getExportSize(const ExportOptions& options) const;
    
    // Session management
    void setSessionId(const std::string& sessionId) { sessionId_ = sessionId; }
    std::string getSessionId() const { return sessionId_; }
    void clearSessionData() { sessionId_.clear(); }
    
    // Batch export
    std::vector<std::string> batchExport(const std::vector<ExportOptions>& optionsList);
    bool exportWithProgress(const ExportOptions& options, std::function<void(double)> progressCallback);
    
    // Theme and formatting
    void setExportTheme(const std::string& theme) { exportTheme_ = theme; }
    void setTimeFormat(const std::string& format) { timeFormat_ = format; }
    void setDateFormat(const std::string& format) { dateFormat_ = format; }
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    bool hasError() const { return !lastError_.empty(); }
    void clearError() { lastError_.clear(); }
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback) { progressCallback_ = callback; }
    void reportProgress(double progress) { if (progressCallback_) progressCallback_(progress); }
    
private:
    ExportOptions defaultOptions_;
    std::string outputDirectory_;
    std::string sessionId_;
    int compressionLevel_;
    std::string exportTheme_;
    std::string timeFormat_;
    std::string dateFormat_;
    
    std::map<ExportFormat, bool> enabledFormats_;
    std::function<void(double)> progressCallback_;
    mutable std::string lastError_;
    
    // Export data structures
    struct ActivityData {
        std::string neuronId;
        float activity;
        Timestamp timestamp;
        SimulationStep step;
        std::string regionId;
        std::string populationId;
        
        ActivityData() : activity(0.0f), timestamp(0.0f), step(0) {}
        ActivityData(const std::string& id, float act, Timestamp time, SimulationStep s,
                    const std::string& rid = "", const std::string& pid = "")
            : neuronId(id), activity(act), timestamp(time), step(s), regionId(rid), populationId(pid) {}
    };
    
    struct ConnectionInfo {
        std::string sourceId;
        std::string targetId;
        float weight;
        float delay;
        std::string type;
        Timestamp timestamp;
        
        ConnectionInfo() : weight(0.0f), delay(0.0f) {}
    };
    
    void initializeDefaultOptions();
    void initializeEnabledFormats();
    std::string formatTimestamp(Timestamp timestamp) const;
    std::string formatDate(std::chrono::system_clock::time_point point) const;
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSV(const std::string& text) const;
    std::string compressData(const std::string& data) const;
    double calculateEstimatedSize(const ExportOptions& options) const;
    std::string generateMetadata(const ExportOptions& options) const;
    std::vector<std::string> getTemplateFiles(ExportFormat format) const;
    std::string applyTemplate(const std::string& content, const std::map<std::string, std::string>& variables) const;
    bool validateData(const std::vector<ActivityData>& activityData) const;
    bool validateData(const std::vector<MemoryEntry>& memoryData) const;
    bool validateData(const std::vector<ProgressData>& progressData) const;
};

} // namespace nlm
