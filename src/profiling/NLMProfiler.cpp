// NLM Performance Profiling and Benchmarking
// Comprehensive profiling tools for NLM simulations

#include "profiling/NLMProfiler.hpp"
#include "profiling/PerformanceAnalyzer.hpp"
#include "profiling/BenchmarkSuite.hpp"
#include "profiling/ProfilerConfig.hpp"
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>

namespace nlm {

NLMProfiler::NLMProfiler() : _isRunning(false), _startTime(std::chrono::high_resolution_clock::now()) {
    NLM_LOG_INFO("Initializing NLM Performance Profiler");
}

NLMProfiler::~NLMProfiler() {
    stop();
    generateReport();
}

void NLMProfiler::start() {
    if (_isRunning) {
        return;
    }
    
    _startTime = std::chrono::high_resolution_clock::now();
    _isRunning = true;
    
    NLM_LOG_INFO("Performance profiler started");
}

void NLMProfiler::stop() {
    if (!_isRunning) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    _totalDuration = std::chrono::duration<double>(endTime - _startTime);
    _isRunning = false;
    
    NLM_LOG_INFO("Performance profiler stopped");
}

void NLMProfiler::recordEvent(const std::string& eventName, double duration) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    EventData event;
    event.name = eventName;
    event.duration = duration;
    event.timestamp = std::chrono::high_resolution_clock::now();
    event.threadId = std::this_thread::get_id();
    
    _events.push_back(event);
    
    // Update statistics
    _eventStats[eventName].totalDuration += duration;
    _eventStats[eventName].count++;
    _eventStats[eventName].maxDuration = std::max(_eventStats[eventName].maxDuration, duration);
    _eventStats[eventName].minDuration = std::min(_eventStats[eventName].minDuration, duration);
}

void NLMProfiler::recordSample(const std::string& sampleName, double value) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    SampleData sample;
    sample.name = sampleName;
    sample.value = value;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    sample.threadId = std::this_thread::get_id();
    
    _samples.push_back(sample);
}

void NLMProfiler::startTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    TimerData timer;
    timer.name = timerName;
    timer.startTime = std::chrono::high_resolution_clock::now();
    timer.threadId = std::this_thread::get_id();
    
    _timers[timerName] = timer;
}

void NLMProfiler::stopTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _timers.find(timerName);
    if (it == _timers.end()) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - it->second.startTime);
    
    recordEvent(timerName, duration);
    _timers.erase(it);
}

void NLMProfiler::addMetric(const std::string& name, double value, const std::string& category) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    MetricData metric;
    metric.name = name;
    metric.value = value;
    metric.category = category;
    metric.timestamp = std::chrono::high_resolution_clock::now();
    metric.threadId = std::this_thread::get_id();
    
    _metrics.push_back(metric);
}

const std::vector<NLMProfiler::EventData>& NLMProfiler::getEvents() const {
    return _events;
}

const std::vector<NLMProfiler::SampleData>& NLMProfiler::getSamples() const {
    return _samples;
}

const std::vector<NLMProfiler::MetricData>& NLMProfiler::getMetrics() const {
    return _metrics;
}

const std::map<std::string, NLMProfiler::EventStats>& NLMProfiler::getEventStats() const {
    return _eventStats;
}

double NLMProfiler::getTotalDuration() const {
    return _totalDuration.count();
}

