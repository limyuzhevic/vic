#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Timing utilities
class TimingUtils {
public:
    // High-resolution timing
    static double now() {
        return std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
    
    // Microsecond precision
    static long long nowMicroseconds() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
    
    // Nanosecond precision  
    static long long nowNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
    
    // Timer class for measuring intervals
    class Timer {
    public:
        Timer() : start_(now()), elapsed_(0.0) {}
        
        void reset() {
            start_ = now();
            elapsed_ = 0.0;
        }
        
        double elapsed() const {
            return now() - start_;
        }
        
        double stop() {
            double current = now();
            elapsed_ = current - start_;
            return elapsed_;
        }
        
        operator double() const {
            return elapsed();
        }
        
    private:
        double start_;
        double elapsed_;
    };

// Benchmark result storage
struct BenchmarkResult {
    std::string name;
    size_t neurons = 0;
    size_t synapses = 0;
    size_t steps = 0;
    double totalTime = 0.0;      // seconds
    double stepsPerSecond = 0.0;
    double spikesPerStep = 0.0;
    double synapsesPerNeuron = 0.0;
    double memoryUsageMB = 0.0;
    std::string timestamp;
    
    // Statistics
    double getDuration() const { return totalTime; }
    double getThroughput() const { return stepsPerSecond; }
    double getEfficiency() const { return neurons / totalTime; }
    
    // Serialization
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\": \"" << name << "\", \"neurons\": " << neurons
           << ", \"synapses\": " << synapses
           << ", \"steps\": " << steps
           << ", \"totalTime\": " << std::fixed << std::setprecision(4) << totalTime
           << ", \"stepsPerSecond\": " << std::fixed << std::setprecision(2) << stepsPerSecond
           << ", \"spikesPerStep\": " << std::fixed << std::setprecision(2) << spikesPerStep
           << ", \"synapsesPerNeuron\": " << std::fixed << std::setprecision(2) << synapsesPerNeuron
           << ", \"memoryUsageMB\": " << std::fixed << std::setprecision(2) << memoryUsageMB
           << ", \"timestamp\": \"" << timestamp << "\"}";
        return ss.str();
    }
};

// Memory tracker
class MemoryTracker {
public:
    struct MemoryInfo {
        size_t totalBytes = 0;
        size_t usedBytes = 0;
        size_t freeBytes = 0;
        double usagePercent = 0.0;
        std::string timestamp;
    };
    
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    
    // Get current memory usage
    static MemoryInfo getMemoryInfo() {
        MemoryInfo info;
        // Simplified memory tracking - in real implementation would use system APIs
        info.totalBytes = 1024 * 1024 * 1024; // 1GB (placeholder)
        info.usedBytes = info.totalBytes * 0.6; // 60% used (placeholder)
        info.freeBytes = info.totalBytes - info.usedBytes;
        info.usagePercent = (info.usedBytes / (double)info.totalBytes) * 100.0;
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        info.timestamp = ss.str();
        
        return info;
    }
    
    // Track memory over time
    void startTracking(const std::string& name) {
        trackingSessions_[name] = nowNanoseconds();
    }
    
    double stopTracking(const std::string& name) {
        auto it = trackingSessions_.find(name);
        if (it != trackingSessions_.end()) {
            double duration = (nowNanoseconds() - it->second) / 1e9; // Convert to seconds
            trackingSessions_.erase(it);
            return duration;
        }
        return 0.0;
    }
    
private:
    std::unordered_map<std::string, long long> trackingSessions_;
};

// Simple profiler
class Profiler {
public:
    struct ProfileData {
        std::string functionName;
        double totalTime = 0.0;
        size_t callCount = 0;
        double averageTime = 0.0;
        double minTime = 0.0;
        double maxTime = 0.0;
        double timePercentage = 0.0;
    };
    
    Profiler() = default;
    ~Profiler() = default;
    
    // Start profiling a function/block
    void startProfile(const std::string& name) {
        profileSessions_[name] = nowNanoseconds();
    }
    
    // Stop profiling and record results
    ProfileData stopProfile(const std::string& name) {
        ProfileData data;
        auto it = profileSessions_.find(name);
        if (it != profileSessions_.end()) {
            long long startNs = it->second;
            long long endNs = nowNanoseconds();
            double duration = (endNs - startNs) / 1e9; // Convert to seconds
            
            data.functionName = name;
            data.totalTime = duration;
            data.callCount = 1;
            data.averageTime = duration;
            data.minTime = duration;
            data.maxTime = duration;
            
            // Update accumulated data
            auto existingIt = accumulatedProfiles_.find(name);
            if (existingIt != accumulatedProfiles_.end()) {
                ProfileData& existing = existingIt->second;
                existing.totalTime += duration;
                existing.callCount += 1;
                existing.averageTime = existing.totalTime / existing.callCount;
                existing.minTime = std::min(existing.minTime, duration);
                existing.maxTime = std::max(existing.maxTime, duration);
                data = existing;
            } else {
                accumulatedProfiles_[name] = data;
            }
            
            profileSessions_.erase(it);
        }
        return data;
    }
    
