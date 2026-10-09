#include "PerformanceMonitor.hpp"
#include <sys/resource.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() {
    running_ = true;
    memoryMonitorThread_ = std::thread(&PerformanceMonitor::memoryMonitorFunc, this);
}

PerformanceMonitor::~PerformanceMonitor() {
    running_ = false;
    if (memoryMonitorThread_.joinable()) {
        memoryMonitorThread_.join();
    }
}

void PerformanceMonitor::startTask(const std::string& taskName) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    TaskMetrics metrics;
    metrics.startTime = std::chrono::high_resolution_clock::now();
    metrics.cpuStartTime = std::chrono::steady_clock::now();
    metrics.memoryUsage = getProcessMemoryUsage() / 1024.0 / 1024.0; // MB
    metrics.peakMemoryUsage = metrics.memoryUsage;
    taskMetrics_[taskName] = metrics;
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::endTask(const std::string& taskName) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceMetrics metrics;
    
    if (taskMetrics_.find(taskName) != taskMetrics_.end()) {
        TaskMetrics& taskMetrics = taskMetrics_[taskName];
        auto endTime = std::chrono::high_resolution_clock::now();
        auto cpuEndTime = std::chrono::steady_clock::now();
        
        metrics.totalTime = std::chrono::duration<double>(endTime - taskMetrics.startTime).count();
        metrics.cpuTime = std::chrono::duration<double>(cpuEndTime - taskMetrics.cpuStartTime).count();
        metrics.memoryUsage = taskMetrics.memoryUsage;
        metrics.peakMemoryUsage = taskMetrics.peakMemoryUsage;
        metrics.neuronsProcessed = taskMetrics.neuronsProcessed;
        metrics.spikesGenerated = taskMetrics.spikesGenerated;
        metrics.synapsesUpdated = taskMetrics.synapsesUpdated;
        
        taskMetrics_[taskName] = TaskMetrics(); // Clear
    }
    
    // Update cumulative metrics
    cumulativeMetrics_.totalTime += metrics.totalTime;
    cumulativeMetrics_.cpuTime += metrics.cpuTime;
    cumulativeMetrics_.memoryUsage = std::max(cumulativeMetrics_.memoryUsage, metrics.peakMemoryUsage);
    cumulativeMetrics_.peakMemoryUsage = std::max(cumulativeMetrics_.peakMemoryUsage, metrics.peakMemoryUsage);
    cumulativeMetrics_.totalNeuronsProcessed += metrics.neuronsProcessed;
    cumulativeMetrics_.totalSpikesGenerated += metrics.spikesGenerated;
    cumulativeMetrics_.totalSynapsesUpdated += metrics.synapsesUpdated;
    
    return metrics;
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::getBatchMetrics(
    const std::vector<BatchSimulationResult>& results) const {
    PerformanceMetrics metrics;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            metrics.totalTime += result.executionTime;
            if (result.brain) {
                metrics.totalNeuronsProcessed += result.brain->getTotalNeuronCount();
                metrics.totalSpikesGenerated += result.brain->getTotalSpikeCount();
            }
        }
    }
    
    if (!results.empty()) {
        metrics.mean_execution_time = metrics.totalTime / results.size();
        metrics.neuronsPerSecond = metrics.totalNeuronsProcessed / (metrics.totalTime > 0 ? metrics.totalTime : 1);
        metrics.spikesPerSecond = metrics.totalSpikesGenerated / (metrics.totalTime > 0 ? metrics.totalTime : 1);
        metrics.efficiencyScore = calculateEfficiencyScore(metrics);
    }
    
    return metrics;
}

std::vector<std::string> PerformanceMonitor::generateOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (cumulativeMetrics_.totalTime > 0 && cumulativeMetrics_.cpuTime > 0) {
        double efficiency = cumulativeMetrics_.cpuTime / cumulativeMetrics_.totalTime;
        if (efficiency < 0.5) {
            recommendations.push_back("CPU efficiency is low. Consider optimizing simulation loops.");
        }
    }
    
    if (cumulativeMetrics_.memoryUsage > 1024) { // > 1 GB
        recommendations.push_back("High memory usage detected. Consider reducing memory footprint.");
    }
    
    if (cumulativeMetrics_.neuronsPerSecond < 1000) {
        recommendations.push_back("Neuron processing rate is slow. Consider SIMD optimization.");
    }
    
    return recommendations;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    taskMetrics_.clear();
    cumulativeMetrics_ = PerformanceMetrics();
}

