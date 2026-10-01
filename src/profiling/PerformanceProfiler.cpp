#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

struct PerformanceProfiler::Impl {
    bool active;
    bool realTimeMonitoring;
    bool detailedTiming;
    std::string profileTarget;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastFrameTime;
    std::map<std::string, ProfileData> profileData;
    std::chrono::duration<double> totalExecutionTime;
    size_t totalMemoryUsage;
    double memoryAllocationRate;
    double cpuUsage;
    size_t ioOperations;
    size_t networkOperations;
    size_t threadCount;
    double fps;
    double averageFPS;
    double minFPS;
    double maxFPS;
    RealTimeStats realTimeStats;
    std::vector<std::chrono::steady_clock::time_point> frameTimestamps;
    
    Impl() : active(false), realTimeMonitoring(false), detailedTiming(true),
             totalMemoryUsage(0), memoryAllocationRate(0.0), cpuUsage(0.0),
             ioOperations(0), networkOperations(0), threadCount(0), fps(0.0),
             averageFPS(0.0), minFPS(0.0), maxFPS(0.0), startTime(std::chrono::steady_clock::now()),
             lastFrameTime(std::chrono::steady_clock::now()) {}
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

bool PerformanceProfiler::initialize() {
    // Initialize profiler
    pImpl->active = true;
    pImpl->frameTimestamps.clear();
    pImpl->lastFrameTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("PerformanceProfiler: Initialized successfully");
    return true;
}

void PerformanceProfiler::startProfile(const std::string& sectionName) {
    // Start timing a section
    if (!pImpl->active) return;
    
    // Create profile data if it doesn't exist
    if (pImpl->profileData.find(sectionName) == pImpl->profileData.end()) {
        pImpl->profileData[sectionName] = ProfileData(sectionName);
    }
    
    // Record timestamp
    auto timestamp = std::chrono::steady_clock::now();
    pImpl->profileData[sectionName].timestamps.push_back(timestamp);
    pImpl->profileData[sectionName].callCount++;
}

void PerformanceProfiler::stopProfile(const std::string& sectionName) {
    if (!pImpl->active) return;
    
    auto it = pImpl->profileData.find(sectionName);
    if (it == pImpl->profileData.end()) return;
    
    ProfileData& data = it->second;
    if (data.callCount == 0) return;
    
    // Calculate duration
    auto endTime = std::chrono::steady_clock::now();
    auto startTime = data.timestamps.back();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    // Update statistics
    data.totalTime += duration;
    data.minTime = std::min(data.minTime, duration);
    data.maxTime = std::max(data.maxTime, duration);
    
    // Calculate average and standard deviation
    data.averageTime = data.totalTime.count() / data.callCount;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (const auto& timestamp : data.timestamps) {
        double time = std::chrono::duration<double>(timestamp - data.timestamps[0]).count();
        variance += (time - data.averageTime) * (time - data.averageTime);
    }
    variance /= data.timestamps.size();
    data.standardDeviation = std::sqrt(variance);
}

std::vector<PerformanceProfiler::ProfileData> PerformanceProfiler::getProfileData() const {
    std::vector<ProfileData> result;
    for (const auto& pair : pImpl->profileData) {
        result.push_back(pair.second);
    }
    return result;
}

PerformanceProfiler::ProfileData PerformanceProfiler::getProfileData(const std::string& sectionName) const {
    auto it = pImpl->profileData.find(sectionName);
    return (it != pImpl->profileData.end()) ? it->second : ProfileData(sectionName);
}

double PerformanceProfiler::getTotalExecutionTime() const {
    return pImpl->totalExecutionTime.count();
}

size_t PerformanceProfiler::getMemoryUsage() const {
    return pImpl->totalMemoryUsage;
}

double PerformanceProfiler::getMemoryAllocationRate() const {
    return pImpl->memoryAllocationRate;
}

double PerformanceProfiler::getCPUUsage() const {
    return pImpl->cpuUsage;
}
size_t PerformanceProfiler::getIOOperations() const {
    return pImpl->ioOperations;
}
size_t PerformanceProfiler::getNetworkOperations() const {
    return pImpl->networkOperations;
}
size_t PerformanceProfiler::getThreadCount() const {
    return pImpl->threadCount;
}
bool PerformanceProfiler::isActive() const {
    return pImpl->active;
}

void PerformanceProfiler::clearProfileData() {
    pImpl->profileData.clear();
    NLM_LOG_INFO("PerformanceProfiler: Profile data cleared");
}

bool PerformanceProfiler::exportProfile(const std::string& filename, const std::string& format) const {
    NLM_LOG_INFO("PerformanceProfiler: Exporting profile to " + filename + "." + format);
    
    if (format == "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "Section,TotalTime,MinTime,MaxTime,CallCount,AverageTime,StandardDeviation,CPUTimePercent,MemoryTimePercent\n";
        
        for (const auto& pair : pImpl->profileData) {
            const ProfileData& data = pair.second;
            file << data.sectionName << ","
                 << data.totalTime.count() << ","
                 << data.minTime.count() << ","
                 << data.maxTime.count() << ","
                 << data.callCount << ","
                 << data.averageTime << ","
                 << data.standardDeviation << ","
                 << data.cpuTimePercent << ","
                 << data.memoryTimePercent << "\n";
        }
        
        file.close();
    }
    else {
        NLM_LOG_WARNING("PerformanceProfiler: Unsupported export format '" + format + "'");
        return false;
    }
    
    return true;
}

void PerformanceProfiler::setProfileTarget(const std::string& target) {
    pImpl->profileTarget = target;
}

std::string PerformanceProfiler::getProfileTarget() const {
    return pImpl->profileTarget;
}

void PerformanceProfiler::setDetailedTiming(bool enable) {
    pImpl->detailedTiming = enable;
}

bool PerformanceProfiler::isDetailedTimingEnabled() const {
    return pImpl->detailedTiming;
}

std::chrono::steady_clock::time_point PerformanceProfiler::getCurrentTime() const {
    return std::chrono::steady_clock::now();
}

void PerformanceProfiler::updateFPS() {
    auto currentTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration<double>(currentTime - pImpl->lastFrameTime);
    
    if (deltaTime.count() > 0.0) {
        pImpl->fps = 1.0 / deltaTime.count();
        pImpl->frameTimestamps.push_back(currentTime);
        pImpl->lastFrameTime = currentTime;
        
        // Update average FPS
        if (pImpl->frameTimestamps.size() > 1) {
            double totalTime = 0.0;
            for (size_t i = 1; i < pImpl->frameTimestamps.size(); ++i) {
                totalTime += std::chrono::duration<double>(pImpl->frameTimestamps[i] - pImpl->frameTimestamps[i-1]).count();
            }
            pImpl->averageFPS = (pImpl->frameTimestamps.size() - 1) / totalTime;
        }
        
        // Update min/max FPS
        pImpl->minFPS = std::min(pImpl->minFPS, pImpl->fps);
        pImpl->maxFPS = std::max(pImpl->maxFPS, pImpl->fps);
    }
}

double PerformanceProfiler::getFPS() const {
    return pImpl->fps;
}

std::vector<std::string> PerformanceProfiler::getBottleneckSections(double threshold) const {
    std::vector<std::string> bottlenecks;
    double totalTime = getTotalExecutionTime();
    
    for (const auto& pair : pImpl->profileData) {
        const ProfileData& data = pair.second;
        if (data.averageTime > 0.0 && data.averageTime / totalTime > threshold) {
            bottlenecks.push_back(data.sectionName);
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceProfiler::getTopSlowSections(size_t count) const {
    std::vector<std::pair<std::string, double>> sections;
    
    for (const auto& pair : pImpl->profileData) {
        const ProfileData& data = pair.second;
        sections.emplace_back(pair.first, data.averageTime);
    }
    
    // Sort by average time (descending)
    std::sort(sections.begin(), sections.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top N
    std::vector<std::string> topSections;
    size_t limit = std::min(count, sections.size());
    for (size_t i = 0; i < limit; ++i) {
        topSections.push_back(sections[i].first);
    }
    
    return topSections;
}

void PerformanceProfiler::startRealTimeMonitoring() {
    pImpl->realTimeMonitoring = true;
    NLM_LOG_INFO("PerformanceProfiler: Started real-time monitoring");
}

void PerformanceProfiler::stopRealTimeMonitoring() {
    pImpl->realTimeMonitoring = false;
    NLM_LOG_INFO("PerformanceProfiler: Stopped real-time monitoring");
}

PerformanceProfiler::RealTimeStats PerformanceProfiler::getRealTimeStats() const {
    return pImpl->realTimeStats;
}

} // namespace nlm