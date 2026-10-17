// Performance Monitoring and Benchmarking Tools
// 
// This system provides comprehensive performance monitoring and benchmarking
// capabilities for the NLM project:
// - Real-time performance metrics tracking
// - Memory usage monitoring and analysis
// - Benchmarking framework for configuration comparison
// - Automated performance testing
// - Results visualization and reporting
// - Resource utilization monitoring
// - Performance regression detection
// - Historical performance tracking

#ifndef NLM_PERFORMANCE_TOOLS_H
#define NLM_PERFORMANCE_TOOLS_H

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <cmath>
#include <functional>

#include "core/ErrorHandling.hpp"

namespace nlm {

// ============================================================================
// Performance Metrics Data Structures
// ============================================================================

struct PerformanceMetrics {
    // Timestamp and duration
    std::chrono::high_resolution_clock::time_point timestamp;
    double durationSeconds;
    
    // Computation metrics
    size_t stepsExecuted;
    double stepsPerSecond;
    double cpuUsagePercentage;
    
    // Memory metrics
    size_t memoryUsageBytes;
    size_t peakMemoryBytes;
    size_t memoryAllocationCount;
    size_t memoryDeallocationCount;
    
    // Neural metrics
    size_t neuronCount;
    size_t synapseCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t totalSpikeCount;
    
    // System metrics
    size_t threadCount;
    double systemLoad;
    double temperature; // CPU temperature if available
    
    // I/O metrics
    uint64_t bytesRead;
    uint64_t bytesWritten;
    size_t fileOpenCount;
    size_t fileCloseCount;
    
    // Configuration metrics
    std::string configurationHash;
    std::map<std::string, std::string> configParameters;
    
    PerformanceMetrics()
        : stepsExecuted(0), durationSeconds(0.0), stepsPerSecond(0.0), 
          cpuUsagePercentage(0.0), memoryUsageBytes(0), peakMemoryBytes(0),
          memoryAllocationCount(0), memoryDeallocationCount(0), neuronCount(0),
          synapseCount(0), activeNeuronCount(0), firingNeuronCount(0),
          averageFiringRate(0.0f), excitationInhibitionRatio(0.0f),
          totalSpikeCount(0), threadCount(0), systemLoad(0.0), 
          temperature(0.0), bytesRead(0), bytesWritten(0),
          fileOpenCount(0), fileCloseCount(0) {}
};

struct BenchmarkResult {
    std::string benchmarkName;
    std::string configurationDescription;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    // Overall performance metrics
    double totalDuration;
    double throughput; // steps per second
    double efficiency;  // relative to baseline
    
    // Memory efficiency
    size_t peakMemoryUsage;
    double memoryEfficiency; // memory per step
    
    // Neural performance
    float finalFiringRate;
    float learningSpeed; // learning per step
    float stability;    // consistency of performance
    
    // Resource utilization
    double averageCPUUsage;
    double peakCPUUsage;
    
    // Quality metrics
    float predictionAccuracy;
    float memoryRetention;
    float generalizationScore;
    
    // Comparison metrics
    std::vector<double> performanceHistory;
    std::vector<size_t> memoryHistory;
    
    // Error and failure metrics
    size_t errorCount;
    std::vector<std::string> errors;
    
    BenchmarkResult()
        : totalDuration(0.0), throughput(0.0), efficiency(1.0),
          memoryEfficiency(0.0), averageCPUUsage(0.0), peakCPUUsage(0.0),
          learningSpeed(0.0f), stability(0.0f), predictionAccuracy(0.0f),
          memoryRetention(0.0f), generalizationScore(0.0f),
          errorCount(0) {}
};

struct PerformanceProfile {
    std::string profileName;
    std::map<std::string, double> parameters;
    std::map<std::string, double> expectedMetrics;
    
    // Performance targets
    double minThroughput;
    size_t maxMemoryUsage;
    double maxCPUUsage;
    float minAccuracy;
    
    // Configuration constraints
    std::vector<std::string> requiredConfigs;
    std::vector<std::string> forbiddenConfigs;
    
