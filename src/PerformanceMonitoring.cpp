#include "PerformanceMonitoring.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <filesystem>

namespace nlm {

PerformanceMetrics::PerformanceMetrics() : isMonitoring_(false) {}

PerformanceMetrics::~PerformanceMetrics() {
    // Clear samples on destruction
    std::lock_guard<std::mutex> lock(samplesMutex_);
    samples_.clear();
}

void PerformanceMetrics::startMonitoring() {
    if (!isMonitoring_) {
        isMonitoring_ = true;
        startTime_ = std::chrono::steady_clock::now();
    }
}

void PerformanceMetrics::stopMonitoring() {
    if (isMonitoring_) {
        isMonitoring_ = false;
    }
}

void PerformanceMetrics::recordSample(const PerformanceSample& sample) {
    std::lock_guard<std::mutex> lock(samplesMutex_);
    samples_.push_back(sample);
}

PerformanceMetrics::PerformanceSample PerformanceMetrics::getLatestSample() const {
    std::lock_guard<std::mutex> lock(samplesMutex_);
    if (samples_.empty()) {
        return PerformanceSample();
    }
    return samples_.back();
}

PerformanceMetrics::PerformanceAnalysis PerformanceMetrics::analyze() const {
    PerformanceAnalysis analysis;
    
    std::lock_guard<std::mutex> lock(samplesMutex_);
    
    if (samples_.empty()) {
        return analysis;
    }
    
    size_t sampleCount = samples_.size();
    
    // Calculate basic statistics
    for (const auto& sample : samples_) {
        analysis.totalDuration = std::max(analysis.totalDuration, sample.timestamp);
        analysis.totalSteps = std::max(analysis.totalSteps, sample.simulationStep);
        analysis.totalSpikes += sample.totalSpikeCount;
        
        analysis.avgCpuUsage += sample.cpuUsage;
        analysis.peakCpuUsage = std::max(analysis.peakCpuUsage, sample.cpuUsage);
        analysis.avgMemoryUsage += sample.memoryUsageMB;
        analysis.peakMemoryUsage = std::max(analysis.peakMemoryUsage, sample.memoryUsageMB);
        analysis.avgSpikeRate += sample.spikeRate;
        analysis.avgFiringRate += sample.firingRate;
    }
    
    // Compute averages
    if (sampleCount > 0) {
        analysis.avgCpuUsage /= sampleCount;
        analysis.avgMemoryUsage /= sampleCount;
        analysis.avgSpikeRate /= sampleCount;
        analysis.avgFiringRate /= sampleCount;
    }
    
    // Calculate stability score based on variance
    if (sampleCount > 1) {
        double cpuVariance = 0.0, memoryVariance = 0.0;
        for (const auto& sample : samples_) {
            cpuVariance += std::pow(sample.cpuUsage - analysis.avgCpuUsage, 2);
            memoryVariance += std::pow(sample.memoryUsageMB - analysis.avgMemoryUsage, 2);
        }
        cpuVariance /= (sampleCount - 1);
        memoryVariance /= (sampleCount - 1);
        
        analysis.stabilityScore = 1.0 / (1.0 + std::sqrt(cpuVariance + memoryVariance));
    }
    
    // Calculate efficiency score
    analysis.efficiencyScore = (analysis.avgCpuUsage * analysis.peakMemoryUsage) / 
                              (analysis.avgFiringRate * 100.0 + 1.0); // Normalize
    
    // Calculate convergence rate (how quickly activity stabilizes)
    if (sampleCount > 10) {
        double earlyActivity = 0.0, lateActivity = 0.0;
        size_t earlySamples = sampleCount / 4;
        size_t lateSamples = sampleCount / 4;
        
        for (size_t i = 0; i < earlySamples; ++i) {
            earlyActivity += samples_[i].spikeRate;
        }
        earlyActivity /= earlySamples;
        
        for (size_t i = sampleCount - lateSamples; i < sampleCount; ++i) {
            lateActivity += samples_[i].spikeRate;
        }
        lateActivity /= lateSamples;
        
        analysis.convergenceRate = lateActivity / (earlyActivity + 0.001);
    }
    
    // Calculate activity variance
    if (sampleCount > 1) {
        double spikeVariance = 0.0, firingVariance = 0.0;
        for (const auto& sample : samples_) {
            spikeVariance += std::pow(sample.spikeRate - analysis.avgSpikeRate, 2);
            firingVariance += std::pow(sample.firingRate - analysis.avgFiringRate, 2);
        }
        spikeVariance /= (sampleCount - 1);
        firingVariance /= (sampleCount - 1);
        
        analysis.activityVariance = std::sqrt(spikeVariance + firingVariance);
    }
    
    // Calculate scalability factor (this would need additional testing)
    analysis.scalabilityFactor = 1.0; // Placeholder
    
    return analysis;
}

void PerformanceMetrics::clear() {
    std::lock_guard<std::mutex> lock(samplesMutex_);
    samples_.clear();
    isMonitoring_ = false;
}

double PerformanceMetrics::getDuration() const {
    if (!isMonitoring_) {
        return 0.0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
    return static_cast<double>(duration.count());
}

PerformanceProfiler::PerformanceProfiler() : isProfiling_(false) {}

PerformanceProfiler::~PerformanceProfiler() {
    reset();
}

void PerformanceProfiler::startProfiling(const std::string& description) {
    if (!isProfiling_) {
        isProfiling_ = true;
        startTime_ = std::chrono::steady_clock::now();
    }
}

void PerformanceProfiler::stopProfiling() {
    if (isProfiling_) {
        isProfiling_ = false;
    }
}

void PerformanceProfiler::recordEvent(const std::string& eventName) {
    if (isProfiling_) {
        eventTimes_[eventName].push_back(std::chrono::steady_clock::now());
    }
}

void PerformanceProfiler::recordMetric(const std::string& metricName, double value) {
    metricValues_[metricName].push_back(value);
}

void PerformanceProfiler::reset() {
    isProfiling_ = false;
    startTime_ = std::chrono::steady_clock::time_point();
    eventTimes_.clear();
    metricValues_.clear();
}

std::string PerformanceProfiler::getResults() const {
    std::stringstream ss;
    ss << "=== Performance Profiler Results ===" << std::endl;
    ss << "Profiling active: " << (isProfiling_ ? "Yes" : "No") << std::endl;
    ss << "Duration: " << getDuration() << " seconds" << std::endl;
    ss << std::endl;
    
    ss << "Events:" << std::endl;
    for (const auto& pair : eventTimes_) {
        ss << "  " << pair.first << ": " << pair.second.size() << " occurrences" << std::endl;
    }
    
    ss << std::endl;
    ss << "Metrics:" << std::endl;
    for (const auto& pair : metricValues_) {
        ss << "  " << pair.first << ": " << pair.second.size() << " values" << std::endl;
        if (!pair.second.empty()) {
            double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
            double avg = sum / pair.second.size();
            double min = *std::min_element(pair.second.begin(), pair.second.end());
            double max = *std::max_element(pair.second.begin(), pair.second.end());
            ss << "    Average: " << avg << std::endl;
            ss << "    Min: " << min << std::endl;
            ss << "    Max: " << max << std::endl;
        }
    }
    
    return ss.str();
}

double PerformanceProfiler::getDuration() const {
    if (!isProfiling_) {
        return 0.0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
    return static_cast<double>(duration.count());
}

const std::vector<std::chrono::steady_clock::time_point>& PerformanceProfiler::getEventTimes(const std::string& eventName) const {
    auto it = eventTimes_.find(eventName);
    if (it != eventTimes_.end()) {
        return it->second;
    }
    static const std::vector<std::chrono::steady_clock::time_point> empty;
    return empty;
}

const std::vector<double>& PerformanceProfiler::getMetricValues(const std::string& metricName) const {
    auto it = metricValues_.find(metricName);
    if (it != metricValues_.end()) {
        return it->second;
    }
    static const std::vector<double> empty;
    return empty;
}

PerformanceBenchmark::PerformanceBenchmark() {}

PerformanceBenchmark::~PerformanceBenchmark() {}

PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::runBenchmark(
    const std::string& testName, std::function<void()> benchmarkFunction) {
    BenchmarkResult result;
    result.testName = testName;
    
    // Start timing
    auto startTime = std::chrono::steady_clock::now();
    
    // Run benchmark
    benchmarkFunction();
    
    // Calculate duration
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    result.duration = static_cast<double>(duration.count());
    
    // For now, return placeholder metrics
    // In a real implementation, these would be collected during the benchmark
    result.cpuUsage = 50.0; // Placeholder
    result.memoryUsage = 100.0; // Placeholder
    result.spikeRate = 10.0; // Placeholder
    result.efficiency = 0.75; // Placeholder
    
    return result;
}

std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceBenchmark::runBenchmarks(
    const std::vector<std::pair<std::string, std::function<void()>>>& benchmarks) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        results.push_back(runBenchmark(benchmark.first, benchmark.second));
    }
    
    previousResults_ = results;
    return results;
}

std::string PerformanceBenchmark::generateReport(const std::vector<BenchmarkResult>& results) const {
    std::stringstream ss;
    ss << "=== Performance Benchmark Report ===" << std::endl << std::endl;
    
    if (results.empty()) {
        ss << "No benchmark results available." << std::endl;
        return ss.str();
    }
    
    ss << "Benchmark Summary:" << std::endl;
    ss << std::left << std::setw(20) << "Test" 
       << std::setw(12) << "Duration" 
       << std::setw(12) << "Spikes" 
       << std::setw(12) << "CPU %"
       << std::setw(12) << "Memory MB"
       << std::setw(12) << "Spike Rate"
       << std::setw(12) << "Efficiency" << std::endl;
    
    ss << std::string(100, '-') << std::endl;
    
    for (const auto& result : results) {
        ss << std::left << std::setw(20) << result.testName
           << std::setw(12) << result.duration << "s"
           << std::setw(12) << result.totalSpikes
           << std::setw(12) << result.cpuUsage << "%"
           << std::setw(12) << result.memoryUsage << "MB"
           << std::setw(12) << result.spikeRate << "/s"
           << std::setw(12) << result.efficiency << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceBenchmark::compareBenchmarks(const std::vector<BenchmarkResult>& results) const {
    std::stringstream ss;
    ss << "=== Benchmark Comparison ===" << std::endl << std::endl;
    
    if (results.size() < 2) {
        ss << "At least 2 benchmarks required for comparison." << std::endl;
        return ss.str();
    }
    
    ss << "Comparing " << results.size() << " benchmarks:" << std::endl;
    
    // Find best performer for each metric
    double bestCpu = 0.0, bestMemory = 0.0, bestSpikeRate = 0.0, bestEfficiency = 0.0;
    std::string bestCpuName, bestMemoryName, bestSpikeRateName, bestEfficiencyName;
    
    for (const auto& result : results) {
        if (result.cpuUsage > bestCpu) {
            bestCpu = result.cpuUsage;
            bestCpuName = result.testName;
        }
        if (result.memoryUsage < bestMemory || bestMemory == 0.0) {
            bestMemory = result.memoryUsage;
            bestMemoryName = result.testName;
        }
        if (result.spikeRate > bestSpikeRate) {
            bestSpikeRate = result.spikeRate;
            bestSpikeRateName = result.testName;
        }
        if (result.efficiency > bestEfficiency) {
            bestEfficiency = result.efficiency;
            bestEfficiencyName = result.testName;
        }
    }
    
    ss << "Best performers:" << std::endl;
    ss << "  - CPU Usage: " << bestCpuName << " (" << bestCpu << "%)" << std::endl;
    ss << "  - Memory Usage: " << bestMemoryName << " (" << bestMemory << " MB)" << std::endl;
    ss << "  - Spike Rate: " << bestSpikeRateName << " (" << bestSpikeRate << "/s)" << std::endl;
    ss << "  - Efficiency: " << bestEfficiencyName << " (" << bestEfficiency << ")" << std::endl;
    
    return ss.str();
}

bool PerformanceBenchmark::saveResults(const std::string& filename, 
                                       const std::vector<BenchmarkResult>& results) const {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // For now, just write a simple text format
        // In a real implementation, this would use proper serialization
        file << "NLM Benchmark Results" << std::endl;
        file << "Version: 1.0" << std::endl;
        file << "Generated: " << std::chrono::system_clock::now() << std::endl;
        file << "Number of results: " << results.size() << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << "Test: " << result.testName << std::endl;
            file << "Duration: " << result.duration << std::endl;
            file << "Spikes: " << result.totalSpikes << std::endl;
            file << "CPU Usage: " << result.cpuUsage << std::endl;
            file << "Memory Usage: " << result.memoryUsage << std::endl;
            file << "Spike Rate: " << result.spikeRate << std::endl;
            file << "Efficiency: " << result.efficiency << std::endl;
            file << std::endl;
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceBenchmark::loadResults(const std::string& filename, 
                                       std::vector<BenchmarkResult>& results) const {
    try {
        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::getline(file, line); // Skip header
        std::getline(file, line); // Skip version
        std::getline(file, line); // Skip timestamp
        
        // Find number of results
        std::getline(file, line);
        size_t pos = line.find(":");
        if (pos == std::string::npos) {
            return false;
        }
        
        int resultCount = std::stoi(line.substr(pos + 1));
        
        // Skip empty line
        std::getline(file, line);
        
        // Parse results
        for (int i = 0; i < resultCount; ++i) {
            BenchmarkResult result;
            
            // Parse test name
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.testName = line.substr(pos + 1);
            }
            
            // Parse other fields
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.duration = std::stod(line.substr(pos + 1));
            }
            
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.totalSpikes = std::stoull(line.substr(pos + 1));
            }
            
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.cpuUsage = std::stod(line.substr(pos + 1));
            }
            
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.memoryUsage = std::stod(line.substr(pos + 1));
            }
            
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.spikeRate = std::stod(line.substr(pos + 1));
            }
            
