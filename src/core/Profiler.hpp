// nlm_profiler.h - Performance profiling and debugging tools for NLM
//
// This header provides comprehensive profiling capabilities for NLM executables,
// including performance timing, memory tracking, and detailed execution analysis.

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>

namespace nlm {

class Profiler {
public:
    struct ProfileData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double durationMs;
        uint64_t memoryUsage;
        size_t callCount;
        std::string threadId;
        std::map<std::string, std::string> metadata;
        
        ProfileData(const std::string& n) : name(n), startTime(), endTime(), 
            durationMs(0.0), memoryUsage(0), callCount(0), threadId("unknown") {}
    };
    
    struct Summary {
        double totalTimeMs;
        size_t totalCalls;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        std::vector<ProfileData> topProfiles;
        std::string timestamp;
    };
    
    Profiler() {
        initialize();
    }
    
    void initialize() {
        currentProfiles.clear();
        profileHistory.clear();
        samplingActive = false;
        enableMemoryTracking = false;
    }
    
    void startProfile(const std::string& name, const std::map<std::string, std::string>& metadata = {}) {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        ProfileData data(name);
        data.startTime = std::chrono::high_resolution_clock::now();
        data.metadata = metadata;
        data.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        
        currentProfiles.push_back(data);
        
        if (enableMemoryTracking) {
            trackMemoryUsage(data);
        }
        
        if (verboseLogging) {
            logProfileStart(data);
        }
    }
    
    void stopProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        auto it = std::find_if(currentProfiles.begin(), currentProfiles.end(),
            [&name](const ProfileData& data) { return data.name == name; });n        
        if (it != currentProfiles.end()) {
            it->endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(it->endTime - it->startTime);
            it->durationMs = static_cast<double>(duration.count());
            it->callCount++;
            
            if (enableMemoryTracking) {
                updateMemoryUsage(*it);
            }
            
            if (verboseLogging) {
                logProfileStop(*it);
            }
            
            profileHistory.push_back(*it);
            currentProfiles.erase(it);
        }
    }
    
    void startSampling(size_t intervalMs = 100) {
        samplingActive = true;
        samplingInterval = intervalMs;
        samplerThread = std::thread(&Profiler::samplingWorker, this);
        
        if (verboseLogging) {
            std::cout << "[Profiler] Started sampling with interval " << intervalMs << "ms" << std::endl;
        }
    }
    
    void stopSampling() {
        if (samplingActive) {
            samplingActive = false;
            if (samplerThread.joinable()) {
                samplerThread.join();
            }
            
            if (verboseLogging) {
                std::cout << "[Profiler] Stopped sampling" << std::endl;
            }
        }
    }
    
    void enableMemoryTracking(bool enable) {
        enableMemoryTracking = enable;
        if (verboseLogging) {
            std::cout << "[Profiler] Memory tracking " << (enable ? "enabled" : "disabled") << std::endl;
        }
    }
    
    void setVerboseLogging(bool enable) {
        verboseLogging = enable;
    }
    
    Summary generateSummary() const {
        Summary summary;
        
        // Calculate totals
        summary.totalTimeMs = 0.0;
        summary.totalCalls = 0;
        
        for (const auto& profile : profileHistory) {
            summary.totalTimeMs += profile.durationMs;
            summary.totalCalls += profile.callCount;
        }
        
        if (summary.totalCalls > 0) {
            summary.averageTimeMs = summary.totalTimeMs / summary.totalCalls;
        }
        
        // Find min/max
        if (!profileHistory.empty()) {
            summary.minTimeMs = profileHistory[0].durationMs;
            summary.maxTimeMs = profileHistory[0].durationMs;
            
            for (const auto& profile : profileHistory) {
                summary.minTimeMs = std::min(summary.minTimeMs, profile.durationMs);
                summary.maxTimeMs = std::max(summary.maxTimeMs, profile.durationMs);
            }
        }
        
        // Get top profiles by duration
        std::vector<const ProfileData*> topProfiles;
        for (const auto& profile : profileHistory) {
            topProfiles.push_back(&profile);
        }
        
        std::sort(topProfiles.begin(), topProfiles.end(),
            [](const ProfileData* a, const ProfileData* b) { return a->durationMs > b->durationMs; });
        
        // Copy top 10 profiles
        summary.topProfiles.reserve(std::min(static_cast<size_t>(10), topProfiles.size()));
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), topProfiles.size()); ++i) {
            summary.topProfiles.push_back(*topProfiles[i]);
        }
        
        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        summary.timestamp = ss.str();
        
        return summary;
    }
    
    void saveProfile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open profile file for writing: " << filename << std::endl;
            return;
        }
        
        Summary summary = generateSummary();
        
        file << "NLM Profiler Results" << std::endl;
        file << "=====================" << std::endl;
        file << "Timestamp: " << summary.timestamp << std::endl;
        file << "Total profiles: " << profileHistory.size() << std::endl;
        file << "Total calls: " << summary.totalCalls << std::endl;
        file << std::endl;
        
        file << "Summary:" << std::endl;
        file << "  Total time: " << std::fixed << std::setprecision(2) << summary.totalTimeMs << "ms" << std::endl;
        file << "  Average time: " << std::fixed << std::setprecision(2) << summary.averageTimeMs << "ms" << std::endl;
        file << "  Min time: " << std::fixed << std::setprecision(2) << summary.minTimeMs << "ms" << std::endl;
        file << "  Max time: " << std::fixed << std::setprecision(2) << summary.maxTimeMs << "ms" << std::endl;
        file << std::endl;
        
        file << "Top 10 Profiles:" << std::endl;
        file << std::left << std::setw(20) << "Name" 
              << std::setw(15) << "Duration(ms)" 
              << std::setw(10) << "Calls" 
              << std::setw(15) << "Thread" << std::endl;
        file << std::string(60, '-') << std::endl;
        
        for (const auto& profile : summary.topProfiles) {
            file << std::left << std::setw(20) << profile.name
                 << std::setw(15) << std::fixed << std::setprecision(2) << profile.durationMs
                 << std::setw(10) << profile.callCount
                 << std::setw(15) << profile.threadId << std::endl;
        }
        
        file.close();
        
        if (verboseLogging) {
            std::cout << "[Profiler] Profile saved to: " << filename << std::endl;
        }
    }
    
    void printProfile() const {
        Summary summary = generateSummary();
        
        std::cout << "=== NLM Profiler Results ===" << std::endl;
        std::cout << "Timestamp: " << summary.timestamp << std::endl;
        std::cout << "Total profiles: " << profileHistory.size() << std::endl;
        std::cout << "Total calls: " << summary.totalCalls << std::endl;
        std::cout << std::endl;
        
        std::cout << "Summary Statistics:" << std::endl;
        std::cout << "  Total time: " << std::fixed << std::setprecision(2) << summary.totalTimeMs << "ms" << std::endl;
        std::cout << "  Average time: " << std::fixed << std::setprecision(2) << summary.averageTimeMs << "ms" << std::endl;
        std::cout << "  Min time: " << std::fixed << std::setprecision(2) << summary.minTimeMs << "ms" << std::endl;
        std::cout << "  Max time: " << std::fixed << std::setprecision(2) << summary.maxTimeMs << "ms" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Top 10 Profiles:" << std::endl;
        std::cout << std::left << std::setw(25) << "Name" 
                  << std::setw(15) << "Duration(ms)" 
                  << std::setw(10) << "Calls" 
                  << std::setw(20) << "Thread" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& profile : summary.topProfiles) {
            std::cout << std::left << std::setw(25) << profile.name
                      << std::setw(15) << std::fixed << std::setprecision(2) << profile.durationMs
                      << std::setw(10) << profile.callCount
                      << std::setw(20) << profile.threadId << std::endl;
        }
    }
    
    size_t getProfileCount() const {
        return profileHistory.size();
    }
    
    std::vector<ProfileData> getRecentProfiles(size_t limit = 10) const {
        std::vector<ProfileData> recent;
        size_t start = std::max(static_cast<size_t>(0), profileHistory.size() - limit);
        
        for (size_t i = start; i < profileHistory.size(); ++i) {
            recent.push_back(profileHistory[i]);
        }
        
        return recent;
    }
    
    void clearProfiles() {
        std::lock_guard<std::mutex> lock(profileMutex);
        currentProfiles.clear();
        profileHistory.clear();
        
        if (verboseLogging) {
            std::cout << "[Profiler] Cleared all profiles" << std::endl;
        }
    }
    
    bool isSampling() const {
        return samplingActive;
    }
    
    static Profiler& getInstance() {
        static Profiler instance;
        return instance;
    }
    
    // RAII profile wrapper for convenience
    class ScopedProfile {
    public:
        ScopedProfile(Profiler& profiler, const std::string& name, 
                     const std::map<std::string, std::string>& metadata = {}) 
            : profiler(profiler), name(name), metadata(metadata) {
            profiler.startProfile(name, metadata);
        }
        
        ~ScopedProfile() {
            profiler.stopProfile(name);
        }
        
    private:
        Profiler& profiler;
        std::string name;
        std::map<std::string, std::string> metadata;
    };
    
