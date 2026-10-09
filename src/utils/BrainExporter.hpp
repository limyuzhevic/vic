#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>

namespace nlm {

class BrainExporter {
public:
    BrainExporter();
    ~BrainExporter();
    
    // Disable copying, enable moving
    BrainExporter(const BrainExporter&) = delete;
    BrainExporter& operator=(const BrainExporter&) = delete;
    BrainExporter(BrainExporter&&) noexcept;
    BrainExporter& operator=(BrainExporter&&) noexcept;
    
    // Export formats
    enum class ExportFormat {
        Binary,
        Text,
        JSON,
        CSV,
        XML,
        YAML,
        HTML,
        Markdown,
        Python,
        MATLAB,
        Octave,
        NeuralNetwork,
        SynapseMatrix,
        NeuronData,
        SpikeTrains,
        ConnectivityMatrix,
        WeightMatrix,
        Custom
    };
    
    // Export configuration
    struct ExportConfig {
        ExportFormat format;
        std::string filename;
        std::string directory;
        bool overwrite;
        bool compress;
        bool validate;
        std::string compressionAlgorithm;
        bool includeConfig;
        bool includeMetadata;
        bool includeStatistics;
        bool includeConnectivity;
        bool includePlasticity;
        bool includeSpiking;
        bool includeLearning;
        bool includeSynapses;
        bool includeNeurons;
        bool includeRegions;
        bool includeTimeSeries;
        bool includeAnalysis;
        std::vector<std::string> selectedRegions;
        std::vector<std::string> selectedNeurons;
        std::vector<std::string> selectedSynapses;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        double timestep;
        std::string delimiter;
        std::string escapeChar;
        bool includeHeaders;
        bool includeComments;
        std::string encoding;
        std::string lineEnding;
        bool prettyPrint;
        size_t maxDepth;
        std::string customTemplate;
        std::map<std::string, std::string> customOptions;
    } config;
    
    // Export result
    struct ExportResult {
        bool success;
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        std::string filename;
        std::string format;
        size_t bytesWritten;
        size_t totalDataSize;
        double duration;
        size_t neuronCount;
        size_t synapseCount;
        size_t totalSpikes;
        std::string errorCode;
    } lastResult;
    
    // Initialization
    bool initialize();
    
    // Export commands
    bool exportCommand(const std::vector<std::string>& args);
    bool saveCommand(const std::vector<std::string>& args);
    bool writeCommand(const std::vector<std::string>& args);
    bool dumpCommand(const std::vector<std::string>& args);
    bool serializeCommand(const std::vector<std::string>& args);
    
    // Export formats
    bool exportToBinary(const std::string& filename);
    bool exportToText(const std::string& filename);
    bool exportToJSON(const std::string& filename);
    bool exportToCSV(const std::string& filename);
    bool exportToXML(const std::string& filename);
    bool exportToYAML(const std::string& filename);
    bool exportToHTML(const std::string& filename);
    bool exportToMarkdown(const std::string& filename);
    bool exportToPython(const std::string& filename);
    bool exportToMATLAB(const std::string& filename);
    bool exportToOctave(const std::string& filename);
    bool exportToNeuralNetwork(const std::string& filename);
    bool exportToSynapseMatrix(const std::string& filename);
    bool exportToNeuronData(const std::string& filename);
    bool exportToSpikeTrains(const std::string& filename);
    bool exportToConnectivityMatrix(const std::string& filename);
    bool exportToWeightMatrix(const std::string& filename);
    
    // Configuration export
    bool exportConfiguration(const std::string& filename, const std::string& format = "text");
    bool exportBrainState(const std::string& filename, const std::string& format = "binary");
    bool exportNetworkTopology(const std::string& filename, const std::string& format = "text");
    bool exportSynapticWeights(const std::string& filename, const std::string& format = "text");
    bool exportSpikeData(const std::string& filename, const std::string& format = "text");
    bool exportLearningHistory(const std::string& filename, const std::string& format = "text");
    bool exportPlasticityData(const std::string& filename, const std::string& format = "text");
    bool exportRegionData(const std::string& filename, const std::string& format = "text");
    bool exportTimeSeriesData(const std::string& filename, const std::string& format = "text");
    
