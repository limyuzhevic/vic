// NLM Performance Benchmarking Module
// Comprehensive performance monitoring and benchmarking for NLM applications
// Provides timing, memory, and resource usage tracking

#ifndef NLM_PERFORMANCE_BENCHMARKS_H
#define NLM_PERFORMANCE_BENCHMARKS_H

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../world/SimpleWorld.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>

namespace nlm {
namespace performance {

// Performance measurement types
enum class MeasurementType {
    Time,
    Memory,
    CPU,
    Network,
    I/O,
    Custom
};

// Performance measurement result
struct PerformanceResult {
    std::string metricName;
    MeasurementType type;
    double value;
    std::string unit;
    std::map<std::string, std::string> metadata;
    double timestamp;
    
    PerformanceResult(const std::string& name, MeasurementType t, double val, const std::string& unit = "")
        : metricName(name), type(t), value(val), unit(unit), timestamp(0.0) {}
};

// Benchmarking configuration
struct BenchmarkConfig {
    bool enabled = true;
    bool detailedReporting = true;
    bool saveToFile = true;
    std::string outputFile = "benchmark_results.json";
    std::string benchmarkName = "NLM Benchmark";
    double warmupTime = 2.0; // seconds
    double measurementTime = 10.0; // seconds
    int measurementIterations = 5;
    bool useSystemTimer = true;
    std::string outputFormat = "json"; // json, csv, text
    std::map<std::string, std::string> customParameters;
};

// Timing measurements
struct TimingMeasurements {
    double totalTime;
    double setupTime;
    double simulationTime;
    double teardownTime;
    double perStepTime;
    std::vector<double> stepTimes;
    std::vector<double> fpsSamples;
    
    TimingMeasurements() : totalTime(0.0), setupTime(0.0), simulationTime(0.0), 
                           teardownTime(0.0), perStepTime(0.0) {}
};

// Memory measurements
struct MemoryMeasurements {
    size_t peakMemoryUsage;
    size_t currentMemoryUsage;
    size_t memoryAllocations;
    size_t memoryDeallocations;
    double memoryGrowthRate;
    std::vector<size_t> memorySamples;
    
    MemoryMeasurements() : peakMemoryUsage(0), currentMemoryUsage(0), 
                           memoryAllocations(0), memoryDeallocations(0),
                           memoryGrowthRate(0.0) {}
};

// CPU measurements
struct CPUMeasurements {
    double cpuUsage;
    double userTime;
    double systemTime;
    int cpuCores;
    double cpuFrequency;
    std::vector<double> cpuSamples;
    
    CPUMeasurements() : cpuUsage(0.0), userTime(0.0), systemTime(0.0),
                       cpuCores(0), cpuFrequency(0.0) {}
};

// Comprehensive benchmark results
struct BenchmarkResults {
    BenchmarkConfig config;
    TimingMeasurements timing;
    MemoryMeasurements memory;
    CPUMeasurements cpu;
    std::vector<PerformanceResult> customMetrics;
    std::string timestamp;
    std::string systemInfo;
    
    // Calculate derived metrics
    double getAverageFPS() const {
        if (timing.stepTimes.empty()) return 0.0;
        double totalTime = std::accumulate(timing.stepTimes.begin(), timing.stepTimes.end(), 0.0);
        return timing.stepTimes.size() / totalTime;
    }
    
    double getMemoryUsageMB() const {
        return memory.peakMemoryUsage / (1024.0 * 1024.0);
    }
    
    double getSimulationTimePerStep() const {
        if (timing.stepTimes.empty()) return 0.0;
        return timing.simulationTime / timing.stepTimes.size();
    }
};

// Performance counter for precise timing
class PerformanceCounter {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running;
    
public:
    PerformanceCounter() : running(false) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
        running = false;
    }
    
    double elapsedSeconds() const {
        if (!running) {
            auto duration = std::chrono::duration<double>(endTime - startTime);
            return duration.count();
        } else {
            auto duration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime);
            return duration.count();
        }
    }
    
    bool isRunning() const { return running; }
};

