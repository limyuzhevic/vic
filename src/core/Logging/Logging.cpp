#include "Logging.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// PerformanceMetricsLogger implementation
struct PerformanceMetricsLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance tracking
    PerformanceMetrics currentMetrics;
    std::chrono::steady_clock::time_point stepStartTime;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> benchmarks;
    std::unordered_map<std::string, std::vector<double>> benchmarkTimes;
    
    // Neural activity tracking
    std::unordered_map<NeuronId, double> neuronFiringRates;
    std::unordered_map<SynapseId, double> synapticWeights;
    
    // Context tracking
    LoggingContext currentContext;
    
    // System monitoring
    double currentCpuUsage = 0.0;
    double currentMemoryUsage = 0.0;
    
    // Counters
    uint64_t totalSpikes = 0;
    uint64_t totalSynapticOperations = 0;
    uint64_t totalSteps = 0;
    uint64_t completedSteps = 0;
};

PerformanceMetricsLogger::PerformanceMetricsLogger() : pImpl(std::make_unique<Impl>()) {}

PerformanceMetricsLogger::PerformanceMetricsLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

PerformanceMetricsLogger::~PerformanceMetricsLogger() = default;

void PerformanceMetricsLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the log entry
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add context information
    if (!pImpl->currentContext.correlationId.empty()) {
        oss << " [Corr:" << pImpl->currentContext.correlationId << "]";
    }
    
    if (!pImpl->currentContext.componentId.empty()) {
        oss << " [" << pImpl->currentContext.componentId << "]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void PerformanceMetricsLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel PerformanceMetricsLogger::getLevel() const {
    return pImpl->level;
}

void PerformanceMetricsLogger::recordStepStart(const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentContext = context;
    pImpl->stepStartTime = std::chrono::steady_clock::now();
    pImpl->completedSteps++;
}

void PerformanceMetricsLogger::recordStepEnd(const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto stepEndTime = std::chrono::steady_clock::now();
    auto stepDuration = std::chrono::duration<double>(stepEndTime - pImpl->stepStartTime).count();
    
    pImpl->currentMetrics.stepTime = stepDuration;
    pImpl->currentMetrics.peakStepTime = std::max(pImpl->currentMetrics.peakStepTime, stepDuration);
    pImpl->currentMetrics.simulationTime += stepDuration;
    pImpl->currentMetrics.completedSteps = pImpl->completedSteps;
    
    // Log step completion
    std::ostringstream oss;
    oss << "Step " << context.stepNumber << " completed in " << std::fixed << std::setprecision(6) 
        << stepDuration << " seconds (Avg: " << (pImpl->currentMetrics.simulationTime / context.stepNumber) 
        << "s)";
    
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = oss.str();
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void PerformanceMetricsLogger::recordSpike(const LoggingContext& context, double firingRate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentContext = context;
    
    pImpl->currentMetrics.totalSpikes++;
    pImpl->currentMetrics.peakFiringRate = std::max(pImpl->currentMetrics.peakFiringRate, firingRate);
    pImpl->currentMetrics.averageFiringRate = 
        pImpl->currentMetrics.averageFiringRate * (static_cast<double>(pImpl->totalSpikes - 1) / pImpl->totalSpikes) +
        firingRate / static_cast<double>(pImpl->totalSpikes);
    
    std::ostringstream oss;
    oss << "Spike recorded: Neuron " << context.componentId << " firing at " 
        << std::fixed << std::setprecision(2) << firingRate << " Hz";
    
    LogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = oss.str();
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void PerformanceMetricsLogger::recordSynapticOperation(const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentMetrics.totalSynapticOperations++;
    
    if (pImpl->currentMetrics.totalSynapticOperations % 1000 == 0) {
        std::ostringstream oss;
        oss << "Synaptic operations: " << pImpl->currentMetrics.totalSynapticOperations << " completed";
        
        LogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = oss.str();
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        
        log(entry);
    }
}

void PerformanceMetricsLogger::recordCheckpoint(const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentMetrics.checkpointCount++;
    
    std::ostringstream oss;
    oss << "Checkpoint saved: Step " << context.stepNumber 
        << " (Total checkpoints: " << pImpl->currentMetrics.checkpointCount << ")";
    
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = oss.str();
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void PerformanceMetricsLogger::recordNeuronFiring(const LoggingContext& context, NeuronId neuronId,
                                                 double firingRate, NeuronType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuronFiringRates[neuronId] = firingRate;
    
    std::ostringstream oss;
    oss << "Neuron " << neuronId.index() << " (Type: " << static_cast<int>(type) 
        << ") firing at " << std::fixed << std::setprecision(2) << firingRate << " Hz";
    
    LogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = oss.str();
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void PerformanceMetricsLogger::recordSynapticWeight(const LoggingContext& context, SynapseId synapseId,
                                                   SynapticWeight oldWeight, SynapticWeight newWeight) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->synapticWeights[synapseId] = newWeight;
    
    double change = newWeight - oldWeight;
    
    std::ostringstream oss;
    oss << "Synaptic weight change: Synapse " << synapseId.index() 
        << " from " << std::fixed << std::setprecision(6) << oldWeight << " to " 
        << newWeight << " (Change: " << change << ")";
    
    LogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = oss.str();
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void PerformanceMetricsLogger::recordMemoryUsage(const LoggingContext& context, double memoryMB) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentMetrics.memoryUsage = memoryMB;
    pImpl->currentMetrics.peakMemoryUsage = std::max(pImpl->currentMetrics.peakMemoryUsage, memoryMB);
    
    if (memoryMB > 1000) { // Log if using more than 1GB
        std::ostringstream oss;
        oss << "High memory usage: " << std::fixed << std::setprecision(2) << memoryMB << " MB";
        
        LogEntry entry;
        entry.level = LogLevel::Warning;
        entry.message = oss.str();
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        
        log(entry);
    }
}

void PerformanceMetricsLogger::recordCpuUsage(const LoggingContext& context, double cpuPercentage) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentMetrics.cpuUsage = cpuPercentage;
    
    if (cpuPercentage > 90) { // Log if CPU usage is high
        std::ostringstream oss;
        oss << "High CPU usage: " << std::fixed << std::setprecision(1) << cpuPercentage << "%";
        
        LogEntry entry;
        entry.level = LogLevel::Warning;
        entry.message = oss.str();
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        
        log(entry);
    }
}

PerformanceMetrics PerformanceMetricsLogger::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentMetrics;
}

std::string PerformanceMetricsLogger::generatePerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    oss << "Simulation Time: " << std::fixed << std::setprecision(3) 
        << pImpl->currentMetrics.simulationTime << " seconds\n";
    oss << "Average Step Time: " << std::fixed << std::setprecision(6) 
        << pImpl->currentMetrics.stepTime << " seconds\n";
    oss << "Peak Step Time: " << std::fixed << std::setprecision(6) 
        << pImpl->currentMetrics.peakStepTime << " seconds\n";
    oss << "Memory Usage: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.memoryUsage << " MB\n";
    oss << "Peak Memory Usage: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.peakMemoryUsage << " MB\n";
    oss << "CPU Usage: " << std::fixed << std::setprecision(1) 
        << pImpl->currentMetrics.cpuUsage << "%\n";
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.averageFiringRate << " Hz\n";
    oss << "Peak Firing Rate: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.peakFiringRate << " Hz\n";
    oss << "Total Spikes: " << pImpl->currentMetrics.totalSpikes << "\n";
    oss << "Total Synaptic Operations: " << pImpl->currentMetrics.totalSynapticOperations << "\n";
    oss << "Completed Steps: " << pImpl->currentMetrics.completedSteps << "\n";
    oss << "Checkpoints Saved: " << pImpl->currentMetrics.checkpointCount << "\n";
    oss << "==========================\n";
    
    return oss.str();
}

