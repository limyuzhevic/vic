// Comprehensive profiling utilities for NLM
// CPU, memory, I/O, and network profiling tools

#pragma once

#include "PerformanceMonitor.hpp"
#include <chrono>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace nlm {

// System information utilities
class SystemInfo {
public:
    static SystemInfo& getInstance() {
        static SystemInfo instance;
        return instance;
    }

    void initialize();
    void update();
    
    uint32_t getCpuCount() const { return cpu_count_; }
    uint64_t getTotalMemoryBytes() const { return total_memory_bytes_; }
    uint64_t getAvailableMemoryBytes() const { return available_memory_bytes_; }
    double getSystemLoadAverage() const { return system_load_average_; }
    std::string getSystemUptime() const { return system_uptime_; }
    
    std::unordered_map<std::string, double> getProcessCpuTimes() const { return process_cpu_times_; }
    std::unordered_map<std::string, uint64_t> getProcessMemoryUsage() const { return process_memory_usage_; }

private:
    SystemInfo() = default;
    
    uint32_t cpu_count_;
    uint64_t total_memory_bytes_;
    uint64_t available_memory_bytes_;
    double system_load_average_;
    std::string system_uptime_;
    std::unordered_map<std::string, double> process_cpu_times_;
    std::unordered_map<std::string, uint64_t> process_memory_usage_;
    
    mutable std::mutex mutex_;
};

// CPU profiler with detailed sampling
class DetailedCpuProfiler {
public:
    DetailedCpuProfiler();
    ~DetailedCpuProfiler();

    void startSampling(int sampling_interval_ms = 100);
    void stopSampling();
    bool isSampling() const { return sampling_; }

    void profileFunctionEntry(const std::string& function_name, 
                            const std::string& file_path, 
                            int line_number,
                            int thread_id = 0);
    void profileFunctionExit(const std::string& function_name);
    void profileMemoryAllocation(size_t size, const std::string& allocation_site);
    void profileMemoryDeallocation(size_t size, const std::string& allocation_site);

    std::vector<CpuProfile> getProfileData() const { return cpu_profiles_; }
    std::vector<MemoryProfile> getMemoryProfileData() const { return memory_profiles_; }

    void saveProfile(const std::string& directory) const;
    void saveDetailedReport(const std::string& output_file) const;

    double getAverageCpuUsage() const;
    double getPeakCpuUsage() const;
    size_t getTotalMemoryAllocated() const;
    size_t getTotalMemoryDeallocated() const;

private:
    bool sampling_;
    int sampling_interval_ms_;
    std::thread sampling_thread_;
    std::atomic<bool> sampling_running_;
    
    mutable std::mutex profiles_mutex_;
    std::vector<CpuProfile> cpu_profiles_;
    std::vector<MemoryProfile> memory_profiles_;
    
    std::unordered_map<std::string, int> function_call_counts_;
    std::unordered_map<std::string, double> function_total_times_;
    std::unordered_map<std::string, double> function_self_times_;
    std::unordered_map<std::string, int> memory_allocations_;
    std::unordered_map<std::string, size_t> memory_allocation_sizes_;

    void samplingLoop();
    std::string getCurrentThreadName() const;
    int getCurrentThreadId() const;
    std::string getCurrentFunctionName() const;
    std::string getCallStack() const;
    uint64_t getCurrentCycleCount() const;
    uint64_t getCurrentCacheMissCount() const;
};

// I/O profiler for file operations
class IoProfiler {
public:
    IoProfiler();
    ~IoProfiler();

    void startProfiling(int buffer_size = 4096);
    void stopProfiling();
    bool isProfiling() const { return profiling_; }

    void profileRead(const std::string& file_path, size_t bytes, 
                    double duration, int thread_id = 0);
    void profileWrite(const std::string& file_path, size_t bytes,
                     double duration, int thread_id = 0);
    void profileSeek(const std::string& file_path, off_t position,
                    double duration, int thread_id = 0);

    std::vector<IoProfile> getReadProfiles() const { return read_profiles_; }
    std::vector<IoProfile> getWriteProfiles() const { return write_profiles_; }
    std::vector<IoProfile> getSeekProfiles() const { return seek_profiles_; }