void PerformanceMonitor::memoryMonitorFunc() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::lock_guard<std::mutex> lock(metricsMutex_);
        size_t currentMemory = getProcessMemoryUsage();
        
        for (auto& [taskName, taskMetrics] : taskMetrics_) {
            taskMetrics.memoryUsage = currentMemory / 1024.0 / 1024.0;
            taskMetrics.peakMemoryUsage = std::max(taskMetrics.peakMemoryUsage, taskMetrics.memoryUsage);
        }
    }
}

double PerformanceMonitor::calculateEfficiencyScore(const PerformanceMetrics& metrics) const {
    // Simple efficiency score based on processing rate and memory usage
    double timeEfficiency = std::min(1.0, 100.0 / metrics.mean_execution_time); // Better if < 100s
    double memoryEfficiency = std::min(1.0, 1024.0 / metrics.memoryUsage); // Better if < 1GB
    double spikeEfficiency = std::min(1.0, metrics.spikesPerSecond / 10000.0); // Better if > 10k spikes/s
    
    return (timeEfficiency + memoryEfficiency + spikeEfficiency) / 3.0;
}

ResourceMonitor::ResourceMonitor() {
    startMonitoring();
}

ResourceMonitor::~ResourceMonitor() {
    stopMonitoring();
}

void ResourceMonitor::startMonitoring() {
    if (!monitoring_) {
        monitoring_ = true;
        monitorThread_ = std::thread(&ResourceMonitor::monitorFunc, this);
    }
}

void ResourceMonitor::stopMonitoring() {
    if (monitoring_) {
        monitoring_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
    }
}

ResourceMonitor::ResourceUsage ResourceMonitor::getCurrentResourceUsage() const {
    std::lock_guard<std::mutex> lock(usageMutex_);
    return currentUsage_;
}

std::vector<ResourceMonitor::ResourceUsage> ResourceMonitor::getResourceHistory(
    size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(usageMutex_);
    
    if (history_.size() <= maxHistory) {
        return history_;
    }
    
    std::vector<ResourceUsage> recentHistory;
    recentHistory.reserve(maxHistory);
    
    size_t startIndex = history_.size() - maxHistory;
    for (size_t i = startIndex; i < history_.size(); ++i) {
        recentHistory.push_back(history_[i]);
    }
    
    return recentHistory;
}

bool ResourceMonitor::isOverloaded() const {
    ResourceUsage usage = getCurrentResourceUsage();
    
    // Check for overload conditions
    if (usage.cpuUsagePercent > 90) return true;
    if (usage.memoryUsageMB > usage.memoryAvailableMB * 0.9) return true;
    if (usage.activeThreads > std::thread::hardware_concurrency() * 0.9) return true;
    
    return false;
}

void ResourceMonitor::monitorFunc() {
    while (monitoring_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        ResourceUsage usage;
        usage.timestamp = std::chrono::system_clock::now();
        usage.cpuUsagePercent = getSystemCpuUsage();
        usage.memoryUsageMB = getSystemMemoryUsage();
        usage.memoryAvailableMB = getSystemMemoryAvailable();
        usage.activeThreads = getActiveThreadCount();
        usage.pendingTasks = 0; // Would need task queue integration
        usage.gpuMemoryUsageMB = 0; // GPU monitoring would be platform-specific
        usage.gpuComputeUsage = 0; // GPU monitoring would be platform-specific
        
        {
            std::lock_guard<std::mutex> lock(usageMutex_);
            currentUsage_ = usage;
            history_.push_back(usage);
            
            // Keep history size limited
            if (history_.size() > 10000) {
                history_.erase(history_.begin(), history_.begin() + 1000);
            }
        }
    }
}

ExecutionTracker::ExecutionTracker() {
}

ExecutionTracker::~ExecutionTracker() {
}

void ExecutionTracker::recordEvent(const std::string& eventType, const std::string& description,
                                   size_t threadId, const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    ExecutionEvent event(eventType, description, threadId, metadata);
    events_.push_back(event);
}

