// PerformanceMonitor implementation for NLM
#include "utils/PerformanceMonitor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <sys/resource.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <sys/stat.h>
#include <cstring>

namespace nlm {

struct PerformanceMonitor::Impl {
    PerformanceMonitorConfig config;
    std::atomic<bool> monitoring{false};
    std::atomic<bool> paused{false};
    PerformanceHistory history;
    std::vector<PerformanceAlert> alerts;
    std::mutex alertsMutex;
    
    // System monitoring state
    struct SysInfo {
        long uptime{0};
        size_t totalRam{0};
        size_t freeRam{0};
        size_t buffers{0};
        size_t cached{0};
        size_t swapTotal{0};
        size_t swapFree{0};
        size_t processes{0};
        
        // Network interface stats
        struct InterfaceStats {
            uint64_t bytesReceived{0};
            uint64_t bytesSent{0};
            uint64_t packetsReceived{0};
            uint64_t packetsSent{0};
            std::string name;
        };
        
        std::vector<InterfaceStats> interfaces;
        
        // Disk stats
        struct DiskStats {
            std::string mountPoint;
            std::string device;
            uint64_t readBytes{0};
            uint64_t writeBytes{0};
            uint64_t readCount{0};
            uint64_t writeCount{0};
            uint64_t readTime{0};
            uint64_t writeTime{0};
        };
        
        std::vector<DiskStats> disks;
        
        // Thread stats
        struct ThreadInfo {
            pid_t pid{0};
            std::string name;
            double cpuUsage{0.0};
            size_t memoryUsage{0};
            bool active{false};
        };
        
        std::vector<ThreadInfo> threads;
        
        // Previous values for rate calculations
        uint64_t prevNetReceived{0};
        uint64_t prevNetSent{0};
        uint64_t prevDiskRead{0};
        uint64_t prevDiskWrite{0};
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    SysInfo sysInfo;
    std::mutex sysInfoMutex;
};

PerformanceMonitor::PerformanceMonitor() : monitoring(false), paused(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->history.startTime = std::chrono::steady_clock::now();
    pImpl->sysInfo.lastUpdate = pImpl->history.startTime;
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept
    : pImpl(std::move(other.pImpl)), config(std::move(other.config)),
      monitoring(other.monitoring.load()), paused(other.paused.load()) {
    other.monitoring.store(false);
    other.paused.store(false);
}

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        config = std::move(other.config);
        monitoring = other.monitoring.load();
        paused = other.paused.load();
        other.monitoring.store(false);
        other.paused.store(false);
    }
    return *this;
}

bool PerformanceMonitor::configure(const PerformanceMonitorConfig& cfg) {
    if (!cfg.validate()) {
        std::cerr << "Error: Invalid performance monitor configuration" << std::endl;
        return false;
    }
    
    pImpl->config = cfg;
    return true;
}

bool PerformanceMonitor::startMonitoring() {
    if (monitoring.load()) {
        std::cout << "Performance monitoring is already running" << std::endl;
        return true;
    }
    
    if (!pImpl->config.enabled) {
        std::cout << "Performance monitoring is disabled" << std::endl;
        return false;
    }
    
    pImpl->monitoring.store(true);
    pImpl->paused.store(false);
    
    // Initialize system info
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        // Read initial system info
        updateSystemInfo(pImpl->sysInfo);
    }
    
    std::thread monitorThread(&PerformanceMonitor::monitoringWorker, this);
    pImpl->monitorThread = std::move(monitorThread);
    
    std::cout << "Performance monitoring started" << std::endl;
    return true;
}

bool PerformanceMonitor::stopMonitoring() {
    if (!monitoring.load()) {
        std::cout << "Performance monitoring is not running" << std::endl;
        return true;
    }
    
    pImpl->monitoring.store(false);
    
    if (pImpl->monitorThread.joinable()) {
        pImpl->monitorThread.join();
    }
    
    std::cout << "Performance monitoring stopped" << std::endl;
    return true;
}

bool PerformanceMonitor::pauseMonitoring() {
    if (!monitoring.load()) {
        std::cout << "Performance monitoring is not running" << std::endl;
        return true;
    }
    
    pImpl->paused.store(true);
    std::cout << "Performance monitoring paused" << std::endl;
    return true;
}

bool PerformanceMonitor::resumeMonitoring() {
    if (!monitoring.load()) {
        std::cout << "Performance monitoring is not running" << std::endl;
        return true;
    }
    
    pImpl->paused.store(false);
    std::cout << "Performance monitoring resumed" << std::endl;
    return true;
}

PerformanceState PerformanceMonitor::getCurrentState() const {
    PerformanceState state;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        state = convertSysInfoToState(pImpl->sysInfo);
    }
    
    state.timestamp = std::chrono::steady_clock::now();
    return state;
}

PerformanceHistory PerformanceMonitor::getHistory() const {
    return pImpl->history;
}

std::vector<PerformanceAlert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    return pImpl->alerts;
}

double PerformanceMonitor::getCurrentCpuUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
    return convertSysInfoToState(pImpl->sysInfo).cpuUsage;
}

double PerformanceMonitor::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
    return convertSysInfoToState(pImpl->sysInfo).memoryUsage;
}