    void saveProfiles(const std::string& output_file) const;
    double getTotalReadBytes() const;
    double getTotalWriteBytes() const;
    double getAverageReadLatency() const;
    double getAverageWriteLatency() const;

private:
    bool profiling_;
    int buffer_size_;
    mutable std::mutex profiles_mutex_;
    
    std::vector<IoProfile> read_profiles_;
    std::vector<IoProfile> write_profiles_;
    std::vector<IoProfile> seek_profiles_;

    void setupFileMonitoring();
    void cleanupFileMonitoring();
    std::string getCurrentOperation() const;
};

// Network profiler for socket operations
class NetworkProfiler {
public:
    NetworkProfiler();
    ~NetworkProfiler();

    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return profiling_; }

    void profileTcpConnect(const std::string& host, uint16_t port,
                          double duration, int thread_id = 0);
    void profileTcpSend(const std::string& endpoint, size_t bytes,
                      double duration, int thread_id = 0);
    void profileTcpReceive(const std::string& endpoint, size_t bytes,
                         double duration, int thread_id = 0);
    void profileUdpSend(const std::string& host, uint16_t port,
                      size_t bytes, double duration, int thread_id = 0);

    std::vector<NetworkProfile> getTcpConnectProfiles() const { return tcp_connect_profiles_; }
    std::vector<NetworkProfile> getTcpSendProfiles() const { return tcp_send_profiles_; }
    std::vector<NetworkProfile> getTcpReceiveProfiles() const { return tcp_receive_profiles_; }
    std::vector<NetworkProfile> getUdpSendProfiles() const { return udp_send_profiles_; }

    void saveProfiles(const std::string& output_file) const;
    double getAverageNetworkLatency() const;
    double getThroughputMbps() const;

private:
    bool profiling_;
    mutable std::mutex profiles_mutex_;
    
    std::vector<NetworkProfile> tcp_connect_profiles_;
    std::vector<NetworkProfile> tcp_send_profiles_;
    std::vector<NetworkProfile> tcp_receive_profiles_;
    std::vector<NetworkProfile> udp_send_profiles_;

    void setupNetworkMonitoring();
    void cleanupNetworkMonitoring();
    std::string getNetworkEndpoint(const sockaddr_in& addr) const;
};

// GPU profiler (placeholder for future implementation)
class GpuProfiler {
public:
    GpuProfiler() { /* TODO: Initialize GPU monitoring */ }
    ~GpuProfiler() { /* TODO: Cleanup GPU monitoring */ }

    void startProfiling() { /* TODO: Start GPU profiling */ }
    void stopProfiling() { /* TODO: Stop GPU profiling */ }
    bool isProfiling() const { return false; } // TODO: Implement

    void profileKernelLaunch(const std::string& kernel_name, uint32_t grid_size,
                            uint32_t block_size, double execution_time,
                            uint64_t memory_used, int device_id = 0);

    // TODO: Add GPU profiling methods and data structures
    void saveProfiles(const std::string& output_file) const { /* TODO: Implement */ }
    double getAverageKernelExecutionTime() const { return 0.0; } // TODO: Implement
    uint64_t getTotalGpuMemoryUsed() const { return 0; } // TODO: Implement

private:
    // TODO: Add GPU profiling state
};

// Advanced memory profiler with leak detection
class AdvancedMemoryProfiler {
public:
    AdvancedMemoryProfiler();
    ~AdvancedMemoryProfiler();

    void startTracking(size_t tracking_threshold = 1024);  // Track allocations > threshold bytes
    void stopTracking();
    bool isTracking() const { return tracking_; }

    void trackAllocation(size_t size, const std::string& allocation_site,
                        const std::string& stack_trace = "");
    void trackDeallocation(size_t size, const std::string& allocation_site);

    std::vector<MemoryProfile> getLeakCandidates() const;
    std::vector<MemoryProfile> getAllocationHistory() const { return allocation_history_; }
    std::vector<MemoryProfile> getDeallocationHistory() const { return deallocation_history_; }

    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocatedMemory() const;
    size_t getTotalDeallocatedMemory() const;
    size_t getMemoryLeakSize() const;

