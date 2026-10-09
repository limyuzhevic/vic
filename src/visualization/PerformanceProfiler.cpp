// Implementation of performance profiler
#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : profilingInterval_(1000.0), memoryLeakDetectionThreshold_(1000), 
      performanceAnomalyThreshold_(2.0f), maxMemoryBlocks_(10000),
      enableThreadAnalysis_(true), enableMemoryAnalysis_(true),
      enableRealTimeMonitoring_(true), isProfiling_(false),
      peakMemoryUsage_(0), realTimeThreadActive_(false) {
}

PerformanceProfiler::~PerformanceProfiler() {
    shutdown();
}

bool PerformanceProfiler::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PerformanceProfiler: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    resourceUsageHistory_.clear();
    performanceProfiles_.clear();
    memoryBlocks_.clear();
    allocationHistory_.clear();
    threadUsageHistory_.clear();
    componentProfiles_.clear();
    
    currentResourceUsage_.step = 0;
    currentResourceUsage_.timestamp = 0.0;
    currentResourceUsage_.cpuUsage = 0;
    currentResourceUsage_.memoryUsage = 0;
    currentResourceUsage_.memoryFootprint = 0;
    currentResourceUsage_.fileDescriptors = 0;
    currentResourceUsage_.networkConnections = 0;
    
    if (enableRealTimeMonitoring_) {
        startRealTimeThread();
    }
    
    NLM_LOG_INFO("PerformanceProfiler initialized successfully");
    return true;
}

void PerformanceProfiler::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stopRealTimeThread();
    
    brain_.reset();
    resourceUsageHistory_.clear();
    performanceProfiles_.clear();
    memoryBlocks_.clear();
    allocationHistory_.clear();
    threadUsageHistory_.clear();
    componentProfiles_.clear();
    
    isProfiling_ = false;
    realTimeThreadActive_ = false;
    
    NLM_LOG_INFO("PerformanceProfiler shutdown complete");
}

void PerformanceProfiler::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) return;
    
    currentStep_ = brain_->getCurrentStep();
    
    if (enableRealTimeMonitoring_) {
        updateResourceUsage();
    }
    
    updatePerformanceProfile();
}

void PerformanceProfiler::startProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    isProfiling_ = true;
    profilingStartTime_ = std::chrono::steady_clock::now();
    lastSampleTime_ = profilingStartTime_;
    
    NLM_LOG_INFO("Performance profiling started");
}

void PerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    isProfiling_ = false;
    
    // Generate final performance report
    updatePerformanceProfile();
    
    NLM_LOG_INFO("Performance profiling stopped");
}

void PerformanceProfiler::pauseProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    isProfiling_ = false;
}

void PerformanceProfiler::resumeProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    isProfiling_ = true;
}

ResourceUsage PerformanceProfiler::getCurrentResourceUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentResourceUsage_;
}

std::vector<ResourceUsage> PerformanceProfiler::getResourceUsageHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return resourceUsageHistory_;
}

void PerformanceProfiler::recordResourceUsage(const ResourceUsage& usage) {
    std::lock_guard<std::mutex> lock(mutex_);
    resourceUsageHistory_.push_back(usage);
    
    // Keep only recent history
    if (resourceUsageHistory_.size() > maxResourceHistory_) {
        resourceUsageHistory_.erase(resourceUsageHistory_.begin());
    }
}

PerformanceProfile PerformanceProfiler::getCurrentProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentProfile_;
}

std::vector<PerformanceProfile> PerformanceProfiler::getPerformanceProfiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return performanceProfiles_;
}

