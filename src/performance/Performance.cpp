#include "Performance.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct Profiler::Impl {
    struct ProfileRecord {
        double startTime;
        double elapsedTime;
        std::string name;
    };
    
    std::vector<ProfileRecord> activeProfiles;
    std::map<std::string, ProfileData> profileData;
    std::vector<ProfileCallback> callbacks;
};

void Profiler::startProfile(const std::string& name) {
    ProfileRecord record;
    record.name = name;
    record.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9; // nanoseconds to seconds
    pImpl->activeProfiles.push_back(record);
}

void Profiler::stopProfile(const std::string& name) {
    if (pImpl->activeProfiles.empty()) return;
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    // Find and remove the most recent matching profile
    for (auto it = pImpl->activeProfiles.rbegin(); it != pImpl->activeProfiles.rend(); ++it) {
        if (it->name == name) {
            double elapsed = endTime - it->startTime;
            
            // Update profile data
            auto& data = pImpl->profileData[name];
            data.name = name;
            data.totalTime += elapsed;
            data.minTime = (data.minTime == 0 || elapsed < data.minTime) ? elapsed : data.minTime;
            data.maxTime = (elapsed > data.maxTime) ? elapsed : data.maxTime;
            data.callCount++;
            data.averageTime = data.totalTime / data.callCount;
            
            // Notify callbacks
            for (auto& callback : pImpl->callbacks) {
                callback(data);
            }
            
            // Remove this profile record
            auto revIt = std::make_reverse_iterator(it);
            pImpl->activeProfiles.erase(std::next(revIt).base());
            return;
        }
    }
}

const Profiler::ProfileData* Profiler::getProfile(const std::string& name) const {
    auto it = pImpl->profileData.find(name);
    return it != pImpl->profileData.end() ? &it->second : nullptr;
}

const std::map<std::string, Profiler::ProfileData>& Profiler::getAllProfiles() const {
    return pImpl->profileData;
}

void Profiler::clear() {
    pImpl->activeProfiles.clear();
    pImpl->profileData.clear();
}

double Profiler::getTotalTime() const {
    double total = 0.0;
    for (const auto& pair : pImpl->profileData) {
        total += pair.second.totalTime;
    }
    return total;
}

size_t Profiler::getProfileCount() const {
    return pImpl->profileData.size();
}

std::string Profiler::exportProfileData() const {
    std::ostringstream oss;
    oss << "NLM Performance Profile Data" << std::endl;
    oss << "Generated: " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
    oss << std::endl;
    
    oss << "Profile Statistics:" << std::endl;
    oss << "  Total profiles: " << getProfileCount() << std::endl;
    oss << "  Total execution time: " << getTotalTime() << "s" << std::endl;
    oss << std::endl;
    
    oss << "Detailed Profiles:" << std::endl;
    for (const auto& pair : pImpl->profileData) {
        const auto& data = pair.second;
        oss << "  " << data.name << ":" << std::endl;
        oss << "    Calls: " << data.callCount << std::endl;
        oss << "    Total time: " << data.totalTime << "s" << std::endl;
        oss << "    Average time: " << data.averageTime << "s" << std::endl;
        oss << "    Min time: " << data.minTime << "s" << std::endl;
        oss << "    Max time: " << data.maxTime << "s" << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

void Profiler::exportProfileToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportProfileData();
        file.close();
    }
}

void Profiler::registerCallback(ProfileCallback callback) {
    pImpl->callbacks.push_back(callback);
}

std::vector<std::string> Profiler::getActiveProfiles() const {
    std::vector<std::string> names;
    for (const auto& record : pImpl->activeProfiles) {
        names.push_back(record.name);
    }
    return names;
}

void Profiler::merge(const Profiler& other) {
    for (const auto& pair : other.pImpl->profileData) {
        auto& thisData = pImpl->profileData[pair.first];
        
        // Merge profile data
        thisData.name = pair.second.name;
        thisData.totalTime += pair.second.totalTime;
        thisData.minTime = std::min(thisData.minTime, pair.second.minTime);
        thisData.maxTime = std::max(thisData.maxTime, pair.second.maxTime);
        thisData.callCount += pair.second.callCount;
        thisData.averageTime = thisData.totalTime / thisData.callCount;
    }
}

void Profiler::resetSession() {
    clear();
}

struct PerformanceAnalyzer::Impl {
    std::vector<BenchmarkResult> results;
    double memorySampleInterval = 1000.0;
};

