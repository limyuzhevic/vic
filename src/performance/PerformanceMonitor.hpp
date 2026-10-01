#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace nlm {

// ===============================================================
// PERFORMANCE MONITORING INFRASTRUCTURE
// ===============================================================

struct PerformanceMetrics {
    // Timing metrics
    double cpuTime = 0.0;           // CPU time in milliseconds
    double wallTime = 0.0;          // Wall clock time in milliseconds
    double stepTime = 0.0;          // Average time per step
    double maxStepTime = 0.0;       // Maximum step time
    double minStepTime = 0.0;       // Minimum step time
    
    // Memory metrics
    size_t memoryAllocated = 0;     // Total memory allocated (bytes)
    size_t memoryPeak = 0;          // Peak memory usage (bytes)
    size_t memoryFootprint = 0;     // Current memory footprint (bytes)
    size_t neuronMemory = 0;        // Memory used by neurons (bytes)
    size_t synapseMemory = 0;        // Memory used by synapses (bytes)
    size_t memorySystemMemory = 0;  // Memory used by memory systems (bytes)
    
    // Neural activity metrics
    size_t totalSpikes = 0;         // Total number of spikes
    size_t activeNeurons = 0;       // Number of active neurons
    float firingRate = 0.0f;         // Average firing rate (Hz)
    float excitationInhibitionRatio = 0.0f; // E/I balance
    
    // System metrics
    int cpuUsage = 0;               // CPU usage percentage (0-100)
    int memoryUsagePercent = 0;     // Memory usage percentage (0-100)
    int systemLoad = 0;             // System load average
    int threadCount = 0;            // Number of active threads
    
    // Integration metrics
    int systemsConnected = 0;       // Number of connected systems
    float integrationScore = 0.0f;   // Overall integration score (0-1)
    size_t totalExperiments = 0;    // Number of experiments run
    double totalReward = 0.0;        // Total reward accumulated
    double averageEfficiency = 0.0f; // Average efficiency
    
    // Development metrics
    std::string developmentalStage; // Current developmental stage
    float plasticityRate = 0.0f;     // Current plasticity rate
    float learningRate = 0.0f;       // Current learning rate
    
    // Experiment metrics
    double experimentTime = 0.0;     // Total experiment time
    double simulationTime = 0.0;     // Total simulation time
    size_t stepsPerSecond = 0;       // Steps per second
    
    // Snapshot data
    std::chrono::system_clock::time_point timestamp;
    std::string sessionId;
};

struct PerformanceSnapshot {
    PerformanceMetrics metrics;
    std::unordered_map<std::string, double> customMetrics;
    std::unordered_map<std::string, std::string> systemInfo;
    
    std::chrono::system_clock::time_point timestamp;
    double elapsedTime = 0.0;
};

struct NeuralPerformanceMetrics {
    // Neuron-level metrics
    size_t totalNeurons = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    size_t silentNeurons = 0;
    
    // Synapse-level metrics
    size_t totalSynapses = 0;
    size_t strongExcitatory = 0;
    size_t weakExcitatory = 0;
    size_t strongInhibitory = 0;
    size_t weakInhibitory = 0;
    
    // Network-level metrics
    double clusteringCoefficient = 0.0;
    double pathLength = 0.0;
    double smallWorldCoefficient = 0.0;
    double modularity = 0.0;
    
    // Activity patterns
    std::vector<float> firingRateDistribution;
    std::vector<float> synapticWeightDistribution;
    std::vector<size_t> activityPatterns;
    
    // Plasticity metrics
    size_t totalSTDPChanges = 0;
    size_t totalHebbianChanges = 0;
    size_t totalStructuralChanges = 0;
    double plasticityEfficiency = 0.0;
};

struct MemorySystemMetrics {
    // Working memory metrics
    size_t workingMemoryCapacity = 0;
    size_t workingMemoryUsage = 0;
    float workingMemoryUtilization = 0.0f;
    size_t workingMemoryActiveTraces = 0;
    
    // Episodic memory metrics
    size_t episodicMemoryCapacity = 0;
    size_t episodicMemoryUsage = 0;
    float episodicMemoryUtilization = 0.0f;
    size_t episodicMemoryEpisodes = 0;
    size_t episodicMemoryReplayCount = 0;
    
    // Associative memory metrics
    size_t associativeMemoryCapacity = 0;
    size_t associativeMemoryUsage = 0;
    float associativeMemoryUtilization = 0.0f;
    size_t associativeMemoryPatternCount = 0;
    
