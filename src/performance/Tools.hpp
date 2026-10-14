// Performance Tools and Utilities
// Comprehensive benchmarking and profiling tools

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace nlm {

class BenchmarkRunner {
public:
    static BenchmarkRunner& instance() {
        static BenchmarkRunner runner;
        return runner;
    }
    
    void registerBenchmark(const std::string& name, std::function<double()> benchmarkFunc,
                           const std::string& category = "general",
                           const std::unordered_map<std::string, std::string>& params = {});
    
    double runBenchmark(const std::string& name, const std::string& paramSet = "default");
    std::vector<std::pair<std::string, double>> runBenchmarks(const std::vector<std::string>& names,
                                                              const std::string& paramSet = "default");
    
    void setIterations(size_t iterations) { defaultIterations_ = iterations; }
    void setWarmupIterations(size_t iterations) { warmupIterations_ = iterations; }
    void enableProfiler(bool enabled) { profilerEnabled_ = enabled; }
    
    void exportResults(const std::string& filename, const std::string& format = "csv");
    void generateReport(const std::string& filename, const std::string& format = "html");
    
    struct BenchmarkResult {
        std::string name;
        std::string category;
        std::unordered_map<std::string, std::string> parameters;
        double meanTime;
        double stdDevTime;
        double minTime;
        double maxTime;
        double memoryUsageMB;
        double cpuUsagePercent;
        size_t iterations;
        std::vector<double> times;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
    };
    
private:
    BenchmarkRunner() : defaultIterations_(100), warmupIterations_(10), profilerEnabled_(false) {}
    
    std::unordered_map<std::string, std::vector<BenchmarkResult>> benchmarkRegistry_;
    size_t defaultIterations_;
    size_t warmupIterations_;
    bool profilerEnabled_;
    std::unordered_map<std::string, BenchmarkResult> currentResults_;
};

class ProfilerViewer {
public:
    static ProfilerViewer& instance() {
        static ProfilerViewer viewer;
        return viewer;
    }
    
    void loadProfile(const std::string& filename);
    void displayProfileSummary() const;
    void displayTimeline() const;
    void displayMemoryUsage() const;
    void displayCPUTimeline() const;
    void exportProfile(const std::string& filename, const std::string& format = "json");
    
    void setFilter(const std::string& category, double minTime = 0.0);
    void clearFilter() { categoryFilter_.clear(); minTimeFilter_ = 0.0; }
    
    struct ProfileData {
        std::string sessionName;
        double startTime;
        double endTime;
        std::vector<TimelineEvent> timelineEvents;
        std::vector<MemorySample> memorySamples;
        std::vector<CPUSample> cpuSamples;
        std::unordered_map<std::string, double> customMetrics;
    };
    
private:
    ProfilerViewer() {}
    std::vector<ProfileData> profiles_;
    std::string categoryFilter_;
    double minTimeFilter_;
};

class MemoryAnalyzer {
public:
    static MemoryAnalyzer& instance() {
        static MemoryAnalyzer analyzer;
        return analyzer;
    }
    
    struct LeakInfo {
        size_t address;
        size_t size;
        std::string type;
        std::string allocationStack;
    };
    
    struct AllocationInfo {
        size_t address;
        size_t size;
        std::string type;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string allocationStack;
    };
    
    void startTracking(const std::string& sessionName = "default");
    void stopTracking();
    void recordAllocation(size_t address, size_t size, const std::string& type,
                         const std::string& stack = "");
    void recordDeallocation(size_t address);
    
    std::vector<LeakInfo> detectLeaks() const;
    double getCurrentMemoryUsageMB() const;
    double getPeakMemoryUsageMB() const;
    
    void exportLeaks(const std::string& filename) const;
    void generateLeakReport(const std::string& filename) const;
    
    void setTrackingEnabled(bool enabled) { trackingEnabled_ = enabled; }
    
private:
    MemoryAnalyzer() : trackingEnabled_(false) {}
    
    bool trackingEnabled_;
    std::string currentSession_;
    std::vector<AllocationInfo> allocations_;
    std::unordered_set<size_t> allocatedAddresses_;
};

class CPUMemoryProfiler {
public:
    static CPUMemoryProfiler& instance() {
        static CPUMemoryProfiler profiler;
        return profiler;
    }
    
    struct ProfilingResult {
        double cpuTime;
        double memoryTime;
        double totalTime;
        double efficiencyScore;
        std::vector<double> cpuUsageTimeline;
        std::vector<double> memoryUsageTimeline;
        std::vector<double> combinedTimeline;
    };
    
    ProfilingResult profileOperation(const std::function<void()>& operation,
                                     const std::string& operationName = "operation",
                                     bool measureMemory = true);
    
    void startProfilingSession(const std::string& sessionName);
    void endProfilingSession();
    
    std::vector<ProfilingResult> getSessionResults() const { return sessionResults_; }
    
    void exportResults(const std::string& filename, const std::string& format = "csv") const;
    void generatePerformanceReport(const std::string& filename) const;
    
private:
    CPUMemoryProfiler() {}
    
    std::vector<ProfilingResult> sessionResults_;
    std::string currentSession_;
};

class PerformanceComparator {
public:
    static PerformanceComparator& instance() {
        static PerformanceComparator comparator;
        return comparator;
    }
    
    struct ComparisonResult {
        std::string testName;
        std::string baselineName;
        std::string currentName;
        
        double baselineTime;
        double currentTime;
        double timeRatio;
        bool isRegression;
        double confidence;
        
        double baselineMemory;
        double currentMemory;
        double memoryRatio;
        
        double baselineCPU;
        double currentCPU;
        double cpuRatio;
        
        std::string regressionThreshold;
    };
    
    void addBaseline(const std::string& name, const std::vector<BenchmarkRunner::BenchmarkResult>& results);
    void addCurrent(const std::string& name, const std::vector<BenchmarkRunner::BenchmarkResult>& results);
    
    std::vector<ComparisonResult> compareAll() const;
    std::vector<ComparisonResult> compare(const std::string& baselineName, 
                                          const std::string& currentName) const;
    
    void exportComparison(const std::string& filename, const std::string& format = "csv") const;
    void generateRegressionReport(const std::string& filename) const;
    
    void setRegressionThreshold(double timeThresholdPercent) { 
        timeThresholdPercent_ = timeThresholdPercent; 
    }
    
private:
    PerformanceComparator() : timeThresholdPercent_(10.0) {}  // 10% default threshold
    
    std::unordered_map<std::string, std::vector<BenchmarkRunner::BenchmarkResult>> baselineSuites_;
    std::unordered_map<std::string, std::vector<BenchmarkRunner::BenchmarkResult>> currentSuites_;
    double timeThresholdPercent_;
};

class OutputFormatter {
public:
    static OutputFormatter& instance() {
        static OutputFormatter formatter;
        return formatter;
    }
    
    std::string formatBenchmarkResult(const BenchmarkRunner::BenchmarkResult& result,
                                      const std::string& format = "table");
    
    std::string formatProfilerOutput(const std::string& sessionName,
                                     const ProfilerViewer::ProfileData& profile,
                                     const std::string& format = "text");
    
    std::string formatComparisonResult(const PerformanceComparator::ComparisonResult& result,
                                       const std::string& format = "text");
    
    void writeToFile(const std::string& filename, const std::string& content);
    void appendToFile(const std::string& filename, const std::string& content);
    
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSV(const std::string& text) const;
    
    // Export formats
    enum class Format {
        TEXT,
        CSV,
        JSON,
        HTML
    };
};

class BenchmarkSuite {
public:
    static BenchmarkSuite& instance() {
        static BenchmarkSuite suite;
        return suite;
    }
    
    void addBenchmark(const std::string& name, std::function<double()> benchmark,
                     const std::string& category = "general",
                     const std::unordered_map<std::string, std::string>& parameters = {});
    
    void runParameterizedBenchmarks(const std::unordered_map<std::string, std::vector<std::string>>& paramGrid);
    
    std::vector<BenchmarkRunner::BenchmarkResult> runAllBenchmarks();
    std::vector<BenchmarkRunner::BenchmarkResult> runBenchmarksByCategory(const std::string& category);
    
    void saveBaseline(const std::string& filename);
    void loadBaseline(const std::string& filename);
    
    void detectRegressions(const std::string& baselineFile = "baseline.json",
                          const std::string& outputFile = "regression_report.json");
    
    std::vector<PerformanceComparator::ComparisonResult> getRegressionReport() const;
    
    void generateComprehensiveReport(const std::string& filename, 
                                    const std::string& format = "html");
    
private:
    BenchmarkSuite() {}
    
    BenchmarkRunner benchmarkRunner_;
    PerformanceComparator comparator_;
};

} // namespace nlm