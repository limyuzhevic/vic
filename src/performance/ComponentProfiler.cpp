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
#include <stack>
#include <unordered_map>

namespace nlm {

struct ComponentProfiler::Impl {
    std::atomic<bool> enabled{false};
    std::mutex dataMutex;
    
    std::map<std::string, ProfileData> profiles;
    std::map<std::string, std::stack<std::chrono::high_resolution_clock::time_point>> activeProfiles;
    std::thread samplingThread;
    double samplingInterval = 0.1; // seconds
    bool trackMemory = false;
};

ComponentProfiler::ComponentProfiler() : pImpl(std::make_unique<Impl>()) {
}

ComponentProfiler::~ComponentProfiler() {
    if (pImpl->enabled) {
        stopProfiling();
    }
}

void ComponentProfiler::startProfiling(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->enabled) return;
    
    profiles[componentName].componentName = componentName;
    activeProfiles[componentName].push(std::chrono::high_resolution_clock::now());
    
    // Increment call count
    profiles[componentName].callCount++;
}

void ComponentProfiler::stopProfiling(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!pImpl->enabled) return;
    
    auto it = activeProfiles.find(componentName);
    if (it != activeProfiles.end() && !it->second.empty()) {
        auto startTime = it->second.top();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        double durationSeconds = duration.count() * 1e-9;
        
        ProfileData& data = profiles[componentName];
        data.totalTime += durationSeconds;
        data.minTime = std::min(data.minTime, durationSeconds);
        data.maxTime = std::max(data.maxTime, durationSeconds);
        
        it->second.pop();
        
        // Record in cumulative time
        if (!it->second.empty()) {
            data.cumulativeTime += durationSeconds;
        }
    }
}

void ComponentProfiler::pauseProfiling(const std::string& componentName) {
    // Implementation would track pause time
}

void ComponentProfiler::resumeProfiling(const std::string& componentName) {
    // Implementation would resume timing from pause point
}

ComponentProfiler::ProfileData ComponentProfiler::getProfileData(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    auto it = profiles.find(componentName);
    if (it != profiles.end()) {
        return it->second;
    }
    return ProfileData();
}

std::vector<std::string> ComponentProfiler::getProfiledComponents() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::string> componentNames;
    for (const auto& pair : profiles) {
        componentNames.push_back(pair.first);
    }
    return componentNames;
}

std::vector<ComponentProfiler::ProfileData> ComponentProfiler::getAllProfileData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<ProfileData> result;
    for (const auto& pair : profiles) {
        result.push_back(pair.second);
    }
    return result;
}

void ComponentProfiler::clearProfile(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    profiles.erase(componentName);
    activeProfiles.erase(componentName);
}

void ComponentProfiler::clearAllProfiles() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    profiles.clear();
    activeProfiles.clear();
}

void ComponentProfiler::saveProfiles(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filepath);
    }
    
    file << "Component Profiling Data" << std::endl;
    file << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    file << std::endl;
    
    for (const auto& pair : profiles) {
        const ProfileData& data = pair.second;
        file << "Component: " << data.componentName << std::endl;
        file << "  Call Count: " << data.callCount << std::endl;
        file << "  Total Time: " << data.totalTime << " seconds" << std::endl;
        file << "  Average Time: " << data.averageTime << " seconds" << std::endl;
        file << "  Min Time: " << data.minTime << " seconds" << std::endl;
        file << "  Max Time: " << data.maxTime << " seconds" << std::endl;
        file << "  Cumulative Time: " << data.cumulativeTime << " seconds" << std::endl;
        file << "  Memory Allocated: " << data.memoryAllocated << " bytes" << std::endl;
        file << "  Memory Freed: " << data.memoryFreed << " bytes" << std::endl;
        file << std::endl;
    }
}

void ComponentProfiler::loadProfiles(const std::string& filepath) {
    // Implementation would load from file
}

std::string ComponentProfiler::generateProfileReport() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::stringstream ss;
    ss << "Component Profiling Report" << std::endl;
    ss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    ss << std::endl;
    
    ss << "Performance Summary:" << std::endl;
    for (const auto& pair : profiles) {
        const ProfileData& data = pair.second;
        ss << "  " << data.componentName << ": " 
           << data.callCount << " calls, " 
           << data.totalTime << " total time, "
           << data.averageTime << " avg time" << std::endl;
    }
    
    return ss.str();
}

std::string ComponentProfiler::findSlowestComponents(size_t topN) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::pair<std::string, double>> componentTimes;
    
    for (const auto& pair : profiles) {
        const ProfileData& data = pair.second;
        componentTimes.push_back(std::make_pair(data.componentName, data.maxTime));
    }
    
    // Sort by maximum time (slowest first)
    std::sort(componentTimes.begin(), componentTimes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::stringstream ss;
    ss << "Slowest Components (top " << topN << "):" << std::endl;
    for (size_t i = 0; i < std::min(topN, componentTimes.size()); ++i) {
        ss << "  " << (i + 1) << ". " << componentTimes[i].first << ": " 
           << componentTimes[i].second << " seconds (max)" << std::endl;
    }
    
    return ss.str();
}

std::string ComponentProfiler::findMostCalledComponents(size_t topN) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    std::vector<std::pair<std::string, uint64_t>> componentCounts;
    
    for (const auto& pair : profiles) {
        const ProfileData& data = pair.second;
        componentCounts.push_back(std::make_pair(data.componentName, data.callCount));
    }
    
    // Sort by call count (most called first)
    std::sort(componentCounts.begin(), componentCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::stringstream ss;
    ss << "Most Called Components (top " << topN << "):" << std::endl;
    for (size_t i = 0; i < std::min(topN, componentCounts.size()); ++i) {
        ss << "  " << (i + 1) << ". " << componentCounts[i].first << ": " 
           << componentCounts[i].second << " calls" << std::endl;
    }
    
    return ss.str();
}

double ComponentProfiler::getTotalProfilingTime() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    double totalTime = 0.0;
    for (const auto& pair : profiles) {
        totalTime += pair.second.totalTime;
    }
    return totalTime;
}

void ComponentProfiler::setProfilerEnabled(bool enable) {
    if (enable && !pImpl->enabled) {
        startProfiling();
    } else if (!enable && pImpl->enabled) {
        stopProfiling();
    }
    pImpl->enabled = enable;
}

void ComponentProfiler::startProfiling() {
    pImpl->enabled = true;
    
    // Start sampling thread if not already running
    if (!pImpl->samplingThread.joinable()) {
        pImpl->samplingThread = std::thread([this]() {
            while (pImpl->enabled) {
                try {
                    std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->samplingInterval));
                } catch (const std::exception& e) {
                    std::cerr << "Component profiler sampling error: " << e.what() << std::endl;
                }
            }
        });
    }
}

void ComponentProfiler::stopProfiling() {
    pImpl->enabled = false;
    if (pImpl->samplingThread.joinable()) {
        pImpl->samplingThread.join();
    }
}

void ComponentProfiler::setSamplingInterval(double intervalSeconds) {
    pImpl->samplingInterval = intervalSeconds;
}

void ComponentProfiler::enableMemoryTracking(bool enable) {
    pImpl->trackMemory = enable;
}

bool ComponentProfiler::isProfiling(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return activeProfiles.find(componentName) != activeProfiles.end() && 
           !activeProfiles.at(componentName).empty();
}

bool ComponentProfiler::isEnabled() const {
    return pImpl->enabled;
}

void ComponentProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    profiles.clear();
    activeProfiles.clear();
}

} // namespace nlm