std::string PerformanceMetricsLogger::generateNeuralAnalysisReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Neural Analysis Report ===\n";
    oss << "Recorded Neurons: " << pImpl->neuronFiringRates.size() << "\n";
    oss << "Recorded Synapses: " << pImpl->synapticWeights.size() << "\n";
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.averageFiringRate << " Hz\n";
    oss << "Peak Firing Rate: " << std::fixed << std::setprecision(2) 
        << pImpl->currentMetrics.peakFiringRate << " Hz\n";
    
    if (!pImpl->neuronFiringRates.empty()) {
        double sum = 0.0, sumSq = 0.0;
        for (const auto& pair : pImpl->neuronFiringRates) {
            sum += pair.second;
            sumSq += pair.second * pair.second;
        }
        double mean = sum / pImpl->neuronFiringRates.size();
        double variance = sumSq / pImpl->neuronFiringRates.size() - mean * mean;
        oss << "Firing Rate Variance: " << std::fixed << std::setprecision(6) << variance << "\n";
    }
    
    oss << "==========================\n";
    return oss.str();
}

void PerformanceMetricsLogger::setCurrentContext(const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentContext = context;
}

LoggingContext PerformanceMetricsLogger::getCurrentContext() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentContext;
}

void PerformanceMetricsLogger::startBenchmark(const std::string& benchmarkName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->benchmarks[benchmarkName] = std::chrono::steady_clock::now();
}

void PerformanceMetricsLogger::endBenchmark(const std::string& benchmarkName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarks.find(benchmarkName);
    if (it != pImpl->benchmarks.end()) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - it->second).count();
        
        pImpl->benchmarkTimes[benchmarkName].push_back(duration);
        
        std::ostringstream oss;
        oss << "Benchmark \"" << benchmarkName << "\" completed in " << std::fixed 
            << std::setprecision(6) << duration << " seconds";
        
        LogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = oss.str();
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        
        log(entry);
        
        pImpl->benchmarks.erase(it);
    }
}

double PerformanceMetricsLogger::getBenchmarkTime(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarkTimes.find(benchmarkName);
    if (it != pImpl->benchmarkTimes.end() && !it->second.empty()) {
        return it->second.back();
    }
    return 0.0;
}

std::string PerformanceMetricsLogger::exportMetricsJson() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    json j;
    j["simulation_time"] = pImpl->currentMetrics.simulationTime;
    j["step_time"] = pImpl->currentMetrics.stepTime;
    j["peak_step_time"] = pImpl->currentMetrics.peakStepTime;
    j["memory_usage_mb"] = pImpl->currentMetrics.memoryUsage;
    j["peak_memory_usage_mb"] = pImpl->currentMetrics.peakMemoryUsage;
    j["cpu_usage_percent"] = pImpl->currentMetrics.cpuUsage;
    j["average_firing_rate_hz"] = pImpl->currentMetrics.averageFiringRate;
    j["peak_firing_rate_hz"] = pImpl->currentMetrics.peakFiringRate;
    j["total_spikes"] = pImpl->currentMetrics.totalSpikes;
    j["total_synaptic_operations"] = pImpl->currentMetrics.totalSynapticOperations;
    j["completed_steps"] = pImpl->currentMetrics.completedSteps;
    j["checkpoint_count"] = pImpl->currentMetrics.checkpointCount;
    
    // Add neuron firing rates
    json neuronRates;
    for (const auto& pair : pImpl->neuronFiringRates) {
        neuronRates[std::to_string(pair.first.index())] = pair.second;
    }
    j["neuron_firing_rates"] = neuronRates;
    
    // Add synaptic weights
    json synapticWeights;
    for (const auto& pair : pImpl->synapticWeights) {
        synapticWeights[std::to_string(pair.first.index())] = pair.second;
    }
    j["synaptic_weights"] = synapticWeights;
    
    return j.dump(2);
}

