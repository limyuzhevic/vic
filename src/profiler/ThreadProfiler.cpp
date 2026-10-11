// ThreadProfiler.cpp - Thread-level profiling implementation

#include "profiler/ThreadProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <sys/types.h>
#include <unistd.h>

namespace nlm {

ThreadProfiler::ThreadProfiler()
    : isActive(false), sessionName("thread_default"),
      totalContextSwitches(0), totalPageFaults(0) {
}

ThreadProfiler::ThreadProfiler(const ThreadProfilerConfig& config)
    : isActive(false), sessionName("thread_default"), threadConfig(config),
      totalContextSwitches(0), totalPageFaults(0) {
}

ThreadProfiler::~ThreadProfiler() {
    if (isActive) {
        stopProfiling();
    }
}

void ThreadProfiler::startProfiling(const std::string& sessionName) {
    NLM_LOG_INFO("Starting thread profiling session: " + sessionName);
    
    this->sessionName = sessionName;
    isActive = true;
    
    startTime = std::chrono::steady_clock::now();
    endTime = std::chrono::steady_clock::time_point();
    
    threadInfos.clear();
    synchronizationEvents.clear();
    
    totalContextSwitches = 0;
    totalPageFaults = 0;
    
    if (threadConfig.verbose) {
        NLM_LOG_INFO("Thread profiling started with config:");
        NLM_LOG_INFO("  - Thread tracking: " + std::to_string(threadConfig.enableThreadTracking));
        NLM_LOG_INFO("  - Synchronization tracking: " + std::to_string(threadConfig.enableSynchronizationTracking));
        NLM_LOG_INFO("  - Contention detection: " + std::to_string(threadConfig.enableContentionDetection));
        NLM_LOG_INFO("  - Sampling interval: " + std::to_string(threadConfig.samplingIntervalMs) + "ms");
    }
    
    registerThread("Main Thread");
    
    NLM_LOG_INFO("Thread profiling session started");
}

void ThreadProfiler::stopProfiling() {
    if (!isActive) {
        NLM_LOG_WARNING("Cannot stop profiling: thread profiling not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping thread profiling session: " + sessionName);
    
    endTime = std::chrono::steady_clock::now();
    isActive = false;
    
    cleanupOldData();
    
    NLM_LOG_INFO("Thread profiling session stopped");
}

void ThreadProfiler::registerThread(const std::string& name,
                                   int cpuAffinity,
                                   size_t stackSize) {
    if (!isActive || !threadConfig.enableThreadTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    ThreadInfo info;
    info.id = std::this_thread::get_id();
    info.name = name.empty() ? "Thread-" + std::to_string(std::hash<std::thread::id>{}(info.id)) : name;
    info.cpuAffinity = cpuAffinity;
    info.stackSize = stackSize;
    info.creationTime = std::chrono::steady_clock::now();
    info.lastActivity = info.creationTime;
    info.totalExecutionTime = 0;
    info.cpuTime = 0;
    info.waitTime = 0;
    info.sleepTime = 0;
    info.contextSwitches = 0;
    info.pageFaults = 0;
    info.isDetached = false;
    info.isJoined = false;
    
    threadInfos[info.id] = info;
    
    if (threadConfig.verbose) {
        NLM_LOG_DEBUG("Thread registered: " + info.name + " (ID: " + 
                     std::to_string(reinterpret_cast<uintptr_t>(info.id)) + ")");
    }
}

void ThreadProfiler::unregisterThread() {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        ThreadInfo& info = it->second;
        info.isDetached = true;
        info.isJoined = false;
        
        if (threadConfig.verbose) {
            NLM_LOG_DEBUG("Thread unregistered: " + info.name);
        }
        
        threadInfos.erase(it);
    }
}

void ThreadProfiler::recordThreadExecution(int64_t durationNs) {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        ThreadInfo& info = it->second;
        info.totalExecutionTime += durationNs;
        info.lastActivity = std::chrono::steady_clock::now();
        
        if (threadConfig.verbose) {
            NLM_LOG_DEBUG("Thread execution recorded: " + info.name + " - " + 
                         std::to_string(durationNs) + "ns");
        }
    }
}

void ThreadProfiler::recordThreadCPUTime(int64_t durationNs) {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        it->second.cpuTime += durationNs;
    }
}

void ThreadProfiler::recordThreadWaitTime(int64_t durationNs) {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        it->second.waitTime += durationNs;
    }
}

void ThreadProfiler::recordThreadSleepTime(int64_t durationNs) {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        it->second.sleepTime += durationNs;
    }
}