    // Memory consolidation metrics
    size_t consolidationEvents = 0;
    float consolidationEfficiency = 0.0f;
    size_t memoryDecayEvents = 0;
    
    // Memory retrieval metrics
    double retrievalAccuracy = 0.0f;
    double retrievalLatency = 0.0f;
    size_t retrievalSuccessCount = 0;
    size_t retrievalFailureCount = 0;
};

struct NeuromodulationMetrics {
    // Dopamine system metrics
    float dopamineLevel = 0.0f;
    float dopamineVariance = 0.0f;
    float rewardPredictionError = 0.0f;
    float rewardPrecision = 0.0f;
    
    // Curiosity system metrics
    float curiosityLevel = 0.0f;
    float curiosityVariance = 0.0f;
    size_t explorationEvents = 0;
    float explorationEfficiency = 0.0f;
    
    // Novelty detection metrics
    float noveltyLevel = 0.0f;
    float noveltySensitivity = 0.0f;
    size_t noveltyEvents = 0;
    float noveltyPredictiveness = 0.0f;
    
    // Prediction error metrics
    float predictionErrorLevel = 0.0f;
    float predictionErrorSensitivity = 0.0f;
    size_t predictionErrorEvents = 0;
    float predictionAccuracy = 0.0f;
    
    // Neuromodulation integration
    float neuromodulationEfficiency = 0.0f;
    size_t neuromodulationEvents = 0;
    float neuromodulationStability = 0.0f;
};

struct DevelopmentMetrics {
    // Developmental stage metrics
    std::string currentStage;
    float stageProgress = 0.0f;
    double stageDuration = 0.0;
    
    // Plasticity metrics
    float initialPlasticity = 0.0f;
    float criticalPeriodPlasticity = 0.0f;
    float maturationPlasticity = 0.0f;
    float adultPlasticity = 0.0f;
    float agingPlasticity = 0.0f;
    
    // Synaptogenesis metrics
    size_t synaptogenesisEvents = 0;
    float synaptogenesisRate = 0.0f;
    size_t synapseGrowth = 0;
    
    // Pruning metrics
    size_t pruningEvents = 0;
    float pruningRate = 0.0f;
    size_t synapseElimination = 0;
    
    // Maturation metrics
    float maturationProgress = 0.0f;
    size_t maturationEvents = 0;
    float maturityLevel = 0.0f;
    
    // Development efficiency
    float developmentalEfficiency = 0.0f;
    double timeToMaturity = 0.0;
    float resourceEfficiency = 0.0f;
};

struct ExperimentMetrics {
    // Experiment setup
    std::string experimentType;
    std::string experimentId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    // Performance metrics
    double experimentDuration = 0.0;
    size_t simulationSteps = 0;
    double stepsPerSecond = 0.0;
    float efficiencyScore = 0.0f;
    
    // Behavioral metrics
    double totalReward = 0.0;
    double averageRewardPerStep = 0.0;
    double rewardVariance = 0.0;
    float learningCurve = 0.0f;
    
    // Neural metrics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t totalSpikes = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 0.0f;
    
    // Memory metrics
    size_t memoryEpisodesStored = 0;
    size_t memoryConsolidationEvents = 0;
    float memoryEfficiency = 0.0f;
    
    // Neuromodulation metrics
    float averageDopamineLevel = 0.0f;
    float averageCuriosityLevel = 0.0f;
    float averageNoveltyLevel = 0.0f;
    
    // Integration metrics
    float integrationScore = 0.0f;
    size_t successfulIntegrations = 0;
    size_t failedIntegrations = 0;
    
    // Quality metrics
    double reproducibility = 0.0f;
    double robustness = 0.0f;
    double scalability = 0.0f;
};

// ===============================================================
// PERFORMANCE MONITORING INTERFACES
// ===============================================================

class IPerformanceMonitor {
public:
    virtual ~IPerformanceMonitor() = default;
    
    // Core monitoring methods
    virtual void startMeasurement() = 0;
    virtual void stopMeasurement() = 0;
    virtual PerformanceMetrics getCurrentMetrics() const = 0;
    virtual std::vector<PerformanceSnapshot> getHistoricalSnapshots() const = 0;
    
    // Snapshot management
    virtual void takeSnapshot(const std::string& label = "") = 0;
    virtual void restoreSnapshot(const std::string& label) = 0;
    virtual void deleteSnapshot(const std::string& label) = 0;
    
