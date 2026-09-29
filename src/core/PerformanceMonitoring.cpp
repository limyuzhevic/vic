// performance_monitoring.hpp - Performance monitoring and debugging tools

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <atomic>
#include <functional>
#include <queue>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "UnifiedCLI.hpp"

namespace nlm {

// Performance monitoring classes
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        // System metrics
        double cpuUsage;              // Percentage (0-100)
        double memoryUsage;           // MB
        double memoryUsagePercent;    // Percentage (0-100)
        size_t totalMemory;           // Total memory in MB
        uint64_t activeThreads;       // Number of active threads
        double diskIO;                // MB/s read+write
        double networkIO;             // MB/s upload+download
        
        // Simulation metrics
        double simulationTime;        // Real time elapsed (seconds)
        double simulationSpeed;       // Steps per second
        uint64_t totalSpikeCount;     // Total spikes generated
        uint64_t firingNeuronCount;   // Number of neurons currently firing
        double averageFiringRate;     // Average firing rate (Hz)
        double peakFiringRate;        // Peak firing rate (Hz)
        
        // Network metrics
        double averageConnectionStrength; // Average synaptic strength
        double maxConnectionStrength;     // Maximum synaptic strength
        double minConnectionStrength;     // Minimum synaptic strength
        
        // Activity metrics
        double totalActivity;         // Total neural activity
        double peakActivity;          // Peak activity level
        double averageActivity;       // Average activity level
        
        // Developmental metrics
        std::string developmentalStage;  // Current developmental stage
        double plasticityRate;           // Current plasticity rate
        double noveltyLevel;             // Novelty detection level
        double curiosityLevel;          // Curiosity level
        double dopamineLevel;           // Dopamine level
        
