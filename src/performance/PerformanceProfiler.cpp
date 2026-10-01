#include "PerformanceProfiler.hpp"
#include "../../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <cstring>

#ifdef __linux__
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <dirent.h>
#include <fcntl.h>
#endif

#ifdef __APPLE__
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#include <libproc.h>
#endif

namespace nlm {

struct PerformanceProfiler::Impl {
    bool initialized;
    bool isProfiling;
    bool realTimeMonitoring;
    bool detailedTiming;
    bool memoryTracking;
    bool cpuTracking;
    bool ioTracking;
    bool gpuTracking;
    std::string profileTarget;
    std::chrono::milliseconds profilingInterval;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::chrono::high_resolution_clock::time_point lastRealTimeUpdate;
    std::map<std::string, TimerData> timers;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> pausedTimers;
    ProfileStats stats;
    std::thread realTimeThread;
    bool stopRealTimeThread;
    mutable std::mutex dataMutex;
    
    Impl() : initialized(false), isProfiling(false), realTimeMonitoring(false), 
             detailedTiming(true), memoryTracking(true), cpuTracking(true), 
             ioTracking(true), gpuTracking(false), stopRealTimeThread(false),
             profilingInterval(std::chrono::milliseconds(100)) {
        startTime = std::chrono::high_resolution_clock::now();
        lastFrameTime = startTime;
        lastRealTimeUpdate = startTime;
        stats.startTime = startTime;
        stats.memorySamples.reserve(1000);
        stats.cpuSamples.reserve(1000);
        stats.ioSamples.reserve(1000);
        stats.gpuSamples.reserve(1000);
        stats.timers.reserve(100);
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() {
    shutdown();
}

bool PerformanceProfiler::initialize(std::shared_ptr<Config> config) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->initialized) {
        shutdown();
    }
    
    pImpl->initialized = true;
    pImpl->isProfiling = true;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->lastFrameTime = pImpl->startTime;
    pImpl->lastRealTimeUpdate = pImpl->startTime;
    
    // Load configuration
    if (config) {
        pImpl->profileTarget = config->getOr<std::string>("profile.target", "neural_network");
        pImpl->profilingInterval = std::chrono::milliseconds(
            config->getOr<size_t>("profile.interval_ms", 100)
        );
        pImpl->detailedTiming = config->getOr<bool>("profile.detailed_timing", true);
        pImpl->memoryTracking = config->getOr<bool>("profile.memory_tracking", true);
        pImpl->cpuTracking = config->getOr<bool>("profile.cpu_tracking", true);
        pImpl->ioTracking = config->getOr<bool>("profile.io_tracking", true);
        pImpl->gpuTracking = config->getOr<bool>("profile.gpu_tracking", false);
    }
    
    // Initialize real-time monitoring thread
    pImpl->stopRealTimeThread = false;
    pImpl->realTimeThread = std::thread(&PerformanceProfiler::updateRealTimeStats, this);
    
    NLM_LOG_INFO("PerformanceProfiler: Initialized successfully with target: " + pImpl->profileTarget);
    return true;
}

void PerformanceProfiler::shutdown() {
    if (!pImpl->initialized) return;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->dataMutex);
        pImpl->isProfiling = false;
        pImpl->realTimeMonitoring = false;
        pImpl->stopRealTimeThread = true;
    }
    
    if (pImpl->realTimeThread.joinable()) {
        pImpl->realTimeThread.join();
    }
    
    pImpl->initialized = false;
    NLM_LOG_INFO("PerformanceProfiler: Shutdown completed");
}

