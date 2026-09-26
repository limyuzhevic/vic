// Performance Monitoring and Benchmarking Tools
// Header file for NLM's performance monitoring and benchmarking system

#ifndef NLM_PERFORMANCE_MONITORING_H
#define NLM_PERFORMANCE_MONITORING_H

#include <string>
#include <vector>
#include <chrono>
#include <limits>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>

namespace nlm {

class PerformanceMonitor;

// Macro for timing code blocks
#define NLM_PERFORMANCE_SCOPE(operation) \
    class NLMPerformanceScope { \
    public: \
        NLMPerformanceScope(const std::string& op) : operation_(op) { \
            PerformanceMonitor::getInstance().startMeasure(operation_); \
        } \
        ~NLMPerformanceScope() { \
            PerformanceMonitor::getInstance().stopMeasure(operation_); \
        } \
    private: \
        std::string operation_; \
    } nlm_perf_scope_##__LINE__(operation)

// Macro for simple benchmarking
#define NLM_BENCHMARK(operation) NLM_BENCHMARK_OBJ(operation, __LINE__)
#define NLM_BENCHMARK_OBJ(operation, line) NLM_BENCHMARK_INTERNAL(operation, line)
#define NLM_BENCHMARK_INTERNAL(operation, line) \
    class NLMBenchmark_##line { \
    public: \
        NLMBenchmark_##line() { \
            startTime_ = std::chrono::high_resolution_clock::now(); \
        } \
        ~NLMBenchmark_##line() { \
            auto endTime = std::chrono::high_resolution_clock::now(); \
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_); \
            double durationMs = static_cast<double>(duration.count()) / 1000.0; \
            NLM_LOG_INFO(std::string("Benchmark " #operation " completed in ") + std::to_string(durationMs) + "ms"); \
        } \
    private: \
        std::chrono::high_resolution_clock::time_point startTime_; \
    } nlm_benchmark_##line

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::string operation;
        double totalTimeMs;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        uint64_t callCount;
        uint64_t maxMemoryUsage;
        std::vector<double> recentTimes;
        
        PerformanceMetrics() : totalTimeMs(0.0), averageTimeMs(0.0), 
                              minTimeMs(std::numeric_limits<double>::max()),
                              maxTimeMs(0.0), callCount(0), maxMemoryUsage(0) {}
    };
    
    static PerformanceMonitor& getInstance();
    
    void startMeasure(const std::string& operation);
    void stopMeasure(const std::string& operation);
    
    const PerformanceMetrics& getMetrics(const std::string& operation) const;
    std::vector<std::string> getAllOperations() const;
    
    std::string generateReport() const;
    void reset();
    
    void saveReport(const std::string& filepath) const;
    
    std::string getCurrentTimestamp() const;
    
    void setValidationEnabled(bool enabled);
    bool isValidationEnabled() const;
    
    void addCustomValidator(const std::string& key, 
                           std::function<bool(const ConfigValue&)> validator);
    
    std::string validateAll() const;
    
private:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> operationTimers_;
    std::unordered_map<std::string, PerformanceMetrics> metrics_;
    bool validationEnabled_;
};

// Benchmarking utilities
class Benchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double durationMs;
        uint64_t iterations;
        double itemsPerSecond;
        std::string details;
    };
    
    Benchmark(const std::string& name);
    ~Benchmark();
    
    static std::string generateBenchmarkReport();
    static void saveBenchmarkReport(const std::string& filepath);
    static void clearResults();
    static std::vector<BenchmarkResult>& getResults();
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point startTime_;
    static std::vector<BenchmarkResult> results_;
};

// Convenience functions for performance profiling
void profileNeuralNetwork(const std::string& networkName, int steps);
void profilePlasticityRules(int iterations);
void generateSystemPerformanceReport();

// Global initialization functions
void initializePerformanceMonitoring();
void shutdownPerformanceMonitoring();

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_H