PerformanceAnalyzer::BenchmarkResult PerformanceAnalyzer::runBenchmark(const std::string& name, BenchmarkFunction benchmark,
                                                                        size_t iterations, bool warmUp) {
    BenchmarkResult result(name);
    
    // Warm up
    if (warmUp) {
        for (size_t i = 0; i < warmUpIterations_; ++i) {
            benchmark();
        }
    }
    
    // Actual benchmark
    Timer timer;
    timer.start();
    
    for (size_t i = 0; i < benchmarkIterations_; ++i) {
        benchmark();
    }
    
    timer.stop();
    
    result.executionTime = timer.getElapsedTime();
    result.memoryUsage = measureMemoryUsage();
    result.operationsPerSecond = (benchmarkIterations_ * 1000.0) / result.executionTime; // Assuming milliseconds
    
    results.push_back(result);
    return result;
}

std::vector<PerformanceAnalyzer::BenchmarkResult> PerformanceAnalyzer::runComparisonBenchmark(
    const std::vector<std::pair<std::string, BenchmarkFunction>>& benchmarks,
    size_t iterations) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& pair : benchmarks) {
        results.push_back(runBenchmark(pair.first, pair.second, iterations));
    }
    
    return results;
}

void PerformanceAnalyzer::profileBrainComponents(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    // Profile initialization
    Timer timer;
    timer.start();
    brain->initialize();
    timer.stop();
    
    // Profile simulation steps
    Timer simTimer;
    simTimer.start();
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    simTimer.stop();
    
    // Record metrics
    double initTime = timer.getElapsedTime();
    double simTime = simTimer.getElapsedTime();
    
    std::cout << "Brain component profiling:" << std::endl;
    std::cout << "  Initialization: " << initTime << "s" << std::endl;
    std::cout << "  Simulation (1000 steps): " << simTime << "s" << std::endl;
    std::cout << "  Average step time: " << (simTime / 1000.0) << "s" << std::endl;
    std::cout << "  Steps per second: " << (1000.0 / simTime) << std::endl;
}

void PerformanceAnalyzer::profileSimulation(const std::string& testName, std::shared_ptr<Brain> brain,
                                            SimulationStep steps, Timestamp timestep) {
    Timer timer;
    timer.start();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, timestep);
    }
    
    timer.stop();
    
    BenchmarkResult result(testName);
    result.executionTime = timer.getElapsedTime();
    result.operationsPerSecond = (steps * 1000.0) / result.executionTime; // Operations per second
    
    results.push_back(result);
    
    std::cout << "Simulation benchmark: " << testName << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    std::cout << "  Execution time: " << result.executionTime << "s" << std::endl;
    std::cout << "  Steps per second: " << result.operationsPerSecond << std::endl;
}

double PerformanceAnalyzer::measureMemoryUsage() const {
    // Simplified memory measurement - in real implementation, use platform-specific APIs
    return 0.0; // Placeholder
}

std::vector<std::string> PerformanceAnalyzer::getPerformanceRecommendations() const {
    std::vector<std::string> recommendations;
    
    for (const auto& result : results) {
        if (result.executionTime > 1.0) {
            recommendations.push_back("Optimize " + result.testName + ": High execution time (>1s)");
        }
        
        if (result.operationsPerSecond < 10000) {
            recommendations.push_back("Improve performance of " + result.testName + ": Low operations per second (<10k)");
        }
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Performance looks good! All benchmarks within acceptable ranges.");
    }
    
    return recommendations;
}

std::string PerformanceAnalyzer::exportAnalysis() const {
    std::ostringstream oss;
    oss << "NLM Performance Analysis" << std::endl;
    oss << std::endl;
    
    oss << "Benchmark Results:" << std::endl;
    for (const auto& result : results) {
        oss << "  " << result.testName << ":" << std::endl;
        oss << "    Execution time: " << result.executionTime << "s" << std::endl;
        oss << "    Operations/sec: " << result.operationsPerSecond << std::endl;
        oss << "    Memory usage: " << result.memoryUsage << " MB" << std::endl;
        oss << std::endl;
    }
    
    oss << "Performance Recommendations:" << std::endl;
    auto recommendations = getPerformanceRecommendations();
    for (const auto& rec : recommendations) {
        oss << "  - " << rec << std::endl;
    }
    
    return oss.str();
}

void PerformanceAnalyzer::exportAnalysisToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportAnalysis();
        file.close();
    }
}

