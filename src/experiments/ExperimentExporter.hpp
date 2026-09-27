#pragma once

#include "ExperimentEnhanced.hpp"
#include "ConfigurationManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace nlm {

// Export result status
enum class ExportStatus {
    SUCCESS,
    FAILED,
    CANCELLED
};

// Export options for customizing output format
struct ExportOptions {
    std::string filename;
    ExportFormat format;
    bool includeMetadata;
    bool includeStepData;
    bool includeMetrics;
    bool prettyPrint;
    std::string delimiter;
    std::string timestampFormat;
    std::map<std::string, std::string> customFields;
    
    ExportOptions() : format(ExportFormat::CSV), includeMetadata(true), 
                     includeStepData(true), includeMetrics(true), 
                     prettyPrint(false), delimiter(","), 
                     timestampFormat("%Y-%m-%d %H:%M:%S") {}
};

// Experiment exporter base class
class ExperimentExporter {
public:
    virtual ~ExperimentExporter() = default;
    
    virtual ExportStatus export(const Experiment& experiment, 
                               const std::string& filepath, 
                               const ExportOptions& options) = 0;
    
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
    
    // Utility methods
    static std::string getTimestamp(const std::string& format = "%Y-%m-%d %H:%M:%S");
    static std::string escapeCSV(const std::string& value);
    static std::string escapeJSON(const std::string& value);
    static std::string escapeYAML(const std::string& value);
};

// CSV Exporter
class CSVExporter : public ExperimentExporter {
public:
    ExportStatus export(const Experiment& experiment, 
                       const std::string& filepath, 
                       const ExportOptions& options) override;
    
    std::string getFormatName() const override {
        return "CSV (Comma-Separated Values)";
    }
    
    std::vector<std::string> getSupportedExtensions() const override {
        return {".csv", ".txt"};
    }
};

// JSON Exporter
class JSONExporter : public ExperimentExporter {
public:
    ExportStatus export(const Experiment& experiment, 
                       const std::string& filepath, 
                       const ExportOptions& options) override;
    
    std::string getFormatName() const override {
        return "JSON (JavaScript Object Notation)";
    }
    
    std::vector<std::string> getSupportedExtensions() const override {
        return {".json"};
    }
};

// YAML Exporter
class YAMLEXporter : public ExperimentExporter {
public:
    ExportStatus export(const Experiment& experiment, 
                       const std::string& filepath, 
                       const ExportOptions& options) override;
    
    std::string getFormatName() const override {
        return "YAML (YAML Ain't Markup Language)";
    }
    
    std::vector<std::string> getSupportedExtensions() const override {
        return {".yaml", ".yml"};
    }
};

// Experiment Result Exporter - Main interface for exporting experiment results
class ExperimentResultExporter {
public:
    ExperimentResultExporter();
    ~ExperimentResultExporter();
    
    // Register exporter
    void registerExporter(std::unique_ptr<ExperimentExporter> exporter);
    
    // Export experiment results
    ExportStatus export(const Experiment& experiment,
                       const std::string& filepath,
                       ExportFormat format,
                       const ExportOptions& options = ExportOptions());
    
    // Get available formats
    std::vector<ExportFormat> getAvailableFormats() const;
    
    // Get exporter info
    std::string getExporterName(ExportFormat format) const;
    std::vector<std::string> getExporterExtensions(ExportFormat format) const;
    
    // Batch export
    std::vector<ExportStatus> exportMultiple(const std::vector<std::unique_ptr<Experiment>>& experiments,
                                            const std::string& outputDirectory,
                                            ExportFormat format,
                                            const ExportOptions& options = ExportOptions());
    
    // Format conversion
    ExportStatus convertFormat(const std::string& inputFile,
                              const std::string& outputFile,
                              ExportFormat targetFormat,
                              const ExportOptions& options = ExportOptions());
    
    // Validation
    bool validateExportOptions(ExportFormat format, const ExportOptions& options) const;
    
    // Progress tracking
    using ExportProgressCallback = std::function<void(double, const std::string&)>;
    void setProgressCallback(ExportProgressCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global experiment result exporter instance
ExperimentResultExporter& getExperimentResultExporter();

// Export utilities
namespace ExportUtils {
    // Data transformation utilities
    std::string transformExperimentToCSV(const Experiment& experiment, const ExportOptions& options);
    std::string transformExperimentToJSON(const Experiment& experiment, const ExportOptions& options);
    std::string transformExperimentToYAML(const Experiment& experiment, const ExportOptions& options);
    
    // File utilities
    bool writeToFile(const std::string& filepath, const std::string& content);
    std::string readFromFile(const std::string& filepath);
    
    // Formatting utilities
    std::string formatValue(const ExperimentParameter& value, ExportFormat format);
    std::string formatTimestamp(const std::string& timestamp, const std::string& format);
    
    // Validation utilities
    bool isValidFilename(const std::string& filename);
    std::string generateTimestampedFilename(const std::string& baseName, ExportFormat format);
}

} // namespace nlm