void ExecutionTracker::startTiming(const std::string& sectionName, size_t threadId) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    activeTimers_[sectionName] = std::chrono::high_resolution_clock::now();
}

 double ExecutionTracker::endTiming(const std::string& sectionName, size_t threadId) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    if (activeTimers_.find(sectionName) != activeTimers_.end()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = activeTimers_[sectionName];
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        // Record the completed timing as an event
        ExecutionEvent event("timing", sectionName, threadId);
        event.duration = duration;
        events_.push_back(event);
        
        activeTimers_.erase(sectionName);
        sectionTimes_[sectionName] = duration; // Update stored time
        
        return duration;
    }
    
    return 0.0;
}

std::map<std::string, double> ExecutionTracker::getExecutionSummary(
    const std::vector<BatchSimulationResult>& results) const {
    std::map<std::string, double> summary;
    
    if (results.empty()) {
        return summary;
    }
    
    double totalTime = 0;
    double cpuTime = 0;
    size_t totalNeurons = 0;
    size_t totalSpikes = 0;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            totalTime += result.executionTime;
            totalNeurons += result.metrics.count("neuron_count") ? result.metrics.at("neuron_count") : 0;
            totalSpikes += result.metrics.count("spike_count") ? result.metrics.at("spike_count") : 0;
        }
    }
    
    summary["mean_execution_time"] = totalTime / results.size();
    summary["total_neurons_processed"] = totalNeurons;
    summary["total_spikes_generated"] = totalSpikes;
    summary["neurons_per_second"] = totalNeurons / (totalTime > 0 ? totalTime : 1);
    summary["spikes_per_second"] = totalSpikes / (totalTime > 0 ? totalTime : 1);
    
    return summary;
}

std::vector<std::string> ExecutionTracker::generateOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Analyze section times
    for (const auto& [sectionName, duration] : sectionTimes_) {
        if (duration > 10.0) { // Sections taking > 10 seconds
            recommendations.push_back("Optimize section: " + sectionName + " (takes " + 
                                    std::to_string(duration) + "s)");
        }
    }
    
    // Check for repeated patterns
    std::map<std::string, size_t> sectionCounts;
    for (const auto& event : events_) {
        if (event.eventType == "timing") {
            sectionCounts[event.description]++;
        }
    }
    
    for (const auto& [sectionName, count] : sectionCounts) {
        if (count > 3) {
            recommendations.push_back("Section " + sectionName + " is executed " + 
                                    std::to_string(count) + " times. Consider batching.");
        }
    }
    
    return recommendations;
}

MemoryMonitor::MemoryMonitor() {
    running_ = true;
    monitorThread_ = std::thread(&MemoryMonitor::monitorFunc, this);
}

MemoryMonitor::~MemoryMonitor() {
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

MemoryMonitor::MemoryStats MemoryMonitor::getCurrentMemoryStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    MemoryStats stats;
    
    if (!memoryHistory_.empty()) {
        stats = memoryHistory_.back();
    }
    
    return stats;
}

std::vector<MemoryMonitor::MemoryStats> MemoryMonitor::getMemoryHistory(
    size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    if (memoryHistory_.size() <= maxHistory) {
        return memoryHistory_;
    }
    
    std::vector<MemoryStats> recentHistory;
    recentHistory.reserve(maxHistory);
    
    size_t startIndex = memoryHistory_.size() - maxHistory;
    for (size_t i = startIndex; i < memoryHistory_.size(); ++i) {
        recentHistory.push_back(memoryHistory_[i]);
    }
    
    return recentHistory;
}

bool MemoryMonitor::detectMemoryLeak(size_t sampleSize) const {
    if (memoryHistory_.size() < sampleSize * 2) {
        return false;
    }
    
    std::vector<size_t> recentMemory;
    size_t historyStart = memoryHistory_.size() - sampleSize;
    
    for (size_t i = historyStart; i < memoryHistory_.size(); ++i) {
        recentMemory.push_back(memoryHistory_[i].processMemory);
    }
    
    // Check if memory is consistently increasing
    bool increasing = true;
    for (size_t i = 1; i < recentMemory.size(); ++i) {
        if (recentMemory[i] < recentMemory[i-1]) {
            increasing = false;
            break;
        }
    }
    
    return increasing;
}

double MemoryMonitor::getMemoryUsageTrend(size_t windowSize) const {
    if (memoryHistory_.size() < windowSize + 1) {
        return 0.0;
    }
    
    std::vector<size_t> recentMemory;
    size_t historyStart = memoryHistory_.size() - windowSize - 1;
    
    for (size_t i = historyStart; i < memoryHistory_.size(); ++i) {
        recentMemory.push_back(memoryHistory_[i].processMemory);
    }
    
    // Calculate linear regression slope
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    size_t n = recentMemory.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = static_cast<double>(recentMemory[i]);
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    return slope;
}