void PerformanceProfiler::startProfile(const std::string& sectionName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->isProfiling) return;
    
    // Check if already profiling this section
    if (pImpl->pausedTimers.find(sectionName) != pImpl->pausedTimers.end()) {
        return; // Already paused
    }
    
    if (pImpl->stats.activeSections.end() == std::find(
        pImpl->stats.activeSections.begin(), pImpl->stats.activeSections.end(), sectionName)) {
        pImpl->stats.activeSections.push_back(sectionName);
    }
    
    // Create or get timer data
    if (pImpl->stats.timers.find(sectionName) == pImpl->stats.timers.end()) {
        pImpl->stats.timers[sectionName] = TimerData(sectionName);
    }
    
    TimerData& timer = pImpl->stats.timers[sectionName];
    timer.startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopProfile(const std::string& sectionName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->isProfiling) return;
    
    auto it = pImpl->stats.timers.find(sectionName);
    if (it == pImpl->stats.timers.end()) return;
    
    TimerData& timer = it->second;
    if (timer.startTime.time_since_epoch().count() == 0) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - timer.startTime);
    
    timer.totalDuration += duration;
    timer.minDuration = std::min(timer.minDuration, duration);
    timer.maxDuration = std::max(timer.maxDuration, duration);
    timer.callCount++;
    
    if (pImpl->detailedTiming) {
        timer.samples.push_back(duration);
        timer.sampleCount++;
    }
    
    // Remove from active sections
    auto activeIt = std::find(pImpl->stats.activeSections.begin(), 
                             pImpl->stats.activeSections.end(), sectionName);
    if (activeIt != pImpl->stats.activeSections.end()) {
        pImpl->stats.activeSections.erase(activeIt);
    }
}

void PerformanceProfiler::pauseProfile(const std::string& sectionName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->stats.timers.find(sectionName);
    if (it == pImpl->stats.timers.end()) return;
    
    TimerData& timer = it->second;
    if (timer.startTime.time_since_epoch().count() != 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - timer.startTime);
        timer.totalDuration += duration;
        timer.startTime = std::chrono::high_resolution_clock::time_point();
        pImpl->pausedTimers[sectionName] = now;
    }
}

void PerformanceProfiler::resumeProfile(const std::string& sectionName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->pausedTimers.find(sectionName);
    if (it == pImpl->pausedTimers.end()) return;
    
    pImpl->stats.timers[sectionName].startTime = std::chrono::high_resolution_clock::now();
    pImpl->pausedTimers.erase(it);
}

bool PerformanceProfiler::isProfiling(const std::string& sectionName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->stats.timers.find(sectionName);
    if (it == pImpl->stats.timers.end()) return false;
    
    const TimerData& timer = it->second;
    return timer.startTime.time_since_epoch().count() != 0;
}

const PerformanceProfiler::TimerData& PerformanceProfiler::getTimerData(const std::string& sectionName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    static const TimerData emptyTimer("");
    auto it = pImpl->stats.timers.find(sectionName);
    return (it != pImpl->stats.timers.end()) ? it->second : emptyTimer;
}

std::vector<std::string> PerformanceProfiler::getAllSectionNames() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::string> names;
    names.reserve(pImpl->stats.timers.size());
    for (const auto& pair : pImpl->stats.timers) {
        names.push_back(pair.first);
    }
    return names;
}

void PerformanceProfiler::startRealTimeMonitoring(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->realTimeMonitoring = true;
    pImpl->profilingInterval = interval;
}

void PerformanceProfiler::stopRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->realTimeMonitoring = false;
}

bool PerformanceProfiler::isRealTimeMonitoringActive() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->realTimeMonitoring;
}

PerformanceProfiler::ProfileStats PerformanceProfiler::getProfileStats() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->stats;
}

PerformanceProfiler::MemorySample PerformanceProfiler::getCurrentMemorySample() const {
    MemorySample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    
#ifdef __linux__
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        sample.totalMemoryBytes = memInfo.totalram * memInfo.mem_unit;
        sample.allocatedMemoryBytes = memInfo.totalram * memInfo.mem_unit - memInfo.freeram * memInfo.mem_unit;
    }
#endif
    
    sample.allocationCount = pImpl->stats.memorySamples.empty() ? 1 : pImpl->stats.memorySamples.back().allocationCount + 1;
    return sample;
}