// Memory tracker for memory usage monitoring
class MemoryTracker {
private:
    std::vector<size_t> memorySamples;
    std::atomic<size_t> peakMemory;
    std::atomic<size_t> currentMemory;
    std::thread trackingThread;
    std::atomic<bool> tracking;
    std::mutex mutex;
    
public:
    MemoryTracker() : peakMemory(0), currentMemory(0), tracking(false) {}
    
    void startTracking(double interval = 0.1) {
        tracking = true;
        trackingThread = std::thread([this, interval]() {
            while (tracking) {
                size_t memUsage = getCurrentMemoryUsage();
                std::lock_guard<std::mutex> lock(mutex);
                memorySamples.push_back(memUsage);
                if (memUsage > peakMemory) {
                    peakMemory = memUsage;
                }
                currentMemory = memUsage;
                std::this_thread::sleep_for(std::chrono::duration<double>(interval));
            }
        });
    }
    
    void stopTracking() {
        tracking = false;
        if (trackingThread.joinable()) {
            trackingThread.join();
        }
    }
    
    virtual size_t getCurrentMemoryUsage() const {
        // Platform-specific implementation would go here
        // For now, return a placeholder
        return 0;
    }
    
    size_t getPeakMemory() const { return peakMemory; }
    size_t getCurrentMemory() const { return currentMemory; }
    const std::vector<size_t>& getMemorySamples() const { return memorySamples; }
    
    ~MemoryTracker() {
        stopTracking();
    }
};

// CPU monitor for CPU usage tracking
class CPUMonitor {
private:
    std::vector<double> cpuSamples;
    std::thread monitoringThread;
    std::atomic<bool> monitoring;
    std::mutex mutex;
    int cpuCores;
    
public:
    CPUMonitor() : monitoring(false), cpuCores(std::thread::hardware_concurrency()) {}
    
    void startMonitoring(double interval = 0.1) {
        monitoring = true;
        monitoringThread = std::thread([this, interval]() {
            while (monitoring) {
                double cpuUsage = getCPUUsage();
                std::lock_guard<std::mutex> lock(mutex);
                cpuSamples.push_back(cpuUsage);
                std::this_thread::sleep_for(std::chrono::duration<double>(interval));
            }
        });
    }
    
    void stopMonitoring() {
        monitoring = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    virtual double getCPUUsage() const {
        // Platform-specific implementation would go here
        // For now, return a placeholder
        return 0.0;
    }
    
    double getAverageCPUUsage() const {
        if (cpuSamples.empty()) return 0.0;
        double sum = std::accumulate(cpuSamples.begin(), cpuSamples.end(), 0.0);
        return sum / cpuSamples.size();
    }
    
    const std::vector<double>& getCPUSamples() const { return cpuSamples; }
    
    ~CPUMonitor() {
        stopMonitoring();
    }
};

// Main benchmarking class
class NLMBenchmark {
private:
    BenchmarkConfig config_;
    BenchmarkResults results_;
    PerformanceCounter timer_;
    MemoryTracker memoryTracker_;
    CPUMonitor cpuMonitor_;
    
    // Current simulation context
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
public:
    NLMBenchmark(const BenchmarkConfig& config) : config_(config) {
        if (config_.enabled) {
            memoryTracker_.startTracking(0.1);
            cpuMonitor_.startMonitoring(0.1);
        }
    }
    
    ~NLMBenchmark() {
        if (config_.enabled) {
            memoryTracker_.stopTracking();
            cpuMonitor_.stopMonitoring();
        }
    }
    
    // Initialize benchmark with simulation components
    void initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                    std::shared_ptr<SimpleWorld> world) {
        brain_ = brain;
        agent_ = agent;
        world_ = world;
    }
    
    // Run benchmark
    BenchmarkResults runBenchmark() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (!config_.enabled) {
            return BenchmarkResults(); // Empty results if disabled
        }
        
        // Warmup phase
        std::cout << "Warming up... (" << config_.warmupTime << " seconds)" << std::endl;
        warmup();
        
        // Measurement phase
        std::cout << "Running measurements... (" << config_.measurementTime << " seconds, " 
                  << config_.measurementIterations << " iterations)" << std::endl;
        runMeasurements();
        
