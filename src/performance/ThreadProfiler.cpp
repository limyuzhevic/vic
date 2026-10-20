// ThreadProfiler.cpp
#include "ThreadProfiler.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

ThreadProfiler::ThreadProfiler() : running_(false), stopSampling_(false) {}
ThreadProfiler::~ThreadProfiler() {
    stopSampling();
}

void ThreadProfiler::start() {
    if (running_) return;
    
    running_ = true;
    stopSampling_ = false;
    
    samplingThread_ = std::thread(&ThreadProfiler::samplingLoop, this);
}

void ThreadProfiler::stop() {
    if (!running_) return;
    
    running_ = false;
    stopSampling_ = true;
    
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void ThreadProfiler::samplingLoop() {
    while (!stopSampling_) {
        updateSamples();
        
        std::this_thread::sleep_for(std::chrono::duration<double>(0.1)); // 100ms interval
    }
}

void ThreadProfiler::updateSamples() {
    auto now = std::chrono::high_resolution_clock::now();
    double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    // Simplified thread sampling
    // In a real implementation, this would use platform-specific APIs
    // to get thread information
    
    ThreadSample sample(timestamp, 0, "unknown", 0.0, 0.1, 0, 0, "running");
    
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.push_back(sample);
    
    // Keep only last hour of samples
    double cutoffTime = timestamp - 3600.0; // 1 hour
    while (!samples_.empty() && samples_.front().timestamp < cutoffTime) {
        samples_.erase(samples_.begin());
    }
}

size_t ThreadProfiler::getThreadCount() const {
    if (samples_.empty()) return 0;
    
    std::unordered_set<uint64_t> threadIds;
    for (const auto& sample : samples_) {
        threadIds.insert(sample.threadId);
    }
    
    return threadIds.size();
}

std::vector<uint64_t> ThreadProfiler::getActiveThreads(double startTime, double endTime) const {
    std::vector<uint64_t> activeThreads;
    
    for (const auto& sample : samples_) {
        if (sample.timestamp >= startTime && sample.timestamp <= endTime) {
            if (std::find(activeThreads.begin(), activeThreads.end(), sample.threadId) == activeThreads.end()) {
                activeThreads.push_back(sample.threadId);
            }
        }
    }
    
    return activeThreads;
}

std::vector<ThreadProfiler::ThreadAnalysis> ThreadProfiler::analyzeThreadPerformance() const {
    std::unordered_map<uint64_t, ThreadAnalysis> analysisMap;
    
    // Initialize analysis for each thread
    for (const auto& sample : samples_) {
        if (analysisMap.find(sample.threadId) == analysisMap.end()) {
            analysisMap[sample.threadId].threadId = sample.threadId;
            analysisMap[sample.threadId].threadName = sample.threadName;
        }
        
        auto& analysis = analysisMap[sample.threadId];
        analysis.cpuUtilization += sample.cpuTime;
        analysis.averageWallTime += sample.wallTime;
        analysis.peakMemoryUsage = std::max(analysis.peakMemoryUsage, sample.memoryUsage);
    }
    
    // Calculate averages
    for (auto& pair : analysisMap) {
        auto& analysis = pair.second;
        
        if (!samples_.empty()) {
            analysis.averageWallTime /= samples_.size();
            analysis.cpuUtilization /= samples_.size();
            
            if (analysis.averageWallTime > 0) {
                analysis.efficiency = (analysis.cpuUtilization / analysis.averageWallTime) * 100.0;
            }
        }
        
        // Determine bottleneck
        if (analysis.cpuUtilization > 80.0) {
            analysis.bottleneck = "cpu";
            analysis.recommendation = "Consider reducing CPU load or optimizing hot code paths";
        } else if (analysis.peakMemoryUsage > 100 * 1024 * 1024) { // 100MB
            analysis.bottleneck = "memory";
            analysis.recommendation = "Consider reducing memory usage or using memory pools";
        } else if (analysis.averageWallTime > 0.1) { // 100ms
            analysis.bottleneck = "synchronization";
            analysis.recommendation = "Consider reducing synchronization overhead or using lock-free algorithms";
        } else {
            analysis.bottleneck = "balanced";
            analysis.recommendation = "Performance is good, minor optimizations may help";
        }
    }
    
    std::vector<ThreadAnalysis> result;
    for (auto& pair : analysisMap) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::string ThreadProfiler::generateThreadReport() const {
    std::stringstream ss;
    ss << "Thread Profiler Report\n";
    ss << "======================\n\n";
    
    if (samples_.empty()) {
        ss << "No thread samples collected.\n";
        return ss.str();
    }
    
    ss << "Total samples: " << samples_.size() << "\n";
    ss << "Active threads: " << getThreadCount() << "\n\n";
    
    // Analyze thread performance
    auto analyses = analyzeThreadPerformance();
    if (!analyses.empty()) {
        ss << "Thread Performance Analysis:\n";
        for (const auto& analysis : analyses) {
            ss << "  Thread " << analysis.threadId << " (" << analysis.threadName << "):\n";
            ss << "    CPU utilization: " << analysis.cpuUtilization << "%\n";
            ss << "    Average wall time: " << analysis.averageWallTime << "s\n";
            ss << "    Peak memory usage: " << analysis.peakMemoryUsage / (1024 * 1024) << " MB\n";
            ss << "    Efficiency: " << analysis.efficiency << "%\n";
            ss << "    Bottleneck: " << analysis.bottleneck << "\n";
            ss << "    Recommendation: " << analysis.recommendation << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool ThreadProfiler::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["samples"] = json::array();
    
    for (const auto& sample : samples_) {
        json jSample;
        jSample["timestamp"] = sample.timestamp;
        jSample["threadId"] = sample.threadId;
        jSample["threadName"] = sample.threadName;
        jSample["cpuTime"] = sample.cpuTime;
        jSample["wallTime"] = sample.wallTime;
        jSample["stackSize"] = sample.stackSize;
        jSample["memoryUsage"] = sample.memoryUsage;
        jSample["state"] = sample.state;
        j["samples"].push_back(jSample);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool ThreadProfiler::loadData(const std::string& filename) {
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
        ThreadSample sample(
            jSample["timestamp"].get<double>(),
            jSample["threadId"].get<uint64_t>(),
            jSample.value("threadName", "unknown"),
            jSample.value("cpuTime", 0.0),
            jSample.value("wallTime", 0.0),
            jSample.value("stackSize", 0),
            jSample.value("memoryUsage", 0),
            jSample.value("state", "running")
        );
        samples_.push_back(sample);
    }
    
    file.close();
    return true;
}

void ThreadProfiler::clearSamples() {
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.clear();
}

} // namespace nlm
