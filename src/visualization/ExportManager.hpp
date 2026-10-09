// Export manager for visualization output export
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

struct ExportOptions {
    ExportFormat format;
    std::string outputPath;
    std::string filename;
    std::string title;
    std::string description;
    std::map<std::string, std::string> metadata;
    bool includeTimestamps;
    bool includeMetadata;
    bool compress;
    std::string compressionAlgorithm;
    double quality; // For image formats
    int resolutionWidth;
    int resolutionHeight;
    std::string colorSpace;
    bool includeAnnotations;
    std::vector<std::string> annotations;
    std::chrono::steady_clock::time_point exportTime;
};

struct ExportReport {
    std::string exportType;
    std::string outputPath;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double duration;
    bool success;
    std::string errorMessage;
    size_t bytesWritten;
    std::vector<std::string> generatedFiles;
    std::map<std::string, std::string> statistics;
    std::vector<std::string> warnings;
    std::vector<std::string> messages;
    bool requiresValidation;
};

struct StaticImageExport {
    std::string filename;
    std::vector<std::vector<Color>> pixelData;
    int width;
    int height;
    std::string format;
    double quality;
    std::string colorSpace;
    std::chrono::steady_clock::time_point timestamp;
};

struct InteractiveWebExport {
    std::string filename;
    std::string htmlContent;
    std::vector<std::string> cssFiles;
    std::vector<std::string> jsFiles;
    std::map<std::string, std::string> assets;
    std::string title;
    std::string description;
    std::chrono::steady_clock::time_point timestamp;
};

struct VideoExport {
    std::string filename;
    std::vector<StaticImageExport> frames;
    double frameRate;
    int duration;
    std::string codec;
    int bitrate;
    std::string resolution;
    std::chrono::steady_clock::time_point timestamp;
};

struct ThreeDExport {
    std::string filename;
    std::vector<std::vector<std::vector<Color>>> voxelData;
    int width;
    int height;
    int depth;
    std::string format;
    std::chrono::steady_clock::time_point timestamp;
};

struct ConsoleOutputExport {
    std::string title;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<std::string> outputLines;
    std::map<std::string, std::vector<float>> metrics;
    std::vector<std::string> annotations;
};

class ExportManager {
public:
    ExportManager();
    ~ExportManager();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Export methods
    ExportReport exportNetworkVisualization(const ExportOptions& options) const;
    ExportReport exportActivityAnalysis(const ExportOptions& options) const;
    ExportReport exportBrainState(const ExportOptions& options) const;
    ExportReport exportPerformanceAnalysis(const ExportOptions& options) const;
    ExportReport exportMemoryAnalysis(const ExportOptions& options) const;
    ExportReport exportDevelopmentAnalysis(const ExportOptions& options) const;
    
    // Specific export methods
    ExportReport exportToPNG(const ExportOptions& options) const;
    ExportReport exportToSVG(const ExportOptions& options) const;
    ExportReport exportToPDF(const ExportOptions& options) const;
    ExportReport exportToHTML(const ExportOptions& options) const;
    ExportReport exportToJSON(const ExportOptions& options) const;
    ExportReport exportToCSV(const ExportOptions& options) const;
    ExportReport exportToText(const ExportOptions& options) const;
    ExportReport exportTo3D(const ExportOptions& options) const;
    ExportReport exportToVideo(const ExportOptions& options) const;
    
    // Console output
    ExportReport exportToConsole(const ExportOptions& options) const;
    
    // Interactive web
    ExportReport exportInteractiveWeb(const ExportOptions& options) const;
    
    // Video recording
    ExportReport startVideoRecording(const ExportOptions& options);
    ExportReport stopVideoRecording(const ExportOptions& options);
    bool isRecording() const;
    
    // Batch export
    std::vector<ExportReport> exportBatch(const std::vector<ExportOptions>& options) const;
    
    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setDefaultQuality(double quality);
    void setCompressionEnabled(bool enable);
    void setIncludeMetadata(bool include);
    void setIncludeTimestamps(bool include);
    void setMaxExportSize(size_t maxSize);
    
    // Configuration getters
    std::string getOutputDirectory() const;
    double getDefaultQuality() const;
    bool isCompressionEnabled() const;
    bool isMetadataIncluded() const;
    bool areTimestampsIncluded() const;
    size_t getMaxExportSize() const;
    