std::string PerformanceMetricsLogger::exportMetricsCsv() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Metric,Value\n";
    oss << "SimulationTime," << std::fixed << std::setprecision(6) << pImpl->currentMetrics.simulationTime << "\n";
    oss << "AverageStepTime," << std::fixed << std::setprecision(6) << pImpl->currentMetrics.stepTime << "\n";
    oss << "PeakStepTime," << std::fixed << std::setprecision(6) << pImpl->currentMetrics.peakStepTime << "\n";
    oss << "MemoryUsageMB," << std::fixed << std::setprecision(2) << pImpl->currentMetrics.memoryUsage << "\n";
    oss << "PeakMemoryUsageMB," << std::fixed << std::setprecision(2) << pImpl->currentMetrics.peakMemoryUsage << "\n";
    oss << "CpuUsagePercent," << std::fixed << std::setprecision(1) << pImpl->currentMetrics.cpuUsage << "\n";
    oss << "AverageFiringRateHz," << std::fixed << std::setprecision(2) << pImpl->currentMetrics.averageFiringRate << "\n";
    oss << "PeakFiringRateHz," << std::fixed << std::setprecision(2) << pImpl->currentMetrics.peakFiringRate << "\n";
    oss << "TotalSpikes," << pImpl->currentMetrics.totalSpikes << "\n";
    oss << "TotalSynapticOperations," << pImpl->currentMetrics.totalSynapticOperations << "\n";
    oss << "CompletedSteps," << pImpl->currentMetrics.completedSteps << "\n";
    oss << "CheckpointCount," << pImpl->currentMetrics.checkpointCount << "\n";
    
    return oss.str();
}

// EnhancedConsoleLogger implementation
struct EnhancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool enableFiltering = false;
    std::string moduleFilter;
    std::string outputFormat = "human"; // human, json, csv, dashboard
    bool useFileOutput = false;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Progress indicator
    bool showProgress = false;
    double currentProgress = 0.0;
    std::string progressMessage;
    
    // Dashboard
    bool showDashboard = false;
    std::unordered_map<std::string, std::string> dashboardMetrics;
    
    // Log rotation
    bool enableRotation = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    size_t currentFileSize = 0;
    int rotationCount = 0;
    std::string baseFilename;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() = default;

void EnhancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply module filtering if enabled
    if (pImpl->enableFiltering && !pImpl->moduleFilter.empty()) {
        if (entry.file.find(pImpl->moduleFilter) == std::string::npos) {
            return;
        }
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output;
    
    if (pImpl->outputFormat == "json") {
        output = formatEntryJson(entry);
    } else if (pImpl->outputFormat == "csv") {
        output = formatEntryCsv(entry);
    } else if (pImpl->outputFormat == "dashboard") {
        output = formatEntryDashboard(entry);
    } else {
        output = formatEntryHuman(entry);
    }
    
    // Console output
    std::cout << output;
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output;
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size();
        
        // Check for log rotation
        if (pImpl->enableRotation && pImpl->currentFileSize > pImpl->maxFileSize) {
            rotateLogFiles();
        }
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setFilter(const std::string& moduleFilter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->moduleFilter = moduleFilter;
    pImpl->enableFiltering = !moduleFilter.empty();
}

void EnhancedConsoleLogger::clearFilter() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->moduleFilter.clear();
    pImpl->enableFiltering = false;
}

bool EnhancedConsoleLogger::shouldLog(const std::string& module, LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (level < pImpl->level) {
        return false;
    }
    
    if (pImpl->enableFiltering && !pImpl->moduleFilter.empty()) {
        if (module.find(pImpl->moduleFilter) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}

void EnhancedConsoleLogger::setOutputFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputFormat = format;
}

std::string EnhancedConsoleLogger::formatEntry(const LogEntry& entry, const LoggingContext& context) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->outputFormat == "json") {
        return formatEntryJson(entry);
    } else if (pImpl->outputFormat == "csv") {
        return formatEntryCsv(entry);
    } else if (pImpl->outputFormat == "dashboard") {
        return formatEntryDashboard(entry);
    } else {
        return formatEntryHuman(entry);
    }
}

void EnhancedConsoleLogger::setLogRotation(bool enable, size_t maxFileSize, size_t maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableRotation = enable;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Set base filename for rotation
    size_t lastSlash = filepath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        pImpl->baseFilename = filepath.substr(0, lastSlash + 1);
    } else {
        pImpl->baseFilename = ".";
    }
    
    // Open new file
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->useFileOutput = pImpl->fileOut.is_open();
    pImpl->currentFileSize = 0;
}

void EnhancedConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->useFileOutput = false;
    }
}

void EnhancedConsoleLogger::showProgressBar(double progress, const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->showProgress = true;
    pImpl->currentProgress = progress;
    pImpl->progressMessage = message;
    
    // Clear line and show progress bar
    std::cout << "\r" << std::string(80, ' '); // Clear line
    std::cout << "\r[";
    
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
    
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    
    std::cout.flush();
}

void EnhancedConsoleLogger::updateDashboard(const std::unordered_map<std::string, std::string>& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->showDashboard = true;
    pImpl->dashboardMetrics = metrics;
    
    // Clear dashboard area (simplified)
    std::cout << "\n" << std::string(80, ' ') << "\n";
    
    // Show dashboard
    std::cout << "=== DASHBOARD ===\n";
    for (const auto& pair : metrics) {
        std::cout << pair.first << ": " << pair.second << "\n";
    }
    std::cout << "==================\n";
    
    std::cout.flush();
}

void EnhancedConsoleLogger::hideDashboard() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->showDashboard = false;
    std::cout << "\n" << std::string(80, ' ') << "\n";
    std::cout.flush();
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setTimeFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // For now, just store the format (could be implemented more fully)
}

std::string EnhancedConsoleLogger::formatEntryJson(const LogEntry& entry) const {
    json j;
    j["level"] = levelToString(entry.level);
    j["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
    j["message"] = entry.message;
    j["file"] = entry.file;
    j["line"] = entry.line;
    j["function"] = entry.function;
    
    return j.dump() + "\n";
}

std::string EnhancedConsoleLogger::formatEntryCsv(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << levelToString(entry.level) << ","
        << std::chrono::system_clock::to_time_t(entry.timestamp) << ","
        << entry.message << ","
        << entry.file << ","
        << entry.line << ","
        << entry.function << "\n";
    return oss.str();
}

std::string EnhancedConsoleLogger::formatEntryDashboard(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%H:%M:%S");
    
    oss << " - " << entry.message;
    
    return oss.str() + "\n";
}

std::string EnhancedConsoleLogger::formatEntryHuman(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    if (!entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    return oss.str() + "\n";
}

void EnhancedConsoleLogger::rotateLogFiles() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rotate existing files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string source, destination;
        if (i == 0) {
            source = pImpl->baseFilename + "log.txt";
        } else {
            source = pImpl->baseFilename + "log_" + std::to_string(i - 1) + ".txt";
        }
        destination = pImpl->baseFilename + "log_" + std::to_string(i) + ".txt";
        
        std::ifstream src(source, std::ios::binary);
        std::ofstream dst(destination, std::ios::binary);
        
        if (src && dst) {
            dst << src.rdbuf();
        }
        
        if (src) {
            src.close();
            std::remove(source.c_str());
        }
    }
    
    // Reopen log file
    pImpl->fileOut.open(pImpl->baseFilename + "log.txt", std::ios::app);
    pImpl->currentFileSize = 0;
    pImpl->rotationCount++;
}