double PerformanceMonitor::getCurrentMemoryAvailable() const {
    std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
    return convertSysInfoToState(pImpl->sysInfo).memoryAvailable;
}

size_t PerformanceMonitor::getActiveThreadCount() const {
    std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
    return convertSysInfoToState(pImpl->sysInfo).activeThreads;
}

std::string PerformanceMonitor::generateReport() const {
    std::ostringstream oss;
    oss << "=== Performance Monitor Report ===" << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Monitoring: " << (monitoring.load() ? "Yes" : "No") << std::endl;
    oss << "Paused: " << (paused.load() ? "Yes" : "No") << std::endl;
    oss << std::endl;
    
    auto currentState = getCurrentState();
    
    oss << "=== System Metrics ===" << std::endl;
    oss << "CPU Usage: " << (currentState.cpuUsage * 100.0) << "%" << std::endl;
    oss << "Memory Usage: " << currentState.memoryUsage << " MB" << std::endl;
    oss << "Memory Available: " << currentState.memoryAvailable << " MB" << std::endl;
    oss << "Active Threads: " << currentState.activeThreads << std::endl;
    oss << "Total Threads: " << currentState.totalThreads << std::endl;
    oss << "Average Thread CPU: " << currentState.averageThreadCpu << "%" << std::endl;
    oss << "Max Thread CPU: " << currentState.maxThreadCpu << "%" << std::endl;
    oss << std::endl;
    
    oss << "=== I/O Metrics ===" << std::endl;
    oss << "Disk Read Rate: " << currentState.diskReadRate << " MB/s" << std::endl;
    oss << "Disk Write Rate: " << currentState.diskWriteRate << " MB/s" << std::endl;
    oss << "Network Receive Rate: " << currentState.networkReceiveRate << " Mbps" << std::endl;
    oss << "Network Transmit Rate: " << currentState.networkTransmitRate << " Mbps" << std::endl;
    oss << std::endl;
    
    oss << "=== Process Metrics ===" << std::endl;
    oss << "Process ID: " << currentState.processId << std::endl;
    oss << "Process CPU Time: " << currentState.processCpuTime << " seconds" << std::endl;
    oss << "Process User Time: " << currentState.processUserTime << " seconds" << std::endl;
    oss << "Process System Time: " << currentState.processSystemTime << " seconds" << std::endl;
    oss << "Process Memory: " << (currentState.processMemory / 1024 / 1024) << " MB" << std::endl;
    oss << "Process Peak Memory: " << (currentState.processPeakMemory / 1024 / 1024) << " MB" << std::endl;
    oss << std::endl;
    
    oss << "=== Alert Summary ===" << std::endl;
    auto activeAlerts = getActiveAlerts();
    if (activeAlerts.empty()) {
        oss << "No active alerts" << std::endl;
    } else {
        oss << "Active alerts: " << activeAlerts.size() << std::endl;
        for (const auto& alert : activeAlerts) {
            oss << "  [" << alertLevelToString(alert.level) << "] " << alert.message << std::endl;
        }
    }
    
    return oss.str();
}

bool PerformanceMonitor::exportMetrics(const std::string& filename, const std::string& format) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::string content;
    if (format == "text") {
        content = generateReport();
    } else if (format == "json") {
        content = generateJSONReport();
    } else if (format == "csv") {
        content = generateCSVReport();
    } else {
        std::cerr << "Unsupported format: " << format << std::endl;
        return false;
    }
    
    return writeToFile(filename, content);
}

bool PerformanceMonitor::exportHistory(const std::string& filename, const std::string& format) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::string content;
    if (format == "json") {
        content = generateHistoryJSON();
    } else if (format == "csv") {
        content = generateHistoryCSV();
    } else {
        std::cerr << "Unsupported format: " << format << std::endl;
        return false;
    }
    
    return writeToFile(filename, content);
}

void PerformanceMonitor::acknowledgeAlert(size_t index) {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    if (index < pImpl->alerts.size()) {
        pImpl->alerts[index].acknowledged = true;
    }
}

void PerformanceMonitor::clearAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    pImpl->alerts.clear();
}

std::vector<PerformanceAlert> PerformanceMonitor::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    std::vector<PerformanceAlert> activeAlerts;
    for (const auto& alert : pImpl->alerts) {
        if (!alert.acknowledged) {
            activeAlerts.push_back(alert);
        }
    }
    return activeAlerts;
}