struct PerformanceMonitor::Impl {
    mutable std::vector<SystemMetrics> systemHistory;
    mutable std::vector<NeuralMetrics> neuralHistory;
    size_t samplingInterval = 100;
    bool monitoring = false;
    size_t lastSampleTime = 0;
    
    // Simple CPU measurement placeholder
    double getCpuUsage() const {
        return 0.0; // Placeholder
    }
    
    double getMemoryUsage() const {
        return 0.0; // Placeholder
    }
};

void PerformanceMonitor::startMonitoring(size_t samplingInterval) {
    monitoring_ = true;
    samplingInterval_ = samplingInterval;
    lastSampleTime_ = 0;
}

void PerformanceMonitor::stopMonitoring() {
    monitoring_ = false;
}

PerformanceMonitor::SystemMetrics PerformanceMonitor::getSystemMetrics() const {
    SystemMetrics metrics;
    metrics.cpuUsage = 0.0; // Placeholder
    metrics.memoryUsage = 0.0; // Placeholder
    metrics.diskUsage = 0.0; // Placeholder
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    return metrics;
}

PerformanceMonitor::NeuralMetrics PerformanceMonitor::getNeuralMetrics() const {
    NeuralMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
    return metrics;
}

void PerformanceMonitor::recordSystemSnapshot() {
    if (!monitoring_) return;
    
    SystemMetrics metrics = getSystemMetrics();
    pImpl->systemHistory.push_back(metrics);
}

void PerformanceMonitor::recordNeuralSnapshot(std::shared_ptr<Brain> brain) {
    if (!monitoring_) return;
    
    NeuralMetrics metrics = getNeuralMetrics();
    // TODO: Extract actual metrics from brain
    pImpl->neuralHistory.push_back(metrics);
}

double PerformanceMonitor::getAverageCpuUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        total += metrics.cpuUsage;
    }
    return total / pImpl->systemHistory.size();
}

double PerformanceMonitor::getMaxCpuUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double max = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        max = std::max(max, metrics.cpuUsage);
    }
    return max;
}

double PerformanceMonitor::getAverageMemoryUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        total += metrics.memoryUsage;
    }
    return total / pImpl->systemHistory.size();
}

double PerformanceMonitor::getMaxMemoryUsage() const {
    if (pImpl->systemHistory.empty()) return 0.0;
    
    double max = 0.0;
    for (const auto& metrics : pImpl->systemHistory) {
        max = std::max(max, metrics.memoryUsage);
    }
    return max;
}

std::string PerformanceMonitor::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "NLM Performance Monitoring Report" << std::endl;
    oss << std::endl;
    
    oss << "System Performance:" << std::endl;
    oss << "  Average CPU usage: " << getAverageCpuUsage() << "%" << std::endl;
    oss << "  Max CPU usage: " << getMaxCpuUsage() << "%" << std::endl;
    oss << "  Average memory usage: " << getAverageMemoryUsage() << " MB" << std::endl;
    oss << "  Max memory usage: " << getMaxMemoryUsage() << " MB" << std::endl;
    oss << "  Total samples: " << pImpl->systemHistory.size() << std::endl;
    oss << std::endl;
    
    oss << "Neural Performance:" << std::endl;
    oss << "  Neural samples: " << pImpl->neuralHistory.size() << std::endl;
    
    return oss.str();
}

std::string PerformanceMonitor::exportMonitoringData() const {
    std::ostringstream oss;
    oss << "NLM Performance Monitoring Data" << std::endl;
    oss << std::endl;
    
    oss << "System Metrics:" << std::endl;
    for (const auto& metrics : pImpl->systemHistory) {
        oss << "  " << metrics.timestamp << ": "
            << "CPU: " << metrics.cpuUsage << "%, "
            << "Memory: " << metrics.memoryUsage << " MB" << std::endl;
    }
    
    oss << std::endl << "Neural Metrics:" << std::endl;
    for (const auto& metrics : pImpl->neuralHistory) {
        oss << "  " << metrics.timestamp << ": "
            << "Neurons: " << metrics.totalNeurons << ", "
            << "Spikes: " << metrics.totalSpikeCount << ", "
            << "Firing rate: " << metrics.averageFiringRate << std::endl;
    }
    
    return oss.str();
}

void PerformanceMonitor::exportMonitoringToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportMonitoringData();
        file.close();
    }
}

void PerformanceMonitor::clearHistory() {
    pImpl->systemHistory.clear();
    pImpl->neuralHistory.clear();
}

} // namespace nlm