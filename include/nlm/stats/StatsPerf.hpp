#ifndef NLM_STATS_PERF_HPP
#define NLM_STATS_PERF_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <memory>
#include <functional>

namespace nlm {
namespace stats {

// Performance measurement types
enum class PerformanceMetric {
    SimulationTime,
    StepTime,
    MemoryUsage,
    CPUUsage,
    NetworkTraffic,
    FileIO,
    SpikeCount,
    FiringRate,
    SynapticActivity,
    PlasticityUpdateTime,
    CheckpointTime,
    ReplayTime,
    Custom
};

// Statistics collection types
enum class StatisticsType {
    Counter,
    Gauge,
    Histogram,
    Summary,
    Timer,
    Distribution
};

// Performance sample
struct PerformanceSample {
    std::chrono::system_clock::time_point timestamp;
    double value;
    std::map<std::string, std::string> tags;
    std::string description;
    
    PerformanceSample() : value(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    PerformanceSample(double val, const std::string& desc = "") 
        : value(val), description(desc) {
        timestamp = std::chrono::system_clock::now();
    }
    
    PerformanceSample(double val, const std::map<std::string, std::string>& tagMap, const std::string& desc = "")
        : value(val), tags(tagMap), description(desc) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Statistics data point
struct StatisticsDataPoint {
    std::string name;
    StatisticsType type;
    std::chrono::system_clock::time_point timestamp;
    double value;
    std::map<std::string, std::string> tags;
    std::string unit;
    std::string description;
    
    StatisticsDataPoint() : value(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Histogram data structure
class Histogram {
public:
    struct Bucket {
        double lowerBound;
        double upperBound;
        size_t count;
        double sum;
        double sumOfSquares;
        
        Bucket() : lowerBound(0.0), upperBound(0.0), count(0), sum(0.0), sumOfSquares(0.0) {}
        Bucket(double lower, double upper) : lowerBound(lower), upperBound(upper), count(0), sum(0.0), sumOfSquares(0.0) {}
    };
    
    Histogram() : minValue_(0.0), maxValue_(0.0), totalCount_(0), totalSum_(0.0) {}
    explicit Histogram(size_t bucketCount, double minValue = 0.0, double maxValue = 1.0);
    
    void addValue(double value);
    double getCount() const { return totalCount_; }
    double getSum() const { return totalSum_; }
    double getMean() const;
    double getVariance() const;
    double getStandardDeviation() const;
    double getMin() const { return minValue_; }
    double getMax() const { return maxValue_; }
    double getPercentile(double percentile) const;
    const std::vector<Bucket>& getBuckets() const { return buckets_; }
    
    // Bucket management
    void setBucketCount(size_t count);
    void setRange(double minValue, double maxValue);
    
private:
    std::vector<Bucket> buckets_;
    double minValue_;
    double maxValue_;
    size_t totalCount_;
    double totalSum_;
    double totalSumOfSquares_;
    
    size_t findBucketIndex(double value) const;
};

// Statistics collector
class StatisticsCollector {
public:
    StatisticsCollector() = default;
    ~StatisticsCollector() = default;
    
    // Statistics recording
    void recordCounter(const std::string& name, int64_t value, 
                      const std::map<std::string, std::string>& tags = {});
    void recordGauge(const std::string& name, double value,
                    const std::map<std::string, std::string>& tags = {});
    void recordHistogram(const std::string& name, double value,
                        const std::map<std::string, std::string>& tags = {});
    void recordTimer(const std::string& name, std::chrono::duration<double> duration,
                    const std::map<std::string, std::string>& tags = {});
    void recordDistribution(const std::string& name, const std::vector<double>& values,
                           const std::map<std::string, std::string>& tags = {});
    
    // Statistics retrieval
    int64_t getCounter(const std::string& name, const std::map<std::string, std::string>& filters = {}) const;
    double getGauge(const std::string& name, const std::map<std::string, std::string>& filters = {}) const;
    Histogram getHistogram(const std::string& name, const std::map<std::string, std::string>& filters = {}) const;
    double getTimerAverage(const std::string& name, const std::map<std::string, std::string>& filters = {}) const;
    std::vector<double> getDistribution(const std::string& name, 
                                       const std::map<std::string, std::string>& filters = {}) const;
    
    // Statistics management
    void clear();
    std::vector<std::string> getMetricNames() const;
    std::vector<std::map<std::string, std::string>> getTagCombinations(const std::string& metricName) const;
    
    // Statistics analysis
    std::map<std::string, double> calculateAggregates(const std::string& metricName) const;
    std::string generateReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
    
private:
    struct MetricData {
        std::string name;
        StatisticsType type;
        std::vector<StatisticsDataPoint> dataPoints;
    };
    
    std::map<std::string, MetricData> metrics_;
    mutable std::mutex dataMutex_;
    
    MetricData* getOrCreateMetric(const std::string& name, StatisticsType type);
    std::vector<const StatisticsDataPoint*> filterDataPoints(const MetricData& metric,
                                                            const std::map<std::string, std::string>& filters) const;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    // Profile recording
    void startProfile(const std::string& operationName,
                     const std::map<std::string, std::string>& tags = {});
    void stopProfile(const std::string& operationName);
    void addSample(const std::string& operationName, double value,
                  const std::map<std::string, std::string>& tags = {});
    
    // Profile retrieval
    double getOperationTime(const std::string& operationName) const;
    double getOperationAverageTime(const std::string& operationName) const;
    std::vector<double> getOperationTimes(const std::string& operationName) const;
    
    // Profile management
    void clear();
    std::vector<std::string> getOperationNames() const;
    
    // Profile analysis
    std::string generateProfileReport() const;
    std::map<std::string, double> getPerformanceMetrics() const;
    
private:
    struct ProfileData {
        std::string name;
        std::vector<double> times;
        std::vector<PerformanceSample> samples;
        std::chrono::system_clock::time_point startTime;
        bool isRunning;
    };
    
    std::map<std::string, ProfileData> profiles_;
    mutable std::mutex profileMutex_;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler() = default;
    ~MemoryProfiler() = default;
    
    // Memory tracking
    void recordAllocation(size_t size, const std::string& location = "");
    void recordDeallocation(size_t size, const std::string& location = "");
    void recordMemoryAccess(const std::string& threadId, size_t address, size_t size);
    
    // Memory statistics
    size_t getTotalAllocated() const;
    size_t getCurrentAllocated() const;
    size_t getPeakAllocated() const;
    size_t getTotalDeallocated() const;
    
    // Memory analysis
    std::vector<std::string> getMemoryLeaks() const;
    std::map<std::string, size_t> getAllocationBreakdown() const;
    std::string generateMemoryReport() const;
    
    // Memory tracking control
    void enableTracking(bool enable);
    bool isTrackingEnabled() const;
    
private:
    struct AllocationRecord {
        std::chrono::system_clock::time_point timestamp;
        size_t size;
        std::string location;
        std::thread::id threadId;
    };
    
    std::vector<AllocationRecord> allocations_;
    std::vector<AllocationRecord> deallocations_;
    mutable std::mutex memoryMutex_;
    bool trackingEnabled_;
    
    size_t calculateCurrentAllocated() const;
};

// Real-time statistics collector
class RealTimeStatisticsCollector {
public:
    RealTimeStatisticsCollector() = default;
    ~RealTimeStatisticsCollector() = default;
    
    // Real-time data collection
    void collectSystemMetrics();
    void collectSimulationMetrics(const std::map<std::string, std::any>& simulationState);
    void collectNetworkMetrics();
    void collectFileIOMetrics();
    
    // Data retrieval
    std::map<std::string, double> getSystemMetrics() const;
    std::map<std::string, double> getSimulationMetrics() const;
    std::map<std::string, double> getNetworkMetrics() const;
    std::map<std::string, double> getFileIOMetrics() const;
    
    // Monitoring control
    void startMonitoring(const std::chrono::milliseconds& interval);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Monitoring history
    std::vector<std::chrono::system_clock::time_point> getTimeSeries() const;
    std::map<std::string, std::vector<double>> getMetricTimeSeries(const std::string& metricName) const;
    
private:
    struct SystemMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t memoryTotal;
        double networkRx;
        double networkTx;
        size_t diskReads;
        size_t diskWrites;
    };
    
    std::map<std::string, double> systemMetrics_;
    std::map<std::string, double> simulationMetrics_;
    std::map<std::string, double> networkMetrics_;
    std::map<std::string, double> fileIOMetrics_;
    mutable std::mutex metricsMutex_;
    
    std::chrono::milliseconds monitoringInterval_;
    bool monitoringEnabled_;
    std::thread monitoringThread_;
    std::vector<std::chrono::system_clock::time_point> timePoints_;
    
    void monitoringWorker();
};

// Statistics exporter
class StatisticsExporter {
public:
    virtual ~StatisticsExporter() = default;
    
    virtual std::string getExportFormat() const = 0;
    virtual bool exportToFile(const std::string& filepath, 
                            const StatisticsCollector& stats) = 0;
    virtual bool exportToStream(std::ostream& stream, 
                              const StatisticsCollector& stats) = 0;
    virtual std::string exportToString(const StatisticsCollector& stats) = 0;
};

// JSON statistics exporter
class JSONStatisticsExporter : public StatisticsExporter {
public:
    std::string getExportFormat() const override { return "JSON"; }
    bool exportToFile(const std::string& filepath, const StatisticsCollector& stats) override;
    bool exportToStream(std::ostream& stream, const StatisticsCollector& stats) override;
    std::string exportToString(const StatisticsCollector& stats) override;
};

// CSV statistics exporter
class CSVStatisticsExporter : public StatisticsExporter {
public:
    std::string getExportFormat() const override { return "CSV"; }
    bool exportToFile(const std::string& filepath, const StatisticsCollector& stats) override;
    bool exportToStream(std::ostream& stream, const StatisticsCollector& stats) override;
    std::string exportToString(const StatisticsCollector& stats) override;
};

// Metrics registry
class MetricsRegistry {
public:
    MetricsRegistry() = default;
    ~MetricsRegistry() = default;
    
    // Metric registration
    void registerMetric(const std::string& name, StatisticsType type,
                       const std::string& description = "",
                       const std::string& unit = "");
    bool hasMetric(const std::string& name) const;
    
    // Metric metadata
    StatisticsType getMetricType(const std::string& name) const;
    std::string getMetricDescription(const std::string& name) const;
    std::string getMetricUnit(const std::string& name) const;
    std::vector<std::string> getAllMetricNames() const;
    
    // Metric validation
    bool validateMetricName(const std::string& name) const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct MetricInfo {
        std::string name;
        StatisticsType type;
        std::string description;
        std::string unit;
    };
    
    std::map<std::string, MetricInfo> metrics_;
    std::vector<std::string> validationErrors_;
};

// Statistics utilities
namespace utils {
    std::string formatTime(std::chrono::duration<double> duration);
    std::string formatBytes(size_t bytes);
    std::string formatPercentage(double value);
    std::string formatNumber(double value);
    
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    
    double calculateMean(const std::vector<double>& values);
    double calculateVariance(const std::vector<double>& values);
    double calculateStandardDeviation(const std::vector<double>& values);
    double calculatePercentile(const std::vector<double>& values, double percentile);
    
    std::string generateUUID();
    std::string generateTimestamp();
    std::string generateHash(const std::string& input);
    
    bool isFileAccessible(const std::string& path, std::ios_base::openmode mode = std::ios_base::in);
    std::string getFileSize(const std::string& path);
    
    std::string escapeJSON(const std::string& input);
    std::string escapeCSV(const std::string& input);
}

} // namespace stats
} // namespace nlm

#endif // NLM_STATS_PERF_HPP