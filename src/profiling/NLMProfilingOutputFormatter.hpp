#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

// NLM profiling output formatters
class NLMProfilingOutputFormatter {
public:
    // Format profiling data as text report
    static std::string formatTextReport(const std::string& title, const std::string& data);
    
    // Format profiling data as HTML dashboard
    static std::string formatHTMLDashboard(const std::string& title, const std::string& data,
                                          const std::vector<std::pair<std::string, double>>& metrics = {});
    
    // Format profiling data as CSV
    static std::string formatCSVReport(const std::string& title, const std::string& data,
                                       const std::vector<std::pair<std::string, double>>& metrics = {});
    
    // Format profiling data as JSON
    static std::string formatJSONReport(const std::string& title, const std::string& data,
                                        const std::vector<std::pair<std::string, double>>& metrics = {});
    
    // Generate HTML visualization
    static std::string generateHTMLVisualization(const std::string& title,
                                                 const std::vector<std::pair<std::string, double>>& metrics,
                                                 const std::vector<std::string>& snapshots);
    
    // Generate CSV chart data
    static std::string generateCSVChartData(const std::string& title,
                                           const std::vector<std::pair<std::string, double>>& metrics);
    
    // Generate JSON profile data
    static std::string generateJSONProfile(const std::string& title,
                                          const std::vector<std::pair<std::string, double>>& metrics,
                                          const std::vector<std::string>& snapshots,
                                          const std::string& metadata = "");
    
private:
    // Helper functions
    static std::string escapeJSON(const std::string& input);
    static std::string escapeHTML(const std::string& input);
    static std::string generateProgressBar(double value, int width = 50);
    static std::string formatTime(double seconds);
    static std::string formatMemory(size_t bytes);
};

// Neural network simulation profiler
class NeuralSimulationProfiler {
public:
    NeuralSimulationProfiler(const std::string& simulationName);
    
    // Start profiling simulation
    void startProfiling();
    
    // Stop profiling simulation
    void stopProfiling();
    
    // Record simulation step timing
    void recordStepTiming(double stepTime, size_t neuronCount, size_t synapseCount);
    
    // Record neuron processing timing
    void recordNeuronProcessingTime(double time, size_t neuronId);
    
    // Record synapse processing timing
    void recordSynapseProcessingTime(double time, size_t synapseId);
    
    // Record memory usage during simulation
    void recordMemoryUsage(size_t memoryUsage, const std::string& context);
    
    // Record CPU usage during simulation
    void recordCPUUsage(double cpuUsage);
    
    // Get profiling statistics
    struct SimulationStats {
        std::string name;
        double totalSimulationTime;
        double averageStepTime;
        size_t totalSteps;
        size_t totalNeuronUpdates;
        size_t totalSynapseUpdates;
        size_t peakMemoryUsage;
        double averageCPUUsage;
        double peakCPUUsage;
        size_t currentNeuronCount;
        size_t currentSynapseCount;
        
        SimulationStats(const std::string& name) 
            : name(name), totalSimulationTime(0.0), averageStepTime(0.0),
              totalSteps(0), totalNeuronUpdates(0), totalSynapseUpdates(0),
              peakMemoryUsage(0), averageCPUUsage(0.0), peakCPUUsage(0.0),
              currentNeuronCount(0), currentSynapseCount(0) {}
    };
    
    SimulationStats getSimulationStats() const;
    
    // Export profiling data
    std::string exportProfilingData() const;
    
    // Generate profiling report
    std::string generateProfilingReport() const;
    
    // Export to different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
private:
    std::string simulationName_;
    bool isProfiling_;
    double totalSimulationTime_;
    double totalStepTime_;
    size_t totalSteps_;
    size_t totalNeuronUpdates_;
    size_t totalSynapseUpdates_;
    size_t peakMemoryUsage_;
    double totalCPUUsage_;
    double peakCPUUsage_;
    std::chrono::system_clock::time_point startTime_;
    
    // Per-step data
    std::vector<double> stepTimes_;
    std::vector<size_t> neuronCounts_;
    std::vector<size_t> synapseCounts_;
    std::vector<size_t> memoryUsages_;
    std::vector<double> cpuUsages_;
    
