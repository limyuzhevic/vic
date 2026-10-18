#include "Profiler.hpp"
#include <fstream>
#include <algorithm>
#include <thread>
#include <map>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <ctime>

namespace nlm {

struct Profiler::Impl {
    struct ProfilingData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        double cpuTimeMs;
        double wallClockTimeMs;
        size_t memoryUsage;
        size_t allocations;
        size_t deallocations;
        double gpuTimeMs;
        std::unordered_map<std::string, double> additionalMetrics;
        bool isRunning;
        std::string threadName;
    };
    
    std::unordered_map<std::string, std::vector<ProfilingData>> threadProfiles;
    std::unordered_map<std::string, std::vector<double>> timingHistory;
    std::string currentThreadName;
    bool memoryTrackingEnabled;
    size_t peakMemoryUsage;
    size_t totalAllocated;
    size_t totalDeallocated;
    std::mutex mutex;
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->memoryTrackingEnabled = false;
    pImpl->peakMemoryUsage = 0;
    pImpl->totalAllocated = 0;
    pImpl->totalDeallocated = 0;
    setThreadName("main");
}

Profiler::~Profiler() = default;

void Profiler::start(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string threadName = getCurrentThreadName();
    auto& profiles = pImpl->threadProfiles[threadName];
    
    ProfilingData data;
    data.name = name;
    data.startTime = std::chrono::high_resolution_clock::now();
    data.isRunning = true;
    data.threadName = threadName;
    
    profiles.push_back(std::move(data));
}

void Profiler::stop(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string threadName = getCurrentThreadName();
    auto& profiles = pImpl->threadProfiles[threadName];
    
    for (auto& profile : profiles) {
        if (profile.name == name && profile.isRunning) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - profile.startTime);
            
            profile.cpuTimeMs = static_cast<double>(duration.count()) / 1000.0;
            profile.wallClockTimeMs = profile.cpuTimeMs;
            profile.isRunning = false;
            
            // Update timing history
            pImpl->timingHistory[name].push_back(profile.cpuTimeMs);
            break;
        }
    }
}

PerformanceMetrics Profiler::getCurrentMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PerformanceMetrics metrics(name);
    
    for (const auto& threadPair : pImpl->threadProfiles) {
        for (const auto& profile : threadPair.second) {
            if (profile.name == name && profile.isRunning) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - profile.startTime);
                
                metrics.cpuTimeMs = static_cast<double>(duration.count()) / 1000.0;
                metrics.wallClockTimeMs = metrics.cpuTimeMs;
                metrics.memoryUsage = profile.memoryUsage;
                metrics.allocations = profile.allocations;
                metrics.deallocations = profile.deallocations;
                metrics.gpuTimeMs = profile.gpuTimeMs;
                metrics.additionalMetrics = profile.additionalMetrics;
                return metrics;
            }
        }
    }
    
    return metrics;
}

std::vector<PerformanceMetrics> Profiler::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceMetrics> allMetrics;
    
    for (const auto& threadPair : pImpl->threadProfiles) {
        for (const auto& profile : threadPair.second) {
            if (!profile.isRunning) {
                PerformanceMetrics metrics(profile.name);
                metrics.cpuTimeMs = profile.cpuTimeMs;
                metrics.wallClockTimeMs = profile.wallClockTimeMs;
                metrics.memoryUsage = profile.memoryUsage;
                metrics.allocations = profile.allocations;
                metrics.deallocations = profile.deallocations;
                metrics.gpuTimeMs = profile.gpuTimeMs;
                metrics.additionalMetrics = profile.additionalMetrics;
                allMetrics.push_back(metrics);
            }
        }
    }
    
    return allMetrics;
}

void Profiler::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->threadProfiles.clear();
    pImpl->timingHistory.clear();
}

bool Profiler::exportMetrics(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    file << "  \"profiles\": [" << std::endl;
    
    bool first = true;
    for (const auto& threadPair : pImpl->threadProfiles) {
        for (const auto& profile : threadPair.second) {
            if (!profile.isRunning) {
                if (!first) file << "," << std::endl;
                first = false;
                
                file << "    {" << std::endl;
                file << "      \"name\": \"" << profile.name << "\"," << std::endl;
                file << "      \"thread\": \"" << profile.threadName << "\"," << std::endl;
                file << "      \"cpuTimeMs\": " << std::fixed << std::setprecision(3) << profile.cpuTimeMs << "," << std::endl;
                file << "      \"wallClockTimeMs\": " << std::fixed << std::setprecision(3) << profile.wallClockTimeMs << "," << std::endl;
                file << "      \"memoryUsage\": " << profile.memoryUsage << "," << std::endl;
                file << "      \"allocations\": " << profile.allocations << "," << std::endl;
                file << "      \"deallocations\": " << profile.deallocations << "," << std::endl;
                file << "      \"gpuTimeMs\": " << std::fixed << std::setprecision(3) << profile.gpuTimeMs << "," << std::endl;
                file << "      \"isRunning\": false" << std::endl;
                file << "    }";
            }
        }
    }
    
    file << std::endl << "  ]," << std::endl;
    file << "  \"timingHistory\": {" << std::endl;
    
    first = true;
    for (const auto& entry : pImpl->timingHistory) {
        if (!first) file << "," << std::endl;
        first = false;
        
        file << "    \"" << entry.first << \": [";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            if (i > 0) file << ", ";
            file << std::fixed << std::setprecision(3) << entry.second[i];
        }
        file << "]";
    }
    
    file << std::endl << "  }" << std::endl;
    file << "}" << std::endl;
    
    return true;
}

