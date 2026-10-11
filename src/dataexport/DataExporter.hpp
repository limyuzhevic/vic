#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Brain;
class DebugConsole;

// Export format enumeration
enum class ExportFormat {
    CSV,
    JSON,
    Binary,
    ProtoBuf,
    HDF5,
    Custom
};

// Import format enumeration
enum class ImportFormat {
    CSV,
    JSON,
    Binary,
    ProtoBuf,
    HDF5,
    Custom
};

// Simulation data structure
struct SimulationData {
    // Time series data
    struct TimeSeries {
        std::vector<double> timestamps;
        std::map<std::string, std::vector<double>> metrics;
        std::map<std::string, std::vector<double>> neurons;
        std::map<std::string, std::vector<double>> synapses;
        std::map<std::string, std::vector<double>> regions;
    } timeSeries;
    
    // Neural state data
    struct NeuralState {
        size_t neuronId;
        NeuronType type;
        double membranePotential;
        double firingRate;
        double lastSpikeTime;
        bool isActive;
        bool isFiring;
        double regionId;
        double populationId;
        std::map<std::string, double> additionalProperties;
    } neuralState;
    
    // Synaptic data
    struct SynapseState {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        float weight;
        Delay delay;
        SynapseType type;
        bool isActive;
        std::vector<double> spikeHistoryPre;
        std::vector<double> spikeHistoryPost;
    } synapseState;
    
    // Brain state
    struct BrainState {
        SimulationStep step;
        double time;
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        std::map<std::string, double> regionStats;
        std::map<std::string, double> memoryStats;
        std::map<std::string, double> neuromodulatorStats;
    } brainState;
    
    // Sensor data
    struct SensorData {
        Timestamp timestamp;
        std::vector<double> readings;
        std::vector<std::string> sensorIds;
        std::map<std::string, double> processedReadings;
    } sensorData;
    
    // Environment data
    struct EnvironmentData {
        Timestamp timestamp;
        std::vector<double> temperature;
        std::vector<double> humidity;
        std::vector<double> pressure;
        std::vector<double> lightLevel;
        std::vector<double> noiseLevel;
        std::vector<double> vibrationLevel;
        std::map<std::string, double> additionalMetrics;
    } environmentData;
    
    // Experiment metadata
    struct ExperimentMetadata {
        std::string experimentId;
        std::string version;
        std::string description;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> parameters;
        std::map<std::string, std::string> tags;
        std::string author;
        std::string notes;
    } metadata;
    
    // Data compression options
    struct CompressionOptions {
        bool enabled = false;
        std::string algorithm;  // "gzip", "bzip2", "lz4", "zstd"
        int level = 6;  // Compression level
        bool chunks = false;  // Chunked compression
        size_t chunkSize = 1024 * 1024;  // 1MB chunks
    } compression;
    
    // Data chunk for streaming
    struct DataChunk {
        size_t chunkId;
        size_t totalChunks;
        std::vector<uint8_t> data;
        double timestamp;
    } currentChunk;
};

// Import result
struct ImportResult {
    bool success;
    std::string message;
    ImportFormat format;
    size_t recordsImported;
    size_t totalRecords;
    std::chrono::duration<double> processingTime;
    std::map<std::string, std::string> metadata;
    
    ImportResult() : success(false), recordsImported(0), totalRecords(0) {}
    ImportResult(bool s, const std::string& msg, ImportFormat fmt)
        : success(s), message(msg), format(fmt), recordsImported(0), totalRecords(0) {}
};

// Export result
struct ExportResult {
    bool success;
    std::string message;
    ExportFormat format;
    size_t recordsExported;
    size_t totalRecords;
    std::chrono::duration<double> processingTime;
    std::map<std::string, std::string> metadata;
    
    ExportResult() : success(false), recordsExported(0), totalRecords(0) {}
    ExportResult(bool s, const std::string& msg, ExportFormat fmt)
        : success(s), message(msg), format(fmt), recordsExported(0), totalRecords(0) {}
};

// Data comparison result
struct ComparisonResult {
    bool identical;
    double similarityScore;
    std::vector<size_t> differingSteps;
    std::vector<std::string> differences;
    std::map<std::string, double> differenceMetrics;
    
    ComparisonResult() : identical(true), similarityScore(1.0) {}
};

// Replay configuration
struct ReplayConfig {
    std::string filepath;  // Path to saved data file
    SimulationStep startStep = 0;
    SimulationStep endStep = static_cast<SimulationStep>(-1);  // Until end of file
    double startTime = 0.0;
    double endTime = std::numeric_limits<double>::max();
    bool skipMissingData = true;
    bool applyNeuronsOnly = false;
    bool applySynapsesOnly = false;
    bool applyBrainOnly = false;
    std::vector<std::string> includeTypes;  // Types of data to include
    std::vector<std::string> excludeTypes; // Types of data to exclude
    std::map<std::string, double> timeScaling;  // Time scaling factors for different data types
    std::map<std::string, double> valueScaling;  // Value scaling factors for different data types
    
    ReplayConfig(const std::string& path) : filepath(path) {}
};

// Data exporter configuration
struct DataExporterConfig {
    bool enabled = true;
    std::vector<ExportFormat> exportFormats;
    std::string defaultFormat;
    std::string outputDirectory;
    bool overwriteExisting = false;
    bool enableCompression = true;
    std::string compressionAlgorithm = "gzip";
    int compressionLevel = 6;
    bool enableStreaming = false;
    size_t chunkSize = 1024 * 1024;  // 1MB
    bool enableValidation = true;
    bool enableIntegrityCheck = true;
    std::string integrityAlgorithm = "sha256";
    bool enableVersionControl = false;
    std::string versionFile;
    bool backupExisting = true;
    std::string backupExtension = ".bak";
    bool enableProgressReporting = true;
    std::chrono::duration<double> progressInterval = std::chrono::seconds(1);
};

