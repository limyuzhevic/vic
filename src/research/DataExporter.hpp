#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../debug/VisualizationInterface.hpp"
#include "../debug/BrainInspector.hpp"
#include "../debug/MemoryProfiler.hpp"
#include "../research/ParameterOptimizer.hpp"
#include "../research/BatchProcessor.hpp"

namespace nlm {

class DataExporter {
public:
    DataExporter();
    ~DataExporter();
    
    // Export formats
    enum class ExportFormat {
        Binary,
        JSON,
        CSV,
        XML,
        HDF5,
        Parquet,
        MATLAB,
        Python
    };
    
    // Export types
    enum class ExportType {
        NeuralData,
        ExperimentResults,
        MemoryAnalysis,
        PerformanceMetrics,
        VisualizationFrames,
        CompleteDataset,
        Custom
    };
    
    // Configuration
    struct ExportConfig {
        std::string outputDirectory;
        ExportFormat format;
        std::vector<ExportType> exportTypes;
        bool compress;
        bool includeTimestamps;
        std::string filenamePrefix;
        std::string filenameSuffix;
        bool overwriteExisting;
        std::map<std::string, std::string> metadata;
        
        ExportConfig() : format(ExportFormat::JSON), compress(false), 
                        includeTimestamps(true), overwriteExisting(true) {}
    };
    
    // Initialize exporter
    bool initialize(const ExportConfig& config);
    
    // Export single experiment results
    bool exportExperiment(
        const std::shared_ptr<Experiment>& experiment,
        const std::string& outputPath = "");
    
    // Export batch processing results
    bool exportBatchResults(
        const std::vector<BatchProcessor::BatchResult>& batchResults,
        const std::string& outputPath = "");
    
    // Export parameter optimization results
    bool exportOptimizationResults(
        const std::vector<ParameterOptimizer::OptimizationResult>& optimizationResults,
        const std::string& outputPath = "");
    
    // Export brain state
    bool exportBrainState(
        const Brain& brain,
        const std::string& outputPath = "");
    
    // Export neural data
    bool exportNeuralData(
        const Brain& brain,
        const std::vector<SimulationStep>& steps,
        const std::string& outputPath = "");
    
    // Export environment data
    bool exportEnvironmentData(
        const Environment& environment,
        const std::vector<SimulationStep>& steps,
        const std::string& outputPath = "");
    
    // Export debug/analysis results
    bool exportDebugResults(
        const BrainInspector& inspector,
        const MemoryProfiler& profiler,
        const std::string& outputPath = "");
    
    // Export visualization frames
    bool exportVisualizationFrames(
        VisualizationInterface& vizInterface,
        const std::vector<std::chrono::steady_clock::time_point>& frameTimes,
        const std::string& outputPath = "");
    
    // Export combined research dataset
    bool exportResearchDataset(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        const std::string& outputPath = "");
    
    // Get exported file list
    std::vector<std::string> getExportedFiles() const;
    
    // Export management
    void clearExports();
    bool removeExport(const std::string& filename);
    bool removeAllExports();
    
    // Progress tracking
    double getExportProgress() const;
    std::string getCurrentExportStatus() const;
    
    // Configuration
    void setExportFormat(ExportFormat format);
    void setCompression(bool enabled);
    void setOutputDirectory(const std::string& directory);
    void setFilenamePrefix(const std::string& prefix);
    void addMetadata(const std::string& key, const std::string& value);
    
    // Progress callbacks
    using ExportProgressCallback = std::function<void(double progress, const std::string& status)>;
    void setProgressCallback(ExportProgressCallback callback);
    
    // Export statistics
    struct ExportStats {
        size_t totalExports;
        size_t successfulExports;
        size_t failedExports;
        double totalTime;
        std::chrono::duration<double> lastExportTime;
        std::vector<std::string> recentErrors;
    };
    
    ExportStats getExportStats() const;
    
    // Advanced features
    bool enableDataCompression(bool enable);
    bool enableIncrementalExport(bool enable);
    bool enableExportVerification(bool enable);
    
    // Data validation
    bool validateExportData(const std::string& filename) const;
    bool verifyExportIntegrity(const std::string& filename) const;
    
    // Analysis integration
    bool exportWithAnalysis(
        const Brain& brain,
        const Environment& environment,
        const std::shared_ptr<Experiment>& experiment,
        bool includeDebugAnalysis = true,
        bool includePerformanceAnalysis = true);
    
    // Research workflow support
    bool exportForPublication(const std::string& outputPath = "");
    bool exportForReproducibility(const std::string& outputPath = "");
    bool exportForArchiving(const std::string& outputPath = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
