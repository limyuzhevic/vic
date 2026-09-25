#include "experiments/PerformanceProfiler.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/Memory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace nlm {

PerformanceProfiler::PerformanceProfiler(bool enabled, const std::string& outputDir,
                                       const std::string& format, bool consoleOutput)
    : enabled_(enabled), running_(false), outputDir_(outputDir), 
      outputFormat_(format), consoleOutput_(consoleOutput),
      samplingIntervalUs_(1000.0), monitoringActive_(false) {
    if (enabled_) {
        ensureOutputDirectory();
        if (consoleOutput_) {
            std::cout << "[PerformanceProfiler] Initialized with output directory: " << outputDir_ << std::endl;
        }
    }
}

PerformanceProfiler::~PerformanceProfiler() {
    stop();
    for (auto& pair : activeSessions_) {
        delete pair.second;
    }
    activeSessions_.clear();
}

void PerformanceProfiler::start() {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
    monitoringActive_ = true;
    
    // Start system monitoring thread
    monitoringThread_ = std::thread(&PerformanceProfiler::monitoringLoop, this);
    
    if (consoleOutput_) {
        std::cout << "[PerformanceProfiler] Started monitoring" << std::endl;
    }
}

void PerformanceProfiler::stop() {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    monitoringActive_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    calculateStatistics();
    
    if (consoleOutput_) {
        std::cout << "[PerformanceProfiler] Stopped. Total time: " << formatTime(metrics_.totalTimeUs) << std::endl;
    }
}

void PerformanceProfiler::reset() {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_ = ProfilingMetrics();
    recordedMetrics_.clear();
    
    for (auto& pair : activeSessions_) {
        delete pair.second;
        pair.second = nullptr;
    }
    
    if (consoleOutput_) {
        std::cout << "[PerformanceProfiler] Reset profiling data" << std::endl;
    }
}

ProfilingSession* PerformanceProfiler::startSession(const std::string& name, bool enabled) {
    if (!enabled_) return nullptr;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto session = new ProfilingSession(name, enabled);
    activeSessions_[name] = session;
    
    if (consoleOutput_) {
        std::cout << "[PerformanceProfiler] Started session: " << name << std::endl;
    }
    
    return session;
}

void PerformanceProfiler::endSession(const std::string& name) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = activeSessions_.find(name);
    if (it != activeSessions_.end()) {
        it->second->stop();
        if (consoleOutput_) {
            std::cout << "[PerformanceProfiler] Ended session: " << name 
                     << " (time: " << formatTime(it->second->getTotalTime()) << ")" << std::endl;
        }
    }
}

void PerformanceProfiler::recordMetric(const std::string& name, double value) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    recordedMetrics_[name].push_back(value);
}

void PerformanceProfiler::recordMemoryUsage(size_t memoryBytes) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.currentMemoryUsage = memoryBytes;
    if (memoryBytes > metrics_.peakMemoryUsage) {
        metrics_.peakMemoryUsage = memoryBytes;
    }
    metrics_.totalAllocatedMemory += memoryBytes;
}

void PerformanceProfiler::recordStepMetrics(uint64_t step, double timeUs) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.totalSteps = step;
    metrics_.simulationTimeUs += timeUs;
}

void PerformanceProfiler::recordSystemEvent(const std::string& eventType, double timeUs) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.systemTimingsUs[eventType] += timeUs;
}

void PerformanceProfiler::updateSystemMetrics() {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    updateCPUMetrics();
    updateMemoryMetrics();
}

double PerformanceProfiler::getCurrentCPULoad() const {
    if (!enabled_) return 0.0;
    
    double load = 0.0;
    int nhyper, ncpu, ndbl;
    
    // Get CPU load information
    nhyper = getloadavg(&load, 1);
    if (nhyper < 0) return 0.0;
    
    // Convert to percentage (simple approximation)
    return (load / ncpu) * 100.0;
}

size_t PerformanceProfiler::getCurrentMemoryUsage() const {
    if (!enabled_) return 0;
    
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    return memInfo.totalram * sizeof(unsigned long) - memInfo.freeram * sizeof(unsigned long);
}

