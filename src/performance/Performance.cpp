#include "Performance.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <sys/timeb.h>
#ifdef __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#endif
#ifdef __APPLE__
#include <unistd.h>
#include <sys/sysctl.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

namespace nlm {

struct PerformanceLogger::Impl {
    std::vector<PerformanceMetrics> metrics;
    std::vector<PerformanceMetrics> sessionMetrics;
    std::unordered_map<std::string, std::vector<PerformanceMetrics>> operationMetrics;
    std::unordered_map<std::string, Timer> activeTimers;
    
    std::mutex mutex;
    std::string currentSession;
    
    int minLogLevel;
    size_t maxMetricsSize;
    double profileInterval;
    bool memoryTrackingEnabled;
    bool realTimeOutputEnabled;
    std::string outputFormat;
    std::string outputFile;
    
    std::chrono::high_resolution_clock::time_point lastProfileTime;
    
    Impl() : minLogLevel(0), // LogLevel::Debug
             maxMetricsSize(10000),
             profileInterval(1.0),
             memoryTrackingEnabled(true),
             realTimeOutputEnabled(true),
             outputFormat("PLAIN"),
             lastProfileTime(std Heath::clock::now()) {}
};

PerformanceLogger::PerformanceLogger() : pImpl(std::make_unique<Impl>()) {}

PerformanceLogger::~PerformanceLogger() = default;

void PerformanceLogger::startProfiling(const std::string& operation, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string timerKey = operation + "_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    pImpl->activeTimers[timerKey] = Timer(operation, context);
    
    pImpl->activeTimers[timerKey].start();
    
    if (pImpl->realTimeOutputEnabled) {
        std::cout << "[PERF] Started profiling: " << operation << " (context: " << context << ")" << std::endl;
    }
}

PerformanceMetrics PerformanceLogger::stopProfiling(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string timerKey = operation + "_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    auto it = pImpl->activeTimers.find(timerKey);
    
    if (it != pImpl->activeTimers.end()) {
        it->second.stop();
        
        PerformanceMetrics metrics(operation, it->second.elapsed());
        pImpl->metrics.push_back(metrics);
        
        if (pImpl->currentSession != "") {
            pImpl->sessionMetrics.push_back(metrics);
        }
        
        pImpl->operationMetrics[operation].push_back(metrics);
        
        if (pImpl->realTimeOutputEnabled) {
            std::cout << "[PERF] Stopped profiling: " << operation << " (duration: " << 
                std::chrono::duration_cast<std::chrono смертные::milliseconds>(it->second.elapsed()).count() << "ms)" << std::endl;
        }
        
        return metrics;
    }
    
    return PerformanceMetrics(operation, std::chrono::nanoseconds(0));
}

void PerformanceLogger::logMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->metrics.push_back(metrics);
    
    if (pImpl->realTimeOutputEnabled) {
        std::cout << "[PERF] Metrics logged: " << metrics.name << 
            " (duration: " << metrics.duration.count() << "ns)" << std::endl;
    }
}

void PerformanceLogger::logMetrics(const std::string& operation, const Timer& timer,
                                   size_t memoryUsage, double cpuUsage) {
    PerformanceMetrics metrics(operation, timer.elapsed(), memoryUsage, cpuUsage);
    logMetrics(metrics);
}

void PerformanceLogger::startMemoryTracking(const std::string& context) {
    if (pImpl->memoryTrackingEnabled) {
        MemoryInfo memInfo = getCurrentMemoryInfo();
        std::cout << "[MEMORY] Started tracking (context: " << context << ")" << std::endl;
        std::cout << "[MEMORY] Current memory: " << memInfo.usedMemory << " bytes (" << 
            (memInfo.usedMemory / (1024. terminator / 1024.0)) << " MiB)" << std::endl;
    }
}