    // Advanced export options
    bool exportWithAnalysis(const std::string& filename, const std::string& format = "text", bool includeStatistics = true);
    bool exportWithStatistics(const std::string& filename, const std::string& format = "text");
    bool exportWithMetadata(const std::string& filename, const std::string& format = "text");
    bool exportWithComments(const std::string& filename, const std::string& format = "text");
    bool exportWithHeaders(const std::string& filename, const std::string& format = "text");
    bool exportWithCompression(const std::string& filename, const std::string& format = "text", const std::string& algorithm = "gzip");
    
    // Batch export
    bool exportMultiple(const std::vector<std::string>& filenames, const std::string& format = "text");
    bool exportArchive(const std::string& archiveFile, const std::vector<std::string>& filenames);
    
    // Export configuration
    bool configure(const ExportConfig& config);
    bool updateConfig(const std::string& key, const std::string& value);
    ExportConfig getConfig() const { return config; }
    
    // Format detection and conversion
    ExportFormat detectFormat(const std::string& filename) const;
    std::string getFormatExtension(ExportFormat format) const;
    std::string getFormatDescription(ExportFormat format) const;
    bool validateFormatSupport(ExportFormat format) const;
    
    // Results and statistics
    const ExportResult& getLastResult() const { return lastResult; }
    std::vector<std::string> getAvailableFormats() const;
    std::vector<std::string> getRecentExports() const;
    size_t getExportCount() const;
    
    // State
    bool isExporting() const { return exporting; }
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // Help and documentation
    void showHelp() const;
    void showExportFormats() const;
    void showConfigurationExport() const;
    void showAdvancedExport() const;
    void showBatchExport() const;
    
    // File operations
    bool ensureExportDirectory(const std::string& filename) const;
    bool validateExportPath(const std::string& path) const;
    bool saveExportWithBackup(const std::string& filename, const std::string& backupName = "");
    bool exportWithProgress(const std::string& filename, const std::string& format = "text", std::function<void(double)> progressCallback = nullptr);
    bool exportWithValidation(const std::string& filename, const std::string& format = "text", std::function<bool(const std::string&)> validationCallback = nullptr);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    bool exporting;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Thread management
    std::thread exportThread;
    std::mutex exportMutex;
    std::condition_variable exportCondition;
    
    // Helper methods
    std::string generateExportFilename(const std::string& baseName, const std::string& format) const;
    std::string getCurrentTimestamp() const;
    std::string formatExportData(const std::string& format, const std::string& data) const;
    bool writeExportData(const std::string& filename, const std::string& data);
    bool createExportArchive(const std::string& archiveFile, const std::vector<std::string>& files);
    bool extractExportArchive(const std::string& archiveFile, const std::string& outputDir);
    std::string getFormatSpecificHeader(const ExportFormat& format) const;
    std::string getFormatSpecificFooter(const ExportFormat& format) const;
    std::string getConfigurationSection(const std::string& format) const;
    std::string getBrainStateSection(const std::string& format) const;
    std::string getNetworkTopologySection(const std::string& format) const;
    std::string getSynapticWeightsSection(const std::string& format) const;
    std::string getSpikeDataSection(const std::string& format) const;
    std::string getLearningHistorySection(const std::string& format) const;
    std::string getPlasticityDataSection(const std::string& format) const;
    std::string getRegionDataSection(const std::string& format) const;
    std::string getTimeSeriesDataSection(const std::string& format) const;
    std::string getStatisticsSection(const std::string& format) const;
    std::string getMetadataSection(const std::string& format) const;
    std::string getCommentsSection(const std::string& format) const;
    bool exportBinary(const std::string& filename);
    bool exportText(const std::string& filename);
    bool exportJSON(const std::string& filename);
    bool exportCSV(const std::string& filename);
    bool exportXML(const std::string& filename);
    bool exportYAML(const std::string& filename);
    bool exportHTML(const std::string& filename);
    bool exportMarkdown(const std::string& filename);
    bool exportPython(const std::string& filename);
    bool exportMATLAB(const std::string& filename);
    bool exportOctave(const std::string& filename);
    bool exportNeuralNetwork(const std::string& filename);
    bool exportSynapseMatrix(const std::string& filename);
    bool exportNeuronData(const std::string& filename);
    bool exportSpikeTrains(const std::string& filename);
    bool exportConnectivityMatrix(const std::string& filename);
    bool exportWeightMatrix(const std::string& filename);
    bool exportToFile(const std::string& filename, const std::string& data, bool compress = false);
    bool extractFromFile(const std::string& filename, std::string& data, bool decompress = false);
    void cleanup();
};