    // Get all accumulated profile data
    std::vector<ProfileData> getProfileData() const {
        std::vector<ProfileData> results;
        for (const auto& pair : accumulatedProfiles_) {
            results.push_back(pair.second);
        }
        
        // Calculate percentages
        double totalTime = 0.0;
        for (const auto& pair : accumulatedProfiles_) {
            totalTime += pair.second.totalTime;
        }
        
        for (auto& profile : results) {
            if (totalTime > 0.0) {
                profile.timePercentage = (profile.totalTime / totalTime) * 100.0;
            }
        }
        
        return results;
    }
    
    // Clear all accumulated data
    void clear() {
        accumulatedProfiles_.clear();
        profileSessions_.clear();
    }
    
    // Generate report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "Profile Report" << std::endl;
        ss << "==============" << std::endl;
        
        auto profiles = getProfileData();
        if (profiles.empty()) {
            ss << "No profile data available." << std::endl;
            return ss.str();
        }
        
        ss << std::left << std::setw(30) << "Function" 
           << std::right << std::setw(12) << "Total Time"
           << std::right << std::setw(12) << "Avg Time"
           << std::right << std::setw(12) << "Min Time"
           << std::right << std::setw(12) << "Max Time"
           << std::right << std::setw(12) << "Calls"
           << std::right << std::setw(12) << "% of Total" << std::endl;
        
        ss << std::left << std::setw(30) << "---------"
           << std::right << std::setw(12) << "----------"
           << std::right << std::setw(12) << "----------"
           << std::right << std::setw(12) << "----------"
           << std::right << std::setw(12) << "----------"
           << std::right << std::setw(12) << "----"
           << std::right << std::setw(12) << "------------" << std::endl;
        
        for (const auto& profile : profiles) {
            ss << std::left << std::setw(30) << profile.functionName
               << std::right << std::setw(12) << std::fixed << std::setprecision(4) << profile.totalTime
               << std::right << std::setw(12) << std::fixed << std::setprecision(4) << profile.averageTime
               << std::right << std::setw(12) << std::fixed << std::setprecision(4) << profile.minTime
               << std::right << std::setw(12) << std::fixed << std::setprecision(4) << profile.maxTime
               << std::right << std::setw(12) << profile.callCount
               << std::right << std::setw(12) << std::fixed << std::setprecision(2) << profile.timePercentage << "%" << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::unordered_map<std::string, long long> profileSessions_;
    std::unordered_map<std::string, ProfileData> accumulatedProfiles_;
};

// Core performance monitor
class PerformanceMonitor {
public:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    
    // Start monitoring
    void start(const std::string& name) {
        MonitoringSession session;
        session.name = name;
        session.startTime = now();
        currentSession_ = session;
    }
    
    // Stop monitoring and record results
    BenchmarkResult stop() {
        if (!currentSession_.name.empty()) {
            BenchmarkResult result;
            result.name = currentSession_.name;
            result.totalTime = now() - currentSession_.startTime;
            result.timestamp = getTimestamp();
            
            // Get memory info
            MemoryTracker::MemoryInfo memInfo = MemoryTracker::getMemoryInfo();
            result.memoryUsageMB = memInfo.usedBytes / (1024.0 * 1024.0);
            
            // Store result
            results_.push_back(result);
            
            currentSession_.name.clear();
            return result;
        }
        return BenchmarkResult();
    }
    
    // Get all results
    const std::vector<BenchmarkResult>& getResults() const {
        return results_;
    }
    
    // Clear all results
    void clear() {
        results_.clear();
    }
    
    // Generate summary report
    std::string generateSummary() const {
        std::stringstream ss;\n        ss << "Performance Monitor Summary" << std::endl;
        ss << "===========================" << std::endl;
        ss << "Total sessions: " << results_.size() << std::endl;
        
        if (results_.empty()) {
            ss << "No results available." << std::endl;
            return ss.str();
        }
        
        double totalTime = 0.0;
        for (const auto& result : results_) {
            totalTime += result.totalTime;
        }
        
        ss << "Total time: " << std::fixed << std::setprecision(4) << totalTime << "s" << std::endl;
        ss << "Average session time: " << std::fixed << std::setprecision(4) << (totalTime / results_.size()) << "s" << std::endl;
        
        return ss.str();
    }
    
private:
    struct MonitoringSession {
        std::string name;
        double startTime = 0.0;
    };
    
    std::vector<BenchmarkResult> results_;
    MonitoringSession currentSession_;
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

} // namespace nlm