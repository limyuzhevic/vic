#include "ResourceMonitor.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sstream>

namespace nlm {

ResourceMonitor::ResourceMonitor() : monitoringActive(false) {
}

void ResourceMonitor::startMonitoring(const std::string& sessionName) {
    if (sessions.find(sessionName) != sessions.end()) {
        log("ERROR: Session already exists: " + sessionName);
        return;
    }
    
    MonitoringSession session;
    session.sessionName = sessionName;
    session.startTime = std::chrono::steady_clock::now();
    session.isActive = true;
    
    sessions[sessionName] = session;
    
    // Start monitoring thread
    monitoringActive = true;
    monitoringThread = std::thread([this, sessionName]() {
        while (monitoringActive && hasSession(sessionName)) {
            ResourceUsage usage = collectResourceUsage();
            recordResourceUsage(sessionName, usage);
            
            // Check thresholds
            if (usage.cpuUsage > thresholds.maxCpuUsage ||
                usage.memoryUsage > thresholds.maxMemoryUsage ||
                usage.virtualMemoryUsage > thresholds.maxVirtualMemoryUsage ||
                usage.diskUsage > thresholds.maxDiskUsage ||
                usage.networkUsage > thresholds.maxNetworkUsage) {
                if (thresholds.thresholdExceededCallback) {
                    thresholds.thresholdExceededCallback(usage);
                }
            }
            
            std::this_thread::sleep_for(thresholds.monitoringInterval);
        }
    });
    
    log("Started resource monitoring for session: " + sessionName);
}

void ResourceMonitor::stopMonitoring(const std::string& sessionName) {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return;
    }
    
    sessions[sessionName].endTime = std::chrono::steady_clock::now();
    sessions[sessionName].isActive = false;
    
    monitoringActive = false;
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
    
    log("Stopped resource monitoring for session: " + sessionName);
}

ResourceMonitor::ResourceUsage ResourceMonitor::getCurrentResourceUsage() const {
    return collectResourceUsage();
}

void ResourceMonitor::recordResourceUsage(const std::string& sessionName, const ResourceUsage& usage) {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return;
    }
    
    sessions[sessionName].resourceUsageHistory.push_back(usage);
}

ResourceMonitor::ResourceUsage ResourceMonitor::getLastResourceUsage(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end() || sessions.at(sessionName).resourceUsageHistory.empty()) {
        return ResourceUsage();
    }
    
    return sessions.at(sessionName).resourceUsageHistory.back();
}

std::vector<ResourceMonitor::ResourceUsage> ResourceMonitor::getResourceUsageHistory(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return std::vector<ResourceUsage>();
    }
    
    return sessions.at(sessionName).resourceUsageHistory;
}

ResourceMonitor::ResourceStatistics ResourceMonitor::calculateStatistics(const std::string& sessionName) const {
    ResourceStatistics stats;
    
    if (sessions.find(sessionName) == sessions.end() || sessions.at(sessionName).resourceUsageHistory.empty()) {
        return stats;
    }
    
    const auto& history = sessions.at(sessionName).resourceUsageHistory;
    size_t count = history.size();
    
    if (count == 0) return stats;
    
    double totalCpu = 0.0, totalMemory = 0.0, totalDisk = 0.0, totalNetwork = 0.0;
    size_t maxMemory = 0;
    
    for (const auto& usage : history) {
        totalCpu += usage.cpuUsage;
        totalMemory += usage.memoryUsage;
        totalDisk += usage.diskUsage;
        totalNetwork += usage.networkUsage;
        if (usage.memoryUsage > maxMemory) maxMemory = usage.memoryUsage;
    }
    
    stats.averageCpuUsage = totalCpu / count;
    stats.averageMemoryUsage = totalMemory / count;
    stats.peakMemoryUsage = maxMemory;
    stats.averageDiskUsage = totalDisk / count;
    stats.averageNetworkUsage = totalNetwork / count;
    
    // Calculate standard deviations
    double cpuVariance = 0.0, memoryVariance = 0.0;
    for (const auto& usage : history) {
        cpuVariance += (usage.cpuUsage - stats.averageCpuUsage) * (usage.cpuUsage - stats.averageCpuUsage);
        memoryVariance += (static_cast<double>(usage.memoryUsage) - stats.averageMemoryUsage) * 
                         (static_cast<double>(usage.memoryUsage) - stats.averageMemoryUsage);
    }
    
    stats.cpuUsageStdDev = std::sqrt(cpuVariance / count);
    stats.memoryUsageStdDev = std::sqrt(memoryVariance / count);
    
    return stats;
}