PerformanceProfiler::CPUSample PerformanceProfiler::getCurrentCPUSample() const {
    CPUSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    sample.threadCount = std::thread::hardware_concurrency();
    
#ifdef __linux__
    FILE* file = fopen("/proc/stat", "r");
    if (file) {
        char line[256];
        if (fgets(line, sizeof(line), file) != NULL) {
            double user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
            int matches = sscanf(line, "cpu  %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
                                &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
            if (matches == 10) {
                double total = user + nice + system + idle + iowait + irq + softirq + steal;
                double idleTotal = idle + iowait;
                if (total > 0) {
                    sample.cpuUsagePercent = (total - idleTotal) / total * 100.0;
                }
            }
        }
        fclose(file);
    }
#endif
    
    return sample;
}

PerformanceProfiler::IOSample PerformanceProfiler::getCurrentIOSample() const {
    IOSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    return sample;
}

PerformanceProfiler::GPUSample PerformanceProfiler::getCurrentGPUSample() const {
    GPUSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    return sample;
}

void PerformanceProfiler::trackMemoryAllocation(size_t size) {
    if (!pImpl->memoryTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->stats.memorySamples.empty()) {
        auto& lastSample = pImpl->stats.memorySamples.back();
        lastSample.allocationCount++;
    }
}

void PerformanceProfiler::trackMemoryDeallocation(size_t size) {
    if (!pImpl->memoryTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->stats.memorySamples.empty()) {
        auto& lastSample = pImpl->stats.memorySamples.back();
        lastSample.deallocationCount++;
    }
}

uint64_t PerformanceProfiler::getCurrentMemoryUsage() const {
    return pImpl->stats.totalMemoryUsage;
}

uint64_t PerformanceProfiler::getPeakMemoryUsage() const {
    uint64_t peak = 0;
    for (const auto& sample : pImpl->stats.memorySamples) {
        if (sample.totalMemoryBytes > peak) peak = sample.totalMemoryBytes;
    }
    return peak;
}

double PerformanceProfiler::getMemoryAllocationRate() const {
    if (pImpl->stats.memorySamples.size() < 2) return 0.0;
    
    auto first = pImpl->stats.memorySamples.front();
    auto last = pImpl->stats.memorySamples.back();
    auto timeDiff = std::chrono::duration<double>(last.timestamp - first.timestamp).count();
    
    if (timeDiff <= 0.0) return 0.0;
    
    return static_cast<double>(last.allocationCount - first.allocationCount) / timeDiff;
}

void PerformanceProfiler::updateCPUUsage() {
    if (!pImpl->cpuTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    CPUSample sample = getCurrentCPUSample();
    pImpl->stats.cpuSamples.push_back(sample);
    pImpl->stats.cpuUsage = sample.cpuUsagePercent;
}

double PerformanceProfiler::getCurrentCPUUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->stats.cpuUsage;
}

uint32_t PerformanceProfiler::getActiveThreadCount() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->stats.threadCount;
}

void PerformanceProfiler::trackReadOperation(size_t bytes) {
    if (!pImpl->ioTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->stats.ioSamples.empty()) {
        auto& lastSample = pImpl->stats.ioSamples.back();
        lastSample.readBytes += bytes;
        lastSample.readOperations++;
    }
}

void PerformanceProfiler::trackWriteOperation(size_t bytes) {
    if (!pImpl->ioTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->stats.ioSamples.empty()) {
        auto& lastSample = pImpl->stats.ioSamples.back();
        lastSample.writeBytes += bytes;
        lastSample.writeOperations++;
    }
}

size_t PerformanceProfiler::getTotalReadBytes() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    size_t total = 0;
    for (const auto& sample : pImpl->stats.ioSamples) {
        total += sample.readBytes;
    }
    return total;
}

size_t PerformanceProfiler::getTotalWriteBytes() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    size_t total = 0;
    for (const auto& sample : pImpl->stats.ioSamples) {
        total += sample.writeBytes;
    }
    return total;
}

double PerformanceProfiler::getAverageReadSpeedMBps() const {
    if (pImpl->stats.ioSamples.size() < 2) return 0.0;
    
    auto first = pImpl->stats.ioSamples.front();
    auto last = pImpl->stats.ioSamples.back();
    auto timeDiff = std::chrono::duration<double>(last.timestamp - first.timestamp).count();
    
    if (timeDiff <= 0.0) return 0.0;
    
    return static_cast<double>(last.readBytes - first.readBytes) / timeDiff / (1024 * 1024);
}

double PerformanceProfiler::getAverageWriteSpeedMBps() const {
    if (pImpl->stats.ioSamples.size() < 2) return 0.0;
    
    auto first = pImpl->stats.ioSamples.front();
    auto last = pImpl->stats.ioSamples.back();
    auto timeDiff = std::chrono::duration<double>(last.timestamp - first.timestamp).count();
    
    if (timeDiff <= 0.0) return 0.0;
    
    return static_cast<double>(last.writeBytes - first.writeBytes) / timeDiff / (1024 * 1024);
}

void PerformanceProfiler::updateGPUUsage() {
    if (!pImpl->gpuTracking) return;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    GPUSample sample = getCurrentGPUSample();
    pImpl->stats.gpuSamples.push_back(sample);
}

double PerformanceProfiler::getGPUUsagePercent() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    if (pImpl->stats.gpuSamples.empty()) return 0.0;
    return pImpl->stats.gpuSamples.back().gpuUsagePercent;
}

uint64_t PerformanceProfiler::getGPUMemoryUsedMB() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    if (pImpl->stats.gpuSamples.empty()) return 0;
    return pImpl->stats.gpuSamples.back().memoryUsedMB;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->stats.timers.clear();
    pImpl->stats.activeSections.clear();
    pImpl->pausedTimers.clear();
    pImpl->stats.memorySamples.clear();
    pImpl->stats.cpuSamples.clear();
    pImpl->stats.ioSamples.clear();
    pImpl->stats.gpuSamples.clear();
    pImpl->stats.totalMemoryUsage = 0;
    pImpl->stats.cpuUsage = 0.0;
    pImpl->stats.totalExecutionTime = std::chrono::duration<double>(0);
    NLM_LOG_INFO("PerformanceProfiler: Reset completed");
}

void PerformanceProfiler::clearSection(const std::string& sectionName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->stats.timers.erase(sectionName);
    auto activeIt = std::find(pImpl->stats.activeSections.begin(), 
                             pImpl->stats.activeSections.end(), sectionName);
    if (activeIt != pImpl->stats.activeSections.end()) {
        pImpl->stats.activeSections.erase(activeIt);
    }
}

void PerformanceProfiler::clearAllSections() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->stats.timers.clear();
    pImpl->stats.activeSections.clear();
}

void PerformanceProfiler::setProfilingInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->profilingInterval = interval;
}

void PerformanceProfiler::setDetailedTiming(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->detailedTiming = enable;
}

void PerformanceProfiler::setMemoryTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->memoryTracking = enable;
}

void PerformanceProfiler::setCPUTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->cpuTracking = enable;
}

void PerformanceProfiler::setIOTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->ioTracking = enable;
}

void PerformanceProfiler::setGPUTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->gpuTracking = enable;
}