    // Performance analysis
    virtual PerformanceMetrics analyzePerformance(size_t window = 0) const = 0;
    virtual NeuralPerformanceMetrics analyzeNeuralPerformance(size_t window = 0) const = 0;
    virtual MemorySystemMetrics analyzeMemoryPerformance(size_t window = 0) const = 0;
    virtual NeuromodulationMetrics analyzeNeuromodulationPerformance(size_t window = 0) const = 0;
    virtual DevelopmentMetrics analyzeDevelopmentPerformance(size_t window = 0) const = 0;
    virtual ExperimentMetrics analyzeExperimentPerformance(size_t window = 0) const = 0;
    
    // Trend analysis
    virtual std::vector<double> analyzeTrends(const std::string& metricName, size_t window = 0) const = 0;
    virtual double calculateTrend(const std::string& metricName, size_t window = 0) const = 0;
    virtual std::string getPerformanceGrade() const = 0;
    
    // Performance optimization
    virtual std::vector<std::string> getOptimizationSuggestions() const = 0;
    virtual double calculateEfficiency(const std::string& component = "") const = 0;
    virtual double calculateStability(const std::string& component = "") const = 0;
    
    // Data export
    virtual std::string exportMetrics(const std::string& format = "json") const = 0;
    virtual bool importMetrics(const std::string& data, const std::string& format = "json") = 0;
    virtual bool saveMetrics(const std::string& filepath) const = 0;
    virtual bool loadMetrics(const std::string& filepath) = 0;
    
    // Configuration
    virtual void setMeasurementInterval(double interval) = 0;
    virtual void setMaxHistorySize(size_t maxSize) = 0;
    virtual void setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds) = 0;
    
    // Monitoring control
    virtual void enableMonitoring(bool enable) = 0;
    virtual bool isMonitoringEnabled() const = 0;
    virtual void reset() = 0;
};

// ===============================================================
// PERFORMANCE MONITOR IMPLEMENTATION
// ===============================================================

class PerformanceMonitor : public IPerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() override;
    
    void startMeasurement() override;
    void stopMeasurement() override;
    PerformanceMetrics getCurrentMetrics() const override;
    std::vector<PerformanceSnapshot> getHistoricalSnapshots() const override;
    
    void takeSnapshot(const std::string& label = "") override;
    void restoreSnapshot(const std::string& label) override;
    void deleteSnapshot(const std::string& label) override;
    
    PerformanceMetrics analyzePerformance(size_t window = 0) const override;
    NeuralPerformanceMetrics analyzeNeuralPerformance(size_t window = 0) const override;
    MemorySystemMetrics analyzeMemoryPerformance(size_t window = 0) const override;
    NeuromodulationMetrics analyzeNeuromodulationPerformance(size_t window = 0) const override;
    DevelopmentMetrics analyzeDevelopmentPerformance(size_t window = 0) const override;
    ExperimentMetrics analyzeExperimentPerformance(size_t window = 0) const override;
    
    std::vector<double> analyzeTrends(const std::string& metricName, size_t window = 0) const override;
    double calculateTrend(const std::string& metricName, size_t window = 0) const override;
    std::string getPerformanceGrade() const override;
    
    std::vector<std::string> getOptimizationSuggestions() const override;
    double calculateEfficiency(const std::string& component = "") const override;
    double calculateStability(const std::string& component = "") const override;
    
    std::string exportMetrics(const std::string& format = "json") const override;
    bool importMetrics(const std::string& data, const std::string& format = "json") override;
    bool saveMetrics(const std::string& filepath) const override;
    bool loadMetrics(const std::string& filepath) override;
    
    void setMeasurementInterval(double interval) override;
    void setMaxHistorySize(size_t maxSize) override;
    void setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds) override;
    
    void enableMonitoring(bool enable) override;
    bool isMonitoringEnabled() const override;
    void reset() override;
    
    // Brain integration
    void attachBrain(std::shared_ptr<Brain> brain);
    void attachWorld(std::shared_ptr<SimpleWorld> world);
    void attachAgent(std::shared_ptr<AgentBrain> agent);
    
    // Event-based monitoring
    void onStepCompleted(size_t step, double stepTime);
    void onMemoryUpdate(size_t memoryUsage);
    void onNeuralEvent(const std::string& eventType, const std::unordered_map<std::string, double>& data);
    void onSystemEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data);
    
    // Configuration and setup
    void setupStandardMonitoring();
    void setupComprehensiveMonitoring();
    void setupMinimalMonitoring();
    
    // Report generation
    std::string generatePerformanceReport() const;
    std::string generateBenchmarkReport() const;
    std::string generateOptimizationReport() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void updateSystemMetrics();
    void updateNeuralMetrics();
    void updateMemoryMetrics();
    void updateNeuromodulationMetrics();
    void updateDevelopmentMetrics();
    void updateExperimentMetrics();
    
    double calculateCpuUsage() const;
    double calculateMemoryUsage() const;
    int calculateSystemLoad() const;
    size_t calculateActiveThreads() const;
    
    double calculateEfficiencyScore() const;
    double calculateStabilityScore() const;
    std::string determinePerformanceGrade() const;
    
    PerformanceMetrics calculateRollingAverage(size_t window) const;
    NeuralPerformanceMetrics calculateNeuralRollingAverage(size_t window) const;
    
    void exportToJson(std::ostream& out) const;
    void importFromJson(std::istream& in);
    
    void writeReport(const std::string& filename, const std::string& content) const;
};