// SystemMonitor implementation
struct SystemMonitor::Impl {
    bool monitoring = false;
    std::thread monitorThread;
    mutable std::mutex mutex;
    
    // Resource tracking
    uint64_t diskReads = 0;
    uint64_t diskWrites = 0;
    uint64_t networkSent = 0;
    uint64_t networkReceived = 0;
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double memoryAvailable = 0.0;
    double memoryTotal = 0.0;
    double diskReadBytes = 0.0;
    double diskWriteBytes = 0.0;
    double networkBytesSent = 0.0;
    double networkBytesReceived = 0.0;
    
    // Performance tracking
    std::chrono::steady_clock::time_point startTime;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers;
    std::unordered_map<std::string, std::vector<double>> timerResults;
};

SystemMonitor::SystemMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->monitoring = false;
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
}

void SystemMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->monitoring) {
        return;
    }
    
    pImpl->monitoring = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    
    // Initialize resource counters (simplified - in reality would read from system)
    pImpl->diskReadBytes = 0.0;
    pImpl->diskWriteBytes = 0.0;
    pImpl->networkBytesSent = 0.0;
    pImpl->networkBytesReceived = 0.0;
    
    // Start monitoring thread
    pImpl->monitorThread = std::thread([this]() {
        // Monitor thread implementation would go here
        // For simplicity, we'll just update basic counters
        while (pImpl->monitoring) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void SystemMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->monitoring) {
        return;
    }
    
    pImpl->monitoring = false;
    
    if (pImpl->monitorThread.joinable()) {
        pImpl->monitorThread.join();
    }
}

bool SystemMonitor::isMonitoring() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->monitoring;
}

double SystemMonitor::getCpuUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->cpuUsage;
}

double SystemMonitor::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryUsage;
}

double SystemMonitor::getMemoryAvailable() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryAvailable;
}

double SystemMonitor::getMemoryTotal() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryTotal;
}

uint64_t SystemMonitor::getDiskReadBytes() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return static_cast<uint64_t>(pImpl->diskReadBytes);
}

uint64_t SystemMonitor::getDiskWriteBytes() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return static_cast<uint64_t>(pImpl->diskWriteBytes);
}

double SystemMonitor::getDiskReadRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl->startTime).count();
    return elapsed > 0 ? pImpl->diskReadBytes / elapsed : 0.0;
}

double SystemMonitor::getDiskWriteRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl->startTime).count();
    return elapsed > 0 ? pImpl->diskWriteBytes / elapsed : 0.0;
}

uint64_t SystemMonitor::getNetworkBytesSent() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return static_cast<uint64_t>(pImpl->networkBytesSent);
}

uint64_t SystemMonitor::getNetworkBytesReceived() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return static_cast<uint64_t>(pImpl->networkBytesReceived);
}

void SystemMonitor::startTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timers[timerName] = std::chrono::steady_clock::now();
}

double SystemMonitor::getElapsedTime(const std::string& timerName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timers.find(timerName);
    if (it != pImpl->timers.end()) {
        auto duration = std::chrono::duration<double>(std::chrono::steady_clock::now() - it->second).count();
        return duration;
    }
    return 0.0;
}

void SystemMonitor::resetTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->timers.find(timerName);
    if (it != pImpl->timers.end()) {
        it->second = std::chrono::steady_clock::now();
    }
}

// NeuralPerformanceTracker implementation
struct NeuralPerformanceTracker::Impl {
    std::mutex mutex;
    
    // Neural activity tracking
    std::unordered_map<NeuronId, double> neuronFiringRates;
    std::unordered_map<NeuronId, uint32_t> neuronRefractoryPeriods;
    std::unordered_map<NeuronId, std::vector<Timestamp>> neuronSpikeTimes;
    
    // Synaptic activity tracking
    std::unordered_map<SynapseId, double> synapticWeights;
    std::unordered_map<SynapseId, std::vector<double>> synapticEfficacies;
    std::unordered_map<SynapseId, uint32_t> synapsePreSpikeCounts;
    std::unordered_map<SynapseId, uint32_t> synapsePostSpikeCounts;
    
    // Network dynamics tracking
    std::unordered_map<PopulationId, double> populationFiringRates;
    std::unordered_map<PopulationId, double> populationVariances;
    
    // Learning progress tracking
    std::vector<double> learningRates;
    std::vector<double> plasticityAmounts;
    std::vector<double> weightChangeVariances;
};

NeuralPerformanceTracker::NeuralPerformanceTracker() : pImpl(std::make_unique<Impl>()) {}

NeuralPerformanceTracker::~NeuralPerformanceTracker() = default;

void NeuralPerformanceTracker::recordNeuronFiring(const LoggingContext& context, NeuronId neuronId,
                                                  double firingRate, NeuronType type, Timestamp spikeTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuronFiringRates[neuronId] = firingRate;
    pImpl->neuronSpikeTimes[neuronId].push_back(spikeTime);
}

void NeuralPerformanceTracker::recordRefractoryPeriod(const LoggingContext& context, NeuronId neuronId,
                                                      uint32_t duration) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuronRefractoryPeriods[neuronId] = duration;
}

