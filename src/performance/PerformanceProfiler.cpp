// PerformanceProfiler.cpp - Core profiling system implementation
// This file implements the advanced performance profiling capabilities
// for NLM neural simulations, integrating with existing infrastructure

#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

PerformanceProfiler::PerformanceProfiler() 
    : isRunning_(false)
    , sessionStartTime_()
    , sessionEndTime_()
    , ioStats_()
    , cpuStats_()
    , currentSnapshot_()
    , config_()
{
    // Initialize with default configuration
    config_.enableMemoryProfiling = true;
    config_.enableCPUProfiling = true;
    config_.enableTimelineRecording = true;
    config_.enableIOLogging = false;
    config_.samplingInterval = 0.1; // 100ms
    config_.maxSamples = 1000000;
    config_.outputDirectory = ".";
    
    updatePerformanceThresholds();
}

PerformanceProfiler::~PerformanceProfiler() {
    if (isRunning_) {
        stop();
    }
}

void PerformanceProfiler::start(bool enableMemory, bool enableCPU, 
                                bool enableTimeline, bool enableIOLogging) {
    if (isRunning_) {
        return; // Already running
    }
    
    isRunning_ = true;
    sessionStartTime_ = std::chrono::high_resolution_clock::now();
    
    config_.enableMemoryProfiling = enableMemory;
    config_.enableCPUProfiling = enableCPU;
    config_.enableTimelineRecording = enableTimeline;
    config_.enableIOLogging = enableIOLogging;
    
    // Reset statistics
    ioStats_ = IOStats();
    cpuStats_ = CPUStats();
    currentSnapshot_ = PerformanceSnapshot();
    
    NLM_LOG_INFO("PerformanceProfiler started");
}

void PerformanceProfiler::stop(const std::string& outputFile) {
    if (!isRunning_) {
        return; // Not running
    }
    
    sessionEndTime_ = std::chrono::high_resolution_clock::now();
    isRunning_ = false;
    
    // Final snapshot
    updateSnapshot();
    
    // Generate report
    std::string report = generateReport();
    NLM_LOG_INFO("PerformanceProfiler stopped. Session duration: " + 
                std::to_string(getSessionDuration()) + " seconds");
    
    // Save to file if specified
    if (!outputFile.empty()) {
        saveToFile(outputFile);
    }
    
    // Save report to file
    std::string reportFile = config_.outputDirectory + "/performance_report.txt";
    std::ofstream reportStream(reportFile);
    if (reportStream.is_open()) {
        reportStream << report;
        NLM_LOG_INFO("Performance report saved to: " + reportFile);
    }
}

uint64_t PerformanceProfiler::startEvent(const std::string& eventName,
                                         ProfileEventType eventType,
                                         uint32_t threadId) {
    if (!isRunning_) return 0;
    
    uint64_t eventId = getNextAllocationId();
    activeEvents_[eventId] = std::chrono::high_resolution_clock::now();
    
    // Create timing sample
    TimingSample sample;
    sample.eventType = eventType;
    sample.eventName = eventName;
    sample.threadId = threadId;
    
    timingSamples_.push_back(sample);
    
    // Clean up old samples
    if (timingSamples_.size() > config_.maxSamples) {
        timingSamples_.pop_front();
    }
    
    return eventId;
}

void PerformanceProfiler::endEvent(uint64_t eventId) {
    if (!isRunning_ || !activeEvents_.count(eventId)) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto startTime = activeEvents_[eventId];
    
    // Calculate duration
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Find and update the timing sample
    for (auto it = timingSamples_.rbegin(); it != timingSamples_.rend(); ++it) {
        if (it->eventName == "TEMPORARY" || it->wallTime == 0) { // Placeholder for now
            it->duration = duration;
            it->wallTime = duration;
            break;
        }
    }
    
    activeEvents_.erase(eventId);
}

void PerformanceProfiler::recordTiming(ProfileEventType eventType, 
                                       const std::string& eventName,
                                       double duration,
                                       uint32_t threadId) {
    if (!isRunning_) return;
    
    TimingSample sample;
    sample.eventType = eventType;
    sample.eventName = eventName;
    sample.duration = duration;
    sample.wallTime = duration;
    sample.threadId = threadId;
    
    timingSamples_.push_back(sample);
    
    if (config_.enableTimelineRecording) {
        recordTimelineEvent(sample);
    }
    
    // Clean up old samples
    if (timingSamples_.size() > config_.maxSamples) {
        timingSamples_.pop_front();
    }
}