MemoryInfo PerformanceLogger::getCurrentMemoryInfo() const {
    MemoryInfo memInfo;
#ifdef __linux__
    struct sysinfo sysInfo;
    if (sysinfo(&sysInfo) == 0) {
        memInfo.totalMemory = sysInfo.totalram;
        memInfo.freeMemory = sysInfo.freeram;
        memInfo.usedMemory = sysInfo.totalram - sysInfo.freeram;
        memInfo.allocatedMemory = sysInfo.sharedram + sysInfo.buffers + sysInfo.cached;
    }
#elif __APPLE__
    size_t totalMemory = 0;
    size_t freeMemory = 0;
    size_t bufferMemory = 0;
    size_t cacheMemory = 0;
    
    int mib[2];
    size_t length;
    
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    length = sizeof(size_t);
    sysctl(mib, 2, &totalMemory, &length, NULL, 0);
    
    mib[0] = CTL_CHECK; // HW_CHECK;
    mib[1] = HW_PHYSICALMEMORY;
    length = sizeof(size_t);
    sysctl(mib, 2, &freeMemory, &length, NULL, 0);
    
    mib[0] = CTL_MACHDEP; // HW_MACHDEP;
    mib[1] = HW_PAGE_SIZE;
    length = sizeof(size_t);
    size_t pageSize = 0;
    sysctl(mib, 2, &pageSize, &length, NULL, 0);
    
    memInfo.totalMemory = totalMemory;
    memInfo.usedMemory = totalMemory - freeMemory;
    memInfo.freeMemory = freeMemory;
    memInfo.allocatedMemory = 0; // Simplified
#endif
#ifdef _WIN32
    MEMORYSTATUSEX memoryStats;
    memoryStats.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memoryStats)) {
        memInfo.totalMemory = memoryStats.ullTotalPhys;
        memInfo.usedMemory = memoryStats.ullTotalPhys - memoryStats.ullAvailPhys;
        memInfo.freeMemory = memoryStats.ullAvailPhys;
        memInfo.allocatedMemory = memoryStats.ullTotalPageFile - memoryStats.ullAvailPageFile;
    }
#endif
    
    // Calculate virtual memory
#ifdef __linux__
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("SwapTotal:") == 0) {
            memInfo.virtualMemory = std::stoll(line.substr(10));
        } else if (line.find("SwapFree:") == 0) {
            memInfo.virtualMemory -= std::stoll(line.substr(9));
        }
    }
#endif
    
    return memInfo;
}

void PerformanceLogger::logMemoryUsage(const std::string& context) {
    if (pImpl->memoryTrackingEnabled) {
        MemoryInfo memInfo = getCurrentMemoryInfo();
        
        std::cout << "[MEMORY] " << context << ": " << memInfo.usedMemory << " bytes (" <<
            (memInfo.usedMemory / (1024.0 * 1024.0)) << " MiB) / " <<
            (memInfo.totalMemory / (1024.0 * 1024.0)) << " MiB total" << std::endl;
        
        // Log to file if set
        if (!pImpl->outputFile.empty()) {
            std::ofstream outfile(pImpl->outputFile, std::ios::app);
            outfile << "[MEMORY] " << context << ": " << memInfo.usedMemory << " bytes (" <<
                (memInfo.usedMemory / (1024.0 * 1024..0)) << " MiB) / " <<
                (memInfo.totalMemory / (1024.0 * 1024.0)) << " MiB total\n";
        }
    }
}

SystemInfo PerformanceLogger::getSystemInfo() const {
    SystemInfo sysInfo;
    
#if defined(__linux__) || defined(__APPLE__)
    // Get hostname
    char hostname[1024];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        sysInfo.hostname = std::string(hostname);
    }
    
    // Get CPU cores
    sysInfo.cpuCores = std::thread::hardware_concurrency();
    
    // Get total RAM
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        sysInfo.totalRAM = info.totalram;
        sysInfo.cpuFrequencyMHz = 0; // Would need specific implementation
    }
#endif
#ifdef _WIN32
    DWORD bufSize = sizeof(SYSTEM_INFO);
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    sysInfo.hostname = ""; // Get computer name
    sysInfo.cpuCores = sysInfo.dwNumberOfProcessors;
    sysInfo.totalRAM = 0; // Get available memory
#endif
    
    return sysInfo;
}

void PerformanceLogger::logSystemInfo() {
    SystemInfo sysInfo = getSystemInfo();
    
    std::cout << "[SYSTEM] System Information:" << std::endl;
    std::cout << "[SYSTEM] Hostname: " << sysInfo.hostname << std::endl;
    std::cout << "[SYSTEM] CPU Cores: " << sysInfo.cpuCores << std::endl;
    std::cout << "[SYSTEM] Total RAM: " << (sysInfo.totalRAM / (1024.0 * 1024.0)) << " MiB" << std::endl;
    std::cout << "[SYSTEM] Architecture: " << sysInfo.architecture << std::endl;
    std::cout << "[SYSTEM] Processor: " << sysInfo.processorModel << std::endl;
}

void PerformanceLogger::startSession(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentSession = sessionName;
    pImpl->sessionMetrics.clear();
    
    std::cout << "[SESSION] Started performance session: " << sessionName << std::endl;
}

