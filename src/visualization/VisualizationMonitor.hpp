// Visualization and monitoring system for NLM neural simulation

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <deque>
#include <fstream>
#include <iomanip>

namespace nlm {

// Monitoring data points
struct MonitorData {
    std::chrono::system_clock::time_point timestamp;
    float cpuUsage;                   // CPU usage percentage
    float memoryUsage;                 // Memory usage in MB
    float memoryPeak;                  // Peak memory usage in MB
    float neuralActivity;              // Average firing rate
    float networkConnectivity;         // Network connectivity metric
    float plasticityRate;              // Plasticity update rate
    float curiosityLevel;              // Current curiosity level
    float noveltyLevel;                // Current novelty level
    float predictionError;             // Prediction error
    size_t totalSpikeCount;            // Total spike count
    size_t activeNeuronCount;          // Number of active neurons
    size_t firingNeuronCount;          // Number of firing neurons
    float averageFiringRate;           // Average firing rate
    float excitationInhibitionRatio;   // E/I ratio
    size_t regionCount;                // Number of brain regions
    size_t neuronCount;                // Total neuron count
    size_t synapseCount;                // Total synapse count
    
    MonitorData()
        : cpuUsage(0), memoryUsage(0), memoryPeak(0), neuralActivity(0), 
          networkConnectivity(0), plasticityRate(0), curiosityLevel(0), 
          noveltyLevel(0), predictionError(0), totalSpikeCount(0), 
          activeNeuronCount(0), firingNeuronCount(0), averageFiringRate(0),
          excitationInhibitionRatio(0), regionCount(0), neuronCount(0), 
          synapseCount(0) {}
    
    std::string toCSV() const {
        std::ostringstream oss;
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << ",";
        oss << cpuUsage << "," << memoryUsage << "," << memoryPeak << ",";
        oss << neuralActivity << "," << networkConnectivity << "," << plasticityRate << ",";
        oss << curiosityLevel << "," << noveltyLevel << "," << predictionError << ",";
        oss << totalSpikeCount << "," << activeNeuronCount << "," << firingNeuronCount << ",";
        oss << averageFiringRate << "," << excitationInhibitionRatio << ",";
        oss << regionCount << "," << neuronCount << "," << synapseCount;
        return oss.str();
    }
};

// Real-time monitoring system
class MonitoringSystem {
public:
    MonitoringSystem();
    ~MonitoringSystem();
    
    // Start monitoring
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Update monitoring data
    void updateMonitorData();
    
    // Add data to monitor (thread-safe)
    void addMonitorData(const MonitorData& data);
    
    // Get latest monitor data
    MonitorData getLatestData() const;
    
    // Get monitor data history
    std::vector<MonitorData> getDataHistory(size_t count) const;
    
    // Set brain reference for data collection
    void setBrain(std::shared_ptr<class Brain> brain);
    
    // Set monitoring interval (Hz)
    void setMonitoringInterval(double hz);
    
    // Get monitoring statistics
    std::map<std::string, float> getStatistics() const;
    
    // Export data to CSV
    bool exportData(const std::string& filepath) const;
    
    // Export data to JSON
    bool exportDataJSON(const std::string& filepath) const;
    
    // Get monitoring status
    bool isMonitoring() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network visualization system
class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Set brain reference
    void setBrain(std::shared_ptr<class Brain> brain);
    
    // Generate network graph data
    std::vector<std::vector<float>> generateAdjacencyMatrix() const;
    
    // Get community structure
    std::vector<size_t> getCommunityStructure() const;
    
    // Calculate network metrics
    struct NetworkMetrics {
        float density;                    // Network density
        float averageClustering;          // Average clustering coefficient
        float averagePathLength;          // Average shortest path length
        float characteristicPathLength;   // Characteristic path length
        float modularity;                 // Network modularity
        float participationCoefficient;   // Participation coefficient
    };
    
    NetworkMetrics calculateMetrics() const;
    
    // Visualize network as graph
    void visualizeNetworkGraph(const std::string& outputPath) const;
    
    // Visualize weight matrix
    void visualizeWeightMatrix(const std::string& outputPath, bool logarithmicScale = false) const;
    