void NeuralPerformanceTracker::recordSynapticWeightChange(const LoggingContext& context, SynapseId synapseId,
                                                         SynapticWeight oldWeight, SynapticWeight newWeight,
                                                         double changeMagnitude) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->synapticWeights[synapseId] = newWeight;
    pImpl->synapticEfficacies[synapseId].push_back(changeMagnitude);
    pImpl->synapsePreSpikeCounts[synapseId]++;
}

void NeuralPerformanceTracker::recordSynapticActivity(const LoggingContext& context, SynapseId synapseId,
                                                     double efficacy, uint32_t preSpikeCount, uint32_t postSpikeCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->synapticEfficacies[synapseId].push_back(efficacy);
    pImpl->synapsePreSpikeCounts[synapseId] = preSpikeCount;
    pImpl->synapsePostSpikeCounts[synapseId] = postSpikeCount;
}

void NeuralPerformanceTracker::recordPopulationActivity(const LoggingContext& context, PopulationId populationId,
                                                       double averageFiringRate, double populationVariance) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->populationFiringRates[populationId] = averageFiringRate;
    pImpl->populationVariances[populationId] = populationVariance;
}

void NeuralPerformanceTracker::recordConnectivityMetrics(const LoggingContext& context, double connectionDensity,
                                                         double averagePathLength, double clusteringCoefficient) {
    // Store connectivity metrics (simplified)
}

void NeuralPerformanceTracker::recordLearningProgress(const LoggingContext& context, double learningRate,
                                                      double plasticityAmount, double weightChangeVariance) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->learningRates.push_back(learningRate);
    pImpl->plasticityAmounts.push_back(plasticityAmount);
    pImpl->weightChangeVariances.push_back(weightChangeVariance);
}

NeuralPerformanceTracker::NeuralMetrics NeuralPerformanceTracker::getCurrentNeuralMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NeuralMetrics metrics;
    
    // Calculate average firing rate
    if (!pImpl->neuronFiringRates.empty()) {
        double sum = 0.0;
        for (const auto& pair : pImpl->neuronFiringRates) {
            sum += pair.second;
        }
        metrics.averageFiringRate = sum / pImpl->neuronFiringRates.size();
        
        // Find peak firing rate
        metrics.peakFiringRate = 0.0;
        for (const auto& pair : pImpl->neuronFiringRates) {
            metrics.peakFiringRate = std::max(metrics.peakFiringRate, pair.second);
        }
        
        // Calculate variance
        double mean = metrics.averageFiringRate;
        double varianceSum = 0.0;
        for (const auto& pair : pImpl->neuronFiringRates) {
            varianceSum += (pair.second - mean) * (pair.second - mean);
        }
        metrics.meanFiringRateVariance = varianceSum / pImpl->neuronFiringRates.size();
    }
    
    // Calculate average synaptic weight
    if (!pImpl->synapticWeights.empty()) {
        double sum = 0.0;
        for (const auto& pair : pImpl->synapticWeights) {
            sum += pair.second;
        }
        metrics.averageSynapticWeight = sum / pImpl->synapticWeights.size();
    }
    
    // Calculate average weight change
    double weightChangeSum = 0.0;
    uint32_t weightChangeCount = 0;
    for (const auto& efficacies : pImpl->synapticEfficacies) {
        if (!efficacies.empty()) {
            double sum = std::accumulate(efficacies.begin(), efficacies.end(), 0.0);
            weightChangeSum += sum;
            weightChangeCount += efficacies.size();
        }
    }
    metrics.averageSynapticChange = weightChangeCount > 0 ? weightChangeSum / weightChangeCount : 0.0;
    
    // Count total spikes
    metrics.totalSpikes = 0;
    for (const auto& spikes : pImpl->neuronSpikeTimes) {
        metrics.totalSpikes += spikes.size();
    }
    
    // Count total synaptic events
    metrics.totalSynapticEvents = 0;
    for (const auto& preCount : pImpl->synapsePreSpikeCounts) {
        metrics.totalSynapticEvents += preCount.second;
    }
    
    // Calculate average refractory period
    if (!pImpl->neuronRefractoryPeriods.empty()) {
        uint32_t sum = 0;
        for (const auto& pair : pImpl->neuronRefractoryPeriods) {
            sum += pair.second;
        }
        metrics.averageRefractoryPeriod = static_cast<double>(sum) / pImpl->neuronRefractoryPeriods.size();
    }
    
    // Calculate connectivity density (simplified)
    if (!pImpl->populationFiringRates.empty()) {
        double sum = 0.0;
        for (const auto& pair : pImpl->populationFiringRates) {
            sum += pair.second;
        }
        metrics.connectivityDensity = sum / pImpl->populationFiringRates.size();
    }
    
    return metrics;
}

std::string NeuralPerformanceTracker::generateNeuralReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto metrics = getCurrentNeuralMetrics();
    
    std::ostringstream oss;
    oss << "=== Neural Performance Report ===\n";
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << metrics.averageFiringRate << " Hz\n";
    oss << "Peak Firing Rate: " << std::fixed << std::setprecision(2) 
        << metrics.peakFiringRate << " Hz\n";
    oss << "Firing Rate Variance: " << std::fixed << std::setprecision(6) 
        << metrics.meanFiringRateVariance << " Hz²\n";
    oss << "Average Synaptic Weight: " << std::fixed << std::setprecision(6) 
        << metrics.averageSynapticWeight << "\n";
    oss << "Average Synaptic Change: " << std::fixed << std::setprecision(6) 
        << metrics.averageSynapticChange << "\n";
    oss << "Total Spikes: " << metrics.totalSpikes << "\n";
    oss << "Total Synaptic Events: " << metrics.totalSynapticEvents << "\n";
    oss << "Average Refractory Period: " << std::fixed << std::setprecision(1) 
        << metrics.averageRefractoryPeriod << " steps\n";
    oss << "Network Connectivity Density: " << std::fixed << std::setprecision(4) 
        << metrics.connectivityDensity << "\n";
    oss << "==========================\n";
    
    return oss.str();
}

