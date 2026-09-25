// Performance Profiling and Analysis for NLM
// Advanced performance measurement and analysis tools

#ifndef NLM_PERFORMANCE_ANALYSIS_H
#define NLM_PERFORMANCE_ANALYSIS_H

#include "core/Diagnostics.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <optional>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

namespace nlm {

// Performance profiling information
struct ProfileInfo {
    std::string name;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::chrono::duration<double> duration;
    std::vector<std::chrono::duration<double>> sampleTimes;
    double cpuTime;              // CPU time in seconds
    double realTime;             // Wall clock time in seconds
    size_t memoryUsed;           // Memory used in bytes
    size_t peakMemory;           // Peak memory used in bytes
    size_t allocations;          // Number of memory allocations
    size_t deallocations;        // Number of memory deallocations
    std::unordered_map<std::string, double> customMetrics;
    std::unordered_map<std::string, std::string> metadata;
    
    ProfileInfo(const std::string& n = "")
        : name(n), cpuTime(0.0), realTime(0.0), memoryUsed(0), 
          peakMemory(0), allocations(0), deallocations(0) {}
    
    void start() {
        startTime = std::chrono::system_clock::now();
        sampleTimes.clear();
        cpuTime = 0.0;
        realTime = 0.0;
        memoryUsed = 0;
        peakMemory = 0;
        allocations = 0;
        deallocations = 0;
    }
    
    void end() {
        endTime = std::chrono::system_clock::now();
        duration = endTime - startTime;
        realTime = duration.count();
    }
    
    void sample() {
        sampleTimes.push_back(std::chrono::duration<double>(
            std::chrono::system_clock::now() - startTime));
    }
    
    double getSamplesPerSecond() const {
        if (sampleTimes.empty() || duration.count() == 0.0) return 0.0;
        return static_cast<double>(sampleTimes.size()) / duration.count();
    }
    
    double getMinSampleTime() const {
        if (sampleTimes.empty()) return 0.0;
        return std::min_element(sampleTimes.begin(), sampleTimes.end()->count();
    }
    
    double getMaxSampleTime() const {
        if (sampleTimes.empty()) return 0.0;
        return std::max_element(sampleTimes.begin(), sampleTimes.end()->count();
    }
    
    double getAverageSampleTime() const {
        if (sampleTimes.empty()) return 0.0;
        double sum = std::accumulate(sampleTimes.begin(), sampleTimes.end(), 0.0,
                                     [](double acc, const auto& t) { return acc + t.count(); });
        return sum / sampleTimes.size();
    }
    
    double getSampleTimeStdDev() const {
        if (sampleTimes.size() < 2) return 0.0;
        double avg = getAverageSampleTime();
        double sumSquares = 0.0;
        for (const auto& t : sampleTimes) {
            double diff = t.count() - avg;
            sumSquares += diff * diff;
        }
        return std::sqrt(sumSquares / (sampleTimes.size() - 1));
    }
    
    std::string getFormattedProfile() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        ss << "=== Profile: " << name << " ===" << std::endl;
        ss << "Duration: " << duration.count() << "s" << std::endl;
        ss << "Samples/sec: " << getSamplesPerSecond() << std::endl;
        ss << "Min sample: " << getMinSampleTime() << "s" << std::endl;
        ss << "Max sample: " << getMaxSampleTime() << "s" << std::endl;
        ss << "Avg sample: " << getAverageSampleTime() << "s" << std::endl;
        ss << "Std dev sample: " << getSampleTimeStdDev() << "s" << std::endl;
        ss << "Memory used: " << (memoryUsed / 1024 / 1024) << "MB" << std::endl;
        ss << "Peak memory: " << (peakMemory / 1024 / 1024) << "MB" << std::endl;
        ss << "Allocations: " << allocations" << std::endl;
        ss << "Deallocations: " << deallocations << std::endl;
        return ss.str();
    }
};

// Performance analysis results
struct PerformanceAnalysis {
    std::string profileName;
    double totalTime;             // Total execution time in seconds
    double cpuEfficiency;        // CPU utilization efficiency
    double memoryEfficiency;     // Memory utilization efficiency
    double samplesPerSecond;     // Sampling frequency
    double minTime;               // Minimum time for any operation
    double maxTime;               // Maximum time for any operation
    double avgTime;               // Average time for operations
    double stdDevTime;            // Standard deviation of operation times
    double throughput;           // Operations per second
    double cpuTimePerOperation;  // CPU time per operation in seconds
    double memoryPerOperation;   // Memory per operation in bytes
    
    PerformanceAnalysis() : totalTime(0.0), cpuEfficiency(0.0), memoryEfficiency(0.0),
                           samplesPerSecond(0.0), minTime(0.0), maxTime(0.0), avgTime(0.0),
                           stdDevTime(0.0), throughput(0.0), cpuTimePerOperation(0.0),
                           memoryPerOperation(0.0) {}
    
