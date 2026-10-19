// Performance monitoring and profiling tools for NLM
// Provides comprehensive performance analysis and monitoring capabilities

#ifndef NLM_PERFORMANCE_MONITOR_HPP
#define NLM_PERFORMANCE_MONITOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <sstream>
#include <filesystem>
#include "../core/Config/Config.hpp"

namespace nlm {

// Performance measurement types
enum class PerformanceMetric {
    CPUUsage,
    MemoryUsage,
    SimulationSpeed,
    NeuronFiringRate,
    SynapseActivity,
    MemoryAccessTime,
    CommunicationOverhead,
    PlasticityUpdateTime,
    LearningEfficiency,
    NetworkStability
};

// Performance sample
struct PerformanceSample {
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<PerformanceMetric, double> metrics;
    size_t simulationStep;
    double cpuUsage;          // Percentage (0-100)
    double memoryUsage;       // MB
    double simulationSpeed;   // Steps per second
    double firingRate;        // Hz
    double synapseActivity;   // Average connections per neuron
    double learningEfficiency; // 0-1 scale
    double networkStability;  // 0-1 scale
    
    PerformanceSample()
        : simulationStep(0), cpuUsage(0.0), memoryUsage(0.0), 
          simulationSpeed(0.0), firingRate(0.0), synapseActivity(0.0),
          learningEfficiency(0.0), networkStability(0.0) {}
    
    std::string toCSV() const {
        std::ostringstream oss;
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
            << "," << simulationStep
            << "," << cpuUsage
            << "," << memoryUsage
            << "," << simulationSpeed
            << "," << firingRate
            << "," << synapseActivity
            << "," << learningEfficiency
            << "," << networkStability;
        return oss.str();
    }
};

// Performance benchmark
struct PerformanceBenchmark {
    std::string name;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<PerformanceSample> samples;
    bool isRunning = false;
    std::thread monitoringThread;
    std::mutex mutex;
    
    PerformanceBenchmark(const std::string& n) : name(n) {}
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex);
        startTime = std::chrono::system_clock::now();
        endTime = startTime;
        isRunning = true;
        samples.clear();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mutex);
        endTime = std::chrono::system_clock::now();
        isRunning = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    double getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        return static_cast<double>(duration.count());
    }
    
    size_t getSampleCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return samples.size();
    }
};