std::string ResourceMonitor::analyzeResourceUsage(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    std::stringstream analysis;
    analysis << "Resource Usage Analysis for Session: " << sessionName << std::endl;
    analysis << "===========================================" << std::endl;
    
    const auto& stats = calculateStatistics(sessionName);
    
    analysis << "Average CPU Usage: " << stats.averageCpuUsage << "%" << std::endl;
    analysis << "CPU Usage Standard Deviation: " << stats.cpuUsageStdDev << "%" << std::endl;
    analysis << "Average Memory Usage: " << stats.averageMemoryUsage / (1024 * 1024) << " MB" << std::endl;
    analysis << "Peak Memory Usage: " << stats.peakMemoryUsage / (1024 * 1024) << " MB" << std::endl;
    analysis << "Memory Usage Standard Deviation: " << stats.memoryUsageStdDev / (1024 * 1024) << " MB" << std::endl;
    analysis << "Average Disk Usage: " << stats.averageDiskUsage << "%" << std::endl;
    analysis << "Average Network Usage: " << stats.averageNetworkUsage << " MB/s" << std::endl;
    
    return analysis.str();
}

std::string ResourceMonitor::generateResourceReport(const std::string& sessionName) const {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    std::stringstream report;
    report << "Resource Monitor Report for Session: " << sessionName << std::endl;
    report << "=====================================" << std::endl;
    report << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    report << std::endl;
    
    const auto& stats = calculateStatistics(sessionName);
    
    report << "Summary:" << std::endl;
    report << "  - Monitoring Duration: " << std::chrono::duration<double>(sessions.at(sessionName).endTime - sessions.at(sessionName).startTime).count() << " seconds" << std::endl;
    report << "  - Data Points Collected: " << sessions.at(sessionName).resourceUsageHistory.size() << std::endl;
    report << "  - Peak Memory Usage: " << stats.peakMemoryUsage / (1024 * 1024) << " MB" << std::endl;
    report << "  - Average CPU Usage: " << stats.averageCpuUsage << "%" << std::endl;
    report << std::endl;
    
    // Threshold analysis
    report << "Threshold Analysis:" << std::endl;
    if (stats.averageCpuUsage > thresholds.maxCpuUsage) {
        report << "  - WARNING: Average CPU usage exceeds threshold (" << thresholds.maxCpuUsage << "%)" << std::endl;
    }
    if (stats.averageMemoryUsage > thresholds.maxMemoryUsage) {
        report << "  - WARNING: Average memory usage exceeds threshold (" << thresholds.maxMemoryUsage / (1024 * 1024) << " MB)" << std::endl;
    }
    if (stats.averageDiskUsage > thresholds.maxDiskUsage) {
        report << "  - WARNING: Average disk usage exceeds threshold (" << thresholds.maxDiskUsage << "%)" << std::endl;
    }
    
    return report.str();
}

void ResourceMonitor::setResourceThresholds(const ResourceThresholds& thresholds) {
    this->thresholds = thresholds;
}

const ResourceMonitor::ResourceThresholds& ResourceMonitor::getResourceThresholds() const {
    return thresholds;
}