bool Profiler::importMetrics(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simplified JSON parsing - in production, use a proper JSON library
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // For now, just clear and return success
    clear();
    return true;
}

std::string Profiler::generateReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== NLM Performance Profiler Report ===" << std::endl;
    oss << std::endl;
    oss << "Total profiles: " << getAllMetrics().size() << std::endl;
    oss << "Threads: " << pImpl->threadProfiles.size() << std::endl;
    oss << "Peak memory usage: " << pImpl->peakMemoryUsage << " bytes" << std::endl;
    oss << std::endl;
    
    oss << "=== Profile Details ===" << std::endl;
    auto metrics = getAllMetrics();
    for (const auto& metric : metrics) {
        oss << metric.name << ": " << std::fixed << std::setprecision(3) 
            << metric.cpuTimeMs << " ms (wall: " << metric.wallClockTimeMs << " ms)";
        if (metric.memoryUsage > 0) {
            oss << " | Memory: " << metric.memoryUsage << " bytes";
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string Profiler::generateHTMLReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html><head><title>NLM Profiler Report</title>" << std::endl;
    oss << "<style>body { font-family: Arial, sans-serif; margin: 20px; } "
        << "table { border-collapse: collapse; width: 100%; } "
        << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } "
        << "th { background-color: #f2f2f2; } "
        << ".metric-name { font-weight: bold; } "
        << ".time-cell { font-family: monospace; } "
        << ".memory-cell { font-family: monospace; }" << std::endl;
    oss << "</style></head><body>" << std::endl;
    oss << "<h1>NLM Performance Profiler Report</h1>" << std::endl;
    oss << "<p>Generated: " << ProfilerUtils::getCurrentTimestamp() << "</p>" << std::endl;
    oss << "<table>" << std::endl;
    oss << "<tr><th>Profile Name</th><th>Thread</th><th>CPU Time (ms)</th><th>Wall Clock (ms)</th>" 
        << "<th>Memory Usage (bytes)</th><th>Allocations</th><th>Deallocations</th></tr>" << std::endl;
    
    auto metrics = getAllMetrics();
    for (const auto& metric : metrics) {
        oss << "<tr>" << std::endl;
        oss << "<td class=\"metric-name\">" << metric.name << "</td>" << std::endl;
        oss << "<td>" << (metric.name.find("thread_") != std::string::npos ? "worker" : "main") 
            << "</td>" << std::endl;
        oss << "<td class=\"time-cell\">" << std::fixed << std::setprecision(3) 
            << metric.cpuTimeMs << "</td>" << std::endl;
        oss << "<td class=\"time-cell\">" << std::fixed << std::setprecision(3) 
            << metric.wallClockTimeMs << "</td>" << std::endl;
        oss << "<td class=\"memory-cell\">" << metric.memoryUsage << "</td>" << std::endl;
        oss << "<td>" << metric.allocations << "</td>" << std::endl;
        oss << "<td>" << metric.deallocations << "</td>" << std::endl;
        oss << "</tr>" << std::endl;
    }
    
    oss << "</table></body></html>" << std::endl;
    return oss.str();
}

template<typename Func>
double Profiler::benchmark(const std::string& name, Func&& func, size_t iterations) {
    start(name);
    
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    
    stop(name);
    
    auto metrics = getAllMetrics();
    for (const auto& metric : metrics) {
        if (metric.name == name) {
            return metric.cpuTimeMs / iterations;  // Average time per iteration
        }
    }
    
    return 0.0;
}

void Profiler::enableMemoryTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryTrackingEnabled = enable;
}

size_t Profiler::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->peakMemoryUsage;
}

void Profiler::setThreadName(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentThreadName = name;
}

std::string Profiler::getCurrentThreadName() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentThreadName;
}