void PerformanceProfiler::monitoringLoop() {
    while (monitoringActive_ && running_) {
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long>(samplingIntervalUs_)));
        updateSystemMetrics();
    }
}

void PerformanceProfiler::updateCPUMetrics() {
    metrics_.cpuTimeUs += getCurrentCPULoad() * 1000.0; // Rough approximation
}

void PerformanceProfiler::updateMemoryMetrics() {
    metrics_.currentMemoryUsage = getCurrentMemoryUsage();
    if (metrics_.currentMemoryUsage > metrics_.peakMemoryUsage) {
        metrics_.peakMemoryUsage = metrics_.currentMemoryUsage;
    }
}

void PerformanceProfiler::calculateStatistics() {
    if (!enabled_) return;
    
    // Calculate derived metrics
    if (metrics_.totalTimeUs > 0) {
        metrics_.simulationSpeed = (metrics_.totalSteps / metrics_.totalTimeUs) * 1000000.0; // steps per second
    }
    
    if (metrics_.totalSteps > 0) {
        metrics_.visualizationTimeUs = metrics_.totalTimeUs * 0.2; // Rough estimate
        metrics_.exportTimeUs = metrics_.totalTimeUs * 0.1; // Rough estimate
        metrics_.memoryTimeUs = metrics_.totalTimeUs * 0.3; // Rough estimate
    }
    
    if (metrics_.totalSteps > 0) {
        metrics_.memoryPerStep = static_cast<double>(metrics_.totalAllocatedMemory) / metrics_.totalSteps;
    }
    
    if (metrics_.totalTimeUs > 0) {
        metrics_.cpuUsagePercent = (metrics_.cpuTimeUs / metrics_.totalTimeUs) * 100.0;
    }
}

void PerformanceProfiler::ensureOutputDirectory() const {
    if (outputDir_.empty()) return;
    
    struct stat info;
    if (stat(outputDir_.c_str(), &info) != 0) {
        mkdir(outputDir_.c_str(), 0755);
    }
}

double PerformanceProfiler::getCurrentTimestamp() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double, std::micro>(duration).count();
}

