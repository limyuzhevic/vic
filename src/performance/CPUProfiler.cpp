// CPUProfiler.cpp
#include "CPUProfiler.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>

using json = nlohmann::json;

namespace nlm {

CPUProfiler::CPUProfiler() : running_(false), stopSampling_(false) {}
CPUProfiler::~CPUProfiler() {
    stopSampling();
}

void CPUProfiler::start() {
    if (running_) return;
    
    running_ = true;
    stopSampling_ = false;
    
    samplingThread_ = std::thread(&CPUProfiler::samplingLoop, this);
}

void CPUProfiler::stop() {
    if (!running_) return;
    
    running_ = false;
    stopSampling_ = true;
    
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void CPUProfiler::samplingLoop() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (!stopSampling_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime);
        lastTime = currentTime;
        
        updateSamples();
        
        std::this_thread::sleep_for(std::chrono::duration<double>(0.1)); // 100ms interval
    }
}

void CPUProfiler::updateSamples() {
    auto now = std::chrono::high_resolution_clock::now();
    double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    // Simplified CPU time calculation
    double cpuTime = std::min(1.0, timestamp - (samples_.empty() ? timestamp : samples_.back().timestamp));
    double wallTime = 0.1; // 100ms sampling interval
    
    CPUSample sample(timestamp, cpuTime, wallTime);
    
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.push_back(sample);
    
    // Keep only last hour of samples
    double cutoffTime = timestamp - 3600.0; // 1 hour
    while (!samples_.empty() && samples_.front().timestamp < cutoffTime) {
        samples_.erase(samples_.begin());
    }
}

double CPUProfiler::getCPUUtilization() const {
    if (samples_.empty()) return 0.0;
    
    double totalWallTime = 0;
    double totalCPUTime = 0;
    
    for (const auto& sample : samples_) {
        totalWallTime += sample.wallTime;
        totalCPUTime += sample.cpuTime;
    }
    
    if (totalWallTime <= 0) return 0.0;
    
    return totalCPUTime / totalWallTime;
}

double CPUProfiler::getCPUUtilization(double startTime, double endTime) const {
    std::vector<CPUSample> relevantSamples;
    
    for (const auto& sample : samples_) {
        if (sample.timestamp >= startTime && sample.timestamp <= endTime) {
            relevantSamples.push_back(sample);
        }
    }
    
    if (relevantSamples.empty()) return 0.0;
    
    double totalWallTime = 0;
    double totalCPUTime = 0;
    
    for (const auto& sample : relevantSamples) {
        totalWallTime += sample.wallTime;
        totalCPUTime += sample.cpuTime;
    }
    
    if (totalWallTime <= 0) return 0.0;
    
    return totalCPUTime / totalWallTime;
}