    PerformanceProfile()
        : minThroughput(0.0), maxMemoryUsage(0), maxCPUUsage(0.0), 
          minAccuracy(0.0f) {}
};

// ============================================================================
// Performance Monitor
// ============================================================================

class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Start monitoring a specific brain
    void startMonitoring(std::shared_ptr<Brain> brain, const std::string& sessionId = "default") {
        std::lock_guard<std::mutex> lock(mutex);
        
        monitoringSessions[sessionId] = {
            brain, std::chrono::high_resolution_clock::now(),
            PerformanceMetrics(), 0, 0.0, 0.0
        };
        
        NLM_LOG_INFO("Started performance monitoring for session: " + sessionId);
    }
    
    // Stop monitoring a session
    void stopMonitoring(const std::string& sessionId) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = monitoringSessions.find(sessionId);
        if (it != monitoringSessions.end()) {
            collectMetrics(it->second);
            NLM_LOG_INFO("Stopped performance monitoring for session: " + sessionId);
            monitoringSessions.erase(it);
        }
    }
    
    // Get current metrics for a session
    PerformanceMetrics getCurrentMetrics(const std::string& sessionId) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = monitoringSessions.find(sessionId);
        if (it != monitoringSessions.end()) {
            return it->second.currentMetrics;
        }
        
        return PerformanceMetrics();
    }
    
    // Get historical metrics for a session
    std::vector<PerformanceMetrics> getHistoricalMetrics(const std::string& sessionId, 
                                                         size_t maxPoints = 100) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<PerformanceMetrics> history;
        auto it = monitoringHistory.find(sessionId);
        if (it != monitoringHistory.end()) {
            history = it->second;
            if (history.size() > maxPoints) {
                history.erase(history.begin(), history.begin() + (history.size() - maxPoints));
            }
        }
        
        return history;
    }
    
    // Update metrics for a session
    void updateMetrics(const std::string& sessionId, const PerformanceMetrics& metrics) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = monitoringSessions.find(sessionId);
        if (it != monitoringSessions.end()) {
            it->second.currentMetrics = metrics;
            it->second.currentMetrics.timestamp = std::chrono::high_resolution_clock::now();
            
            monitoringHistory[sessionId].push_back(metrics);
            if (monitoringHistory[sessionId].size() > 1000) {
                monitoringHistory[sessionId].erase(monitoringHistory[sessionId].begin());
            }
        }
    }
    
    // Get system-wide performance statistics
    std::map<std::string, double> getSystemStatistics() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::map<std::string, double> stats;
        
        size_t totalSessions = monitoringSessions.size();
        stats["active_sessions"] = static_cast<double>(totalSessions);
        
        if (totalSessions > 0) {
            double totalSteps = 0.0;
            double totalDuration = 0.0;
            
            for (const auto& pair : monitoringSessions) {
                totalSteps += pair.second.currentMetrics.stepsExecuted;
                totalDuration += pair.second.currentMetrics.durationSeconds;
            }
            
            stats["average_steps_per_session"] = totalSteps / totalSessions;
            stats["average_duration_per_session"] = totalDuration / totalSessions;
        }
        
        return stats;
    }
    
    // Clear all monitoring data
    void clearAllData() {
        std::lock_guard<std::mutex> lock(mutex);
        monitoringSessions.clear();
        monitoringHistory.clear();
        NLM_LOG_INFO("Cleared all performance monitoring data");
    }
    
    // Export metrics to file
    bool exportMetrics(const std::string& sessionId, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw IOError("Cannot open file for writing: " + filepath);
            }
            
            auto metrics = getHistoricalMetrics(sessionId);
            file << "Performance Metrics Export\n";
            file << "Generated: " << std::chrono::high_resolution_clock::now() << "\n";
            file << "Session ID: " + sessionId + "\n\n";
            
            file << "Timestamp,Duration,Steps,StepsPerSecond,CPU%,Memory(MB),Neurons,Synapses,\n";
            
            for (const auto& metric : metrics) {
                file << metric.timestamp << ","
                     << metric.durationSeconds << ","
                     << metric.stepsExecuted << ","
                     << metric.stepsPerSecond << ","
                     << metric.cpuUsagePercentage << ","
                     << (metric.memoryUsageBytes / (1024 * 1024)) << ","
                     << metric.neuronCount << ","
                     << metric.synapseCount << ",\n";
            }
            
            file.close();
            NLM_LOG_INFO("Exported performance metrics to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to export metrics: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    PerformanceMonitor() {}
    
    struct MonitoringSession {
        std::shared_ptr<Brain> brain;
        std::chrono::high_resolution_clock::time_point startTime;
        PerformanceMetrics currentMetrics;
        uint64_t stepsCompleted;
        double totalCPUUsage;
    };
    
    mutable std::mutex mutex;
    std::map<std::string, MonitoringSession> monitoringSessions;
    std::map<std::string, std::vector<PerformanceMetrics>> monitoringHistory;
    
    void collectMetrics(MonitoringSession& session) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - session.startTime).count();
        
        if (duration > 0.0) {
            session.currentMetrics.durationSeconds = duration;
            session.currentMetrics.stepsPerSecond = session.currentMetrics.stepsExecuted / duration;
            session.currentMetrics.cpuUsagePercentage = session.totalCPUUsage / duration;
        }
    }
    
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
};