// ===============================================================
// PERFORMANCE MONITOR FACTORY AND UTILITIES
// ===============================================================

class PerformanceMonitorFactory {
public:
    static std::shared_ptr<IPerformanceMonitor> createStandardMonitor();
    static std::shared_ptr<IPerformanceMonitor> createComprehensiveMonitor();
    static std::shared_ptr<IPerformanceMonitor> createBenchmarkMonitor();
    static std::shared_ptr<IPerformanceMonitor> createAnalysisMonitor();
    
    static void setupGlobalMonitor(std::shared_ptr<IPerformanceMonitor> monitor);
    static std::shared_ptr<IPerformanceMonitor> getGlobalMonitor();
    
    static std::string levelToString(double value);
    static std::string gradeToString(const std::string& grade);
    static double calculateGradeScore(double efficiency, double stability);
    static std::string getPerformanceDescription(const std::string& grade);
    
    static void setupDevelopmentPerformanceMonitoring();
    static void setupProductionPerformanceMonitoring();
    static void setupResearchPerformanceMonitoring();
    
    static std::string getSystemInfo();
    static std::vector<std::string> getAvailableMetrics();
    static bool isMetricAvailable(const std::string& metricName);
};

// ===============================================================
// PERFORMANCE ANALYSIS UTILITIES
// ===============================================================

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    static PerformanceMetrics analyzeTimeSeries(const std::vector<double>& data);
    static PerformanceMetrics analyzeMemoryTrace(const std::vector<size_t>& memoryData);
    static PerformanceMetrics analyzeSpikeTrain(const std::vector<size_t>& spikeTimes);
    
    static NeuralPerformanceMetrics analyzeNeuralActivity(const std::shared_ptr<Brain>& brain);
    static MemorySystemMetrics analyzeMemoryUsage(const std::shared_ptr<Brain>& brain);
    static NeuromodulationMetrics analyzeNeuromodulation(const std::shared_ptr<Brain>& brain);
    static DevelopmentMetrics analyzeDevelopment(const std::shared_ptr<Brain>& brain);
    
    static std::vector<double> detectAnomalies(const std::vector<double>& data, double threshold = 2.0);
    static std::vector<std::string> findPerformanceIssues(const PerformanceMetrics& metrics);
    static std::vector<std::string> findOptimizationOpportunities(const PerformanceMetrics& metrics);
    
    static std::string generatePerformanceInsight(const PerformanceMetrics& metrics);
    static std::string generateOptimizationRecommendation(const PerformanceMetrics& metrics);
    
    static double calculateTrend(const std::vector<double>& data, bool useLinearRegression = true);
    static double calculateVolatility(const std::vector<double>& data);
    static double calculateEfficiency(const PerformanceMetrics& metrics);
    static double calculateStability(const PerformanceMetrics& metrics);
    
    static std::unordered_map<std::string, double> extractKeyMetrics(const PerformanceMetrics& metrics);
    static PerformanceMetrics mergeMetrics(const std::vector<PerformanceMetrics>& metricsList);
    
    static std::string exportAnalysis(const PerformanceMetrics& metrics, const std::string& format = "json");
    static bool importAnalysis(const std::string& data, PerformanceMetrics& out, const std::string& format = "json");
    
    static void saveAnalysisReport(const PerformanceMetrics& metrics, const std::string& filename);
    static void saveBenchmarkReport(const PerformanceMetrics& baseline, const PerformanceMetrics& current, const std::string& filename);
    
    static std::string getAnalysisSummary(const PerformanceMetrics& metrics);
    static std::string getComparisonReport(const PerformanceMetrics& baseline, const PerformanceMetrics& current);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static double calculateMean(const std::vector<double>& data);
    static double calculateStdDev(const std::vector<double>& data, double mean = 0.0);
    static double calculateMedian(const std::vector<double>& data);
    static double calculatePercentile(const std::vector<double>& data, double percentile);
    
    static size_t findOutliers(const std::vector<double>& data, double threshold);
    static std::vector<size_t> findPeaks(const std::vector<double>& data, size_t minPeakDistance = 1);
    static double calculateKurtosis(const std::vector<double>& data);
    static double calculateSkewness(const std::vector<double>& data);
    
    static std::string formatTime(double seconds);
    static std::string formatMemory(size_t bytes);
    static std::string formatPercentage(double value);
    static std::string formatEfficiency(double efficiency);
    
    static void normalizeMetrics(std::unordered_map<std::string, double>& metrics);
    static void scaleMetrics(std::unordered_map<std::string, double>& metrics, double scaleFactor);
    static void smoothMetrics(std::unordered_map<std::string, double>& metrics, size_t windowSize);
    
    static double exponentialMovingAverage(const std::vector<double>& data, double alpha = 0.3);
    static double movingAverage(const std::vector<double>& data, size_t window);
    static double weightedMovingAverage(const std::vector<double>& data, size_t window);
    
    static std::vector<double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    static double correlationCoefficient(const std::vector<double>& x, const std::vector<double>& y);
    static double covariance(const std::vector<double>& x, const std::vector<double>& y);
    
    static std::string createJsonReport(const std::unordered_map<std::string, double>& metrics);
    static bool parseJsonReport(const std::string& jsonData, std::unordered_map<std::string, double>& outMetrics);
    
    static std::string createTextReport(const PerformanceMetrics& metrics);
    static bool parseTextReport(const std::string& textReport, PerformanceMetrics& outMetrics);
    
    static std::string createHtmlReport(const PerformanceMetrics& metrics);
    static bool parseHtmlReport(const std::string& htmlReport, PerformanceMetrics& outMetrics);
};