std::string PerformanceMonitor::getSystemInfo() const {
    std::ostringstream oss;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        oss << "=== System Information ===" << std::endl;
        oss << "Uptime: " << pImpl->sysInfo.uptime << " seconds" << std::endl;
        oss << "Total RAM: " << (pImpl->sysInfo.totalRam / 1024 / 1024) << " MB" << std::endl;
        oss << "Free RAM: " << (pImpl->sysInfo.freeRam / 1024 / 1024) << " MB" << std::endl;
        oss << "Buffers: " << (pImpl->sysInfo.buffers / 1024 / 1024) << " MB" << std::endl;
        oss << "Cached: " << (pImpl->sysInfo.cached / 1024 / 1024) << " MB" << std::endl;
        oss << "Swap Total: " << (pImpl->sysInfo.swapTotal / 1024 / 1024) << " MB" << std::endl;
        oss << "Swap Free: " << (pImpl->sysInfo.swapFree / 1024 / 1024) << " MB" << std::endl;
        oss << "Processes: " << pImpl->sysInfo.processes << std::endl;
        
        oss << std::endl << "=== Network Interfaces ===" << std::endl;
        for (const auto& iface : pImpl->sysInfo.interfaces) {
            oss << iface.name << ": received " << iface.bytesReceived << " bytes, sent " << iface.bytesSent << " bytes" << std::endl;
        }
        
        oss << std::endl << "=== Disks ===" << std::endl;
        for (const auto& disk : pImpl->sysInfo.disks) {
            oss << disk.mountPoint << " (" << disk.device << ")" << std::endl;
        }
        
        oss << std::endl << "=== Threads ===" << std::endl;
        for (const auto& thread : pImpl->sysInfo.threads) {
            oss << "  " << thread.name << " (PID: " << thread.pid << ") - CPU: " << thread.cpuUsage << "%, Memory: " << (thread.memoryUsage / 1024) << " KB" << std::endl;
        }
    }
    
    return oss.str();
}

std::unordered_map<std::string, std::string> PerformanceMonitor::getSystemInfoMap() const {
    std::unordered_map<std::string, std::string> info;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        info["uptime"] = std::to_string(pImpl->sysInfo.uptime);
        info["total_ram_mb"] = std::to_string(pImpl->sysInfo.totalRam / 1024 / 1024);
        info["free_ram_mb"] = std::to_string(pImpl->sysInfo.freeRam / 1024 / 1024);
        info["cached_mb"] = std::to_string(pImpl->sysInfo.cached / 1024 / 1024);
        info["swap_total_mb"] = std::to_string(pImpl->sysInfo.swapTotal / 1024 / 1024);
        info["swap_free_mb"] = std::to_string(pImpl->sysInfo.swapFree / 1024 / 1024);
        info["processes"] = std::to_string(pImpl->sysInfo.processes);
        
        std::stringstream interfaces;
        for (size_t i = 0; i < pImpl->sysInfo.interfaces.size(); ++i) {
            if (i > 0) interfaces << "; ";
            interfaces << pImpl->sysInfo.interfaces[i].name << "(rx:" << pImpl->sysInfo.interfaces[i].bytesReceived 
                      << "tx:" << pImpl->sysInfo.interfaces[i].bytesSent << ")";
        }
        info["interfaces"] = interfaces.str();
    }
    
    return info;
}

// Private methods
void PerformanceMonitor::monitoringWorker() {
    while (monitoring.load()) {
        if (paused.load()) {
            std::this_thread::sleep_for(pImpl->config.samplingInterval);
            continue;
        }
        
        PerformanceState state;
        collectSystemMetrics(state);
        
        // Add to history
        {
            std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
            pImpl->history.samples.push_back(state);
            if (pImpl->history.samples.size() > pImpl->config.historySize) {
                pImpl->history.samples.pop_front();
            }
        }
        
        // Check for alerts
        checkForAlerts(state);
        
        // Sleep for the configured interval
        std::this_thread::sleep_for(pImpl->config.samplingInterval);
    }
}

void PerformanceMonitor::collectSystemMetrics(PerformanceState& state) const {
    // Collect all metrics
    collectCpuMetrics(state);
    collectMemoryMetrics(state);
    collectDiskMetrics(state);
    collectNetworkMetrics(state);
    collectThreadMetrics(state);
    collectProcessMetrics(state);
    collectIoMetrics(state);
}

void PerformanceMonitor::collectCpuMetrics(PerformanceState& state) const {
    // Simple CPU calculation based on /proc/stat
    double cpuUsage = 0.0;
    
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string line;
        if (std::getline(statFile, line) && line.find("cpu  ") == 0) {
            std::istringstream iss(line);
            std::string cpuLabel;
            long long user, nice, system, idle, iowait, irq, softirq, steal;
            
            iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
            
            long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            long long idleTime = idle + iowait;
            
            if (total > 0) {
                cpuUsage = 1.0 - (static_cast<double>(idleTime) / total);
            }
        }
    }
    
    state.cpuUsage = cpuUsage;
    
    // Update system info
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        pImpl->sysInfo.cpuUsage = cpuUsage;
    }
}