// ============================================================================
// Benchmarking Framework
// ============================================================================

class BenchmarkFramework {
public:
    static BenchmarkFramework& getInstance() {
        static BenchmarkFramework instance;
        return instance;
    }
    
    // Run a benchmark with a specific configuration
    BenchmarkResult runBenchmark(const std::string& name,
                                 const std::shared_ptr<Config>& config,
                                 const std::function<std::shared_ptr<Brain>(const Config&)>& brainFactory,
                                 uint64_t steps,
                                 bool warmup = true) {
        BenchmarkResult result;
        result.benchmarkName = name;
        result.startTime = std::chrono::high_resolution_clock::now();
        result.configurationDescription = config->summary();
        
        NLM_LOG_INFO("Starting benchmark: " + name);
        
        try {
            // Create brain
            auto brain = brainFactory(config);
            brain->initialize();
            
            // Warmup phase
            if (warmup && steps > 100) {
                NLM_LOG_INFO("Performing warmup (10% of steps)...");
                auto warmupStart = std::chrono::high_resolution_clock::now();
                for (uint64_t i = 0; i < steps / 10; ++i) {
                    brain->step(i, i * 0.001);
                }
                auto warmupDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - warmupStart).count();
                NLM_LOG_INFO("Warmup completed in " + std::to_string(warmupDuration) + "s");
            }
            
            // Main benchmark run
            NLM_LOG_INFO("Running benchmark for " + std::to_string(steps) + " steps...");
            auto benchmarkStart = std::chrono::high_resolution_clock::now();
            
            for (uint64_t i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
                
                // Collect metrics periodically
                if (i % 100 == 0) {
                    collectBenchmarkMetrics(brain, result);
                }
            }
            
            auto benchmarkEnd = std::chrono::high_resolution_clock::now();
            result.endTime = benchmarkEnd;
            result.totalDuration = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
            
            if (result.totalDuration > 0.0) {
                result.throughput = steps / result.totalDuration;
            }
            
            // Collect final metrics
            collectBenchmarkMetrics(brain, result);
            
            // Calculate efficiency (normalized to 1.0 for baseline)
            double baselineThroughput = 1000.0; // Example baseline
            if (baselineThroughput > 0.0) {
                result.efficiency = result.throughput / baselineThroughput;
            }
            
            NLM_LOG_INFO("Benchmark completed: " + name);
            NLM_LOG_INFO("  Duration: " + std::to_string(result.totalDuration) + "s");
            NLM_LOG_INFO("  Throughput: " + std::to_string(result.throughput) + " steps/s");
            NLM_LOG_INFO("  Memory efficiency: " + std::to_string(result.memoryEfficiency) + " bytes/step");
            NLM_LOG_INFO("  Average CPU usage: " + std::to_string(result.averageCPUUsage) + "%");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Benchmark failed: " + std::string(e.what()));
            result.errors.push_back(std::string("Benchmark error: ") + e.what());
            result.errorCount++;
        }
        