        // Timing
        std::chrono::system_clock::time_point timestamp;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0),
                               memoryUsagePercent(0.0), totalMemory(0),
                               activeThreads(0), diskIO(0.0),
                               networkIO(0.0), simulationTime(0.0),
                               simulationSpeed(0.0), totalSpikeCount(0),
                               firingNeuronCount(0), averageFiringRate(0.0),
                               peakFiringRate(0.0), averageConnectionStrength(0.0),
                               maxConnectionStrength(0.0), minConnectionStrength(0.0),
                               totalActivity(0.0), peakActivity(0.0),
                               averageActivity(0.0), plasticityRate(0.0),
                               noveltyLevel(0.0), curiosityLevel(0.0),
                               dopamineLevel(0.0) {}
    };
    
    struct PerformanceAnalysis {
        // Performance grades
        std::string overallGrade;           // A, B, C, D, F
        std::string cpuGrade;               // CPU performance grade
        std::string memoryGrade;            // Memory performance grade
        std::string simulationGrade;        // Simulation performance grade
        
        // Performance metrics
        double averageCpuUsage;             // Average CPU usage
        double averageMemoryUsage;          // Average memory usage
        double peakMemoryUsage;             // Peak memory usage
        double averageSimulationSpeed;      // Average simulation speed
        double simulationEfficiency;        // Steps per real time ratio
        
        // Anomaly detection
        bool cpuAnomaly;                   // CPU usage anomaly detected
        bool memoryAnomaly;                 // Memory usage anomaly detected
        bool simulationAnomaly;             // Simulation performance anomaly
        
        // Recommendations
        std::vector<std::string> recommendations; // Performance improvement suggestions
        
        // Trend analysis
        double cpuTrend;                    // CPU usage trend (+1, 0, -1)
        double memoryTrend;                 // Memory usage trend (+1, 0, -1)
        double simulationTrend;             // Simulation trend (+1, 0, -1)
    };
    
    struct ResourceLimits {
        double maxMemoryMB;      // Maximum memory usage in MB
        double maxCpuPercent;    // Maximum CPU usage percentage
        uint64_t maxThreads;     // Maximum number of threads
        double maxDiskIO;        // Maximum disk I/O in MB/s
        double maxNetworkIO;     // Maximum network I/O in MB/s
        uint64_t maxSteps;       // Maximum simulation steps
        std::chrono::duration<double> maxSimulationTime; // Maximum simulation time
        
        ResourceLimits() : maxMemoryMB(1024.0), maxCpuPercent(100.0),
                          maxThreads(8), maxDiskIO(100.0), maxNetworkIO(100.0),
                          maxSteps(1000000), maxSimulationTime(std::chrono::hours(1)) {}
    };
    
    struct BenchmarkResult {
        std::string name;                           // Benchmark name
        std::chrono::duration<double> elapsedTime;   // Total execution time
        PerformanceMetrics metrics;                // Performance metrics during benchmark
        uint64_t iterations;                        // Number of iterations
        double itemsPerSecond;                      // Items processed per second
        std::string status;                         // "passed", "failed", "skipped"
        std::map<std::string, std::chrono::duration<double>> phaseTimes; // Time per phase
        
        BenchmarkResult() : iterations(0), itemsPerSecond(0.0) {}
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Core monitoring functionality
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    bool isMonitoring() const { return isMonitoring_; }
    
    // Metrics collection
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getHistory(size_t limit = 1000) const;
    
    // Performance analysis
    PerformanceAnalysis analyzePerformance(const std::string& timeRange = "last_10min") const;
    bool isPerformanceAcceptable() const;
    
    // Resource monitoring
    ResourceLimits getResourceLimits() const { return resourceLimits_; }
    void setResourceLimits(const ResourceLimits& limits) { resourceLimits_ = limits; }
    bool checkResourceLimits(const PerformanceMetrics& metrics) const;
    
    // Profiling
    class Profiler {
    public:
        struct ProfileData {
            std::string name;
            std::chrono::duration<double> totalTime;
            std::chrono::duration<double> minTime;
            std::chrono::duration<double> maxTime;
            uint64_t callCount;
            double averageTime;
            double totalPercentage;
            
            ProfileData() : totalTime(0), minTime(std::chrono::duration<double>::max()),
                          maxTime(0), callCount(0), averageTime(0), totalPercentage(0) {}
        };
        
        Profiler();
        ~Profiler();
        
        void startProfile(const std::string& name);
        void stopProfile(const std::string& name);
        bool isProfiling(const std::string& name) const;
        
        std::chrono::duration<double> getProfileTime(const std::string& name) const;
        const ProfileData& getProfileData(const std::string& name) const;
        std::map<std::string, ProfileData> getAllProfiles() const;
        
        void resetProfiles();
        void resetProfile(const std::string& name);
        
        std::string getProfileReport() const;
        std::string getDetailedProfileReport() const;
        
        // Profile filtering
        std::map<std::string, ProfileData> getProfilesAboveTime(double threshold) const;
        std::map<std::string, ProfileData> getProfilesBelowTime(double threshold) const;
        std::map<std::string, ProfileData> getProfilesAboveCount(uint64_t threshold) const;
        
    private:
        std::map<std::string, std::chrono::steady_clock::time_point> startTimes_;
        std::map<std::string, std::chrono::duration<double>> accumulatedTimes_;
        std::map<std::string, ProfileData> profileData_;
        mutable std::mutex profileMutex_;
    };
    
    Profiler& getProfiler() { return profiler_; }
    
    // Benchmarking
    class Benchmark {
    public:
        Benchmark();
        ~Benchmark();
        
        // Benchmark registration
        void addBenchmark(const std::string& name,
                         const std::function<void()>& benchmarkFunc,
                         const std::map<std::string, std::string>& parameters = {});
        
        // Benchmark execution
        BenchmarkResult runBenchmark(const std::string& name,
                                    const std::map<std::string, std::string>& parameters = {});
        std::vector<BenchmarkResult> runAllBenchmarks();
        
        // Benchmark management
        bool removeBenchmark(const std::string& name);
        bool hasBenchmark(const std::string& name) const;
        std::vector<std::string> getBenchmarkNames() const;
        
        // Benchmark configuration
        void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }
        std::shared_ptr<Config> getConfig() const { return config_; }
        
        // Benchmark reporting
        std::string getBenchmarkReport() const;
        std::string getBenchmarkReport(const std::string& benchmarkName) const;
        std::string getBenchmarkSummaryReport() const;
        
        // Benchmark filtering
        std::vector<BenchmarkResult> getPassedBenchmarks() const;
        std::vector<BenchmarkResult> getFailedBenchmarks() const;
        std::vector<BenchmarkResult> getBenchmarksAboveTime(double threshold) const;
        std::vector<BenchmarkResult> getBenchmarksAboveItemsPerSecond(double threshold) const;
        
    private:
        std::map<std::string, std::function<void()>> benchmarks_;
        std::map<std::string, std::map<std::string, std::string>> benchmarkParameters_;
        std::shared_ptr<Config> config_;
        mutable std::mutex benchmarksMutex_;
    };
    
    Benchmark& getBenchmark() { return benchmark_; }
    
    // Alert and notification system
    class AlertSystem {
    public:
        enum class AlertLevel {
            Info,
            Warning,
            Error,
            Critical
        };
        
        struct Alert {
            std::string id;
            AlertLevel level;
            std::string category;           // "performance", "resource", "simulation", "error"
            std::string message;
            std::chrono::system_clock::time_point timestamp;
            std::map<std::string, std::string> context; // Additional context data
            bool acknowledged;
            
            Alert() : level(AlertLevel::Info), acknowledged(false) {}
            Alert(AlertLevel level, const std::string& message)
                : level(level), message(message), acknowledged(false) {
                timestamp = std::chrono::system_clock::now();
            }
        };
        
        AlertSystem();
        ~AlertSystem();
        
        // Alert management
        void addAlert(const Alert& alert);
        void addAlert(AlertLevel level, const std::string& message);
        void addPerformanceAlert(AlertLevel level, const std::string& message);
        void addResourceAlert(AlertLevel level, const std::string& message);
        void addSimulationAlert(AlertLevel level, const std::string& message);
        
        std::vector<Alert> getAlerts(AlertLevel level = AlertLevel::Info) const;
        std::vector<Alert> getAlerts(const std::string& category) const;
        std::vector<Alert> getUnacknowledgedAlerts() const;
        
        void acknowledgeAlert(const std::string& alertId);
        void clearAlerts();
        void clearAlerts(AlertLevel level);
        void clearAlerts(const std::string& category);
        
        // Alert filtering and analysis
        std::vector<Alert> getRecentAlerts(std::chrono::duration<double> timeWindow) const;
        bool hasAlerts(AlertLevel level) const;
        bool hasAlerts(const std::string& category) const;
        size_t getAlertCount(AlertLevel level = AlertLevel::Info) const;
        
        // Alert reporting
        std::string getAlertReport() const;
        std::string getAlertReport(AlertLevel level) const;
        std::string getAlertReport(const std::string& category) const;
        
        // Alert conditions
        void checkPerformanceAlert(const PerformanceMetrics& metrics);
        void checkResourceAlert(const PerformanceMetrics& metrics);
        void checkSimulationAlert(const PerformanceMetrics& metrics);
        void checkThresholdAlert(const std::string& alertId,
                                AlertLevel level,
                                const std::string& category,
                                const std::string& message,
                                bool condition);
        
        // Alert configuration
        void setAlertThresholds(const std::map<std::string, double>& thresholds);
        std::map<std::string, double> getAlertThresholds() const;
        
        // Alert recovery
        void resolveAlert(const std::string& alertId, const std::string& resolutionMessage);
        
    private:
        std::vector<Alert> alerts_;
        std::map<std::string, double> alertThresholds_;
        std::mutex alertsMutex_;
        
        std::string generateAlertId();
    };
    
    AlertSystem& getAlertSystem() { return alertSystem_; }
    
    // Visualization system
    class Visualization {
    public:
        virtual ~Visualization() = default;
        
        virtual std::string getName() const = 0;
        virtual std::string getType() const = 0;
        virtual std::string getDescription() const = 0;
        
        virtual void update(const PerformanceMetrics& metrics) = 0;
        virtual std::string generateOutput() const = 0;
        
        virtual void setConfig(const std::shared_ptr<Config>& config) {}
        virtual std::shared_ptr<Config> getConfig() const { return nullptr; }
        
        virtual void setBrain(std::shared_ptr<Brain> brain) {}
        virtual std::shared_ptr<Brain> getBrain() const { return nullptr; }
    };
    
    class TextVisualization : public Visualization {
    public:
        TextVisualization();
        ~TextVisualization();
        
        std::string getName() const override { return "Text Visualization"; }
        std::string getType() const override { return "console"; }
        std::string getDescription() const override { return "Text-based console visualization"; }
        
        void update(const PerformanceMetrics& metrics) override;
        std::string generateOutput() const override;
        
        void setConfig(const std::shared_ptr<Config>& config) override;
        std::shared_ptr<Config> getConfig() const override;
    };
    
    class JSONVisualization : public Visualization {
    public:
        JSONVisualization();
        ~JSONVisualization();
        
        std::string getName() const override { return "JSON Visualization"; }
        std::string getType() const override { return "json"; }
        std::string getDescription() const override { return "JSON-based structured output"; }
        
        void update(const PerformanceMetrics& metrics) override;
        std::string generateOutput() const override;
        
        void setConfig(const std::shared_ptr<Config>& config) override;
        std::shared_ptr<Config> getConfig() const override;
    };
    
    class HTMLVisualization : public Visualization {
    public:
        HTMLVisualization();
        ~HTMLVisualization();
        
        std::string getName() const override { return "HTML Visualization"; }
        std::string getType() const override { return "html"; }
        std::string getDescription() const override { return "HTML-based interactive visualization"; }
        
        void update(const PerformanceMetrics& metrics) override;
        std::string generateOutput() const override;
        
        void setConfig(const std::shared_ptr<Config>& config) override;
        std::shared_ptr<Config> getConfig() const override;
    };
    
    // Add visualization
    void addVisualization(std::unique_ptr<Visualization> viz);
    void removeVisualization(const std::string& name);
    std::vector<std::string> getVisualizationNames() const;
    Visualization* getVisualization(const std::string& name);
    
    // Visualization management
    void updateAllVisualizations(const PerformanceMetrics& metrics);
    std::string generateAllVisualizationOutput() const;
    
    // Export functionality
    bool exportMetrics(const std::string& filepath,
                     const std::string& format = "csv",
                     const std::vector<std::string>& fields = {});
    bool exportProfiles(const std::string& filepath,
                       const std::string& format = "json");
    bool exportAlerts(const std::string& filepath,
                     const std::string& format = "json");
    
    // Profiling session management
    class ProfilingSession {
    public:
        ProfilingSession(const std::string& name);
        ~ProfilingSession();
        
        void start();
        void stop();
        bool isActive() const { return isActive_; }
        
        std::chrono::duration<double> getDuration() const;
        PerformanceMetrics getMetrics() const { return currentMetrics_; }
        
        std::string getName() const { return name_; }
        std::chrono::system_clock::time_point getStartTime() const { return startTime_; }
        
    private:
        std::string name_;
        bool isActive_;
        PerformanceMetrics currentMetrics_;
        std::chrono::system_clock::time_point startTime_;
    };
    
    ProfilingSession* startProfilingSession(const std::string& name);
    void stopProfilingSession(const std::string& name);
    bool hasProfilingSession(const std::string& name) const;
    ProfilingSession* getProfilingSession(const std::string& name);
    std::vector<std::string> getProfilingSessionNames() const;
    
    // Data export
    struct ExportOptions {
        std::string format;                    // "csv", "json", "xml", "sqlite"
        std::vector<std::string> fields;       // Fields to export
        std::string compression;               // "none", "gzip", "bz2"
        bool includeMetadata;                 // Include metadata in export
        std::string dateFormat;               // Date format for timestamps
        std::string timeZone;                 // Timezone for timestamps
        
        ExportOptions() : format("csv"), compression("none"), includeMetadata(true),
                        dateFormat("ISO"), timeZone("UTC") {}
    };
    
    bool exportData(const std::string& filepath, const ExportOptions& options = ExportOptions());
    
    // System monitoring
    class SystemMonitor {
    public:
        SystemMonitor();
        ~SystemMonitor();
        
        // System resource monitoring
        struct SystemResources {
            double cpuUsage;              // CPU usage percentage
            double memoryUsage;           // Memory usage in MB
            double memoryUsagePercent;    // Memory usage percentage
            size_t totalMemory;           // Total memory in MB
            uint64_t diskAvailable;       // Available disk space in bytes
            uint64_t diskTotal;           // Total disk space in bytes
            uint64_t networkSent;         // Network bytes sent
            uint64_t networkReceived;     // Network bytes received
            std::chrono::system_clock::time_point timestamp;
        };
        
        SystemResources getSystemResources() const;
        bool isSystemResourcesAvailable() const;
        
        // Process monitoring
        struct ProcessInfo {
            uint64_t pid;                 // Process ID
            std::string name;             // Process name
            std::string command;          // Command line
            double cpuUsage;              // CPU usage percentage
            double memoryUsage;           // Memory usage in MB
            uint64_t startTime;           // Process start time (epoch)
            std::string user;             // User who owns the process
            std::vector<std::string> threads; // Thread names
            
            ProcessInfo() : pid(0), cpuUsage(0.0), memoryUsage(0.0), startTime(0) {}
        };
        
        std::vector<ProcessInfo> getProcessList() const;
        ProcessInfo getProcessInfo(uint64_t pid) const;
        
        // Network monitoring
        struct NetworkInfo {
            std::string interface;        // Network interface name
            std::string ipAddress;        // IP address
            std::string macAddress;       // MAC address
            double uploadSpeed;           // Upload speed in MB/s
            double downloadSpeed;         // Download speed in MB/s
            uint64_t packetsSent;         // Packets sent
            uint64_t packetsReceived;     // Packets received
            uint64_t bytesSent;           // Bytes sent
            uint64_t bytesReceived;       // Bytes received
            
            NetworkInfo() : uploadSpeed(0.0), downloadSpeed(0.0),
                          packetsSent(0), packetsReceived(0),
                          bytesSent(0), bytesReceived(0) {}
        };
        
        std::vector<NetworkInfo> getNetworkInfo() const;
        
        // System performance analysis
        struct SystemPerformanceAnalysis {
            double averageCpuUsage;        // Average CPU usage
            double peakCpuUsage;           // Peak CPU usage
            double averageMemoryUsage;     // Average memory usage
            double peakMemoryUsage;        // Peak memory usage
            double diskUsagePercent;       // Disk usage percentage
            double networkUsage;           // Network usage in MB/s
            uint64_t totalProcesses;       // Total number of processes
            uint64_t zombieProcesses;      // Number of zombie processes
            
            std::string systemStatus;      // Overall system status
            std::vector<std::string> issues; // Identified issues
            
            SystemPerformanceAnalysis() : averageCpuUsage(0.0), peakCpuUsage(0.0),
                                        averageMemoryUsage(0.0), peakMemoryUsage(0.0),
                                        diskUsagePercent(0.0), networkUsage(0.0),
                                        totalProcesses(0), zombieProcesses(0) {}
        };
        
        SystemPerformanceAnalysis analyzeSystemPerformance() const;
        
        // System alerts
        bool checkSystemHealth() const;
        std::vector<std::string> getSystemIssues() const;
        
    private:
        mutable std::mutex systemMutex_;
    };
    
    SystemMonitor& getSystemMonitor() { return systemMonitor_; }
    
    // Debug and development tools
    class DebugTools {
    public:
        DebugTools();
        ~DebugTools();
        
        // Memory debugging
        struct MemoryDebugInfo {
            size_t totalAllocated;        // Total memory allocated
            size_t currentUsage;         // Current memory usage
            size_t peakUsage;            // Peak memory usage
            size_t fragmentation;        // Memory fragmentation
            uint64_t allocationCount;    // Number of allocations
            uint64_t deallocationCount;  // Number of deallocations
            
            std::map<std::string, size_t> allocationSizes; // Allocation sizes by type
            std::vector<std::string> allocationStackTraces; // Stack traces for allocations
            
            MemoryDebugInfo() : totalAllocated(0), currentUsage(0),
                              peakUsage(0), fragmentation(0),
                              allocationCount(0), deallocationCount(0) {}
        };
        
        MemoryDebugInfo getMemoryDebugInfo() const;
        void triggerMemoryCheck(std::shared_ptr<Brain> brain);
        
        // CPU debugging
        struct CpuDebugInfo {
            std::vector<std::pair<std::chrono::steady_clock::time_point, double>> cpuHistory; // CPU usage history
            double averageCpu;           // Average CPU usage
            double peakCpu;              // Peak CPU usage
            uint64_t sampleCount;        // Number of samples
            double lastSample;           // Last CPU sample
            
            CpuDebugInfo() : averageCpu(0.0), peakCpu(0.0), sampleCount(0),
                           lastSample(0.0) {}
        };
        
        CpuDebugInfo getCpuDebugInfo() const;
        void sampleCpuUsage();
        
        // Network debugging
        struct NetworkDebugInfo {
            std::vector<std::pair<std::chrono::steady_clock::time_point, double>> uploadHistory; // Upload history
            std::vector<std::pair<std::chrono::steady_clock::time_point, double>> downloadHistory; // Download history
            double averageUpload;        // Average upload speed
            double averageDownload;      // Average download speed
            uint64_t sampleCount;        // Number of samples
            
            NetworkDebugInfo() : averageUpload(0.0), averageDownload(0.0), sampleCount(0) {}
        };
        
        NetworkDebugInfo getNetworkDebugInfo() const;
        void sampleNetworkUsage();
        
        // File debugging
        struct FileDebugInfo {
            std::vector<std::string> openFiles;        // List of open files
            uint64_t totalOpenFiles;                // Total number of open files
            uint64_t fileReadOperations;            // Number of file read operations
            uint64_t fileWriteOperations;           // Number of file write operations
            uint64_t fileErrors;                    // Number of file errors
            
            FileDebugInfo() : totalOpenFiles(0), fileReadOperations(0),
                            fileWriteOperations(0), fileErrors(0) {}
        };
        
        FileDebugInfo getFileDebugInfo() const;
        
        // Thread debugging
        struct ThreadDebugInfo {
            std::vector<std::string> activeThreads;   // List of active threads
            uint64_t totalThreads;                  // Total number of threads
            uint64_t daemonThreads;                // Number of daemon threads
            uint64_t userThreads;                   // Number of user threads
            
            ThreadDebugInfo() : totalThreads(0), daemonThreads(0), userThreads(0) {}
        };
        
        ThreadDebugInfo getThreadDebugInfo() const;
        
        // Process debugging
        struct ProcessDebugInfo {
            std::vector<uint64_t> childProcesses;   // Child process IDs
            uint64_t totalChildProcesses;          // Total number of child processes
            uint64_t zombieProcesses;              // Number of zombie processes
            uint64_t stoppedProcesses;            // Number of stopped processes
            
            ProcessDebugInfo() : totalChildProcesses(0), zombieProcesses(0),
                               stoppedProcesses(0) {}
        };
        
        ProcessDebugInfo getProcessDebugInfo() const;
        
        // System call debugging
        struct SystemCallDebugInfo {
            std::map<std::string, uint64_t> systemCallCounts; // System call counts
            uint64_t totalSystemCalls;                      // Total system calls
            double systemCallErrorRate;                    // Error rate for system calls
            
            SystemCallDebugInfo() : totalSystemCalls(0), systemCallErrorRate(0.0) {}
        };
        
        SystemCallDebugInfo getSystemCallDebugInfo() const;
        
        // Performance debugging
        struct PerformanceDebugInfo {
            std::vector<std::pair<std::chrono::steady_clock::time_point, PerformanceMetrics>> performanceHistory; // Performance history
            double averageCpuUsage;                          // Average CPU usage
            double averageMemoryUsage;                       // Average memory usage
            double averageSimulationSpeed;                    // Average simulation speed
            
            std::vector<std::string> performanceIssues;      // Identified performance issues
            
            PerformanceDebugInfo() : averageCpuUsage(0.0), averageMemoryUsage(0.0),
                                   averageSimulationSpeed(0.0) {}
        };
        
        PerformanceDebugInfo getPerformanceDebugInfo() const;
        
        // Debug mode control
        void enableDebugMode(bool enable);
        bool isDebugMode() const { return debugMode_; }
        
        // Debug output
        void setDebugOutput(std::ostream& output);
        std::ostream& getDebugOutput();
        void logDebugMessage(const std::string& message);
        void logDebugMessage(const std::string& message, const std::string& category);
        
        // Debug filtering
        void setDebugLevel(int level);
        int getDebugLevel() const { return debugLevel_; }
        bool shouldLogDebug(int level) const;
        
        // Debug timing
        struct DebugTiming {
            std::chrono::steady_clock::time_point startTime;
            std::chrono::duration<double> totalTime;
            std::map<std::string, std::chrono::duration<double>> phaseTimes;
            std::map<std::string, std::chrono::duration<double>> operationTimes;
            
            DebugTiming() : totalTime(0), startTime(std::chrono::steady_clock::now()) {}
        };
        
        DebugTiming startDebugTiming(const std::string& name);
        void stopDebugTiming(const std::string& name);
        double getDebugTiming(const std::string& name) const;
        std::string getDebugTimingReport() const;
        
    private:
        bool debugMode_;
        int debugLevel_;
        std::ostream* debugOutput_;
        std::map<std::string, std::chrono::steady_clock::time_point> timingStartTimes_;
        std::map<std::string, std::chrono::duration<double>> timingAccumulatedTimes_;
        mutable std::mutex debugMutex_;
    };
    
    DebugTools& getDebugTools() { return debugTools_; }
    