std::string NeuralPerformanceTracker::exportNeuralMetricsJson() const {
    auto metrics = getCurrentNeuralMetrics();
    
    json j;
    j["average_firing_rate_hz"] = metrics.averageFiringRate;
    j["peak_firing_rate_hz"] = metrics.peakFiringRate;
    j["firing_rate_variance_hz2"] = metrics.meanFiringRateVariance;
    j["average_synaptic_weight"] = metrics.averageSynapticWeight;
    j["average_synaptic_change"] = metrics.averageSynapticChange;
    j["total_spikes"] = metrics.totalSpikes;
    j["total_synaptic_events"] = metrics.totalSynapticEvents;
    j["average_refractory_period_steps"] = metrics.averageRefractoryPeriod;
    j["connectivity_density"] = metrics.connectivityDensity;
    
    return j.dump(2);
}

std::string NeuralPerformanceTracker::exportNeuralMetricsCsv() const {
    auto metrics = getCurrentNeuralMetrics();
    
    std::ostringstream oss;
    oss << "Metric,Value\n";
    oss << "AverageFiringRateHz," << std::fixed << std::setprecision(2) << metrics.averageFiringRate << "\n";
    oss << "PeakFiringRateHz," << std::fixed << std::setprecision(2) << metrics.peakFiringRate << "\n";
    oss << "FiringRateVarianceHz2," << std::fixed << std::setprecision(6) << metrics.meanFiringRateVariance << "\n";
    oss << "AverageSynapticWeight," << std::fixed << std::setprecision(6) << metrics.averageSynapticWeight << "\n";
    oss << "AverageSynapticChange," << std::fixed << std::setprecision(6) << metrics.averageSynapticChange << "\n";
    oss << "TotalSpikes," << metrics.totalSpikes << "\n";
    oss << "TotalSynapticEvents," << metrics.totalSynapticEvents << "\n";
    oss << "AverageRefractoryPeriodSteps," << std::fixed << std::setprecision(1) << metrics.averageRefractoryPeriod << "\n";
    oss << "ConnectivityDensity," << std::fixed << std::setprecision(4) << metrics.connectivityDensity << "\n";
    
    return oss.str();
}

// LogAggregator implementation
struct LogAggregator::Impl {
    std::mutex mutex;
    
    std::vector<LogEntry> logEntries;
    std::vector<PerformanceMetrics> performanceMetrics;
    std::vector<NeuralPerformanceTracker::NeuralMetrics> neuralMetrics;
    
    // Analysis results
    std::vector<LogPattern> logPatterns;
    std::vector<std::string> anomalyReports;
};

LogAggregator::LogAggregator() : pImpl(std::make_unique<Impl>()) {}

void LogAggregator::addLogEntry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logEntries.push_back(entry);
}

void LogAggregator::addPerformanceMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMetrics.push_back(metrics);
}

void LogAggregator::addNeuralMetrics(const NeuralPerformanceTracker::NeuralMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuralMetrics.push_back(metrics);
}

std::vector<LogAggregator::LogPattern> LogAggregator::analyzeLogPatterns() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::unordered_map<std::string, std::unordered_map<LogLevel, uint64_t>> patternCounts;
    std::unordered_map<std::string, std::vector<Timestamp>> componentTimes;
    
    for (const auto& entry : pImpl->logEntries) {
        std::string component = entry.file.empty() ? "unknown" : entry.file;
        patternCounts[component][entry.level]++;
        componentTimes[component].push_back(std::chrono::system_clock::now());
    }
    
    std::vector<LogPattern> patterns;
    
    for (const auto& componentPair : patternCounts) {
        for (const auto& levelPair : componentPair.second) {
            LogPattern pattern;
            pattern.component = componentPair.first;
            pattern.level = levelPair.first;
            pattern.count = levelPair.second;
            
            auto it = componentTimes.find(componentPair.first);
            if (it != componentTimes.end() && !it->second.empty()) {
                pattern.firstOccurrence = it->second.front();
                pattern.lastOccurrence = it->second.back();
                
                uint64_t totalForComponent = 0;
                for (const auto& level : componentPair.second) {
                    totalForComponent += level.second;
                }
                pattern.percentage = totalForComponent > 0 ? 
                    (static_cast<double>(levelPair.second) / totalForComponent * 100.0) : 0.0;
            }
            
            patterns.push_back(pattern);
        }
    }
    
    return patterns;
}

std::string LogAggregator::generateLogSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Log Summary ===\n";
    oss << "Total Log Entries: " << pImpl->logEntries.size() << "\n";
    oss << "Total Performance Metrics: " << pImpl->performanceMetrics.size() << "\n";
    oss << "Total Neural Metrics: " << pImpl->neuralMetrics.size() << "\n";
    
    auto patterns = analyzeLogPatterns();
    oss << "\nLog Patterns by Component and Level:\n";
    for (const auto& pattern : patterns) {
        oss << pattern.component << " [" << levelToString(pattern.level) << "] : "
            << pattern.count << " (" << std::fixed << std::setprecision(1) << pattern.percentage << "%)\n";
    }
    
    oss << "==========================\n";
    return oss.str();
}