void NLMProfiler::generateReport(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open report file: " + outputPath);
        return;
    }
    
    file << "=== NLM Performance Profile Report ===\n";
    file << "Generated: " << getCurrentTimestamp() << "\n";
    file << "Total Duration: " << std::fixed << std::setprecision(6) << getTotalDuration() << " seconds\n\n";
    
    // Event Statistics
    file << "=== Event Statistics ===\n";
    for (const auto& pair : _eventStats) {
        const auto& stats = pair.second;
        file << "Event: " << pair.first << "\n";
        file << "  Count: " << stats.count << "\n";
        file << "  Total Duration: " << std::fixed << std::setprecision(6) << stats.totalDuration << "s\n";
        file << "  Average Duration: " << (stats.count > 0 ? stats.totalDuration / stats.count : 0) << "s\n";
        file << "  Min Duration: " << std::fixed << std::setprecision(6) << stats.minDuration << "s\n";
        file << "  Max Duration: " << std::fixed << std::setprecision(6) << stats.maxDuration << "s\n";
        file << "  Average Frequency: " << (getTotalDuration() > 0 ? stats.count / getTotalDuration() : 0) << " events/s\n\n";
    }
    
    // Sample Statistics
    if (!_samples.empty()) {
        file << "=== Sample Statistics ===\n";
        
        std::map<std::string, std::vector<double>> samplesByName;
        for (const auto& sample : _samples) {
            samplesByName[sample.name].push_back(sample.value);
        }
        
        for (const auto& pair : samplesByName) {
            const auto& values = pair.second;
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double mean = sum / values.size();
            double variance = 0.0;
            
            for (double value : values) {
                variance += (value - mean) * (value - mean);
            }
            variance /= values.size();
            double stddev = std::sqrt(variance);
            
            file << "Sample: " << pair.first << "\n";
            file << "  Count: " << values.size() << "\n";
            file << "  Mean: " << std::fixed << std::setprecision(6) << mean << "\n";
            file << "  Std Dev: " << std::fixed << std::setprecision(6) << stddev << "\n";
            file << "  Min: " << *std::min_element(values.begin(), values.end()) << "\n";
            file << "  Max: " << *std::max_element(values.begin(), values.end()) << "\n\n";
        }
    }
    
    // Metrics Summary
    if (!_metrics.empty()) {
        file << "=== Metric Summary ===\n";
        
        std::map<std::string, std::vector<MetricData>> metricsByCategory;
        for (const auto& metric : _metrics) {
            metricsByCategory[metric.category].push_back(metric);
        }
        
        for (const auto& pair : metricsByCategory) {
            file << "Category: " << pair.first << "\n";
            
            std::map<std::string, std::vector<double>> categoryMetrics;
            for (const auto& metric : pair.second) {
                categoryMetrics[metric.name].push_back(metric.value);
            }
            
            for (const auto& metricPair : categoryMetrics) {
                const auto& values = metricPair.second;
                double sum = std::accumulate(values.begin(), values.end(), 0.0);
                double mean = sum / values.size();
                
                file << "  " << metricPair.first << ": " << std::fixed << std::setprecision(6) << mean << "\n";
            }
            file << "\n";
        }
    }
    
    file << "=== Performance Summary ===\n";
    
    // Calculate overall performance metrics
    double totalEvents = 0;
    double totalSamples = 0;
    double totalMetrics = 0;
    
    for (const auto& pair : _eventStats) {
        totalEvents += pair.second.count;
    }
    
    totalSamples = _samples.size();
    totalMetrics = _metrics.size();
    
    file << "Total Events Recorded: " << totalEvents << "\n";
    file << "Total Samples Recorded: " << totalSamples << "\n";
    file << "Total Metrics Recorded: " << totalMetrics << "\n";
    file << "Events per Second: " << (getTotalDuration() > 0 ? totalEvents / getTotalDuration() : 0) << "\n";
    file << "Samples per Second: " << (getTotalDuration() > 0 ? totalSamples / getTotalDuration() : 0) << "\n";
    
    NLM_LOG_INFO("Performance report generated: " + outputPath);
}

std::string NLMProfiler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ============================================================================
// PerformanceAnalyzer
// ============================================================================

PerformanceAnalyzer::PerformanceAnalyzer() {
    NLM_LOG_INFO("Initializing Performance Analyzer");
}

PerformanceAnalyzer::~PerformanceAnalyzer() = default;

void PerformanceAnalyzer::analyzeProfiles(const std::vector<std::string>& profileFiles) {
    NLM_LOG_INFO("Analyzing " + std::to_string(profileFiles.size()) + " profile files");
    
    for (const auto& filePath : profileFiles) {
        analyzeProfile(filePath);
    }
    
    generateComparativeReport();
}

void PerformanceAnalyzer::analyzeProfile(const std::string& profileFile) {
    // Load profile data
    std::ifstream file(profileFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open profile file: " + profileFile);
        return;
    }
    
    // Parse profile data (simplified - would need proper parsing)
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Event:") != std::string::npos) {
            // Parse event line
            // In practice, this would parse structured data
        }
    }
    
    file.close();
}

void PerformanceAnalyzer::generateComparativeReport() {
    NLM_LOG_INFO("Generating comparative performance report");
    
    // Generate comparative analysis
    // This would include:
    // - Performance comparison across runs
    // - Bottleneck identification
    // - Optimization recommendations
}