void PerformanceProfiler::recordMemoryAlloc(size_t size,
                                            const char* allocationSite,
                                            bool fromPool,
                                            const char* poolName) {
    if (!isRunning_ || !config_.enableMemoryProfiling) return;
    
    MemorySample sample;
    sample.allocationId = getNextAllocationId();
    sample.size = size;
    sample.allocationSite = allocationSite;
    sample.fromMemoryPool = fromPool;
    sample.poolName = poolName;
    sample.threadId = getCurrentThreadId();
    sample.isAllocation = true;
    sample.eventType = ProfileEventType::MEMORY_ALLOC;
    
    memorySamples_.push_back(sample);
    
    activeAllocations_[sample.allocationId] = sample;
}

void PerformanceProfiler::recordMemoryFree(uint64_t allocationId) {
    if (!isRunning_ || !config_.enableMemoryProfiling) return;
    
    MemorySample sample;
    sample.allocationId = allocationId;
    sample.isAllocation = false;
    sample.eventType = ProfileEventType::MEMORY_FREE;
    sample.threadId = getCurrentThreadId();
    
    memorySamples_.push_back(sample);
    
    activeAllocations_.erase(allocationId);
}

void PerformanceProfiler::recordCPUMetrics(uint64_t cpuCycles,
                                          uint64_t instructions,
                                          uint64_t cacheMisses) {
    if (!isRunning_) return;
    
    cpuStats_.totalCycles += cpuCycles;
    cpuStats_.totalInstructions += instructions;
    cpuStats_.totalCacheMisses += cacheMisses;
    
    currentSnapshot_.cacheMisses = cpuStats_.totalCacheMisses;
    currentSnapshot_.cacheHits = cpuStats_.totalInstructions - cpuStats_.totalCacheMisses;
}

void PerformanceProfiler::recordIOStats(uint64_t readBytes,
                                        uint64_t writeBytes,
                                        double readTime,
                                        double writeTime) {
    if (!isRunning_) return;
    
    ioStats_.readBytes += readBytes;
    ioStats_.writeBytes += writeBytes;
    ioStats_.readTime += readTime;
    ioStats_.writeTime += writeTime;
    
    currentSnapshot_.memoryUsage += readBytes + writeBytes;
}

PerformanceSnapshot PerformanceProfiler::getSnapshot() const {
    PerformanceSnapshot snapshot = currentSnapshot_;
    
    // Update snapshot from current state
    if (isRunning_) {
        snapshot.timestamp = std::chrono::high_resolution_clock::now();
        snapshot.cpuTime = cpuStats_.totalCPUTime;
        snapshot.memoryUsage = currentSnapshot_.memoryUsage;
        snapshot.peakMemoryUsage = currentSnapshot_.peakMemoryUsage;
        snapshot.cacheMisses = cpuStats_.totalCacheMisses;
        snapshot.cacheHits = cpuStats_.totalInstructions - cpuStats_.totalCacheMisses;
    }
    
    return snapshot;
}

std::vector<TimelineSegment> PerformanceProfiler::getTimelineSegments() const {
    return timelineSegments_;
}

void PerformanceProfiler::exportToJSON(std::ostream& output) const {
    nlohmann::json jsonData;
    
    jsonData["profiler_version"] = "1.0.0";
    jsonData["session_start"] = std::chrono::high_resolution_clock::to_time_t(sessionStartTime_);
    jsonData["session_duration"] = getSessionDuration();
    
    // Export timing samples
    nlohmann::json timingArray = nlohmann::json::array();
    for (const auto& sample : timingSamples_) {
        nlohmann::json timingJson;
        timingJson["timestamp"] = std::chrono::high_resolution_clock::to_time_t(sample.timestamp);
        timingJson["event_name"] = sample.eventName;
        timingJson["event_type"] = static_cast<int>(sample.eventType);
        timingJson["duration"] = sample.duration;
        timingJson["thread_id"] = sample.threadId;
        timingArray.push_back(timingJson);
    }
    jsonData["timing_samples"] = timingArray;
    
    // Export memory samples
    nlohmann::json memoryArray = nlohmann::json::array();
    for (const auto& sample : memorySamples_) {
        nlohmann::json memoryJson;
        memoryJson["timestamp"] = std::chrono::high_resolution_clock::to_time_t(sample.timestamp);
        memoryJson["allocation_id"] = sample.allocationId;
        memoryJson["size"] = sample.size;
        memoryJson["is_allocation"] = sample.isAllocation;
        memoryJson["thread_id"] = sample.threadId;
        memoryJson["from_pool"] = sample.fromMemoryPool;
        memoryArray.push_back(memoryJson);
    }
    jsonData["memory_samples"] = memoryArray;
    
    output << jsonData.dump(2);
}