    // Validation
    bool validateExportOptions(const ExportOptions& options) const;
    std::vector<std::string> getExportWarnings(const ExportOptions& options) const;
    std::vector<std::string> getExportErrors(const ExportOptions& options) const;
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback);
    void setExportStartedCallback(std::function<void(const ExportOptions&)> callback);
    void setExportCompletedCallback(std::function<void(const ExportReport&)> callback);
    void setExportErrorCallback(std::function<void(const std::string&)> callback);
    
    // State management
    bool isExporting() const;
    std::vector<ExportReport> getExportHistory() const;
    void clearExportHistory();
    
    // Statistics
    size_t getTotalExports() const;
    size_t getSuccessfulExports() const;
    size_t getFailedExports() const;
    double getSuccessRate() const;
    
    // Visualization data preparation
    void prepareVisualizationDataForExport(ExportFormat format, std::vector<float>& exportData) const;
    void prepareNetworkDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& networkData) const;
    void prepareActivityDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& activityData) const;
    void prepareBrainStateDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& brainStateData) const;
    void preparePerformanceDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& performanceData) const;
    void prepareMemoryDataForExport(const VisualizationData& data, ExportFormat format, std::vector<float>& memoryData) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    std::string outputDirectory_;
    double defaultQuality_;
    bool compressionEnabled_;
    bool metadataIncluded_;
    bool timestampsIncluded_;
    size_t maxExportSize_;
    
    // State
    bool isExporting_;
    std::vector<ExportReport> exportHistory_;
    
    // Statistics
    size_t totalExports_;
    size_t successfulExports_;
    size_t failedExports_;
    
    // Callbacks
    std::function<void(double)> progressCallback_;
    std::function<void(const ExportOptions&)> exportStartedCallback_;
    std::function<void(const ExportReport&)> exportCompletedCallback_;
    std::function<void(const std::string&)> exportErrorCallback_;
    
    // Video recording
    bool isRecording_;
    std::vector<StaticImageExport> videoFrames_;
    ExportOptions currentVideoOptions_;
    
    // Export methods
    ExportReport performExport(const ExportOptions& options, const std::function<void(const ExportOptions&)>& preExport) const;
    ExportReport createStaticImageExport(const ExportOptions& options) const;
    ExportReport createInteractiveWebExport(const ExportOptions& options) const;
    ExportReport createVideoExport(const ExportOptions& options) const;
    ExportReport create3DExport(const ExportOptions& options) const;
    ExportReport createJSONExport(const ExportOptions& options) const;
    ExportReport createCSVExport(const ExportOptions& options) const;
    ExportReport createTextExport(const ExportOptions& options) const;
    ExportReport createConsoleExport(const ExportOptions& options) const;
    
    // Format-specific exports
    ExportReport exportAsPNG(const ExportOptions& options) const;
    ExportReport exportAsSVG(const ExportOptions& options) const;
    ExportReport exportAsPDF(const ExportOptions& options) const;
    ExportReport exportAsHTML(const ExportOptions& options) const;
    ExportReport exportAsJSON(const ExportOptions& options) const;
    ExportReport exportAsCSV(const ExportOptions& options) const;
    ExportReport exportAsText(const ExportOptions& options) const;
    ExportReport exportAs3D(const ExportOptions& options) const;
    
    // Helper methods
    void createDirectoryStructure(const std::string& path) const;
    std::string generateFilename(const ExportOptions& options) const;
    std::string generateExportId() const;
    void updateExportStatistics(ExportReport& report) const;
    
    // Validation helpers
    bool validateFileSize(const ExportOptions& options) const;
    bool validateOutputPath(const ExportOptions& options) const;
    
    // Video recording helpers
    void startRecording(const ExportOptions& options);
    void stopRecording();
    void addFrameToVideo(const StaticImageExport& frame);
    
    // Data preparation helpers
    void prepareNetworkData(const VisualizationData& data, ExportFormat format, std::vector<float>& networkData) const;
    void prepareActivityData(const VisualizationData& data, ExportFormat format, std::vector<float>& activityData) const;
    void prepareBrainStateData(const VisualizationData& data, ExportFormat format, std::vector<float>& brainStateData) const;
    void preparePerformanceData(const VisualizationData& data, ExportFormat format, std::vector<float>& performanceData) const;
    void prepareMemoryData(const VisualizationData& data, ExportFormat format, std::vector<float>& memoryData) const;
    
    // Format conversion helpers
    void convertToPNG(const std::vector<float>& sourceData, int width, int height, const std::string& outputPath) const;
    void convertToSVG(const std::vector<float>& sourceData, int width, int height, const std::string& outputPath) const;
    void convertToPDF(const std::vector<float>& sourceData, int width, int height, const std::string& outputPath) const;
    void convertToHTML(const std::vector<float>& sourceData, const std::string& outputPath) const;
    
    // File I/O helpers
    void writeDataToFile(const std::string& filepath, const std::string& data) const;
    void writeDataToFile(const std::string& filepath, const std::vector<float>& data) const;
    void writeDataToFile(const std::string& filepath, const std::vector<std::vector<Color>>& data) const;
    
    // Statistics helpers
    void updateSuccessStatistics(ExportReport& report) const;
    void updateFailureStatistics(ExportReport& report) const;
    
    // Callback helpers
    void invokeProgressCallback(double progress) const;
    void invokeExportStartedCallback(const ExportOptions& options) const;
    void invokeExportCompletedCallback(const ExportReport& report) const;
    void invokeExportErrorCallback(const std::string& error) const;
    
    mutable std::mutex mutex_;
};
