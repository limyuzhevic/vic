#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <unordered_map>

namespace nlm {

// Visualization interface for debugging and analysis
// Enhanced with comprehensive monitoring and real-time visualization
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with enhanced monitoring
    bool initialize();
    
    // Update visualization data with performance metrics
    void update();
    
    // Render current state with detailed statistics
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate with automatic adjustment based on system load
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Network visualization with performance metrics
    void visualizeNetwork(const class Brain& brain);
    
    // Enhanced activity visualization with multiple views
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot with timing and frequency analysis
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization with connectivity statistics
    void visualizeWeights(const class Brain& brain);
    
    // Real-time monitoring dashboard
    void updateMonitoringDashboard(const class Brain& brain, double timestep);
    
    // Performance profiling tools
    void startPerformanceProfiling();
    void endPerformanceProfiling();
    void logPerformanceMetrics(const std::string& operation, double duration, size_t count = 1);
    
    // Memory usage monitoring
    void logMemoryUsage();
    
    // Network topology analysis
    void analyzeNetworkTopology(const class Brain& brain);
    
    // Connectivity metrics
    struct ConnectivityMetrics {
        size_t totalConnections;
        float averageWeight;
        float maxWeight;
        float minWeight;
        float weightStdDev;
        size_t excitatoryConnections;
        size_t inhibitoryConnections;
        float excitationInhibitionRatio;
    };
    
    ConnectivityMetrics computeConnectivityMetrics(const class Brain& brain) const;
    
    // Activity statistics
    struct ActivityStatistics {
        size_t totalSpikes;
        float averageFiringRate;
        float maxFiringRate;
        float minFiringRate;
        double lastActivityTime;
        size_t activeNeurons;
        size_t maxActiveNeurons;
    };
    
    ActivityStatistics computeActivityStatistics(const class Brain& brain) const;
    
    // Development stage tracking
    void updateDevelopmentMetrics(const class Brain& brain);
    
    // Memory and checkpoint statistics
    void updateMemoryStatistics();
    
    // Real-time debug console
    void printDebugInfo(const class Brain& brain, double currentTime) const;
    
    // Performance benchmarking
    void runPerformanceBenchmark(const class Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