void PerformanceMonitor::collectMemoryMetrics(PerformanceState& state) const {
    long long totalMem = 0;
    long long freeMem = 0;
    long long buffers = 0;
    long long cached = 0;
    long long swapTotal = 0;
    long long swapFree = 0;
    
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        while (std::getline(meminfoFile, line)) {
            if (line.find("MemTotal:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                totalMem = value * 1024;  // Convert to bytes
            } else if (line.find("MemFree:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                freeMem = value * 1024;  // Convert to bytes
            } else if (line.find("Buffers:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                buffers = value * 1024;  // Convert to bytes
            } else if (line.find("Cached:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                cached = value * 1024;  // Convert to bytes
            } else if (line.find("SwapTotal:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                swapTotal = value * 1024;  // Convert to bytes
            } else if (line.find("SwapFree:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                swapFree = value * 1024;  // Convert to bytes
            }
        }
    }
    
    double usedMem = static_cast<double>(totalMem - freeMem);
    state.memoryUsage = usedMem / (1024 * 1024);  // Convert to MB
    state.memoryAvailable = static_cast<double>(freeMem) / (1024 * 1024);  // Convert to MB
    
    // Update system info
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        pImpl->sysInfo.totalMem = totalMem;
        pImpl->sysInfo.freeMem = freeMem;
        pImpl->sysInfo.buffers = buffers;
        pImpl->sysInfo.cached = cached;
        pImpl->sysInfo.swapTotal = swapTotal;
        pImpl->sysInfo.swapFree = swapFree;
    }
}

void PerformanceMonitor::collectDiskMetrics(PerformanceState& state) const {
    double readRate = 0.0;
    double writeRate = 0.0;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        
        // Read current disk stats
        updateDiskStats(pImpl->sysInfo.disks);
        
        // Calculate rates
        if (!pImpl->sysInfo.disks.empty()) {
            readRate = calculateDiskReadRate();
            writeRate = calculateDiskWriteRate();
        }
        
        // Update history
        pImpl->sysInfo.diskReadRate = readRate;
        pImpl->sysInfo.diskWriteRate = writeRate;
    }
    
    state.diskReadRate = readRate;
    state.diskWriteRate = writeRate;
}

void PerformanceMonitor::collectNetworkMetrics(PerformanceState& state) const {
    double receiveRate = 0.0;
    double transmitRate = 0.0;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        
        // Read current network stats
        updateNetworkStats(pImpl->sysInfo.interfaces);
        
        // Calculate rates
        receiveRate = calculateNetworkReceiveRate();
        transmitRate = calculateNetworkTransmitRate();
        
        // Update history
        pImpl->sysInfo.networkReceiveRate = receiveRate;
        pImpl->sysInfo.networkTransmitRate = transmitRate;
    }
    
    state.networkReceiveRate = receiveRate;
    state.networkTransmitRate = transmitRate;
}

void PerformanceMonitor::collectThreadMetrics(PerformanceState& state) const {
    size_t activeThreads = 0;
    size_t totalThreads = 0;
    double avgThreadCpu = 0.0;
    double maxThreadCpu = 0.0;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->sysInfoMutex);
        updateThreadStats(pImpl->sysInfo.threads);
        
        // Calculate metrics
        activeThreads = countActiveThreads(pImpl->sysInfo.threads);
        totalThreads = pImpl->sysInfo.threads.size();
        avgThreadCpu = calculateAverageThreadCpu(pImpl->sysInfo.threads);
        maxThreadCpu = calculateMaxThreadCpu(pImpl->sysInfo.threads);
        
        // Update history
        pImpl->sysInfo.activeThreads = activeThreads;
        pImpl->sysInfo.totalThreads = totalThreads;
        pImpl->sysInfo.averageThreadCpu = avgThreadCpu;
        pImpl->sysInfo.maxThreadCpu = maxThreadCpu;
    }
    
    state.activeThreads = activeThreads;
    state.totalThreads = totalThreads;
    state.averageThreadCpu = avgThreadCpu;
    state.maxThreadCpu = maxThreadCpu;
}

void PerformanceMonitor::collectProcessMetrics(PerformanceState& state) const {
    pid_t pid = getpid();
    
    // Get process resource usage
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        state.processCpuTime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6 +
                              usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
        state.processUserTime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
        state.processSystemTime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    }
    
    // Get process memory usage
    state.processId = pid;
    
    // Try to read from /proc/pid/status
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                state.processMemory = value * 1024;  // Convert to bytes
            } else if (line.find("VmHWM:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                state.processPeakMemory = value * 1024;  // Convert to bytes
            }
        }
    }
}

void PerformanceMonitor::collectIoMetrics(PerformanceState& state) const {
    uint64_t ioReads = 0;
    uint64_t ioWrites = 0;
    uint64_t ioReadBytes = 0;
    uint64_t ioWriteBytes = 0;
    
    // Read I/O stats from /proc/diskstats or /proc/<pid>/io
    // For simplicity, we'll read from /proc/diskstats
    std::ifstream diskstatsFile("/proc/diskstats");
    if (diskstatsFile.is_open()) {
        std::string line;
        while (std::getline(diskstatsFile, line)) {
            std::istringstream iss(line);
            uint64_t major, minor, reads, writes, readBytes, writeBytes, 
                     readsMerged, writesMerged, inProgress, timeSpentIo,
                     timeSpentRead, timeSpentWrite, weightedIoTime;
            
            if (iss >> major >> minor >> reads >> writes >> readBytes >> writeBytes) {
                ioReads += reads;
                ioWrites += writes;
                ioReadBytes += readBytes;
                ioWriteBytes += writeBytes;
            }
        }
    }
    
    state.ioReads = ioReads;
    state.ioWrites = ioWrites;
    state.ioReadBytes = ioReadBytes;
    state.ioWriteBytes = ioWriteBytes;
}

