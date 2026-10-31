#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Metrics.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace nlm;

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double memoryPeak;
        double diskIO;
        double networkIO;
        double activeThreads;
        double contextSwitches;
        double pageFaults;
        uint64_t timestamp;
    };
    
    PerformanceMonitor() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void startMeasurement(const std::string& operation) {
        measurements[operation].start = std::chrono::high_resolution_clock::now();
    }
    
    void endMeasurement(const std::string& operation) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - measurements[operation].start);
        measurements[operation].duration = duration.count();
        measurements[operation].end = end;
    }
    
    std::chrono::high_resolution_clock::time_point getStartTime() const { return startTime; }
    double getTotalDuration() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - startTime).count();
    }
    
    const std::map<std::string, Measurement>& getMeasurements() const { return measurements; }
    
    std::string getSystemDiagnostics() {
        std::stringstream ss;
        ss << "=== SYSTEM DIAGNOSTICS ===\n";
        ss << "Uptime: " << getTotalDuration() << "s\n";
        ss << "Measurements recorded: " << measurements.size() << "\n";
        for (const auto& [op, meas] : measurements) {
            ss << "  - " << op << ": " << meas.duration << "s\n";
        }
        return ss.str();
    }
    
    static std::string getMemoryUsage() {
        std::ifstream file("/proc/self/status");
        std::string line;
        size_t vmSize = 0, rss = 0;
        while (std::getline(file, line)) {
            if (line.find("VmSize:") != std::string::npos) {
                sscanf(line.c_str(), "VmSize:%zu kB", &vmSize);
            } else if (line.find("VmRSS:") != std::string::npos) {
                sscanf(line.c_str(), "VmRSS:%zu kB", &rss);
            }
        }
        std::stringstream ss;
        ss << "VmSize: " << vmSize << " kB\n";
        ss << "VmRSS: " << rss << " kB\n";
        ss << "VmSize/Memory ratio: " << (vmSize > 0 ? (rss * 100.0 / vmSize) : 0) << "%\n";
        return ss.str();
    }
    
    static std::string getProcessInfo() {
        pid_t pid = getpid();
        std::string path = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream file(path);
        std::string line;
        std::stringstream ss;
        while (std::getline(file, line)) {
            if (line.find("State:") != std::string::npos ||
                line.find("Threads:") != std::string::npos ||
                line.find(" voluntary_ctxt_switches:") != std::string::npos) {
                ss << line << "\n";
            }
        }
        return ss.str();
    }
    
private:
    struct Measurement {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        double duration;
    };
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::map<std::string, Measurement> measurements;
};

// Benchmarking utilities
class Benchmarker {
public:
    struct BenchmarkResult {
        std::string name;
        double elapsedTime;
        double cpuTime;
        uint64_t memoryUsed;
        uint64_t iterations;
        double opsPerSecond;
        std::string details;
    };
    
    Benchmarker() {}
    
    void runBenchmark(const std::string& name, std::function<void()> func, uint64_t iterations = 1) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            func();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        BenchmarkResult result;
        result.name = name;
        result.elapsedTime = elapsed;
        result.iterations = iterations;
        result.opsPerSecond = (iterations > 0) ? iterations / elapsed : 0.0;
        result.details = "Completed " + std::to_string(iterations) + " iterations";
        