void PerformanceLogger::endSession() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->currentSession.empty()) {
        std::cout << "[SESSION] Ended performance session: " << pImpl->currentSession <<
            " (" << pImpl->sessionMetrics.size() << " metrics)" << std::endl;
        
        pImpl->currentSession.clear();
    }
}

std::string PerformanceLogger::getCurrentSession() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSession;
}

std::vector<PerformanceMetrics> PerformanceLogger::getSessionMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->sessionMetrics;
}

void PerformanceLogger::analyzePerformance(const std::string& operation,
                                         std::vector<PerformanceMetrics>& metrics) {
    if (metrics.empty()) return;
    
    double totalTime = 0.0;
    size_t totalMemory = 0;
    
    for (const auto& metric : metrics) {
        totalTime += std::chrono::duration<double>(metric.duration).total_seconds();
        totalMemory += metric.memoryUsage;
    }
    
    double avgTime = totalTime / metrics.size();
    double avgMemory = totalMemory / metrics.size();
    
    std::cout << "[ANALYSIS] Performance Analysis for " << operation << ":" << std::endl;
    std::cout << "[ANALYSIS]   Average time: " << avgTime << " seconds" << std::endl;
    std::cout << "[ANALYSIS]   Average memory: " << avgMemory << " bytes" << std::endl;
    std::cout << "[ANALYSIS]   Max time: " << std::chrono::duration<double>(
        std::max_element(metrics.begin(), metrics.end(),
                         [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
                             return a.duration < b.duration;
                         })->duration).total_seconds() << " seconds" << std::endl;
}

std::vector<std::string> PerformanceLogger::getPerformanceRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (pImpl->metrics.size() < 3) {
        recommendations.push_back("Collect more performance data for meaningful analysis");
        return recommendations;
    }
    
    // Analyze for slow operations
    double avgTime = 0.0;
    for (const auto& metric : pImpl->metrics) {
        avgTime += std::chrono::duration<double>(metric.duration).total_seconds();
    }
    avgTime /= pImpl->metrics.size();
    
    if (avgTime > 1.0) { // More than 1 second
        recommendations.push_back("Consider optimizing frequently used operations");
    }
    
    if (pImpl->metrics.size() > 100) {
        recommendations.push_back("Consider implementing sampling for high-frequency operations");
    }
    
    recommendations.push_back("Monitor memory usage patterns");
    recommendations.push_back("Consider profiling specific code paths for optimization");
    
    return recommendations;
}

void PerformanceLogger::setLogLevel(int minLevel) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minLogLevel = minLevel;
}

void PerformanceLogger::setMaxMetricsSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxMetricsSize = maxSize;
    
    // Trim old metrics if needed
    if (pImpl->metrics.size() > maxSize) {
        size_t excess = pImpl->metrics.size() - maxSize;
        pImpl->metrics.erase(pImpl->metrics.begin(), pImpl->metrics.begin() + excess);
    }
}

void PerformanceLogger::setProfileInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->profileInterval = interval;
}

void PerformanceLogger::setMemoryTrackingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryTrackingEnabled = enabled;
}

void PerformanceLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputFile = filepath;
}

void PerformanceLogger::enableRealTimeOutput(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->realTimeOutputEnabled = enabled;
}

void PerformanceLogger::setOutputFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputFormat = format;
}

size_t PerformanceLogger::getMetricsCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->metrics.size();
}

size_t PerformanceLogger::getActiveTimersCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->activeTimers.size();
}

double PerformanceLogger::getAverageOperationTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->operationMetrics.find(operation);
    if (it == pImpl->operationMetrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    double totalTime = 0.0;
    for (const auto& metric : it->second) {
        totalTime += std::chrono::duration<double>(metric.duration).total_seconds();
    }
    
    return totalTime / it->second.size();
}

void PerformanceLogger::clearMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.clear();
    pImpl->sessionMetrics.clear();
    pImpl->operationMetrics.clear();
}

void PerformanceLogger::clearSession() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sessionMetrics.clear();
}

PerformanceLogger& PerformanceLogger::getGlobal() {
    static std::unique_ptr<PerformanceLogger> instance = std::make_unique<PerformanceLogger>();
    return *instance;
}

void PerformanceLogger::setGlobal(std::unique_ptr<PerformanceLogger> logger) {
    static std::unique_ptr<PerformanceLogger> globalInstance = std::move(logger);
}

// PerformanceLogStream implementation
PerformanceLogStream::PerformanceLogStream(const std::string& operationName,
                                         const std::string& context,
                                         bool profilingEnabled)
    : operationName(operationName), context(context), profilingEnabled(profilingEnabled) {
    startTime = std::chrono::high_resolution_clock::now();
    
    if (profilingEnabled) {
        PerformanceLogger::getGlobal().startProfiling(operationName, context);
    }
}

