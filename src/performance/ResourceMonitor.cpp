#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <sys/resource.h>
#include <unistd.h>
#include <psapi.h>

namespace nlm {

struct ResourceMonitor::Impl {
    std::atomic<bool> monitoring{false};
    std::thread monitoringThread;
    std::mutex dataMutex;
    double samplingInterval = 1.0; // seconds
    
    ResourceUsage currentUsage;
    std::chrono::system_clock::time_point startTime;
};

ResourceMonitor::ResourceMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
}

ResourceMonitor::~ResourceMonitor() {
    stopMonitoring();
}

void ResourceMonitor::startMonitoring() {
    if (pImpl->monitoring) return;
    
    pImpl->monitoring = true;
    
    pImpl->monitoringThread = std::thread([this]() {
        while (pImpl->monitoring) {
            try {
                std::lock_guard<std::mutex> lock(pImpl->dataMutex);
                updateResourceUsage();
                std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->samplingInterval));
            } catch (const std::exception& e) {
                std::cerr << "Resource monitoring error: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });
}

void ResourceMonitor::stopMonitoring() {
    if (pImpl->monitoring) {
        pImpl->monitoring = false;
        if (pImpl->monitoringThread.joinable()) {
            pImpl->monitoringThread.join();
        }
    }
}

void ResourceMonitor::updateResourceUsage() {
    // Get system memory information
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        pImpl->currentUsage.totalMemory = memInfo.totalram;
        pImpl->currentUsage.availableMemory = memInfo.freeram;
    }
    
    // Get process memory information
    getrusage(RUSAGE_SELF, &memInfo);
    pImpl->currentUsage.processMemory = memInfo.ru_maxrss * 1024; // Convert KB to bytes
    
    // Get soft and hard limits
    struct rlimit softLimit, hardLimit;
    if (getrlimit(RLIMIT_DATA, &softLimit) == 0) {
        pImpl->currentUsage.softMemoryLimit = softLimit.rlim_cur;
    }
    if (getrlimit(RLIMIT_AS, &hardLimit) == 0) {
        pImpl->currentUsage.hardMemoryLimit = hardLimit.rlim_cur;
    }
    
    // Calculate percentages
    if (pImpl->currentUsage.totalMemory > 0) {
        pImpl->currentUsage.memoryPercent = 
            (static_cast<double>(pImpl->currentUsage.totalMemory - pImpl->currentUsage.availableMemory) /
             pImpl->currentUsage.totalMemory) * 100.0;
    }
    
    // Convert to MB for user-friendly display
    pImpl->currentUsage.memoryUsage = pImpl->currentUsage.processMemory / (1024.0 * 1024.0);
    pImpl->currentUsage.swapUsage = 0.0; // Simplified for this implementation
}

ResourceUsage ResourceMonitor::getCurrentResourceUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentUsage;
}

double ResourceMonitor::getCpuUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentUsage.cpuUsage;
}

double ResourceMonitor::getMemoryUsageMB() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentUsage.memoryUsage;
}

double ResourceMonitor::getMemoryPercent() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentUsage.memoryPercent;
}

uint64_t ResourceMonitor::getProcessMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentUsage.processMemory;
}

bool ResourceMonitor::setMemoryLimit(uint64_t limitBytes) {
    struct rlimit rlim;
    rlim.rlim_cur = limitBytes;
    rlim.rlim_max = limitBytes;
    
    if (setrlimit(RLIMIT_DATA, &rlim) == 0 && setrlimit(RLIMIT_AS, &rlim) == 0) {
        return true;
    }
    return false;
}

void ResourceMonitor::reportMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::cout << "Memory Usage Report:" << std::endl;
    std::cout << "  Process Memory: " << formatMemorySize(pImpl->currentUsage.processMemory) << std::endl;
    std::cout << "  Total Memory: " << formatMemorySize(pImpl->currentUsage.totalMemory) << std::endl;
    std::cout << "  Available Memory: " << formatMemorySize(pImpl->currentUsage.availableMemory) << std::endl;
    std::cout << "  Memory Usage: " << formatPercentage(pImpl->currentUsage.memoryPercent) << std::endl;
    std::cout << "  Soft Limit: " << formatMemorySize(pImpl->currentUsage.softMemoryLimit) << std::endl;
    std::cout << "  Hard Limit: " << formatMemorySize(pImpl->currentUsage.hardMemoryLimit) << std::endl;
}

uint64_t ResourceMonitor::getThreadCount() const {
    return pImpl->currentUsage.activeThreads;
}

void ResourceMonitor::setThreadAffinity(int threadId, int cpuId) {
    // Simplified thread affinity setting
    // In a real implementation, this would use pthread_setaffinity_np or similar
    std::cout << "Thread affinity setting requested: Thread " << threadId << " to CPU " << cpuId << std::endl;
}

void ResourceMonitor::setMonitoringInterval(double intervalSeconds) {
    pImpl->samplingInterval = intervalSeconds;
}

void ResourceMonitor::enableResourceTracking(bool enable) {
    if (enable) {
        startMonitoring();
    } else {
        stopMonitoring();
    }
}

bool ResourceMonitor::isMonitoring() const {
    return pImpl->monitoring;
}

void ResourceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    updateResourceUsage();
}

} // namespace nlm