            std::getline(file, line);
            pos = line.find(":");
            if (pos != std::string::npos) {
                result.efficiency = std::stod(line.substr(pos + 1));
            }
            
            results.push_back(result);
            
            // Skip empty line
            if (i < resultCount - 1) {
                std::getline(file, line);
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

PerformanceMonitor::PerformanceMonitor() {}

PerformanceMonitor::~PerformanceMonitor() {}

void PerformanceMonitor::startMonitoring() {
    metrics_.startMonitoring();
}

void PerformanceMonitor::stopMonitoring() {
    metrics_.stopMonitoring();
}

PerformanceMetrics::PerformanceAnalysis PerformanceMonitor::getAnalysis() const {
    return metrics_.analyze();
}

std::string PerformanceMonitor::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Monitoring Report ===" << std::endl << std::endl;
    
    ss << "Monitoring Status: " << (isMonitoring() ? "Active" : "Inactive") << std::endl;
    ss << "Duration: " << metrics_.getDuration() << " seconds" << std::endl;
    ss << "Samples Collected: " << metrics_.getSampleCount() << std::endl;
    ss << std::endl;
    
    // Add metrics analysis
    auto analysis = getAnalysis();
    ss << "Performance Analysis:" << std::endl;
    ss << "  - Total Duration: " << analysis.totalDuration << " seconds" << std::endl;
    ss << "  - Total Steps: " << analysis.totalSteps << std::endl;
    ss << "  - Total Spikes: " << analysis.totalSpikes << std::endl;
    ss << "  - Average CPU Usage: " << analysis.avgCpuUsage << "%" << std::endl;
    ss << "  - Peak CPU Usage: " << analysis.peakCpuUsage << "%" << std::endl;
    ss << "  - Average Memory Usage: " << analysis.avgMemoryUsage << " MB" << std::endl;
    ss << "  - Peak Memory Usage: " << analysis.peakMemoryUsage << " MB" << std::endl;
    ss << "  - Average Spike Rate: " << analysis.avgSpikeRate << "/s" << std::endl;
    ss << "  - Average Firing Rate: " << analysis.avgFiringRate << "/s/neuron" << std::endl;
    ss << "  - Stability Score: " << analysis.stabilityScore << std::endl;
    ss << "  - Efficiency Score: " << analysis.efficiencyScore << std::endl;
    ss << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::reset() {
    metrics_.clear();
}

} // namespace nlm
