#include "MemoryProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace nlm {

// Global allocation tracking
static std::unordered_map<void*, AllocationTracker> allocationMap_;
static std::mutex allocationMapMutex_;

MemoryProfiler::MemoryProfiler() : isRunning_(false), currentMemoryUsage_(0), peakMemoryUsage_(0) {
    lastMonitoringTime_ = std::chrono::system_clock::now();
}

MemoryProfiler::~MemoryProfiler() {
    stop();
}

void MemoryProfiler::start() {
    if (!isRunning_) {
        isRunning_ = true;
        updateMemoryUsage();
        
        // Start monitoring thread
        monitoringThread_ = std::thread(&MemoryProfiler::monitorMemory, this);
        
        NLM_LOG_INFO("Memory profiler started");
    }
}

void MemoryProfiler::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        // Record final allocations as completed
        {
            std::lock_guard<std::mutex> lock(allocationMutex_);
            for (auto& allocation : activeAllocations_) {
                allocation.isAllocated = false;
                allocation.deallocationTime = std::chrono::system_clock::now();
                completedAllocations_.push_back(allocation);
            }
            activeAllocations_.clear();
        }
        
        // Detect leaks
        detectMemoryLeaks();
        
        NLM_LOG_INFO("Memory profiler stopped");
    }
}

bool MemoryProfiler::isRunning() const {
    return isRunning_.load();
}

void MemoryProfiler::startAllocationTracking(const std::string& component, const std::string& file, int line) {
    // Record allocation in memory profiler
    recordAllocation(component, 0, file, line);
    
    // Also track in global allocator
    void* ptr = memoryAllocWrapper(0, component, file, line);
    
    std::lock_guard<std::mutex> lock(allocationMapMutex_);
    allocationMap_[ptr] = AllocationTracker(component, file, line, 0);
}

void MemoryProfiler::endAllocationTracking(const std::string& component, const std::string& file, int line) {
    // Find and end the allocation
    std::lock_guard<std::mutex> lock(allocationMapMutex_);
    
    for (auto it = allocationMap_.begin(); it != allocationMap_.end(); ++it) {
        if (it->second.component == component && 
            it->second.file == file && 
            it->second.line == line && 
            it->second.isAllocated) {
            
            it->second.isAllocated = false;
            it->second.deallocationTime = std::chrono::system_clock::now();
            break;
        }
    }
}

void MemoryProfiler::recordAllocation(const std::string& component, size_t size, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    
    if (size == 0) {
        // This is a tracking record, add to active allocations
        activeAllocations_.push_back(AllocationTracker(component, file, line, 0));
    } else {
        // This is a real allocation
        currentMemoryUsage_ += size;
        peakMemoryUsage_ = std::max(peakMemoryUsage_, currentMemoryUsage_);
        
        // Update profile data
        if (!profiles_.empty()) {
            auto& latestProfile = profiles_.back();
            latestProfile.totalAllocations++;
            latestProfile.currentMemoryUsage = currentMemoryUsage_;
            latestProfile.allocationsByComponent[component] += size;
        }
    }
}

void MemoryProfiler::recordDeallocation(const std::string& component, size_t size, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    
    if (size == 0) {
        // This is a tracking record, remove from active allocations
        for (auto it = activeAllocations_.begin(); it != activeAllocations_.end(); ++it) {
            if (it->component == component && 
                it->file == file && 
                it->line == line) {
                completedAllocations_.push_back(*it);
                it = activeAllocations_.erase(it);
                break;
            }
        }
    } else {
        // This is a real deallocation
        currentMemoryUsage_ = std::max(0zu, currentMemoryUsage_ - size);
        
        // Update profile data
        if (!profiles_.empty()) {
            auto& latestProfile = profiles_.back();
            latestProfile.totalDeallocations++;
            latestProfile.currentMemoryUsage = currentMemoryUsage_;
            latestProfile.deallocationsByComponent[component] += size;
        }
    }
}

MemoryProfile MemoryProfiler::createProfile(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(profileMutex_);
    
    MemoryProfile profile(name, description);
    profile.totalAllocations = std::accumulate(activeAllocations_.begin(), activeAllocations_.end(), 0,
        [](size_t sum, const AllocationTracker& alloc) { return sum + alloc.size; });
    profile.totalDeallocations = std::accumulate(completedAllocations_.begin(), completedAllocations_.end(), 0,
        [](size_t sum, const AllocationTracker& alloc) { return sum + alloc.size; });
    profile.netAllocation = profile.totalAllocations - profile.totalDeallocations;
    profile.currentMemoryUsage = currentMemoryUsage_;
    profile.peakMemoryUsage = peakMemoryUsage_;
    profile.allocations = activeAllocations_;
    
    profiles_.push_back(profile);
    
    // Clean up old profiles
    if (profiles_.size() > config_.maxTrackedAllocations) {
        profiles_.erase(profiles_.begin(), profiles_.begin() + (profiles_.size() - config_.maxTrackedAllocations));
    }
    
    NLM_LOG_INFO("Created memory profile: " + name);
    
    return profile;
}