void PerformanceProfiler::updatePerformanceProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PerformanceProfile profile;
    profile.step = currentStep_;
    profile.timestamp = currentResourceUsage_.timestamp;
    profile.profileType = "realtime";
    
    // Compute overall performance metrics
    profile.overallPerformance = 1.0f / (currentResourceUsage_.cpuUsage + 1.0f); // Inverse of CPU usage
    profile.efficiency = 0.0f; // Placeholder - would compute efficiency
    profile.utilization = static_cast<float>(currentResourceUsage_.cpuUsage) / 100.0f;
    
    // Component metrics
    for (const auto& pair : componentProfiles_) {
        profile.componentMetrics[pair.first] = pair.second;
    }
    
    // Detect bottlenecks
    profile.bottlenecks = detectPerformanceBottlenecks();
    profile.hasIssues = !profile.bottlenecks.empty();
    profile.isOptimizing = profile.overallPerformance > 0.8f;
    
    // Performance trend
    profile.performanceTrend.clear();
    if (resourceUsageHistory_.size() > 1) {
        computePerformanceTrend(profile.performanceTrend);
    }
    
    // Resource usage history
    profile.resourceUsageHistory = resourceUsageHistory_;
    
    currentProfile_ = profile;
    performanceProfiles_.push_back(profile);
    
    // Maintain profile history
    if (performanceProfiles_.size() > maxProfileHistory_) {
        performanceProfiles_.erase(performanceProfiles_.begin());
    }
}

std::vector<PerformanceBottleneck> PerformanceProfiler::detectPerformanceBottlenecks() const {
    std::vector<PerformanceBottleneck> bottlenecks;
    
    identifyCpuBottlenecks(bottlenecks);
    identifyMemoryBottlenecks(bottlenecks);
    identifyIOBottlenecks(bottlenecks);
    identifyNetworkBottlenecks(bottlenecks);
    
    return bottlenecks;
}

bool PerformanceProfiler::hasPerformanceIssues() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceBottleneck> bottlenecks = detectPerformanceBottlenecks();
    return !bottlenecks.empty();
}

PerformanceProfile PerformanceProfiler::analyzePerformanceTrends() const {
    PerformanceProfile profile;
    profile.step = currentStep_;
    profile.timestamp = currentResourceUsage_.timestamp;
    profile.profileType = "trends";
    
    // Compute performance trend
    profile.performanceTrend.clear();
    computePerformanceTrend(profile.performanceTrend);
    
    // Compute moving averages
    std::vector<float> cpuUsageTrend;
    for (size_t i = 0; i < resourceUsageHistory_.size(); ++i) {
        float avgCpu = 0.0f;
        size_t windowSize = std::min(static_cast<size_t>(10), resourceUsageHistory_.size());
        
        for (size_t j = 0; j < windowSize && i + j < resourceUsageHistory_.size(); ++j) {
            avgCpu += static_cast<float>(resourceUsageHistory_[i + j].cpuUsage);
        }
        
        avgCpu /= windowSize;
        cpuUsageTrend.push_back(avgCpu);
    }
    
    profile.componentMetrics["cpu_usage_trend"] = computeMean(cpuUsageTrend);
    profile.componentMetrics["cpu_usage_volatility"] = computeVariance(cpuUsageTrend, profile.componentMetrics["cpu_usage_trend"]);
    
    return profile;
}

void PerformanceProfiler::recordAllocation(const AllocationInfo& allocation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!enableMemoryAnalysis_) return;
    
    allocationHistory_.push_back(allocation);
    
    // Create memory block
    MemoryBlock block;
    block.address = reinterpret_cast<void*>(allocationInfo.size()); // Placeholder - would need real address
    block.size = allocation.size;
    block.allocation = allocation;
    block.isAllocated = true;
    block.allocationStep = allocation.step;
    block.deallocationStep = -1;
    block.timestamp = std::chrono::steady_clock::now();
    
    memoryBlocks_.push_back(block);
    
    // Maintain memory blocks
    if (memoryBlocks_.size() > maxMemoryBlocks_) {
        memoryBlocks_.erase(memoryBlocks_.begin());
    }
}

void PerformanceProfiler::recordDeallocation(void* address, SimulationStep step) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!enableMemoryAnalysis_) return;
    
    for (auto& block : memoryBlocks_) {
        if (block.address == address && block.isAllocated) {
            block.isAllocated = false;
            block.deallocationStep = step;
            break;
        }
    }
}

std::vector<MemoryBlock> PerformanceProfiler::getMemoryBlocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryBlocks_;
}

std::vector<LeakPattern> PerformanceProfiler::detectMemoryLeaks() const {
    std::vector<LeakPattern> leaks;
    
    if (!enableMemoryAnalysis_) return leaks;
    
    detectAllocationLeaks();
    detectReferenceLeaks();
    detectCircularReferences();
    
    return leaks;
}

size_t PerformanceProfiler::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t totalUsage = 0;
    for (const auto& block : memoryBlocks_) {
        if (block.isAllocated) {
            totalUsage += block.size;
        }
    }
    
    return totalUsage;
}

size_t PerformanceProfiler::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return peakMemoryUsage_;
}

void PerformanceProfiler::recordThreadActivity(const ThreadUsage& thread) {
    std::lock_guard<std::mutex> lock(mutex_);
    threadUsageHistory_.push_back(thread);
}

std::vector<ThreadUsage> PerformanceProfiler::getThreadUsageHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return threadUsageHistory_;
}

void PerformanceProfiler::analyzeThreadPerformance() const {
    // Analyze thread performance
    // This is a placeholder - real implementation would analyze thread performance
}

void PerformanceProfiler::startComponentProfile(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(mutex_);
    componentTimers_[componentName] = std::chrono::steady_clock::now();
}

void PerformanceProfiler::stopComponentProfile(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = componentTimers_.find(componentName);
    if (it != componentTimers_.end()) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second);
        
        componentProfiles_[componentName] += static_cast<float>(duration.count());
        
        componentTimers_.erase(it);
    }
}

float PerformanceProfiler::getComponentExecutionTime(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = componentProfiles_.find(componentName);
    if (it != componentProfiles_.end()) {
        return it->second;
    }
    
    return 0.0f;
}

std::map<std::string, float> PerformanceProfiler::getAllComponentProfiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return componentProfiles_;
}

void PerformanceProfiler::enableRealTimeMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    enableRealTimeMonitoring_ = enable;
    
    if (enable && !realTimeThreadActive_) {
        startRealTimeThread();
    } else if (!enable && realTimeThreadActive_) {
        stopRealTimeThread();
    }
}

bool PerformanceProfiler::isRealTimeMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enableRealTimeMonitoring_;
}

void PerformanceProfiler::startRealTimeThread() {
    if (realTimeThreadActive_) return;
    
    realTimeThreadActive_ = true;
    realTimeThread_ = std::thread([this]() {
        auto nextTime = std::chrono::steady_clock::now();
        
        while (realTimeThreadActive_) {
            auto now = std::chrono::steady_clock::now();
            nextTime += std::chrono::duration<double, std::milli>(profilingInterval_);
            
            std::this_thread::sleep_until(nextTime);
            
            if (realTimeThreadActive_ && isProfiling_) {
                updateResourceUsage();
            }
        }
    });
}

void PerformanceProfiler::stopRealTimeThread() {
    realTimeThreadActive_ = false;
    
    if (realTimeThread_.joinable()) {
        realTimeThread_.join();
    }
}

bool PerformanceProfiler::isRealTimeThreadActive() const {
    return realTimeThreadActive_;
}

void PerformanceProfiler::setProfilingInterval(double interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    profilingInterval_ = interval;
}

void PerformanceProfiler::setMemoryLeakDetectionThreshold(size_t threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryLeakDetectionThreshold_ = threshold;
}

void PerformanceProfiler::setPerformanceAnomalyThreshold(float threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    performanceAnomalyThreshold_ = threshold;
}

void PerformanceProfiler::setMaxMemoryBlocks(size_t maxBlocks) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxMemoryBlocks_ = maxBlocks;
}

void PerformanceProfiler::setEnableThreadAnalysis(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    enableThreadAnalysis_ = enable;
}

void PerformanceProfiler::setEnableMemoryAnalysis(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    enableMemoryAnalysis_ = enable;
}

void PerformanceProfiler::exportPerformanceProfile(const std::string& filepath, ExportFormat format) const {
    // Export performance profile to file
    switch (format) {
        case ExportFormat::JSON:
            exportPerformanceProfileToJSON(filepath);
            break;
        case ExportFormat::CSV:
            exportPerformanceProfileToCSV(filepath);
            break;
        default:
            exportPerformanceProfileToText(filepath);
            break;
    }
}

void PerformanceProfiler::exportResourceUsage(const std::string& filepath) const {
    // Export resource usage to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "step,timestamp,cpu_usage,memory_usage,memory_footprint\n";
        
        for (const auto& usage : resourceUsageHistory_) {
            file << usage.step << ","
                 << usage.timestamp << ","
                 << usage.cpuUsage << ","
                 << usage.memoryUsage << ","
                 << usage.memoryFootprint << "\n";
        }
        
        file.close();
    }
}

void PerformanceProfiler::exportMemoryAnalysis(const std::string& filepath) const {
    // Export memory analysis to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "address,size,allocation_step,deallocation_step,is_allocated\n";
        
        for (const auto& block : memoryBlocks_) {
            file << block.address << ","
                 << block.size << ","
                 << block.allocationStep << ","
                 << block.deallocationStep << ","
                 << (block.isAllocated ? "true" : "false") << "\n";
        }
        
        file.close();
    }
}

void PerformanceProfiler::exportThreadAnalysis(const std::string& filepath) const {
    // Export thread analysis to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "thread_name,start_time,end_time,cpu_time,memory_usage\n";
        
        for (const auto& thread : threadUsageHistory_) {
            file << thread.threadName << ","
                 << thread.startTime.time_since_epoch().count() << ","
                 << thread.endTime.time_since_epoch().count() << ","
                 << thread.cpuTime << ","
                 << thread.memoryUsage << "\n";
        }
        
        file.close();
    }
}

void PerformanceProfiler::preparePerformanceDataForVisualization(std::vector<float>& visualizationData) const {
    // Prepare performance data for visualization
    visualizationData.clear();
    
    if (resourceUsageHistory_.empty()) {
        return;
    }
    
    // Extract CPU usage trend
    std::vector<float> cpuUsageTrend;
    for (const auto& usage : resourceUsageHistory_) {
        cpuUsageTrend.push_back(static_cast<float>(usage.cpuUsage));
    }
    
    // Normalize to [0, 1]
    if (!cpuUsageTrend.empty()) {
        float minCpu = *std::min_element(cpuUsageTrend.begin(), cpuUsageTrend.end());
        float maxCpu = *std::max_element(cpuUsageTrend.begin(), cpuUsageTrend.end());
        
        for (float& cpu : cpuUsageTrend) {
            if (maxCpu - minCpu < 1e-10f) {
                cpu = 0.5f;
            } else {
                cpu = (cpu - minCpu) / (maxCpu - minCpu);
            }
        }
    }
    
    visualizationData = cpuUsageTrend;
}

void PerformanceProfiler::prepareBottleneckDataForVisualization(std::vector<std::string>& bottleneckData) const {
    // Prepare bottleneck data for visualization
    bottleneckData.clear();
    
    std::vector<PerformanceBottleneck> bottlenecks = detectPerformanceBottlenecks();
    
    for (const auto& bottleneck : bottlenecks) {
        std::string bottleneckInfo = bottleneck.component + ": " + bottleneck.description + 
                                    " (severity: " + std::to_string(bottleneck.severity) + ")";
        bottleneckData.push_back(bottleneckInfo);
    }
}

void PerformanceProfiler::prepareMemoryDataForVisualization(std::vector<float>& memoryData) const {
    // Prepare memory data for visualization
    memoryData.clear();
    
    size_t currentUsage = getCurrentMemoryUsage();
    float usageRatio = static_cast<float>(currentUsage) / static_cast<float>(peakMemoryUsage_);
    
    memoryData.push_back(usageRatio);
    memoryData.push_back(static_cast<float>(currentUsage));
    memoryData.push_back(static_cast<float>(peakMemoryUsage_));
}

void PerformanceProfiler::prepareThreadDataForVisualization(std::vector<std::string>& threadData) const {
    // Prepare thread data for visualization
    threadData.clear();
    
    for (const auto& thread : threadUsageHistory_) {
        std::string threadInfo = thread.threadName + " " +
                                std::to_string(thread.cpuTime) + "ms " +
                                std::to_string(thread.memoryUsage) + " bytes";
        threadData.push_back(threadInfo);
    }
}

void PerformanceProfiler::updateResourceUsage() {
    // Update current resource usage
    ResourceUsage usage;
    usage.step = currentStep_;
    usage.timestamp = brain_->getCurrentTime();
    
    // Simulate resource usage (placeholder - real implementation would monitor actual resources)
    usage.cpuUsage = static_cast<size_t>(100.0f * (1.0f - currentProfile_.efficiency));
    usage.memoryUsage = getCurrentMemoryUsage();
    usage.memoryFootprint = peakMemoryUsage_;
    usage.fileDescriptors = 10; // Placeholder
    usage.networkConnections = 2; // Placeholder
    
    currentResourceUsage_ = usage;
    recordResourceUsage(usage);
    
    // Update peak memory usage
    if (usage.memoryUsage > peakMemoryUsage_) {
        peakMemoryUsage_ = usage.memoryUsage;
    }
}

void PerformanceProfiler::analyzeResourceUsageTrends() const {
    // Analyze resource usage trends
    // This is a placeholder - real implementation would analyze trends
}

void PerformanceProfiler::detectPerformanceAnomalies() const {
    // Detect performance anomalies
    // This is a placeholder - real implementation would detect anomalies
}

void PerformanceProfiler::detectMemoryLeaks() const {
    // Detect memory leaks
    // This is a placeholder - real implementation would detect leaks
}

void PerformanceProfiler::analyzeThreadPerformance() const {
    // Analyze thread performance
    // This is a placeholder - real implementation would analyze thread performance
}

void PerformanceProfiler::updatePerformanceProfile() const {
    // Update performance profile
    // This is a placeholder - real implementation would update profile
}

void PerformanceProfiler::cleanupMemoryBlocks() const {
    // Cleanup memory blocks
    // This is a placeholder - real implementation would cleanup blocks
}

void PerformanceProfiler::freeMemoryBlock(void* address) {
    // Free memory block
    // This is a placeholder - real implementation would free memory
}

void PerformanceProfiler::reclaimMemory() const {
    // Reclaim memory
    // This is a placeholder - real implementation would reclaim memory
}

void PerformanceProfiler::compactMemory() const {
    // Compact memory
    // This is a placeholder - real implementation would compact memory
}

void PerformanceProfiler::monitorThreadUsage() const {
    // Monitor thread usage
    // This is a placeholder - real implementation would monitor threads
}

void PerformanceProfiler::cleanupCompletedThreads() const {
    // Cleanup completed threads
    // This is a placeholder - real implementation would cleanup threads
}

void PerformanceProfiler::identifyCpuBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const {
    // Identify CPU bottlenecks
    // This is a placeholder - real implementation would identify CPU bottlenecks
}

void PerformanceProfiler::identifyMemoryBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const {
    // Identify memory bottlenecks
    // This is a placeholder - real implementation would identify memory bottlenecks
}

void PerformanceProfiler::identifyIOBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const {
    // Identify I/O bottlenecks
    // This is a placeholder - real implementation would identify I/O bottlenecks
}

void PerformanceProfiler::identifyNetworkBottlenecks(std::vector<PerformanceBottleneck>& bottlenecks) const {
    // Identify network bottlenecks
    // This is a placeholder - real implementation would identify network bottlenecks
}

void PerformanceProfiler::detectAllocationLeaks() const {
    // Detect allocation leaks
    // This is a placeholder - real implementation would detect allocation leaks
}

void PerformanceProfiler::detectReferenceLeaks() const {
    // Detect reference leaks
    // This is a placeholder - real implementation would detect reference leaks
}

void PerformanceProfiler::detectCircularReferences() const {
    // Detect circular references
    // This is a placeholder - real implementation would detect circular references
}

void PerformanceProfiler::computePerformanceTrend(std::vector<float>& trend) const {
    // Compute performance trend
    // This is a placeholder - real implementation would compute trend
}

void PerformanceProfiler::detectPerformanceCycles() const {
    // Detect performance cycles
    // This is a placeholder - real implementation would detect cycles
}

void PerformanceProfiler::identifyOptimalOperatingPoints() const {
    // Identify optimal operating points
    // This is a placeholder - real implementation would identify optimal points
}