    void enableLeakDetection(bool enable) { leak_detection_enabled_ = enable; }
    void setLeakThreshold(double threshold) { leak_threshold_ = threshold; }
    void saveLeakReport(const std::string& output_file) const;

private:
    bool tracking_;
    bool leak_detection_enabled_;
    double leak_threshold_;
    size_t tracking_threshold_;
    
    mutable std::mutex tracking_mutex_;
    
    std::vector<MemoryProfile> allocation_history_;
    std::vector<MemoryProfile> deallocation_history_;
    std::vector<MemoryProfile> leak_candidates_;
    
    std::unordered_map<std::string, size_t> allocation_counts_;
    std::unordered_map<std::string, size_t> allocation_sizes_;
    std::unordered_map<std::string, size_t> deallocation_counts_;
    std::unordered_map<std::string, size_t> deallocation_sizes_;

    void checkForLeaks();
    bool isLikelyLeak(const MemoryProfile& alloc, const MemoryProfile& dealloc) const;
    std::string analyzeStackTrace(const std::string& stack_trace) const;
};

// Integration layer for NLM specific profiling
class NlmSpecificProfiler {
public:
    NlmSpecificProfiler();
    ~NlmSpecificProfiler();

    void startProfiling(const std::shared_ptr<Brain>& brain);
    void stopProfiling();

    void profileNeuronActivity(uint64_t neuron_id, double potential, 
                             bool is_firing, double time_step);
    void profileSynapticActivity(uint64_t synapse_id, double weight,
                               bool is_strong, double change_amount);
    void profileMemoryOperation(const std::string& operation_type,
                              size_t size, uint64_t address);
    void profileEventProcessing(const std::string& event_type, 
                              double processing_time, int queue_size);

    std::vector<MemoryProfile> getNeuronActivityProfiles() const { return neuron_activity_profiles_; }
    std::vector<MemoryProfile> getSynapticActivityProfiles() const { return synaptic_activity_profiles_; }

    void saveNlmSpecificProfiles(const std::string& output_directory) const;
    void generatePerformanceInsights(const std::string& output_file) const;

private:
    std::shared_ptr<Brain> brain_;
    bool profiling_enabled_;
    
    mutable std::mutex profiles_mutex_;
    
    std::vector<MemoryProfile> neuron_activity_profiles_;
    std::vector<MemoryProfile> synaptic_activity_profiles_;
    std::vector<MemoryProfile> memory_operation_profiles_;
    std::vector<MemoryProfile> event_processing_profiles_;

    std::string getNeuronActivityType(double potential, bool is_firing) const;
    std::string getSynapticActivityType(double weight, bool is_strong) const;
};

// Performance benchmarking framework
class BenchmarkFramework {
public:
    BenchmarkFramework();
    ~BenchmarkFramework();

    void registerBenchmark(const std::string& name, 
                          std::function<void()> benchmark_func,
                          const BenchmarkConfig& config);

    void runBenchmark(const std::string& name);
    void runAllBenchmarks();
    
    std::vector<RegressionData> getRegressionResults() const { return regression_results_; }
    std::unordered_map<std::string, double> getBenchmarkResults() const { return benchmark_results_; }

    void setBaseline(const std::string& benchmark_name, double baseline_value);
    double getBaseline(const std::string& benchmark_name) const;
    
    void saveBenchmarkReport(const std::string& output_file) const;
    void saveRegressionReport(const std::string& output_file) const;

    void enableAutoRegressionDetection(bool enable) { auto_regression_detection_ = enable; }
    void setRegressionThreshold(double threshold) { regression_threshold_ = threshold; }

private:
    std::unordered_map<std::string, std::function<void()>> benchmark_functions_;
    std::unordered_map<std::string, BenchmarkConfig> benchmark_configs_;
    std::unordered_map<std::string, double> benchmark_results_;
    std::unordered_map<std::string, double> benchmark_baselines_;
    
    std::vector<RegressionData> regression_results_;
    
    bool auto_regression_detection_;
    double regression_threshold_;
    
    mutable std::mutex benchmark_mutex_;

    void checkForRegressions();
    double measureBenchmarkPerformance(const std::string& name);
};