    mutable std::mutex mutex_;
};

// System resource profiler
class SystemResourceProfiler {
public:
    SystemResourceProfiler();
    
    // Start profiling system resources
    void startProfiling(const std::string& context = "");
    
    // Stop profiling system resources
    void stopProfiling();
    
    // Record system resource usage
    void recordResourceUsage(double cpuUsage, size_t memoryUsage,
                            uint64_t networkBytes, uint64_t diskBytes,
                            const std::string& context = "");
    
    // Record thread information
    void recordThreadInfo(const std::string& threadName, size_t threadId,
                         double cpuTime, size_t memoryUsage);
    
    // Get system profiling statistics
    struct SystemStats {
        std::string context;
        double averageCPUUsage;
        size_t averageMemoryUsage;
        uint64_t averageNetworkBytes;
        uint64_t averageDiskBytes;
        size_t totalThreadCount;
        double totalCPUUsage;
        size_t peakMemoryUsage;
        uint64_t totalNetworkBytes;
        uint64_t totalDiskBytes;
        
        SystemStats(const std::string& ctx) 
            : context(ctx), averageCPUUsage(0.0), averageMemoryUsage(0),
              averageNetworkBytes(0), averageDiskBytes(0), totalThreadCount(0),
              totalCPUUsage(0.0), peakMemoryUsage(0), totalNetworkBytes(0),
              totalDiskBytes(0) {}
    };
    
    SystemStats getSystemStats() const;
    
    // Export system profiling data
    std::string exportSystemProfilingData() const;
    
    // Generate system profiling report
    std::string generateSystemProfilingReport() const;
    
    // Export to different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
private:
    std::string context_;
    bool isProfiling_;
    double totalCPUUsage_;
    size_t totalMemoryUsage_;
    uint64_t totalNetworkBytes_;
    uint64_t totalDiskBytes_;
    size_t totalThreadCount_;
    size_t peakMemoryUsage_;
    
    // Per-sample data
    std::vector<double> cpuUsages_;
    std::vector<size_t> memoryUsages_;
    std::vector<uint64_t> networkBytesList_;
    std::vector<uint64_t> diskBytesList_;
    
    // Thread statistics
    struct ThreadStat {
        std::string name;
        size_t id;
        double cpuTime;
        size_t memoryUsage;
        
        ThreadStat(const std::string& n, size_t i, double cpu, size_t mem)
            : name(n), id(i), cpuTime(cpu), memoryUsage(mem) {}
    };
    
    std::vector<ThreadStat> threadStats_;
    
    mutable std::mutex mutex_;
    std::chrono::system_clock::time_point startTime_;
};

// Memory leak detection with advanced analysis
class AdvancedMemoryLeakDetector {
public:
    AdvancedMemoryLeakDetector();
    
    // Track allocation
    void trackAllocation(void* pointer, size_t size, const std::string& context,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    // Track deallocation
    void trackDeallocation(void* pointer, const std::string& context);
    
    // Get memory leak analysis
    struct MemoryLeakAnalysis {
        size_t totalLeaks;
        size_t totalLeakedMemory;
        std::string mostCommonContext;
        std::string mostCommonFile;
        int mostCommonLine;
        std::string mostCommonFunction;
        std::vector<std::pair<void*, size_t>> leaks;
        std::unordered_map<std::string, size_t> contextCounts;
        std::unordered_map<std::string, size_t> fileCounts;
        
        MemoryLeakAnalysis() : totalLeaks(0), totalLeakedMemory(0), mostCommonLine(0) {}
    };
    
    MemoryLeakAnalysis analyzeLeaks() const;
    
    // Check for specific types of leaks
    bool checkForBufferOverflows() const;
    bool checkForUseAfterFree() const;
    bool checkForDoubleFree() const;
    
    // Export leak analysis
    std::string exportLeakAnalysis() const;
    
    // Generate leak report
    std::string generateLeakReport() const;
    
    // Export to different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
private:
    struct AllocationInfo {
        void* pointer;
        size_t size;
        std::string context;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        bool isFreed;
        
        AllocationInfo(void* ptr, size_t sz, const std::string& ctx,
                      const std::string& f = "", int ln = 0,
                      const std::string& func = "")
            : pointer(ptr), size(sz), context(ctx), file(f), line(ln),
              function(func), timestamp(std::chrono::system_clock::now()),
              isFreed(false) {}
    };
    
    std::vector<AllocationInfo> allocations_;
    mutable std::mutex mutex_;
};

// Enhanced logging manager
class EnhancedLoggingManager {
public:
    EnhancedLoggingManager();
    