void ThreadProfiler::recordContextSwitch() {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    totalContextSwitches++;
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        it->second.contextSwitches++;
    }
    
    if (threadConfig.verbose) {
        NLM_LOG_DEBUG("Context switch recorded");
    }
}

void ThreadProfiler::recordPageFault() {
    if (!isActive) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    totalPageFaults++;
    
    auto it = threadInfos.find(std::this_thread::get_id());
    if (it != threadInfos.end()) {
        it->second.pageFaults++;
    }
    
    if (threadConfig.verbose) {
        NLM_LOG_DEBUG("Page fault recorded");
    }
}

void ThreadProfiler::recordSynchronizationEvent(const SynchronizationEvent& event) {
    if (!isActive || !threadConfig.enableSynchronizationTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    synchronizationEvents.push_back(event);
    
    if (threadConfig.verbose) {
        NLM_LOG_DEBUG("Synchronization event recorded: " + event.eventType + " - " + event.lockName);
    }
}

ThreadProfiler::ThreadInfo ThreadProfiler::getThreadInfo(std::thread::id threadId) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = threadInfos.find(threadId);
    if (it != threadInfos.end()) {
        return it->second;
    }
    
    ThreadInfo info;
    info.id = threadId;
    info.name = "Unknown Thread";
    return info;
}

ThreadProfiler::ThreadInfo ThreadProfiler::getCurrentThreadInfo() const {
    return getThreadInfo(std::this_thread::get_id());
}

ThreadProfiler::ThreadStats ThreadProfiler::getThreadStats() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    ThreadStats stats;
    stats.totalThreads = threadInfos.size();
    stats.activeThreads = 0;
    stats.detachedThreads = 0;
    stats.joinedThreads = 0;
    stats.totalExecutionTime = 0;
    stats.totalCpuTime = 0;
    stats.totalWaitTime = 0;
    stats.totalSleepTime = 0;
    stats.totalContextSwitches = totalContextSwitches;
    stats.totalPageFaults = totalPageFaults;
    
    for (const auto& pair : threadInfos) {
        const auto& info = pair.second;
        if (info.lastActivity > startTime) {
            stats.activeThreads++;
        }
        
        if (info.isDetached) {
            stats.detachedThreads++;
        } else if (info.isJoined) {
            stats.joinedThreads++;
        }
        
        stats.totalExecutionTime += info.totalExecutionTime;
        stats.totalCpuTime += info.cpuTime;
        stats.totalWaitTime += info.waitTime;
        stats.totalSleepTime += info.sleepTime;
        
        stats.threadTypes[info.name]++;
        
        if (threadConfig.enableCustomMetrics) {
            for (const auto& metric : info.customMetrics) {
                stats.performanceMetrics[metric.first] += metric.second;
            }
        }
    }
    
    // Convert nanoseconds to milliseconds
    stats.totalExecutionTime /= 1000000;
    stats.totalCpuTime /= 1000000;
    stats.totalWaitTime /= 1000000;
    stats.totalSleepTime /= 1000000;
    
    return stats;
}

std::vector<ThreadProfiler::SynchronizationEvent> ThreadProfiler::getSynchronizationEvents() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return synchronizationEvents;
}

std::vector<ThreadProfiler::ThreadInfo> ThreadProfiler::getActiveThreads() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::vector<ThreadInfo> activeThreads;
    for (const auto& pair : threadInfos) {
        const auto& info = pair.second;
        if (info.lastActivity > startTime) {
            activeThreads.push_back(info);
        }
    }
    
    return activeThreads;
}

std::vector<std::string> ThreadProfiler::detectContention() const {
    std::vector<std::string> contentions;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Check for lock contention
    std::unordered_map<std::string, int> lockCounts;
    for (const auto& event : synchronizationEvents) {
        if (event.eventType == "lock") {
            lockCounts[event.lockName]++;
        }
    }
    
    for (const auto& pair : lockCounts) {
        if (pair.second > 10) { // More than 10 locks in the period
            contentions.push_back("High lock contention on: " + pair.first + " (" + 
                                std::to_string(pair.second) + " locks)");
        }
    }
    
    // Check for thread starvation
    for (const auto& pair : threadInfos) {
        const auto& info = pair.second;
        if (info.waitTime > info.totalExecutionTime * 0.5) { // Wait time > 50% of execution time
            contentions.push_back("Thread starvation detected for thread: " + info.name);
        }
    }
    
    return contentions;
}