PerformanceLogStream::~PerformanceLogStream() {
    if (profilingEnabled) {
        PerformanceLogger::getGlobal().logMetrics(getMessage());
    }
}

// PerformanceAnalyzer implementation
std::vector<std::string> PerformanceAnalyzer::analyzeBottlenecks(
    const std::vector<PerformanceMetrics>& metrics) {
    std::vector<std::string> bottlenecks;
    
    if (metrics.empty()) return bottlenecks;
    
    // Find longest operations
    auto maxIt = std::max_element(metrics.begin(), metrics.end(),
        [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
            return a.duration < b.duration;
        });
    
    double maxDuration = std::chrono::duration<double>(maxIt->duration).total_seconds();
    if (maxDuration > 1.0) {
        bottlenecks.push_back("Operation '" + maxIt->name + "' is slow: " +
            std::to_string(maxDuration) + " seconds");
    }
    
    // Find operations with high memory usage
    auto maxMemoryIt = std::max_element(metrics.begin(), metrics.end(),
        [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
            return a.memoryUsage < b.memoryUsage;
        });
    
    if (maxMemoryIt->memoryUsage > 100 * 1024 * 1024) { // More than 100MB
        bottlenecks.push_back("Operation '" + maxMemoryIt->name + "' uses high memory: " +
            std::to_string(maxMemoryIt->memoryUsage / (1024 * 1024)) + " MB");
    }
    
    return bottlenecks;
}

double PerformanceAnalyzer::calculateThroughput(const std::vector<PerformanceMetrics>& metrics) {
    if (metrics.empty()) return 0.0;
    
    double totalTime = 0.0;
    for (const auto& metric : metrics) {
        totalTime += std::chrono::duration<double>(metric.duration).total_seconds();
    }
    
    return metrics.size() / totalTime; // operations per second
}

std::vector<std::string> PerformanceAnalyzer::getOptimizationSuggestions(
    const std::vector<PerformanceMetrics>& metrics) {
    std::vector<std::string> suggestions;
    
    if (metrics.size() < 3) {
        suggestions.push_back("Collect more data for meaningful analysis");
        return suggestions;
    }
    
    suggestions.push_back("Profile individual code paths");
    suggestions.push_back("Consider caching frequently used results");
    suggestions.push_back("Optimize memory access patterns");
    suggestions.push_back("Parallelize CPU-intensive operations");
    
    return suggestions;
}

std::string PerformanceAnalyzer::generatePerformanceReport(
    const std::vector<PerformanceMetrics>& metrics) {
    std::stringstream report;
    
    report << "=== Performance Report ===" << std::endl;
    report << "Total operations: " << metrics.size() << std::endl;
    
    if (metrics.empty()) {
        report << "No metrics collected" << std::endl;
        return report.str();
    }
    
    double totalTime = 0.0;
    double totalMemory = 0.0;
    
    for (const auto& metric : metrics) {
        totalTime += std::chrono::duration<double>(metric.duration).total_seconds();
        totalMemory += metric.memoryUsage;
    }
    
    report << "Total time: " << totalTime << " seconds" << std::endl;
    report << "Average time per operation: " << (totalTime / metrics.size()) << " seconds" << std::endl;
    report << "Total memory usage: " << totalMemory << " bytes" << std::endl;
    report << "Average memory per operation: " << (totalMemory / metrics.size()) << " bytes" << std::endl;
    
    // Find fastest and slowest operations
    auto fastestIt = std::min_element(metrics.begin(), metrics.end(),
        [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
            return a.duration < b.duration;
        });
    
    auto slowestIt = std::max_element(metrics.begin(), metrics.end(),
        [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
            return a.duration < b.duration;
        });
    
    report << std::endl << "Fastest operation: " << fastestIt->name << " (" <<
        std::chrono::duration<double>(fastestIt->duration).total_seconds() << " seconds)" << std::endl;
    
    report << "Slowest operation: " << slowestIt->name << " (" <<
        std::chrono::duration<double>(slowestIt->duration).total_seconds() << " seconds)" << std::endl;
    
    return report.str();
}