private:
    bool isMonitoring_;
    PerformanceMetrics currentMetrics_;
    std::vector<PerformanceMetrics> metricsHistory_;
    Profiler profiler_;
    Benchmark benchmark_;
    AlertSystem alertSystem_;
    SystemMonitor systemMonitor_;
    DebugTools debugTools_;
    
    std::vector<std::unique_ptr<Visualization>> visualizations_;
    
    std::map<std::string, ProfilingSession*> profilingSessions_;
    
    ResourceLimits resourceLimits_;
    
    std::shared_ptr<Brain> monitoredBrain_;
    std::chrono::steady_clock::time_point monitoringStart_;
    
    mutable std::mutex metricsMutex_;
    mutable std::mutex visualizationsMutex_;
    mutable std::mutex profilingSessionsMutex_;
    
    // Update thread function
    void updateThreadFunction();
    
    // System resource monitoring
    void updateSystemResources();
    
    // Performance metrics calculation
    void calculatePerformanceMetrics();
    
    // Alert checking
    void checkAlerts();
    
    // Visualization updates
    void updateVisualizations();
};

// Convenience functions
namespace Performance {
    // Global performance monitor instance
    PerformanceMonitor& getMonitor();
    
    // Performance monitoring control
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    bool isMonitoring();
    
    // Performance metrics access
    PerformanceMonitor::PerformanceMetrics getCurrentMetrics();
    std::vector<PerformanceMonitor::PerformanceMetrics> getMetricsHistory(size_t limit = 1000);
    
