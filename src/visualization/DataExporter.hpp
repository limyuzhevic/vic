#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

/**
 * Data export system for NLM
 * 
 * Provides comprehensive data export capabilities for:
 * - Neural network data (structure, connectivity, dynamics)
 * - Spike activity and timing data
 * - Performance and metrics data
 * - Configuration and parameter data
 * - Analysis results and statistical data
 * - Batch processing results
 * - Real-time data streams
 * 
 * Supported output formats:
 * - CSV: Comma-separated values for spreadsheet import
 * - JSON: Structured data for programmatic access
 * - HDF5: Hierarchical data format for large datasets
 * - MATLAB: Octave/MATLAB format
 * - SQLite: Relational database format
 * - XML: Extensible markup format
 * - Binary: Compact binary format
 * - Parquet: Columnar storage format
 * 
 * Features:
 * - Multiple output format support
 * - Metadata and header information
 * - Compression support
 * - Chunked export for large datasets
 * - Progress reporting
 * - Error handling and recovery
 * - Streaming export for large datasets
 * - Custom data transformations
 * 
 * Integration with:
 * - All visualization systems
 * - Data collection systems
 * - Batch processing pipeline
 * - Experiment management
 * 
 * @see Visualizer
 * @see NetworkVisualizer
 * @see SpikeVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 */
class DataExporter {
public:
    /**
     * Export format enumeration
     */
    enum class ExportFormat {
        CSV,
        JSON,
        HDF5,
        MATLAB,
        SQLITE,
        XML,
        BINARY,
        PARQUET,
        TEXT,
        YAML,
        TOML
    };

    /**
     * Export configuration
     */
    struct ExportConfig {
        ExportFormat format = ExportFormat::CSV;
        std::string outputPath = ".";
        std::string filename = "export_data";
        bool compress = false;
        bool includeMetadata = true;
        bool chunkedExport = false;
        size_t chunkSize = 1000000; // 1MB chunks
        bool overwrite = false;
        bool validateData = true;
        std::string delimiter = ",";
        std::string quoteChar = "\"";
        std::string commentChar = "#";
        std::string dateTimeFormat = "%Y-%m-%d %H:%M:%S";
        std::string encoding = "utf-8";
        std::string dataPath = "/data";
        std::string groupName = "nlm_data";
        std::string experimentName = "";
        std::string version = "1.0.0";
        std::unordered_map<std::string, std::string> metadata;
        std::vector<std::string> includeDatasets;
        std::vector<std::string> excludeDatasets;
        bool exportOnlyModified = false;
    };

    /**
     * Dataset information
     */
    struct DatasetInfo {
        std::string name;
        std::string description;
        std::string format;
        size_t rows = 0;
        size_t columns = 0;
        double sizeMB = 0.0;
        std::chrono::system_clock::time_point created;
        std::string creator;
        std::unordered_map<std::string, std::string> metadata;
    };

    /**
     * Export progress information
     */
    struct ExportProgress {
        double current = 0.0;
        double total = 0.0;
        std::string currentDataset;
        size_t rowsProcessed = 0;
        size_t totalRows = 0;
        double speed = 0.0; // MB/s
        std::chrono::system_clock::time_point startTime;
        bool cancelled = false;
        std::string status;
        
        double getProgressPercent() const {
            return (total > 0) ? (current / total) * 100.0 : 0.0;
        }
        
        std::chrono::system_clock::duration getElapsedTime() const {
            return std::chrono::system_clock::now() - startTime;
        }
    };

    /**
     * Export callback function
     */
    using ExportCallback = std::function<void(const ExportProgress& progress)>;

    /**
     * Data transformation function
     */
    using DataTransform = std::function<std::vector<std::vector<std::string>>(const std::vector<std::vector<std::string>>& data)>;

    /**
     * DataExporter constructor
     */
    DataExporter();
    
    /**
     * DataExporter constructor with configuration
     */
    explicit DataExporter(const ExportConfig& config);
    
    /**
     * DataExporter destructor
     */
    ~DataExporter();

    /**
     * Configure export settings
     */
    void configure(const ExportConfig& config);
    
    /**
     * Export brain data
     */
    bool exportBrainData(const Brain& brain, const std::string& datasetName = "brain_data") const;
    
    /**
     * Export neuron data
     */
    bool exportNeuronData(const Brain& brain, const std::string& datasetName = "neurons") const;
    
    /**
     * Export synapse data
     */
    bool exportSynapseData(const Brain& brain, const std::string& datasetName = "synapses") const;
    
    /**
     * Export spike data
     */
    bool exportSpikeData(const std::vector<SpikeEvent>& spikes, const std::string& datasetName = "spikes") const;
    
    /**
     * Export performance data
     */
    bool exportPerformanceData(const std::string& datasetName = "performance") const;
    
    /**
     * Export configuration data
     */
    bool exportConfigData(const std::string& datasetName = "config") const;
    
    /**
     * Export experiment results
     */
    bool exportExperimentResults(const std::string& experimentPath, const std::string& datasetName = "experiments") const;
    
    /**
     * Export custom data
     */
    bool exportCustomData(const std::vector<std::vector<std::string>>& data, const std::string& datasetName) const;
    
    /**
     * Set export callback
     */
    void setExportCallback(const ExportCallback& callback);
    
    /**
     * Add data transformation
     */
    void addDataTransformation(const DataTransform& transform);
    
    /**
     * Get export statistics
     */
    std::vector<DatasetInfo> getExportStats() const;
    
    /**
     * Get current configuration
     */
    ExportConfig getConfig() const;
    
    /**
     * Check if export is available for format
     */
    static bool isFormatSupported(ExportFormat format);
    
    /**
     * Get list of supported formats
     */
    static std::vector<ExportFormat> getSupportedFormats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    DataExporter(const DataExporter&) = delete;
    DataExporter& operator=(const DataExporter&) = delete;
};

} // namespace nlm
