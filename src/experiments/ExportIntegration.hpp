// ExportIntegration.hpp - Export and Integration System
// Handles multiple export formats, debug system integration, and real-time visualization

#pragma once

/**
 * ExportIntegration - Comprehensive export and integration for NLM performance
 * 
 * This module provides advanced export and integration capabilities for NLM performance data,
 * including:
 * 
 * 1. Multiple Export Formats
 *    - JSON export with customizable structure
 *    - CSV export for spreadsheet compatibility
 *    - YAML export for configuration files
 *    - XML export for interoperability
 *    - Database export for long-term storage
 * 
 * 2. Integration with Existing Debug System
 *    - Integration with NLM's existing debug infrastructure
 *    - Custom debug hooks and instrumentation
 *    - Debug log merging and consolidation
 *    - Debug system compatibility modes
 * 
 * 3. Real-time Visualization Support
 *    - Web-based visualization interfaces
 *    - Real-time data streaming
 *    - Interactive dashboards
 *    - Performance alerting and notifications
 * 
 * 4. Historical Performance Tracking
 *    - Long-term performance database
 *    - Performance trend analysis over time
 *    - Version-controlled performance data
 *    - Performance regression detection
 * 
 * 5. Performance Benchmarking
 *    - Automated performance benchmarking
 *    - Benchmark comparison and reporting
 *    - Performance target tracking
 *    - Performance KPI monitoring
 *    
 * Features:
 * - Format-agnostic export architecture
 * - Pluggable integration points
 * - Real-time and post-mortem modes
 * - Configuration-driven behavior
 * - Error handling and recovery
 * - Performance-optimized for large datasets
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <set>
#include <queue>
#include <list>
#include <regex>
#include <numeric>
#include <filesystem>

namespace nlm {

/**
 * Export format types
 */
enum class ExportFormat {
    NONE = 0,
    JSON = 1,
    CSV = 2,
    YAML = 3,
    XML = 4,
    DATABASE = 5,
    TSDB = 6,  // Time-series database
    CUSTOM = 7
};

/**
 * Integration target types
 */
enum class IntegrationTarget {
    NONE = 0,
    DEBUG_SYSTEM = 1,
    VISUALIZATION_SYSTEM = 2,
    MONITORING_SYSTEM = 3,
    LOGGING_SYSTEM = 4,
    DATABASE = 5,
    API = 6,
    CUSTOM = 7
};

/**
 * Export configuration
 */
struct ExportConfig {
    std::string name;
    bool enabled = true;
    ExportFormat format = ExportFormat::JSON;
    std::string outputPath; // File path or URL
    std::string compression; // none, gzip, lz4, snappy
    bool overwriteExisting = true;
    std::string fileExtension; // Custom file extension
    std::unordered_map<std::string, std::string> formatOptions; // Format-specific options
    
    // Export filtering
    uint64_t minTimestamp = 0;
    uint64_t maxTimestamp = UINT64_MAX;
    std::unordered_set<ProfileEventType> excludeEventTypes;
    
    // Performance
    size_t batchSize = 1000; // Records per batch
    bool streamOutput = false; // Stream output to avoid memory issues
    double maxMemoryUsageMB = 1024.0; // Maximum memory usage in MB
    
    ExportConfig() = default;
};

/**
 * Integration configuration
 */
struct IntegrationConfig {
    std::string name;
    bool enabled = true;
    IntegrationTarget target = IntegrationTarget::NONE;
    std::string endpoint; // API endpoint, database connection, etc.
    std::unordered_map<std::string, std::string> connectionParameters;
    std::string authenticationToken;
    std::chrono::seconds reconnectInterval = std::chrono::seconds(30);
    bool autoReconnect = true;
    bool enableSSL = true;
    
    // Integration behavior
    std::vector<std::string> eventTypesToExport; // Specific event types to export
    bool exportAllEvents = true;
    std::string dataTransformScript; // Custom transformation script
    
    IntegrationConfig() = default;
};

/**
 * Real-time visualization configuration
 */
struct VisualizationConfig {
    std::string name;
    bool enabled = false;
    std::string visualizationType; // "dashboard", "chart", "graph", "table"
    std::string dataSource; // WebSocket endpoint, API endpoint, etc.
    std::string updateIntervalMs; // Update interval in milliseconds
    bool enableInteractive = true;
    bool enableAnnotations = true;
    bool enableFiltering = true;
    
    // Dashboard configuration
    std::unordered_map<std::string, std::string> dashboardWidgets; // Widget configurations
    std::unordered_map<std::string, std::string> dashboardLayout; // Widget layout
    