        return result;
    }
    
    // Run multiple benchmarks and compare results
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<std::shared_ptr<Config>>& configs,
                                                    const std::function<std::shared_ptr<Brain>(const Config&)>& brainFactory,
                                                    uint64_t steps,
                                                    const std::string& suiteName = "Benchmark Suite") {
        std::vector<BenchmarkResult> results;
        
        NLM_LOG_INFO("Starting benchmark suite: " + suiteName + " with " + std::to_string(configs.size()) + " configurations");
        
        for (size_t i = 0; i < configs.size(); ++i) {
            std::string name = suiteName + " - Configuration " + std::to_string(i + 1);
            NLM_LOG_INFO("Running benchmark " + std::to_string(i + 1) + "/" + std::to_string(configs.size()));
            
            BenchmarkResult result = runBenchmark(name, configs[i], brainFactory, steps);
            results.push_back(result);
        }
        
        // Generate comparison report
        generateBenchmarkReport(results, suiteName);
        
        return results;
    }
    
    // Compare benchmark results against a baseline
    std::map<std::string, double> compareAgainstBaseline(const BenchmarkResult& newResult, 
                                                         const BenchmarkResult& baseline) {
        std::map<std::string, double> comparison;
        
        if (baseline.totalDuration > 0.0) {
            comparison["throughput_ratio"] = newResult.throughput / baseline.throughput;
        }
        
        if (baseline.peakMemoryUsage > 0) {
            comparison["memory_efficiency_ratio"] = 
                static_cast<double>(baseline.peakMemoryUsage) / newResult.peakMemoryUsage;
        }
        
        if (baseline.averageCPUUsage > 0.0) {
            comparison["cpu_efficiency_ratio"] = baseline.averageCPUUsage / newResult.averageCPUUsage;
        }
        
        if (baseline.finalFiringRate > 0.0f) {
            comparison["performance_ratio"] = newResult.finalFiringRate / baseline.finalFiringRate;
        }
        
        return comparison;
    }
    
    // Save benchmark results to file
    bool saveBenchmarkResults(const BenchmarkResult& result, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw IOError("Cannot open file for writing: " + filepath);
            }
            
            file << "Benchmark Results\n";
            file << "Generated: " << std::chrono::high_resolution_clock::now() << "\n\n";
            file << "Benchmark Name: " + result.benchmarkName + "\n";
            file << "Configuration: " + result.configurationDescription + "\n\n";
            
            file << "Performance Metrics:\n";
            file << "  Total Duration: " << result.totalDuration << "s\n";
            file << "  Throughput: " << result.throughput << " steps/s\n";
            file << "  Efficiency: " << (result.efficiency * 100) << "%\n";
            file << "  Peak Memory: " << result.peakMemoryUsage << " bytes\n";
            file << "  Memory Efficiency: " << result.memoryEfficiency << " bytes/step\n";
            file << "  Average CPU: " << result.averageCPUUsage << "%\n";
            file << "  Peak CPU: " << result.peakCPUUsage << "%\n";
            file << "  Final Firing Rate: " << result.finalFiringRate << "\n";
            file << "  Learning Speed: " << result.learningSpeed << "\n";
            file << "  Stability: " << result.stability << "\n";
            file << "  Prediction Accuracy: " << result.predictionAccuracy << "\n";
            file << "  Memory Retention: " << result.memoryRetention << "\n";
            file << "  Generalization Score: " << result.generalizationScore << "\n";
            file << "  Error Count: " << result.errorCount << "\n\n";
            
            if (!result.errors.empty()) {
                file << "Errors:\n";
                for (const auto& error : result.errors) {
                    file << "  - " << error << "\n";
                }
            }
            
            file.close();
            NLM_LOG_INFO("Benchmark results saved to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save benchmark results: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    BenchmarkFramework() {}
    
    void collectBenchmarkMetrics(std::shared_ptr<Brain> brain, BenchmarkResult& result) {
        // Collect current metrics
        PerformanceMetrics metrics;
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        metrics.stepsExecuted = result.benchmarkName.empty() ? 0 : result.peakMemoryUsage; // Placeholder
        metrics.memoryUsageBytes = 0; // Placeholder - would need actual memory tracking
        metrics.neuronCount = brain->getTotalNeuronCount();
        metrics.synapseCount = brain->getTotalSynapseCount();
        metrics.activeNeuronCount = brain->getActiveNeuronCount();
        metrics.firingNeuronCount = brain->getFiringNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        metrics.totalSpikeCount = brain->getTotalSpikeCount();
        
        // Update result metrics
        result.peakMemoryUsage = std::max(result.peakMemoryUsage, metrics.memoryUsageBytes);
        result.performanceHistory.push_back(metrics.stepsExecuted);
        result.memoryHistory.push_back(metrics.memoryUsageBytes);
        
        // Update running averages
        if (result.performanceHistory.size() > 1) {
            double sum = std::accumulate(result.performanceHistory.begin(), result.performanceHistory.end(), 0.0);
            result.throughput = sum / result.performanceHistory.size();
        }
    }
    
    void generateBenchmarkReport(const std::vector<BenchmarkResult>& results, const std::string& suiteName) {
        NLM_LOG_INFO("Benchmark suite report: " + suiteName);
        NLM_LOG_INFO("Benchmarks completed: " + std::to_string(results.size()));
        
        double fastestDuration = 1e9;
        double slowestDuration = 0.0;
        double avgThroughput = 0.0;
        size_t totalErrors = 0;
        
        for (const auto& result : results) {
            fastestDuration = std::min(fastestDuration, result.totalDuration);
            slowestDuration = std::max(slowestDuration, result.totalDuration);
            avgThroughput += result.throughput;
            totalErrors += result.errorCount;
        }
        
        if (!results.empty()) {
            avgThroughput /= results.size();
            
            NLM_LOG_INFO("Performance Summary:");
            NLM_LOG_INFO("  Fastest benchmark: " + std::to_string(fastestDuration) + "s");
            NLM_LOG_INFO("  Slowest benchmark: " + std::to_string(slowestDuration) + "s");
            NLM_LOG_INFO("  Average throughput: " + std::to_string(avgThroughput) + " steps/s");
            NLM_LOG_INFO("  Total errors: " + std::to_string(totalErrors));
        }
    }
    
    BenchmarkFramework(const BenchmarkFramework&) = delete;
    BenchmarkFramework& operator=(const BenchmarkFramework&) = delete;
};