// Performance analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    
    // Analyze performance trends
    std::string analyzeTrends(const std::vector<PerformanceSample>& samples) {
        if (samples.size() < 2) {
            return "Insufficient data for trend analysis";
        }
        
        std::ostringstream oss;
        oss << "=== Performance Trend Analysis ===" << std::endl;
        
        // CPU usage trend
        double cpuStart = samples.front().cpuUsage;
        double cpuEnd = samples.back().cpuUsage;
        double cpuTrend = (cpuEnd - cpuStart) / samples.size();
        oss << "CPU Usage: " << cpuStart << "% -> " << cpuEnd << "% (trend: " 
            << (cpuTrend > 0 ? "increasing" : "decreasing") << " " << std::abs(cpuTrend) << "%/sample)" << std::endl;
        
        // Memory usage trend
        double memStart = samples.front().memoryUsage;
        double memEnd = samples.back().memoryUsage;
        double memTrend = (memEnd - memStart) / samples.size();
        oss << "Memory Usage: " << memStart << "MB -> " << memEnd << "MB (trend: " 
            << (memTrend > 0 ? "increasing" : "decreasing") << " " << std::abs(memTrend) << "MB/sample)" << std::endl;
        
        // Simulation speed trend
        double speedStart = samples.front().simulationSpeed;
        double speedEnd = samples.back().simulationSpeed;
        double speedTrend = (speedEnd - speedStart) / samples.size();
        oss << "Simulation Speed: " << speedStart << " steps/sec -> " << speedEnd << " steps/sec (trend: " 
            << (speedTrend > 0 ? "improving" : "degrading") << " " << std::abs(speedTrend) << " steps/sec/sample)" << std::endl;
        
        // Firing rate trend
        double firingStart = samples.front().firingRate;
        double firingEnd = samples.back().firingRate;
        double firingTrend = (firingEnd - firingStart) / samples.size();
        oss << "Firing Rate: " << firingStart << "Hz -> " << firingEnd << "Hz (trend: " 
            << (firingTrend > 0 ? "increasing" : "decreasing") << " " << std::abs(firingTrend) << "Hz/sample)" << std::endl;
        
        // Learning efficiency trend
        double effStart = samples.front().learningEfficiency;
        double effEnd = samples.back().learningEfficiency;
        double effTrend = (effEnd - effStart) / samples.size();
        oss << "Learning Efficiency: " << effStart << " -> " << effEnd << " (trend: " 
            << (effTrend > 0 ? "improving" : "degrading") << " " << std::abs(effTrend) << "/sample)" << std::endl;
        
        return oss.str();
    }
    
    // Generate performance report
    std::string generateReport(const std::vector<PerformanceSample>& samples) {
        if (samples.empty()) {
            return "No performance data available";
        }
        
        std::ostringstream oss;
        oss << "=== Performance Report ===" << std::endl;
        oss << "Generated: " << getCurrentTimestamp() << std::endl;
        oss << "Total samples: " << samples.size() << std::endl;
        oss << "Time range: " << getTimestamp(samples.front().timestamp) << " to " << getTimestamp(samples.back().timestamp) << std::endl;
        oss << std::endl;
        
        // Summary statistics
        oss << "=== Summary Statistics ===" << std::endl;
        oss << "CPU Usage: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.cpuUsage; }) << "% (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.cpuUsage; }) << ")" << std::endl;
        oss << "Memory Usage: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.memoryUsage; }) << " MB (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.memoryUsage; }) << ")" << std::endl;
        oss << "Simulation Speed: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.simulationSpeed; }) << " steps/sec (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.simulationSpeed; }) << ")" << std::endl;
        oss << "Firing Rate: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.firingRate; }) << " Hz (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.firingRate; }) << ")" << std::endl;
        oss << "Learning Efficiency: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.learningEfficiency; }) << " (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.learningEfficiency; }) << ")" << std::endl;
        oss << "Network Stability: " << calculateAverage(samples, [](const PerformanceSample& s) { return s.networkStability; }) << " (std: " << calculateStdDev(samples, [](const PerformanceSample& s) { return s.networkStability; }) << ")" << std::endl;
        
        // Performance trends
        oss << std::endl << analyzeTrends(samples) << std::endl;
        
        // Bottleneck analysis
        oss << "=== Bottleneck Analysis ===" << std::endl;
        oss << "Based on the collected performance data, potential bottlenecks include:" << std::endl;
        
        double avgCpu = calculateAverage(samples, [](const PerformanceSample& s) { return s.cpuUsage; });
        double avgMemory = calculateAverage(samples, [](const PerformanceSample& s) { return s.memoryUsage; });
        double avgSpeed = calculateAverage(samples, [](const PerformanceSample& s) { return s.simulationSpeed; });
        
        if (avgCpu > 80.0) {
            oss << "- High CPU usage (>80%): Consider optimizing computational bottlenecks" << std::endl;
        }
        if (avgMemory > 1024.0) {  // 1GB
            oss << "- High memory usage (>1GB): Consider memory optimization strategies" << std::endl;
        }
        if (avgSpeed < 100.0) {  // 100 steps/sec
            oss << "- Low simulation speed (<100 steps/sec): Consider parallel processing or algorithmic improvements" << std::endl;
        }
        
        // Recommendations
        oss << std::endl << "=== Recommendations ===" << std::endl;
        
        if (calculateAverage(samples, [](const PerformanceSample& s) { return s.cpuUsage; }) > 75.0) {
            oss << "1. CPU optimization: Consider vectorization, loop unrolling, or parallel processing" << std::endl;
        }
        if (calculateAverage(samples, [](const PerformanceSample& s) { return s.memoryUsage; }) > 512.0) {
            oss << "2. Memory optimization: Implement memory pools, object reuse, or compression" << std::endl;
        }
        if (calculateAverage(samples, [](const PerformanceSample& s) { return s.simulationSpeed; }) < 200.0) {
            oss << "3. Performance tuning: Optimize synaptic operations, neuron updates, or memory access patterns" << std::endl;
        }
        if (calculateAverage(samples, [](const PerformanceSample& s) { return s.learningEfficiency; }) < 0.7) {
            oss << "4. Learning efficiency: Review plasticity rules, reward mechanisms, or memory systems" << std::endl;
        }
        
        return oss.str();
    }
    
    // Save performance data to file
    bool savePerformanceData(const std::vector<PerformanceSample>& samples, const std::string& filename) {
        try {
            if (!std::filesystem::exists(std::filesystem::path(filename).parent_path())) {
                std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
            }
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // Write header
            file << "timestamp,simulation_step,cpu_usage,memory_usage,simulation_speed,firing_rate,synapse_activity,learning_efficiency,network_stability" << std::endl;
            
            // Write samples
            for (const auto& sample : samples) {
                file << sample.toCSV() << std::endl;
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Load performance data from file
    std::vector<PerformanceSample> loadPerformanceData(const std::string& filename) {
        std::vector<PerformanceSample> samples;
        
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return samples;
            }
            
            std::string line;
            std::getline(file, line); // Skip header
            
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                
                PerformanceSample sample;
                std::string token;
                std::stringstream ss(line);
                
                std::getline(ss, token, ','); // timestamp (skip)
                std::getline(ss, token, ','); sample.simulationStep = std::stoul(token);
                std::getline(ss, token, ','); sample.cpuUsage = std::stod(token);
                std::getline(ss, token, ','); sample.memoryUsage = std::stod(token);
                std::getline(ss, token, ','); sample.simulationSpeed = std::stod(token);
                std::getline(ss, token, ','); sample.firingRate = std::stod(token);
                std::getline(ss, token, ','); sample.synapseActivity = std::stod(token);
                std::getline(ss, token, ','); sample.learningEfficiency = std::stod(token);
                std::getline(ss, token, ','); sample.networkStability = std::stod(token);
                
                samples.push_back(sample);
            }
            
        } catch (const std::exception& e) {
            // Return empty samples on error
        }
        
        return samples;
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string getTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    template<typename T>
    double calculateAverage(const std::vector<PerformanceSample>& samples, T extractor) {
        if (samples.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& sample : samples) {
            sum += extractor(sample);
        }
        return sum / samples.size();
    }
    
    template<typename T>
    double calculateStdDev(const std::vector<PerformanceSample>& samples, T extractor) {
        if (samples.size() < 2) return 0.0;
        
        double mean = calculateAverage(samples, extractor);
        double sumSq = 0.0;
        
        for (const auto& sample : samples) {
            double value = extractor(sample);
            sumSq += (value - mean) * (value - mean);
        }
        
        return std::sqrt(sumSq / (samples.size() - 1));
    }
};