void PerformanceProfiler::exportToCSV(std::ostream& output) const {
    output << "timestamp,event_name,event_type,duration,thread_id,memory_size,is_allocation\n";
    
    for (const auto& sample : timingSamples_) {
        output << std::chrono::high_resolution_clock::to_time_t(sample.timestamp) << ","
               << sample.eventName << ","
               << static_cast<int>(sample.eventType) << ","
               << sample.duration << ","
               << sample.threadId << ","
               << "0" << ","
               << sample.isAllocation << "\n";
    }
    
    for (const auto& sample : memorySamples_) {
        output << std::chrono::high_resolution_clock::to_time_t(sample.timestamp) << ","
               << "MEMORY_" << sample.isAllocation << ","
               << static_cast<int>(sample.eventType) << ","
               << sample.size << ","
               << sample.threadId << ","
               << sample.size << ","
               << sample.isAllocation << "\n";
    }
}

std::string PerformanceProfiler::generateReport() const {
    std::ostringstream report;
    
    report << "╔══════════════════════════════════════════════════════════════════╗\n";
    report << "║                    NLM Performance Profiler Report                 ║\n";
    report << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    report << "Session Information:\n";
    report << "  Start Time: " << std::chrono::high_resolution_clock::to_time_t(sessionStartTime_) << "\n";
    report << "  Duration: " << getSessionDuration() << " seconds\n";
    report << "  Events Recorded: " << getEventCount() << "\n";
    report << "  Memory Allocations: " << getMemoryAllocCount() << "\n\n";
    
    report << "Performance Snapshot:\n";
    report << "  Wall Time per Step: " << currentSnapshot_.wallTime << " s\n";
    report << "  CPU Time per Step: " << currentSnapshot_.cpuTime << " s\n";
    report << "  Memory Usage: " << formatBytes(currentSnapshot_.memoryUsage) << "\n";
    report << "  Peak Memory Usage: " << formatBytes(currentSnapshot_.peakMemoryUsage) << "\n";
    report << "  Cache Misses: " << currentSnapshot_.cacheMisses << "\n";
    report << "  Cache Hits: " << currentSnapshot_.cacheHits << "\n";
    report << "  Real-Time Factor: " << currentSnapshot_.realTimeFactor << "\n\n";
    
    report << "Neural Processing:\n";
    report << "  Neurons Processed: " << currentSnapshot_.neuronsProcessed << "\n";
    report << "  Spikes Processed: " << currentSnapshot_.spikesProcessed << "\n";
    report << "  Synaptic Events: " << currentSnapshot_.synapticEvents << "\n";
    report << "  Average Firing Rate: " << currentSnapshot_.avgFiringRate << " Hz\n\n";
    
    report << "Parallel Processing:\n";
    report << "  Active Threads: " << currentSnapshot_.activeThreads << "\n";
    report << "  Parallel Efficiency: " << (currentSnapshot_.parallelEfficiency * 100) << "%\n\n";
    
    report << "SIMD Processing:\n";
    report << "  SIMD Operations: " << currentSnapshot_.simdOperations << "\n";
    report << "  SIMD Utilization: " << (currentSnapshot_.simdUtilization * 100) << "%\n\n";
    
    report << "Performance Bottlenecks:\n";
    std::vector<std::string> bottlenecks = identifyBottlenecks();
    if (bottlenecks.empty()) {
        report << "  No significant bottlenecks detected.\n";
    } else {
        for (const auto& bottleneck : bottlenecks) {
            report << "  - " << bottleneck << "\n";
        }
    }
    
    return report.str();
}