    // Visualize region connectivity
    void visualizeRegionConnectivity(const std::string& outputPath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Activity visualization system
class ActivityVisualizer {
public:
    ActivityVisualizer();
    ~ActivityVisualizer();
    
    // Set brain reference
    void setBrain(std::shared_ptr<class Brain> brain);
    
    // Generate spike raster data
    struct SpikeRasterData {
        std::vector<double> spikeTimes;        // Spike times in seconds
        std::vector<size_t> neuronIndices;     // Neuron indices
        std::vector<size_t> regionIds;         // Region IDs
    };
    
    SpikeRasterData generateSpikeRaster(size_t maxSpikes = 10000) const;
    
    // Generate firing rate data
    std::vector<std::vector<float>> generateFiringRateHeatmap(size_t timeWindow = 1000) const;
    
    // Generate activity timeline
    std::vector<std::vector<float>> generateActivityTimeline(size_t timeWindow = 1000) const;
    
    // Visualize spike raster
    void visualizeSpikeRaster(const std::string& outputPath, const SpikeRasterData& data) const;
    
    // Visualize firing rate heatmap
    void visualizeFiringRateHeatmap(const std::string& outputPath, const std::vector<std::vector<float>>& heatmap) const;
    
    // Visualize activity timeline
    void visualizeActivityTimeline(const std::string& outputPath, const std::vector<std::vector<float>>& timeline) const;
    
    // Generate animated activity visualization
    void generateAnimation(const std::string& outputPath, size_t duration = 1000, double fps = 10.0) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System performance monitor
class SystemPerformanceMonitor {
public:
    SystemPerformanceMonitor();
    ~SystemPerformanceMonitor();
    
    // Start system monitoring
    void startSystemMonitoring();
    
    // Stop system monitoring
    void stopSystemMonitoring();
    
    // Get CPU usage
    float getCPUUsage() const;
    
    // Get memory usage
    struct MemoryInfo {
        uint64_t total;      // Total memory in bytes
        uint64_t used;       // Used memory in bytes
        uint64_t available;  // Available memory in bytes
        uint64_t peak;       // Peak memory usage in bytes
    };
    
    MemoryInfo getMemoryInfo() const;
    
    // Get disk usage
    struct DiskInfo {
        uint64_t total;      // Total disk space in bytes
        uint64_t used;       // Used disk space in bytes
        uint64_t available;  // Available disk space in bytes
    };
    
    DiskInfo getDiskInfo() const;
    
    // Get process information
    struct ProcessInfo {
        pid_t pid;            // Process ID
        std::string name;    // Process name
        float cpuUsage;       // CPU usage percentage
        uint64_t memory;     // Memory usage in bytes
    };
    
    ProcessInfo getProcessInfo() const;
    
    // Get network statistics
    struct NetworkStats {
        uint64_t bytesReceived;   // Total bytes received
        uint64_t bytesSent;       // Total bytes sent
        uint64_t packetsReceived; // Total packets received
        uint64_t packetsSent;     // Total packets sent
    };
    
    NetworkStats getNetworkStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Progress tracker for long-running simulations
class ProgressTracker {
public:
    ProgressTracker();
    ~ProgressTracker();
    
    // Start tracking progress
    void startProgress(const std::string& description);
    
    // Update progress
    void updateProgress(double progress, const std::string& message = "");
    
    // Get current progress
    double getProgress() const;
    
    // Get current message
    std::string getMessage() const;
    
    // Check if progress tracking is active
    bool isTracking() const;
    
    // Stop progress tracking
    void stopProgress();
    
    // Generate progress report
    std::string generateProgressReport() const;
    
    // Export progress data
    bool exportProgressData(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive visualization and monitoring system
class VisualizationAndMonitoringSystem {
public:
    VisualizationAndMonitoringSystem();
    ~VisualizationAndMonitoringSystem();
    
    // Initialize all systems
    bool initialize(std::shared_ptr<class Brain> brain);
    
    // Start all monitoring and visualization
    void startAll();
    
    // Stop all monitoring and visualization
    void stopAll();
    
    // Update all systems
    void update(double timestep);
    
    // Generate comprehensive report
    std::string generateComprehensiveReport() const;
    
    // Export all data
    bool exportAllData(const std::string& outputDirectory) const;
    
    // Get current system status
    std::map<std::string, std::string> getSystemStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