    std::string getFormattedAnalysis() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4);
        ss << "=== Performance Analysis: " << profileName << " ===" << std::endl;
        ss << "Total Time: " << totalTime << "s" << std::endl;
        ss << "Samples/sec: " << samplesPerSecond << std::endl;
        ss << "Efficiency: CPU=" << cpuEfficiency << "%, Memory=" << memoryEfficiency << "%" << std::endl;
        ss << "Time Stats - Min: " << minTime << "s, Max: " << maxTime << "s, Avg: " << avgTime << "s" << std::endl;
        ss << "Variation - StdDev: " << stdDevTime << "s" << std::endl;
        ss << "Throughput: " << throughput << " ops/sec" << std::endl;
        ss << "CPU Time/Op: " << cpuTimePerOperation << "s" << std::endl;
        ss << "Memory/Op: " << (memoryPerOperation / 1024) << "KB" << std::endl;
        return ss.str();
    }
};

// Performance profiler class
class PerformanceProfiler {
public:
    // Profiler callbacks
    using ProfileCallback = std::function<void(const ProfileInfo&)>
    using AnalysisCallback = std::function<void(const PerformanceAnalysis&)>
    
    // Profiler singleton
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    // Profile management
    ProfileInfo startProfile(const std::string& name) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        ProfileInfo profile(name);
        profile.start();
        
        activeProfiles_[name] = profile;
        currentProfile_ = name;
        
        // Trigger profile callback
        for (const auto& callback : profileCallbacks_) {
            try {
                callback(profile);
            } catch (const std::exception& e) {
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Profile callback error: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
            }
        }
        
        return profile;
    }
    
    void endProfile() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (currentProfile_.empty() || !activeProfiles_.count(currentProfile_)) {
            getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                "No active profile to end", ErrorSeverity::Warning, ErrorCategory::System);
            return;
        }
        
        ProfileInfo& profile = activeProfiles_[currentProfile_];
        profile.end();
        
        // Store completed profile
        completedProfiles_.push_back(profile);
        
        // Calculate analysis
        PerformanceAnalysis analysis = analyzeProfile(profile);
        
        // Trigger analysis callback
        for (const auto& callback : analysisCallbacks_) {
            try {
                callback(analysis);
            } catch (const std::exception& e) {
                getEnhancedErrorManager().addError(NLMErrors::UnknownError,
                    "Analysis callback error: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System);
            }
        }
        
        // Remove from active profiles
        activeProfiles_.erase(currentProfile_);
        currentProfile_.clear();
    }
    
    void sampleCurrentProfile() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (currentProfile_.empty() || !activeProfiles_.count(currentProfile_)) {
            return;
        }
        
        activeProfiles_[currentProfile_].sample();
    }
    
    // Profile retrieval and analysis
    std::vector<ProfileInfo> getActiveProfiles() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<ProfileInfo> profiles;
        for (const auto& pair : activeProfiles_) {
            profiles.push_back(pair.second);
        }
        return profiles;
    }
    
    std::vector<ProfileInfo> getCompletedProfiles() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return completedProfiles_;
    }
    
    ProfileInfo getProfile(const std::string& name) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (activeProfiles_.count(name)) {
            return activeProfiles_.at(name);
        }
        
        // Search in completed profiles
        for (const auto& profile : completedProfiles_) {
            if (profile.name == name) {
                return profile;
            }
        }
        
        // Return empty profile
        return ProfileInfo(name);
    }
    
    // Analysis functions
    PerformanceAnalysis analyzeProfile(const ProfileInfo& profile) const {
        PerformanceAnalysis analysis;
        analysis.profileName = profile.name;
        
        if (profile.duration.count() > 0.0) {
            analysis.totalTime = profile.duration.count();
            analysis.samplesPerSecond = profile.getSamplesPerSecond();
            analysis.minTime = profile.getMinSampleTime();
            analysis.maxTime = profile.getMaxSampleTime();
            analysis.avgTime = profile.getAverageSampleTime();
            analysis.stdDevTime = profile.getSampleTimeStdDev();
            
            if (profile.sampleTimes.size() > 1) {
                analysis.throughput = static_cast<double>(profile.sampleTimes.size() - 1) / analysis.totalTime;
            }
            
            // Calculate efficiency metrics (mock implementation)
            analysis.cpuEfficiency = std::min(100.0, analysis.samplesPerSecond * 100.0);
            analysis.memoryEfficiency = std::min(100.0, profile.peakMemory / (1024 * 1024));
            
            analysis.cpuTimePerOperation = profile.cpuTime / profile.sampleTimes.size();
            analysis.memoryPerOperation = profile.peakMemory / (profile.sampleTimes.size() > 0 ? profile.sampleTimes.size() : 1);
        }
        
        return analysis;
    }
    
    std::vector<PerformanceAnalysis> analyzeAllProfiles() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<PerformanceAnalysis> analyses;
        
        for (const auto& profile : completedProfiles_) {
            analyses.push_back(analyzeProfile(profile));
        }
        
        return analyses;
    }
    
    PerformanceAnalysis getProfileAnalysis(const std::string& name) const {
        ProfileInfo profile = getProfile(name);
        return analyzeProfile(profile);
    }
    
    // Statistics
    size_t getActiveProfileCount() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return activeProfiles_.size();
    }
    
    size_t getCompletedProfileCount() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return completedProfiles_.size();
    }
    
    double getTotalProfilingTime() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        double total = 0.0;
        for (const auto& profile : completedProfiles_) {
            total += profile.duration.count();
        }
        return total;
    }
    
    // Callbacks
    void addProfileCallback(ProfileCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        profileCallbacks_.push_back(callback);
    }
    
    void removeProfileCallback(ProfileCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        profileCallbacks_.erase(std::remove(profileCallbacks_.begin(), profileCallbacks_.end(), callback),
                               profileCallbacks_.end());
    }
    
    void addAnalysisCallback(AnalysisCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        analysisCallbacks_.push_back(callback);
    }
    
    void removeAnalysisCallback(AnalysisCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        analysisCallbacks_.erase(std::remove(analysisCallbacks_.begin(), analysisCallbacks_.end(), callback),
                                analysisCallbacks_.end());
    }
    
    // Profile management
    void clearProfiles() {
        std::unique_lock<std::mutex> lock(mutex_);
        activeProfiles_.clear();
        completedProfiles_.clear();
        currentProfile_.clear();
    }
    
    void clearCompletedProfiles() {
        std::unique_lock<std::mutex> lock(mutex_);
        completedProfiles_.clear();
    }
    
    // Global profiling utilities
    template<typename Func>
    auto profileFunction(Func&& func, const std::string& name = "anonymous") {
        auto start = std::chrono::high_resolution_clock::now();
        
        ProfileInfo profile(name);
        profile.start();
        
        // Call the function
        auto result = func();
        
        profile.end();
        profile.cpuTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - start).count();
        
        return std::make_tuple(result, profile);
    }
    
    template<typename... Args>
    auto profileFunctionWithArgs(std::function<Args...> func, Args&&... args, const std::string& name = "anonymous") {
        auto start = std::chrono::high_resolution_clock::now();
        
        ProfileInfo profile(name);
        profile.start();
        
        // Call the function with arguments
        auto result = func(std::forward<Args>(args)...);
        
        profile.end();
        profile.cpuTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - start).count();
        
        return std::make_tuple(result, profile);
    }
    
    // Profiling macros
