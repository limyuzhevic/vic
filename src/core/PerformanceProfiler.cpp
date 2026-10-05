#include "PerformanceProfiler.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceProfiler::Impl {
    std::vector<PerformanceResult> performanceData;
    std::chrono::system_clock::time_point measurementStartTime;
    bool isMeasuring = false;
    std::string currentOperation;
    std::atomic<bool> monitoringResources;
    std::thread resourceMonitorThread;
    std::map<std::string, double> currentResourceUsage;
    std::chrono::system_clock::time_point lastResourceUpdate;
    std::chrono::milliseconds resourceUpdateInterval;
    std::string outputFile;
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->resourceUpdateInterval = std::chrono::milliseconds(100);
    pImpl->monitoringResources = false;
}

PerformanceProfiler::~PerformanceProfiler() {
    stopResourceMonitoring();
}

bool PerformanceProfiler::runBenchmark(const BenchmarkConfig& config, const std::string& testName) {
    std::cout << "Running benchmark: " << testName << std::endl;
    
    if (config.warmUp) {
        std::cout << "Running warm-up iterations..." << std::endl;
        for (uint32_t i = 0; i < config.iterations / 10; ++i) {
            startMeasurement("warmup");
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            stopMeasurement();
        }
    }
    
    std::cout << "Running main benchmark iterations..." << std::endl;
    pImpl->performanceData.clear();
    
    for (uint32_t i = 0; i < config.iterations; ++i) {
        std::cout << "  Iteration " << (i + 1) << "/" << config.iterations << std::endl;
        
        startMeasurement(testName);
        // Simulate work - in real implementation this would run actual NLM operations
        std::this_thread::sleep_for(config.duration / config.iterations);
        PerformanceResult result = stopMeasurement();
        
        pImpl->performanceData.push_back(result);
    }
    
    std::cout << "Benchmark completed. Saving results..." << std::endl;
    
    if (!config.outputFile.empty()) {
        saveResults(config.outputFile, config.format);
    }
    
    return true;
}