void PerformanceMonitor::checkForAlerts(const PerformanceState& state) {
    if (!pImpl->config.alertsEnabled) return;
    
    // Check CPU threshold
    if (state.cpuUsage > pImpl->config.cpuThreshold) {
        double excess = state.cpuUsage - pImpl->config.cpuThreshold;
        std::string message = "CPU usage exceeded threshold: " + std::to_string(state.cpuUsage * 100) + "% > " +
                             std::to_string(pImpl->config.cpuThreshold * 100) + "%";
        triggerAlert("cpu", state.cpuUsage, AlertLevel::Warning, message);
    }
    
    // Check memory threshold
    double memoryUsageRatio = state.memoryUsage / (state.memoryAvailable + state.memoryUsage);
    if (memoryUsageRatio > pImpl->config.memoryThreshold) {
        double excess = memoryUsageRatio - pImpl->config.memoryThreshold;
        std::string message = "Memory usage exceeded threshold: " + std::to_string(memoryUsageRatio * 100) + "% > " +
                             std::to_string(pImpl->config.memoryThreshold * 100) + "%";
        triggerAlert("memory", memoryUsageRatio, AlertLevel::Warning, message);
    }
}

void PerformanceMonitor::triggerAlert(const std::string& metricName, double value, AlertLevel level, const std::string& message) {
    PerformanceAlert alert;
    alert.level = level;
    alert.message = message;
    alert.metricName = metricName;
    alert.value = value;
    alert.threshold = getCustomThreshold(metricName);
    alert.timestamp = std::chrono::steady_clock::now();
    alert.acknowledged = false;
    
    // Add to alerts list
    {
        std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
        pImpl->alerts.push_back(alert);
        
        // Call alert callback if configured
        if (pImpl->config.alertCallback) {
            pImpl->config.alertCallback(alert);
        }
    }
}