    // Performance analysis
    PerformanceMonitor::PerformanceAnalysis analyzePerformance(const std::string& timeRange = "last_10min");
    bool isPerformanceAcceptable();
    
    // Benchmarking
    void addBenchmark(const std::string& name,
                     const std::function<void()>& benchmarkFunc,
                     const std::map<std::string, std::string>& parameters = {});
    
    PerformanceMonitor::BenchmarkResult runBenchmark(const std::string& name,
                                                      const std::map<std::string, std::string>& parameters = {});
    
    // Alert system
    void addAlert(const PerformanceMonitor::AlertSystem::Alert& alert);
    void addAlert(PerformanceMonitor::AlertSystem::AlertLevel level,
                 const std::string& message);
    
    std::vector<PerformanceMonitor::AlertSystem::Alert> getAlerts(
        PerformanceMonitor::AlertSystem::AlertLevel level = PerformanceMonitor::AlertSystem::AlertLevel::Info);
    
    // Visualization
    void addVisualization(std::unique_ptr<PerformanceMonitor::Visualization> viz);
    
    // Export
    bool exportMetrics(const std::string& filepath,
                      const PerformanceMonitor::ExportOptions& options = PerformanceMonitor::ExportOptions());
    
    // Profiling
    PerformanceMonitor::ProfilingSession* startProfilingSession(const std::string& name);
    void stopProfilingSession(const std::string& name);
    