std::vector<MemoryProfile> MemoryProfiler::getProfiles() const {
    std::lock_guard<std::mutex> lock(profileMutex_);
    return profiles_;
}

MemoryProfile MemoryProfiler::getLatestProfile() const {
    std::lock_guard<std::mutex> lock(profileMutex_);
    return profiles_.empty() ? MemoryProfile("Empty", "No profiles recorded") : profiles_.back();
}

std::vector<MemoryLeakInfo> MemoryProfiler::getDetectedLeaks() const {
    std::lock_guard<std::mutex> lock(profileMutex_);
    return detectedLeaks_;
}

void MemoryProfiler::setConfig(const MemoryProfilingConfig& config) {
    config_ = config;
}

MemoryProfilingConfig MemoryProfiler::getConfig() const {
    return config_;
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return currentMemoryUsage_;
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return peakMemoryUsage_;
}

void MemoryProfiler::updateMemoryUsage() {
    // Simulate memory usage calculation
    // In real implementation, would use platform-specific memory monitoring APIs
    static size_t currentUsage = 0;
    
    // Simulate memory usage variation
    currentUsage = 50 * 1024 * 1024 + (rand() % 100 * 1024 * 1024); // 50MB - 150MB
    currentMemoryUsage_ = currentUsage;
    peakMemoryUsage_ = std::max(peakMemoryUsage_, currentMemoryUsage_);
}

void MemoryProfiler::detectMemoryLeaks() {
    if (!config_.enableMemoryLeakDetection) return;
    
    std::lock_guard<std::mutex> lock(allocationMutex_);
    
    // Analyze active allocations for leaks
    analyzeLeaks(activeAllocations_);
    
    // Analyze completed allocations for suspicious patterns
    analyzeLeaks(completedAllocations_);
}

void MemoryProfiler::analyzeLeaks(const std::vector<AllocationTracker>& allocations) {
    for (const auto& allocation : allocations) {
        if (allocation.isAllocated) {
            // This allocation was never deallocated - potential leak
            size_t leakedSize = allocation.size > 0 ? allocation.size : 1024; // Assume 1KB if unknown size
            
            if (leakedSize >= config_.memoryLeakThreshold) {
                MemoryLeakInfo leak(allocation.component, allocation.file, allocation.line, leakedSize);
                detectedLeaks_.push_back(leak);
            }
        }
    }
}

void MemoryProfiler::monitorMemory() {
    while (isRunning_) {
        updateMemoryUsage();
        
        // Check for leaks periodically
        if (++monitoringCount_ % 10 == 0) { // Every 10th iteration
            detectMemoryLeaks();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.samplingIntervalMs));
    }
}

