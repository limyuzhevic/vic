#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>
#include <unordered_map>
#include <functional>

namespace nlm {

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Disable copying, enable moving
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) noexcept;
    Profiler& operator=(Profiler&&) noexcept;
    
    // Profiling modes
    enum class ProfilingMode {
        None,
        CPU,
        Memory,
        SpikeTiming,
        Learning,
        Network,
        All
    };
    
    // Profiling configuration
    struct ProfilingConfig {
        ProfilingMode mode;
        bool enabled;
        double sampleInterval;
        size_t maxSamples;
        bool detailed;
        std::string outputFile;
        std::string format;
        bool continuous;
        bool realTime;
        double threshold;
        std::vector<std::string> metrics;
        std::vector<std::string> excludeMetrics;
        std::chrono::steady_clock::time_point startTime;
    } config;
    
    // Performance metrics
    struct PerformanceMetrics {
        std::chrono::steady_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        double peakMemoryUsage;
        size_t totalSpikes;
        size_t spikeRate;
        size_t activeNeurons;
        size_t totalNeurons;
        size_t totalSynapses;
        double averageFiringRate;
        double averageWeight;
        double weightVariance;
        double totalWeight;
        size_t strengthenedSynapses;
        size_t weakenedSynapses;
        size_t unchangedSynapses;
        double learningRate;
        double plasticityStrength;
        size_t networkDiameter;
        size_t averagePathLength;
        double synchronization;
        double stability;
        double entropy;
        std::vector<double> cpuHistory;
        std::vector<double> memoryHistory;
        std::vector<size_t> spikeHistory;
        std::vector<size_t> neuronHistory;
        std::vector<size_t> synapseHistory;
        std::vector<double> activityHistory;
        std::vector<double> weightHistory;
        std::chrono::steady_clock::time_point lastUpdate;
    } metrics;
    
    // Profiling results
    struct ProfilingResults {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        double totalDuration;
        ProfilingMode mode;
        std::vector<PerformanceMetrics> dataPoints;
        std::map<std::string, std::vector<double>> metricSeries;
        double cpuUsageAverage;
        double cpuUsagePeak;
        double memoryUsageAverage;
        double memoryUsagePeak;
        size_t totalSpikeCount;
        size_t totalLearningEvents;
        size_t totalPlasticityEvents;
        double averageLearningRate;
        double averageFiringRate;
        double averageWeightChange;
        double networkStability;
        double networkEntropy;
        std::string report;
    } results;
    
    // Initialization
    bool initialize();
    
    // Profiling control
    bool startProfiling(const ProfilingConfig& config);
    bool stopProfiling();
    bool pauseProfiling();
    bool resumeProfiling();
    
    // Configuration
    bool configure(const ProfilingConfig& config);
    bool updateConfig(const std::string& key, const std::string& value);
    ProfilingConfig getConfig() const { return config; }
    
    // Metrics collection
    void collectMetrics();
    void collectCPUMetrics();
    void collectMemoryMetrics();
    void collectSpikeMetrics();
    void collectLearningMetrics();
    void collectNetworkMetrics();
    
    // Analysis and reporting
    bool generateReport();
    bool exportResults(const std::string& filename, const std::string& format = "text");
    bool exportMetrics(const std::string& filename, const std::vector<std::string>& metrics = {});
    bool exportCSV(const std::string& filename) const;
    bool exportJSON(const std::string& filename) const;
    bool exportHTML(const std::string& filename) const;
    
    // Analysis functions
    double calculateMean(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data) const;
    double calculateVariance(const std::vector<double>& data) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculatePercentile(const std::vector<double>& data, double percentile) const;
    size_t findPeak(const std::vector<size_t>& data) const;
    double findInflectionPoint(const std::vector<double>& data) const;
    std::string analyzeTrend(const std::vector<double>& data) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    std::vector<double> smoothData(const std::vector<double>& data, int windowSize) const;
    std::vector<size_t> findPeaks(const std::vector<size_t>& data, size_t minDistance = 1) const;
    std::string detectAnomalies(const std::vector<double>& data, double threshold = 2.0) const;
    
    // Spike analysis
    std::vector<size_t> analyzeSpikeTiming(const std::vector<size_t>& spikes) const;
    double calculateFiringRate(const std::vector<size_t>& spikes, double duration) const;
    std::vector<size_t> findBurstPatterns(const std::vector<size_t>& spikes) const;
    double calculateCoefficientOfVariation(const std::vector<size_t>& spikes) const;
    
    // Learning analysis
    double calculateLearningRate(const std::vector<double>& weightChanges) const;
    std::vector<double> calculatePlasticityProfile(const std::vector<double>& weights) const;
    double calculateSynapticStability(const std::vector<double>& weights) const;
    size_t countLearningEvents(const std::vector<double>& weightChanges, double threshold) const;
    
    // Network analysis
    size_t calculateNetworkDiameter(const std::vector<std::vector<size_t>>& adjacencyMatrix) const;
    double calculateAveragePathLength(const std::vector<std::vector<size_t>>& adjacencyMatrix) const;
    double calculateClusteringCoefficient(const std::vector<std::vector<size_t>>& adjacencyMatrix) const;
    size_t findConnectedComponents(const std::vector<std::vector<size_t>>& adjacencyMatrix) const;
    
    // Results access
    const ProfilingResults& getResults() const { return results; }
    const PerformanceMetrics& getCurrentMetrics() const { return metrics; }
    std::vector<PerformanceMetrics> getMetricsHistory(size_t count = 0) const;
    std::map<std::string, std::vector<double>> getMetricSeries() const { return results.metricSeries; }
    
    // State
    bool isProfiling() const { return profiling; }
    bool isPaused() const { return paused; }
    bool isRunning() const { return running; }
    double getProgress() const;
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    
    // Help and documentation
    void showHelp() const;
    void showProfilingModes() const;
    void showMetricsList() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    bool profiling;
    bool paused;
    bool running;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    // Thread management
    std::thread profilingThread;
    std::mutex metricsMutex;
    std::condition_variable metricsCondition;
    
    // Helper methods
    void startProfilingThread();
    void stopProfilingThread();
    void profilingWorker();
    bool validateProfilingConfig(const ProfilingConfig& config) const;
    bool isMetricEnabled(const std::string& metric) const;
    bool isMetricExcluded(const std::string& metric) const;
    void addMetricSeries(const std::string& name, const std::vector<double>& data);
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateHTMLReport() const;
    std::string formatMetricValue(double value, const std::string& unit = "") const;
    std::string getProfilingModeString(ProfilingMode mode) const;
    std::string getFormatString(const std::string& format) const;
    bool ensureOutputDirectory(const std::string& filename) const;
    bool writeToFile(const std::string& filename, const std::string& content) const;
    std::string getFormattedTimestamp(const std::chrono::steady_clock::time_point& timestamp) const;
    std::string getSystemInfo() const;
    std::string getProfilerInfo() const;
    bool saveProfilingResults(const std::string& filename) const;
    bool loadProfilingResults(const std::string& filename);
    std::string getProfilingResultsFilename(const std::string& prefix = "profile") const;
    void cleanup();
};