std::vector<std::string> PerformanceProfiler::identifyBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check for performance issues based on thresholds
    if (currentSnapshot_.wallTime > thresholds_.maxWallTimePerStep) {
        bottlenecks.push_back("Wall time per step exceeds threshold: " + 
                            formatDuration(currentSnapshot_.wallTime));
    }
    
    if (currentSnapshot_.memoryUsage > thresholds_.maxMemoryUsage) {
        bottlenecks.push_back("Memory usage exceeds threshold: " + formatBytes(currentSnapshot_.memoryUsage));
    }
    
    if (currentSnapshot_.cpuTime > thresholds_.maxCPUTimePerStep) {
        bottlenecks.push_back("CPU time per step exceeds threshold: " + formatDuration(currentSnapshot_.cpuTime));
    }
    
    if (currentSnapshot_.cacheMisses > thresholds_.maxCacheMisses) {
        bottlenecks.push_back("Cache misses exceed threshold: " + std::to_string(currentSnapshot_.cacheMisses));
    }
    
    if (currentSnapshot_.simdUtilization < thresholds_.minSIMDEfficiency) {
        bottlenecks.push_back("SIMD utilization below threshold: " + 
                            std::to_string(currentSnapshot_.simdUtilization * 100) + "%");
    }
    
    if (currentSnapshot_.parallelEfficiency < thresholds_.minParallelEfficiency) {
        bottlenecks.push_back("Parallel efficiency below threshold: " + 
                            std::to_string(currentSnapshot_.parallelEfficiency * 100) + "%");
    }
    
    return bottlenecks;
}

double PerformanceProfiler::getSessionDuration() const {
    if (!isRunning_) {
        return std::chrono::duration<double>(sessionEndTime_ - sessionStartTime_).count();
    } else {
        return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - sessionStartTime_).count();
    }
}

void PerformanceProfiler::clear() {
    timingSamples_.clear();
    memorySamples_.clear();
    snapshots_.clear();
    timelineSegments_.clear();
    activeEvents_.clear();
    activeAllocations_.clear();
    markers_.clear();
    ioStats_ = IOStats();
    cpuStats_ = CPUStats();
    currentSnapshot_ = PerformanceSnapshot();
    
    NLM_LOG_INFO("PerformanceProfiler data cleared");
}

void PerformanceProfiler::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        exportToJSON(file);
        NLM_LOG_INFO("Performance profiler data saved to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to save performance profiler data to: " + filename);
    }
}

bool PerformanceProfiler::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load performance profiler data from: " + filename);
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Parse timing samples
        if (jsonData.contains("timing_samples")) {
            for (const auto& item : jsonData["timing_samples"]) {
                TimingSample sample;
                // Parse fields (simplified for now)
                timingSamples_.push_back(sample);
            }
        }
        
        NLM_LOG_INFO("Performance profiler data loaded from: " + filename);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading performance profiler data: " + std::string(e.what()));
        return false;
    }
}

std::string PerformanceProfiler::generateWebVisualization() const {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <title>NLM Performance Timeline</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html << "        .timeline { position: relative; height: 200px; border: 1px solid #ccc; }\n";
    html << "        .event { position: absolute; background-color: #4CAF50; color: white; padding: 2px; border-radius: 3px; }\n";
    html << "        .timeline-axis { position: absolute; bottom: 0; left: 0; right: 0; height: 20px; border-top: 1px solid #ccc; }\n";
    html << "        .timeline-label { position: absolute; bottom: -20px; font-size: 10px; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1>NLM Performance Timeline</h1>\n";
    html << "    <div class=\"timeline\" id=\"timeline\"></div>\n";
    html << "    <div class=\"timeline-axis\"></div>\n";
    html << "    <script>\n";
    html << "        // JavaScript for timeline visualization (simplified)\n";
    html << "        function drawTimeline() {\n";
    html << "            const timeline = document.getElementById('timeline');\n";
    html << "            const duration = " << getSessionDuration() << ";\n";
    html << "            const events = [];// To be filled from data\n";
    html << "            // Timeline drawing logic would go here\n";
    html << "        }\n";
    html << "        window.onload = drawTimeline;\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void PerformanceProfiler::registerProfiler(std::function<void(std::ostream&)> customExport) {
    customExporters_.push_back(customExport);
}

void PerformanceProfiler::addMarker(const std::string& markerName, const std::string& data) {
    double currentTime = getSessionDuration();
    markers_.push_back({currentTime, markerName + (data.empty() ? "" : " (" + data + ")")});
}