bool PerformanceProfiler::runStressTest(const StressTestConfig& config) {
    std::cout << "Running stress test with type: " << config.stressType << std::endl;
    
    startResourceMonitoring();
    
    auto startTime = std::chrono::system_clock::now();
    auto endTime = startTime + config.duration;
    
    std::cout << "Stress test will run for " << std::chrono::duration_cast<std::chrono::seconds>(config.duration).count() << " seconds" << std::endl;
    
    while (std::chrono::system_clock::now() < endTime) {
        if (config.enableSimulation) {
            startMeasurement("stress_simulation");
            // Simulate neural computation stress
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            stopMeasurement();
        }
        
        // Monitor and adjust load
        auto resourceUsage = getResourceUsage();
        double cpuUsage = resourceUsage["cpu"];
        double memoryUsage = resourceUsage["memory"];
        
        if (cpuUsage > config.loadFactor * 100) {
            std::cout << "CPU usage high (" << cpuUsage << "%), reducing load" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        if (config.checkpointFrequency && (std::chrono::system_clock::now() - startTime) > std::chrono::seconds(30)) {
            std::cout << "Taking checkpoint..." << std::endl;
            // In real implementation, save checkpoint
        }
    }
    
    stopResourceMonitoring();
    
    return true;
}

bool PerformanceProfiler::runProfiling(const ProfilingConfig& config) {
    std::cout << "Starting profiling with sampling rate: " << config.samplingRate << std::endl;
    
    startResourceMonitoring();
    
    auto startTime = std::chrono::system_clock::now();
    auto endTime = startTime + config.recordingDuration;
    
    while (std::chrono::system_clock::now() < endTime) {
        // Collect profiling data
        auto resourceUsage = getResourceUsage();
        
        // In real implementation, record detailed profiling data
        // based on sampling rate and configuration
        
        std::this_thread::sleep_for(config.recordingDuration / 1000);
    }
    
    stopResourceMonitoring();
    
    return true;
}

bool PerformanceProfiler::runVisualization(const VisualizationConfig& config) {
    std::cout << "Generating visualization: " << config.outputFile << " (format: " << config.format << ")" << std::endl;
    
    // In real implementation, generate visualization based on current brain state
    // This would create plots, network diagrams, etc.
    
    std::cout << "Visualization completed (simulated)" << std::endl;
    return true;
}

bool PerformanceProfiler::runDebugMode(const DebugConfig& config) {
    std::cout << "Starting debug mode with log level: " << config.logLevel << std::endl;
    
    // In real implementation, enable debugging features
    // breakpoints, watchpoints, detailed logging
    
    std::cout << "Debug mode activated (simulated)" << std::endl;
    return true;
}

void PerformanceProfiler::startMeasurement(const std::string& operationName) {
    pImpl->isMeasuring = true;
    pImpl->currentOperation = operationName;
    pImpl->measurementStartTime = std::chrono::system_clock::now();
}

PerformanceResult PerformanceProfiler::stopMeasurement() {
    PerformanceResult result;
    
    if (!pImpl->isMeasuring) {
        return result;
    }
    
    auto endTime = std::chrono::system_clock::now();
    result.wallTime = std::chrono::duration<double>(endTime - pImpl->measurementStartTime);
    result.operationName = pImpl->currentOperation;
    
    // Simulate CPU time and other metrics
    result.cpuTime = result.wallTime * 0.8; // Assume 80% CPU usage
    result.memoryUsage = 1024 * 1024; // 1 MB simulated
    result.peakMemoryUsage = 2 * 1024 * 1024; // 2 MB simulated
    result.instructionCount = static_cast<uint64_t>(result.wallTime.count() * 1e9); // 1 billion ops/sec
    result.cacheMisses = static_cast<uint64_t>(result.instructionCount * 0.05); // 5% cache miss rate
    result.throughput = calculateThroughput(result);
    
    pImpl->isMeasuring = false;
    
    return result;
}

void PerformanceProfiler::startResourceMonitoring() {
    pImpl->monitoringResources = true;
    pImpl->currentResourceUsage.clear();
    
    pImpl->resourceMonitorThread = std::thread([this]() {
        monitorResources(pImpl->monitoringResources);
    });
}

std::map<std::string, double> PerformanceProfiler::getResourceUsage() const {
    std::map<std::string, double> usage;
    
    // Get current resource usage
    usage["cpu"] = getCurrentCpuUsage();
    usage["memory"] = getCurrentMemoryUsage();
    usage["threads"] = static_cast<double>(std::thread::hardware_concurrency());
    usage["timestamp"] = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    pImpl->currentResourceUsage = usage;
    pImpl->lastResourceUpdate = std::chrono::system_clock::now();
    
    return usage;
}

void PerformanceProfiler::stopResourceMonitoring() {
    pImpl->monitoringResources = false;
    
    if (pImpl->resourceMonitorThread.joinable()) {
        pImpl->resourceMonitorThread.join();
    }
}

bool PerformanceProfiler::saveResults(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        nlohmann::json jsonData;
        
        for (const auto& result : pImpl->performanceData) {
            nlohmann::json resultJson;
            resultJson["operation"] = result.operationName;
            resultJson["wall_time"] = result.wallTime.count();
            resultJson["cpu_time"] = result.cpuTime.count();
            resultJson["memory_usage"] = result.memoryUsage;
            resultJson["peak_memory_usage"] = result.peakMemoryUsage;
            resultJson["instruction_count"] = result.instructionCount;
            resultJson["cache_misses"] = result.cacheMisses;
            resultJson["throughput"] = result.throughput;
            
            jsonData["results"].push_back(resultJson);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonData << std::endl;
        
    } else if (format == "csv") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "operation,wall_time,wall_time_seconds,cpu_time,memory_usage,peak_memory_usage,instructions,cache_misses,throughput" << std::endl;
        
        for (const auto& result : pImpl->performanceData) {
            file << result.operationName << ","
                 << result.wallTime.count() << ","
                 << result.wallTime.count() << ","
                 << result.cpuTime.count() << ","
                 << result.memoryUsage << ","
                 << result.peakMemoryUsage << ","
                 << result.instructionCount << ","
                 << result.cacheMisses << ","
                 << result.throughput << std::endl;
        }
        
    } else {
        // Default to text format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(pImpl->performanceData);
    }
    
    return true;
}