        // Collect results
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        collectResults(totalTime);
        
        // Generate system info
        results_.systemInfo = getSystemInfo();
        results_.timestamp = getCurrentTimestamp();
        
        // Save results if configured
        if (config_.saveToFile && config_.outputFile != "") {
            saveResults();
        }
        
        return results_;
    }
    
    // Print benchmark report
    void printReport() const {
        std::cout << "=== " << config_.benchmarkName << " ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Timing Results:" << std::endl;
        std::cout << "  Total time: " << std::fixed << std::setprecision(3) 
                  << results_.timing.totalTime << " seconds" << std::endl;
        std::cout << "  Setup time: " << std::fixed << std::setprecision(3) 
                  << results_.timing.setupTime << " seconds" << std::endl;
        std::cout << "  Simulation time: " << std::fixed << std::setprecision(3) 
                  << results_.timing.simulationTime << " seconds" << std::endl;
        std::cout << "  Teardown time: " << std::fixed << std::setprecision(3) 
                  << results_.timing.teardownTime << " seconds" << std::endl;
        std::cout << "  Steps per second: " << std::fixed << std::setprecision(1) 
                  << results_.getAverageFPS() << " Hz" << std::endl;
        std::cout << "  Average step time: " << std::fixed << std::setprecision(6) 
                  << results_.getSimulationTimePerStep() << " seconds" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Memory Results:" << std::endl;
        std::cout << "  Peak memory usage: " << std::fixed << std::setprecision(2) 
                  << results_.getMemoryUsageMB() << " MB" << std::endl;
        std::cout << "  Current memory usage: " << std::fixed << std::setprecision(2) 
                  << memoryTracker_.getCurrentMemory() / (1024.0 * 1024.0) << " MB" << std::endl;
        std::cout << "  Memory samples: " << results_.memory.memorySamples.size() << std::endl;
        std::cout << std::endl;
        
        std::cout << "CPU Results:" << std::endl;
        std::cout << "  Average CPU usage: " << std::fixed << std::setprecision(1) 
                  << results_.cpu.getAverageCPUUsage() << "%" << std::endl;
        std::cout << "  CPU samples: " << results_.cpu.cpuSamples.size() << std::endl;
        std::cout << std::endl;
        
        if (!results_.customMetrics.empty()) {
            std::cout << "Custom Metrics:" << std::endl;
            for (const auto& metric : results_.customMetrics) {
                std::cout << "  " << metric.metricName << ": " << std::fixed << std::setprecision(3) 
                          << metric.value << " " << metric.unit << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    // Add custom metric
    void addCustomMetric(const std::string& name, MeasurementType type, double value, 
                        const std::string& unit = "") {
        results_.customMetrics.emplace_back(name, type, value, unit);
    }
    
protected:
    void warmup() {
        if (!brain_ || !agent_ || !world_) return;
        
        timer_.start();
        double warmupEndTime = timer_.elapsedSeconds() + config_.warmupTime;
        
        int steps = 0;
        while (timer_.elapsedSeconds() < warmupEndTime) {
            // Run a small simulation for warmup
            world_->reset();
            agent_->initialize(*world_);
            
            for (int step = 0; step < 10 && timer_.elapsedSeconds() < warmupEndTime; ++step) {
                world_->update(0.1);
                agent_->processSensoryInput(world_->getSensoryPercept());
                brain_->step(step);
                agent_->decodeMotorCommand();
                world_->applyMotorCommand(agent_->decodeMotorCommand(), world_->getSimulationTime());
                steps++;
            }
        }
        
        timer_.stop();
        std::cout << "Warmup completed: " << steps << " steps" << std::endl;
    }
    
    void runMeasurements() {
        if (!brain_ || !agent_ || !world_) return;
        
        timer_.start();
        double measurementEndTime = timer_.elapsedSeconds() + config_.measurementTime;
        
        int totalSteps = 0;
        results_.timing.stepTimes.reserve(config_.measurementIterations * 1000);
        
        for (int iteration = 0; iteration < config_.measurementIterations; ++iteration) {
            std::cout << "  Iteration " << (iteration + 1) << "/" << config_.measurementIterations << "..." << std::endl;
            
            world_->reset();
            agent_->initialize(*world_);
            
            int stepsInIteration = 0;
            while (timer_.elapsedSeconds() < measurementEndTime) {
                world_->update(0.1);
                agent_->processSensoryInput(world_->getSensoryPercept());
                brain_->step(totalSteps);
                auto startStepTime = std::chrono::high_resolution_clock::now();
                
                agent_->decodeMotorCommand();
                world_->applyMotorCommand(agent_->decodeMotorCommand(), world_->getSimulationTime());
                
                auto endStepTime = std::chrono::high_resolution_clock::now();
                double stepTime = std::chrono::duration<double>(endStepTime - startStepTime).count();
                results_.timing.stepTimes.push_back(stepTime);
                
                stepsInIteration++;
                totalSteps++;
            }
            
            // Small pause between iterations
            std::this_thread::sleep_for(std::chrono::duration<double>(0.5));
        }
        
        timer_.stop();
        results_.timing.simulationTime = timer_.elapsedSeconds();
        
        std::cout << "Measurement completed: " << totalSteps << " total steps" << std::endl;
    }
    
    void collectResults(double totalTime) {
        results_.timing.totalTime = totalTime;
        results_.timing.perStepTime = totalTime / results_.timing.stepTimes.size();
        
        // Copy memory and CPU measurements
        results_.memory.peakMemoryUsage = memoryTracker_.getPeakMemory();
        results_.memory.currentMemoryUsage = memoryTracker_.getCurrentMemory();
        results_.memory.memorySamples = memoryTracker_.getMemorySamples();
        
        results_.cpu.cpuUsage = cpuMonitor_.getAverageCPUUsage();
        results_.cpu.cpuSamples = cpuMonitor_.getCPUSamples();
        
        // Calculate memory growth rate
        if (results_.memory.memorySamples.size() > 1) {
            size_t firstSample = results_.memory.memorySamples.front();
            size_t lastSample = results_.memory.memorySamples.back();
            results_.memory.memoryGrowthRate = (lastSample - firstSample) / 
                                              (results_.memory.memorySamples.size() - 1);
        }
    }
    
    std::string getSystemInfo() const {
        std::stringstream info;
        info << "CPU Cores: " << std::thread::hardware_concurrency() << "\n";
        info << "Benchmark Name: " << config_.benchmarkName << "\n";
        info << "Output Format: " << config_.outputFormat << "\n";
        for (const auto& param : config_.customParameters) {
            info << param.first << ": " << param.second << "\n";
        }
        return info.str();
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void saveResults() {
        if (config_.outputFormat == "json") {
            saveAsJSON();
        } else if (config_.outputFormat == "csv") {
            saveAsCSV();
        } else if (config_.outputFormat == "text") {
            saveAsText();
        }
    }
    
    void saveAsJSON() {
        std::ofstream file(config_.outputFile);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open output file: " << config_.outputFile << std::endl;
            return;
        }
        
        // Write JSON results
        file << "{\n";
        file << "  \"benchmark_name\": \"" << config_.benchmarkName << "\",\n";
        file << "  \"timestamp\": \"" << results_.timestamp << "\",\n";
        file << "  \"system_info\": \"" << results_.systemInfo << "\",\n";
        file << "  \"timing\": {\n";
        file << "    \"total_time\": " << results_.timing.totalTime << ",\n";
        file << "    \"setup_time\": " << results_.timing.setupTime << ",\n";
        file << "    \"simulation_time\": " << results_.timing.simulationTime << ",\n";
        file << "    \"teardown_time\": " << results_.timing.teardownTime << ",\n";
        file << "    \"steps_per_second\": " << results_.getAverageFPS() << ",\n";
        file << "    \"average_step_time\": " << results_.getSimulationTimePerStep() << "\n";
        file << "  },\n";
        file << "  \"memory\": {\n";
        file << "    \"peak_memory_bytes\": " << results_.memory.peakMemoryUsage << ",\n";
        file << "    \"current_memory_bytes\": " << results_.memory.currentMemoryUsage << ",\n";
        file << "    \"memory_samples\": [";
        for (size_t i = 0; i < results_.memory.memorySamples.size(); ++i) {
            file << results_.memory.memorySamples[i];
            if (i < results_.memory.memorySamples.size() - 1) file << ", ";
        }
        file << "],\n";
        file << "    \"memory_growth_rate\": " << results_.memory.memoryGrowthRate << "\n";
        file << "  },\n";
        file << "  \"cpu\": {\n";
        file << "    \"cpu_usage_percent\": " << results_.cpu.cpuUsage << ",\n";
        file << "    \"cpu_samples\": [";
        for (size_t i = 0; i < results_.cpu.cpuSamples.size(); ++i) {
            file << results_.cpu.cpuSamples[i];
            if (i < results_.cpu.cpuSamples.size() - 1) file << ", ";
        }
        file << "]\n";
        file << "  },\n";
        
        if (!results_.customMetrics.empty()) {
            file << "  \"custom_metrics\": [\n";
            for (size_t i = 0; i < results_.customMetrics.size(); ++i) {
                const auto& metric = results_.customMetrics[i];
                file << "    {\n";
                file << "      \"name\": \"" << metric.metricName << "\",\n";
                file << "      \"type\": " << static_cast<int>(metric.type) << ",\n";
                file << "      \"value\": " << metric.value << ",\n";
                file << "      \"unit\": \"" << metric.unit << "\",\n";
                file << "      \"timestamp\": " << metric.timestamp << "\n";
                file << "    }";
                if (i < results_.customMetrics.size() - 1) file << ",\n";
            }
            file << "\n  ]\n";
        }
        
        file << "}\n";
        file.close();
        std::cout << "Results saved to JSON file: " << config_.outputFile << std::endl;
    }
    
    void saveAsCSV() {
        // Simplified CSV output
        std::ofstream file(config_.outputFile);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open output file: " << config_.outputFile << std::endl;
            return;
        }
        
        file << "metric,value,unit,timestamp\n";
        file << "total_time," << results_.timing.totalTime << ",seconds," << results_.timestamp << "\n";
        file << "peak_memory_mb," << results_.getMemoryUsageMB() << ",MB," << results_.timestamp << "\n";
        file << "steps_per_second," << results_.getAverageFPS() << ",Hz," << results_.timestamp << "\n";
        
        for (const auto& metric : results_.customMetrics) {
            file << metric.metricName << "," << metric.value << "," << metric.unit << "," << metric.timestamp << "\n";
        }
        
        file.close();
        std::cout << "Results saved to CSV file: " << config_.outputFile << std::endl;
    }
    
    void saveAsText() {
        std::ofstream file(config_.outputFile);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open output file: " << config_.outputFile << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark Results" << std::endl;
        file << "==================================" << std::endl;
        file << "Benchmark: " << config_.benchmarkName << std::endl;
        file << "Timestamp: " << results_.timestamp << std::endl;
        file << "Total Time: " << std::fixed << std::setprecision(3) << results_.timing.totalTime << " seconds" << std::endl;
        file << "Steps per Second: " << std::fixed << std::setprecision(1) << results_.getAverageFPS() << " Hz" << std::endl;
        file << "Peak Memory: " << std::fixed << std::setprecision(2) << results_.getMemoryUsageMB() << " MB" << std::endl;
        file << "Average Step Time: " << std::fixed << std::setprecision(6) << results_.getSimulationTimePerStep() << " seconds" << std::endl;
        file << "CPU Usage: " << std::fixed << std::setprecision(1) << results_.cpu.cpuUsage << "%" << std::endl;
        
        file.close();
        std::cout << "Results saved to text file: " << config_.outputFile << std::endl;
    }
};

// Convenience function to create and run benchmark
inline BenchmarkResults runBenchmark(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<SimpleWorld> world, const BenchmarkConfig& config) {
    NLMBenchmark benchmark(config);
    benchmark.initialize(brain, agent, world);
    return benchmark.runBenchmark();
}

} // namespace performance
} // namespace nlm

#endif // NLM_PERFORMANCE_BENCHMARKS_H