std::string PerformanceProfiler::getTimestampString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void PerformanceProfiler::exportToText(const std::string& filename) const {
    if (!enabled_) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        if (consoleOutput_) {
            std::cerr << "[PerformanceProfiler] Failed to open file: " << filename << std::endl;
        }
        return;
    }
    
    file << "Performance Profiling Results" << std::endl;
    file << "================================" << std::endl;
    file << "Generated: " << getTimestampString() << std::endl;
    file << "Total Time: " << formatTime(metrics_.totalTimeUs) << std::endl;
    file << "Total Steps: " << metrics_.totalSteps << std::endl;
    file << std::endl;
    
    // Timing breakdown
    file << "Timing Breakdown (microseconds):" << std::endl;
    file << "  Total: " << formatTime(metrics_.totalTimeUs) << std::endl;
    file << "  CPU: " << formatTime(metrics_.cpuTimeUs) << std::endl;
    file << "  Memory: " << formatTime(metrics_.memoryTimeUs) << std::endl;
    file << "  Simulation: " << formatTime(metrics_.simulationTimeUs) << std::endl;
    file << "  Visualization: " << formatTime(metrics_.visualizationTimeUs) << std::endl;
    file << "  Export: " << formatTime(metrics_.exportTimeUs) << std::endl;
    file << std::endl;
    
    // Performance metrics
    file << "Performance Metrics:" << std::endl;
    file << "  Simulation Speed: " << metrics_.simulationSpeed << " steps/second" << std::endl;
    file << "  FPS: " << metrics_.fps << std::endl;
    file << "  Memory per Step: " << formatBytes(static_cast<size_t>(metrics_.memoryPerStep)) << std::endl;
    file << "  CPU Usage: " << formatPercentage(metrics_.cpuUsagePercent) << std::endl;
    file << std::endl;
    
    // Memory usage
    file << "Memory Usage:" << std::endl;
    file << "  Peak: " << formatBytes(metrics_.peakMemoryUsage) << std::endl;
    file << "  Current: " << formatBytes(metrics_.currentMemoryUsage) << std::endl;
    file << "  Total Allocated: " << formatBytes(metrics_.totalAllocatedMemory) << std::endl;
    file << std::endl;
    
    // Event counts
    file << "Event Counts:" << std::endl;
    file << "  Spike Events: " << metrics_.spikeEvents << std::endl;
    file << "  Plasticity Events: " << metrics_.plasticityEvents << std::endl;
    file << "  Development Events: " << metrics_.developmentEvents << std::endl;
    file << "  Prediction Events: " << metrics_.predictionEvents << std::endl;
    file << "  Memory Events: " << metrics_.memoryEvents << std::endl;
    file << std::endl;
    
    // System timings
    if (!metrics_.systemTimingsUs.empty()) {
        file << "System Timings (microseconds):" << std::endl;
        for (const auto& pair : metrics_.systemTimingsUs) {
            file << "  " << pair.first << ": " << formatTime(pair.second) << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
}

void PerformanceProfiler::exportToJSON(const std::string& filename) const {
    if (!enabled_) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        if (consoleOutput_) {
            std::cerr << "[PerformanceProfiler] Failed to open file: " << filename << std::endl;
        }
        return;
    }
    
    file << "{
    \"timestamp\": \"" << getTimestampString() << "\",
    \"total_time_us\": " << metrics_.totalTimeUs << ",
    \"total_steps\": " << metrics_.totalSteps << ",
    \"simulation_speed\": " << metrics_.simulationSpeed << ",
    \"fps\": " << metrics_.fps << ",
    \"memory_per_step_bytes\": " << metrics_.memoryPerStep << ",
    \"cpu_usage_percent\": " << metrics_.cpuUsagePercent << ",
    \"peak_memory_usage\": " << metrics_.peakMemoryUsage << ",
    \"current_memory_usage\": " << metrics_.currentMemoryUsage << ",
    \"total_allocated_memory\": " << metrics_.totalAllocatedMemory << ",
    \"spike_events\": " << metrics_.spikeEvents << ",
    \"plasticity_events\": " << metrics_.plasticityEvents << ",
    \"development_events\": " << metrics_.developmentEvents << ",
    \"prediction_events\": " << metrics_.predictionEvents << ",
    \"memory_events\": " << metrics_.memoryEvents";
    
    if (!metrics_.systemTimingsUs.empty()) {
        file << ",\n  \"system_timings\": {";
        bool first = true;
        for (const auto& pair : metrics_.systemTimingsUs) {
            if (!first) file << ",\n   ";
            file << "\"" << pair.first << "\": " << pair.second;
            first = false;
        }
        file << "}";
    }
    
    file << "\n}";
    file.close();
}

void PerformanceProfiler::exportToCSV(const std::string& filename) const {
    if (!enabled_) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        if (consoleOutput_) {
            std::cerr << "[PerformanceProfiler] Failed to open file: " << filename << std::endl;
        }
        return;
    }
    
    file << "Metric,Value,Unit\n";
    file << "Total Time," << metrics_.totalTimeUs << "us\n";
    file << "Total Steps," << metrics_.totalSteps << "steps\n";
    file << "Simulation Speed," << metrics_.simulationSpeed << "steps/s\n";
    file << "FPS," << metrics_.fps << "Hz\n";
    file << "Memory per Step," << metrics_.memoryPerStep << "bytes/step\n";
    file << "CPU Usage," << metrics_.cpuUsagePercent << "%\n";
    file << "Peak Memory Usage," << metrics_.peakMemoryUsage << "bytes\n";
    file << "Current Memory Usage," << metrics_.currentMemoryUsage << "bytes\n";
    file << "Total Allocated Memory," << metrics_.totalAllocatedMemory << "bytes\n";
    file << "Spike Events," << metrics_.spikeEvents << "events\n";
    file << "Plasticity Events," << metrics_.plasticityEvents << "events\n";
    file << "Development Events," << metrics_.developmentEvents << "events\n";
    file << "Prediction Events," << metrics_.predictionEvents << "events\n";
    file << "Memory Events," << metrics_.memoryEvents << "events\n";
    
    file.close();
}