    // Debug
    void enableDebugMode(bool enable);
    PerformanceMonitor::DebugTools& getDebugTools();
}

// Global performance monitor instance
inline PerformanceMonitor g_performanceMonitor;

// Global performance namespace functions
inline PerformanceMonitor& Performance::getMonitor() {
    return g_performanceMonitor;
}

inline void Performance::startMonitoring(std::shared_ptr<Brain> brain) {
    g_performanceMonitor.startMonitoring(brain);
}

inline void Performance::stopMonitoring() {
    g_performanceMonitor.stopMonitoring();
}

inline bool Performance::isMonitoring() {
    return g_performanceMonitor.isMonitoring();
}

inline PerformanceMonitor::PerformanceMetrics Performance::getCurrentMetrics() {
    return g_performanceMonitor.getCurrentMetrics();
}

inline std::vector<PerformanceMonitor::PerformanceMetrics> Performance::getMetricsHistory(size_t limit) {
    return g_performanceMonitor.getHistory(limit);
}

inline PerformanceMonitor::PerformanceAnalysis Performance::analyzePerformance(const std::string& timeRange) {
    return g_performanceMonitor.analyzePerformance(timeRange);
}

inline bool Performance::isPerformanceAcceptable() {
    return g_performanceMonitor.isPerformanceAcceptable();
}