// ===============================================================
// PERFORMANCE MONITORING MACROS AND STREAMS
// ===============================================================

// Performance monitoring macros
#define NLM_PERF_START() nlm::PerformanceMonitor::getGlobalMonitor()->startMeasurement()
#define NLM_PERF_STOP() nlm::PerformanceMonitor::getGlobalMonitor()->stopMeasurement()
#define NLM_PERF_TAKE_SNAPSHOT(label) nlm::PerformanceMonitor::getGlobalMonitor()->takeSnapshot(label)
#define NLM_PERF_GET_METRICS() nlm::PerformanceMonitor::getGlobalMonitor()->getCurrentMetrics()
#define NLM_PERF_ENABLE_MONITORING(enable) nlm::PerformanceMonitor::getGlobalMonitor()->enableMonitoring(enable)
#define NLM_PERF_ANALYZE() nlm::PerformanceMonitor::getGlobalMonitor()->analyzePerformance()

// Performance analysis macros
#define NLM_PERF_ANALYZE_TIME() nlm::PerformanceAnalyzer::analyzeTimeSeries(std::vector<double>(/* time data */))
#define NLM_PERF_ANALYZE_MEMORY() nlm::PerformanceAnalyzer::analyzeMemoryTrace(std::vector<size_t>(/* memory data */))
#define NLM_PERF_ANALYZE_SPIKES() nlm::PerformanceAnalyzer::analyzeSpikeTrain(std::vector<size_t>(/* spike data */))

// Performance stream for detailed logging
class PerformanceStream {
public:
    PerformanceStream(const std::string& operation, std::shared_ptr<IPerformanceMonitor> monitor);
    ~PerformanceStream();
    
    template<typename T>
    PerformanceStream& operator<<(const T& value) {
        data_ << value;
        return *this;
    }
    
    PerformanceStream& withMetric(const std::string& name, double value);
    PerformanceStream& withMemory(size_t bytes);
    PerformanceStream& withCpu(double usage);
    PerformanceStream& withSystemLoad(int load);
    
    std::string getData() const;

private:
    std::string operation_;
    std::shared_ptr<IPerformanceMonitor> monitor_;
    std::ostringstream data_;
    
    std::unordered_map<std::string, double> metrics_;
};

// Performance monitoring stream macro
#define NLM_PERF_STREAM(operation) nlm::PerformanceStream operation##_stream(operation, nlm::PerformanceMonitorFactory::getGlobalMonitor())

// ===============================================================
// PERFORMANCE MONITORING HELP AND UTILITIES
// ===============================================================

void showPerformanceHelp();
void showPerformanceSetupGuide();
void showPerformanceAnalysisGuide();
void showPerformanceBenchmarkingGuide();

} // namespace nlm

// Include implementation header
#include "PerformanceMonitor.ipp"  