bool PerformanceProfiler::loadResults(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        pImpl->performanceData.clear();
        
        for (const auto& resultJson : jsonData["results"]) {
            PerformanceResult result;
            result.operationName = resultJson["operation"];
            result.wallTime = std::chrono::duration<double>(resultJson["wall_time"]);
            result.cpuTime = std::chrono::duration<double>(resultJson["cpu_time"]);
            result.memoryUsage = resultJson["memory_usage"];
            result.peakMemoryUsage = resultJson["peak_memory_usage"];
            result.instructionCount = resultJson["instruction_count"];
            result.cacheMisses = resultJson["cache_misses"];
            result.throughput = resultJson["throughput"];
            
            pImpl->performanceData.push_back(result);
        }
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

double PerformanceProfiler::calculateThroughput(const PerformanceResult& result) const {
    if (result.wallTime.count() > 0) {
        return static_cast<double>(result.instructionCount) / result.wallTime.count();
    }
    return 0.0;
}

double PerformanceProfiler::calculateEfficiency(const PerformanceResult& result, const BenchmarkConfig& config) const {
    // Calculate efficiency based on ideal performance vs actual
    double idealThroughput = static_cast<double>(config.iterations) / config.duration.count();
    double actualThroughput = calculateThroughput(result);
    
    if (idealThroughput > 0) {
        return (actualThroughput / idealThroughput) * 100.0;
    }
    
    return 0.0;
}

std::string PerformanceProfiler::generateReport(const std::vector<PerformanceResult>& results) const {
    std::stringstream report;
    report << "NLM Performance Report" << std::endl;
    report << std::string(50, '=') << std::endl << std::endl;
    
    report << "Total Measurements: " << results.size() << std::endl;
    
    if (results.empty()) {
        return report.str();
    }
    
    // Calculate statistics
    std::vector<double> wallTimes;
    std::vector<double> cpuTimes;
    std::vector<double> throughputs;
    
    for (const auto& result : results) {
        wallTimes.push_back(result.wallTime.count());
        cpuTimes.push_back(result.cpuTime.count());
        throughputs.push_back(result.throughput);
    }
    
    auto avgWallTime = calculateStatistics(wallTimes);
    auto avgCpuTime = calculateStatistics(cpuTimes);
    auto avgThroughput = calculateStatistics(throughputs);
    
    report << std::endl;
    report << "Performance Summary:" << std::endl;
    report << "  Average Wall Time: " << avgWallTime << " seconds" << std::endl;
    report << "  Average CPU Time: " << avgCpuTime << " seconds" << std::endl;
    report << "  Average Throughput: " << avgThroughput << " operations/second" << std::endl;
    
    report << std::endl;
    report << "Detailed Results:" << std::endl;
    report << std::left << std::setw(20) << std::setfill(' ') << "Operation";
    report << std::setw(15) << "Wall Time (s)";
    report << std::setw(15) << "CPU Time (s)";
    report << std::setw(15) << "Memory (MB)";
    report << std::setw(15) << "Throughput" << std::endl;
    report << std::setw(65) << std::setfill('-') << "" << std::endl;
    
    for (const auto& result : results) {
        report << std::left << std::setw(20) << result.operationName;
        report << std::setw(15) << std::fixed << std::setprecision(4) << result.wallTime.count();
        report << std::setw(15) << std::fixed << std::setprecision(4) << result.cpuTime.count();
        report << std::setw(15) << (result.memoryUsage / (1024 * 1024));
        report << std::setw(15) << std::fixed << std::setprecision(2) << result.throughput << std::endl;
    }
    
    return report.str();
}

bool PerformanceProfiler::saveBenchmarkConfig(const BenchmarkConfig& config, const std::string& filepath) const {
    nlohmann::json jsonData;
    
    jsonData["output_file"] = config.outputFile;
    jsonData["format"] = config.format;
    jsonData["duration"] = std::chrono::duration_cast<std::chrono::seconds>(config.duration).count();
    jsonData["iterations"] = config.iterations;
    jsonData["warm_up"] = config.warmUp;
    jsonData["detailed"] = config.detailed;
    jsonData["parallel"] = config.parallel;
    
    // Convert custom parameters
    nlohmann::json customJson;
    for (const auto& pair : config.customParameters) {
        customJson[pair.first] = pair.second;
    }
    jsonData["custom_parameters"] = customJson;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << jsonData << std::endl;
    return true;
}

bool PerformanceProfiler::loadBenchmarkConfig(BenchmarkConfig& config, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        config.outputFile = jsonData["output_file"];
        config.format = jsonData["format"];
        config.duration = std::chrono::seconds(jsonData["duration"]);
        config.iterations = jsonData["iterations"];
        config.warmUp = jsonData["warm_up"];
        config.detailed = jsonData["detailed"];
        config.parallel = jsonData["parallel"];
        
        // Load custom parameters
        config.customParameters.clear();
        if (jsonData.contains("custom_parameters")) {
            for (auto it = jsonData["custom_parameters"].begin(); it != jsonData["custom_parameters"].end(); ++it) {
                config.customParameters[it.key()] = it.value();
            }
        }
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

BenchmarkConfig PerformanceProfiler::createCpuBenchmarkConfig(std::chrono::seconds duration) {
    BenchmarkConfig config;
    config.duration = duration;
    config.iterations = 100;
    config.warmUp = true;
    config.detailed = true;
    config.format = "json";
    config.outputFile = "cpu_benchmark_results.json";
    config.customParameters["test_type"] = "cpu_intensive";
    config.customParameters["memory_limit"] = "2GB";
    return config;
}

BenchmarkConfig PerformanceProfiler::createMemoryBenchmarkConfig(std::chrono::seconds duration) {
    BenchmarkConfig config;
    config.duration = duration;
    config.iterations = 50;
    config.warmUp = false;
    config.detailed = true;
    config.format = "csv";
    config.outputFile = "memory_benchmark_results.csv";
    config.customParameters["test_type"] = "memory_intensive";
    config.customParameters["memory_usage"] = "high";
    return config;
}

BenchmarkConfig PerformanceProfiler::createScalabilityBenchmarkConfig(std::chrono::seconds duration) {
    BenchmarkConfig config;
    config.duration = duration;
    config.iterations = 30;
    config.warmUp = true;
    config.detailed = true;
    config.format = "json";
    config.outputFile = "scalability_benchmark_results.json";
    config.customParameters["test_type"] = "scalability";
    config.customParameters["neuron_counts"] = "100, 1000, 10000";
    return config;
}

StressTestConfig PerformanceProfiler::createMemoryStressConfig(float loadFactor) {
    StressTestConfig config;
    config.loadFactor = loadFactor;
    config.stressType = "memory";
    config.duration = std::chrono::seconds(300);
    config.enableSimulation = true;
    config.checkpointFrequency = true;
    return config;
}

StressTestConfig PerformanceProfiler::createCpuStressConfig(float loadFactor) {
    StressTestConfig config;
    config.loadFactor = loadFactor;
    config.stressType = "cpu";
    config.duration = std::chrono::seconds(300);
    config.enableSimulation = true;
    config.checkpointFrequency = true;
    return config;
}

ProfilingConfig PerformanceProfiler::createDetailedProfilingConfig() {
    ProfilingConfig config;
    config.enableSampling = true;
    config.samplingRate = 0.1;
    config.trackMemory = true;
    config.trackThreads = true;
    config.trackSystem = true;
    config.recordingDuration = std::chrono::seconds(300);
    config.outputFormat = "json";
    return config;
}

VisualizationConfig PerformanceProfiler::createNetworkVisualizationConfig(const std::string& outputFile) {
    VisualizationConfig config;
    config.outputFile = outputFile;
    config.format = "png";
    config.interactive = false;
    config.animation = false;
    config.frameInterval = std::chrono::milliseconds(100);
    config.scale = 1.0;
    config.colorScheme = "default";
    config.showConnections = true;
    config.showWeights = true;
    config.showActivity = true;
    return config;
}

void PerformanceProfiler::recordPerformanceData(const PerformanceResult& result) {
    pImpl->performanceData.push_back(result);
}

double PerformanceProfiler::calculateStatistics(const std::vector<double>& values) const {
    if (values.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

void PerformanceProfiler::monitorResources(std::atomic<bool>& stopFlag) {
    while (!stopFlag) {
        // Update resource usage
        getResourceUsage();
        
        // Sleep for interval
        std::this_thread::sleep_for(pImpl->resourceUpdateInterval);
    }
}

double PerformanceProfiler::getCurrentCpuUsage() const {
    // Simulate CPU usage - in real implementation, read from system
    return 50.0 + (rand() % 50); // 50-100%
}

double PerformanceProfiler::getCurrentMemoryUsage() const {
    // Simulate memory usage - in real implementation, read from system
    return 512.0 + (rand() % 1024); // 512-1536 MB
}

std::string PerformanceProfiler::formatAsText(const PerformanceResult& result) const {
    std::stringstream ss;
    ss << result.operationName << ": "
       << std::fixed << std::setprecision(4) << result.wallTime.count() << "s ("
       << result.throughput << " ops/sec)";
    return ss.str();
}

std::string PerformanceProfiler::formatAsJson(const PerformanceResult& result) const {
    nlohmann::json json;
    json["operation"] = result.operationName;
    json["wall_time"] = result.wallTime.count();
    json["throughput"] = result.throughput;
    return json.dump();
}

std::string PerformanceProfiler::formatAsCsv(const PerformanceResult& result) const {
    std::stringstream ss;
    ss << result.operationName << ","
       << std::fixed << std::setprecision(4) << result.wallTime.count() << ","
       << result.throughput;
    return ss.str();
}

} // namespace nlm