bool PerformanceProfiler::exportProfile(const std::string& filename, const std::string& format) const {
    if (format == "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "Section,TotalTime,MinTime,MaxTime,CallCount,AverageTime,StandardDeviation,CPUTimePercent,MemoryTimePercent\n";
        
        std::lock_guard<std::mutex> lock(pImpl->dataMutex);
        for (const auto& pair : pImpl->stats.timers) {
            const TimerData& data = pair.second;
            file << data.name << ","
                 << data.totalDuration.count() << ","
                 << data.minDuration.count() << ","
                 << data.maxDuration.count() << ","
                 << data.callCount << ","
                 << (data.callCount > 0 ? data.totalDuration.count() / data.callCount : 0.0) << ","
                 << (data.sampleCount > 0 ? std::sqrt(data.samples.size() > 1 ? 
                     std::accumulate(data.samples.begin(), data.samples.end(), 0.0,
                         [](double sum, const auto& d) { return sum + d.count() * d.count(); }) / data.samples.size() - 
                     std::accumulate(data.samples.begin(), data.samples.end(), 0.0,
                         [](double sum, const auto& d) { return sum + d.count(); }) / data.samples.size() * 
                     std::accumulate(data.samples.begin(), data.samples.end(), 0.0,
                         [](double sum, const auto& d) { return sum + d.count() * d.count(); }) / data.samples.size() : 0.0) : 0.0) << ","
                 << data.cpuTimePercent << ","
                 << data.memoryTimePercent << "\n";
        }
        file.close();
    } else {
        NLM_LOG_WARNING("PerformanceProfiler: Unsupported export format '" + format + "'");
        return false;
    }
    
    NLM_LOG_INFO("PerformanceProfiler: Exported profile to " + filename);
    return true;
}

bool PerformanceProfiler::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    file << "{\n";
    file << "  \"profiling_info\": {\n";
    file << "    \"start_time\": \"" << std::chrono::system_clock::to_time_t(pImpl->stats.startTime) << "\",\n";
    file << "    \"total_time\": " << pImpl->stats.totalExecutionTime.count() << ",\n";
    file << "    \"active_sections\": [";
    
    for (size_t i = 0; i < pImpl->stats.activeSections.size(); ++i) {
        file << "\"" << pImpl->stats.activeSections[i] << "\"";
        if (i < pImpl->stats.activeSections.size() - 1) file << ", ";
    }
    file << "],\n";
    
    file << "    \"timers\": {\n";
    bool firstTimer = true;
    for (const auto& pair : pImpl->stats.timers) {
        if (!firstTimer) file << ",\n";
        firstTimer = false;
        
        const TimerData& data = pair.second;
        file << "      \"" << data.name << "\": {\n";
        file << "        \"total_time\": " << data.totalDuration.count() << ",\n";
        file << "        \"min_time\": " << data.minDuration.count() << ",\n";
        file << "        \"max_time\": " << data.maxDuration.count() << ",\n";
        file << "        \"call_count\": " << data.callCount << ",\n";
        file << "        \"average_time\": " << (data.callCount > 0 ? data.totalDuration.count() / data.callCount : 0.0) << "\n";
        file << "      }";
    }
    
    file << "\n    }\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool PerformanceProfiler::exportToSQLite(const std::string& filename) const {
    NLM_LOG_INFO("PerformanceProfiler: SQLite export not yet implemented");
    return false;
}