void MemoryMonitor::monitorFunc() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        MemoryStats stats = getCurrentMemoryStats();
        stats.timestamp = std::chrono::system_clock::now();
        
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            memoryHistory_.push_back(stats);
            
            // Keep history size limited
            if (memoryHistory_.size() > 10000) {
                memoryHistory_.erase(memoryHistory_.begin(), memoryHistory_.begin() + 1000);
            }
        }
    }
}

size_t MemoryMonitor::getProcessMemoryUsage() const {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes (Linux)
}

size_t MemoryMonitor::getProcessVirtualMemoryUsage() const {
    // This is a simplified implementation
    // For accurate results, would need to read from /proc/self/status
    return 0;
}

AggregatedResult ExperimentResultAggregator::aggregateBatchResults(
    const std::vector<BatchSimulationResult>& results) {
    AggregatedResult aggregated;
    aggregated.experimentName = "batch_simulation";
    aggregated.totalRuns = results.size();
    
    double totalTime = 0;
    double totalNeurons = 0;
    double totalSpikes = 0;
    double totalSynapses = 0;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            aggregated.successfulRuns++;
            aggregated.failedRuns++;
            
            totalTime += result.executionTime;
            
            if (result.metrics.find("neuron_count") != result.metrics.end()) {
                aggregated.rawData["neuron_count"].push_back(result.metrics.at("neuron_count"));
                totalNeurons += result.metrics.at("neuron_count");
            }
            
            if (result.metrics.find("spike_count") != result.metrics.end()) {
                aggregated.rawData["spike_count"].push_back(result.metrics.at("spike_count"));
                totalSpikes += result.metrics.at("spike_count");
            }
            
            if (result.metrics.find("synapse_count") != result.metrics.end()) {
                aggregated.rawData["synapse_count"].push_back(result.metrics.at("synapse_count"));
                totalSynapses += result.metrics.at("synapse_count");
            }
            
            // Store other metrics
            for (const auto& [key, value] : result.metrics) {
                if (key != "execution_time" && key != "neuron_count" && 
                    key != "spike_count" && key != "synapse_count") {
                    aggregated.rawData[key].push_back(value);
                }
            }
        } else if (result.status == "failed") {
            aggregated.failedRuns++;
        }
    }
    
    // Calculate statistics
    if (aggregated.successfulRuns > 0) {
        aggregated.meanExecutionTime = totalTime / aggregated.successfulRuns;
        aggregated.minExecutionTime = totalTime; // Will be updated below
        aggregated.maxExecutionTime = 0;
        
        std::vector<double> times;
        for (const auto& result : results) {
            if (result.status == "completed") {
                times.push_back(result.executionTime);
                aggregated.minExecutionTime = std::min(aggregated.minExecutionTime, result.executionTime);
                aggregated.maxExecutionTime = std::max(aggregated.maxExecutionTime, result.executionTime);
            }
        }
        
        if (!times.empty()) {
            aggregated.stdExecutionTime = 0;
            for (double t : times) {
                aggregated.stdExecutionTime += (t - aggregated.meanExecutionTime) * (t - aggregated.meanExecutionTime);
            }
            aggregated.stdExecutionTime = std::sqrt(aggregated.stdExecutionTime / times.size());
        }
        
        // Calculate other metric statistics
        for (auto& [metric, values] : aggregated.rawData) {
            if (values.size() > 1) {
                double sum = std::accumulate(values.begin(), values.end(), 0.0);
                aggregated.meanMetrics[metric] = sum / values.size();
                
                double sumSq = 0;
                for (double v : values) {
                    sumSq += (v - aggregated.meanMetrics[metric]) * (v - aggregated.meanMetrics[metric]);
                }
                aggregated.stdMetrics[metric] = std::sqrt(sumSq / values.size());
                
                aggregated.minMetrics[metric] = *std::min_element(values.begin(), values.end());
                aggregated.maxMetrics[metric] = *std::max_element(values.begin(), values.end());
            }
        }
    }
    
    return aggregated;
}