std::string ThreadProfiler::generateReport(const std::string& format) const {
    std::ostringstream oss;
    
    if (format == "json" || format == "auto") {
        oss << generateJsonReport();
    } else if (format == "csv" || format == "auto") {
        oss << generateCsvReport();
    } else {
        oss << generateTextReport();
    }
    
    return oss.str();
}

bool ThreadProfiler::exportData(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    std::string data;
    if (format == "json") {
        data = generateJsonReport();
    } else if (format == "csv") {
        data = generateCsvReport();
    } else {
        data = generateTextReport();
    }
    
    file << data;
    file.close();
    
    if (threadConfig.verbose) {
        NLM_LOG_INFO("Thread data exported to: " + filename);
    }
    
    return true;
}

std::string ThreadProfiler::analyzePerformance() const {
    std::ostringstream oss;
    oss << "Thread Performance Analysis\n";
    oss << "==========================\n\n";
    
    ThreadStats stats = getThreadStats();
    
    oss << "Thread Statistics:\n";
    oss << "  Total threads: " << stats.totalThreads << "\n";
    oss << "  Active threads: " << stats.activeThreads << "\n";
    oss << "  Detached threads: " << stats.detachedThreads << "\n";
    oss << "  Joined threads: " << stats.joinedThreads << "\n";
    oss << "  Total execution time: " << stats.totalExecutionTime << "ms\n";
    oss << "  Total CPU time: " << stats.totalCpuTime << "ms\n";
    oss << "  Total wait time: " << stats.totalWaitTime << "ms\n";
    oss << "  Total sleep time: " << stats.totalSleepTime << "ms\n";
    oss << "  Total context switches: " << stats.totalContextSwitches << "\n";
    oss << "  Total page faults: " << stats.totalPageFaults << "\n\n";
    
    if (stats.totalExecutionTime > 0) {
        oss << "Performance Metrics:\n";
        oss << "  CPU utilization: " << (stats.totalCpuTime / stats.totalExecutionTime) * 100 << "%\n";
        oss << "  Wait time percentage: " << (stats.totalWaitTime / stats.totalExecutionTime) * 100 << "%\n";
        oss << "  Sleep time percentage: " << (stats.totalSleepTime / stats.totalExecutionTime) * 100 << "%\n";
    }
    
    // Detect contention
    auto contentions = detectContention();
    if (!contentions.empty()) {
        oss << "\nContention Issues:\n";
        for (const auto& contention : contentions) {
            oss << "  - " << contention << "\n";
        }
    }
    
    return oss.str();
}

int64_t ThreadProfiler::getDuration() const {
    if (!isActive) {
        if (endTime > startTime) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return duration.count();
        }
        return 0;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    return duration.count();
}

void ThreadProfiler::cleanupOldData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Remove old synchronization events
    auto cutoffTime = std::chrono::steady_clock::now() - 
        std::chrono::minutes(threadConfig.samplingIntervalMs * 1000);
    
    synchronizationEvents.erase(
        std::remove_if(synchronizationEvents.begin(), synchronizationEvents.end(),
            [cutoffTime](const SynchronizationEvent& event) {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count() - 
                    event.timestamp > 3600000; // Remove events older than 1 hour
            }),
        synchronizationEvents.end());
    
    // Remove inactive threads
    auto threadCutoffTime = std::chrono::steady_clock::now() - 
        std::chrono::minutes(threadConfig.samplingIntervalMs * 10);
    
    threadInfos.erase(
        std::remove_if(threadInfos.begin(), threadInfos.end(),
            [threadCutoffTime](const std::pair<std::thread::id, ThreadInfo>& pair) {
                return pair.second.lastActivity < threadCutoffTime && 
                       pair.second.isJoined;
            }),
        threadInfos.end());
}

void ThreadProfiler::updateThreadStats() {
    // Thread stats are updated in real-time, no need for periodic updates
}