inline void Performance::addBenchmark(const std::string& name,
                                     const std::function<void()>& benchmarkFunc,
                                     const std::map<std::string, std::string>& parameters) {
    g_performanceMonitor.getBenchmark().addBenchmark(name, benchmarkFunc, parameters);
}

inline PerformanceMonitor::BenchmarkResult Performance::runBenchmark(const std::string& name,
                                                                   const std::map<std::string, std::string>& parameters) {
    return g_performanceMonitor.getBenchmark().runBenchmark(name, parameters);
}

inline void Performance::addAlert(const PerformanceMonitor::AlertSystem::Alert& alert) {
    g_performanceMonitor.getAlertSystem().addAlert(alert);
}

inline void Performance::addAlert(PerformanceMonitor::AlertSystem::AlertLevel level,
                                 const std::string& message) {
    g_performanceMonitor.getAlertSystem().addAlert(level, message);
}

inline std::vector<PerformanceMonitor::AlertSystem::Alert> Performance::getAlerts(
    PerformanceMonitor::AlertSystem::AlertLevel level) {
    return g_performanceMonitor.getAlertSystem().getAlerts(level);
}

inline void Performance::addVisualization(std::unique_ptr<PerformanceMonitor::Visualization> viz) {
    g_performanceMonitor.addVisualization(std::move(viz));
}

inline bool Performance::exportMetrics(const std::string& filepath,
                                      const PerformanceMonitor::ExportOptions& options) {
    return g_performanceMonitor.exportMetrics(filepath, options);
}

inline PerformanceMonitor::ProfilingSession* Performance::startProfilingSession(const std::string& name) {
    return g_performanceMonitor.startProfilingSession(name);
}

inline void Performance::stopProfilingSession(const std::string& name) {
    g_performanceMonitor.stopProfilingSession(name);
}

inline void Performance::enableDebugMode(bool enable) {
    g_performanceMonitor.getDebugTools().enableDebugMode(enable);
}

inline PerformanceMonitor::DebugTools& Performance::getDebugTools() {
    return g_performanceMonitor.getDebugTools();
}

} // namespace nlm