std::vector<CPUProfiler::CPUBottleneck> CPUProfiler::analyzeCPUUsage() const {
    std::vector<CPUBottleneck> bottlenecks;
    
    if (samples_.empty()) return bottlenecks;
    
    // Group samples by 1-second intervals
    std::unordered_map<double, std::vector<CPUSample>> timeBuckets;
    
    for (const auto& sample : samples_) {
        double bucket = std::floor(sample.timestamp / 1.0) * 1.0; // 1-second buckets
        timeBuckets[bucket].push_back(sample);
    }
    
    // Analyze each bucket
    for (const auto& pair : timeBuckets) {
        double bucketTime = pair.first;
        const auto& bucketSamples = pair.second;
        
        double totalWallTime = 0;
        double totalCPUTime = 0;
        
        for (const auto& sample : bucketSamples) {
            totalWallTime += sample.wallTime;
            totalCPUTime += sample.cpuTime;
        }
        
        if (totalWallTime <= 0) continue;
        
        double cpuPercentage = (totalCPUTime / totalWallTime) * 100.0;
        double efficiency = (totalCPUTime / totalWallTime) * 100.0;
        
        CPUBottleneck bottleneck;
        bottleneck.cpuPercentage = cpuPercentage;
        bottleneck.wallTimePercentage = (totalWallTime / samples_.size()) * 100.0; // Simplified
        bottleneck.efficiency = efficiency;
        
        // Identify bottleneck type
        if (cpuPercentage > 90.0) {
            bottleneck.location = "CPU bound";
            bottleneck.description = "High CPU utilization indicates CPU bottleneck";
            bottleneck.impact = cpuPercentage / 100.0;
            bottleneck.recommendation = "Consider optimizing hot code paths, using SIMD, or parallel processing";
        } else if (cpuPercentage < 10.0) {
            bottleneck.location = "Memory bound";
            bottleneck.description = "Low CPU utilization suggests memory bandwidth bottleneck";
            bottleneck.impact = (100.0 - cpuPercentage) / 100.0;
            bottleneck.recommendation = "Consider reducing memory accesses, using cache-friendly data structures";
        } else {
            bottleneck.location = "Balanced";
            bottleneck.description = "CPU utilization is within normal range";
            bottleneck.impact = 0.1;
            bottleneck.recommendation = "Performance is acceptable, minor optimizations may help";
        }
        
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::string CPUProfiler::generateCPUReport() const {
    std::stringstream ss;
    ss << "CPU Profiler Report\n";
    ss << "===================\n\n";
    
    if (samples_.empty()) {
        ss << "No CPU samples collected.\n";
        return ss.str();
    }
    
    // Calculate statistics
    double totalWallTime = 0;
    double totalCPUTime = 0;
    double maxCPU = 0;
    double minCPU = 100;
    
    for (const auto& sample : samples_) {
        totalWallTime += sample.wallTime;
        totalCPUTime += sample.cpuTime;
        maxCPU = std::max(maxCPU, sample.cpuTime);
        minCPU = std::min(minCPU, sample.cpuTime);
    }
    
    if (totalWallTime <= 0) return ss.str();
    
    ss << "Total samples: " << samples_.size() << "\n";
    ss << "Total wall time: " << totalWallTime << "s\n";
    ss << "Total CPU time: " << totalCPUTime << "s\n";
    ss << "Average CPU utilization: " << (totalCPUTime / totalWallTime * 100.0) << "%\n";
    ss << "Peak CPU utilization: " << (maxCPU * 100.0) << "%\n";
    ss << "Minimum CPU utilization: " << (minCPU * 100.0) << "%\n\n";
    
    // Analyze bottlenecks
    auto bottlenecks = analyzeCPUUsage();
    if (!bottlenecks.empty()) {
        ss << "CPU Bottleneck Analysis:\n";
        for (const auto& bottleneck : bottlenecks) {
            ss << "  " << bottleneck.location << ": " << bottleneck.cpuPercentage << "% utilization\n";
            ss << "    Impact: " << bottleneck.impact * 100 << "%\n";
            ss << "    Recommendation: " << bottleneck.recommendation << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string CPUProfiler::generateCPUHeatmap() const {
    std::stringstream ss;
    ss << "CPU Heatmap\n";
    ss << "===========\n\n";
    
    if (samples_.empty()) {
        ss << "No CPU data available.\n";
        return ss.str();
    }
    
    // Create simple CPU heatmap
    ss << "CPU utilization over time (1-second intervals):\n";
    
    // Group samples by second
    std::unordered_map<size_t, std::vector<CPUSample>> timeBuckets;
    
    for (const auto& sample : samples_) {
        size_t second = static_cast<size_t>(sample.timestamp);
        timeBuckets[second].push_back(sample);
    }
    
    for (const auto& pair : timeBuckets) {
        size_t second = pair.first;
        const auto& bucketSamples = pair.second;
        
        double totalCPU = 0;
        for (const auto& sample : bucketSamples) {
            totalCPU += sample.cpuTime;
        }
        
        double cpuPercent = (totalCPU / bucketSamples.size()) * 100.0;
        
        ss << "  " << second << "s: [";
        int barLength = int(cpuPercent / 5); // 5% per character
        for (int i = 0; i < barLength && i < 20; ++i) ss << "#";
        ss << " " << std::fixed << std::setprecision(1) << cpuPercent << "%\n";
    }
    
    return ss.str();
}

bool CPUProfiler::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["samples"] = json::array();
    
    for (const auto& sample : samples_) {
        json jSample;
        jSample["timestamp"] = sample.timestamp;
        jSample["cpuTime"] = sample.cpuTime;
        jSample["wallTime"] = sample.wallTime;
        
        // Add thread CPU times
        if (!sample.threadCPUTimes.empty()) {
            jSample["threadCPUTimes"] = sample.threadCPUTimes;
        }
        
        j["samples"].push_back(jSample);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool CPUProfiler::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearSamples();
    
    for (const auto& jSample : j["samples"]) {
        CPUSample sample(
            jSample["timestamp"].get<double>(),
            jSample["cpuTime"].get<double>(),
            jSample["wallTime"].get<double>()
        );
        
        // Load thread CPU times
        if (jSample.contains("threadCPUTimes")) {
            sample.threadCPUTimes = jSample["threadCPUTimes"].get<std::unordered_map<uint64_t, double>>();
        }
        
        samples_.push_back(sample);
    }
    
    file.close();
    return true;
}

void CPUProfiler::clearSamples() {
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.clear();
}

} // namespace nlm