bool ResourceMonitor::saveMonitoringData(const std::string& sessionName, const std::string& filepath) const {
    if (sessions.find(sessionName) == sessions.end()) {
        log("ERROR: Session not found: " + sessionName);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Simple JSON-like output (placeholder for actual JSON library)
    file << "{\n  \"sessionName\": \"" << sessionName << "\"," << std::endl;
    file << "  \"startTime\": " << std::chrono::duration<double>(sessions.at(sessionName).startTime.time_since_epoch()).count() << "," << std::endl;
    file << "  \"endTime\": " << std::chrono::duration<double>(sessions.at(sessionName).endTime.time_since_epoch()).count() << "," << std::endl;
    file << "  \"usageHistory\": [" << std::endl;
    
    const auto& session = sessions.at(sessionName);
    for (size_t i = 0; i < session.resourceUsageHistory.size(); ++i) {
        const auto& usage = session.resourceUsageHistory[i];
        file << "    {\"timestamp\": " << std::chrono::duration<double>(usage.timestamp.time_since_epoch()).count() << "," << std::endl;
        file << "     \"cpuUsage\": " << usage.cpuUsage << "," << std::endl;
        file << "     \"memoryUsage\": " << usage.memoryUsage << "," << std::endl;
        file << "     \"virtualMemoryUsage\": " << usage.virtualMemoryUsage << "," << std::endl;
        file << "     \"diskUsage\": " << usage.diskUsage << "," << std::endl;
        file << "     \"networkUsage\": " << usage.networkUsage << "}" << std::endl;
        
        if (i < session.resourceUsageHistory.size() - 1) file << ",";
    }
    
    file << "  ]\n}";
    
    file.close();
    log("Monitoring data saved to: " + filepath);
    return true;
}

bool ResourceMonitor::loadMonitoringData(const std::string& sessionName, const std::string& filepath) {
    // TODO: Implement monitoring data loading
    log("Loading monitoring data from: " + filepath);
    return false;
}

bool ResourceMonitor::hasSession(const std::string& sessionName) const {
    return sessions.find(sessionName) != sessions.end();
}

std::vector<std::string> ResourceMonitor::getSessionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : sessions) {
        names.push_back(pair.first);
    }
    return names;
}

void ResourceMonitor::clearSession(const std::string& sessionName) {
    if (sessions.find(sessionName) != sessions.end()) {
        sessions.erase(sessionName);
        log("Cleared monitoring session: " + sessionName);
    }
}

void ResourceMonitor::clearAllSessions() {
    sessions.clear();
    log("Cleared all monitoring sessions");
}

std::string ResourceMonitor::optimizeResourceUsage(const std::string& sessionName) {
    if (sessions.find(sessionName) == sessions.end()) {
        return "ERROR: Session not found";
    }
    
    return "Resource optimization recommendations for " + sessionName + " will be generated here.";
}

std::vector<std::string> ResourceMonitor::getResourceOptimizationRecommendations(const std::string& sessionName) const {
    std::vector<std::string> recommendations;
    
    if (!hasSession(sessionName)) {
        return recommendations;
    }
    
    recommendations.push_back("Consider reducing memory allocation to stay within thresholds");
    recommendations.push_back("Optimize CPU-intensive operations to reduce CPU usage");
    recommendations.push_back("Implement memory pooling to reduce memory fragmentation");
    recommendations.push_back("Consider using more efficient algorithms to reduce disk usage");
    recommendations.push_back("Implement data compression to reduce network usage");
    
    return recommendations;
}

// Implementation of helper functions
ResourceMonitor::ResourceUsage ResourceMonitor::collectResourceUsage() const {
    ResourceUsage usage;
    
    usage.timestamp = std::chrono::steady_clock::now();
    usage.processId = getProcessId();
    
    // Get system metrics
    usage.cpuUsage = calculateCpuUsage();
    usage.memoryUsage = getMemoryUsage();
    usage.virtualMemoryUsage = getVirtualMemoryUsage();
    usage.diskUsage = getDiskUsage("/");
    usage.networkUsage = getNetworkUsage();
    
    return usage;
}

double ResourceMonitor::calculateCpuUsage() const {
    // Placeholder implementation
    return 50.0;  // Return 50% CPU usage as placeholder
}

size_t ResourceMonitor::getMemoryUsage() const {
    // Placeholder implementation
    return 200 * 1024 * 1024;  // Return 200 MB as placeholder
}

size_t ResourceMonitor::getVirtualMemoryUsage() const {
    // Placeholder implementation
    return 500 * 1024 * 1024;  // Return 500 MB as placeholder
}

