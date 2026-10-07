#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Enhanced logging and visualization system for NLM
// Provides real-time debugging, network visualization, and performance monitoring

namespace nlm {

// Enhanced log entry with additional context
struct EnhancedLogEntry : public LogEntry {
    // Performance metrics
    double cpuUsage;           // CPU usage percentage
    double memoryUsage;        // Memory usage percentage
    double frameTime;          // Last frame execution time
    
    // Network statistics
    size_t totalSpikes;        // Total spikes processed
    size_t neuronsActive;      // Number of active neurons
    float averageFiringRate;   // Average firing rate across network
    
    // Visualization data
    bool enableNetworkViz;     // Enable network visualization
    bool enablePerformanceViz; // Enable performance visualization
    bool enableDebugViz;       // Enable debug visualization
    
    EnhancedLogEntry()
        : LogEntry()
        , cpuUsage(0.0)
        , memoryUsage(0.0)
        , frameTime(0.0)
        , totalSpikes(0)
        , neuronsActive(0)
        , averageFiringRate(0.0f)
        , enableNetworkViz(false)
        , enablePerformanceViz(false)
        , enableDebugViz(false) {}
};

// Enhanced console logger with visualization support
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Visualization configuration
    void enableNetworkVisualization(bool enable);
    void enablePerformanceVisualization(bool enable);
    void enableDebugVisualization(bool enable);
    
    // Set performance metrics
    void setPerformanceMetrics(double cpuUsage, double memoryUsage, 
                               double frameTime, size_t totalSpikes,
                               size_t neuronsActive, float avgFiringRate);
    
    // Generate visualization output
    void generateNetworkVisualization(const std::vector<std::vector<float>>& adjacencyMatrix);
    void generatePerformanceGraph(const std::vector<double>& timestamps,
                                 const std::vector<double>& cpuUsage,
                                 const std::vector<double>& memoryUsage);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Performance monitor for real-time metrics collection
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring
    void start();
    
    // Stop monitoring
    void stop();
    
    // Get current metrics
    double getCPUUsage() const;
    double getMemoryUsage() const;
    double getFrameTime() const;
    size_t getTotalSpikes() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Update metrics
    void updateMetrics(size_t spikes, size_t activeNeurons, float firingRate);
    
    // Configure monitoring
    void setMonitoringInterval(double interval); // seconds
    void enableMemoryTracking(bool enable);
    void enableCPUMonitoring(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network visualizer for debugging and analysis
class NeuralNetworkVisualizer {
public:
    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer();
    
    // Configure visualization
    void setGraphSize(size_t width, size_t height);
    void setNeuronRadius(float radius);
    void setConnectionThickness(float thickness);
    void setColorMap(const std::vector<std::string>& colors);
    
    // Set network state
    void setNetworkState(const std::vector<std::vector<float>>& adjacencyMatrix,
                        const std::vector<float>& neuronStates,
                        const std::vector<std::string>& neuronLabels);
    
    // Generate visualizations
    std::string generateAdjacencyMatrixVisualization() const;
    std::string generateNeuronActivityVisualization() const;
    std::string generateTimeSeriesVisualization(const std::vector<double>& data) const;
    
    // Export visualizations
    bool exportToFile(const std::string& filename) const;
    bool exportToHTML(const std::string& filename) const;
    
    // Real-time visualization update
    void update(const std::vector<std::vector<float>>& newAdjacencyMatrix);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global debug utilities
namespace Debug {
    // Performance timing utilities
    class Timer {
    public:
        Timer();
        ~Timer();
        
        void start();
        void stop();
        double getElapsedTime() const; // in milliseconds
        double getAverageTime() const;
        
        // Convenience for timing code blocks
        template<typename Func>
        auto measure(Func&& func) {
            start();
            auto result = func();
            stop();
            return result;
        }
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Memory tracking utilities
    class MemoryTracker {
    public:
        MemoryTracker();
        ~MemoryTracker();
        
        void trackAllocation(size_t size, const char* file, int line);
        void trackDeallocation(size_t size, const char* file, int line);
        void printReport();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Network debugging utilities
    class NetworkDebugger {
    public:
        NetworkDebugger();
        ~NetworkDebugger();
        
        void logSpike(const SpikeEvent& spike);
        void logConnection(const SynapseHandle& synapse);
        void logNeuronActivity(const NeuronId& neuron, float activity);
        void printNetworkState();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

// Enhanced logging macros with file/line/function support
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with enhanced capabilities
class EnhancedLogStream {
public:
    EnhancedLogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~EnhancedLogStream();
    
    template<typename T>
    EnhancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance tracking
    EnhancedLogStream& withPerformance(const Debug::Timer& timer);
    EnhancedLogStream& withMemory(const Debug::MemoryTracker& tracker);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::shared_ptr<PerformanceMonitor> perfMonitor_;
    std::shared_ptr<Debug::MemoryTracker> memoryTracker_;
};

#define NLM_LOG_STREAM(level) nlm::EnhancedLogStream(level, __FILE__, __LINE__, __func__)

// Convenience functions for quick debugging
void debugNetworkState(const std::shared_ptr<Brain>& brain);
void debugPerformanceMetrics(const PerformanceMonitor& monitor);
void debugMemoryUsage();
void generatePerformanceReport(const std::string& filename);

} // namespace nlm

} // namespace nlm