// ============================================================================
// Performance Profiler
// ============================================================================

class PerformanceProfiler {
public:
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    // Start profiling a specific code section
    void startProfile(const std::string& profileName) {
        std::lock_guard<std::mutex> lock(mutex);
        
        ProfileData data;
        data.name = profileName;
        data.startTime = std::chrono::high_resolution_clock::now();
        data.stepCount = 0;
        
        activeProfiles[profileName] = data;
        NLM_LOG_DEBUG("Started profiling: " + profileName);
    }
    
    // Stop profiling a section
    ProfileResult stopProfile(const std::string& profileName) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = activeProfiles.find(profileName);
        if (it == activeProfiles.end()) {
            throw PerformanceError("No active profile found: " + profileName);
        }
        
        ProfileResult result;
        result.name = profileName;
        result.duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - it->second.startTime).count();
        result.stepCount = it->second.stepCount;
        
        if (result.duration > 0.0) {
            result.stepsPerSecond = result.stepCount / result.duration;
        }
        
        NLM_LOG_DEBUG("Stopped profiling: " + profileName + " (" + std::to_string(result.duration) + "s, " + std::to_string(result.stepsPerSecond) + " steps/s)");
        
        activeProfiles.erase(it);
        profileHistory.push_back(result);
        
        if (profileHistory.size() > 100) {
            profileHistory.erase(profileHistory.begin());
        }
        
        return result;
    }
    
    // Increment step count for a profile
    void incrementStepCount(const std::string& profileName) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = activeProfiles.find(profileName);
        if (it != activeProfiles.end()) {
            it->second.stepCount++;
        }
    }
    
    // Get all profile results
    std::vector<ProfileResult> getProfileHistory(size_t maxResults = 50) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<ProfileResult> results;
        size_t startIndex = std::max(0, static_cast<int>(profileHistory.size() - maxResults));
        
        for (size_t i = startIndex; i < profileHistory.size(); ++i) {
            results.push_back(profileHistory[i]);
        }
        
        return results;
    }
    
    // Get average performance across all profiles
    std::map<std::string, double> getAveragePerformance() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::map<std::string, double> averages;
        
        if (profileHistory.empty()) {
            return averages;
        }
        
        double totalDuration = 0.0;
        double totalStepsPerSecond = 0.0;
        
        for (const auto& profile : profileHistory) {
            totalDuration += profile.duration;
            totalStepsPerSecond += profile.stepsPerSecond;
        }
        
        averages["average_duration_seconds"] = totalDuration / profileHistory.size();
        averages["average_steps_per_second"] = totalStepsPerSecond / profileHistory.size();
        
        return averages;
    }
    
    // Clear all profiling data
    void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        profileHistory.clear();
        NLM_LOG_INFO("Cleared performance profiling history");
    }
    