// Performance regression detector
class PerformanceRegressionDetector {
public:
    PerformanceRegressionDetector();
    ~PerformanceRegressionDetector();

    void addBaselineMeasurement(const std::string& benchmark_name,
                               double performance_value,
                               const std::string& timestamp);

    void addCurrentMeasurement(const std::string& benchmark_name,
                              double performance_value,
                              const std::string& timestamp);

    std::vector<RegressionData> detectRegressions() const;
    double calculateRegressionPercentage(double baseline, double current) const;

    void setRegressionThreshold(double threshold) { regression_threshold_ = threshold; }
    void enableHistoricalAnalysis(bool enable) { historical_analysis_enabled_ = enable; }

    void saveRegressionAnalysis(const std::string& output_file) const;

private:
    struct PerformanceMeasurement {
        std::string benchmark_name;
        double performance_value;
        std::string timestamp;
        std::string git_commit;
        std::string branch_name;
    };

    std::vector<PerformanceMeasurement> baseline_measurements_;
    std::vector<PerformanceMeasurement> current_measurements_;
    
    double regression_threshold_;
    bool historical_analysis_enabled_;
    
    mutable std::mutex regression_mutex_;

    std::string getCurrentGitCommit() const;
    std::string getCurrentBranch() const;
    std::vector<PerformanceMeasurement> loadHistoricalData() const;
};

// Performance optimization assistant
class PerformanceOptimizationAssistant {
public:
    PerformanceOptimizationAssistant();
    ~PerformanceOptimizationAssistant();

    std::vector<std::string> analyzeSystemState(const PerformanceMetrics& metrics) const;
    std::vector<std::string> generateOptimizationRecommendations(const PerformanceMetrics& metrics) const;
    std::vector<std::string> suggestPerformanceImprovements(const std::string& component) const;

    void applyOptimization(const std::string& optimization_type);
    bool isOptimizationSafe(const std::string& optimization_type) const;
    double estimatePerformanceGain(const std::string& optimization_type) const;

    void loadOptimizationPresets(const std::string& preset_file);
    void saveOptimizationPresets(const std::string& preset_file) const;

    void enableAutoOptimization(bool enable) { auto_optimization_enabled_ = enable; }
    void setOptimizationSafetyLevel(int level) { safety_level_ = level; }

private:
    std::unordered_map<std::string, std::vector<std::string>> optimization_recommendations_;
    std::unordered_map<std::string, double> optimization_effects_;
    
    bool auto_optimization_enabled_;
    int safety_level_;
    
    mutable std::mutex optimization_mutex_;

    void initializeOptimizationRecommendations();
    std::vector<std::string> getMemoryOptimizations() const;
    std::vector<std::string> getCpuOptimizations() const;
    std::vector<std::string> getIoOptimizations() const;
    std::vector<std::string> getSynapticOptimizations() const;
    std::vector<std::string> getEventQueueOptimizations() const;
};

// Cloud performance monitoring integration
class CloudPerformanceMonitor {
public:
    CloudPerformanceMonitor();
    ~CloudPerformanceMonitor();

    void connect(const std::string& provider, const std::string& credentials);
    void disconnect();
    bool isConnected() const { return is_connected_; }

    void sendMetrics(const std::vector<PerformanceMetrics>& metrics);
    void sendProfiles(const std::vector<CpuProfile>& profiles);
    void sendAlerts(const std::vector<PerformanceAlert>& alerts);

    void setupAlertForwarding(const std::string& endpoint);
    void enableCloudLogging(bool enable) { cloud_logging_enabled_ = enable; }

    void enableDistributedTracing(bool enable) { distributed_tracing_enabled_ = enable; }
    void setSamplingRate(double rate) { sampling_rate_ = rate; }

private:
    bool is_connected_;
    std::string provider_;
    std::string credentials_;
    std::string endpoint_;
    
    bool cloud_logging_enabled_;
    bool distributed_tracing_enabled_;
    double sampling_rate_;
    
    mutable std::mutex cloud_mutex_;

    void sendToCloud(const std::string& data_type, const std::string& data);
};

