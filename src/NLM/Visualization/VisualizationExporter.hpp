#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <map>
#include <functional>

namespace nlm {

class VisualizationInterface;
class Brain;
class MemorySystem;
class PerformanceMetrics;

class VisualizationExporter {
public:
    VisualizationExporter();
    ~VisualizationExporter();
    
    // Export visualization to file
    bool exportVisualization(const std::string& filename, 
                           const VisualizationInterface& visualizer, 
                           const std::string& format = "auto");
    
    // Export visualization with additional context
    bool exportWithContext(const std::string& filename,
                          const VisualizationInterface& visualizer,
                          const Brain& brain,
                          const MemorySystem& memorySystem,
                          const PerformanceMetrics& metrics,
                          const std::string& format = "auto");
    
    // Export multiple visualizations
    bool exportMultiple(const std::vector<std::string>& filenames,
                       const std::vector<std::unique_ptr<VisualizationInterface>>& visualizers,
                       const std::string& format = "auto");
    
    // Export all visualizations from manager
    bool exportFromManager(const std::string& filenamePrefix,
                          const VisualizationInterface* manager,
                          const std::string& format = "auto");
    
    // Supported export formats
    enum class ExportFormat {
        AUTO,
        JSON,
        XML,
        CSV,
        PNG,
        JPEG,
        BMP,
        SVG,
        OBJ,
        GLTF,
        FBX,
        VTK,
        HDF5,
        DICOM,
        HTML,
        PDF,
        TEXT
    };
    
    // Format detection
    static ExportFormat detectFormat(const std::string& filename);
    static std::string getFormatExtension(ExportFormat format);
    static std::string getFormatDescription(ExportFormat format);
    
    // Configuration management
    void setExportPath(const std::string& path);
    std::string getExportPath() const;
    
    // Quality settings
    void setCompressionLevel(int level);
    int getCompressionLevel() const;
    
    void setQualitySettings(int quality);
    int getQualitySettings() const;
    
    void setExportResolution(int width, int height);
    void getExportResolution(int& width, int& height) const;
    
    // Format-specific options
    void setJSONIndentation(int spaces);
    void setCSVDelimiter(char delimiter);
    void setImageFormat(const std::string& format);
    void setImageQuality(float quality);
    
    // Validation
    bool validateExportOptions(ExportFormat format, const std::string& filename) const;
    std::vector<std::string> getExportErrors() const;
    void clearExportErrors();
    
    // Progress monitoring
    void setProgressCallback(std::function<void(double)> callback);
    void removeProgressCallback();
    double getProgress() const;
    
    // Export history
    void enableExportHistory(bool enable);
    std::vector<std::string> getExportHistory() const;
    void clearExportHistory();
    void saveExportHistory(const std::string& filename) const;
    void loadExportHistory(const std::string& filename);
    
    // Specialized exporters
    bool exportToJSON(const std::string& filename, const VisualizationInterface& visualizer);
    bool exportToXML(const std::string& filename, const VisualizationInterface& visualizer);
    bool exportToCSV(const std::string& filename, const VisualizationInterface& visualizer);
    bool exportToImage(const std::string& filename, const VisualizationInterface& visualizer);
    bool exportTo3DModel(const std::string& filename, const VisualizationInterface& visualizer);
    
    // Batch operations
    bool exportBatch(const std::vector<std::pair<std::string, ExportFormat>>& exports);
    void cancelBatchExport();
    bool isBatchExporting() const;
    
    // Resource management
    size_t getMemoryUsage() const;
    void cleanupTemporaryFiles();
    
    // Version and compatibility
    std::string getExporterVersion() const;
    std::vector<std::string> getSupportedFormats() const;
    std::string getFormatCapabilities(ExportFormat format) const;
    
    // Callbacks for export events
    std::function<void(const std::string&)> onExportStarted;
    std::function<void(const std::string&, bool)> onExportCompleted;
    std::function<void(const std::string&, const std::string&)> onExportError;
    std::function<void(double)> onExportProgress;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm