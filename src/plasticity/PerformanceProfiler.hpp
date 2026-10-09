#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <variant>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {

// Performance profiling configuration
struct PerformanceProfilerConfig {
    bool enabled{true};                         // Enable performance profiling
    std::string profilerMode{"comprehensive"};   // Profiling mode: basic, comprehensive, advanced
    std::string outputFormat{"json"};            // Output format: json, csv, text, html
    std::string outputDirectory{"."};           // Output directory
    std::chrono::milliseconds profilingInterval{1000};  // Profiling interval (ms)
    bool realTimeMonitoring{false};              // Enable real-time monitoring
    bool generateReports{true};                  // Generate profiling reports
    bool exportMetrics{true};                    // Export metrics to files
    size_t maxReportHistory{100};                // Maximum report history size
    bool enableAlerting{true};                   // Enable performance alerts
    double performanceThreshold{0.80};           // Performance threshold (0-1)
    std::string logLevel{"info"};               // Log level for profiling
    bool enableCustomMetrics{true};              // Enable custom metrics
    
    // Neuron-specific profiling
    struct NeuronProfiling {
        bool enableNeuronMetrics{true};           // Enable neuron metrics collection
        bool trackNeuronTypes{true};              // Track neuron type performance
        bool trackSynapticActivity{true};          // Track synaptic activity
        bool trackMembraneDynamics{true};          // Track membrane potential dynamics
        bool trackSpikingBehavior{true};           // Track spiking behavior
        bool trackAdaptation{true};                // Track adaptation currents
        bool trackPlasticity{true};                // Track plasticity events
    } neuron;
    
    // Synaptic profiling
    struct SynapseProfiling {
        bool enableSynapseMetrics{true};           // Enable synapse metrics collection
        bool trackWeightChanges{true};             // Track synaptic weight changes
        bool trackTransmission{true};               // Track signal transmission
        bool trackCompatibility{true};             // Track synapse compatibility
        bool trackMaturation{true};                 // Track synapse maturation
        bool trackPlasticity{true};                 // Track plasticity effects
    } synapse;
    
    // Plasticity profiling
    struct PlasticityProfiling {
        bool enablePlasticityMetrics{true};         // Enable plasticity metrics collection
        bool trackSTDP{true};                      // Track STDP events
        bool trackHebbian{true};                   // Track Hebbian learning
        bool trackStructural{true};                 // Track structural plasticity
        bool trackLearningRates{true};              // Track learning rate changes
        bool trackStability{true};                  // Track network stability
    } plasticity;
    
    // Network profiling
    struct NetworkProfiling {
        bool enableNetworkMetrics{true};           // Enable network metrics collection
        bool trackTopology{true};                   // Track network topology
        bool trackActivity{true};                   // Track network activity
        bool trackFlow{true};                       // Track signal flow
        bool trackSynchronization{true};            // Track synchronization
        bool trackEfficiency{true};                 // Track computational efficiency
    } network;
    
    // System profiling
    struct SystemProfiling {
        bool enableSystemMetrics{true};             // Enable system metrics collection
        bool trackCPU{true};                        // Track CPU usage
        bool trackMemory{true};                     // Track memory usage
        bool trackI/O{true};                        // Track I/O operations
        bool trackThreads{true};                    // Track thread usage
        bool trackNetwork{true};                    // Track network operations
    } system;
    
    // Validation
    bool validate() const {
        if (profilingInterval.count() < 10) return false;
        if (performanceThreshold < 0.0 || performanceThreshold > 1.0) return false;
        return true;
    }
};

// Performance profiling results
struct PerformanceProfile {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double totalDuration{0.0};
    PerformanceProfilerConfig config;
    
    // Neuron performance
    struct NeuronPerformance {
        size_t totalNeurons{0};
        size_t activeNeurons{0};
        double averageFiringRate{0.0};
        double maxFiringRate{0.0};
        double averageMembranePotential{0.0};
        double membranePotentialVariance{0.0};
        double averageAdaptation{0.0};
        double plasticityEfficiency{0.0};
        
        // Neuron type performance
        std::unordered_map<std::string, double> neuronTypePerformance;
        
        // Synaptic metrics
        size_t totalSynapses{0};
        double averageWeight{0.0};
        double weightStdDev{0.0};
        size_t strengthenedSynapses{0};
        size_t weakenedSynapses{0};
        size_t unchangedSynapses{0};
    } neuron;
    
    // Synaptic performance
    struct SynapsePerformance {
        double averageTransmissionDelay{0.0};
        size_t totalTransmissions{0};
        double transmissionSuccessRate{0.0};
        double averageCompatibility{0.0};
        double synapseMaturation{0.0};
        
        // Plasticity effects
        size_t totalSTDPChanges{0};
        size_t totalHebbianChanges{0};
        size_t totalStructuralChanges{0};
        double averageWeightChange{0.0};
    } synapse;
    
    // Plasticity performance
    struct PlasticityPerformance {
        double learningRate{0.0};
        double plasticityStrength{0.0};
        size_t totalLearningEvents{0};
        double convergenceRate{0.0};
        double stabilityScore{0.0};
        double entropy{0.0};
        
        // Plasticity rule performance
        struct RulePerformance {
            std::string ruleName;
            size_t events{0};
            double efficiency{0.0};
            double stability{0.0};
        };
        
        std::vector<RulePerformance> rulePerformance;
    } plasticity;
    
    // Network performance
    struct NetworkPerformance {
        size_t neuronsPerSecond{0};                // Neurons processed per second
        size_t synapsesPerSecond{0};               // Synapses processed per second
        double averagePathLength{0.0};
        size_t networkDiameter{0};
        double clusteringCoefficient{0.0};
        double synchronizationScore{0.0};
        double computationalEfficiency{0.0};
        
        // Activity metrics
        size_t totalSpikes{0};
        double spikeRate{0.0};
        double activitySpread{0.0};
        double burstFrequency{0.0};
    } network;
    
    // System performance
    struct SystemPerformance {
        double cpuUsage{0.0};
        double memoryUsage{0.0};
        double memoryAvailable{0.0};
        size_t activeThreads{0};
        double diskReadRate{0.0};
        double diskWriteRate{0.0};
        double networkReceiveRate{0.0};
        double networkTransmitRate{0.0};
        
        // Resource usage
        double cpuTime{0.0};
        double userTime{0.0};
        double systemTime{0.0};
        size_t processMemory{0};
    } system;
    
    // Performance analysis
    struct PerformanceAnalysis {
        std::string performanceGrade;               // A-F grade
        std::vector<std::string> strengths;        // Areas of strength
        std::vector<std::string> weaknesses;       // Areas of weakness
        std::vector<std::string> recommendations;  // Recommendations for improvement
        double overallScore{0.0};                   // Overall performance score (0-1)
        
        // Bottleneck identification
        std::vector<std::string> bottlenecks;
        
        // Trend analysis
        struct Trend {
            std::string metric;
            std::string direction;  // increasing, decreasing, stable
            double rate;            // rate of change
            std::string confidence;  // high, medium, low
        };
        
        std::vector<Trend> trends;
    } analysis;
    
    // Profile metadata
    struct ProfileMetadata {
        std::string version{"1.0"};
        std::string buildInfo;
        std::string configuration;
        std::chrono::steady_clock::time_point generated;
        std::string generatedBy;
        std::unordered_map<std::string, std::string> customFields;
    } metadata;
    
    // Performance summary
    struct PerformanceSummary {
        std::string profileType;                    // Type of profile (neurons, synapses, plasticity, etc.)
        std::string scope;                          // Scope (network, region, population, etc.)
        std::chrono::milliseconds duration;        // Duration of profiling
        size_t dataPoints;                          // Number of data points collected
        double dataCollectionRate;                 // Points per second
        bool completed;                             // Whether profile was completed
        std::string status;                        // Status (success, error, incomplete)
    } summary;
};

// Performance profiling interface
class IPerformanceProfiler {
public:
    virtual ~IPerformanceProfiler() = default;
    
    // Configuration
    virtual bool configure(const PerformanceProfilerConfig& config) = 0;
    virtual PerformanceProfilerConfig getConfig() const = 0;
    
    // Profiling control
    virtual bool startProfiling() = 0;
    virtual bool stopProfiling() = 0;
    virtual bool pauseProfiling() = 0;
    virtual bool resumeProfiling() = 0;
    virtual bool isProfiling() const = 0;
    virtual bool isPaused() const = 0;
    
    // Profile generation
    virtual bool generateProfile() = 0;
    virtual PerformanceProfile getCurrentProfile() const = 0;
    virtual std::vector<PerformanceProfile> getProfileHistory(size_t count = 0) = 0;
    
    // Profiling hooks
    virtual void recordNeuronProfile(const std::string& neuronId, const std::unordered_map<std::string, double>& metrics) = 0;
    virtual void recordSynapseProfile(const std::string& synapseId, const std::unordered_map<std::string, double>& metrics) = 0;
    virtual void recordPlasticityProfile(const std::string& ruleName, const std::unordered_map<std::string, double>& metrics) = 0;
    virtual void recordNetworkProfile(const std::string& networkId, const std::unordered_map<std::string, double>& metrics) = 0;
    virtual void recordSystemProfile(const std::unordered_map<std::string, double>& metrics) = 0;
    
    // Metrics collection
    virtual void recordMetric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordTimer(const std::string& name, std::chrono::duration<double> duration, const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordCounter(const std::string& name, size_t count = 1, const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    virtual void recordGauge(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags = {}) = 0;
    
    // Reporting
    virtual std::string generateReport(const PerformanceProfile& profile) = 0;
    virtual std::string generateDetailedReport(const PerformanceProfile& profile) = 0;
    virtual bool exportProfile(const std::string& filename, const PerformanceProfile& profile) = 0;
    
    // Analysis
    virtual std::string analyzePerformance(const PerformanceProfile& profile) = 0;
    virtual std::vector<std::string> getPerformanceRecommendations(const PerformanceProfile& profile) = 0;
    virtual bool detectBottlenecks(const PerformanceProfile& profile) = 0;
    
    // Alerting
    virtual std::vector<std::string> getActiveAlerts() = 0;
    virtual void acknowledgeAlert(const std::string& alertId) = 0;
    virtual void clearAlerts() = 0;
    
    // Integration
    virtual bool integrateWithProfiler(std::shared_ptr<Profiler> profiler) = 0;
    virtual bool integrateWithMonitor(std::shared_ptr<PerformanceMonitor> monitor) = 0;
    virtual bool integrateWithMetrics(std::shared_ptr<MetricsCollector> metrics) = 0;
    
    // Configuration
    virtual bool setCustomMetric(const std::string& name, const std::function<double()>& getter) = 0;
    virtual bool removeCustomMetric(const std::string& name) = 0;
    
    // Export
    virtual bool exportToJSON(const std::string& filename) = 0;
    virtual bool exportToCSV(const std::string& filename) = 0;
    virtual bool exportToHTML(const std::string& filename) = 0;
    
    // Health and status
    virtual bool isHealthy() const = 0;
    virtual std::string getStatus() const = 0;
    virtual size_t getTotalProfileCount() const = 0;
};

// Main performance profiler implementation
class PerformanceProfiler : public IPerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler() override;
    
    // Disable copying, enable moving
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) noexcept;
    PerformanceProfiler& operator=(PerformanceProfiler&&) noexcept;
    
    // Configuration
    bool configure(const PerformanceProfilerConfig& config) override;
    PerformanceProfilerConfig getConfig() const override { return config; }
    
    // Profiling control
    bool startProfiling() override;
    bool stopProfiling() override;
    bool pauseProfiling() override;
    bool resumeProfiling() override;
    bool isProfiling() const override { return profiling; }
    bool isPaused() const override { return paused; }
    
    // Profile generation
    bool generateProfile() override;
    PerformanceProfile getCurrentProfile() const override;
    std::vector<PerformanceProfile> getProfileHistory(size_t count = 0) override;
    
    // Profiling hooks
    void recordNeuronProfile(const std::string& neuronId, const std::unordered_map<std::string, double>& metrics) override;
    void recordSynapseProfile(const std::string& synapseId, const std::unordered_map<std::string, double>& metrics) override;
    void recordPlasticityProfile(const std::string& ruleName, const std::unordered_map<std::string, double>& metrics) override;
    void recordNetworkProfile(const std::string& networkId, const std::unordered_map<std::string, double>& metrics) override;
    void recordSystemProfile(const std::unordered_map<std::string, double>& metrics) override;
    
    // Metrics collection
    void recordMetric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordTimer(const std::string& name, std::chrono::duration<double> duration, const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordCounter(const std::string& name, size_t count = 1, const std::unordered_map<std::string, std::string>& tags = {}) override;
    void recordGauge(const std::string& name, double value, const std::unordered_map<std::string, std::string>& tags = {}) override;
    
    // Reporting
    std::string generateReport(const PerformanceProfile& profile) override;
    std::string generateDetailedReport(const PerformanceProfile& profile) override;
    bool exportProfile(const std::string& filename, const PerformanceProfile& profile) override;
    
    // Analysis
    std::string analyzePerformance(const PerformanceProfile& profile) override;
    std::vector<std::string> getPerformanceRecommendations(const PerformanceProfile& profile) override;
    bool detectBottlenecks(const PerformanceProfile& profile) override;
    
    // Alerting
    std::vector<std::string> getActiveAlerts() override;
    void acknowledgeAlert(const std::string& alertId) override;
    void clearAlerts() override;
    
    // Integration
    bool integrateWithProfiler(std::shared_ptr<Profiler> profiler) override;
    bool integrateWithMonitor(std::shared_ptr<PerformanceMonitor> monitor) override;
    bool integrateWithMetrics(std::shared_ptr<MetricsCollector> metrics) override;
    
    // Configuration
    bool setCustomMetric(const std::string& name, const std::function<double()>& getter) override;
    bool removeCustomMetric(const std::string& name) override;
    
    // Export
    bool exportToJSON(const std::string& filename) override;
    bool exportToCSV(const std::string& filename) override;
    bool exportToHTML(const std::string& filename) override;
    
    // Health and status
    bool isHealthy() const override;
    std::string getStatus() const override;
    size_t getTotalProfileCount() const override { return profileHistory.size(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PerformanceProfilerConfig config;
    bool profiling;
    bool paused;
    std::chrono::steady_clock::time_point profileStartTime;
    
    // Integrated components
    std::shared_ptr<Profiler> profiler;
    std::shared_ptr<PerformanceMonitor> monitor;
    std::shared_ptr<MetricsCollector> metrics;
    
    // Profile history
    std::vector<PerformanceProfile> profileHistory;
    
    // Alert management
    std::vector<std::string> alerts;
    
    // Custom metrics
    std::unordered_map<std::string, std::function<double()>> customMetrics;
    
    // Collection worker
    void profilingWorker();
    
    // Profile generation
    void collectNeuronMetrics(PerformanceProfile& profile) const;
    void collectSynapseMetrics(PerformanceProfile& profile) const;
    void collectPlasticityMetrics(PerformanceProfile& profile) const;
    void collectNetworkMetrics(PerformanceProfile& profile) const;
    void collectSystemMetrics(PerformanceProfile& profile) const;
    
    // Profile analysis
    std::string calculatePerformanceGrade(const PerformanceProfile& profile) const;
    void analyzeNeuronPerformance(PerformanceProfile& profile) const;
    void analyzeSynapsePerformance(PerformanceProfile& profile) const;
    void analyzePlasticityPerformance(PerformanceProfile& profile) const;
    void analyzeNetworkPerformance(PerformanceProfile& profile) const;
    void analyzeSystemPerformance(PerformanceProfile& profile) const;
    
    // Bottleneck detection
    bool detectNeuronsBottleneck(const PerformanceProfile& profile) const;
    bool detectSynapsesBottleneck(const PerformanceProfile& profile) const;
    bool detectPlasticityBottleneck(const PerformanceProfile& profile) const;
    bool detectNetworkBottleneck(const PerformanceProfile& profile) const;
    bool detectSystemBottleneck(const PerformanceProfile& profile) const;
    
    // Reporting
    std::string generateNeuronReport(const PerformanceProfile& profile) const;
    std::string generateSynapseReport(const PerformanceProfile& profile) const;
    std::string generatePlasticityReport(const PerformanceProfile& profile) const;
    std::string generateNetworkReport(const PerformanceProfile& profile) const;
    std::string generateSystemReport(const PerformanceProfile& profile) const;
    
    // File I/O
    bool writeToFile(const std::string& filename, const std::string& content) const;
    bool ensureOutputDirectory(const std::string& filename) const;
    
    // Utility functions
    double calculateAverage(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values) const;
    double calculatePercentile(const std::vector<double>& values, double percentile) const;
    std::string getPerformanceGrade(double score) const;
    std::string getProfileType(const PerformanceProfile& profile) const;
    
    // Alert generation
    void generatePerformanceAlerts(const PerformanceProfile& profile);
    void generateNeuronAlerts(const PerformanceProfile& profile);
    void generateSynapseAlerts(const PerformanceProfile& profile);
    void generatePlasticityAlerts(const PerformanceProfile& profile);
    void generateNetworkAlerts(const PerformanceProfile& profile);
    void generateSystemAlerts(const PerformanceProfile& profile);
};

} // namespace nlm