double Profiler::getAverageTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timingHistory.find(name);
    if (it == pImpl->timingHistory.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double Profiler::getMinTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timingHistory.find(name);
    if (it == pImpl->timingHistory.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::min_element(it->second.begin(), it->second.end());
}

double Profiler::getMaxTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timingHistory.find(name);
    if (it == pImpl->timingHistory.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

double Profiler::getStdDev(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timingHistory.find(name);
    if (it == pImpl->timingHistory.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    double mean = getAverageTime(name);
    double sum = 0.0;
    for (double time : it->second) {
        sum += (time - mean) * (time - mean);
    }
    return std::sqrt(sum / (it->second.size() - 1));
}

Profiler& getProfiler() {
    static Profiler profiler;
    return profiler;
}

ScopedProfiler::ScopedProfiler(const std::string& name) 
    : name_(name), tag_("") {
    startTime_ = std::chrono::high_resolution_clock::now();
    getProfiler().start(name);
}

ScopedProfiler::~ScopedProfiler() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    double elapsedMs = static_cast<double>(duration.count()) / 1000.0;
    
    getProfiler().stop(name_);
    
    if (!tag_.empty()) {
        getProfiler().getCurrentMetrics(name_).additionalMetrics["tag"] = tag_;
    }
}

void ScopedProfiler::setTag(const std::string& tag) {
    tag_ = tag;
}

void ScopedProfiler::addMetric(const std::string& key, double value) {
    getProfiler().getCurrentMetrics(name_).additionalMetrics[key] = value;
}

BenchmarkRunner::BenchmarkRunner() 
    : numIterations_(1000), warmupEnabled_(true), warmupIterations_(100) {
    pImpl = std::make_unique<Impl>();
}

BenchmarkRunner::~BenchmarkRunner() = default;

void BenchmarkRunner::addBenchmark(const std::string& name, 
                                   std::function<double()> test,
                                   size_t iterations) {
    pImpl->benchmarks[name] = std::make_pair(test, iterations);
}

void BenchmarkRunner::runAllBenchmarks() {
    results_.clear();
    
    for (const auto& benchmark : pImpl->benchmarks) {
        const std::string& name = benchmark.first;
        const auto& test = benchmark.second.first;
        const size_t iterations = benchmark.second.second;
        
        NLM_LOG_INFO("Running benchmark: " + name + " (" + std::to_string(iterations) + " iterations)");
        
        // Warmup if enabled
        if (warmupEnabled_) {
            for (size_t i = 0; i < warmupIterations_; ++i) {
                test();
            }
        }
        
        // Run actual benchmark
        double totalTime = 0.0;
        for (size_t i = 0; i < numIterations_; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            double result = test();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            totalTime += static_cast<double>(duration.count()) / 1000.0;
        }
        
        PerformanceMetrics metrics(name);
        metrics.cpuTimeMs = totalTime;
        metrics.wallClockTimeMs = totalTime;
        metrics.additionalMetrics["iterations"] = static_cast<double>(numIterations_);
        metrics.additionalMetrics["average_time_ms"] = totalTime / numIterations_;
        metrics.additionalMetrics["result"] = test();
        
        results_.push_back(metrics);
        NLM_LOG_INFO("Benchmark completed: " + name + " - " + std::to_string(metrics.additionalMetrics["average_time_ms"]) + " ms per iteration");
    }
}

std::string BenchmarkRunner::generateBenchmarkReport() const {
    std::ostringstream oss;
    oss << "=== NLM Benchmark Report ===" << std::endl;
    oss << std::endl;
    oss << "Total benchmarks: " << results_.size() << std::endl;
    oss << "Iterations per benchmark: " << numIterations_ << std::endl;
    if (warmupEnabled_) {
        oss << "Warmup iterations: " << warmupIterations_ << std::endl;
    }
    oss << std::endl;
    
    oss << "=== Benchmark Results ===" << std::endl;
    for (const auto& metrics : results_) {
        oss << metrics.name << ": " << std::fixed << std::setprecision(3) 
            << metrics.additionalMetrics.at("average_time_ms") << " ms per iteration";
        if (metrics.additionalMetrics.count("result")) {
            oss << " (result: " << metrics.additionalMetrics.at("result") << ")";
        }
        oss << std::endl;
    }
    
    return oss.str();
}

ProfilerUtils::Utils::Utils() = default;
ProfilerUtils::Utils::~Utils() = default;

double ProfilerUtils::currentTimeMs() {
    return std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

double ProfilerUtils::elapsedTimeMs(std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

std::string ProfilerUtils::formatTime(double timeMs) {
    std::ostringstream oss;
    if (timeMs < 1.0) {
        oss << std::fixed << std::setprecision(3) << timeMs << " ms";
    } else if (timeMs < 1000.0) {
        oss << std::fixed << std::setprecision(3) << timeMs << " ms";
    } else {
        oss << std::fixed << std::setprecision(3) << timeMs / 1000.0 << " s";
    }
    return oss.str();
}

std::string ProfilerUtils::formatMemory(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024.0) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
    }
}

bool ProfilerUtils::ensureDirectoryExists(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ProfilerUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ProfilerUtils::toJSON(const PerformanceMetrics& metrics) {
    std::ostringstream oss;
    oss << "{\"name\": \"" << metrics.name << "\", \"cpuTimeMs\": " << std::fixed << std::setprecision(3) 
        << metrics.cpuTimeMs << ", \"wallClockTimeMs\": " << std::fixed << std::setprecision(3) 
        << metrics.wallClockTimeMs << ", \"memoryUsage\": " << metrics.memoryUsage 
        << ", \"allocations\": " << metrics.allocations 
        << ", \"deallocations\": " << metrics.deallocations << "}";
    return oss.str();
}

std::vector<PerformanceMetrics> ProfilerUtils::fromJSON(const std::string& jsonData) {
    return std::vector<PerformanceMetrics>();
}

} // namespace nlm
