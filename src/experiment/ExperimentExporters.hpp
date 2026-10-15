#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Export format
enum class ExportFormat {
    JSON,
    CSV,
    XML,
    YAML,
    Parquet,
    ORC,
    Avro,
    protobuf,
    Text,
    HTML,
    Markdown,
    SQLite,
    Database,
    Custom
};

// Export result
struct ExportResult {
    std::string exportId;
    std::string experimentName;
    bool success;
    std::string outputPath;
    std::chrono::system_clock::time_point timestamp;
    std::string errorMessage;
    std::map<std::string, std::string> exportMetadata;
    
    ExportResult() : success(false), timestamp(std::chrono::system_clock::now()) {}
    ExportResult(const std::string& exportId, const std::string& experimentName, bool success)
        : exportId(exportId), experimentName(experimentName), success(success),
          timestamp(std::chrono::system_clock::now()) {}
};

// Exporter base
class Exporter {
public:
    virtual ~Exporter() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual ExportFormat getFormat() const = 0;
    virtual ExportResult exportData(const std::string& experimentName,
                                const std::map<std::string, std::vector<double>>& data,
                                const std::string& outputPath) = 0;
    virtual std::string getExporterFormat() const = 0;
};

class ExperimentExporters {
public:
    ExperimentExporters();
    ~ExperimentExporters();
    
    // Disable copying, enable moving
    ExperimentExporters(const ExperimentExporters&) = delete;
    ExperimentExporters& operator=(const ExperimentExporters&) = delete;
    ExperimentExporters(ExperimentExporters&&) noexcept;
    ExperimentExporters& operator=(ExperimentExporters&&) noexcept;
    
    // Exporter management
    bool registerExporter(const std::string& name, std::shared_ptr<Exporter> exporter);
    bool unregisterExporter(const std::string& name);
    bool updateExporter(const std::string& name, std::shared_ptr<Exporter> exporter);
    std::shared_ptr<Exporter> getExporter(const std::string& name) const;
    std::vector<std::string> getAllExporterNames() const;
    
    // Exporter lifecycle
    bool initializeExporter(const std::string& name, const Config& config);
    bool shutdownExporter(const std::string& name);
    bool updateExporter(const std::string& name, double timestep);
    bool enableExporter(const std::string& name);
    bool disableExporter(const std::string& name);
    
    // Export execution
    ExportResult exportExperimentData(const std::string& exporterName, const std::string& experimentName);
    std::vector<ExportResult> exportMultipleExperiments(const std::vector<std::string>& experimentNames);
    std::string exportDataToFormat(const std::string& exporterName, const std::string& experimentName,
                                const ExportFormat& format, const std::string& outputPath);
    
    // Exporter status
    bool isExporterEnabled(const std::string& name) const;
    bool isExporterInitialized(const std::string& name) const;
    std::chrono::system_clock::time_point getExporterLastUpdate(const std::string& name) const;
    std::string getExporterStatus(const std::string& name) const;
    
    // Exporter analysis
    std::map<std::string, double> getExporterPerformance(const std::string& name) const;
    std::vector<std::string> getExporterDependencies(const std::string& name) const;
    std::vector<std::string> getExportersByFormat(ExportFormat format) const;
    
    // Exporter backup and recovery
    bool backupExporterData(const std::string& name, const std::string& backupPath) const;
    bool restoreExporterData(const std::string& name, const std::string& backupPath);
    
    // Exporter cleanup
    bool cleanupExporterData(const std::string& name);
    bool cleanupAllExporterData();
    
    // Exporter system management
    bool enableExporterMonitoring(const std::string& name, bool enable);
    bool isExporterMonitoringEnabled(const std::string& name) const;
    bool enableRealTimeExporterUpdates(bool enable);
    bool isRealTimeExporterUpdatesEnabled() const;
    
    // Exporter utilities
    static std::string generateExporterName(const std::string& prefix = "exporter") const;
    static bool isValidExporterName(const std::string& name) const;
    static ExportFormat stringToExportFormat(const std::string& formatStr) const;
    static std::string exportFormatToString(ExportFormat format) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm