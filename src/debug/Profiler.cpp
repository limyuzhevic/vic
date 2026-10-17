#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct Profiler::Impl {
    std::shared_ptr<DebugConfig> config;
    std::atomic<bool> isRunning;
    std::atomic<bool> isInitialized;
    
    // Profile data
    std::vector<ProfileData> profileData;
    std::vector<Bottleneck> bottlenecks;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Current profiling state
    std::map<std::string, std::chrono::steady_clock::time_point> activeProfiles;
    std::vector<size_t> memoryAllocations;
    std::vector<size_t> memoryDeallocations;
    
    // Performance metrics
    std::map<std::string, std::vector<double>> operationDurations;
    std::vector<double> memoryUsageHistory;
    std::vector<double> cpuUsageHistory;
    
    // Configuration
    ProfilerConfig profilerConfig;
    
    Impl() : isRunning(false), isInitialized(false) {
        startTime = std::chrono::steady_clock::now();
        lastUpdateTime = startTime;
        
        profilerConfig.enableSimulationProfiling = true;
        profilerConfig.enableMemoryProfiling = true;
        profilerConfig.enableNeuralEventTimingAnalysis = true;
        profilerConfig.enableBottleneckDetection = true;
        profilerConfig.enableDetailedTiming = true;
        profilerConfig.profileSamplingInterval = 100; // milliseconds
        profilerConfig.maxProfileEntries = 100000;
        profilerConfig.bottleneckThreshold = 0.8;
        profilerConfig.profileMetrics = {"cpu_usage", "memory_usage", "simulation_time"};
        profilerConfig.enableRealTimeAnalysis = true;
        profilerConfig.enableAutomatedOptimizationSuggestions = true;
    }
    
    ~Impl() {
        // Export profile data if needed
        exportProfileData();
    }
    
    void exportProfileData() {
        if (profilerConfig.maxProfileEntries > 0 && profileData.size() > profilerConfig.maxProfileEntries) {
            // Keep only the most recent entries
            size_t keepCount = profilerConfig.maxProfileEntries / 2;
            profileData.erase(profileData.begin(), profileData.begin() + (profileData.size() - keepCount));
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return std::to_string(duration.count());
    }
    
    std::string getThreadId() const {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string getCallStack(int frames = 5) const {
        std::stringstream ss;
        // In a real implementation, this would capture the actual call stack
        // For now, return a placeholder
        ss << "[stack_frame_0, stack_frame_1, ..., stack_frame_" << frames - 1 << "]";
        return ss.str();
    }
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Profiler created");
}

Profiler::~Profiler() {
    NLM_LOG_DEBUG("Profiler destroyed");
}

Profiler::Profiler(Profiler&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

Profiler& Profiler::operator=(Profiler&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Profiler::initialize(std::shared_ptr<DebugConfig> config) {
    pImpl->config = config;
    pImpl->isInitialized = true;
    
    // Load profiler configuration from config
    if (config) {
        ProfilerConfig& cfg = pImpl->profilerConfig;
        cfg.enableSimulationProfiling = config->isSimulationPerformanceProfilingEnabled();
        cfg.enableMemoryProfiling = config->isMemoryProfilingEnabled();
        cfg.enableNeuralEventTimingAnalysis = config->isNeuralEventTimingAnalysisEnabled();
        cfg.enableBottleneckDetection = config->isBottleneckDetectionEnabled();
        cfg.profileMetrics = config->getProfilerMetrics();
    }
    
    NLM_LOG_INFO("Profiler initialized");
}

void Profiler::start() {
    if (!pImpl->isInitialized) {
        NLM_LOG_WARNING("Profiler not initialized, skipping start");
        return;
    }
    
    pImpl->isRunning = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("Profiler started");
}

void Profiler::stop() {
    pImpl->isRunning = false;
    NLM_LOG_INFO("Profiler stopped");
}

void Profiler::update() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    
    // Analyze bottlenecks periodically
    static size_t analysisCount = 0;
    if (++analysisCount >= 10) { // Analyze every 10 updates
        analyzeBottlenecks();
        detectPerformanceIssues();
        if (pImpl->profilerConfig.enableNeuralEventTimingAnalysis) {
            analyzeNeuralEventTiming();
        }
        analysisCount = 0;
    }
}

void Profiler::beginProfile(const std::string& operation) {
    if (!pImpl->isRunning) return;
    
    pImpl->activeProfiles[operation] = std::chrono::steady_clock::now();
    
    // Record profile start
    ProfileData profile;
    profile.timestamp = pImpl->activeProfiles[operation];
    profile.operation = operation;
    profile.duration = 0.0;
    profile.cpuUsage = getCurrentCpuUsage();
    profile.memoryUsage = getCurrentMemoryUsage();
    profile.memoryAllocated = 0;
    profile.memoryFreed = 0;
    profile.threadId = pImpl->getThreadId();
    profile.callStack = pImpl->getCallStack();
    
    pImpl->profileData.push_back(profile);
}

void Profiler::endProfile(const std::string& operation) {
    auto it = pImpl->activeProfiles.find(operation);
    if (it == pImpl->activeProfiles.end()) return;
    
    auto endTime = std::chrono::steady_clock::now();
    auto startTime = it->second;
    
    // Calculate duration
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Update the last profile entry for this operation
    for (auto& profile : pImpl->profileData) {
        if (profile.operation == operation && profile.duration == 0.0 && 
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - profile.timestamp).count() < 1000) {
            profile.duration = duration;
            profile.cpuUsage = getCurrentCpuUsage();
            profile.memoryUsage = getCurrentMemoryUsage();
            break;
        }
    }
    
    pImpl->activeProfiles.erase(it);
    
    // Record operation duration for statistics
    pImpl->operationDurations[operation].push_back(duration);
}

void Profiler::profileMemoryAllocation(size_t size, const std::string& context) {
    if (!pImpl->isRunning || !pImpl->profilerConfig.enableMemoryProfiling) return;
    
    ProfileData profile;
    profile.timestamp = std::chrono::steady_clock::now();
    profile.operation = "memory_allocation";
    profile.duration = 0.0;
    profile.cpuUsage = getCurrentCpuUsage();
    profile.memoryUsage = getCurrentMemoryUsage();
    profile.memoryAllocated = size;
    profile.memoryFreed = 0;
    profile.threadId = pImpl->getThreadId();
    profile.callStack = pImpl->getCallStack();
    profile.metrics["allocation_size"] = static_cast<double>(size);
    profile.metrics["context"] = context;
    
    pImpl->profileData.push_back(profile);
    pImpl->memoryAllocations.push_back(size);
}

void Profiler::profileMemoryDeallocation(size_t size, const std::string& context) {
    if (!pImpl->isRunning || !pImpl->profilerConfig.enableMemoryProfiling) return;
    
    ProfileData profile;
    profile.timestamp = std::chrono::steady_clock::now();
    profile.operation = "memory_deallocation";
    profile.duration = 0.0;
    profile.cpuUsage = getCurrentCpuUsage();
    profile.memoryUsage = getCurrentMemoryUsage();
    profile.memoryAllocated = 0;
    profile.memoryFreed = size;
    profile.threadId = pImpl->getThreadId();
    profile.callStack = pImpl->getCallStack();
    profile.metrics["deallocation_size"] = static_cast<double>(size);
    profile.metrics["context"] = context;
    
    pImpl->profileData.push_back(profile);
    pImpl->memoryDeallocations.push_back(size);
}

std::vector<ProfileData> Profiler::getProfileData(const std::string& operation, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) const {
    std::vector<ProfileData> filteredData;
    for (const auto& profile : pImpl->profileData) {
        if (profile.operation == operation && profile.timestamp >= startTime && profile.timestamp <= endTime) {
            filteredData.push_back(profile);
        }
    }
    return filteredData;
}

std::vector<Bottleneck> Profiler::getBottlenecks() const {
    return pImpl->bottlenecks;
}

std::map<std::string, double> Profiler::getPerformanceMetrics() const {
    std::map<std::string, double> metrics;
    
    // Calculate average operation durations
    for (const auto& [operation, durations] : pImpl->operationDurations) {
        if (!durations.empty()) {
            double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
            metrics[operation + "_avg_duration"] = sum / durations.size();
            metrics[operation + "_min_duration"] = *std::min_element(durations.begin(), durations.end());
            metrics[operation + "_max_duration"] = *std::max_element(durations.begin(), durations.end());
        }
    }
    
    // Add memory metrics
    if (!pImpl->memoryAllocations.empty()) {
        size_t totalAllocated = std::accumulate(pImpl->memoryAllocations.begin(), pImpl->memoryAllocations.end(), 0ULL);
        size_t totalDeallocated = std::accumulate(pImpl->memoryDeallocations.begin(), pImpl->memoryDeallocations.end(), 0ULL);
        metrics["total_memory_allocated"] = static_cast<double>(totalAllocated);
        metrics["total_memory_deallocated"] = static_cast<double>(totalDeallocated);
        metrics["memory_leak_estimate"] = static_cast<double>(totalAllocated - totalDeallocated);
    }
    
    // Add time-based metrics
    auto totalTime = std::chrono::duration<double, std::milli>(pImpl->lastUpdateTime - pImpl->startTime).count();
    metrics["total_profiling_time"] = totalTime;
    
    return metrics;
}

std::string Profiler::getPerformanceReport() const {
    std::stringstream ss;
    ss << "Performance Profiling Report\n";
    ss << "===============================\n";
    ss << "Total profile entries: " << pImpl->profileData.size() << "\n";
    ss << "Active profiles: " << pImpl->activeProfiles.size() << "\n";
    ss << "Detected bottlenecks: " << pImpl->bottlenecks.size() << "\n";
    ss << "\n";
    
    // Add performance metrics
    auto metrics = getPerformanceMetrics();
    for (const auto& [key, value] : metrics) {
        ss << key << ": " << value << "\n";
    }
    
    ss << "\n";
    
    // Add bottlenecks
    if (!pImpl->bottlenecks.empty()) {
        ss << "Bottlenecks:\n";
        for (const auto& bottleneck : pImpl->bottlenecks) {
            ss << "  - " << bottleneck.id << ": " << bottleneck.description << "\n";
            ss << "    Impact: " << bottleneck.impactScore << "\n";
            ss << "    Location: " << bottleneck.location << "\n";
            ss << "    Recommendation: " << bottleneck.recommendation << "\n";
        }
    }
    
    return ss.str();
}

void Profiler::clearProfileData() {
    pImpl->profileData.clear();
    pImpl->activeProfiles.clear();
    pImpl->bottlenecks.clear();
    pImpl->operationDurations.clear();
    pImpl->memoryAllocations.clear();
    pImpl->memoryDeallocations.clear();
    NLM_LOG_INFO("Profile data cleared");
}

void Profiler::analyzeBottlenecks() {
    if (!pImpl->profilerConfig.enableBottleneckDetection) {
        return;
    }
    
    pImpl->bottlenecks.clear();
    
    // Analyze operation durations for slow operations
    for (const auto& [operation, durations] : pImpl->operationDurations) {
        if (durations.empty()) continue;
        
        double avgDuration = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
        double threshold = pImpl->profilerConfig.bottleneckThreshold * 1000; // Convert to milliseconds
        
        if (avgDuration > threshold) {
            Bottleneck bottleneck;
            bottleneck.id = "slow_" + operation;
            bottleneck.description = "Operation " + operation + " has average duration of " + std::to_string(avgDuration) + " ms";
            bottleneck.detectedTime = std::chrono::steady_clock::now();
            bottleneck.impactScore = std::min(1.0, avgDuration / (threshold * 5.0));
            bottleneck.location = "[stack_trace]"; // Would contain actual location
            bottleneck.recommendation = "Consider optimizing " + operation + " implementation";
            
            pImpl->bottlenecks.push_back(bottleneck);
        }
    }
    
    NLM_LOG_INFO("Bottleneck analysis completed, detected " + std::to_string(pImpl->bottlenecks.size()) + " bottlenecks");
}

void Profiler::detectPerformanceIssues() {
    // Detect memory leaks
    if (pImpl->memoryAllocations.size() > 100) {
        size_t recentAllocations = std::accumulate(pImpl->memoryAllocations.end() - 100, pImpl->memoryAllocations.end(), 0ULL);
        size_t recentDeallocations = std::accumulate(pImpl->memoryDeallocations.end() - 100, pImpl->memoryDeallocations.end(), 0ULL);
        
        if (recentAllocations > recentDeallocations * 2) { // Estimated leak
            Bottleneck bottleneck;
            bottleneck.id = "potential_memory_leak";
            bottleneck.description = "Memory allocation significantly exceeds deallocation in recent profile data";
            bottleneck.detectedTime = std::chrono::steady_clock::now();
            bottleneck.impactScore = 0.9;
            bottleneck.location = "memory_profile";
            bottleneck.recommendation = "Review memory allocation/deallocation patterns";
            
            pImpl->bottlenecks.push_back(bottleneck);
        }
    }
}

void Profiler::analyzeNeuralEventTiming() {
    // Analyze neural event timing for unusual patterns
    std::vector<ProfileData> neuralEventProfiles;
    for (const auto& profile : pImpl->profileData) {
        if (profile.operation.find("neural") != std::string::npos || 
            profile.operation.find("spike") != std::string::npos) {
            neuralEventProfiles.push_back(profile);
        }
    }
    
    if (neuralEventProfiles.size() < 10) {
        return;
    }
    
    // Calculate timing statistics
    std::vector<double> intervals;
    for (size_t i = 1; i < neuralEventProfiles.size(); ++i) {
        double interval = std::chrono::duration<double, std::milli>(
            neuralEventProfiles[i].timestamp - neuralEventProfiles[i-1].timestamp).count();
        intervals.push_back(interval);
    }
    
    if (intervals.empty()) {
        return;
    }
    
    double avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
    double stdDev = 0.0;
    for (double interval : intervals) {
        stdDev += (interval - avgInterval) * (interval - avgInterval);
    }
    stdDev = std::sqrt(stdDev / intervals.size());
    
    // Check for unusual timing patterns (high coefficient of variation)
    double coefficientOfVariation = stdDev / avgInterval;
    if (coefficientOfVariation > 0.5) { // High variation indicates potential issues
        Bottleneck bottleneck;
        bottleneck.id = "neural_timing_anomaly";
        bottleneck.description = "Neural event timing shows unusual variation (CV = " + std::to_string(coefficientOfVariation) + ")";
        bottleneck.detectedTime = std::chrono::steady_clock::now();
        bottleneck.impactScore = std::min(1.0, coefficientOfVariation / 2.0);
        bottleneck.location = "neural_event_timing";
        bottleneck.recommendation = "Investigate neural event timing consistency";
        
        pImpl->bottlenecks.push_back(bottleneck);
    }
}

std::vector<std::string> Profiler::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Analyze bottlenecks for recommendations
    for (const auto& bottleneck : pImpl->bottlenecks) {
        recommendations.push_back(bottleneck.recommendation);
    }
    
    // Add general recommendations based on profile data
    if (pImpl->profileData.size() > 1000) {
        recommendations.push_back("Consider increasing profile sampling interval to reduce overhead");
    }
    
    if (pImpl->memoryAllocations.size() > pImpl->memoryDeallocations.size() * 2) {
        recommendations.push_back("Potential memory leak detected. Review memory management patterns.");
    }
    
    return recommendations;
}

size_t Profiler::getTotalProfileCount() const {
    return pImpl->profileData.size();
}

double Profiler::getAverageOperationDuration(const std::string& operation) const {
    auto it = pImpl->operationDurations.find(operation);
    if (it == pImpl->operationDurations.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double Profiler::getPeakMemoryUsage() const {
    double peak = 0.0;
    for (const auto& usage : pImpl->memoryUsageHistory) {
        if (usage > peak) {
            peak = usage;
        }
    }
    return peak;
}

// Helper methods

double Profiler::getCurrentCpuUsage() const {
    // In a real implementation, this would get the actual CPU usage
    // For now, return a simulated value
    return 50.0 + (std::rand() % 30); // 50-80%
}

double Profiler::getCurrentMemoryUsage() const {
    // In a real implementation, this would get the actual memory usage
    // For now, return a simulated value
    return 0.5 + (std::rand() % 30) / 100.0; // 0.5-0.8
}

} // namespace nlm
