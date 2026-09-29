#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <thread>
#include <sstream>
#include <iomanip>
#include <stacktrace>

namespace nlm {

class PerformanceProfiler {
private:
    struct ProfileData {
        std::string functionName;
        std::chrono::duration<double, std::nano> durationNs;
        std::chrono::duration<double, std::nano> cpuTimeNs;
        std::chrono::duration<double, std::nano> wallTimeNs;
        int threadId;
        int64_t memoryDeltaBytes;
        std::string callStack;
        std::chrono::duration<double, std::nano> timestamp;
        std::unordered_map<std::string, std::string> metadata;
    };

    std::vector<ProfileData> profileData_;
    std::mutex profileMutex_;
    bool captureStackTrace_ = false;
    bool trackMemory_ = false;
    bool threadAware_ = false;

public:
    PerformanceProfiler() = default;

    // High-resolution timing
    template<typename Func>
    auto profile(const std::string& operation, Func&& func) {
        auto startWall = std::chrono::high_resolution_clock::now();
        auto startCpu = std::chrono::steady_clock::now();
        
        // Record thread info
        std::thread::id currentThread = std::this_thread::get_id();
        
        // Execute function
        auto result = func();
        
        auto endWall = std::chrono::high_resolution_clock::now();
        auto endCpu = std::chrono::steady_clock::now();
        
        // Calculate durations
        auto wallDuration = std::chrono::duration<double, std::nano>(endWall - startWall);
        auto cpuDuration = std::chrono::duration<double, std::nano>(endCpu - startCpu);
        
        // Record profile data
        ProfileData data;
        data.functionName = operation;
        data.wallTimeNs = wallDuration;
        data.cpuTimeNs = cpuDuration;
        data.durationNs = wallDuration; // Default to wall time
        data.threadId = std::hash<std::thread::id>{}(currentThread);
        data.memoryDeltaBytes = 0; // TODO: Implement memory tracking
        data.callStack = captureStackTrace_ ? getStackTrace() : "";
        data.timestamp = std::chrono::duration<double, std::nano>(std::chrono::high_resolution_clock::now().time_since_epoch());
        
        {
            std::lock_guard<std::mutex> lock(profileMutex_);
            profileData_.push_back(std::move(data));
        }
        
        return result;
    }

    // Manual timing with call stack
    class ManualProfiler {
    private:
        std::string operationName_;
        std::chrono::high_resolution_clock::time_point startTime_;
        std::chrono::steady_clock::time_point cpuStartTime_;
        std::string componentName_;
        bool completed_ = false;
        int64_t initialMemory_ = 0;

    public:
        ManualProfiler(const std::string& operation, const std::string& component = "core")
            : operationName_(operation), componentName_(component) {
            startTime_ = std::chrono::high_resolution_clock::now();
            cpuStartTime_ = std::chrono::steady_clock::now();
            if (trackMemory_) {
                // TODO: Get current memory usage
                initialMemory_ = 0;
            }
        }

        ~ManualProfiler() {
            if (!completed_) {
                complete();
            }
        }

        void complete() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto endCpu = std::chrono::steady_clock::now();
            
            auto wallDuration = std::chrono::duration<double, std::nano>(endTime - startTime_);
            auto cpuDuration = std::chrono::duration<double, std::nano>(endCpu - cpuStartTime_);
            
            // Record profile data
            ProfileData data;
            data.functionName = operationName_;
            data.wallTimeNs = wallDuration;
            data.cpuTimeNs = cpuDuration;
            data.durationNs = wallDuration;
            data.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
            data.memoryDeltaBytes = trackMemory_ ? 0 : 0; // TODO: Calculate memory delta
            data.callStack = captureStackTrace_ ? getStackTrace() : "";
            data.timestamp = std::chrono::duration<double, std::nano>(std::chrono::high_resolution_clock::now().time_since_epoch());
            data.metadata["component"] = componentName_;
            
            PerformanceProfiler& profiler = getInstance();
            profiler.recordProfileData(std::move(data));
            
            completed_ = true;
        }

        void addMetadata(const std::string& key, const std::string& value) {
            if (!metadata_) metadata_ = std::make_unique<std::unordered_map<std::string, std::string>>();
            metadata_->operator[](key) = value;
        }

    private:
        std::unique_ptr<std::unordered_map<std::string, std::string>> metadata_;
    };