bool LogAggregator::detectAnomalies() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Simple anomaly detection: check for empty performance metrics
    if (pImpl->performanceMetrics.empty()) {
        pImpl->anomalyReports.push_back("No performance metrics recorded");
        return true;
    }
    
    // Check for performance degradation
    if (!pImpl->performanceMetrics.empty()) {
        auto latest = pImpl->performanceMetrics.back();
        if (latest.cpuUsage > 90.0 || latest.memoryUsage > 90.0) {
            pImpl->anomalyReports.push_back("High resource usage detected");
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> LogAggregator::getAnomalyReports() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->anomalyReports;
}

std::string LogAggregator::exportAggregatedLogsJson() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    json j;
    
    // Convert log entries
    json entries = json::array();
    for (const auto& entry : pImpl->logEntries) {
        json entryJson;
        entryJson["level"] = levelToString(entry.level);
        entryJson["message"] = entry.message;
        entryJson["file"] = entry.file;
        entryJson["line"] = entry.line;
        entryJson["function"] = entry.function;
        entryJson["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
        entries.push_back(entryJson);
    }
    j["log_entries"] = entries;
    
    // Convert performance metrics
    json performanceMetrics = json::array();
    for (const auto& metrics : pImpl->performanceMetrics) {
        json metricsJson;
        metricsJson["simulation_time"] = metrics.simulationTime;
        metricsJson["peak_step_time"] = metrics.peakStepTime;
        metricsJson["memory_usage"] = metrics.memoryUsage;
        metricsJson["cpu_usage"] = metrics.cpuUsage;
        metricsJson["total_spikes"] = metrics.totalSpikes;
        performanceMetrics.push_back(metricsJson);
    }
    j["performance_metrics"] = performanceMetrics;
    
    return j.dump(2);
}

std::string LogAggregator::exportAggregatedLogsCsv() const {
    std::ostringstream oss;
    
    // Log entries CSV
    oss << "Log Entries\n";
    oss << "Level,Timestamp,Message,File,Line,Function\n";
    for (const auto& entry : pImpl->logEntries) {
        oss << levelToString(entry.level) << ","
            << std::chrono::system_clock::to_time_t(entry.timestamp) << ","
            << entry.message << ","
            << entry.file << ","
            << entry.line << ","
            << entry.function << "\n";
    }
    
    oss << "\nPerformance Metrics\n";
    oss << "SimulationTime,StepTime,MemoryUsage,CPULoad,TotalSpikes\n";
    for (const auto& metrics : pImpl->performanceMetrics) {
        oss << std::fixed << std::setprecision(6) << metrics.simulationTime << ","
            << metrics.stepTime << ","
            << metrics.memoryUsage << ","
            << metrics.cpuUsage << ","
            << metrics.totalSpikes << "\n";
    }
    
    return oss.str();
}

// StructuredLogOutput implementation
struct StructuredLogOutput::Impl {
    std::ostream* jsonOutput = nullptr;
    std::ostream* csvOutput = nullptr;
    std::ostream* humanReadableOutput = nullptr;
    
    int indentation = 2;
    bool includeTimestamp = true;
    bool includeContext = true;
    
    // CSV headers
    std::vector<std::string> csvHeaders;
};

StructuredLogOutput::StructuredLogOutput() : pImpl(std::make_unique<Impl>()) {}

void StructuredLogOutput::setJsonOutput(std::ostream& output) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->jsonOutput = &output;
}

void StructuredLogOutput::setCsvOutput(std::ostream& output, const std::vector<std::string>& headers) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->csvOutput = &output;
    pImpl->csvHeaders = headers;
}

void StructuredLogOutput::setHumanReadableOutput(std::ostream& output) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->humanReadableOutput = &output;
}

void StructuredLogOutput::logEntry(const LogEntry& entry, const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->jsonOutput) {
        json j;
        j["level"] = levelToString(entry.level);
        if (pImpl->includeTimestamp) {
            j["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
        }
        j["message"] = entry.message;
        
        if (pImpl->includeContext) {
            if (!context.correlationId.empty()) {
                j["correlation_id"] = context.correlationId;
            }
            if (!context.componentId.empty()) {
                j["component_id"] = context.componentId;
            }
            j["step_number"] = context.stepNumber;
            j["simulation_time"] = context.simulationTime;
        }
        
        *pImpl->jsonOutput << j.dump(pImpl->indentation) << "\n";
    }
    
    if (pImpl->csvOutput) {
        std::ostringstream oss;
        oss << levelToString(entry.level) << ","
            << (pImpl->includeTimestamp ? std::chrono::system_clock::to_time_t(entry.timestamp) : "") << ","
            << entry.message << ","
            << entry.file << ","
            << entry.line << ","
            << entry.function;
        
        *pImpl->csvOutput << oss.str() << "\n";
    }
    
    if (pImpl->humanReadableOutput) {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "]";
        
        if (pImpl->includeTimestamp) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << " [" << std::put_time(std::localtime(&time), "%H:%M:%S") << "]";
        }
        
        if (pImpl->includeContext) {
            if (!context.correlationId.empty()) {
                oss << " [Corr:" << context.correlationId << "]";
            }
            if (!context.componentId.empty()) {
                oss << " [" << context.componentId << "]";
            }
        }
        
        oss << " " << entry.message;
        
        *pImpl->humanReadableOutput << oss.str() << "\n";
    }
}

void StructuredLogOutput::logPerformanceMetrics(const PerformanceMetrics& metrics, 
                                               const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->jsonOutput) {
        json j;
        j["metrics_type"] = "performance";
        j["simulation_time"] = metrics.simulationTime;
        j["step_time"] = metrics.stepTime;
        j["memory_usage"] = metrics.memoryUsage;
        j["cpu_usage"] = metrics.cpuUsage;
        j["average_firing_rate"] = metrics.averageFiringRate;
        j["total_spikes"] = metrics.totalSpikes;
        
        if (pImpl->includeContext) {
            j["step_number"] = context.stepNumber;
            j["simulation_time"] = context.simulationTime;
        }
        
        *pImpl->jsonOutput << j.dump(pImpl->indentation) << "\n";
    }
    
    if (pImpl->csvOutput) {
        std::ostringstream oss;
        oss << "performance" << ","
            << (pImpl->includeContext ? std::to_string(context.stepNumber) : "") << ","
            << metrics.simulationTime << ","
            << metrics.stepTime << ","
            << metrics.memoryUsage << ","
            << metrics.cpuUsage << ","
            << metrics.averageFiringRate << ","
            << metrics.totalSpikes;
        
        *pImpl->csvOutput << oss.str() << "\n";
    }
    
    if (pImpl->humanReadableOutput) {
        std::ostringstream oss;
        oss << "Performance Metrics: Step " << context.stepNumber << 
            " (Time: " << metrics.simulationTime << "s, "
            << "Mem: " << metrics.memoryUsage << "MB, "
            << "CPU: " << metrics.cpuUsage << "%)";
        
        *pImpl->humanReadableOutput << oss.str() << "\n";
    }
}

