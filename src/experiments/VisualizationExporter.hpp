#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/ExperimentConfig.hpp"
#include "../experiments/PerformanceMetrics.hpp"

namespace nlm {

// Data structure for visualization export
struct VisualizationData {
    std::string format;                     // Output format (CSV, JSON, etc.)
    std::string title;                      // Visualization title
    std::string description;                // Visualization description
    Timestamp timestamp;                    // When data was generated
    SimulationStep startStep;              // Starting simulation step
    SimulationStep endStep;                // Ending simulation step
    
    // Neuron data
    std::vector<size_t> neuronIds;          // Neuron IDs
    std::vector<float> neuronMembranePotentials; // Membrane potentials
    std::vector<float> neuronFiringRates;   // Firing rates
    std::vector<bool> neuronActiveStates;   // Active state (firing or not)
    std::vector<float> neuronTypes;         // Neuron types (0=exc, 1=inh, etc.)
    
    // Synapse data
    std::vector<std::pair<size_t, size_t>> synapseConnections; // (pre, post) neuron pairs
    std::vector<float> synapseWeights;       // Synaptic weights
    std::vector<float> synapseDelays;        // Connection delays
    
    // Time-series data
    std::vector<Timestamp> timestamps;      // Time points
    std::vector<size_t> neuronCounts;        // Neuron count at each time point
    std::vector<size_t> synapseCounts;       // Synapse count at each time point
    std::vector<size_t> spikeCounts;         // Spike count at each time point
    std::vector<float> avgFiringRates;      // Average firing rate at each time point
    
    // Experiment metadata
    std::string experimentName;             // Experiment name
    std::string experimentType;             // Experiment type
    uint64_t seed;                          // Random seed
    
    // Performance metrics
    std::map<std::string, std::vector<double>> performanceMetrics; // Metric name -> time series
    
    // Visualization settings
    std::map<std::string, std::string> visualizationOptions; // Format-specific options
};

// Visualization export result
struct VisualizationExportResult {
    std::string format;                     // Output format
    std::string filepath;                   // Output file path
    Timestamp exportTime;                   // When export was performed
    size_t dataSize;                        // Size of exported data in bytes
    bool success;                           // Whether export succeeded
    std::string errorMessage;               // Error message if failed
    double exportDuration;                  // How long export took
    
    std::string summary() const {
        std::stringstream ss;
        ss << "ExportResult: " << (success ? "SUCCESS" : "FAILED") << "\n";
        ss << "  Format: " << format << "\n";
        ss << "  File: " << filepath << "\n";
        ss << "  Duration: " << exportDuration << "s\n";
        ss << "  Size: " << dataSize << " bytes\n";
        if (!errorMessage.empty()) {
            ss << "  Error: " << errorMessage << "\n";
        }
        return ss.str();
    }
};

// VisualizationExporter class for advanced visualization integration
class VisualizationExporter {
public:
    VisualizationExporter();
    ~VisualizationExporter();
    
    // Configuration
    void configureExportOptions(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getExportOptions() const;
    
    // Export to different formats
    VisualizationExportResult exportToCSV(const VisualizationData& data, const std::string& filepath) const;
    VisualizationExportResult exportToJSON(const VisualizationData& data, const std::string& filepath) const;
    VisualizationExportResult exportToText(const VisualizationData& data, const std::string& filepath) const;
    VisualizationExportResult exportToBin(const VisualizationData& data, const std::string& filepath) const;
    VisualizationExportResult exportToCustomFormat(const VisualizationData& data, const std::string& filepath, 
                                                   const std::string& format) const;
    
    // Convenience methods for common export scenarios
    VisualizationExportResult exportExperimentResults(const std::shared_ptr<Experiment>& experiment,
                                                      const std::string& filepath);
    VisualizationExportResult exportBrainVisualization(const std::shared_ptr<Brain>& brain,
                                                       const std::string& filepath);
    VisualizationExportResult exportPerformanceMetrics(const PerformanceMetrics& metrics,
                                                       const std::string& filepath);
    
    // Export multiple experiments/brains
    std::vector<VisualizationExportResult> exportBatch(const std::vector<VisualizationData>& dataList);
    
    // Format-specific export methods
    std::string generateCSV(const VisualizationData& data) const;
    std::string generateJSON(const VisualizationData& data) const;
    std::string generateText(const VisualizationData& data) const;
    std::vector<uint8_t> generateBinary(const VisualizationData& data) const;
    
    // Data collection from existing sources
    VisualizationData collectDataFromExperiment(const std::shared_ptr<Experiment>& experiment) const;
    VisualizationData collectDataFromBrain(const std::shared_ptr<Brain>& brain) const;
    VisualizationData collectDataFromMetrics(const PerformanceMetrics& metrics) const;
    VisualizationData collectCombinedData(const std::shared_ptr<Experiment>& experiment,
                                          const std::shared_ptr<Brain>& brain,
                                          const PerformanceMetrics& metrics) const;
    
    // Validation and preprocessing
    bool validateData(const VisualizationData& data) const;
    VisualizationData preprocessData(const VisualizationData& data) const;
    
    // File format detection and handling
    std::string detectFormatFromExtension(const std::string& filepath) const;
    bool canExportFormat(const std::string& format) const;
    std::vector<std::string> getSupportedFormats() const;
    
    // Performance and monitoring
    double getExportTime(const std::string& format) const;
    size_t getExportCount(const std::string& format) const;
    std::map<std::string, double> getPerformanceStats() const;
    
    // Cleanup and statistics
    void resetStatistics();
    size_t getTotalExports() const;
    size_t getSuccessfulExports() const;
    size_t getFailedExports() const;
    double getSuccessRate() const;
    
    // Export format support
    struct ExportFormat {
        std::string name;                    // Format name (e.g., "CSV", "JSON")
        std::string extension;               // File extension
        std::string mimeType;                // MIME type
        bool supports2D;                     // Supports 2D visualization
        bool supports3D;                     // Supports 3D visualization
        bool supportsAnimation;              // Supports animation
        bool supportsInteractivity;          // Supports interactive features
        bool isTextBased;                    // Is text-based format
        bool isBinaryFormat;                 // Is binary format
        
        std::string defaultOptions() const {
            return "default"; // Format-specific defaults
        }
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void initializeFormats();
    ExportFormat getFormatInfo(const std::string& format) const;
    std::string formatValue(const std::string& value) const;
    std::string escapeJSON(const std::string& value) const;
    std::string escapeCSV(const std::string& value) const;
    void recordExportStats(const std::string& format, bool success, double duration, size_t dataSize);
};

} // namespace nlm