float PerformanceProfiler::computeMean(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / static_cast<float>(values.size());
}

float PerformanceProfiler::computeVariance(const std::vector<float>& values, float mean) const {
    if (values.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : values) {
        float deviation = value - mean;
        sum += deviation * deviation;
    }
    
    return sum / static_cast<float>(values.size());
}

void PerformanceProfiler::exportPerformanceProfileToJSON(const std::string& filepath) const {
    // Export performance profile to JSON format
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"profiles\": [\n";
        
        for (size_t i = 0; i < performanceProfiles_.size(); ++i) {
            file << "    {\n";
            file << "      \"step\": " << performanceProfiles_[i].step << ",\n";
            file << "      \"timestamp\": " << performanceProfiles_[i].timestamp << ",\n";
            file << "      \"overall_performance\": " << performanceProfiles_[i].overallPerformance << ",\n";
            file << "      \"efficiency\": " << performanceProfiles_[i].efficiency << ",\n";
            file << "      \"utilization\": " << performanceProfiles_[i].utilization << ",\n";
            file << "      \"has_issues\": " << (performanceProfiles_[i].hasIssues ? "true" : "false") << ",\n";
            file << "      \"is_optimization\": " << (performanceProfiles_[i].isOptimizing ? "true" : "false") << "\n";
            file << "    }";
            if (i < performanceProfiles_.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ],\n";
        file << "  \"bottlenecks\": [\n";
        
        for (size_t i = 0; i < performanceProfiles_.size(); ++i) {
            for (size_t j = 0; j < performanceProfiles_[i].bottlenecks.size(); ++j) {
                file << "    {\n";
                file << "      \"type\": \"" << performanceProfiles_[i].bottlenecks[j].bottleneckType << "\",\n";
                file << "      \"component\": \"" << performanceProfiles_[i].bottlenecks[j].component << "\",\n";
                file << "      \"severity\": " << performanceProfiles_[i].bottlenecks[j].severity << ",\n";
                file << "      \"description\": \"" << performanceProfiles_[i].bottlenecks[j].description << "\"\n";
                file << "    }";
                if (i < performanceProfiles_.size() - 1 || j < performanceProfiles_[i].bottlenecks.size() - 1) file << ",";
                file << "\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
    }
}

void PerformanceProfiler::exportPerformanceProfileToCSV(const std::string& filepath) const {
    // Export performance profile to CSV format
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "step,timestamp,overall_performance,efficiency,utilization,has_issues\n";
        
        for (const auto& profile : performanceProfiles_) {
            file << profile.step << ","
                 << profile.timestamp << ","
                 << profile.overallPerformance << ","
                 << profile.efficiency << ","
                 << profile.utilization << ","
                 << (profile.hasIssues ? "true" : "false") << "\n";
        }
        
        file.close();
    }
}

void PerformanceProfiler::exportPerformanceProfileToText(const std::string& filepath) const {
    // Export performance profile to text format
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "Performance Profile Report\n";
        file << "==========================\n\n";
        
        file << "Performance Profiles:\n";
        for (const auto& profile : performanceProfiles_) {
            file << "  Step " << profile.step << ", Time " << profile.timestamp << ":\n";
            file << "    Overall Performance: " << profile.overallPerformance << "\n";
            file << "    Efficiency: " << profile.efficiency << "\n";
            file << "    Utilization: " << profile.utilization << "\n";
            file << "    Has Issues: " << (profile.hasIssues ? "Yes" : "No") << "\n";
            file << "    Is Optimizing: " << (profile.isOptimizing ? "Yes" : "No") << "\n\n";
        }
        
        file << "Performance Bottlenecks:\n";
        bool hasBottlenecks = false;
        for (const auto& profile : performanceProfiles_) {
            for (const auto& bottleneck : profile.bottlenecks) {
                hasBottlenecks = true;
                file << "  " << bottleneck.component << ": " << bottleneck.description << "\n";
            }
        }
        
        if (!hasBottlenecks) {
            file << "  No performance bottlenecks detected\n";
        }
        
        file.close();
    }
}

} // namespace nlm