void PerformanceProfiler::printSummary() const {
    if (!enabled_ || !consoleOutput_) return;
    
    std::cout << "\n=== Performance Summary ===" << std::endl;
    std::cout << "Total Time: " << formatTime(metrics_.totalTimeUs) << std::endl;
    std::cout << "Total Steps: " << metrics_.totalSteps << std::endl;
    std::cout << "Simulation Speed: " << metrics_.simulationSpeed << " steps/s" << std::endl;
    std::cout << "FPS: " << metrics_.fps << std::endl;
    std::cout << "Memory per Step: " << formatBytes(static_cast<size_t>(metrics_.memoryPerStep)) << std::endl;
    std::cout << "CPU Usage: " << formatPercentage(metrics_.cpuUsagePercent) << std::endl;
    std::cout << "Peak Memory: " << formatBytes(metrics_.peakMemoryUsage) << std::endl;
    std::cout << "Spike Events: " << metrics_.spikeEvents << std::endl;
    std::cout << "Plasticity Events: " << metrics_.plasticityEvents << std::endl;\n    std::cout << "Development Events: " << metrics_.developmentEvents << std::endl;
    std::cout << "Prediction Events: " << metrics_.predictionEvents << std::endl;
    std::cout << "Memory Events: " << metrics_.memoryEvents << std::endl;
    std::cout << "=======================\n" << std::endl;
}

void PerformanceProfiler::profileBrainSystems(const Brain& brain, uint64_t step) {
    if (!enabled_) return;
    
    double startTime = getCurrentTimestamp();
    
    // Profile each brain system
    if (auto workingMem = brain.getWorkingMemory()) {
        double timeUs = getCurrentTimestamp() - startTime;
        recordSystemEvent("WorkingMemory", timeUs);
        metrics_.memoryEvents++;
    }
    
    if (auto episodicMem = brain.getEpisodicMemory()) {
        double elapsed = getCurrentTimestamp() - startTime;
        recordSystemEvent("EpisodicMemory", elapsed);
        metrics_.memoryEvents++;
    }
    
    if (auto assocMem = brain.getAssociativeMemory()) {
        double elapsed = getCurrentTimestamp() - startTime;
        recordSystemEvent("AssociativeMemory", elapsed);
        metrics_.memoryEvents++;
    }
    
    if (auto prediction = brain.getPredictionSystem()) {
        double elapsed = getCurrentTimestamp() - startTime;
        recordSystemEvent("PredictionSystem", elapsed);
        metrics_.predictionEvents++;
    }
    
    if (auto planner = brain.getPlanner()) {
        double elapsed = getCurrentTimestamp() - startTime;
        recordSystemEvent("NeuralPlanner", elapsed);
        metrics_.memoryEvents++;
    }
    
    if (auto development = brain.getDevelopmentSystem()) {
        double elapsed = getCurrentTimestamp() - startTime;
        recordSystemEvent("DevelopmentSystem", elapsed);
        metrics_.developmentEvents++;
    }
}

void PerformanceProfiler::profileNeuromodulation(const Neuromodulator& neuromodulator, double timeUs) {
    if (!enabled_) return;
    
    double startTime = getCurrentTimestamp();
    recordSystemEvent("Neuromodulation", timeUs);
    metrics_.spikeEvents++;
}

void PerformanceProfiler::profileMemorySystems(const NeuralWorkingMemory& workingMem,
                                               const NeuralEpisodicMemory& episodicMem,
                                               double timeUs) {
    if (!enabled_) return;
    
    recordSystemEvent("MemorySystems", timeUs);
    metrics_.memoryEvents++;
}

void PerformanceProfiler::profilePrediction(const PredictionSystem& prediction, double timeUs) {
    if (!enabled_) return;
    
    recordSystemEvent("Prediction", timeUs);
    metrics_.predictionEvents++;
}