std::vector<std::string> PerformanceProfiler::getBottleneckSections(double threshold) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::string> bottlenecks;
    
    double totalTime = pImpl->stats.totalExecutionTime.count();
    if (totalTime == 0) return bottlenecks;
    
    for (const auto& pair : pImpl->stats.timers) {
        const TimerData& data = pair.second;
        if (data.callCount > 0) {
            double avgTime = data.totalDuration.count() / data.callCount;
            if (avgTime > 0.0 && avgTime / totalTime > threshold) {
                bottlenecks.push_back(data.name);
            }
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceProfiler::getTopSlowSections(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::pair<std::string, double>> sections;
    
    for (const auto& pair : pImpl->stats.timers) {
        const TimerData& data = pair.second;
        double avgTime = data.callCount > 0 ? data.totalDuration.count() / data.callCount : 0.0;
        sections.emplace_back(pair.first, avgTime);
    }
    
    std::sort(sections.begin(), sections.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> topSections;
    size_t limit = std::min(count, sections.size());
    for (size_t i = 0; i < limit; ++i) {
        topSections.push_back(sections[i].first);
    }
    
    return topSections;
}

double PerformanceProfiler::calculateSectionTimePercent(const std::string& sectionName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->stats.timers.find(sectionName);
    if (it == pImpl->stats.timers.end() || pImpl->stats.totalExecutionTime.count() == 0) return 0.0;
    
    const TimerData& timer = it->second;
    return timer.totalDuration.count() / pImpl->stats.totalExecutionTime.count() * 100.0;
}

std::string PerformanceProfiler::getProfileSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::stringstream ss;
    ss << "Performance Profile Summary:" << std::endl;
    ss << "  Total Execution Time: " << pImpl->stats.totalExecutionTime.count() << "s" << std::endl;
    ss << "  Active Sections: " << pImpl->stats.activeSections.size() << std::endl;
    ss << "  Total Timers: " << pImpl->stats.timers.size() << std::endl;
    ss << "  Current Memory Usage: " << pImpl->stats.totalMemoryUsage << " bytes" << std::endl;
    ss << "  Current CPU Usage: " << pImpl->stats.cpuUsage << "%" << std::endl;
    ss << "  Real-time Monitoring: " << (pImpl->realTimeMonitoring ? "Active" : "Inactive") << std::endl;
    
    return ss.str();
}

void PerformanceProfiler::updateRealTimeStats() {
    while (pImpl->stopRealTimeThread) {
        std::this_thread::sleep_for(pImpl->profilingInterval);
        
        if (!pImpl->isProfiling) break;
        
        updateCPUUsage();
        
        if (pImpl->memoryTracking) {
            MemorySample sample = getCurrentMemorySample();
            std::lock_guard<std::mutex> lock(pImpl->dataMutex);
            pImpl->stats.memorySamples.push_back(sample);
            cleanupOldSamples();
        }
        
        if (pImpl->ioTracking) {
            IOSample sample = getCurrentIOSample();
            std::lock_guard<std::mutex> lock(pImpl->dataMutex);
            pImpl->stats.ioSamples.push_back(sample);
            cleanupOldSamples();
        }
        
        if (pImpl->gpuTracking) {
            updateGPUUsage();
            cleanupOldSamples();
        }
        
        // Update total execution time
        auto now = std::chrono::high_resolution_clock::now();
        pImpl->stats.totalExecutionTime += std::chrono::duration<double>(now - pImpl->lastRealTimeUpdate);
        pImpl->lastRealTimeUpdate = now;
        
        // Update FPS
        auto frameDelta = std::chrono::duration<double>(now - pImpl->lastFrameTime);
        if (frameDelta.count() > 0.0) {
            pImpl->stats.averageFPS = 1.0 / frameDelta.count();
            pImpl->stats.minFPS = std::min(pImpl->stats.minFPS, pImpl->stats.averageFPS);
            pImpl->stats.maxFPS = std::max(pImpl->stats.maxFPS, pImpl->stats.averageFPS);
            pImpl->lastFrameTime = now;
        }
    }
}

void PerformanceProfiler::cleanupOldSamples() {
    auto now = std::chrono::high_resolution_clock::now();
    
    // Remove samples older than 1 hour
    auto cutoff = now - std::chrono::hours(1);
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto removeOlderThan = [cutoff](auto& samples) {
        samples.erase(std::remove_if(samples.begin(), samples.end(),
                                   [cutoff](const auto& sample) {
                                       return sample.timestamp < cutoff;
                                   }),
                     samples.end());
    };
    
    removeOlderThan(pImpl->stats.memorySamples);
    removeOlderThan(pImpl->stats.cpuSamples);
    removeOlderThan(pImpl->stats.ioSamples);
    removeOlderThan(pImpl->stats.gpuSamples);
}

} // namespace nlm