void PerformanceProfiler::updateSnapshot() {
    // Update current snapshot based on collected data
    currentSnapshot_.wallTime = getSessionDuration();
    
    // Calculate averages
    if (!timingSamples_.empty()) {
        double totalDuration = 0.0;
        for (const auto& sample : timingSamples_) {
            totalDuration += sample.duration;
        }
        currentSnapshot_.wallTime = totalDuration / timingSamples_.size();
    }
    
    // Update memory usage
    size_t totalMemory = 0;
    for (const auto& sample : memorySamples_) {
        if (sample.isAllocation) {
            totalMemory += sample.size;
        }
    }
    currentSnapshot_.memoryUsage = totalMemory;
    currentSnapshot_.peakMemoryUsage = totalMemory; // Simplified
    
    // Calculate SIMD efficiency
    currentSnapshot_.simdUtilization = calculateSIMDEfficiency();
    
    // Calculate parallel efficiency
    currentSnapshot_.parallelEfficiency = calculateParallelEfficiency();
}

void PerformanceProfiler::cleanupOldData() {
    // Remove old samples based on age
    auto cutoffTime = std::chrono::high_resolution_clock::now() - 
                      std::chrono::duration<double>(config_.samplingInterval * 10);
    
    // Clean up timing samples
    auto timingIt = std::remove_if(timingSamples_.begin(), timingSamples_.end(),
                                   [cutoffTime](const TimingSample& sample) {
                                       return sample.timestamp < cutoffTime;
                                   });
    timingSamples_.erase(timingIt, timingSamples_.end());
    
    // Clean up memory samples
    auto memoryIt = std::remove_if(memorySamples_.begin(), memorySamples_.end(),
                                   [cutoffTime](const MemorySample& sample) {
                                       return sample.timestamp < cutoffTime;
                                   });
    memorySamples_.erase(memoryIt, memorySamples_.end());
}

void PerformanceProfiler::recordTimelineEvent(const TimingSample& sample) {
    double normalizedTime = sample.wallTime / getSessionDuration();
    double height = 1.0;
    
    // Set height based on event type
    switch (sample.eventType) {
        case ProfileEventType::MEMORY_ALLOC:
        case ProfileEventType::MEMORY_FREE:
            height = 0.2;
            break;
        case ProfileEventType::CPU_CYCLE:
            height = 0.3;
            break;
        case ProfileEventType::CACHE_MISS:
            height = 0.4;
            break;
        case ProfileEventType::EVENT_PROCESSING:
            height = 0.8;
            break;
        default:
            height = 0.5;
    }
    
    TimelineSegment segment(
        sample.wallTime,
        sample.duration,
        sample.eventName,
        sample.eventType,
        height,
        sample.threadId
    );
    
    timelineSegments_.push_back(segment);
}

double PerformanceProfiler::calculateRealTimeFactor(double wallTime, double cpuTime) const {
    if (wallTime == 0.0) return 0.0;
    return cpuTime / wallTime;
}

std::string PerformanceProfiler::formatDuration(double seconds) const {
    if (seconds < 1.0) {
        return std::to_string(seconds * 1000) + " ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + " s";
    } else {
        return std::to_string(seconds / 60.0) + " min";
    }
}

std::string PerformanceProfiler::formatBytes(size_t bytes) const {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024.0) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024.0 * 1024)) + " MB";
    return std::to_string(bytes / (1024.0 * 1024 * 1024)) + " GB";
}