private:
    void logProfileStart(const ProfileData& data) {
        std::cout << "[Profiler] Started: " << data.name << " (thread: " << data.threadId << ")" << std::endl;
    }
    
    void logProfileStop(const ProfileData& data) {
        std::cout << "[Profiler] Stopped: " << data.name << " (duration: " 
                  << std::fixed << std::setprecision(2) << data.durationMs << "ms, calls: " 
                  << data.callCount << ")" << std::endl;
    }
    
    void trackMemoryUsage(ProfileData& data) {
        // Simple memory tracking - in a real implementation, this would use platform-specific APIs
        data.memoryUsage = getCurrentMemoryUsage();
    }
    
    void updateMemoryUsage(ProfileData& data) {
        // Update memory usage tracking
        data.memoryUsage = getCurrentMemoryUsage();
    }
    
    uint64_t getCurrentMemoryUsage() {
        // Simplified memory tracking - returns a dummy value
        return 0;
    }
    
    void samplingWorker() {
        while (samplingActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(samplingInterval));
            
            // Sample current thread activity
            std::lock_guard<std::mutex> lock(profileMutex);
            
            for (auto& profile : currentProfiles) {
                // Update memory usage if tracking
                if (enableMemoryTracking) {
                    updateMemoryUsage(profile);
                }
            }
        }
    }
    
    std::vector<ProfileData> currentProfiles;
    std::vector<ProfileData> profileHistory;
    std::mutex profileMutex;
    std::thread samplerThread;
    bool samplingActive = false;
    bool enableMemoryTracking = false;
    bool verboseLogging = false;
    size_t samplingInterval = 100;
};