// Helper functions
void PerformanceMonitor::updateSystemInfo(Impl::SysInfo& sysInfo) {
    // Read system uptime
    std::ifstream uptimeFile("/proc/uptime");
    if (uptimeFile.is_open()) {
        uptimeFile >> sysInfo.uptime;
    }
    
    // Read memory info
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        while (std::getline(meminfoFile, line)) {
            if (line.find("MemTotal:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.totalRam = value * 1024;  // Convert to bytes
            } else if (line.find("MemFree:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.freeRam = value * 1024;  // Convert to bytes
            } else if (line.find("Buffers:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.buffers = value * 1024;  // Convert to bytes
            } else if (line.find("Cached:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.cached = value * 1024;  // Convert to bytes
            } else if (line.find("SwapTotal:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.swapTotal = value * 1024;  // Convert to bytes
            } else if (line.find("SwapFree:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.swapFree = value * 1024;  // Convert to bytes
            } else if (line.find("Processes:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                sysInfo.processes = value;
            }
        }
    }
    
    // Read network interfaces
    updateNetworkInterfaces(sysInfo.interfaces);
    
    // Read disks
    updateDisks(sysInfo.disks);
    
    // Read threads
    updateThreads(sysInfo.threads);
    
    // Calculate rates
    if (sysInfo.lastUpdate.time_since_epoch().count() > 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - sysInfo.lastUpdate).count();
        if (elapsed > 0) {
            sysInfo.prevNetReceived = calculateNetReceived(sysInfo.interfaces);
            sysInfo.prevNetSent = calculateNetSent(sysInfo.interfaces);
            sysInfo.prevDiskRead = calculateDiskRead(sysInfo.disks);
            sysInfo.prevDiskWrite = calculateDiskWrite(sysInfo.disks);
        }
    }
    
    sysInfo.lastUpdate = now;
}

void PerformanceMonitor::updateNetworkInterfaces(std::vector<Impl::SysInfo::InterfaceStats>& interfaces) {
    interfaces.clear();
    
    DIR* dir = opendir("/sys/class/net");
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;
        
        std::string ifaceName = entry->d_name;
        if (ifaceName == "." || ifaceName == "..") continue;
        
        // Skip virtual interfaces
        if (ifaceName.find("lo") == 0 || ifaceName.find("docker") == 0) continue;
        
        Impl::SysInfo::InterfaceStats iface;
        iface.name = ifaceName;
        
        // Read statistics from /sys/class/net/<iface>/statistics
        std::string rxPath = "/sys/class/net/" + ifaceName + "/statistics/rx_bytes";
        std::string txPath = "/sys/class/net/" + ifaceName + "/statistics/tx_bytes";
        std::string rxPacketsPath = "/sys/class/net/" + ifaceName + "/statistics/rx_packets";
        std::string txPacketsPath = "/sys/class/net/" + ifaceName + "/statistics/tx_packets";
        
        std::ifstream file;
        
        file.open(rxPath);
        if (file.is_open()) {
            file >> iface.bytesReceived;
            file.close();
        }
        
        file.open(txPath);
        if (file.is_open()) {
            file >> iface.bytesSent;
            file.close();
        }
        
        file.open(rxPacketsPath);
        if (file.is_open()) {
            file >> iface.packetsReceived;
            file.close();
        }
        
        file.open(txPacketsPath);
        if (file.is_open()) {
            file >> iface.packetsSent;
            file.close();
        }
        
        if (iface.bytesReceived > 0 || iface.bytesSent > 0) {
            interfaces.push_back(iface);
        }
    }
    
    closedir(dir);
}

void PerformanceMonitor::updateDisks(std::vector<Impl::SysInfo::DiskStats>& disks) {
    disks.clear();
    
    DIR* dir = opendir("/sys/block");
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;
        
        std::string diskName = entry->d_name;
        
        // Check if it's a physical device (not partition)
        std::string devPath = "/sys/block/" + diskName + "/dev/bdi"
        std::ifstream bdiFile(devPath);
        if (!bdiFile.is_open()) continue;
        
        // Read major and minor numbers
        std::string major, minor;
        bdiFile >> major >> minor;
        bdiFile.close();
        
        // Read mount points
        std::string mountPath = "/sys/block/" + diskName + "/mount_points";
        std::ifstream mountFile(mountPath);
        if (!mountFile.is_open()) continue;
        
        std::string mountPoint;
        while (std::getline(mountFile, mountPoint)) {
            Impl::SysInfo::DiskStats disk;
            disk.device = "sd" + diskName[3]; // Convert to sdX format
            disk.mountPoint = mountPoint;
            
            // Read statistics from /sys/block/<diskName>/stat
            std::string statPath = "/sys/block/" + diskName + "/stat";
            std::ifstream statFile(statPath);
            if (statFile.is_open()) {
                std::string line;
                if (std::getline(statFile, line)) {
                    std::istringstream iss(line);
                    uint64_t readIO, writeIO, readBytes, writeBytes, readTime, writeTime, unmergedReads, unmergedWrites;
                    
                    if (iss >> readIO >> writeIO >> readBytes >> writeBytes >> readTime >> writeTime >> 
                        unmergedReads >> unmergedWrites) {
                        disk.readCount = readIO;
                        disk.writeCount = writeIO;
                        disk.readBytes = readBytes;
                        disk.writeBytes = writeBytes;
                        disk.readTime = readTime;
                        disk.writeTime = writeTime;
                    }
                }
                statFile.close();
            }
            
            disks.push_back(disk);
        }
        
        mountFile.close();
    }
    
    closedir(dir);
}

void PerformanceMonitor::updateThreads(std::vector<Impl::SysInfo::ThreadInfo>& threads) {
    threads.clear();
    
    // Get list of all processes
    DIR* procDir = opendir("/proc");
    if (!procDir) return;
    
    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        // Check if it's a numeric directory (process)
        bool isProcess = true;
        for (char c : entry->d_name) {
            if (!std::isdigit(c)) {
                isProcess = false;
                break;
            }
        }
        if (!isProcess) continue;
        
        std::string procPath = "/proc/" + std::string(entry->d_name);
        
        // Check if it's our process (for simplicity)
        pid_t pid = std::stoi(entry->d_name);
        if (pid == getpid()) {
            Impl::SysInfo::ThreadInfo thread;
            thread.pid = pid;
            thread.name = "main_process";
            
            // Read thread info
            std::ifstream statusFile(procPath + "/status");
            if (statusFile.is_open()) {
                std::string line;
                while (std::getline(statusFile, line)) {
                    if (line.find("Name:") == 0) {
                        thread.name = line.substr(6);
                    }
                }
                statusFile.close();
            }
            
            // Read I/O stats
            std::ifstream ioFile(procPath + "/io");
            if (ioFile.is_open()) {
                std::string line;
                while (std::getline(ioFile, line)) {
                    if (line.find("read_bytes:") == 0) {
                        std::istringstream iss(line);
                        std::string key;
                        uint64_t value;
                        iss >> key >> value;
                        thread.memoryUsage = value;  // Approximate
                    }
                }
                ioFile.close();
            }
            
            threads.push_back(thread);
        }
    }
    
    closedir(procDir);
}

PerformanceState PerformanceMonitor::convertSysInfoToState(const Impl::SysInfo& sysInfo) const {
    PerformanceState state;
    
    // Convert CPU usage
    if (!sysInfo.cpuUsageHistory.empty()) {
        double sum = 0.0;
        for (double usage : sysInfo.cpuUsageHistory) {
            sum += usage;
        }
        state.cpuUsage = sum / sysInfo.cpuUsageHistory.size();
    }
    
    // Convert memory usage
    if (!sysInfo.memoryUsageHistory.empty()) {
        double sum = 0.0;
        for (double usage : sysInfo.memoryUsageHistory) {
            sum += usage;
        }
        state.memoryUsage = sum / sysInfo.memoryUsageHistory.size();
        state.memoryAvailable = sysInfo.totalRam / (1024 * 1024) - state.memoryUsage;
    }
    
    // Convert disk I/O rates
    if (!sysInfo.diskReadRateHistory.empty()) {
        double sum = 0.0;
        for (double rate : sysInfo.diskReadRateHistory) {
            sum += rate;
        }
        state.diskReadRate = sum / sysInfo.diskReadRateHistory.size();
        
        sum = 0.0;
        for (double rate : sysInfo.diskWriteRateHistory) {
            sum += rate;
        }
        state.diskWriteRate = sum / sysInfo.diskWriteRateHistory.size();
    }
    
    // Convert network rates
    if (!sysInfo.networkReceiveRateHistory.empty()) {
        double sum = 0.0;
        for (double rate : sysInfo.networkReceiveRateHistory) {
            sum += rate;
        }
        state.networkReceiveRate = sum / sysInfo.networkReceiveRateHistory.size();
        
        sum = 0.0;
        for (double rate : sysInfo.networkTransmitRateHistory) {
            sum += rate;
        }
        state.networkTransmitRate = sum / sysInfo.networkTransmitRateHistory.size();
    }
    
    // Convert thread metrics
    state.activeThreads = sysInfo.activeThreads;
    state.totalThreads = sysInfo.totalThreads;
    state.averageThreadCpu = sysInfo.averageThreadCpu;
    state.maxThreadCpu = sysInfo.maxThreadCpu;
    
    // Convert process metrics
    state.processId = getpid();
    state.processCpuTime = sysInfo.processCpuTime;
    state.processUserTime = sysInfo.processUserTime;
    state.processSystemTime = sysInfo.processSystemTime;
    state.processMemory = sysInfo.processMemory;
    state.processPeakMemory = sysInfo.processPeakMemory;
    
    // Convert I/O metrics
    state.ioReads = sysInfo.ioReads;
    state.ioWrites = sysInfo.ioWrites;
    state.ioReadBytes = sysInfo.ioReadBytes;
    state.ioWriteBytes = sysInfo.ioWriteBytes;
    
    // Convert network metrics
    state.networkPacketsReceived = sysInfo.networkPacketsReceived;
    state.networkPacketsSent = sysInfo.networkPacketsSent;
    state.networkBytesReceived = sysInfo.networkBytesReceived;
    state.networkBytesSent = sysInfo.networkBytesSent;
    
    state.timestamp = std::chrono::steady_clock::now();
    
    return state;
}

// File I/O methods
bool PerformanceMonitor::writeToFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

bool PerformanceMonitor::ensureOutputDirectory(const std::string& filename) const {
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        std::string directory = filename.substr(0, pos);
        if (!std::filesystem::exists(directory)) {
            return std::filesystem::create_directories(directory);
        }
    }
    return true;
}

// Report generation methods
std::string PerformanceMonitor::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"monitoring\": " << (monitoring.load() ? "true" : "false") << "," << std::endl;
    oss << "  \"paused\": " << (paused.load() ? "true" : "false") << "," << std::endl;
    
    auto currentState = getCurrentState();
    
    oss << "  \"cpu_usage\": " << currentState.cpuUsage << "," << std::endl;
    oss << "  \"memory_usage\": " << currentState.memoryUsage << "," << std::endl;
    oss << "  \"memory_available\": " << currentState.memoryAvailable << "," << std::endl;
    oss << "  \"active_threads\": " << currentState.activeThreads << "," << std::endl;
    oss << "  \"disk_read_rate\": " << currentState.diskReadRate << "," << std::endl;
    oss << "  \"disk_write_rate\": " << currentState.diskWriteRate << "," << std::endl;
    oss << "  \"network_receive_rate\": " << currentState.networkReceiveRate << ",\" << std::endl;
    oss << "  \"network_transmit_rate\": " << currentState.networkTransmitRate << "," << std::endl;
    
    oss << "}" << std::endl;
    return oss.str();
}