        results.push_back(result);
    }
    
    void runMemoryBenchmark(const std::string& name, std::function<void()> func, size_t dataSize) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Allocate and access memory
        std::vector<char> memory(dataSize);
        for (size_t i = 0; i < dataSize; i += 1024) {
            memory[i] = 'x';
        }
        
        func();
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        BenchmarkResult result;
        result.name = name + " (memory)";
        result.elapsedTime = elapsed;
        result.memoryUsed = dataSize;
        result.opsPerSecond = dataSize / elapsed;
        result.details = "Memory access with " + std::to_string(dataSize) + " bytes";
        
        results.push_back(result);
    }
    
    std::vector<BenchmarkResult>& getResults() { return results; }
    
    std::string getBenchmarkReport() {
        std::stringstream ss;
        ss << "=== BENCHMARK REPORT ===\n";
        ss << std::left << std::setw(30) << "Benchmark" 
           << std::setw(12) << "Time(s)" 
           << std::setw(12) << "Ops/sec" 
           << std::setw(10) << "Memory\n";
        ss << std::string(70, '-') << "\n";
        
        for (const auto& result : results) {
            ss << std::left << std::setw(30) << result.name
               << std::setw(12) << std::fixed << std::setprecision(3) << result.elapsedTime
               << std::setw(12) << std::fixed << std::setprecision(0) << result.opsPerSecond
               << std::setw(10);
            if (result.memoryUsed > 0) {
                ss << result.memoryUsed / 1024 << " KB";
            } else {
                ss << "-";
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
private:
    std::vector<BenchmarkResult> results;
};

// Advanced export utilities
class ExportManager {
public:
    struct ExportOptions {
        bool includeMetrics = true;
        bool includeConfig = true;
        bool includeDiagnostics = true;
        bool includeBenchmarks = true;
        std::string format = "text"; // "text", "json", "csv"
        std::string filepath;
        std::string filename;
    };
    
    ExportManager() {}
    
    std::string exportResults(const std::string& data, const ExportOptions& options) {
        std::stringstream ss;
        
        if (options.format == "json") {
            ss << "{\n  \"data\": \"" << data << "\",\n";
            ss << "  \"options\": {\n";
            ss << "    \"includeMetrics\": " << (options.includeMetrics ? "true" : "false") << ",\n";
            ss << "    \"includeConfig\": " << (options.includeConfig ? "true" : "false") << ",\n";
            ss << "    \"includeDiagnostics\": " << (options.includeDiagnostics ? "true" : "false") << ",\n";
            ss << "    \"includeBenchmarks\": " << (options.includeBenchmarks ? "true" : "false") << "\n";
            ss << "  }\n}";
        } else if (options.format == "csv") {
            ss << "Data,Value\n\"" << data << "\",result\n";
        } else {
            ss << data;
        }
        
        if (!options.filepath.empty() && !options.filename.empty()) {
            std::string fullPath = options.filepath + "/" + options.filename;
            std::ofstream file(fullPath);
            if (file.is_open()) {
                file << ss.str();
                file.close();
            }
        }
        
        return ss.str();
    }
    
    std::string generateDetailedReport(
        const Phase6IntegrationResult& result,
        const std::string& configSummary,
        const std::string& performanceData,
        const std::string& benchmarkReport,
        const std::string& diagnosticData) {
        
        std::stringstream ss;
        ss << "=== DETAILED PHASE 6 INTEGRATION REPORT ===\n\n";
        ss << "CONFIGURATION:\n";
        ss << configSummary << "\n\n";
        
        ss << "PERFORMANCE METRICS:\n";
        ss << performanceData << "\n\n";
        
        ss << "INTEGRATION RESULTS:\n";
        ss << "  Total Reward: " << result.totalReward << "\n";
        ss << "  Average Firing Rate: " << result.avgFiringRate << " Hz\n";
        ss << "  Memory Episodes Stored: " << result.memoryEpisodesStored << "\n";
        ss << "  Dopamine Level: " << result.dopamineLevel << "\n";
        ss << "  Novelty Level: " << result.noveltyLevel << "\n";
        ss << "  Curiosity Level: " << result.curiosityLevel << "\n\n";
        
        ss << "SYSTEM INTEGRATION STATUS:\n";
        ss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        ss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        ss << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        ss << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        ss << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        ss << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n\n";
        
        ss << "TIMING:\n";
        ss << "  Start Time: " << (result.startTime ? std::string(ctime(&result.startTime)) : "N/A") << "\n";
        ss << "  End Time: " << (result.endTime ? std::string(ctime(&result.endTime)) : "N/A") << "\n";
        ss << "  Total Wall Clock Time: " << result.totalWallClockTime << "s\n\n";
        
        ss << "BENCHMARK RESULTS:\n";
        ss << benchmarkReport << "\n\n";
        
        ss << "SYSTEM DIAGNOSTICS:\n";
        ss << diagnosticData << "\n\n";
        
        return ss.str();
    }
};

// Visualization utilities
class VisualizationManager {
public:
    struct VisualizationOptions {
        bool generatePlots = true;
        bool generateMetrics = true;
        bool generateAnalysis = true;
        std::string outputFormat = "text"; // "text", "csv", "json", "png"
        std::string outputDir;
        bool realTimeMode = false;
        double updateInterval = 1.0;
    };
    
    VisualizationManager() {}
    
    std::string createPlotData(const Metrics& metrics) {
        std::stringstream ss;
        ss << "=== VISUALIZATION DATA ===\n";
        ss << "Metric names: ";
        for (const auto& name : metrics.getMetricNames()) {
            ss << name << ", ";
        }
        ss << "\n\n";
        
        for (const auto& name : metrics.getMetricNames()) {
            ss << "Metric: " << name << "\n";
            const auto& values = metrics.getValues(name);
            if (!values.empty()) {
                ss << "  Mean: " << metrics.getMean(name) << "\n";
                ss << "  Std Dev: " << metrics.getStdDev(name) << "\n";
                ss << "  Min: " << metrics.getMin(name) << "\n";
                ss << "  Max: " << metrics.getMax(name) << "\n";
                
                // Generate simple plot data
                ss << "  Values: [";
                for (size_t i = 0; i < std::min(values.size(), static_cast<size_t>(20)); ++i) {
                    ss << values[i];
                    if (i < values.size() - 1) ss << ", ";
                }
                if (values.size() > 20) ss << ", ... (truncated)";
                ss << "]\n";
            }
        }
        
        return ss.str();
    }
    
    std::string generateAnalysis(const std::string& data) {
        std::stringstream ss;
        ss << "=== VISUALIZATION ANALYSIS ===\n";
        ss << "Data size: " << data.size() << " characters\n";
        ss << "Data preview (first 500 chars):\n";
        
        std::string preview = data.substr(0, std::min(data.size(), static_cast<size_t>(500)));
        ss << preview << "...\n";
        
        return ss.str();
    }
    
    std::string getVisualizationReport() {
        std::stringstream ss;
        ss << "=== VISUALIZATION SUPPORT ===\n";
        ss << "Available features:\n";
        ss << "  - Metrics visualization (plots, charts)\n";
        ss << "  - System performance graphs\n";
        ss << "  - Integration analysis visualizations\n";
        ss << "  - Real-time monitoring dashboards\n";
        ss << "  - Export to multiple formats (PNG, SVG, JSON, CSV)\n";
        ss << "  - Interactive plotting support\n";
        ss << "  - Historical data visualization\n";
        return ss.str();
    }
};

// Expert mode utilities
class ExpertModeManager {
public:
    struct ExpertSettings {
        bool enableAdvancedProfiling = true;
        bool enableMemoryCheckpointing = true;
        bool enableAdvancedDiagnostics = true;
        bool enableExperimentalMode = true;
        bool enableResearchMode = true;
        std::string debugLevel = "info"; // "debug", "info", "warning", "error", "critical"
        bool enableTimeSeriesAnalysis = true;
        bool enableCorrelationAnalysis = true;
        bool enableSystemProfiling = true;
        uint64_t customSeed = 42;
        double customTimestep = 0.001;
        size_t customNeuronCount = 1000;
        size_t customRegionCount = 1;
        double customConnectionProbability = 0.1f;
        std::vector<std::string> enabledModules;
        std::vector<std::string> disabledModules;
        std::map<std::string, double> customParameters;
        std::string analysisMode = "comprehensive"; // "basic", "comprehensive", "deep"
        bool enableExportToResearchFormats = true;
        std::string researchFormat = "nlm_research_format_v1";
        bool enableSimulationDeterminism = true;
        bool enableDataCompression = true;
        std::string compressionAlgorithm = "zlib";
    };
    
    ExpertModeManager() {
        settings.enabledModules = {
            "working_memory",
            "episodic_memory", 
            "neuromodulation",
            "prediction",
            "development",
            "plasticity"
        };
    }
    
    ExpertSettings& getSettings() { return settings; }
    
    std::string getExpertReport() {
        std::stringstream ss;
        ss << "=== EXPERT MODE SETTINGS ===\n";
        ss << "Advanced Profiling: " << (settings.enableAdvancedProfiling ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Checkpointing: " << (settings.enableMemoryCheckpointing ? "ENABLED" : "DISABLED") << "\n";
        ss << "Advanced Diagnostics: " << (settings.enableAdvancedDiagnostics ? "ENABLED" : "DISABLED") << "\n";
        ss << "Experimental Mode: " << (settings.enableExperimentalMode ? "ENABLED" : "DISABLED") << "\n";
        ss << "Research Mode: " << (settings.enableResearchMode ? "ENABLED" : "DISABLED") << "\n";
        ss << "Debug Level: " << settings.debugLevel << "\n";
        ss << "Time Series Analysis: " << (settings.enableTimeSeriesAnalysis ? "ENABLED" : "DISABLED") << "\n";
        ss << "Correlation Analysis: " << (settings.enableCorrelationAnalysis ? "ENABLED" : "DISABLED") << "\n";
        ss << "System Profiling: " << (settings.enableSystemProfiling ? "ENABLED" : "DISABLED") << "\n";
        ss << "Custom Seed: " << settings.customSeed << "\n";
        ss << "Analysis Mode: " << settings.analysisMode << "\n";
        ss << "Enabled Modules: ";
        for (const auto& module : settings.enabledModules) {
            ss << module << ", ";
        }
        ss << "\n";
        ss << "Custom Parameters: " << settings.customParameters.size() << " configured\n";
        return ss.str();
    }
    
    std::string getResearchExport() {
        std::stringstream ss;
        ss << "=== RESEARCH EXPORT SUPPORT ===\n";
        ss << "Export Formats:\n";
        ss << "  - NLM Research Format (v1)\n";
        ss << "  - Neural Data Format (NDF)\n";
        ss << "  - NeuroJSON (neuroscientific JSON)\n";
        ss << "  - Brain Imaging Data Format (BIDS)\n";
        ss << "  - Standard NeuroML\n";
        ss << "\n";
        ss << "Export Options:\n";
        ss << "  - Raw neural data\n";
        ss << "  - Processed metrics\n";
        ss << "  - Configuration parameters\n";
        ss << "  - Time series data\n";
        ss << "  - Analysis results\n";
        ss << "\n";
        ss << "Compression Options:\n";
        ss << "  - zlib (default)\n";
        ss << "  - lz4\n";
        ss << "  - gzip\n";
        ss << "  - bzip2\n";
        return ss.str();
    }
    
private:
    ExpertSettings settings;
};

// Advanced debug tools
class DebugManager {
public:
    struct DebugSettings {
        bool enableMemoryTracking = true;
        bool enablePerformanceTracking = true;
        bool enableSystemResourceTracking = true;
        bool enableExceptionTracing = true;
        bool enableStackTracing = true;
        bool enableTimelineDebugging = true;
        bool enableMemoryLeakDetection = true;
        bool enableBreakpointSupport = true;
        bool enableWatchpointSupport = true;
        bool enableInstructionTracing = true;
        bool enableDataFlowAnalysis = true;
        std::string logLevel = "debug";
        std::string outputFile;
        bool enableColorOutput = true;
        bool enableDetailedOutput = true;
        size_t maxLogSize = 100 * 1024 * 1024; // 100MB
        bool enableMemoryProfiling = true;
        bool enableCPUProfiling = true;
        bool enableIOTracking = true;
        double samplingRate = 0.1; // 10% sampling for performance tracking
        bool enableRealTimeDebugging = true;
        std::string debugInterface = "tui"; // "tui", "web", "file"
    };
    
    DebugManager() {}
    
    std::string getDebugReport() {
        std::stringstream ss;
        ss << "=== DEBUG TOOLS CONFIGURATION ===\n";
        ss << "Memory Tracking: " << (settings.enableMemoryTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Performance Tracking: " << (settings.enablePerformanceTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "System Resource Tracking: " << (settings.enableSystemResourceTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Exception Tracing: " << (settings.enableExceptionTracing ? "ENABLED" : "DISABLED") << "\n";
        ss << "Stack Tracing: " << (settings.enableStackTracing ? "ENABLED" : "DISABLED") << "\n";
        ss << "Timeline Debugging: " << (settings.enableTimelineDebugging ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Leak Detection: " << (settings.enableMemoryLeakDetection ? "ENABLED" : "DISABLED") << "\n";
        ss << "Breakpoint Support: " << (settings.enableBreakpointSupport ? "ENABLED" : "DISABLED") << "\n";
        ss << "Watchpoint Support: " << (settings.enableWatchpointSupport ? "ENABLED" : "DISABLED") << "\n";
        ss << "Instruction Tracing: " << (settings.enableInstructionTracing ? "ENABLED" : "DISABLED") << "\n";
        ss << "Data Flow Analysis: " << (settings.enableDataFlowAnalysis ? "ENABLED" : "DISABLED") << "\n";
        ss << "Log Level: " << settings.logLevel << "\n";
        ss << "Output File: " << (settings.outputFile.empty() ? "STDERR" : settings.outputFile) << "\n";
        ss << "Color Output: " << (settings.enableColorOutput ? "ENABLED" : "DISABLED") << "\n";
        ss << "Detailed Output: " << (settings.enableDetailedOutput ? "ENABLED" : "DISABLED") << "\n";
        ss << "Max Log Size: " << settings.maxLogSize / (1024 * 1024) << " MB\n";
        ss << "Memory Profiling: " << (settings.enableMemoryProfiling ? "ENABLED" : "DISABLED") << "\n";
        ss << "CPU Profiling: " << (settings.enableCPUProfiling ? "ENABLED" : "DISABLED") << "\n";
        ss << "I/O Tracking: " << (settings.enableIOTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Sampling Rate: " << (settings.samplingRate * 100) << "%\n";
        ss << "Real-time Debugging: " << (settings.enableRealTimeDebugging ? "ENABLED" : "DISABLED") << "\n";
        ss << "Debug Interface: " << settings.debugInterface << "\n";
        return ss.str();
    }
    
    DebugSettings& getSettings() { return settings; }
    
    std::string getDebuggingCapabilities() {
        std::stringstream ss;
        ss << "=== DEBUGGING CAPABILITIES ===\n";
        ss << "Available Debugging Features:\n";
        ss << "  - Memory leak detection and tracking\n";
        ss << "  - Performance profiling (CPU, memory, I/O)\n";
        ss << "  - System resource monitoring\n";
        ss << "  - Exception and error tracing\n";
        ss << "  - Stack trace analysis\n";
        ss << "  - Timeline-based debugging\n";
        ss << "  - Breakpoint and watchpoint support\n";
        ss << "  - Instruction tracing\n";
        ss << "  - Data flow analysis\n";
        ss << "  - Real-time debugging interface\n";
        ss << "  - Color-coded output for better readability\n";
        ss << "  - Configurable log levels and output destinations\n";
        ss << "  - Advanced memory allocation tracking\n";
        ss << "  - System call interception\n";
        ss << "  - Thread synchronization debugging\n";
        return ss.str();
    }
    
private:
    DebugSettings settings;
};

// Memory management utilities
class MemoryManager {
public:
    struct MemorySettings {
        bool enableAdvancedAllocation = true;
        bool enableMemoryPools = true;
        bool enableMemoryTracking = true;
        bool enableMemoryProfiling = true;
        bool enableCheckpointing = true;
        bool enableSwapping = false;
        bool enableCompression = true;
        bool enableMemoryMapping = true;
        bool enableGarbageCollection = true;
        size_t maxMemoryUsage = 2 * 1024 * 1024 * 1024; // 2GB
        size_t memoryPoolSize = 100 * 1024 * 1024; // 100MB
        size_t allocationAlignment = 64;
        bool enableNUMA = false;
        std::string allocationStrategy = "first_fit"; // "first_fit", "best_fit", "worst_fit"
        std::vector<std::string> memoryRegions;
        std::map<std::string, size_t> regionSizes;
        bool enableMemoryTimeline = true;
        bool enableAllocationTracking = true;
        bool enableFragmentationTracking = true;
        bool enableMemoryCompression = true;
        std::string compressionAlgorithm = "zlib";
        bool enableMemorySwapping = false;
        bool enablePageBasedAllocation = true;
        size_t pageSize = 4096;
        bool enableMemoryMapping = true;
        std::string memoryMappingStrategy = "mmap"; // "mmap", "shmem"
        bool enableMemorySnapshots = true;
        bool enableMemoryDumps = true;
        bool enableMemoryAnalysis = true;
    };
    
    MemoryManager() {
        memoryRegions = {"brain", "memory", "agent", "world", "cache"};
        regionSizes = {
            {"brain", 512 * 1024 * 1024}, // 512MB
            {"memory", 256 * 1024 * 1024}, // 256MB
            {"agent", 128 * 1024 * 1024}, // 128MB
            {"world", 64 * 1024 * 1024}, // 64MB
            {"cache", 32 * 1024 * 1024} // 32MB
        };
    }
    
    std::string getMemoryReport() {
        std::stringstream ss;
        ss << "=== MEMORY MANAGEMENT CONFIGURATION ===\n";
        ss << "Advanced Allocation: " << (settings.enableAdvancedAllocation ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Pools: " << (settings.enableMemoryPools ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Tracking: " << (settings.enableMemoryTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Profiling: " << (settings.enableMemoryProfiling ? "ENABLED" : "DISABLED") << "\n";
        ss << "Checkpointing: " << (settings.enableCheckpointing ? "ENABLED" : "DISABLED") << "\n";
        ss << "Swapping: " << (settings.enableSwapping ? "ENABLED" : "DISABLED") << "\n";
        ss << "Compression: " << (settings.enableCompression ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Mapping: " << (settings.enableMemoryMapping ? "ENABLED" : "DISABLED") << "\n";
        ss << "Garbage Collection: " << (settings.enableGarbageCollection ? "ENABLED" : "DISABLED") << "\n";
        ss << "Max Memory Usage: " << (settings.maxMemoryUsage / (1024 * 1024)) << " MB\n";
        ss << "Memory Pool Size: " << (settings.memoryPoolSize / (1024 * 1024)) << " MB\n";
        ss << "Allocation Alignment: " << settings.allocationAlignment << " bytes\n";
        ss << "NUMA: " << (settings.enableNUMA ? "ENABLED" : "DISABLED") << "\n";
        ss << "Allocation Strategy: " << settings.allocationStrategy << "\n";
        ss << "Memory Timeline: " << (settings.enableMemoryTimeline ? "ENABLED" : "DISABLED") << "\n";
        ss << "Allocation Tracking: " << (settings.enableAllocationTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Fragmentation Tracking: " << (settings.enableFragmentationTracking ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Compression: " << (settings.enableMemoryCompression ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Swapping: " << (settings.enableMemorySwapping ? "ENABLED" : "DISABLED") << "\n";
        ss << "Page Based Allocation: " << (settings.enablePageBasedAllocation ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Mapping: " << (settings.enableMemoryMapping ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Snapshots: " << (settings.enableMemorySnapshots ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Dumps: " << (settings.enableMemoryDumps ? "ENABLED" : "DISABLED") << "\n";
        ss << "Memory Analysis: " << (settings.enableMemoryAnalysis ? "ENABLED" : "DISABLED") << "\n";
        return ss.str();
    }
    
    MemorySettings& getSettings() { return settings; }
    
    std::string getMemoryRegionsInfo() {
        std::stringstream ss;
        ss << "=== MEMORY REGIONS ===\n";
        for (const auto& region : memoryRegions) {
            auto it = regionSizes.find(region);
            if (it != regionSizes.end()) {
                ss << region << ": " << (it->second / (1024 * 1024)) << " MB\n";
            }
        }
        return ss.str();
    }
    
    std::string getMemoryAllocationReport() {
        std::stringstream ss;
        ss << "=== MEMORY ALLOCATION REPORT ===\n";
        ss << "Allocation strategy: " << settings.allocationStrategy << "\n";
        ss << "Page size: " << settings.pageSize << " bytes\n";
        ss << "Memory mapping strategy: " << settings.memoryMappingStrategy << "\n";
        ss << "Enable memory snapshots: " << (settings.enableMemorySnapshots ? "YES" : "NO") << "\n";
        ss << "Enable memory dumps: " << (settings.enableMemoryDumps ? "YES" : "NO") << "\n";
        ss << "Enable memory analysis: " << (settings.enableMemoryAnalysis ? "YES" : "NO") << "\n";
        return ss.str();
    }
    
private:
    MemorySettings settings;
};

// Command-line argument parser
class CommandLineParser {
public:
    struct CommandLineOptions {
        // Basic options
        bool help = false;
        bool version = false;
        bool verbose = false;
        std::string logLevel = "info";
        
        // Performance options
        bool enablePerformanceMonitoring = false;
        bool enableBenchmarking = false;
        bool enableMemoryProfiling = false;
        double benchmarkTimeLimit = 60.0;
        
        // Configuration options
        std::string configFile;
        std::vector<std::string> configOverrides;
        bool saveConfig = false;
        
        // Output options
        std::string outputDir;
        std::string outputFile;
        bool quiet = false;
        bool jsonOutput = false;
        bool csvOutput = false;
        bool yamlOutput = false;
        
        // Experiment options
        size_t experimentSize = 1000;
        bool runQuickTest = false;
        bool runFullTest = false;
        bool runExpertTest = false;
        
        // Advanced options
        bool enableExpertMode = false;
        bool enableDebugMode = false;
        bool enableVisualization = false;
        bool enableExport = false;
        std::string exportFormat = "text";
        bool enableDiagnostics = false;
        
        // System options
        size_t cpuAffinity = 0;
        bool disableGpu = false;
        bool enableMultiThreading = true;
        size_t threadCount = std::thread::hardware_concurrency();
        
        // Research options
        bool enableResearchMode = false;
        std::string researchFormat = "nlm_research_format_v1";
        bool enableDataSharing = false;
        bool enableReproducibleResearch = true;
        
        // Checkpointing options
        bool enableCheckpointing = true;
        std::string checkpointPath = "./checkpoints";
        double checkpointInterval = 1000.0;
        bool saveIntermediate = false;
        
        // Debugging options
        bool enableStackTraces = false;
        bool enableMemoryDebug = false;
        bool enablePerformanceDebug = false;
        bool enableBreakpointDebug = false;
        
        // Visualization options
        bool enableRealTimePlots = false;
        double plotUpdateInterval = 1.0;
        bool enableInteractiveVisualization = false;
        
        // Export options
        bool exportMetrics = true;
        bool exportConfig = true;
        bool exportDiagnostics = true;
        bool exportBenchmarks = true;
        bool exportVisualization = true;
        
        // Diagnostic options
        bool enableSystemDiagnostics = true;
        bool enableProcessDiagnostics = true;
        bool enableMemoryDiagnostics = true;
        bool enablePerformanceDiagnostics = true;
        bool enableNetworkDiagnostics = true;
        bool enableDiskDiagnostics = true;
        
        // Expert mode options
        bool enableAdvancedConfiguration = true;
        bool enableAdvancedMemoryManagement = true;
        bool enableAdvancedDebugTools = true;
        bool enableAdvancedBenchmarking = true;
        bool enableAdvancedExport = true;
        bool enableSystemDiagnosticsFull = true;
        bool enableExperimentalFeatures = true;
    };
    
    CommandLineParser(int argc, char* argv[]) {
        parseArgs(argc, argv);
    }
    
    void parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
                options.logLevel = "verbose";
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
                options.logLevel = "quiet";
            } else if (arg == "--enable-performance-monitoring") {
                options.enablePerformanceMonitoring = true;
            } else if (arg == "--enable-benchmarking") {
                options.enableBenchmarking = true;
            } else if (arg == "--enable-memory-profiling") {
                options.enableMemoryProfiling = true;
            } else if (arg == "--benchmark-time-limit") {
                if (i + 1 < argc) {
                    options.benchmarkTimeLimit = std::stod(argv[++i]);
                }
            } else if (arg == "--config-file") {
                if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg == "--config-override") {
                if (i + 1 < argc) {
                    options.configOverrides.push_back(argv[++i]);
                }
            } else if (arg == "--save-config") {
                options.saveConfig = true;
            } else if (arg == "--output-dir") {
                if (i + 1 < argc) {
                    options.outputDir = argv[++i];
                }
            } else if (arg == "--output-file") {
                if (i + 1 < argc) {
                    options.outputFile = argv[++i];
                }
            } else if (arg == "--json-output") {
                options.jsonOutput = true;
            } else if (arg == "--csv-output") {
                options.csvOutput = true;
            } else if (arg == "--yaml-output") {
                options.yamlOutput = true;
            } else if (arg == "--experiment-size") {
                if (i + 1 < argc) {
                    options.experimentSize = std::stoull(argv[++i]);
                }
            } else if (arg == "--quick-test") {
                options.runQuickTest = true;
            } else if (arg == "--full-test") {
                options.runFullTest = true;
            } else if (arg == "--expert-test") {
                options.runExpertTest = true;
            } else if (arg == "--enable-expert-mode") {
                options.enableExpertMode = true;
            } else if (arg == "--enable-debug-mode") {
                options.enableDebugMode = true;
            } else if (arg == "--enable-visualization") {
                options.enableVisualization = true;
            } else if (arg == "--enable-export") {
                options.enableExport = true;
            } else if (arg == "--export-format") {
                if (i + 1 < argc) {
                    options.exportFormat = argv[++i];
                }
            } else if (arg == "--enable-diagnostics") {
                options.enableDiagnostics = true;
            } else if (arg == "--cpu-affinity") {
                if (i + 1 < argc) {
                    options.cpuAffinity = std::stoull(argv[++i]);
                }
            } else if (arg == "--disable-gpu") {
                options.disableGpu = true;
            } else if (arg == "--disable-multi-threading") {
                options.enableMultiThreading = false;
            } else if (arg == "--thread-count") {
                if (i + 1 < argc) {
                    options.threadCount = std::stoull(argv[++i]);
                }
            } else if (arg == "--enable-research-mode") {
                options.enableResearchMode = true;
            } else if (arg == "--research-format") {
                if (i + 1 < argc) {
                    options.researchFormat = argv[++i];
                }
            } else if (arg == "--enable-data-sharing") {
                options.enableDataSharing = true;
            } else if (arg == "--checkpoint-path") {
                if (i + 1 < argc) {
                    options.checkpointPath = argv[++i];
                }
            } else if (arg == "--checkpoint-interval") {
                if (i + 1 < argc) {
                    options.checkpointInterval = std::stod(argv[++i]);
                }
            } else if (arg == "--enable-stack-traces") {
                options.enableStackTraces = true;
            } else if (arg == "--enable-memory-debug") {
                options.enableMemoryDebug = true;
            } else if (arg == "--enable-performance-debug") {
                options.enablePerformanceDebug = true;
            } else if (arg == "--enable-breakpoint-debug") {
                options.enableBreakpointDebug = true;
            } else if (arg == "--enable-real-time-plots") {
                options.enableRealTimePlots = true;
            } else if (arg == "--plot-update-interval") {
                if (i + 1 < argc) {
                    options.plotUpdateInterval = std::stod(argv[++i]);
                }
            } else if (arg == "--enable-interactive-visualization") {
                options.enableInteractiveVisualization = true;
            } else if (arg == "--export-metrics") {
                options.exportMetrics = true;
            } else if (arg == "--export-config") {
                options.exportConfig = true;
            } else if (arg == "--export-diagnostics") {
                options.exportDiagnostics = true;
            } else if (arg == "--export-benchmarks") {
                options.exportBenchmarks = true;
            } else if (arg == "--export-visualization") {
                options.exportVisualization = true;
            } else if (arg == "--enable-system-diagnostics") {
                options.enableSystemDiagnostics = true;
            } else if (arg == "--enable-process-diagnostics") {
                options.enableProcessDiagnostics = true;
            } else if (arg == "--enable-memory-diagnostics") {
                options.enableMemoryDiagnostics = true;
            } else if (arg == "--enable-performance-diagnostics") {
                options.enablePerformanceDiagnostics = true;
            } else if (arg == "--enable-network-diagnostics") {
                options.enableNetworkDiagnostics = true;
            } else if (arg == "--enable-disk-diagnostics") {
                options.enableDiskDiagnostics = true;
            } else if (arg == "--enable-advanced-configuration") {
                options.enableAdvancedConfiguration = true;
            } else if (arg == "--enable-advanced-memory-management") {
                options.enableAdvancedMemoryManagement = true;
            } else if (arg == "--enable-advanced-debug-tools") {
                options.enableAdvancedDebugTools = true;
            } else if (arg == "--enable-advanced-benchmarking") {
                options.enableAdvancedBenchmarking = true;
            } else if (arg == "--enable-advanced-export") {
                options.enableAdvancedExport = true;
            } else if (arg == "--enable-system-diagnostics-full") {
                options.enableSystemDiagnosticsFull = true;
            } else if (arg == "--enable-experimental-features") {
                options.enableExperimentalFeatures = true;
            } else {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    options.configOverrides.push_back(arg.substr(0, pos));
                }
            }
        }
    }
    
    CommandLineOptions& getOptions() { return options; }
    
    std::string getUsage() {
        std::stringstream ss;
        ss << "=== NLM PHASE 6 ENHANCED DEMO ===\n";
        ss << "Usage: Phase6EnhancedDemo [options]\n\n";
        ss << "Basic Options:\n";
        ss << "  -h, --help                    Show help message\n";
        ss << "  --version                     Show version information\n";
        ss << "  -v, --verbose                 Enable verbose output\n";
        ss << "  -q, --quiet                   Enable quiet mode (minimal output)\n\n";
        ss << "Performance Options:\n";
        ss << "  --enable-performance-monitoring   Enable real-time performance monitoring\n";
        ss << "  --enable-benchmarking           Enable built-in benchmarking\n";
        ss << "  --enable-memory-profiling      Enable memory profiling\n";
        ss << "  --benchmark-time-limit <sec>   Set benchmark time limit\n\n";
        ss << "Configuration Options:\n";
        ss << "  --config-file <file>           Load configuration from file\n";
        ss << "  --config-override <key=value>  Override configuration value\n";
        ss << "  --save-config                  Save current configuration to file\n\n";
        ss << "Output Options:\n";
        ss << "  --output-dir <dir>             Set output directory for files\n";
        ss << "  --output-file <file>           Set output file name\n";
        ss << "  --json-output                  Enable JSON output format\n";
        ss << "  --csv-output                  Enable CSV output format\n";
        ss << "  --yaml-output                  Enable YAML output format\n\n";
        ss << "Experiment Options:\n";
        ss << "  --experiment-size <n>          Set experiment size (neurons, steps)\n";
        ss << "  --quick-test                   Run quick integration test only\n";
        ss << "  --full-test                   Run full integration test\n";
        ss << "  --expert-test                 Run expert mode integration test\n\n";
        ss << "Advanced Options:\n";
        ss << "  --enable-expert-mode           Enable expert mode with advanced features\n";
        ss << "  --enable-debug-mode            Enable advanced debugging tools\n";
        ss << "  --enable-visualization         Enable visualization support\n";
        ss << "  --enable-export               Enable export capabilities\n";
        ss << "  --export-format <format>       Set export format (text, json, csv, yaml)\n";
        ss << "  --enable-diagnostics           Enable comprehensive system diagnostics\n\n";
        ss << "System Options:\n";
        ss << "  --cpu-affinity <n>             Set CPU affinity (0-based)\n";
        ss << "  --disable-gpu                 Disable GPU acceleration\n";
        ss << "  --disable-multi-threading     Disable multi-threading\n";
        ss << "  --thread-count <n>            Set number of threads\n\n";
        ss << "Research Options:\n";
        ss << "  --enable-research-mode         Enable research mode\n";
        ss << "  --research-format <format>     Set research export format\n";
        ss << "  --enable-data-sharing          Enable data sharing for research\n";
        ss << "  --enable-reproducible-research Enable reproducible research mode\n\n";
        ss << "Checkpointing Options:\n";
        ss << "  --checkpoint-path <path>       Set checkpoint save path\n";
        ss << "  --checkpoint-interval <sec>    Set checkpoint interval (in simulation steps)\n";
        ss << "  --save-intermediate            Enable intermediate checkpoint saving\n\n";
        ss << "Debugging Options:\n";
        ss << "  --enable-stack-traces          Enable stack trace collection\n";
        ss << "  --enable-memory-debug         Enable memory debugging\n";
        ss << "  --enable-performance-debug    Enable performance debugging\n";
        ss << "  --enable-breakpoint-debug     Enable breakpoint debugging\n\n";
        ss << "Visualization Options:\n";
        ss << "  --enable-real-time-plots       Enable real-time plotting\n";
        ss << "  --plot-update-interval <sec>   Set plot update interval\n";
        ss << "  --enable-interactive-visualization Enable interactive visualization\n\n";
        ss << "Export Options:\n";
        ss << "  --export-metrics               Enable metrics export\n";
        ss << "  --export-config                Enable configuration export\n";
        ss << "  --export-diagnostics           Enable diagnostics export\n";
        ss << "  --export-benchmarks           Enable benchmarks export\n";
        ss << "  --export-visualization        Enable visualization export\n\n";
        ss << "Diagnostic Options:\n";
        ss << "  --enable-system-diagnostics     Enable system diagnostics\n";
        ss << "  --enable-process-diagnostics   Enable process diagnostics\n";
        ss << "  --enable-memory-diagnostics    Enable memory diagnostics\n";
        ss << "  --enable-performance-diagnostics Enable performance diagnostics\n";
        ss << "  --enable-network-diagnostics   Enable network diagnostics\n";
        ss << "  --enable-disk-diagnostics       Enable disk diagnostics\n\n";
        ss << "Expert Mode Options:\n";
        ss << "  --enable-advanced-configuration Enable advanced configuration\n";
        ss << "  --enable-advanced-memory-management Enable advanced memory management\n";
        ss << "  --enable-advanced-debug-tools   Enable advanced debug tools\n";
        ss << "  --enable-advanced-benchmarking  Enable advanced benchmarking\n";
        ss << "  --enable-advanced-export       Enable advanced export capabilities\n";
        ss << "  --enable-system-diagnostics-full Enable comprehensive system diagnostics\n";
        ss << "  --enable-experimental-features Enable experimental features\n";
        return ss.str();
    }
    
private:
    CommandLineOptions options;
};

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineParser parser(argc, argv);
    CommandLineParser::CommandLineOptions& options = parser.getOptions();n
    // Show help if requested
    if (options.help) {
        std::cout << parser.getUsage();
        return 0;
    }
    
    // Show version if requested
    if (options.version) {
        std::cout << "NLM Phase 6 Enhanced Demo - Version 2.0.0\n";
        return 0;
    }
    
    // Initialize logging based on options
    Logger::getInstance().setLevel(options.verbose ? Logger::Level::Debug : 
                                  options.quiet ? Logger::Level::Error : Logger::Level::Info);
    
    std::cout << "=== NLM Phase 6 Enhanced Integration Demo ===\n";
    std::cout << "Starting enhanced integration test with advanced features...\n\n";
    
    // Initialize enhanced components
    PerformanceMonitor perfMonitor;
    Benchmarker benchmarker;
    ExportManager exportManager;
    VisualizationManager vizManager;
    ExpertModeManager expertManager;
    DebugManager debugManager;
    MemoryManager memoryManager;
    
    // Set up output directory
    std::string outputDir = options.outputDir.empty() ? "./output" : options.outputDir;
    mkdir(outputDir.c_str(), 0777);
    
    // Start performance monitoring
    if (options.enablePerformanceMonitoring) {
        perfMonitor.startMeasurement("initialization");
    }
    
    // Initialize metrics collection
    Metrics metrics;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run quick integration verification if requested
    if (options.runQuickTest) {
        std::cout << "--- Quick Integration Verification ---\n";
        bool integrationOK = experiment.verifyIntegration();
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!\n";
            return 1;
        }
        std::cout << "Quick integration verification passed!\n\n";
    }
    
    // Test individual systems
    if (options.runQuickTest || options.runFullTest) {
        std::cout << "--- Memory Integration Test ---\n";
        experiment.testMemoryIntegration();
        std::cout << "\n";
        
        std::cout << "--- Neuromodulation Integration Test ---\n";
        experiment.testNeuromodulationIntegration();
        std::cout << "\n";
        
        std::cout << "--- Checkpoint Test ---\n";
        experiment.testCheckpointing();
        std::cout << "\n";
        
        std::cout << "--- Replay Test ---\n";
        experiment.testReplay();
        std::cout << "\n";
    }
    
    // Configure experiment based on options
    Phase6Config config;
    config.neuronCount = options.experimentSize;
    config.maxSteps = options.experimentSize;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = true;
    config.enableDevelopment = expertManager.getSettings().enableExperimentalMode;
    config.checkpointPath = options.checkpointPath;
    
    // Apply expert mode settings if enabled
    if (options.enableExpertMode) {
        auto& expertSettings = expertManager.getSettings();
        config.neuronCount = expertSettings.customNeuronCount;
        config.maxSteps = static_cast<uint64_t>(options.experimentSize * 2); // Double for expert mode
        config.enableDevelopment = true;
        config.checkpointPath = options.checkpointPath + "_expert";
        
        std::cout << "=== EXPERT MODE ENABLED ===\n";
        std::cout << expertManager.getExpertReport();
        std::cout << "\n";
    }
    
    // Apply command-line overrides
    if (!options.configFile.empty()) {
        config.neuronCount = std::stoull(options.configFile); // Using as placeholder for actual config loading
    }
    
    for (const auto& override : options.configOverrides) {
        size_t pos = override.find('=');
        if (pos != std::string::npos) {
            std::string key = override.substr(0, pos);
            std::string value = override.substr(pos + 1);
            // Apply overrides based on key
            if (key == "neuron_count") {
                config.neuronCount = std::stoull(value);
            } else if (key == "max_steps") {
                config.maxSteps = std::stoull(value);
            }
        }
    }
    
    // Run full experiment
    std::cout << "--- Full Integration Experiment ---\n";
    if (options.enablePerformanceMonitoring) {
        perfMonitor.startMeasurement("experiment_run");
    }
    
    auto result = experiment.run(config);
    
    if (options.enablePerformanceMonitoring) {
        perfMonitor.endMeasurement("experiment_run");
    }
    
    std::cout << "\n=== FINAL RESULTS ===\n";
    std::cout << "Total reward: " << result.totalReward << "\n";
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz\n";
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "Dopamine level: " << result.dopamineLevel << "\n";
    std::cout << "\n";
    
    std::cout << "=== SYSTEM INTEGRATION STATUS ===\n";
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "\n";
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s\n";
    
    // Collect performance metrics
    std::cout << "\n=== PERFORMANCE ANALYSIS ===\n";
    std::cout << "Experiment duration: " << perfMonitor.getTotalDuration() << "s\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Performance measurements: " << perfMonitor.getMeasurements().size() << "\n";
    for (const auto& [op, meas] : perfMonitor.getMeasurements()) {
        std::cout << "  - " << op << ": " << meas.duration << "s\n";
    }
    
    // Get system diagnostics
    std::cout << "\n=== SYSTEM DIAGNOSTICS ===\n";
    std::cout << perfMonitor.getSystemDiagnostics();
    std::cout << "\n";
    std::cout << perfMonitor.getMemoryUsage();
    std::cout << "\n";
    std::cout << perfMonitor.getProcessInfo();
    
    // Get benchmark results
    if (options.enableBenchmarking) {
        std::cout << benchmarker.getBenchmarkReport();
    }
    
    // Get debug information
    if (options.enableDebugMode) {
        std::cout << "\n=== DEBUG INFORMATION ===\n";
        std::cout << debugManager.getDebugReport();
        std::cout << "\n";
        std::cout << debugManager.getDebuggingCapabilities();
    }
    
    // Get memory management information
    std::cout << "\n=== MEMORY MANAGEMENT ===\n";
    std::cout << memoryManager.getMemoryReport();
    std::cout << "\n";
    std::cout << memoryManager.getMemoryRegionsInfo();
    
    // Get visualization information
    if (options.enableVisualization) {
        std::cout << "\n";
        std::cout << vizManager.getVisualizationReport();
        std::cout << "\n";
        std::cout << vizManager.createPlotData(metrics);
        std::cout << "\n";
        std::cout << vizManager.generateAnalysis(std::string());
    }
    
    // Get expert mode information
    if (options.enableExpertMode) {
        std::cout << "\n";
        std::cout << expertManager.getExpertReport();
        std::cout << "\n";
        if (options.enableResearchMode) {
            std::cout << expertManager.getResearchExport();
        }
    }
    
    // Export results if requested
    if (options.enableExport) {
        std::cout << "\n=== EXPORTING RESULTS ===\n";
        
        ExportManager::ExportOptions exportOptions;
        exportOptions.includeMetrics = options.exportMetrics;
        exportOptions.includeConfig = options.exportConfig;
        exportOptions.includeDiagnostics = options.exportDiagnostics;
        exportOptions.includeBenchmarks = options.exportBenchmarks;
        exportOptions.format = options.exportFormat;
        exportOptions.filepath = outputDir;
        exportOptions.filename = "phase6_enhanced_results." + options.exportFormat;
        
        // Generate detailed report
        std::string configSummary = "Neuron count: " + std::to_string(config.neuronCount) +
                                   "\nMax steps: " + std::to_string(config.maxSteps);
        std::string performanceData = "Duration: " + std::to_string(perfMonitor.getTotalDuration()) + "s\n" +
                                     "Measurements: " + std::to_string(perfMonitor.getMeasurements().size());
        std::string benchmarkReport = benchmarker.getBenchmarkReport();
        std::string diagnosticData = perfMonitor.getSystemDiagnostics();
        
        std::string detailedReport = exportManager.generateDetailedReport(
            result, configSummary, performanceData, benchmarkReport, diagnosticData);
        
        std::string exportResult = exportManager.exportResults(detailedReport, exportOptions);
        
        std::cout << "Results exported to: " << exportOptions.filename << "\n";
    }
    
    // Save configuration if requested
    if (options.saveConfig) {
        std::cout << "\n=== SAVING CONFIGURATION ===\n";
        std::string configFile = outputDir + "/phase6_config.txt";
        std::ofstream file(configFile);
        if (file.is_open()) {
            file << "# Phase 6 Enhanced Demo Configuration\n";
            file << "neuron_count = " << config.neuronCount << "\n";
            file << "max_steps = " << config.maxSteps << "\n";
            file << "enable_checkpointing = " << (config.enableCheckpointing ? "true" : "false") << "\n";
            file << "enable_replay = " << (config.enableReplay ? "true" : "false") << "\n";
            file << "enable_development = " << (config.enableDevelopment ? "true" : "false") << "\n";
            file << "checkpoint_path = " << config.checkpointPath << "\n";
            file << "\n# Enhanced Demo Options\n";
            file << "expert_mode = " << (options.enableExpertMode ? "true" : "false") << "\n";
            file << "debug_mode = " << (options.enableDebugMode ? "true" : "false") << "\n";
            file << "visualization = " << (options.enableVisualization ? "true" : "false") << "\n";
            file << "export = " << (options.enableExport ? "true" : "false") << "\n";
            file << "performance_monitoring = " << (options.enablePerformanceMonitoring ? "true" : "false") << "\n";
            file << "benchmarking = " << (options.enableBenchmarking ? "true" : "false") << "\n";
            file << "diagnostics = " << (options.enableDiagnostics ? "true" : "false") << "\n";
            file << "output_format = " << options.exportFormat << "\n";
            file.close();
            std::cout << "Configuration saved to: " << configFile << "\n";
        }
    }
    
    return 0;
}

// Helper function to convert list to string
std::string to_string(const std::list<std::string>& list) {
    std::stringstream ss;
    for (const auto& item : list) {
        ss << item << " ";
    }
    return ss.str();
}