private:
    PerformanceProfiler() {}
    
    struct ProfileData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        uint64_t stepCount;
    };
    
    mutable std::mutex mutex;
    std::map<std::string, ProfileData> activeProfiles;
    std::vector<ProfileResult> profileHistory;
    
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
};

// ============================================================================
// Performance Analysis
// ============================================================================

class PerformanceAnalyzer {
public:
    static PerformanceAnalyzer& getInstance() {
        static PerformanceAnalyzer instance;
        return instance;
    }
    
    // Analyze performance trends in metrics data
    std::map<std::string, double> analyzeTrends(const std::vector<PerformanceMetrics>& metrics) {
        std::map<std::string, double> analysis;
        
        if (metrics.size() < 2) {
            return analysis;
        }
        
        // Extract time series data
        std::vector<double> stepsSeries;
        std::vector<double> memorySeries;
        std::vector<double> timeSeries;
        
        for (const auto& metric : metrics) {
            stepsSeries.push_back(static_cast<double>(metric.stepsExecuted));
            memorySeries.push_back(static_cast<double>(metric.memoryUsageBytes));
            timeSeries.push_back(metric.durationSeconds);
        }
        
        // Calculate trend metrics
        analysis["steps_growth_rate"] = calculateGrowthRate(stepsSeries);
        analysis["memory_growth_rate"] = calculateGrowthRate(memorySeries);
        analysis["efficiency_improvement"] = calculateEfficiencyImprovement(metrics);
        analysis["stability_score"] = calculateStability(metrics);
        
        return analysis;
    }
    
    // Detect performance regressions
    std::vector<std::string> detectRegressions(const BenchmarkResult& current, 
                                               const BenchmarkResult& baseline) {
        std::vector<std::string> regressions;
        
        if (current.totalDuration > baseline.totalDuration * 1.5) {
            regressions.push_back("Significant performance degradation: duration increased by " + 
                                std::to_string((current.totalDuration / baseline.totalDuration - 1.0) * 100) + "%");
        }
        
        if (current.peakMemoryUsage > baseline.peakMemoryUsage * 1.2) {
            regressions.push_back("Memory usage increased by " + 
                                std::to_string((current.peakMemoryUsage - baseline.peakMemoryUsage) / baseline.peakMemoryUsage * 100) + "%");
        }
        
        if (current.averageCPUUsage > baseline.averageCPUUsage * 1.2) {
            regressions.push_back("CPU usage increased by " + 
                                std::to_string((current.averageCPUUsage - baseline.averageCPUUsage) / baseline.averageCPUUsage * 100) + "%");
        }
        
        if (current.efficiency < baseline.efficiency * 0.8) {
            regressions.push_back("Efficiency decreased by " + 
                                std::to_string((baseline.efficiency - current.efficiency) / baseline.efficiency * 100) + "%");
        }
        
        return regressions;
    }
    
    // Generate performance recommendations
    std::vector<std::string> generateRecommendations(const std::map<std::string, double>& analysis) {
        std::vector<std::string> recommendations;
        
        if (analysis.find("steps_growth_rate") != analysis.end() && analysis.at("steps_growth_rate") < 0.1) {
            recommendations.push_back("Performance growth is slow. Consider optimizing the simulation loop.");
        }
        
        if (analysis.find("memory_growth_rate") != analysis.end() && analysis.at("memory_growth_rate") > 1.0) {
            recommendations.push_back("Memory usage is growing rapidly. Consider memory pooling or sparse representations.");
        }
        
        if (analysis.find("stability_score") != analysis.end() && analysis.at("stability_score") < 0.8) {
            recommendations.push_back("Performance stability is low. Consider reducing variance in neural activity.");
        }
        
        return recommendations;
    }
    
private:
    PerformanceAnalyzer() {}
    