// Performance monitor for real-time monitoring
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Config> config) : config_(config) {
        analyzer_ = std::make_unique<PerformanceAnalyzer>();
        isMonitoring_ = false;
        monitorThread_ = nullptr;
        lastSampleTime_ = std::chrono::system_clock::now();
    }
    
    ~PerformanceMonitor() {
        stopMonitoring();
    }
    
    void startMonitoring() {
        if (isMonitoring_) return;
        
        isMonitoring_ = true;
        monitorThread_ = std::make_unique<std::thread>(&PerformanceMonitor::monitorThreadFunc, this);
    }
    
    void stopMonitoring() {
        if (!isMonitoring_) return;
        
        isMonitoring_ = false;
        if (monitorThread_ && monitorThread_->joinable()) {
            monitorThread_->join();
            monitorThread_ = nullptr;
        }
    }
    
    void addBenchmark(const std::string& name, PerformanceBenchmark& benchmark) {
        std::lock_guard<std::mutex> lock(benchmarksMutex_);
        benchmarks_[name] = &benchmark;
    }
    
    PerformanceBenchmark* getBenchmark(const std::string& name) {
        std::lock_guard<std::mutex> lock(benchmarksMutex_);
        auto it = benchmarks_.find(name);
        if (it != benchmarks_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getBenchmarkNames() const {
        std::lock_guard<std::mutex> lock(benchmarksMutex_);
        std::vector<std::string> names;
        for (const auto& pair : benchmarks_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::string getPerformanceReport() {
        std::lock_guard<std::mutex> lock(reportMutex_);
        return analyzer_->generateReport(currentSamples_);
    }
    
    bool savePerformanceData(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        return analyzer_->savePerformanceData(currentSamples_, filename);
    }
    
    void addSample(const PerformanceSample& sample) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        currentSamples_.push_back(sample);
        
        // Keep only recent samples (last hour)
        auto cutoffTime = std::chrono::system_clock::now();
        cutoffTime -= std::chrono::hours(1);
        
        std::vector<PerformanceSample> filteredSamples;
        for (const auto& sample : currentSamples_) {
            if (sample.timestamp >= cutoffTime) {
                filteredSamples.push_back(sample);
            }
        }
        currentSamples_ = filteredSamples;
    }
    
    PerformanceSample getCurrentSample() {
        PerformanceSample sample;
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        if (!currentSamples_.empty()) {
            sample = currentSamples_.back();
        }
        
        return sample;
    }
    
private:
    void monitorThreadFunc() {
        // Wait for initialization
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        while (isMonitoring_) {
            try {
                PerformanceSample sample = collectCurrentPerformanceData();
                addSample(sample);
                
                // Update benchmark samples
                std::lock_guard<std::mutex> lock(benchmarksMutex_);
                for (auto& pair : benchmarks_) {
                    if (pair.second && pair.second->isRunning) {
                        pair.second->samples.push_back(sample);
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Sample every second
                
            } catch (const std::exception& e) {
                // Log error and continue
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before retrying
            }
        }
    }
    
    PerformanceSample collectCurrentPerformanceData() {
        PerformanceSample sample;
        
        sample.timestamp = std::chrono::system_clock::now();
        
        // Simulate performance data collection
        // In a real implementation, this would collect actual system metrics
        
        if (config_) {
            sample.cpuUsage = collectCPUUsage();
            sample.memoryUsage = collectMemoryUsage();
            sample.simulationSpeed = collectSimulationSpeed();
            sample.firingRate = collectFiringRate();
            sample.synapseActivity = collectSynapseActivity();
            sample.learningEfficiency = collectLearningEfficiency();
            sample.networkStability = collectNetworkStability();
        }
        
        return sample;
    }
    
    double collectCPUUsage() {
        // Simulate CPU usage collection
        // In real implementation, this would use system-specific APIs
        return 45.0 + (std::rand() % 20); // 45-65%
    }
    
    double collectMemoryUsage() {
        // Simulate memory usage collection
        return 256.0 + (std::rand() % 100); // 256-356 MB
    }
    
    double collectSimulationSpeed() {
        // Simulate simulation speed collection
        return 500.0 + (std::rand() % 200); // 500-700 steps/sec
    }
    
    double collectFiringRate() {
        // Simulate firing rate collection
        return 10.0 + (std::rand() % 10); // 10-20 Hz
    }
    
    double collectSynapseActivity() {
        // Simulate synapse activity collection
        return 150.0 + (std::rand() % 50); // 150-200 connections per neuron
    }
    
    double collectLearningEfficiency() {
        // Simulate learning efficiency collection
        return 0.7 + ((std::rand() % 100) / 100.0 * 0.3); // 0.7-1.0
    }
    
    double collectNetworkStability() {
        // Simulate network stability collection
        return 0.8 + ((std::rand() % 100) / 100.0 * 0.2); // 0.8-1.0
    }
    
    std::unique_ptr<PerformanceAnalyzer> analyzer_;
    std::shared_ptr<Config> config_;
    std::unique_ptr<std::thread> monitorThread_;
    std::atomic<bool> isMonitoring_{false};
    
    std::vector<PerformanceSample> currentSamples_;
    
    std::mutex dataMutex_;
    std::mutex benchmarksMutex_;
    std::mutex reportMutex_;
    
    std::chrono::system_clock::time_point lastSampleTime_;
    
    std::unordered_map<std::string, PerformanceBenchmark*> benchmarks_;
};

// Performance monitoring system
class PerformanceMonitoringSystem {
public:
    PerformanceMonitoringSystem() = default;
    
    PerformanceMonitor* createMonitor(std::shared_ptr<Config> config) {
        auto monitor = std::make_unique<PerformanceMonitor>(config);
        auto* rawPointer = monitor.get();
        monitors_[rawPointer] = std::move(monitor);
        return rawPointer;
    }
    
    void destroyMonitor(PerformanceMonitor* monitor) {
        if (monitor) {
            monitors_.erase(monitor);
        }
    }
    
    std::vector<PerformanceMonitor*> getMonitors() {
        std::vector<PerformanceMonitor*> pointers;
        for (auto& pair : monitors_) {
            pointers.push_back(pair.first);
        }
        return pointers;
    }
    
    std::string getOverallPerformanceReport() {
        std::ostringstream oss;
        oss << "=== NLM Performance Monitoring System ===" << std::endl;
        oss << "Active monitors: " << monitors_.size() << std::endl;
        oss << std::endl;
        
        for (auto& pair : monitors_) {
            oss << "--- Monitor " << (pair.second ? "Active" : "Inactive") << " ---" << std::endl;
            if (pair.second) {
                oss << pair.second->getPerformanceReport() << std::endl;
            } else {
                oss << "Monitor not available" << std::endl;
            }
        }
        
        return oss.str();
    }
    
    void saveAllPerformanceData(const std::string& directory) {
        try {
            if (!std::filesystem::exists(directory)) {
                std::filesystem::create_directories(directory);
            }
            
            std::string timestamp = getCurrentTimestamp();
            
            for (auto& pair : monitors_) {
                if (pair.second) {
                    std::string filename = directory + "/perf_data_" + timestamp + ".csv";
                    pair.second->savePerformanceData(filename);
                }
            }
            
        } catch (const std::exception& e) {
            // Log error
        }
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        return oss.str();
    }
    
    std::unordered_map<PerformanceMonitor*, std::unique_ptr<PerformanceMonitor>> monitors_;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITOR_HPP