    // Chart configuration
    std::string chartType; // "line", "bar", "pie", "heatmap"
    std::string chartXAxis; // X-axis data field
    std::string chartYAxis; // Y-axis data field
    std::string chartAggregation; // "sum", "average", "min", "max"
    
    VisualizationConfig() = default;
};

/**
 * Historical performance tracking configuration
 */
struct HistoricalTrackingConfig {
    std::string name;
    bool enabled = false;
    std::string storageBackend; // "database", "filesystem", "tsdb"
    std::string storageConnectionString; // Connection string
    std::string tableName; // Database table name
    size_t retentionDays = 30; // Number of days to retain data
    bool enableCompression = true;
    bool enableIndexing = true;
    std::vector<std::string> indexFields; // Fields to index
    
    // Time-series configuration
    std::string timeSeriesInterval; // "second", "minute", "hour", "day"
    std::string timeSeriesAggregation; // "raw", "average", "sum", "min", "max"
    
    HistoricalTrackingConfig() = default;
};

/**
 * Performance benchmarking configuration
 */
struct BenchmarkingConfig {
    std::string name;
    bool enabled = false;
    std::string benchmarkSuite; // Reference to benchmark suite
    std::string comparisonBaseline; // Baseline to compare against
    std::string performanceTargetsFile; // File containing performance targets
    std::chrono::seconds benchmarkInterval; // How often to run benchmarks
    bool autoBenchmark = true;
    bool compareToHistorical = true;
    bool generateReports = true;
    
    // Performance targets
    struct PerformanceTarget {
        std::string name;
        std::string metricName;
        double targetValue;
        std::string comparison; // "lt", "gt", "eq", "le", "ge"
        std::string unit;
        bool warningEnabled = true;
        bool criticalEnabled = true;
    };
    
    std::vector<PerformanceTarget> performanceTargets;
    
    BenchmarkingConfig() : benchmarkInterval(std::chrono::seconds(3600)) {} // Run once per hour
};

/**
 * Export result
 */
struct ExportResult {
    std::string exportId;
    std::string configName;
    uint64_t startTimestamp;
    uint64_t endTimestamp;
    uint64_t recordsExported;
    uint64_t recordsProcessed;
    uint64_t recordsFailed;
    double exportDuration; // Seconds
    std::string status; // "completed", "failed", "partial"
    std::string errorMessage;
    uint64_t fileSize; // Bytes
    std::string checksum; // File checksum
    
    ExportResult() : startTimestamp(0), endTimestamp(0), recordsExported(0),
                    recordsProcessed(0), recordsFailed(0), exportDuration(0.0),
                    status("pending"), fileSize(0) {}
};

/**
 * Integration result
 */
struct IntegrationResult {
    std::string integrationId;
    std::string targetName;
    uint64_t startTimestamp;
    uint64_t endTimestamp;
    uint64_t recordsSent;
    uint64_t recordsFailed;
    double transmissionDuration; // Seconds
    std::string status; // "completed", "failed", "partial"
    std::string errorMessage;
    
    IntegrationResult() : startTimestamp(0), endTimestamp(0), recordsSent(0),
                         recordsFailed(0), transmissionDuration(0.0),
                         status("pending") {}
};

/**
 * Exporter interface
 */
class Exporter {
public:
    virtual ~Exporter() = default;
    
    virtual std::string getExporterName() const = 0;
    virtual ExportFormat getExportFormat() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                                   const ExportConfig& config) = 0;
    
    virtual bool validateConfig(const ExportConfig& config) = 0;
    virtual std::string getExporterStatus() const = 0;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

/**
 * JSON exporter
 */
class JSONExporter : public Exporter {
private:
    std::string exporterName_;
    bool running_;
    
public:
    JSONExporter(const std::string& name = "JSON Exporter");
    
    std::string getExporterName() const override { return exporterName_; }
    ExportFormat getExportFormat() const override { return ExportFormat::JSON; }
    std::string getVersion() const override { return "1.0.0"; }
    
    ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                           const ExportConfig& config) override;
    
    bool validateConfig(const ExportConfig& config) override;
    std::string getExporterStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * CSV exporter
 */
class CSVExporter : public Exporter {
private:
    std::string exporterName_;
    bool running_;
    
public:
    CSVExporter(const std::string& name = "CSV Exporter");
    
    std::string getExporterName() const override { return exporterName_; }
    ExportFormat getExportFormat() const override { return ExportFormat::CSV; }
    std::string getVersion() const override { return "1.0.0"; }
    
    ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                           const ExportConfig& config) override;
    
    bool validateConfig(const ExportConfig& config) override;
    std::string getExporterStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * YAML exporter
 */