double ResourceMonitor::getDiskUsage(const std::string& path) const {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        return 0.0;
    }
    
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long free = stat.f_bfree * stat.f_frsize;
    
    return (1.0 - static_cast<double>(free) / total) * 100.0;
}

double ResourceMonitor::getNetworkUsage() const {
    // Placeholder implementation
    return 10.0;  // Return 10 MB/s as placeholder
}

std::string ResourceMonitor::getProcessId() const {
    // Placeholder implementation
    return "12345";  // Return dummy process ID
}

std::vector<std::string> ResourceMonitor::getDiskUsage(const std::string& path) const {
    std::vector<std::string> results;
    
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    
    if (!dir) {
        results.push_back("Error opening directory: " + path);
        return results;
    }
    
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        std::string fullPath = path + "/" + filename;
        
        struct stat st;
        if (stat(fullPath.c_str(), &st) == 0) {
            std::stringstream result;
            result << filename;
            if (S_ISDIR(st.st_mode)) {
                result << "/ (directory)";
            } else {
                result << " (" << st.st_size << " bytes)";
            }
            results.push_back(result.str());
        }
    }
    
    closedir(dir);
    return results;
}

std::vector<std::string> ResourceMonitor::getProcessList() const {
    std::vector<std::string> results;
    
    // Placeholder implementation
    results.push_back("Process 1: NLM Core (CPU: 25%, Memory: 50MB)");
    results.push_back("Process 2: Neural Network (CPU: 75%, Memory: 200MB)");
    results.push_back("Process 3: Environment Simulation (CPU: 10%, Memory: 30MB)");
    
    return results;
}

ResourceMonitor::ResourceUsage ResourceMonitor::getProcessResourceUsage(const std::string& processId) const {
    ResourceUsage usage;
    
    // Placeholder implementation
    usage.processId = processId;
    usage.cpuUsage = 25.0 + (std::rand() % 50);  // Random between 25-75%
    usage.memoryUsage = 50 * 1024 * 1024 + (std::rand() % 100) * 1024 * 1024;  // Random between 50-150 MB
    usage.virtualMemoryUsage = 100 * 1024 * 1024 + (std::rand() % 200) * 1024 * 1024;
    usage.diskUsage = 10.0 + (std::rand() % 40);  // Random between 10-50%
    usage.networkUsage = 1.0 + (std::rand() % 10);  // Random between 1-11 MB/s
    
    return usage;
}

double ResourceMonitor::getNetworkUploadSpeed() const {
    // Placeholder implementation
    return 5.0;  // Return 5 MB/s as placeholder
}

double ResourceMonitor::getNetworkDownloadSpeed() const {
    // Placeholder implementation
    return 10.0;  // Return 10 MB/s as placeholder
}

std::string ResourceMonitor::getNetworkInterfaceStats() const {
    std::stringstream stats;
    stats << "Network Interface Statistics:" << std::endl;
    stats << "  - Upload Speed: " << getNetworkUploadSpeed() << " MB/s" << std::endl;
    stats << "  - Download Speed: " << getNetworkDownloadSpeed() << " MB/s" << std::endl;
    stats << "  - Interface: eth0 (192.168.1.100)" << std::endl;
    
    return stats.str();
}

std::string ResourceMonitor::getSystemInfo() const {
    std::stringstream info;
    info << "System Information:" << std::endl;
    info << "  - Platform: Linux" << std::endl;
    info << "  - Architecture: x86_64" << std::endl;
    info << "  - Available Memory: " << getAvailableMemory() / (1024 * 1024) << " MB" << std::endl;
    info << "  - System Load Average: " << getSystemLoadAverage() << std::endl;
    info << "  - Current CPU Usage: " << calculateCpuUsage() << "%" << std::endl;
    
    return info.str();
}

size_t ResourceMonitor::getAvailableMemory() const {
    // Placeholder implementation
    return 1024 * 1024 * 1024;  // Return 1 GB as placeholder
}

double ResourceMonitor::getSystemLoadAverage() const {
    // Placeholder implementation
    return 1.5;  // Return 1.5 as placeholder
}

} // namespace nlm