std::string PerformanceProfiler::generateHTMLTimeline() const {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <title>NLM Performance Timeline</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html << "        .timeline-container { margin: 20px 0; }\n";
    html << "        .timeline { position: relative; height: 100px; background: #f0f0f0; }\n";
    html << "        .timeline-track { position: absolute; height: 40px; background: #4CAF50; color: white; padding: 5px; margin: 30px 0; }\n";
    html << "        .timeline-axis { position: absolute; bottom: 0; left: 0; right: 0; height: 20px; border-top: 1px solid #ccc; }\n";
    html << "        .timeline-label { position: absolute; bottom: -20px; font-size: 10px; }\n";
    html << "        .event-marker { position: absolute; width: 2px; height: 100%; background: #FF9800; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1>NLM Performance Timeline</h1>\n";
    html << "    <div class=\"timeline-container\">\n";
    html << "        <div class=\"timeline\" id=\"main-timeline\"></div>\n";
    html << "        <div class=\"timeline-axis\"></div>\n";
    html << "    </div>\n";
    html << "    <script>\n";
    html << "        function drawTimeline() {\n";
    html << "            const timeline = document.getElementById('main-timeline');\n";
    html << "            const duration = " << getSessionDuration() << ";\n";
    html << "            const markers = [";
    
    for (size_t i = 0; i < markers_.size(); ++i) {
        html << "{" << markers_[i].first << ", \"" << markers_[i].second << "\"}";
        if (i < markers_.size() - 1) html << ", ";
    }
    
    html << "];\n";
    html << "            // Draw timeline events\n";
    html << "            markers.forEach((marker, index) => {\n";
    html << "                const element = document.createElement('div');\n";
    html << "                element.className = 'event-marker';\n";
    html << "                element.style.left = (marker[0] / duration * 100) + '%';\n";
    html << "                element.title = marker[1];\n";
    html << "                timeline.appendChild(element);\n";
    html << "            });\n";
    html << "        }\n";
    html << "        window.onload = drawTimeline;\n";
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string PerformanceProfiler::generateJSONData() const {
    nlohmann::json jsonData;
    
    jsonData["profiler_version"] = "1.0.0";
    jsonData["session_duration"] = getSessionDuration();
    jsonData["event_count"] = getEventCount();
    
    nlohmann::json timingArray = nlohmann::json::array();
    for (const auto& sample : timingSamples_) {
        nlohmann::json timingJson;
        timingJson["time"] = sample.wallTime;
        timingJson["event"] = sample.eventName;
        timingJson["type"] = static_cast<int>(sample.eventType);
        timingJson["duration"] = sample.duration;
        timingArray.push_back(timingJson);
    }
    jsonData["timing"] = timingArray;
    
    return jsonData.dump(2);
}

std::string PerformanceProfiler::generateCSVData() const {
    std::ostringstream csv;
    csv << "time,event,duration,type\n";
    
    for (const auto& sample : timingSamples_) {
        csv << sample.wallTime << ","
            << sample.eventName << ","
            << sample.duration << ","
            << static_cast<int>(sample.eventType) << "\n";
    }
    
    return csv.str();
}

std::vector<std::string> PerformanceProfiler::detectPerformanceIssues() const {
    std::vector<std::string> issues;
    
    // Simple heuristic for performance issues
    if (timingSamples_.size() > 100) {
        double totalDuration = 0.0;
        for (const auto& sample : timingSamples_) {
            totalDuration += sample.duration;
        }
        double avgDuration = totalDuration / timingSamples_.size();
        
        if (avgDuration > 0.1) { // 100ms average
            issues.push_back("Average event duration exceeds 100ms");
        }
    }
    
    if (currentSnapshot_.memoryUsage > 1024 * 1024 * 1024) { // 1GB
        issues.push_back("Memory usage exceeds 1GB");
    }
    
    return issues;
}

std::vector<TimelineSegment> PerformanceProfiler::generateTimelineFromSamples() const {
    return timelineSegments_;
}

void PerformanceProfiler::updatePerformanceThresholds() {
    thresholds_.maxWallTimePerStep = 0.1;      // 100ms
    thresholds_.maxMemoryUsage = 1024 * 1024 * 1024;  // 1GB
    thresholds_.maxCPUTimePerStep = 0.05;      // 50ms
    thresholds_.maxCacheMisses = 1000000;     // 1M
    thresholds_.maxMemoryBandwidth = 25.6;    // 25.6 GB/s
    thresholds_.minSIMDEfficiency = 0.8;      // 80%
    thresholds_.minParallelEfficiency = 0.7; // 70%
}

double PerformanceProfiler::calculateSIMDEfficiency() const {
    if (timingSamples_.empty()) return 0.0;
    
    size_t simdOperations = 0;
    for (const auto& sample : timingSamples_) {
        if (sample.eventType == ProfileEventType::CPU_CYCLE) {
            simdOperations++;
        }
    }
    
    return static_cast<double>(simdOperations) / timingSamples_.size();
}

double PerformanceProfiler::calculateParallelEfficiency() const {
    if (timingSamples_.empty()) return 0.0;
    
    size_t parallelSamples = 0;
    for (const auto& sample : timingSamples_) {
        if (sample.threadId > 0) {
            parallelSamples++;
        }
    }
    
    return static_cast<double>(parallelSamples) / timingSamples_.size();
}

void PerformanceProfiler::exportToWebFormat(std::ostream& output) const {
    output << generateWebVisualization();
}

} // namespace nlm