std::string ThreadProfiler::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"sessionName\": \"" << sessionName << "\",\n";
    oss << "  \"durationMs\": " << getDuration() << ",\n";
    oss << "  \"isActive\": " << (isActive ? "true" : "false") << ",\n";
    
    ThreadStats stats = getThreadStats();
    oss << "  \"threadStats\": {\n";
    oss << "    \"totalThreads\": " << stats.totalThreads << ",\n";
    oss << "    \"activeThreads\": " << stats.activeThreads << ",\n";
    oss << "    \"detachedThreads\": " << stats.detachedThreads << ",\n";
    oss << "    \"joinedThreads\": " << stats.joinedThreads << ",\n";
    oss << "    \"totalExecutionTime\": " << stats.totalExecutionTime << ",\n";
    oss << "    \"totalCpuTime\": " << stats.totalCpuTime << ",\n";
    oss << "    \"totalWaitTime\": " << stats.totalWaitTime << ",\n";
    oss << "    \"totalSleepTime\": " << stats.totalSleepTime << ",\n";
    oss << "    \"totalContextSwitches\": " << stats.totalContextSwitches << ",\n";
    oss << "    \"totalPageFaults\": " << stats.totalPageFaults << "\n";
    oss << "  },\n";
    
    oss << "  \"synchronizationEvents\": [\n";
    for (size_t i = 0; i < synchronizationEvents.size(); ++i) {
        const auto& event = synchronizationEvents[i];
        oss << "    {\"timestamp\": " << event.timestamp << ", \"threadId\": \"" << 
            std::to_string(reinterpret_cast<uintptr_t>(event.threadId)) << "\", \"eventType\": \"" <<
            event.eventType << "\", \"lockName\": \"" << event.lockName << "\", \"duration\": " <<
            event.duration << ", \"successful\": " << (event.successful ? "true" : "false") << "}";
        if (i < synchronizationEvents.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string ThreadProfiler::generateCsvReport() const {
    std::ostringstream oss;
    oss << "timestamp,threadId,threadName,totalExecutionTime,cpuTime,waitTime,sleepTime,contextSwitches,pageFaults\n";
    
    ThreadStats stats = getThreadStats();
    
    for (const auto& pair : threadInfos) {
        const auto& info = pair.second;
        oss << std::chrono::duration_cast<std::chrono::milliseconds>(
            info.creationTime.time_since_epoch()).count() << ","
            << std::to_string(reinterpret_cast<uintptr_t>(info.id)) << ","
            << info.name << ","
            << info.totalExecutionTime / 1000000 << ","
            << info.cpuTime / 1000000 << ","
            << info.waitTime / 1000000 << ","
            << info.sleepTime / 1000000 << ","
            << info.contextSwitches << ","
            << info.pageFaults << "\n";
    }
    
    return oss.str();
}

std::string ThreadProfiler::generateTextReport() const {
    std::ostringstream oss;
    oss << "Thread Profiling Report\n";
    oss << "======================\n\n";
    oss << "Session: " << sessionName << "\n";
    oss << "Duration: " << getDuration() << "ms\n";
    oss << "Status: " << (isActive ? "Active" : "Stopped") << "\n\n";
    
    ThreadStats stats = getThreadStats();
    
    oss << "Thread Statistics:\n";
    oss << "  Total threads: " << stats.totalThreads << "\n";
    oss << "  Active threads: " << stats.activeThreads << "\n";
    oss << "  Detached threads: " << stats.detachedThreads << "\n";
    oss << "  Joined threads: " << stats.joinedThreads << "\n";
    oss << "  Total execution time: " << stats.totalExecutionTime << "ms\n";
    oss << "  Total CPU time: " << stats.totalCpuTime << "ms\n";
    oss << "  Total wait time: " << stats.totalWaitTime << "ms\n";
    oss << "  Total sleep time: " << stats.totalSleepTime << "ms\n";
    oss << "  Total context switches: " << stats.totalContextSwitches << "\n";
    oss << "  Total page faults: " << stats.totalPageFaults << "\n\n";
    
    oss << "Active Threads:\n";
    for (const auto& pair : threadInfos) {
        const auto& info = pair.second;
        if (info.lastActivity > startTime) {
            oss << "  - " << info.name << " (ID: " << 
                std::to_string(reinterpret_cast<uintptr_t>(info.id)) << ")\n";
            oss << "    Execution time: " << info.totalExecutionTime / 1000000 << "ms\n";
            oss << "    CPU time: " << info.cpuTime / 1000000 << "ms\n";
            oss << "    Wait time: " << info.waitTime / 1000000 << "ms\n";
            oss << "    Sleep time: " << info.sleepTime / 1000000 << "ms\n";
            oss << "    Context switches: " << info.contextSwitches << "\n";
            oss << "    Page faults: " << info.pageFaults << "\n\n";
        }
    }
    
    // Detect and report contention issues
    auto contentions = detectContention();
    if (!contentions.empty()) {
        oss << "Contention Issues:\n";
        for (const auto& contention : contentions) {
            oss << "  - " << contention << "\n";
        }
    }
    
    return oss.str();
}

} // namespace nlm