std::vector<double> ExperimentResultAggregator::performStatisticalAnalysis(
    const std::vector<double>& data,
    const std::string& analysisType) {
    std::vector<double> results;
    
    if (data.empty()) {
        return results;
    }
    
    if (analysisType == "basic") {
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        double sumSq = 0;
        for (double d : data) {
            sumSq += (d - mean) * (d - mean);
        }
        double std = std::sqrt(sumSq / data.size());
        double min = *std::min_element(data.begin(), data.end());
        double max = *std::max_element(data.begin(), data.end());
        
        results = {mean, std, min, max};
    } else if (analysisType == "detailed") {
        // Calculate more statistics
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        double sumSq = 0;
        for (double d : data) {
            sumSq += (d - mean) * (d - mean);
        }
        double std = std::sqrt(sumSq / data.size());
        
        results = {mean, std};
        
        // Add quantiles if requested
        if (data.size() >= 10) {
            std::vector<double> sorted = data;
            std::sort(sorted.begin(), sorted.end());
            results.push_back(sorted[data.size() / 4]); // 25th percentile
            results.push_back(sorted[data.size() / 2]); // 50th percentile
            results.push_back(sorted[3 * data.size() / 4]); // 75th percentile
        }
    }
    
    return results;
}

std::string ExperimentResultAggregator::generateResultSummary(
    const std::vector<AggregatedResult>& aggregatedResults) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6);
    
    ss << "Batch Simulation Results Summary" << std::endl;
    ss << "================================" << std::endl << std::endl;
    
    for (const auto& result : aggregatedResults) {
        ss << result.toString() << std::endl;
    }
    
    ss << std::endl;
    ss << "Total experiments analyzed: " << aggregatedResults.size() << std::endl;
    
    return ss.str();
}

ComparisonResult ResultComparator::compareExperiments(
    const AggregatedResult& baseline,
    const AggregatedResult& current) {
    ComparisonResult comparison;
    comparison.baselineExperiment = baseline.experimentName;
    comparison.currentExperiment = current.experimentName;
    
    // Compare execution times
    if (baseline.meanExecutionTime > 0 && current.meanExecutionTime > 0) {
        comparison.baselineMetrics["execution_time"] = baseline.meanExecutionTime;
        comparison.currentMetrics["execution_time"] = current.meanExecutionTime;
        comparison.differences["execution_time"] = current.meanExecutionTime - baseline.meanExecutionTime;
        comparison.percentChanges["execution_time"] = 
            ((current.meanExecutionTime - baseline.meanExecutionTime) / baseline.meanExecutionTime) * 100;
    }
    
    // Compare other metrics
    std::set<std::string> allMetrics;
    for (const auto& [metric, value] : baseline.meanMetrics) {
        allMetrics.insert(metric);
    }
    for (const auto& [metric, value] : current.meanMetrics) {
        allMetrics.insert(metric);
    }
    
    for (const auto& metric : allMetrics) {
        if (baseline.meanMetrics.find(metric) != baseline.meanMetrics.end() &&
            current.meanMetrics.find(metric) != current.meanMetrics.end()) {
            
            comparison.baselineMetrics[metric] = baseline.meanMetrics.at(metric);
            comparison.currentMetrics[metric] = current.meanMetrics.at(metric);
            
            double diff = current.meanMetrics.at(metric) - baseline.meanMetrics.at(metric);
            double baselineVal = baseline.meanMetrics.at(metric);
            
            comparison.differences[metric] = diff;
            comparison.percentChanges[metric] = (diff / baselineVal) * 100;
        }
    }
    
    return comparison;
}

std::vector<std::string> ResultComparator::generateVisualizationData(
    const ComparisonResult& comparison,
    const std::string& chartType) {
    std::vector<std::string> visualizationData;
    
    if (chartType == "bar") {
        // Generate CSV-like data for bar chart
        visualizationData.push_back("metric,value,baseline,current,change_percent");
        
        for (const auto& [metric, currentValue] : comparison.currentMetrics) {
            if (comparison.baselineMetrics.find(metric) != comparison.baselineMetrics.end()) {
                double baselineValue = comparison.baselineMetrics.at(metric);
                double change = comparison.differences.at(metric);
                double changePercent = comparison.percentChanges.at(metric);
                
                std::stringstream ss;
                ss << std::fixed << std::setprecision(4);
                ss << metric << ","
                   << currentValue << ","
                   << baselineValue << ","
                   << change << ","
                   << changePercent;
                visualizationData.push_back(ss.str());
            }
        }
    }
    
    return visualizationData;
}