void StructuredLogOutput::logNeuralMetrics(const NeuralPerformanceTracker::NeuralMetrics& metrics,
                                           const LoggingContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->jsonOutput) {
        json j;
        j["metrics_type"] = "neural";
        j["average_firing_rate"] = metrics.averageFiringRate;
        j["peak_firing_rate"] = metrics.peakFiringRate;
        j["firing_rate_variance"] = metrics.meanFiringRateVariance;
        j["average_synaptic_weight"] = metrics.averageSynapticWeight;
        j["total_spikes"] = metrics.totalSpikes;
        
        if (pImpl->includeContext) {
            j["step_number"] = context.stepNumber;
        }
        
        *pImpl->jsonOutput << j.dump(pImpl->indentation) << "\n";
    }
    
    if (pImpl->csvOutput) {
        std::ostringstream oss;
        oss << "neural" << ","
            << (pImpl->includeContext ? std::to_string(context.stepNumber) : "") << ","
            << metrics.averageFiringRate << ","
            << metrics.peakFiringRate << ","
            << metrics.meanFiringRateVariance << ","
            << metrics.averageSynapticWeight << ","
            << metrics.totalSpikes;
        
        *pImpl->csvOutput << oss.str() << "\n";
    }
    
    if (pImpl->humanReadableOutput) {
        std::ostringstream oss;
        oss << "Neural Metrics: Avg FR: " << std::fixed << std::setprecision(2) 
            << metrics.averageFiringRate << "Hz, "
            << "Total Spikes: " << metrics.totalSpikes << ", "
            << "Synaptic Weight: " << std::fixed << std::setprecision(6) 
            << metrics.averageSynapticWeight;
        
        *pImpl->humanReadableOutput << oss.str() << "\n";
    }
}

void StructuredLogOutput::setIndentation(int spaces) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->indentation = spaces;
}

void StructuredLogOutput::setIncludeTimestamp(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeTimestamp = include;
}

void StructuredLogOutput::setIncludeContext(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeContext = include;
}

bool StructuredLogOutput::openFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // For now, just implement console output
    return false;
}

void StructuredLogOutput::closeFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Implement file closing
}

// PerformanceBenchmark implementation
struct PerformanceBenchmark::Impl {
    std::mutex mutex;
    
    std::unordered_map<std::string, std::vector<double>> benchmarkResults;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeTimers;
    uint64_t iterationCount = 0;
};

PerformanceBenchmark::PerformanceBenchmark() : pImpl(std::make_unique<Impl>()) {}

void PerformanceBenchmark::start(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->activeTimers[name] = std::chrono::steady_clock::now();
}

double PerformanceBenchmark::stop(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(name);
    if (it != pImpl->activeTimers.end()) {
        auto duration = std::chrono::duration<double>(std::chrono::steady_clock::now() - it->second).count();
        
        pImpl->benchmarkResults[name].push_back(duration);
        pImpl->iterationCount++;
        
        pImpl->activeTimers.erase(it);
        return duration;
    }
    return 0.0;
}

double PerformanceBenchmark::getDuration(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->activeTimers.find(name);
    if (it != pImpl->activeTimers.end()) {
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - it->second).count();
    }
    return 0.0;
}

void PerformanceBenchmark::runBenchmark(const std::string& name, 
                                        std::function<void()> benchmarkFunction) {
    start(name);
    benchmarkFunction();
    stop(name);
}

std::string PerformanceBenchmark::generateBenchmarkReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Benchmark Report ===\n";
    oss << "Total Benchmarks: " << pImpl->benchmarkResults.size() << "\n";
    oss << "Total Iterations: " << pImpl->iterationCount << "\n";
    
    for (const auto& pair : pImpl->benchmarkResults) {
        const auto& times = pair.second;
        if (times.empty()) continue;
        
        double sum = std::accumulate(times.begin(), times.end(), 0.0);
        double avg = sum / times.size();
        double min = *std::min_element(times.begin(), times.end());
        double max = *std::max_element(times.begin(), times.end());
        
        oss << "\nBenchmark: " << pair.first << "\n";
        oss << "  Iterations: " << times.size() << "\n";
        oss << "  Average: " << std::fixed << std::setprecision(6) << avg << "s\n";
        oss << "  Minimum: " << std::fixed << std::setprecision(6) << min << "s\n";
        oss << "  Maximum: " << std::fixed << std::setprecision(6) << max << "s\n";
    }
    
    oss << "==========================\n";
    return oss.str();
}

std::string PerformanceBenchmark::exportBenchmarkDataJson() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    json j;
    j["iteration_count"] = pImpl->iterationCount;
    
    json benchmarks = json::object();
    for (const auto& pair : pImpl->benchmarkResults) {
        json times = json::array();
        for (double time : pair.second) {
            times.push_back(time);
        }
        benchmarks[pair.first] = times;
    }
    
    j["benchmarks"] = benchmarks;
    
    return j.dump(2);
}

double PerformanceBenchmark::getAverageTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarkResults.find(name);
    if (it != pImpl->benchmarkResults.end() && !it->second.empty()) {
        double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
        return sum / it->second.size();
    }
    return 0.0;
}

double PerformanceBenchmark::getMinTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarkResults.find(name);
    if (it != pImpl->benchmarkResults.end() && !it->second.empty()) {
        return *std::min_element(it->second.begin(), it->second.end());
    }
    return 0.0;
}

double PerformanceBenchmark::getMaxTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarkResults.find(name);
    if (it != pImpl->benchmarkResults.end() && !it->second.empty()) {
        return *std::max_element(it->second.begin(), it->second.end());
    }
    return 0.0;
}

uint64_t PerformanceBenchmark::getIterationCount(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->benchmarkResults.find(name);
    if (it != pImpl->benchmarkResults.end()) {
        return it->second.size();
    }
    return 0;
}

// Convenience functions for enhanced logging system
void initializeLoggingSystem() {
    auto& logger = nlm::Logger::getGlobal();
    
    // Add performance metrics logger
    auto perfLogger = std::make_shared<PerformanceMetricsLogger>();
    logger.addLogger(perfLogger);
    
    // Add enhanced console logger
    auto consoleLogger = std::make_shared<EnhancedConsoleLogger>();
    logger.addLogger(consoleLogger);
    
    // Set up system monitor
    static SystemMonitor systemMonitor;
    systemMonitor.startMonitoring();
}

void shutdownLoggingSystem() {
    static SystemMonitor systemMonitor;
    systemMonitor.stopMonitoring();
}

} // namespace nlm
