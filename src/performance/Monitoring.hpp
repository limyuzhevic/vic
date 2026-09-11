namespace nlm {

// Performance monitoring and analysis system
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Real-time brain state monitoring
    struct BrainStateMetrics {
        double simulationTime;           // Current simulation time
        SimulationStep currentStep;      // Current simulation step
        size_t totalNeurons;             // Total neuron count
        size_t activeNeurons;            // Number of active neurons
        size_t firingNeurons;            // Number of firing neurons
        float averageFiringRate;         // Average firing rate
        float excitationInhibitionRatio; // E/I balance ratio
        size_t totalSpikes;              // Total spike count
        float memoryPressure;            // Working memory usage (0.0-1.0)
        float neuromodulationLoad;       // Total neuromodulator activity
        float cpuUsage;                  // CPU utilization (0.0-1.0)
        float memoryUsage;               // Memory usage (MB)
        double updateTime;               // Time for last update (ms)
        double maxUpdateTime;            // Maximum update time (ms)
        size_t spikeQueueSize;           // Size of spike processing queue
    };
    
    BrainStateMetrics getCurrentMetrics() const;
    
    // Performance metrics collection and analysis
    class MetricsCollector {
    public:
        MetricsCollector();
        ~MetricsCollector();
        
        // Start/stop metric collection
        void startCollection();
        void stopCollection();
        bool isCollecting() const;
        
        // Get historical metrics
        std::vector<BrainStateMetrics> getHistory(size_t maxCount = 1000) const;
        
        // Compute metrics statistics
        struct MetricsStats {
            double meanUpdateTime;
            double stdUpdateTime;
            double minUpdateTime;
            double maxUpdateTime;
            float meanFiringRate;
            float meanMemoryPressure;
            float meanNeuromodulationLoad;
            size_t peakNeurons;
            SimulationStep stepsProcessed;
        };
        
        MetricsStats computeStats(const std::vector<BrainStateMetrics>& metrics) const;
        
        // Export metrics
        void exportToCSV(const std::string& filepath) const;
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    MetricsCollector& getMetricsCollector();
    
    // Resource usage tracking
    struct ResourceUsage {
        float cpuUsage;              // CPU utilization (0.0-1.0)
        float memoryUsage;           // Memory usage (MB)
        size_t memoryFootprint;      // Memory footprint (bytes)
        size_t activeThreads;        // Number of active threads
        size_t allocatedMemory;      // Allocated memory (MB)
        float swapUsage;             // Swap usage (0.0-1.0)
        double uptime;               // Uptime in seconds
    };
    
    ResourceUsage getResourceUsage() const;
    
    // Performance alerts and thresholds
    class PerformanceAlert {
    public:
        enum AlertType {
            HighUpdateTime,
            HighMemoryUsage,
            LowFiringRate,
            HighMemoryPressure,
            HighNeuromodulationLoad,
            SpikeQueueOverflow
        };
        
        AlertType getType() const;
        std::string getMessage() const;
        double getTimestamp() const;
        float getValue() const;
        float getThreshold() const;
        
    private:
        AlertType type_;
        std::string message_;
        double timestamp_;
        float value_;
        float threshold_;
    };
    
    std::vector<PerformanceAlert> getAlerts() const;
    void clearAlerts();
    
    // Set performance thresholds
    void setUpdateTimeThreshold(float threshold);
    void setMemoryUsageThreshold(float threshold);
    void setMemoryPressureThreshold(float threshold);
    
    // Neural activity pattern analysis
    std::vector<float> analyzeNeuralActivity(size_t regionId = 0) const;
    float computePatternComplexity(const std::vector<float>& pattern) const;
    float computePatternSynchrony(const std::vector<float>& pattern) const;
    
    // Connectivity analysis
    struct ConnectionStats {
        size_t totalConnections;
        float averageStrength;
        float maxStrength;
        float minStrength;
        size_t strongConnections;     // Strength > 0.8
        size_t weakConnections;       // Strength < 0.2
        size_t bidirectionalConnections;
    };
    
    ConnectionStats analyzeConnectivity(size_t regionId = 0) const;
    std::vector<std::pair<size_t, size_t>> findHubNeurons(size_t regionId = 0) const;
    
    // State visualization data extraction
    std::string generateVisualizationData() const;
    std::string generateJSONVisualization() const;
    std::string generateXMLVisualization() const;
    
    // Performance profiling
    void startProfiling();
    void stopProfiling();
    std::string getProfilingResults() const;
    
    // Resource monitoring
    void setMonitoringInterval(double interval); // seconds
    double getMonitoringInterval() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