class YAMLEExporter : public Exporter {
private:
    std::string exporterName_;
    bool running_;
    
public:
    YAMLEExporter(const std::string& name = "YAML Exporter");
    
    std::string getExporterName() const override { return exporterName_; }
    ExportFormat getExportFormat() const override { return ExportFormat::YAML; }
    std::string getVersion() const override { return "1.0.0"; }
    
    ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                           const ExportConfig& config) override;
    
    bool validateConfig(const ExportConfig& config) override;
    std::string getExporterStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * XML exporter
 */
class XMLExporter : public Exporter {
private:
    std::string exporterName_;
    bool running_;
    
public:
    XMLExporter(const std::string& name = "XML Exporter");
    
    std::string getExporterName() const override { return exporterName_; }
    ExportFormat getExportFormat() const override { return ExportFormat::XML; }
    std::string getVersion() const override { return "1.0.0"; }
    
    ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                           const ExportConfig& config) override;
    
    bool validateConfig(const ExportConfig& config) override;
    std::string getExporterStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * Database exporter
 */
class DatabaseExporter : public Exporter {
private:
    std::string exporterName_;
    bool running_;
    std::string connectionString_;
    
public:
    DatabaseExporter(const std::string& name = "Database Exporter",
                    const std::string& connectionString = "");
    
    std::string getExporterName() const override { return exporterName_; }
    ExportFormat getExportFormat() const override { return ExportFormat::DATABASE; }
    std::string getVersion() const override { return "1.0.0"; }
    
    ExportResult exportData(const std::vector<ProfileDataPoint>& data,
                           const ExportConfig& config) override;
    
    bool validateConfig(const ExportConfig& config) override;
    std::string getExporterStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * Integration adapter
 */
class IntegrationAdapter {
public:
    virtual ~IntegrationAdapter() = default;
    
    virtual std::string getAdapterName() const = 0;
    virtual IntegrationTarget getIntegrationTarget() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual IntegrationResult integrateData(const std::vector<ProfileDataPoint>& data,
                                           const IntegrationConfig& config) = 0;
    
    virtual bool validateConfig(const IntegrationConfig& config) = 0;
    virtual std::string getIntegrationStatus() const = 0;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

/**
 * Debug system integration adapter
 */
class DebugSystemIntegration : public IntegrationAdapter {
private:
    std::string adapterName_;
    bool running_;
    
public:
    DebugSystemIntegration(const std::string& name = "Debug System Integration");
    
    std::string getAdapterName() const override { return adapterName_; }
    IntegrationTarget getIntegrationTarget() const override { return IntegrationTarget::DEBUG_SYSTEM; }
    std::string getVersion() const override { return "1.0.0"; }
    
    IntegrationResult integrateData(const std::vector<ProfileDataPoint>& data,
                                   const IntegrationConfig& config) override;
    
    bool validateConfig(const IntegrationConfig& config) override;
    std::string getIntegrationStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * Visualization system integration adapter
 */
class VisualizationIntegration : public IntegrationAdapter {
private:
    std::string adapterName_;
    bool running_;
    std::string websocketEndpoint_;
    
public:
    VisualizationIntegration(const std::string& name = "Visualization Integration",
                            const std::string& websocketEndpoint = "ws://localhost:8080");
    
    std::string getAdapterName() const override { return adapterName_; }
    IntegrationTarget getIntegrationTarget() const override { return IntegrationTarget::VISUALIZATION_SYSTEM; }
    std::string getVersion() const override { return "1.0.0"; }
    
    IntegrationResult integrateData(const std::vector<ProfileDataPoint>& data,
                                   const IntegrationConfig& config) override;
    
    bool validateConfig(const IntegrationConfig& config) override;
    std::string getIntegrationStatus() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override { return running_.load(); }
};

/**
 * Exporter factory
 */
class ExporterFactory {
public:
    static std::shared_ptr<Exporter> createExporter(ExportFormat format, const std::string& name = "");
    static std::shared_ptr<Exporter> createExporter(const std::string& exporterType, const std::string& name = "");
    
    static std::vector<std::string> getAvailableExporters();
    static std::vector<ExportFormat> getAvailableExportFormats();
};

/**
 * Integration adapter factory
 */
class IntegrationAdapterFactory {
public:
    static std::shared_ptr<IntegrationAdapter> createIntegrationAdapter(IntegrationTarget target, const std::string& name = "");
    static std::shared_ptr<IntegrationAdapter> createIntegrationAdapter(const std::string& adapterType, const std::string& name = "");
    