    double calculateGrowthRate(const std::vector<double>& series) {
        if (series.size() < 2) return 0.0;
        
        double first = series.front();
        double last = series.back();
        
        if (first == 0.0) return 0.0;
        
        return (last - first) / first;
    }
    
    double calculateEfficiencyImprovement(const std::vector<PerformanceMetrics>& metrics) {
        if (metrics.size() < 2) return 1.0;
        
        double firstEfficiency = metrics.front().stepsPerSecond / metrics.front().memoryUsageBytes;
        double lastEfficiency = metrics.back().stepsPerSecond / metrics.back().memoryUsageBytes;
        
        if (firstEfficiency == 0.0) return 1.0;
        
        return lastEfficiency / firstEfficiency;
    }
    
    double calculateStability(const std::vector<PerformanceMetrics>& metrics) {
        if (metrics.size() < 2) return 1.0;
        
        std::vector<double> values;
        for (const auto& metric : metrics) {
            values.push_back(metric.stepsPerSecond);
        }
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        double variance = 0.0;
        for (double value : values) {
            variance += std::pow(value - mean, 2);
        }
        variance /= values.size();
        
        double stdDev = std::sqrt(variance);
        
        if (mean == 0.0) return 0.0;
        
        return 1.0 - (stdDev / mean);
    }
    
    PerformanceAnalyzer(const PerformanceAnalyzer&) = delete;
    PerformanceAnalyzer& operator=(const PerformanceAnalyzer&) = delete;
};

// ============================================================================
// Convenience Functions
// ============================================================================

// Global performance monitoring functions
inline void startPerformanceMonitoring(std::shared_ptr<Brain> brain, const std::string& sessionId = "default") {
    PerformanceMonitor::getInstance().startMonitoring(brain, sessionId);
}

inline void stopPerformanceMonitoring(const std::string& sessionId) {
    PerformanceMonitor::getInstance().stopMonitoring(sessionId);
}

inline void updatePerformanceMetrics(const std::string& sessionId, const PerformanceMetrics& metrics) {
    PerformanceMonitor::getInstance().updateMetrics(sessionId, metrics);
}

inline void runBenchmark(const std::string& name, const std::shared_ptr<Config>& config, uint64_t steps) {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto result = BenchmarkFramework::getInstance().runBenchmark(name, config, 
                                                                 [brain](const Config&) { return brain; }, steps);
    
    NLM_LOG_INFO("Benchmark completed: " + name + " (" + std::to_string(result.throughput) + " steps/s)");
}

inline void profileCodeSection(const std::string& sectionName, const std::function<void()>& code) {
    PerformanceProfiler::getInstance().startProfile(sectionName);
    code();
    auto result = PerformanceProfiler::getInstance().stopProfile(sectionName);
    
    NLM_LOG_INFO("Code section " + sectionName + " completed in " + std::to_string(result.duration) + "s");
}

// ============================================================================
// Error Classes for Performance Tools
// ============================================================================

class PerformanceError : public NLMException {
public:
    explicit PerformanceError(const std::string& message) : NLMException("Performance Error: " + message) {}
    std::string getTypeName() const override { return "PerformanceError"; }
};

class BenchmarkError : public PerformanceError {
public:
    explicit BenchmarkError(const std::string& message) : PerformanceError("Benchmark Error: " + message) {}
    std::string getTypeName() const override { return "BenchmarkError"; }
};

class MonitoringError : public PerformanceError {
public:
    explicit MonitoringError(const std::string& message) : PerformanceError("Monitoring Error: " + message) {}
    std::string getTypeName() const override { return "MonitoringError"; }
};

// ============================================================================
// Performance Utilities
// ============================================================================