#define NLM_PROFILE_START(name) \
    nlm::PerformanceProfiler::getInstance().startProfile(name)

#define NLM_PROFILE_SAMPLE() \
    nlm::PerformanceProfiler::getInstance().sampleCurrentProfile()

#define NLM_PROFILE_END() \
    nlm::PerformanceProfiler::getInstance().endProfile()

#define NLM_PROFILE_SCOPE(name) \
    auto _profile_guard = std::make_shared<nlm::ProfileInfo>(namelm::PerformanceProfiler::getInstance().startProfile(name)); \
    auto _profile_cleanup = [&]() { nlm::PerformanceProfiler::getInstance().endProfile(); }; \
    std::shared_ptr<void> _profile_guard_holder(nullptr, _profile_cleanup)

protected:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
private:
    mutable std::shared_mutex mutex_;
    std::mutex guardMutex_;
    
    std::string currentProfile_;
    std::unordered_map<std::string, ProfileInfo> activeProfiles_;
    std::vector<ProfileInfo> completedProfiles_;
    
    std::vector<ProfileCallback> profileCallbacks_;
    std::vector<AnalysisCallback> analysisCallbacks_;
};

// Global performance profiler instance
inline PerformanceProfiler& getPerformanceProfiler() {
    return PerformanceProfiler::getInstance();
}

// Performance analysis utility functions
inline std::string getPerformanceReport() {
    std::stringstream ss;
    
    ss << "=== NLM Performance Report ===" << std::endl;
    
    // Get profiler statistics
    auto& profiler = getPerformanceProfiler();
    
    ss << "Profiling Statistics:" << std::endl;
    ss << "  Active Profiles: " << profiler.getActiveProfileCount() << std::endl;
    ss << "  Completed Profiles: " << profiler.getCompletedProfileCount() << std::endl;
    ss << "  Total Profiling Time: " << profiler.getTotalProfilingTime() << "s" << std::endl;
    
    // Analyze completed profiles
    auto analyses = profiler.analyzeAllProfiles();
    if (!analyses.empty()) {
        ss << std::endl << "Profile Analyses:" << std::endl;
        for (const auto& analysis : analyses) {
            ss << "  - " << analysis.profileName << ": " 
               << analysis.totalTime << "s, " << analysis.throughput << " ops/sec" << std::endl;
        }
    }
    
    return ss.str();
}

} // namespace nlm

#endif // NLM_PERFORMANCE_ANALYSIS_H