    ManualProfiler createManualProfile(const std::string& operation, const std::string& component = "core") {
        return ManualProfiler(operation, component);
    }

    // Configuration
    void setCaptureStackTrace(bool enable) {
        captureStackTrace_ = enable;
    }

    void setTrackMemory(bool enable) {
        trackMemory_ = enable;
    }

    void setThreadAware(bool enable) {
        threadAware_ = enable;
    }

    // Data access
    std::vector<ProfileData> getProfileData(const std::string& functionName = "",
                                          std::chrono::duration<double, std::nano> range = std::chrono::duration<double, std::nano>(3600000000000.0)) {
        std::lock_guard<std::mutex> lock(profileMutex_);
        std::vector<ProfileData> result;
        
        auto endTime = std::chrono::duration<double, std::nano>(std::chrono::high_resolution_clock::now().time_since_epoch());
        auto startTime = endTime - range;
        
        for (const auto& data : profileData_) {
            if (!functionName.empty() && data.functionName != functionName) continue;
            if (data.timestamp >= startTime) {
                result.push_back(data);
            }
        }
        return result;
    }

    double getAverageWallTime(const std::string& functionName) {
        auto data = getProfileData(functionName);
        if (data.empty()) return 0.0;
        
        double totalNs = 0.0;
        for (const auto& d : data) {
            totalNs += d.wallTimeNs.count();
        }
        return totalNs / data.size() / 1000000.0; // Convert to milliseconds
    }

    double getAverageCpuTime(const std::string& functionName) {
        auto data = getProfileData(functionName);
        if (data.empty()) return 0.0;
        
        double totalNs = 0.0;
        for (const auto& d : data) {
            totalNs += d.cpuTimeNs.count();
        }
        return totalNs / data.size() / 1000000.0; // Convert to milliseconds
    }

    double getAverageMemoryUsage(const std::string& functionName) {
        auto data = getProfileData(functionName);
        if (data.empty()) return 0.0;
        
        int64_t totalBytes = 0;
        for (const auto& d : data) {
            totalBytes += d.memoryDeltaBytes;
        }
        return static_cast<double>(totalBytes) / data.size();
    }

    // Statistics
    std::string generateProfileReport() {
        std::stringstream ss;
        ss << "Performance Profile Report\n";
        ss << "==========================\n\n";
        
        std::unordered_map<std::string, std::vector<ProfileData>> functionProfiles;
        {
            std::lock_guard<std::mutex> lock(profileMutex_);
            functionProfiles = organizeByFunctionName(profileData_);
        }
        
        for (const auto& [functionName, data] : functionProfiles) {
            ss << "Function: " << functionName << "\n";
            ss << "  Count: " << data.size() << "\n";
            ss << "  Avg Wall Time (ms): " << getAverageWallTime(functionName) << "\n";
            ss << "  Avg CPU Time (ms): " << getAverageCpuTime(functionName) << "\n";
            ss << "  Avg Memory Delta (bytes): " << getAverageMemoryUsage(functionName) << "\n";
            ss << "  Total Wall Time (ms): " << getTotalWallTime(functionName) << "\n";
            ss << "  Total CPU Time (ms): " << getTotalCpuTime(functionName) << "\n";
            ss << "\n";
        }
        
        return ss.str();
    }

private:
    void recordProfileData(ProfileData&& data) {
        std::lock_guard<std::mutex> lock(profileMutex_);
        profileData_.push_back(std::move(data));
    }

    std::string getStackTrace() {
        std::stacktrace trace;
        std::stringstream ss;
        ss << trace;
        return ss.str();
    }

    std::unordered_map<std::string, std::vector<ProfileData>> organizeByFunctionName(
        const std::vector<ProfileData>& data) {
        std::unordered_map<std::string, std::vector<ProfileData>> result;
        for (const auto& d : data) {
            result[d.functionName].push_back(d);
        }
        return result;
    }

    double getTotalWallTime(const std::string& functionName) {
        auto data = getProfileData(functionName);
        double totalMs = 0.0;
        for (const auto& d : data) {
            totalMs += d.wallTimeNs.count() / 1000000.0;
        }
        return totalMs;
    }

    double getTotalCpuTime(const std::string& functionName) {
        auto data = getProfileData(functionName);
        double totalMs = 0.0;
        for (const auto& d : data) {
            totalMs += d.cpuTimeNs.count() / 1000000.0;
        }
        return totalMs;
    }

    // Singleton pattern
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
};

} // namespace nlm