namespace PerformanceUtils {
    // Calculate throughput with warmup and steady-state phases
    struct ThroughputAnalysis {
        double warmupTime;
        double steadyStateTime;
        double warmupThroughput;
        double steadyStateThroughput;
        double overallThroughput;
    };
    
    inline ThroughputAnalysis analyzeThroughput(const std::vector<PerformanceMetrics>& metrics) {
        ThroughputAnalysis analysis;
        
        if (metrics.size() < 10) {
            return analysis;
        }
        
        // Find warmup phase (first 10% of steps)
        size_t warmupEnd = metrics.size() / 10;
        double warmupSteps = 0.0;
        double warmupDuration = 0.0;
        
        for (size_t i = 0; i < warmupEnd; ++i) {
            warmupSteps += metrics[i].stepsExecuted;
            warmupDuration += metrics[i].durationSeconds;
        }
        
        if (warmupDuration > 0.0) {
            analysis.warmupThroughput = warmupSteps / warmupDuration;
            analysis.warmupTime = warmupDuration;
        }
        
        // Calculate steady-state performance
        double steadyStateSteps = 0.0;
        double steadyStateDuration = 0.0;
        
        for (size_t i = warmupEnd; i < metrics.size(); ++i) {
            steadyStateSteps += metrics[i].stepsExecuted - metrics[i-1].stepsExecuted;
            steadyStateDuration += metrics[i].durationSeconds - metrics[i-1].durationSeconds;
        }
        
        if (steadyStateDuration > 0.0) {
            analysis.steadyStateThroughput = steadyStateSteps / steadyStateDuration;
            analysis.steadyStateTime = steadyStateDuration;
        }
        
        // Calculate overall throughput
        double totalSteps = metrics.back().stepsExecuted - metrics.front().stepsExecuted;
        double totalDuration = metrics.back().durationSeconds - metrics.front().durationSeconds;
        
        if (totalDuration > 0.0) {
            analysis.overallThroughput = totalSteps / totalDuration;
        }
        
        return analysis;
    }
    
    // Generate performance report
    inline std::string generatePerformanceReport(const std::vector<BenchmarkResult>& benchmarks) {
        std::stringstream ss;
        ss << "Performance Benchmark Report\n";
        ss << "=============================\n\n";
        
        ss << "Benchmarks executed: " << benchmarks.size() << "\n\n";
        
        for (const auto& benchmark : benchmarks) {
            ss << "Benchmark: " << benchmark.benchmarkName << "\n";
            ss << "  Configuration: " << benchmark.configurationDescription << "\n";
            ss << "  Duration: " << benchmark.totalDuration << "s\n";
            ss << "  Throughput: " << benchmark.throughput << " steps/s\n";
            ss << "  Efficiency: " << (benchmark.efficiency * 100) << "%\n";
            ss << "  Peak Memory: " << benchmark.peakMemoryUsage << " bytes\n";
            ss << "  Average CPU: " << benchmark.averageCPUUsage << "%\n";
            ss << "  Final Firing Rate: " << benchmark.finalFiringRate << "\n";
            ss << "  Errors: " << benchmark.errorCount << "\n\n";
        }
        
        return ss.str();
    }
    
    // Export metrics to CSV format
    inline bool exportMetricsToCSV(const std::vector<PerformanceMetrics>& metrics, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw IOError("Cannot open file for writing: " + filepath);
            }
            
            file << "Timestamp,Duration,Steps,StepsPerSecond,CPU%,Memory(MB),Neurons,Synapses,ActiveNeurons,FiringNeurons,AvgFiringRate\n";
            
            for (const auto& metric : metrics) {
                file << metric.timestamp << ","
                     << metric.durationSeconds << ","
                     << metric.stepsExecuted << ","
                     << metric.stepsPerSecond << ","
                     << metric.cpuUsagePercentage << ","
                     << (metric.memoryUsageBytes / (1024 * 1024)) << ","
                     << metric.neuronCount << ","
                     << metric.synapseCount << ","
                     << metric.activeNeuronCount << ","
                     << metric.firingNeuronCount << ","
                     << metric.averageFiringRate << "\n";
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to export metrics to CSV: " + std::string(e.what()));
            return false;
        }
    }
}

} // namespace nlm

#endif // NLM_PERFORMANCE_TOOLS_H