// Data importer configuration
struct DataImporterConfig {
    bool enabled = true;
    std::vector<ImportFormat> importFormats;
    std::string defaultFormat;
    std::string inputDirectory;
    bool overwriteExisting = false;
    bool enableValidation = true;
    bool enableIntegrityCheck = true;
    std::string integrityAlgorithm = "sha256";
    bool enableProgressReporting = true;
    std::chrono::duration<double> progressInterval = std::chrono::seconds(1);
    std::map<std::string, ImportFormat> fileFormatMapping;  // File extension -> format mapping
    bool autoDetectFormat = true;
    bool skipCorruptFiles = false;
    bool validateBeforeImport = true;
};

// Data exporter interface
class DataExporter {
public:
    DataExporter();
    DataExporter(const DataExporterConfig& config);
    ~DataExporter();
    
    // Disable copying
    DataExporter(const DataExporter&) = delete;
    DataExporter& operator=(const DataExporter&) = delete;
    
    // Initialize exporter
    bool initialize();
    
    // Export methods
    ExportResult exportBrainData(const Brain* brain, const std::string& filepath, 
                               ExportFormat format = ExportFormat::JSON,
                               const std::map<std::string, std::string>& options = {});
    
    ExportResult exportSimulationData(const SimulationData& data, const std::string& filepath,
                                     ExportFormat format = ExportFormat::CSV,
                                     const std::map<std::string, std::string>& options = {});
    
    ExportResult exportTimeSeries(const SimulationData::TimeSeries& data, const std::string& filepath,
                                 ExportFormat format = ExportFormat::CSV);
    
    ExportResult exportNeuralData(const std::vector<SimulationData::NeuralState>& neurons,
                                 const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    
    ExportResult exportSynapseData(const std::vector<SimulationData::SynapseState>& synapses,
                                  const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    
    ExportResult exportBrainState(const SimulationData::BrainState& state, const std::string& filepath,
                                 ExportFormat format = ExportFormat::JSON);
    
    ExportResult exportSensorData(const SimulationData::SensorData& data, const std::string& filepath,
                                 ExportFormat format = ExportFormat::CSV);
    
    ExportResult exportEnvironmentData(const SimulationData::EnvironmentData& data, const std::string& filepath,
                                       ExportFormat format = ExportFormat::CSV);
    
    // Export specific types
    ExportResult exportNeurons(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    ExportResult exportSynapses(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    ExportResult exportConnectivity(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::JSON);
    ExportResult exportSpikeData(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    ExportResult exportMemoryData(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::JSON);
    ExportResult exportNeuromodulationData(const Brain* brain, const std::string& filepath, ExportFormat format = ExportFormat::JSON);
    
    // Configuration
    void updateConfig(const DataExporterConfig& config);
    const DataExporterConfig& getConfig() const;
    
    // Import methods
    ImportResult importBrainData(Brain* brain, const std::string& filepath,
                                ImportFormat format = ImportFormat::JSON,
                                const std::map<std::string, std::string>& options = {});
    
    ImportResult importSimulationData(SimulationData& data, const std::string& filepath,
                                      ImportFormat format = ImportFormat::CSV,
                                      const std::map<std::string, std::string>& options = {});
    
    ImportResult importTimeSeries(SimulationData::TimeSeries& data, const std::string& filepath,
                                 ImportFormat format = ImportFormat::CSV);
    
    ImportResult importNeuralData(std::vector<SimulationData::NeuralState>& neurons, const std::string& filepath,
                                 ImportFormat format = ImportFormat::CSV);
    
    ImportResult importSynapseData(std::vector<SimulationData::SynapseState>& synapses, const std::string& filepath,
                                  ImportFormat format = ImportFormat::CSV);
    
    // Comparison methods
    ComparisonResult compareSimulations(const std::string& filepath1, const std::string& filepath2,
                                       ImportFormat format = ImportFormat::CSV);
    
    ComparisonResult compareBrainStates(const Brain* brain1, const Brain* brain2);
    
    ComparisonResult compareTimeSeries(const SimulationData::TimeSeries& ts1, const SimulationData::TimeSeries& ts2);
    
    // Replay functionality
    bool replaySimulation(Brain* brain, const ReplayConfig& config);
    bool replayData(const std::string& filepath, SimulationData& data, const ReplayConfig& config,
                   ImportFormat format = ImportFormat::CSV);
    
    // Data validation
    bool validateData(const SimulationData& data, const std::string& filepath = "") const;
    bool validateExport(const std::string& filepath, ExportFormat format) const;
    bool validateImport(const std::string& filepath, ImportFormat format) const;
    
    // Integrity checking
    bool computeIntegrity(const std::string& filepath, const std::string& algorithm = "sha256") const;
    bool verifyIntegrity(const std::string& filepath, const std::string& checksum) const;
    
    // File management
    std::vector<std::string> listExportFiles(const std::string& directory = "") const;
    std::vector<std::string> listImportFiles(const std::string& directory = "") const;
    bool deleteFile(const std::string& filepath);
    bool moveFile(const std::string& source, const std::string& destination);
    bool copyFile(const std::string& source, const std::string& destination);
    
    // Progress reporting
    void setProgressCallback(std::function<void(double, const std::string&)> callback);
    void clearProgressCallback();
    
    // File format detection
    ExportFormat detectExportFormat(const std::string& filename) const;
    ImportFormat detectImportFormat(const std::string& filename) const;
    
    // Check if engine is active
    bool isActive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