void PerformanceProfiler::profileCognition(const NeuralPlanner& planner, double timeUs) {
    if (!enabled_) return;
    
    recordSystemEvent("Cognition", timeUs);
    metrics_.memoryEvents++;
}

void PerformanceProfiler::profileDevelopment(const DevelopmentSystem& development, double timeUs) {
    if (!enabled_) return;
    
    recordSystemEvent("Development", timeUs);
    metrics_.developmentEvents++;
}

std::string PerformanceProfiler::getCommandLineHelp() {
    return "\n=== Performance Profiler Command Line Options ===\n"
           "  --profile [file]          Enable profiling, specify output directory\n"
           "  --profile-format [text|json|csv] Specify output format (default: text)\n"
           "  --profile-console [0|1]   Enable/disable console output (default: 1)\n"
           "  --profile-interval [ms]    Sampling interval in milliseconds (default: 1)\n"
           "  --profile-memory           Enable memory profiling (default: enabled)\n"
           "  --profile-cpu             Enable CPU profiling (default: enabled)\n"
           "  --profile-system          Enable system-wide profiling (default: enabled)\n"";
}

bool PerformanceProfiler::parseCommandLine(int argc, char* argv[], PerformanceProfiler& profiler) {
    bool enabled = false;
    std::string outputDir = ".";
    std::string format = "text";
    bool consoleOutput = true;
    double interval = 1.0;
    bool memoryProfiling = true;
    bool cpuProfiling = true;
    bool systemProfiling = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--profile") {
            enabled = true;
            if (i + 1 < argc) {
                outputDir = argv[++i];
            }
        } else if (arg == "--profile-format") {
            if (i + 1 < argc) {
                format = argv[++i];
            }
        } else if (arg == "--profile-console") {
            if (i + 1 < argc) {
                consoleOutput = (argv[++i][0] == '1');
            }
        } else if (arg == "--profile-interval") {
            if (i + 1 < argc) {
                interval = std::stod(argv[++i]) * 1000.0; // Convert ms to microseconds
            }
        } else if (arg == "--profile-memory") {
            // Already enabled by default
        } else if (arg == "--profile-cpu") {
            // Already enabled by default
        } else if (arg == "--profile-system") {
            // Already enabled by default
        } else if (arg == "--help") {
            std::cout << getCommandLineHelp() << std::endl;
            return false;
        }
    }
    
    profiler.setEnabled(enabled);
    profiler.setOutputDirectory(outputDir);
    profiler.setOutputFormat(format);
    profiler.setConsoleOutput(consoleOutput);
    profiler.setSamplingInterval(interval);
    
    return true;
}

// PerformanceUtils implementations
double PerformanceUtils::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (double v : values) sum += v;
    return sum / values.size();
}

double PerformanceUtils::calculateStandardDeviation(const std::vector<double>& values) {
    if (values.size() <= 1) return 0.0;
    double mean = calculateMean(values);
    double sumSq = 0.0;
    for (double v : values) {
        sumSq += (v - mean) * (v - mean);
    }
    return std::sqrt(sumSq / (values.size() - 1));
}

double PerformanceUtils::calculateMin(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return *std::min_element(values.begin(), values.end());
}

double PerformanceUtils::calculateMax(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return *std::max_element(values.begin(), values.end());
}

double PerformanceUtils::calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double PerformanceUtils::calculateFPS(double timePerStepUs, uint64_t steps) {
    if (timePerStepUs <= 0.0) return 0.0;
    return (steps * 1000000.0) / timePerStepUs;
}

double PerformanceUtils::calculateMemoryPerStep(size_t totalMemory, uint64_t steps) {
    if (steps == 0) return 0.0;
    return static_cast<double>(totalMemory) / steps;
}

double PerformanceUtils::calculateCPUUsage(double cpuTimeUs, double totalTimeUs) {
    if (totalTimeUs <= 0.0) return 0.0;
    return (cpuTimeUs / totalTimeUs) * 100.0;
}

double PerformanceUtils::calculateSimulationSpeed(double totalSteps, double totalTimeUs) {
    if (totalTimeUs <= 0.0) return 0.0;
    return (totalSteps / totalTimeUs) * 1000000.0;
}