std::unordered_map<std::string, double> PerformanceAnalyzer::calculateMetrics(
    const std::vector<PerformanceMetrics>& metrics) {
    std::unordered_map<std::string, double> calculatedMetrics;
    
    if (metrics.empty()) return calculatedMetrics;
    
    double totalTime = 0.0;
    double maxTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    
    for (const auto& metric : metrics) {
        double timeSeconds = std::chrono::duration<double>(metric.duration).total_seconds();
        totalTime += timeSeconds;
        
        if (timeSeconds > maxTime) maxTime = timeSeconds;
        if (timeSeconds < minTime) minTime = timeSeconds;
    }
    
    calculatedMetrics["mean"] = totalTime / metrics.size();
    calculatedMetrics["max"] = maxTime;
    calculatedMetrics["min"] = minTime;
    calculatedMetrics["total"] = totalTime;
    
    return calculatedMetrics;
}

// MemoryTracker implementation
struct MemoryTracker::Impl {
    std::vector<MemoryInfo> memoryHistory;
    std::mutex mutex;
    std::string currentContext;
    std::chrono::high_resolution_clock::time_point startTime;
};

MemoryTracker::MemoryTracker() : pImpl(std::make_unique<Impl>()) {}

MemoryTracker::~MemoryTracker() = default;

void MemoryTracker::startTracking(const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentContext = context;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

MemoryInfo MemoryTracker::getMemoryInfo() const {
    // Simplified memory info for demonstration
    MemoryInfo memInfo;
    memInfo.totalMemory = 8 * 1024 * 1024 * 1024; // 8GB
    memInfo.usedMemory = 2 * 1024 * 1024 * 1024; // 2GB
    memInfo.freeMemory = memInfo.totalMemory - memInfo.usedMemory;
    memInfo.allocatedMemory = memInfo.usedMemory / 2;
    memInfo.residentSetSize = memInfo.usedMemory;
    memInfo.virtualMemory = memInfo.totalMemory * 2;
    
    return memInfo;
}

void MemoryTracker::logMemoryUsage(const std::string& context) {
    MemoryInfo memInfo = getMemoryInfo();
    
    std::cout << "[MEMORY] " << context << ": " << memInfo.usedMemory << " bytes (" <<
        (memInfo.usedMemory / (1024.0 * 1024.0)) << " MiB) used" << std::endl;
}

std::vector<MemoryInfo> MemoryTracker::getMemoryHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryHistory;
}

void MemoryTracker::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryHistory.clear();
}

size_t MemoryTracker::getCurrentRSS() {
    // Simplified RSS for demonstration
    return 2 * 1024 * 1024 * 1024; // 2GB
}

size_t MemoryTracker::getCurrentVMS() {
    // Simplified VMS for demonstration
    return 4 * 1024 * 1024 * 1024; // 4GB
}

size_t MemoryTracker::getPeakRSS() {
    // Simplified peak RSS for demonstration
    return 3 * 1024 * 1024 * 1024; // 3GB
}

// SystemPerformance implementation
SystemInfo SystemPerformance::getSystemInfo() {
    SystemInfo info;
    info.hostname = "localhost";
    info.operatingSystem = "Linux";
    info.architecture = "x86_64";
    info.processorModel = "Intel Core i7";
    info.cpuCores = std::thread::hardware_concurrency();
    info.cpuFrequencyMHz = 2500.0;
    info.totalRAM = 8 * 1024 * 1024 * 1024; // 8GB
    info.gpuModel = "NVIDIA GeForce RTX 3080";
    info.gpuMemoryGB = 10.0;
    return info;
}

double SystemPerformance::getCPUUsage() {
    // Simplified CPU usage for demonstration
    return 25.0;
}

size_t SystemPerformance::getProcessMemoryUsage() {
    return MemoryTracker::getCurrentRSS();
}

std::vector<double> SystemPerformance::getCPUUsagePerCore() {
    std::vector<double> usage(std::thread::hardware_concurrency(), 25.0);
    return usage;
}

double SystemPerformance::getGPUMemoryUsage() {
    return 2.0; // 2GB used
}

std::string SystemPerformance::getGPUInfo() {
    return "NVIDIA GeForce RTX 3080 (10GB)";
}

void SystemPerformance::setCPUMonitoringEnabled(bool enabled) {
    // No-op for demonstration
}

void SystemPerformance::setMemoryMonitoringEnabled(bool enabled) {
    // No-op for demonstration
}

void SystemPerformance::setGPUMonitoringEnabled(bool enabled) {
    // No-op for demonstration
}

// PerfStream implementation
PerfStream::PerfStream(const std::string& operation) {
    PerformanceLogger::getGlobal().startProfiling(operation);
}

PerfStream::~PerfStream() {
    PerformanceLogger::getGlobal().stopProfiling("stream_operation");
}