    static std::vector<std::string> getAvailableIntegrationAdapters();
    static std::vector<IntegrationTarget> getAvailableIntegrationTargets();
};

/**
 * Export integration manager
 */
class ExportIntegrationManager {
private:
    std::string managerName_;
    bool running_;
    std::vector<std::shared_ptr<Exporter>> exporters_;
    std::vector<std::shared_ptr<IntegrationAdapter>> integrationAdapters_;
    
    std::vector<ExportConfig> exportConfigs_;
    std::vector<IntegrationConfig> integrationConfigs_;
    
    mutable std::mutex mutex_;
    
public:
    ExportIntegrationManager(const std::string& name = "Export Integration Manager");
    ~ExportIntegrationManager();
    
    // Exporter management
    void addExporter(std::shared_ptr<Exporter> exporter);
    bool removeExporter(const std::string& exporterName);
    std::shared_ptr<Exporter> getExporter(const std::string& exporterName);
    std::vector<std::string> getExporterNames() const;
    
    // Integration adapter management
    void addIntegrationAdapter(std::shared_ptr<IntegrationAdapter> adapter);
    bool removeIntegrationAdapter(const std::string& adapterName);
    std::shared_ptr<IntegrationAdapter> getIntegrationAdapter(const std::string& adapterName);
    std::vector<std::string> getIntegrationAdapterNames() const;
    
    // Configuration management
    void addExportConfig(const ExportConfig& config);
    bool removeExportConfig(const std::string& configName);
    std::shared_ptr<ExportConfig> getExportConfig(const std::string& configName);
    
    void addIntegrationConfig(const IntegrationConfig& config);
    bool removeIntegrationConfig(const std::string& configName);
    std::shared_ptr<IntegrationConfig> getIntegrationConfig(const std::string& configName);
    
    // Operation
    ExportResult exportData(const std::vector<ProfileDataPoint>& data, const std::string& configName);
    IntegrationResult integrateData(const std::vector<ProfileDataPoint>& data, const std::string& configName);
    
    // Control
    void start();
    void stop();
    void pause();
    void resume();
    
    bool isRunning() const { return running_.load(); }
    bool isPaused() const { return paused_.load(); }
    
    // Status
    std::string getManagerStatus() const;
    std::string getExporterStatus(const std::string& exporterName) const;
    std::string getIntegrationAdapterStatus(const std::string& adapterName) const;
    
    // Export with filtering
    ExportResult exportFilteredData(const std::vector<ProfileDataPoint>& data,
                                   const ExportConfig& config,
                                   uint64_t startTime = 0, uint64_t endTime = 0);
    
    // Batch export
    std::vector<ExportResult> batchExport(const std::vector<ProfileDataPoint>& data,
                                          const std::vector<std::string>& configNames);
    
    // Real-time streaming
    void startRealTimeExport(const std::string& configName);
    void stopRealTimeExport(const std::string& configName);
    
    // Error handling
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
private:
    std::vector<std::string> errors_;
    std::atomic<bool> paused_{false};
    std::vector<std::thread> exportThreads_;
    std::vector<std::thread> integrationThreads_;
    
    void processExportQueue();
    void processIntegrationQueue();
};

/**
 * Exporter statistics
 */
struct ExporterStatistics {
    std::string exporterName;
    ExportFormat format;
    uint64_t totalExports = 0;
    uint64_t totalRecordsExported = 0;
    uint64_t totalRecordsFailed = 0;
    double averageExportTime = 0.0;
    uint64_t currentMemoryUsage = 0;
    std::string status;
    
    ExporterStatistics() = default;
};

/**
 * Integration statistics
 */
struct IntegrationStatistics {
    std::string adapterName;
    IntegrationTarget target;
    uint64_t totalIntegrations = 0;
    uint64_t totalRecordsSent = 0;
    uint64_t totalRecordsFailed = 0;
    double averageTransmissionTime = 0.0;
    std::string status;
    
    IntegrationStatistics() = default;
};

/**
 * Manager statistics
 */
struct ManagerStatistics {
    uint64_t totalExports = 0;
    uint64_t totalIntegrations = 0;
    double uptime = 0.0;
    std::vector<ExporterStatistics> exporterStats;
    std::vector<IntegrationStatistics> integrationStats;
    
    ManagerStatistics() = default;
};

/**
 * Export integration factory
 */
class ExportIntegrationFactory {
public:
    static std::shared_ptr<ExportIntegrationManager> createManager(const std::string& name = "Export Integration Manager");
    
    static std::vector<std::string> getAvailableManagers();
};

} // namespace nlm