std::string PerformanceUtils::formatTime(double timeUs) {
    std::stringstream ss;
    if (timeUs >= 1000.0) {
        ss << std::fixed << std::setprecision(2) << (timeUs / 1000.0) << "ms";
    } else if (timeUs >= 1000000.0) {
        ss << std::fixed << std::setprecision(2) << (timeUs / 1000000.0) << "s";
    } else {
        ss << std::fixed << std::setprecision(2) << timeUs << "us";
    }
    return ss.str();
}

std::string PerformanceUtils::formatBytes(size_t bytes) {
    if (bytes >= 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
    } else if (bytes >= 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + "MB";
    } else if (bytes >= 1024) {
        return std::to_string(bytes / 1024) + "KB";
    } else {
        return std::to_string(bytes) + "B";
    }
}

std::string PerformanceUtils::formatPercentage(double value) {
    return std::to_string(value) + "%";
}

std::string PerformanceUtils::getSystemInfo() {
    std::stringstream ss;
    
    // Get system information
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    
    ss << "System Information:" << std::endl;
    ss << "  Memory: " << formatBytes(memInfo.totalram * sizeof(unsigned long)) << std::endl;
    ss << "  Free Memory: " << formatBytes(memInfo.freeram * sizeof(unsigned long)) << std::endl;
    
    return ss.str();
}

std::string PerformanceUtils::getPerformanceSummary(const ProfilingMetrics& metrics) {
    std::stringstream ss;
    
    ss << "=== Performance Summary ===" << std::endl;
    ss << "Total Time: " << formatTime(metrics.totalTimeUs) << std::endl;
    ss << "Steps: " << metrics.totalSteps << std::endl;
    ss << "Speed: " << calculateSimulationSpeed(metrics.totalSteps, metrics.totalTimeUs) << " steps/s" << std::endl;
    ss << "Memory per Step: " << formatBytes(static_cast<size_t>(metrics.memoryPerStep)) << std::endl;
    ss << "CPU Usage: " << formatPercentage(metrics.cpuUsagePercent) << std::endl;
    ss << "Peak Memory: " << formatBytes(metrics.peakMemoryUsage) << std::endl;
    ss << "=======================\n";
    
    return ss.str();
}

// Phase6ProfilingIntegration implementations
void Phase6ProfilingIntegration::setupProfiling(PerformanceProfiler& profiler, int argc, char* argv[]) {
    if (PerformanceProfiler::parseCommandLine(argc, argv, profiler)) {
        profiler.start();
        std::cout << "[Phase6Profiling] Performance profiler started" << std::endl;
    }
}

void Phase6ProfilingIntegration::startProfilingCycle(PerformanceProfiler& profiler) {
    profiler.start();
}

void Phase6ProfilingIntegration::recordStepMetrics(PerformanceProfiler& profiler, uint64_t step,
                                                  const Brain& brain, double stepTimeUs) {
    profiler.recordStepMetrics(step, stepTimeUs);
    profiler.profileBrainSystems(brain, step);
}

void Phase6ProfilingIntegration::endProfilingCycle(PerformanceProfiler& profiler) {
    profiler.stop();
    profiler.printSummary();
}

void Phase6ProfilingIntegration::exportProfilingResults(PerformanceProfiler& profiler, 
                                                      const std::string& baseFilename) {
    if (!profiler.isRunning()) {
        std::string textFile = baseFilename + ".txt";
        std::string jsonFile = baseFilename + ".json";
        std::string csvFile = baseFilename + ".csv";
        
        profiler.exportToText(textFile);
        profiler.exportToJSON(jsonFile);
        profiler.exportToCSV(csvFile);
        
        std::cout << "[Phase6Profiling] Profiling results exported:" << std::endl;
        std::cout << "  Text: " << textFile << std::endl;
        std::cout << "  JSON: " << jsonFile << std::endl;
        std::cout << "  CSV: " << csvFile << std::endl;
    }
}

} // namespace nlm