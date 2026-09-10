// Configuration system for NLM visualization
#pragma once

#include <string>
#include <map>
#include <memory>
#include <variant>

namespace nlm {

// Visualization settings
struct VisualizationSettings {
    // Display settings
    bool enableNetworkView = true;
    bool enableActivityView = true;
    bool enableSpikeView = true;
    bool enableWeightsView = true;
    bool enableMemoryView = true;
    bool enableNeuromodulationView = true;
    bool enableDevelopmentView = true;
    
    // Update rates
    double networkUpdateRate = 10.0;
    double activityUpdateRate = 30.0;
    double spikeUpdateRate = 60.0;
    
    // Colors and appearance
    std::string backgroundColor = "#2d2d2d";
    std::string neuronColorActive = "#ff6b6b";
    std::string neuronColorInactive = "#4a4a4a";
    std::string synapseColorExc = "#4caf50";
    std::string synapseColorInh = "#f44336";
    
    // Recording settings
    bool enableRecording = false;
    std::string recordingFormat = "csv"; // csv, json, binary
    size_t recordingInterval = 100;
    bool compressRecording = false;
    size_t maxRecordedFrames = 10000;
    
    // Performance monitoring
    bool enablePerformanceMonitoring = true;
    double performanceMonitorUpdateRate = 1.0;
    
    // Backend settings
    std::string backend = "console"; // console, web, gui
    std::string webPort = "8080";
    bool headless = false;
};

// Visualization backend interface
class VisualizationBackend {
public:
    virtual ~VisualizationBackend() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void setSettings(const VisualizationSettings& settings) = 0;
    virtual bool isActive() const = 0;
    
    // Backend-specific methods
    virtual void setWindowSize(int width, int height) {}
    virtual void setWindowTitle(const std::string& title) {}
    virtual void toggleFullscreen() {}
};

// Recording system for simulation data
class SimulationRecorder {
public:
    SimulationRecorder() = default;
    virtual ~SimulationRecorder() = default;
    
    // Start/stop recording
    virtual bool startRecording(const std::string& filepath, const VisualizationSettings& settings) = 0;
    virtual void stopRecording() = 0;
    virtual bool isRecording() const = 0;
    
    // Record simulation frame
    virtual void recordFrame(const class Brain& brain, size_t step, double time) = 0;
    
    // Replay functionality
    virtual bool loadRecording(const std::string& filepath) = 0;
    virtual void startReplay() = 0;
    virtual void stopReplay() = 0;
    virtual bool isReplaying() const = 0;
    virtual size_t getCurrentReplayFrame() const = 0;
    virtual size_t getTotalFrames() const = 0;
    
    // Get recorded data
    virtual std::vector<double> getRecordedTimes() const = 0;
    virtual std::vector<size_t> getRecordedSteps() const = 0;
    virtual const class Brain* getFrameState(size_t frameIndex) = 0;
    
    // Export functionality
    virtual bool exportData(const std::string& outputPath, const std::string& format) = 0;
    
    // Compression
    virtual void setCompressionLevel(size_t level) {}
};

// Performance monitor for real-time metrics
class PerformanceMonitor {
public:
    PerformanceMonitor() = default;
    virtual ~PerformanceMonitor() = default;
    
    struct Metrics {
        double cpuUsage;              // CPU usage percentage
        double memoryUsage;          // Memory usage in MB
        double simulationTime;       // Time per simulation step
        double renderingTime;        // Time spent rendering
        double updateTime;           // Time spent updating data
        size_t spikeCount;           // Number of spikes processed
        size_t neuronCount;          // Number of active neurons
        size_t synapseCount;         // Number of synapses
        float eiratio;              // Excitatory/inhibitory ratio
        float averageFiringRate;    // Average firing rate
        size_t memoryTraces;        // Working memory traces
        size_t episodicEpisodes;    // Episodic memory episodes
        float noveltyLevel;          // Novelty detection level
        float curiosityLevel;        // Curiosity level
        float dopamineLevel;         // Dopamine level
    };
    
    virtual void update() = 0;
    virtual const Metrics& getMetrics() const = 0;
    virtual void setUpdateRate(double hz) = 0;
    virtual void enableMetrics(bool enable) = 0;
};

// Recording system for visualization
class VisualizationRecorder {
public:
    VisualizationRecorder() = default;
    virtual ~VisualizationRecorder() = default;
    
    // Frame-by-frame neural state recording
    virtual void recordFrame(size_t step, double time, const class Brain& brain,
                            const PerformanceMonitor::Metrics& metrics) = 0;
    
    // Data export
    virtual bool exportCSV(const std::string& filepath) const = 0;
    virtual bool exportJSON(const std::string& filepath) const = 0;
    virtual bool exportBinary(const std::string& filepath) const = 0;
    
    // Compression
    virtual void compress() = 0;
    virtual bool isCompressed() const = 0;
    
    // Statistics
    virtual size_t getTotalFrames() const = 0;
    virtual double getDuration() const = 0;
    virtual std::string getFileSize() const = 0;
};

} // namespace nlm