void PerformanceAnalyzer::identifyBottlenecks() {
    NLM_LOG_INFO("Identifying performance bottlenecks");
    
    // Identify slow events
    for (const auto& pair : _profiler.getEventStats()) {
        if (pair.second.count > 0) {
            double avgDuration = pair.second.totalDuration / pair.second.count;
            if (avgDuration > 0.1) {  // If average duration > 100ms
                NLM_LOG_WARNING("Potential bottleneck: " + pair.first + 
                              " (avg duration: " + std::to_string(avgDuration) + "s)");
            }
        }
    }
}

void PerformanceAnalyzer::generateOptimizationRecommendations() {
    NLM_LOG_INFO("Generating optimization recommendations");
    
    // Generate recommendations based on analysis
    NLM_LOG_INFO("Recommendations:");
    NLM_LOG_INFO("1. Consider optimizing frequently called events");
    NLM_LOG_INFO("2. Reduce event recording overhead for high-frequency events");
    NLM_LOG_INFO("3. Use batch operations for related events");
    NLM_LOG_INFO("4. Profile memory usage alongside CPU usage");
}

// ============================================================================
// BenchmarkSuite
// ============================================================================

BenchmarkSuite::BenchmarkSuite() {
    NLM_LOG_INFO("Initializing Benchmark Suite");
}

BenchmarkSuite::~BenchmarkSuite() {
    generateReport();
}

void BenchmarkSuite::addBenchmark(const std::string& name, std::function<void()> benchmarkFunction) {
    BenchmarkData benchmark;
    benchmark.name = name;
    benchmark.function = benchmarkFunction;
    
    _benchmarks.push_back(benchmark);
}

void BenchmarkSuite::runBenchmarks() {
    NLM_LOG_INFO("Running " + std::to_string(_benchmarks.size()) + " benchmarks");
    
    for (auto& benchmark : _benchmarks) {
        runBenchmark(benchmark);
    }
}

void BenchmarkSuite::runBenchmark(BenchmarkData& benchmark) {
    NLM_LOG_INFO("Running benchmark: " + benchmark.name);
    
    // Warm-up
    benchmark.function();
    
    // Actual benchmark run
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < benchmark.iterations; ++i) {
        benchmark.function();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    benchmark.totalTime = duration.count() / benchmark.iterations;
    
    NLM_LOG_INFO("Benchmark " + benchmark.name + " completed in " + 
                std::to_string(benchmark.totalTime) + "s per iteration");
}

void BenchmarkSuite::generateReport() {
    std::ofstream file("benchmark_results.txt");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open benchmark report file");
        return;
    }
    
    file << "=== NLM Benchmark Results ===\n";
    file << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& benchmark : _benchmarks) {
        file << "Benchmark: " << benchmark.name << "\n";
        file << "  Iterations: " << benchmark.iterations << "\n";
        file << "  Total Time: " << std::fixed << std::setprecision(6) << benchmark.totalTime << "s\n";
        file << "  Operations per Second: " << (benchmark.totalTime > 0 ? 1.0 / benchmark.totalTime : 0) << "\n";
        file << "\n";
    }
    
    NLM_LOG_INFO("Benchmark report generated");
}

// ============================================================================
// ProfilerConfig
// ============================================================================

ProfilerConfig::ProfilerConfig() {
    enableEventRecording = true;
    enableSampling = true;
    enableMetrics = true;
    eventRecordingInterval = 0.001;  // 1ms
    maxEvents = 100000;
    outputPath = "./profiler_report.txt";
    logLevel = "INFO";
}

bool ProfilerConfig::validate() const {
    if (eventRecordingInterval <= 0.0) {
        NLM_LOG_ERROR("Event recording interval must be positive");
        return false;
    }
    
    if (maxEvents <= 0) {
        NLM_LOG_ERROR("Max events must be positive");
        return false;
    }
    
    return true;
}

std::string ProfilerConfig::getOutputPath(const std::string& suffix) const {
    if (suffix.empty()) {
        return outputPath;
    }
    
    size_t dotPos = outputPath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return outputPath + "_" + suffix;
    }
    
    return outputPath.substr(0, dotPos) + "_" + suffix + ".txt";
}

} // namespace nlm