std::string PerformanceMonitor::generateCSVReport() const {
    std::ostringstream oss;
    oss << "Metric,Value" << std::endl;
    oss << "CPU Usage," << getCurrentCpuUsage() << std::endl;
    oss << "Memory Usage," << getCurrentMemoryUsage() << std::endl;
    oss << "Memory Available," << getCurrentMemoryAvailable() << std::endl;
    oss << "Active Threads," << getActiveThreadCount() << std::endl;
    oss << "Network Receive Rate," << getCurrentState().networkReceiveRate << std::endl;
    oss << "Network Transmit Rate," << getCurrentState().networkTransmitRate << std::endl;
    return oss.str();
}

std::string PerformanceMonitor::generateHistoryJSON() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"samples\": [" << std::endl;
    
    size_t sampleCount = std::min(static_cast<size_t>(10), pImpl->history.samples.size());
    for (size_t i = 0; i < sampleCount; ++i) {
        size_t index = pImpl->history.samples.size() - 1 - i;
        const auto& sample = pImpl->history.samples[index];
        
        oss << "    {" << std::endl;
        oss << "      \"timestamp\": \"" << getCurrentTimestamp() << "\"," << std::endl;
        oss << "      \"cpu_usage\": " << sample.cpuUsage << "," << std::endl;
        oss << "      \"memory_usage\": " << sample.memoryUsage << "," << std::endl;
        oss << "      \"memory_available\": " << sample.memoryAvailable << "," << std::endl;
        oss << "      \"active_threads\": " << sample.activeThreads << "," << std::endl;
        oss << "      \"disk_read_rate\": " << sample.diskReadRate << "," << std::endl;
        oss << "      \"disk_write_rate\": " << sample.diskWriteRate << "," << std::endl;
        oss << "      \"network_receive_rate\": " << sample.networkReceiveRate << "," << std::endl;
        oss << "      \"network_transmit_rate\": " << sample.networkTransmitRate << "\"" << std::endl;
        
        if (i < sampleCount - 1) {
            oss << "    }," << std::endl;
        } else {
            oss << "    }" << std::endl;
        }
    }
    
    oss << "  ]" << std::endl;
    oss << "}" << std::endl;
    return oss.str();
}

