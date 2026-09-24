#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <thread>
#include <random>

namespace nlm {

// Singleton instance management
static PerformanceProfiler* instance_ = nullptr;

PerformanceProfiler* PerformanceProfiler::getInstance() {
    if (!instance_) {
        instance_ = new PerformanceProfiler();
    }
    return instance_;
}

void PerformanceProfiler::resetInstance() {
    delete instance_;
    instance_ = nullptr;
}

PerformanceProfiler::PerformanceProfiler() 
    : active_(true), sessionStart_(std::chrono::steady_clock::now()),
      totalTime_(0), lastSystemMetrics_() {
    NLM_LOG_INFO("PerformanceProfiler initialized");
}

PerformanceProfiler::~PerformanceProfiler() {
    if (instance_ == this) {
        instance_ = nullptr;
    }
    NLM_LOG_INFO("PerformanceProfiler destroyed");
}

void PerformanceProfiler::start(const std::string& name) {
    if (!active_) return;
    
    auto now = std::chrono::steady_clock::now();
    
    // Find or create profile entry
    if (results_.find(name) == results_.end()) {
        results_[name] = ProfileResult(name);
    }
    
    // Start timing
    activeProfiles_[name] = now;
}

std::chrono::nanoseconds PerformanceProfiler::stop(const std::string& name) {
    if (!active_) return std::chrono::nanoseconds(0);
    
    auto it = activeProfiles_.find(name);
    if (it == activeProfiles_.end()) {
        return std::chrono::nanoseconds(0);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = endTime - it->second;
    activeProfiles_.erase(it);
    
    // Update profile results
    updateProfile(name, duration);
    
    return duration;
}

PerformanceProfiler::ProfilerScope PerformanceProfiler::profile(const std::string& name) {
    return ProfilerScope(this, name);
}

void PerformanceProfiler::updateProfile(const std::string& name, std::chrono::nanoseconds duration) {
    auto& result = results_[name];
    
    result.duration += duration;
    result.callCount++;
    
    // Update statistics
    double durationMs = std::chrono::duration<double, std::milli>(duration).count();
    result.totalTime += durationMs;
    
    if (result.callCount == 1) {
        result.minTime = durationMs;
        result.maxTime = durationMs;
    } else {
        result.minTime = std::min(result.minTime, durationMs);
        result.maxTime = std::max(result.maxTime, durationMs);
    }
    
    result.averageTime = result.totalTime / result.callCount;
}

const PerformanceProfiler::ProfileResult* PerformanceProfiler::getResult(const std::string& name) const {
    auto it = results_.find(name);
    if (it != results_.end()) {
        return &it->second;
    }
    return nullptr;
}

PerformanceProfiler::SystemMetrics PerformanceProfiler::collectSystemMetrics() const {
    // In a real implementation, this would collect actual system metrics
    // For now, return placeholder values
    SystemMetrics metrics = lastSystemMetrics_;
    
    // Update timestamp
    metrics.timestamp = std::chrono::steady_clock::now();
    
    return metrics;
}

PerformanceProfiler::BenchmarkResult PerformanceProfiler::runBenchmark(const std::string& benchmarkName, 
                                                                       const std::string& description,
                                                                       uint64_t neurons, uint64_t steps) {
    BenchmarkResult result(benchmarkName, description);
    result.neurons = neurons;
    result.steps = steps;
    
    NLM_LOG_INFO("Running benchmark: " + benchmarkName + " - " + description);
    NLM_LOG_INFO("  Neurons: " + std::to_string(neurons) + ", Steps: " + std::to_string(steps));
    
    // In a real implementation, this would run actual benchmarks
    // For now, return placeholder results
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate benchmark work
    for (uint64_t i = 0; i < steps; ++i) {
        // Simulate brain operations
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        
        // Simulate spikes
        if (neurons > 0) {
            static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> dist(0.1, 1.1);
            double spikeRate = dist(rng);
            result.spikesPerSecond += neurons * spikeRate;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = endTime - startTime;
    
    // Calculate spikes per second
    double totalSeconds = std::chrono::duration<double>(result.duration).count();
    if (totalSeconds > 0) {
        result.spikesPerSecond /= totalSeconds;
    }
    
    // Calculate performance score
    double timePerSpike = result.spikesPerSecond > 0 ? 1.0 / result.spikesPerSecond : 0;
    result.performanceScore = neurons * steps / (timePerSpike + 1.0);
    
    // Add to benchmarks list
    benchmarks_.push_back(result);
    
    NLM_LOG_INFO("Benchmark completed: " + benchmarkName);
    NLM_LOG_INFO("  Duration: " + std::to_string(totalSeconds) + "s");
    NLM_LOG_INFO("  Performance score: " + std::to_string(result.performanceScore));
    
    return result;
}

void PerformanceProfiler::printReport(const std::string& reportType) const {
    if (reportType == "csv") {
        writeCSV(std::cout);
    } else if (reportType == "json") {
        // JSON output would require nlohmann/json
        writeJSON(std::cout);
    } else {
        // Default detailed text report
        recursivePrint(results_["root"], 0);
    }
}

bool PerformanceProfiler::saveResults(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for saving results: " + filename);
        return false;
    }
    
    if (format == "csv") {
        writeCSV(file);
    } else if (format == "json") {
        writeJSON(file);
    } else {
        writeTXT(file);
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::loadResults(const std::string& filename, const std::string& format) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for loading results: " + filename);
        return false;
    }
    
    reset();
    
    if (format == "csv") {
        // TODO: Implement CSV loading
        NLM_LOG_WARNING("CSV loading not yet implemented");
    } else if (format == "json") {
        // TODO: Implement JSON loading with nlohmann/json
        NLM_LOG_WARNING("JSON loading not yet implemented");
    }
    
    return true;
}

void PerformanceProfiler::reset() {
    NLM_LOG_INFO("Resetting performance profiler");
    results_.clear();
    activeProfiles_.clear();
    benchmarks_.clear();
    totalTime_ = std::chrono::nanoseconds(0);
    sessionStart_ = std::chrono::steady_clock::now();
}

void PerformanceProfiler::recursivePrint(const ProfileResult& result, int depth) const {
    // Indent for depth
    std::string indent(depth * 2, ' ');
    
    // Print result info
    std::cout << indent << result.name
              << " [calls: " << result.callCount
              << ", total: " << std::fixed << std::setprecision(3) 
              << std::chrono::duration<double, std::milli>(result.duration).count() << "ms"
              << ", avg: " << std::setprecision(3) << result.averageTime << "ms"
              << ", min: " << std::setprecision(3) << result.minTime << "ms"
              << ", max: " << std::setprecision(3) << result.maxTime << "ms" << "]"
              << std::endl;
    
    // Print children
    for (const auto& child : result.children) {
        recursivePrint(child.second, depth + 1);
    }
}

void PerformanceProfiler::writeJSON(std::ostream& out) const {
    nlohmann::json j;
    
    // Convert results to JSON
    for (const auto& entry : results_) {
        nlohmann::json resultJson;
        resultJson["name"] = entry.second.name;
        resultJson["duration"] = std::chrono::duration<double, std::milli>(entry.second.duration).count();
        resultJson["call_count"] = entry.second.callCount;
        resultJson["average_time"] = entry.second.averageTime;
        resultJson["min_time"] = entry.second.minTime;
        resultJson["max_time"] = entry.second.maxTime;
        resultJson["total_time"] = entry.second.totalTime;
        
        // Add children if any
        if (!entry.second.children.empty()) {
            nlohmann::json childrenJson;
            for (const auto& child : entry.second.children) {
                childrenJson[child.first] = writeProfileResultToJSON(child.second);
            }
            resultJson["children"] = childrenJson;
        }
        
        j[entry.first] = resultJson;
    }
    
    out << j.dump(2);
}

nlohmann::json PerformanceProfiler::writeProfileResultToJSON(const ProfileResult& result) const {
    nlohmann::json j;
    j["name"] = result.name;
    j["duration"] = std::chrono::duration<double, std::milli>(result.duration).count();
    j["call_count"] = result.callCount;
    j["average_time"] = result.averageTime;
    j["min_time"] = result.minTime;
    j["max_time"] = result.maxTime;
    j["total_time"] = result.totalTime;
    return j;
}

void PerformanceProfiler::writeCSV(std::ostream& out) const {
    out << "Name,Duration(ms),CallCount,Average(ms),Min(ms),Max(ms),Total(ms)\n";
    
    for (const auto& entry : results_) {
        out << "\"" << entry.first << "\","
            << std::fixed << std::setprecision(3) 
            << std::chrono::duration<double, std::milli>(entry.second.duration).count() << ","
            << entry.second.callCount << ","
            << entry.second.averageTime << ","
            << entry.second.minTime << ","
            << entry.second.maxTime << ","
            << entry.second.totalTime << "\n";
    }
}

void PerformanceProfiler::writeTXT(std::ostream& out) const {
    out << "Performance Profile Report" << std::endl;
    out << "=" << std::string(50, '=') << std::endl;
    out << std::endl;
    
    // Session info
    auto totalTime = std::chrono::duration<double, std::milli>(totalTime_).count();
    out << "Session Time: " << totalTime << "ms" << std::endl;
    out << "Active Profiles: " << activeProfiles_.size() << std::endl;
    out << "Total Operations: " << results_.size() << std::endl;
    out << std::endl;
    
    // Performance score
    double score = getPerformanceScore();
    out << "Performance Score: " << score << std::endl;
    out << std::endl;
    
    // Detailed results
    recursivePrint(results_["root"], 0);
}

double PerformanceProfiler::getPerformanceScore() const {
    return calculatePerformanceScore();
}

double PerformanceProfiler::calculatePerformanceScore() const {
    if (results_.empty()) return 0.0;
    
    double totalScore = 0.0;
    double totalWeight = 0.0;
    
    for (const auto& entry : results_) {
        double durationScore = std::chrono::duration<double, std::milli>(entry.second.duration).count();
        double callScore = static_cast<double>(entry.second.callCount);
        double avgScore = entry.second.averageTime;
        
        // Weighted score (prefer faster, more frequent operations)
        double score = durationScore * callScore / (avgScore + 1.0);
        double weight = durationScore + 1.0;
        
        totalScore += score * weight;
        totalWeight += weight;
    }
    
    return totalWeight > 0 ? totalScore / totalWeight : 0.0;
}

} // namespace nlm