// Main integration class
class NlmPerformanceProfiler {
public:
    static NlmPerformanceProfiler& getInstance() {
        static NlmPerformanceProfiler instance;
        return instance;
    }

    void initialize(const std::shared_ptr<Brain>& brain);
    void shutdown();

    // Start/stop all profiling
    void startAllProfiling();
    void stopAllProfiling();

    // Individual profilers access
    DetailedCpuProfiler& getCpuProfiler() { return cpu_profiler_; }
    IoProfiler& getIoProfiler() { return io_profiler_; }
    NetworkProfiler& getNetworkProfiler() { return network_profiler_; }
    GpuProfiler& getGpuProfiler() { return gpu_profiler_; }
    AdvancedMemoryProfiler& getMemoryProfiler() { return memory_profiler_; }
    NlmSpecificProfiler& getNlmProfiler() { return nlm_profiler_; }

    PerformanceMonitor& getPerformanceMonitor() { return performance_manager_.getMonitor(); }
    BenchmarkFramework& getBenchmarkFramework() { return benchmark_framework_; }
    PerformanceRegressionDetector& getRegressionDetector() { return regression_detector_; }
    PerformanceOptimizationAssistant& getOptimizationAssistant() { return optimization_assistant_; }

    // Integration with NLM systems
    void integrateWithBrain(const std::shared_ptr<Brain>& brain);
    void integrateWithLogger(const std::shared_ptr<Logger>& logger);
    void integrateWithConfig(const std::shared_ptr<Config>& config);

    // Save and load profiles
    void saveAllProfiles(const std::string& output_directory);
    void loadProfiles(const std::string& input_directory);

    // Report generation
    void generateComprehensiveReport(const std::string& output_file);
    void generateRealTimeDashboard(const std::string& output_file);

private:
    NlmPerformanceProfiler()
        : performance_manager_()
        , cpu_profiler_()
        , io_profiler_()
        , network_profiler_()
        , gpu_profiler_()
        , memory_profiler_()
        , nlm_profiler_()
        , benchmark_framework_()
        , regression_detector_()
        , optimization_assistant_()
        , cloud_monitor_()
    {
    }

    PerformanceManager performance_manager_;
    DetailedCpuProfiler cpu_profiler_;
    IoProfiler io_profiler_;
    NetworkProfiler network_profiler_;
    GpuProfiler gpu_profiler_;
    AdvancedMemoryProfiler memory_profiler_;
    NlmSpecificProfiler nlm_profiler_;
    BenchmarkFramework benchmark_framework_;
    PerformanceRegressionDetector regression_detector_;
    PerformanceOptimizationAssistant optimization_assistant_;
    CloudPerformanceMonitor cloud_monitor_;

    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Config> config_;
    bool profiling_enabled_;
};

}} // namespace nlm

// Global performance profiler instance
inline nlm::NlmPerformanceProfiler& getGlobalProfiler() {
    return nlm::NlmPerformanceProfiler::getInstance();
}

// Convenience macros for profiling
#define PROFILE_FUNCTION(name) 
    nlm::getGlobalProfiler().getCpuProfiler().profileFunction(name, [&]() {  // TODO: Get current function
        // Function body
    })

#define START_MONITORING() nlm::getGlobalProfiler().startAllProfiling()
#define STOP_MONITORING() nlm::getGlobalProfiler().stopAllProfiling()

#define RECORD_METRIC(metric) nlm::getGlobalProfiler().getPerformanceMonitor().recordMetrics(metric)

#define PROFILE_ALLOCATION(size, site) nlm::getGlobalProfiler().getCpuProfiler().profileMemoryAllocation(size, site)
#define PROFILE_DEALLOCATION(size, site) nlm::getGlobalProfiler().getCpuProfiler().profileMemoryDeallocation(size, site)

#define PROFILE_READ(file, bytes, duration) nlm::getGlobalProfiler().getIoProfiler().profileRead(file, bytes, duration)
#define PROFILE_WRITE(file, bytes, duration) nlm::getGlobalProfiler().getIoProfiler().profileWrite(file, bytes, duration)

#define PROFILE_NETWORK(endpoint, bytes, duration) nlm::getGlobalProfiler().getNetworkProfiler().profileTcpSend(endpoint, bytes, duration)