std::string PerformanceMonitor::generateHistoryCSV() const {
    std::ostringstream oss;
    oss << "Timestamp,CPU Usage,Memory Usage,Active Threads,Network Receive,Network Transmit" << std::endl;
    
    size_t sampleCount = std::min(static_cast<size_t>(10), pImpl->history.samples.size());
    for (size_t i = 0; i < sampleCount; ++i) {
        size_t index = pImpl->history.samples.size() - 1 - i;
        const auto& sample = pImpl->history.samples[index];
        
        oss << getCurrentTimestamp() << ",";
        oss << sample.cpuUsage << ",";
        oss << sample.memoryUsage << ",";
        oss << sample.activeThreads << ",";
        oss << sample.networkReceiveRate << ",";
        oss << sample.networkTransmitRate << std::endl;
    }
    
    return oss.str();
}

// Helper functions
std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string PerformanceMonitor::alertLevelToString(AlertLevel level) const {
    switch (level) {
        case AlertLevel::Info: return "INFO";
        case AlertLevel::Warning: return "WARNING";
        case AlertLevel::Critical: return "CRITICAL";
        case AlertLevel::Severe: return "SEVERE";
        default: return "UNKNOWN";
    }
}

double PerformanceMonitor::getCustomThreshold(const std::string& metricName) const {
    auto it = pImpl->config.customThresholds.find(metricName);
    if (it != pImpl->config.customThresholds.end()) {
        return it->second;
    }
    
    // Return default thresholds
    if (metricName == "cpu") return pImpl->config.cpuThreshold;
    if (metricName == "memory") return pImpl->config.memoryThreshold;
    
    return 0.0;
}

void PerformanceMonitor::updateDiskStats(std::vector<Impl::SysInfo::DiskStats>& disks) const {
    // Implementation would update disk statistics from /proc/diskstats
    // For simplicity, this is a placeholder
    disks.clear();
}

double PerformanceMonitor::calculateDiskReadRate() const {
    // Implementation would calculate disk read rate from previous and current stats
    // For simplicity, this is a placeholder
    return 0.0;
}

double PerformanceMonitor::calculateDiskWriteRate() const {
    // Implementation would calculate disk write rate from previous and current stats
    // For simplicity, this is a placeholder
    return 0.0;
}

void PerformanceMonitor::updateNetworkStats(std::vector<Impl::SysInfo::InterfaceStats>& interfaces) const {
    // Implementation would update network statistics from /proc/net/dev
    // For simplicity, this is a placeholder
    interfaces.clear();
}

double PerformanceMonitor::calculateNetworkReceiveRate() const {
    // Implementation would calculate network receive rate from previous and current stats
    // For simplicity, this is a placeholder
    return 0.0;
}

double PerformanceMonitor::calculateNetworkTransmitRate() const {
    // Implementation would calculate network transmit rate from previous and current stats
    // For simplicity, this is a placeholder
    return 0.0;
}

void PerformanceMonitor::updateThreadStats(std::vector<Impl::SysInfo::ThreadInfo>& threads) const {
    // Implementation would update thread statistics from /proc/<pid>/status
    // For simplicity, this is a placeholder
    threads.clear();
}
size_t PerformanceMonitor::countActiveThreads(const std::vector<Impl::SysInfo::ThreadInfo>& threads) const {
    size_t count = 0;
    for (const auto& thread : threads) {
        if (thread.active) count++;
    }
    return count;
}

double PerformanceMonitor::calculateAverageThreadCpu(const std::vector<Impl::SysInfo::ThreadInfo>& threads) const {
    if (threads.empty()) return 0.0;
    
    double sum = 0.0;
    size_t activeCount = 0;
    
    for (const auto& thread : threads) {
        if (thread.active) {
            sum += thread.cpuUsage;
            activeCount++;
        }
    }
    
    if (activeCount == 0) return 0.0;
    return sum / activeCount;
}

double PerformanceMonitor::calculateMaxThreadCpu(const std::vector<Impl::SysInfo::ThreadInfo>& threads) const {
    double max = 0.0;
    for (const auto& thread : threads) {
        if (thread.active && thread.cpuUsage > max) {
            max = thread.cpuUsage;
        }
    }
    return max;
}

uint64_t PerformanceMonitor::calculateNetReceived(const std::vector<Impl::SysInfo::InterfaceStats>& interfaces) const {
    uint64_t total = 0;
    for (const auto& iface : interfaces) {
        total += iface.bytesReceived;
    }
    return total;
}

uint64_t PerformanceMonitor::calculateNetSent(const std::vector<Impl::SysInfo::InterfaceStats>& interfaces) const {
    uint64_t total = 0;
    for (const auto& iface : interfaces) {
        total += iface.bytesSent;
    }
    return total;
}

uint64_t PerformanceMonitor::calculateDiskRead(const std::vector<Impl::SysInfo::DiskStats>& disks) const {
    uint64_t total = 0;
    for (const auto& disk : disks) {
        total += disk.readBytes;
    }
    return total;
}

uint64_t PerformanceMonitor::calculateDiskWrite(const std::vector<Impl::SysInfo::DiskStats>& disks) const {
    uint64_t total = 0;
    for (const auto& disk : disks) {
        total += disk.writeBytes;
    }
    return total;
}

} // namespace nlm