    // Log system event
    void logSystemEvent(const std::string& event, const std::string& severity,
                       const std::string& context = "");
    
    // Log neural simulation event
    void logNeuralSimulationEvent(const std::string& event, const std::string& details,
                                const std::string& context = "");
    
    // Log memory event
    void logMemoryEvent(const std::string& event, size_t size,
                       const std::string& context = "");
    
    // Log performance event
    void logPerformanceEvent(const std::string& event, double value,
                            const std::string& context = "");
    
    // Log thread event
    void logThreadEvent(const std::string& threadId, const std::string& event,
                       const std::string& context = "");
    
    // Get logging statistics
    struct LoggingStats {
        size_t totalSystemEvents;
        size_t totalNeuralEvents;
        size_t totalMemoryEvents;
        size_t totalPerformanceEvents;
        size_t totalThreadEvents;
        std::chrono::system_clock::time_point sessionStartTime;
        
        LoggingStats() : totalSystemEvents(0), totalNeuralEvents(0), totalMemoryEvents(0),
                        totalPerformanceEvents(0), totalThreadEvents(0) {
            sessionStartTime = std::chrono::system_clock::now();
        }
    };
    
    LoggingStats getLoggingStats() const;
    
    // Export logging data
    std::string exportLoggingData() const;
    
    // Generate logging report
    std::string generateLoggingReport() const;
    
    // Export to different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
private:
    mutable std::mutex mutex_;
    LoggingStats stats_;
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string category;
        std::string event;
        std::string severity;
        std::string context;
        std::string details;
        
        LogEntry(const std::string& cat, const std::string& evt,
                const std::string& sev, const std::string& ctx,
                const std::string& det)
            : category(cat), event(evt), severity(sev), context(ctx),
              details(det), timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<LogEntry> logEntries_;
};

// Main profiling and logging interface
class NLMProfiling {
public:
    static NLMProfiling& getInstance();
    
    // Set up profiling
    void setupProfiling(const std::string& outputDirectory = "./profiles");
    
    // Start neural simulation profiling
    void startNeuralSimulation(const std::string& simulationName);
    
    // Stop neural simulation profiling
    void stopNeuralSimulation();
    
    // Start system resource profiling
    void startSystemResourceProfiling(const std::string& context = "");
    
    // Stop system resource profiling
    void stopSystemResourceProfiling();
    
    // Record memory allocation
    void recordMemoryAllocation(void* pointer, size_t size, const std::string& context,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    // Record memory deallocation
    void recordMemoryDeallocation(void* pointer, const std::string& context);
    
    // Export all profiling data
    std::string exportAllProfilingData() const;
    
    // Generate comprehensive profiling report
    std::string generateComprehensiveReport() const;
    
    // Export to different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
    // Get current profiling status
    bool isNeuralSimulationProfiling() const { return neuralProfiler_ && neuralProfiler_->isProfiling(); }
    bool isSystemResourceProfiling() const { return systemProfiler_ && systemProfiler_->isProfiling(); }
    
private:
    NLMProfiling() = default;
    std::unique_ptr<NeuralSimulationProfiler> neuralProfiler_;
    std::unique_ptr<SystemResourceProfiler> systemProfiler_;
    std::unique_ptr<AdvancedMemoryLeakDetector> memoryLeakDetector_;
    std::unique_ptr<EnhancedLoggingManager> loggingManager_;
    std::string outputDirectory_;
    mutable std::mutex mutex_;
};

} // namespace nlm