std::string MemoryProfiler::generateHtmlReport() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>NLM Memory Profile Report</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
    oss << "        .profile { margin: 20px 0; padding: 20px; background-color: #f9f9f9; border-radius: 5px; }" << std::endl;
    oss << "        .leak { border-left: 4px solid #ff0000; }" << std::endl;
    oss << "        table { border-collapse: collapse; width: 100%; margin: 10px 0; }" << std::endl;
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "        th { background-color: #f2f2f2; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    
    oss << "    <div class=\"header\">" << std::endl;
    oss << "        <h1>NLM Memory Profile Report</h1>" << std::endl;
    oss << "        <p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>Current Memory Usage: " << currentMemoryUsage_ / 1024 << " KB</h2>" << std::endl;
    oss << "    <h2>Peak Memory Usage: " << peakMemoryUsage_ / 1024 << " KB</h2>" << std::endl;
    
    oss << "    <h2>Memory Profiles</h2>" << std::endl;
    for (const auto& profile : profiles_) {
        oss << "        <div class=\"profile\">" << std::endl;
        oss << "            <h3>" << profile.profileName << "</h3>" << std::endl;
        oss << "            <p>" << profile.description << "</p>" << std::endl;
        oss << "            <p>Timestamp: " << getProfileTimestamp(profile.timestamp) << "</p>" << std::endl;
        oss << "            <p>Total Allocations: " << profile.totalAllocations << "</p>" << std::endl;
        oss << "            <p>Total Deallocations: " << profile.totalDeallocations << "</p>" << std::endl;
        oss << "            <p>Net Allocation: " << profile.netAllocation << " bytes</p>" << std::endl;
        oss << "            <p>Current Usage: " << profile.currentMemoryUsage << " bytes</p>" << std::endl;
        oss << "            <p>Peak Usage: " << profile.peakMemoryUsage << " bytes</p>" << std::endl;
        oss << "        </div>" << std::endl;
    }
    
    oss << "    <h2>Detected Memory Leaks</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Component</th><th>File</th><th>Line</th><th>Leaked Size (bytes)</th><th>Detected Time</th></tr>" << std::endl;
    
    for (const auto& leak : detectedLeaks_) {
        oss << "        <tr class=\"leak\">" << std::endl;
        oss << "            <td>" << leak.component << "</td>" << std::endl;
        oss << "            <td>" << leak.file << "</td>" << std::endl;
        oss << "            <td>" << leak.line << "</td>" << std::endl;
        oss << "            <td>" << leak.leakedSize << "</td>" << std::endl;
        oss << "            <td>" << getProfileTimestamp(leak.detectedTime) << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string MemoryProfiler::generateJsonReport() const {
    std::ostringstream oss;
    
    oss << "{
    \"memoryReport\": {
        \"timestamp\": \"" << getCurrentTimestamp() << "\",
        \"currentMemoryUsage\": " << currentMemoryUsage_ << ",
        \"peakMemoryUsage\": " << peakMemoryUsage_ << ",
        \"profiles\": [";
    
    for (size_t i = 0; i < profiles_.size(); ++i) {
        const auto& profile = profiles_[i];
        oss << "{
            \"name\": \"" << profile.profileName << "\",
            \"description\": \"" << profile.description << "\",
            \"timestamp\": \"" << getProfileTimestamp(profile.timestamp) << "\",
            \"totalAllocations\": " << profile.totalAllocations << ",
            \"totalDeallocations\": " << profile.totalDeallocations << ",
            \"netAllocation\": " << profile.netAllocation << ",
            \"currentMemoryUsage\": " << profile.currentMemoryUsage << ",
            \"peakMemoryUsage\": " << profile.peakMemoryUsage
        }";
        
        if (i < profiles_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "],
        \"detectedLeaks\": [";
    
    for (size_t i = 0; i < detectedLeaks_.size(); ++i) {
        const auto& leak = detectedLeaks_[i];
        oss << "{
            \"component\": \"" << leak.component << "\",
            \"file\": \"" << leak.file << "\",
            \"line\": " << leak.line << ",
            \"leakedSize\": " << leak.leakedSize << ",
            \"detectedTime\": \"" << getProfileTimestamp(leak.detectedTime) << "\"
        }";
        
        if (i < detectedLeaks_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]
    }
}";
    
    return oss.str();
}

std::string MemoryProfiler::generateTextReport() const {
    std::ostringstream oss;
    
    oss << "NLM Memory Profile Report" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Current Memory Usage: " << currentMemoryUsage_ / 1024 << " KB" << std::endl;
    oss << "Peak Memory Usage: " << peakMemoryUsage_ / 1024 << " KB" << std::endl;
    oss << std::endl;
    
    oss << "Memory Profiles:" << std::endl;
    for (const auto& profile : profiles_) {
        oss << "  - " << profile.profileName << ": " << profile.description << std::endl;
        oss << "    Timestamp: " << getProfileTimestamp(profile.timestamp) << std::endl;
        oss << "    Allocations: " << profile.totalAllocations << std::endl;
        oss << "    Deallocations: " << profile.totalDeallocations << std::endl;
        oss << "    Net: " << profile.netAllocation << " bytes" << std::endl;
        oss << std::endl;
    }
    
    oss << "Detected Memory Leaks:" << std::endl;
    if (detectedLeaks_.empty()) {
        oss << "  None detected." << std::endl;
    } else {
        for (const auto& leak : detectedLeaks_) {
            oss << "  - " << leak.component << " at " << leak.file << ":" << leak.line 
                << " (" << leak.leakedSize << " bytes)" << std::endl;
            oss << "    Detected: " << getProfileTimestamp(leak.detectedTime) << std::endl;
        }
    }
    
    return oss.str();
}

std::string MemoryProfiler::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string MemoryProfiler::getProfileTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

void* memoryAllocWrapper(size_t size, const std::string& component, const std::string& file, int line) {
    void* ptr = std::malloc(size);
    if (ptr) {
        std::lock_guard<std::mutex> lock(allocationMapMutex_);
        allocationMap_[ptr] = AllocationTracker(component, file, line, size);
    }
    return ptr;
}

void memoryDeallocWrapper(void* ptr, const std::string& component, const std::string& file, int line) {
    if (ptr) {
        std::lock_guard<std::mutex> lock(allocationMapMutex_);
        auto it = allocationMap_.find(ptr);
        if (it != allocationMap_.end()) {
            it->second.isAllocated = false;
            it->second.deallocationTime = std::chrono::system_clock::now();
            allocationMap_.erase(it);
        }
        std::free(ptr);
    }
}