#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Visualization interface for debugging and analysis
// PLACEHOLDER - Phase 2 will have real visualization
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Output format options
    enum class OutputFormat {
        Text,           // Plain text output
        CSV,            // Comma-separated values
        JSON,           // JavaScript Object Notation
        Parquet,        // Columnar data format
        HDF5,           // Hierarchical Data Format 5
        MAT,            // MATLAB format
        PNG,            // Image format
        HTML,           // Web visualization
        SVG             // Scalable Vector Graphics
    };
    
    // Configure output formats
    void enableOutputFormat(OutputFormat format, const std::string& filepath);
    void disableOutputFormat(OutputFormat format);
    bool isOutputFormatEnabled(OutputFormat format) const;
    
    // Data export
    bool exportData(const std::string& filepath, OutputFormat format = OutputFormat::JSON);
    bool exportNetwork(const std::string& filepath, OutputFormat format = OutputFormat::CSV);
    bool exportActivity(const std::string& filepath, OutputFormat format = OutputFormat::CSV);
    bool exportWeights(const std::string& filepath, OutputFormat format = OutputFormat::CSV);
    
    // Analysis and statistics
    void generateStatisticsReport(const std::string& filepath);
    void generatePerformanceMetrics(const std::string& filepath);
    void generateLearningAnalysis(const std::string& filepath);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Advanced visualization features
    void enable3DVisualization(bool enable);
    void setColorScheme(const std::string& scheme);
    void setVisualizationParameters(const std::string& parameters);
    
    // Batch processing capabilities
    void startBatchVisualization(const std::string& batchName);
    void endBatchVisualization();
    bool isInBatchMode() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    void setMonitoringThresholds(const std::string& thresholds);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization statistics structure
struct VisualizationStats {
    std::string timestamp;
    size_t totalSteps;
    size_t activeNeurons;
    float averageFiringRate;
    float maxFiringRate;
    float plasticityScore;
    size_t spikeCount;
    size_t synapticEvents;
    float memoryUtilization;
    
    VisualizationStats()
        : timestamp(), totalSteps(0), activeNeurons(0), averageFiringRate(0.0f),
          maxFiringRate(0.0f), plasticityScore(0.0f), spikeCount(0),
          synapticEvents(0), memoryUtilization(0.0f) {}
};

} // namespace nlm
