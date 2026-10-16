#pragma once

// Performance optimization and profiling tools for NLM
// Provides comprehensive performance measurement, profiling, and optimization capabilities

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

namespace nlm {

// Performance measurement types
enum class MeasurementType {
    CPU_TIME,
    WALL_TIME,
    MEMORY_USAGE,
    SIMULATION_STEPS,
    SPICE_COUNT,
    THROUGHPUT,
    LATENCY,
    BANDWIDTH
};

// Profile event for detailed performance tracking
struct ProfileEvent {
    std::string name;
    std::string category;
    MeasurementType type;
    double startTime;
    double endTime;
    double duration;
    std::map<std::string, std::any> metadata;
    
    ProfileEvent(const std::string& eventName, const std::string& eventCategory,
                MeasurementType eventType, double start = 0.0)
        : name(eventName), category(eventCategory), type(eventType),
          startTime(start), endTime(0.0), duration(0.0) {}
    
    void end() {
        endTime = getCurrentTime();
        duration = endTime - startTime;
    }
    
    static double getCurrentTime() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    }
};

// Performance metrics collection
class PerformanceMetrics {
public:
    PerformanceMetrics() : totalSteps_(0), totalSpikes_(0), startTime_(0.0), endTime_(0.0) {}
    
    void recordStep(size_t steps) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalSteps_ += steps;
    }
    
    void recordSpikes(size_t spikes) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalSpikes_ += spikes;
    }
    
    void startTiming() {
        std::lock_guard<std::mutex> lock(mutex_);
        startTime_ = ProfileEvent::getCurrentTime();
    }
    
    void endTiming() {
        std::lock_guard<std::mutex> lock(mutex_);
        endTime_ = ProfileEvent::getCurrentTime();
    }
    
    double getElapsedTime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return endTime_ - startTime_;
    }
    
    size_t getTotalSteps() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalSteps_;
    }
    
    size_t getTotalSpikes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalSpikes_;
    }
    
    double getStepsPerSecond() const {
        double elapsed = getElapsedTime();
        if (elapsed <= 0.0) return 0.0;
        return static_cast<double>(getTotalSteps()) / elapsed;
    }
    
    double getSpikesPerSecond() const {
        double elapsed = getElapsedTime();
        if (elapsed <= 0.0) return 0.0;
        return static_cast<double>(getTotalSpikes()) / elapsed;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalSteps_ = 0;
        totalSpikes_ = 0;
        startTime_ = 0.0;
        endTime_ = 0.0;
    }
    
private:
    mutable std::mutex mutex_;
    size_t totalSteps_;
    size_t totalSpikes_;
    double startTime_;
    double endTime_;
};

// Detailed profiler for granular performance analysis
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    
    void startEvent(const std::string& name, const std::string& category = "General",
                   MeasurementType type = MeasurementType::CPU_TIME) {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.emplace_back(name, category, type, ProfileEvent::getCurrentTime());
    }
    
    void endEvent(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& event : events_) {
            if (event.name == name && event.endTime == 0.0) {
                event.end();
                return;
            }
        }
    }
    
    void startBlock(const std::string& name, const std::string& category = "Block") {
        startEvent(name + "_start", category, MeasurementType::CPU_TIME);
    }
    
    void endBlock(const std::string& name) {
        endEvent(name + "_start");
    }
    
    double getEventDuration(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& event : events_) {
            if (event.name == name) {
                return event.duration;
            }
        }
        return 0.0;
    }
    
    std::vector<ProfileEvent> getEventsByCategory(const std::string& category) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ProfileEvent> filteredEvents;
        for (const auto& event : events_) {
            if (event.category == category) {
                filteredEvents.push_back(event);
            }
        }
        return filteredEvents;
    }
    
    std::vector<ProfileEvent> getEventsByType(MeasurementType type) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ProfileEvent> filteredEvents;
        for (const auto& event : events_) {
            if (event.type == type) {
                filteredEvents.push_back(event);
            }
        }
        return filteredEvents;
    }
    
    double getTotalTimeByCategory(const std::string& category) const {
        double total = 0.0;
        auto events = getEventsByCategory(category);
        for (const auto& event : events) {
            total += event.duration;
        }
        return total;
    }
    
    double getAverageTimeByCategory(const std::string& category) const {
        auto events = getEventsByCategory(category);
        if (events.empty()) return 0.0;
        double total = getTotalTimeByCategory(category);
        return total / events.size();
    }
    
    size_t getEventCountByCategory(const std::string& category) const {
        auto events = getEventsByCategory(category);
        return events.size();
    }
    
    std::string generateReport() const {
        std::stringstream ss;
        
        ss << "=== NLM Performance Profile Report ===" << std::endl;
        ss << std::endl;
        
        // Overall timing
        double totalTime = 0.0;
        for (const auto& event : events_) {
            totalTime += event.duration;
        }
        
        ss << "Total profiled time: " << totalTime << " seconds" << std::endl;
        ss << "Total events recorded: " << events_.size() << std::endl;
        ss << std::endl;
        
        // Categorization
        std::map<std::string, double> categoryTimes;
        std::map<std::string, size_t> categoryCounts;
        
        for (const auto& event : events_) {
            categoryTimes[event.category] += event.duration;
            categoryCounts[event.category]++;
        }
        
        ss << "=== Time Distribution by Category ===" << std::endl;
        for (const auto& pair : categoryTimes) {
            ss << pair.first << ": " << pair.second << "s (" 
               << (pair.second / totalTime * 100.0) << "%)" << std::endl;
        }
        
        ss << std::endl;
        
        // Type distribution
        std::map<MeasurementType, double> typeTimes;
        for (const auto& event : events_) {
            typeTimes[event.type] += event.duration;
        }
        
        ss << "=== Time Distribution by Type ===" << std::endl;
        for (const auto& pair : typeTimes) {
            ss << typeToString(pair.first) << ": " << pair.second << "s" << std::endl;
        }
        
        ss << std::endl;
        
        // Top events
        ss << "=== Top 10 Longest Events ===" << std::endl;
        std::vector<const ProfileEvent*> sortedEvents;
        for (const auto& event : events_) {
            if (event.duration > 0.0) {
                sortedEvents.push_back(&event);
            }
        }
        
        std::sort(sortedEvents.begin(), sortedEvents.end(),
                 [](const ProfileEvent* a, const ProfileEvent* b) {
                     return a->duration > b->duration;
                 });
        
        size_t count = std::min(size_t(10), sortedEvents.size());
        for (size_t i = 0; i < count; ++i) {
            const auto& event = *sortedEvents[i];
            ss << std::setw(2) << (i + 1) << ". " << std::left << std::setw(30) 
               << event.name << " - " << event.duration << "s" << std::endl;
        }
        
        ss << std::endl;
        
        // Performance recommendations
        ss << "=== Performance Recommendations ===" << std::endl;
        
        // Check for potential bottlenecks
        for (const auto& pair : categoryTimes) {
            if (pair.second > totalTime * 0.3) {  // > 30% of total time
                ss << "⚠ Category '" << pair.first << "' takes " 
                   << (pair.second / totalTime * 100.0) << "% of total time - consider optimization" << std::endl;
            }
        }
        
        // Check for long-running events
        for (const auto& event : sortedEvents) {
            if (event.duration > totalTime * 0.2) {  // > 20% of total time
                ss << "⚠ Event '" << event.name << "' takes " 
                   << (event.duration / totalTime * 100.0) << "% of total time - consider refactoring" << std::endl;
            }
        }
        
        if (sortedEvents.empty() || totalTime < 1.0) {
            ss << "No significant performance issues detected" << std::endl;
        }
        
        return ss.str();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.clear();
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<ProfileEvent> events_;
    
    std::string typeToString(MeasurementType type) const {
        switch (type) {
            case MeasurementType::CPU_TIME: return "CPU Time";
            case MeasurementType::WALL_TIME: return "Wall Time";
            case MeasurementType::MEMORY_USAGE: return "Memory Usage";
            case MeasurementType::SIMULATION_STEPS: return "Simulation Steps";
            case MeasurementType::SPICE_COUNT: return "Spike Count";
            case MeasurementType::THROUGHPUT: return "Throughput";
            case MeasurementType::LATENCY: return "Latency";
            case MeasurementType::BANDWIDTH: return "Bandwidth";
            default: return "Unknown";
        }
    }
};

// Simulation performance analyzer
class SimulationPerformanceAnalyzer {
public:
    SimulationPerformanceAnalyzer(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    struct PerformanceReport {
        double totalSimulationTime;
        size_t totalSteps;
        size_t totalSpikes;
        double stepsPerSecond;
        double spikesPerSecond;
        double averageStepTime;
        double averageSpikeFrequency;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "=== Simulation Performance Report ===" << std::endl;
            ss << std::endl;
            ss << "Total simulation time: " << totalSimulationTime << " seconds" << std::endl;
            ss << "Total simulation steps: " << totalSteps << std::endl;
            ss << "Total spikes generated: " << totalSpikes << std::endl;
            ss << "Steps per second: " << stepsPerSecond << std::endl;
            ss << "Spikes per second: " << spikesPerSecond << std::endl;
            ss << "Average step time: " << averageStepTime << " seconds" << std::endl;
            ss << "Average spike frequency: " << averageSpikeFrequency << " Hz" << std::endl;
            return ss.str();
        }
    };
    
    PerformanceReport analyzePerformance(const std::string& scenario = "full") {
        PerformanceReport report;
        
        // Get metrics from brain
        report.totalSteps = brain_->getTotalNeuronCount() * 100; // Estimate based on typical usage
        report.totalSpikes = brain_->getTotalSpikeCount();
        
        // Calculate derived metrics
        report.totalSimulationTime = report.totalSteps / 1000.0; // Assume 1k steps per second for estimation
        report.stepsPerSecond = report.totalSteps / report.totalSimulationTime;
        report.spikesPerSecond = report.totalSpikes / report.totalSimulationTime;
        report.averageStepTime = 1.0 / report.stepsPerSecond;
        report.averageSpikeFrequency = report.spikesPerSecond / static_cast<double>(brain_->getTotalNeuronCount());
        
        return report;
    }
    
    // Bottleneck detection
    std::vector<std::string> detectBottlenecks() {
        std::vector<std::string> bottlenecks;
        
        // Check neuron count vs performance
        size_t neuronCount = brain_->getTotalNeuronCount();
        double spikeCount = brain_->getTotalSpikeCount();
        
        double spikePerNeuron = neuronCount > 0 ? spikeCount / neuronCount : 0.0;
        
        if (spikePerNeuron < 0.1) {
            bottlenecks.push_back("Low spike activity - neurons may be under-stimulated");
        }
        
        if (spikePerNeuron > 10.0) {
            bottlenecks.push_back("High spike activity - neurons may be over-stimulated");
        }
        
        // Check region distribution
        auto regions = brain_->getRegions();
        if (regions.size() > 1) {
            size_t minNeurons = SIZE_MAX;
            for (const auto& region : regions) {
                minNeurons = std::min(minNeurons, region->getTotalNeuronCount());
            }
            
            if (minNeurons < 10) {
                bottlenecks.push_back("Some regions have too few neurons (< 10)");
            }
        }
        
        // Check connectivity
        size_t totalSynapses = brain_->getTotalSynapseCount();
        if (totalSynapses < 100) {
            bottlenecks.push_back("Low connectivity - sparse network");
        }
        
        // Check plasticity activity
        auto* stdpSystem = brain_->getSTDP();
        auto* hebbianSystem = brain_->getHebbian();
        
        if (stdpSystem || hebbianSystem) {
            // Check synapse plasticity flags
            for (const auto& region : regions) {
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                        // Check if plasticity is active (has spike history)
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        if (preSpikes.empty() && postSpikes.empty()) {
                            bottlenecks.push_back("Synapse has plasticity enabled but no spike history");
                            break;
                        }
                    }
                }
            }
        }
        
        return bottlenecks;
    }
    
    // Optimization suggestions
    std::vector<std::string> getOptimizationSuggestions() {
        std::vector<std::string> suggestions;
        
        // Suggest region balancing
        auto regions = brain_->getRegions();
        if (regions.size() > 1) {
            size_t maxNeurons = 0;
            size_t minNeurons = SIZE_MAX;
            for (const auto& region : regions) {
                size_t neuronCount = region->getTotalNeuronCount();
                maxNeurons = std::max(maxNeurons, neuronCount);
                minNeurons = std::min(minNeurons, neuronCount);
            }
            
            double ratio = static_cast<double>(maxNeurons) / minNeurons;
            if (ratio > 4.0) {
                suggestions.push_back("Consider balancing neuron distribution across regions (ratio: " + 
                                    std::to_string(ratio) + ":1)");
            }
        }
        
        // Suggest connectivity optimization
        size_t totalSynapses = brain_->getTotalSynapseCount();
        size_t totalPossibleConnections = brain_->getTotalNeuronCount() * (brain_->getTotalNeuronCount() - 1);
        
        if (totalPossibleConnections > 0) {
            double connectivityRatio = totalSynapses / totalPossibleConnections;
            if (connectivityRatio < 0.01) {
                suggestions.push_back("Very low connectivity - consider increasing connection probability");
            }
            else if (connectivityRatio > 0.5) {
                suggestions.push_back("Very high connectivity - may cause instability");
            }
        }
        
        // Suggest plasticity tuning
        auto* stdpSystem = brain_->getSTDP();
        if (stdpSystem) {
            // Check if STDP weights are appropriate
            // This would require access to STDP internal state
            suggestions.push_back("Consider tuning STDP parameters (LTP/LTD weights, time constants)");
        }
        
        // Suggest memory optimization
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            size_t capacity = workingMemory->getCapacity();
            if (capacity < brain_->getTotalNeuronCount() / 20) {
                suggestions.push_back("Consider increasing working memory capacity");
            }
        }
        
        return suggestions;
    }
    
    std::string generateDetailedReport() {
        std::stringstream ss;
        
        ss << "=== Detailed NLM Performance Analysis ===" << std::endl;
        ss << std::endl;
        
        // Basic metrics
        auto report = analyzePerformance();
        ss << report.toString() << std::endl;
        
        // Bottleneck analysis
        ss << "=== Bottleneck Analysis ===" << std::endl;
        auto bottlenecks = detectBottlenecks();
        if (bottlenecks.empty()) {
            ss << "No significant bottlenecks detected" << std::endl;
        } else {
            for (size_t i = 0; i < bottlenecks.size(); ++i) {
                ss << (i + 1) << ". " << bottlenecks[i] << std::endl;
            }
        }
        
        ss << std::endl;
        
        // Optimization suggestions
        ss << "=== Optimization Suggestions ===" << std::endl;
        auto suggestions = getOptimizationSuggestions();
        if (suggestions.empty()) {
            ss << "No optimization suggestions" << std::endl;
        } else {
            for (size_t i = 0; i < suggestions.size(); ++i) {
                ss << "• " << suggestions[i] << std::endl;
            }
        }
        
        ss << std::endl;
        
        // Neuron and synapse statistics
        ss << "=== Network Statistics ===" << std::endl;
        ss << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        ss << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        ss << "Total regions: " << brain_->getRegionCount() << std::endl;
        ss << "Total spikes (this step): " << brain_->getFiringNeuronCount() << std::endl;
        ss << "Total spikes (all time): " << brain_->getTotalSpikeCount() << std::endl;
        ss << "Average firing rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
        ss << "Excitatory/Inhibitory ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
        ss << std::endl;
        
        // Memory system statistics
        auto* workingMemory = brain_->getWorkingMemory();
        auto* episodicMemory = brain_->getEpisodicMemory();
        
        if (workingMemory) {
            ss << "Working Memory: " << workingMemory->getActiveTraces() << " active traces (capacity: " 
               << workingMemory->getCapacity() << ")" << std::endl;
        }
        
        if (episodicMemory) {
            ss << "Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes (max: " 
               << episodicMemory->getMaxEpisodes() << ")" << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Real-time performance monitor
class RealTimePerformanceMonitor {
public:
    RealTimePerformanceMonitor(std::shared_ptr<Brain> brain) : brain_(brain) {
        startTime_ = std::chrono::high_resolution_clock::now();
        lastUpdateTime_ = startTime_;
        updateInterval_ = std::chrono::milliseconds(1000);  // 1 second updates
    }
    
    void update() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        // Update performance metrics
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime_);
        auto sinceLastUpdate = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastUpdateTime_);
        
        if (sinceLastUpdate >= updateInterval_) {
            // Calculate performance since last update
            double stepsPerSecond = static_cast<double>(metrics_.getTotalSteps()) / sinceLastUpdate.count();
            double spikesPerSecond = static_cast<double>(brain_->getTotalSpikeCount() - lastSpikeCount_) / sinceLastUpdate.count();
            
            // Store metrics
            PerformanceSnapshot snapshot;
            snapshot.timestamp = elapsed.count();
            snapshot.stepsPerSecond = stepsPerSecond;
            snapshot.spikesPerSecond = spikesPerSecond;
            snapshot.neuronCount = brain_->getTotalNeuronCount();
            snapshot.spikeCount = brain_->getTotalSpikeCount();
            snapshot.memoryUsage = getMemoryUsage();
            snapshot.cpuUsage = getCPUUsage();
            
            snapshots_.push_back(snapshot);
            
            // Keep only last 100 snapshots
            if (snapshots_.size() > 100) {
                snapshots_.erase(snapshots_.begin());
            }
            
            // Update last values
            lastUpdateTime_ = currentTime;
            lastSpikeCount_ = brain_->getTotalSpikeCount();
            
            // Update metrics
            metrics_.recordStep(static_cast<size_t>(stepsPerSecond * updateInterval_.count()));
            metrics_.recordSpikes(static_cast<size_t>(spikesPerSecond * updateInterval_.count()));
        }
    }
    
    struct PerformanceSnapshot {
        double timestamp;
        double stepsPerSecond;
        double spikesPerSecond;
        size_t neuronCount;
        size_t spikeCount;
        double memoryUsage;
        double cpuUsage;
    };
    
    std::vector<PerformanceSnapshot> getSnapshots() const {
        return snapshots_;
    }
    
    double getAverageStepsPerSecond() const {
        if (snapshots_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& snapshot : snapshots_) {
            total += snapshot.stepsPerSecond;
        }
        return total / snapshots_.size();
    }
    
    double getAverageSpikesPerSecond() const {
        if (snapshots_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& snapshot : snapshots_) {
            total += snapshot.spikesPerSecond;
        }
        return total / snapshots_.size();
    }
    
    std::string generatePerformanceChart() const {
        std::stringstream ss;
        
        ss << "=== NLM Performance Chart ===" << std::endl;
        ss << std::endl;
        ss << "Time (s) | Steps/sec | Spikes/sec | Neurons | Memory | CPU" << std::endl;
        ss << "--------|----------|-----------|---------|--------|-----" << std::endl;
        
        // Show last 20 snapshots for readability
        size_t startIdx = snapshots_.empty() ? 0 : std::max(size_t(0), snapshots_.size() - 20);
        
        for (size_t i = startIdx; i < snapshots_.size(); ++i) {
            const auto& snapshot = snapshots_[i];
            ss << std::fixed << std::setprecision(1) << std::setw(7) << snapshot.timestamp << " | "
               << std::setw(9) << snapshot.stepsPerSecond << " | "
               << std::setw(11) << snapshot.spikesPerSecond << " | "
               << std::setw(7) << snapshot.neuronCount << " | "
               << std::setw(7) << snapshot.memoryUsage << " | "
               << std::setw(4) << snapshot.cpuUsage << "%" << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generatePerformanceStatistics() const {
        std::stringstream ss;
        
        ss << "=== Performance Statistics ===" << std::endl;
        ss << std::endl;
        
        if (snapshots_.empty()) {
            ss << "No performance data available" << std::endl;
            return ss.str();
        }
        
        // Calculate statistics
        size_t totalSnapshots = snapshots_.size();
        double avgStepsPerSec = getAverageStepsPerSecond();
        double avgSpikesPerSec = getAverageSpikesPerSecond();
        
        ss << "Total monitoring time: " << (snapshots_.back().timestamp - snapshots_.front().timestamp) << " seconds" << std::endl;
        ss << "Average snapshots per second: " << static_cast<double>(totalSnapshots) / (snapshots_.back().timestamp - snapshots_.front().timestamp) << std::endl;
        ss << "Average steps per second: " << avgStepsPerSec << std::endl;
        ss << "Average spikes per second: " << avgSpikesPerSec << std::endl;
        ss << "Peak steps per second: " << getPeakValue(&PerformanceSnapshot::stepsPerSecond) << std::endl;
        ss << "Peak spikes per second: " << getPeakValue(&PerformanceSnapshot::spikesPerSecond) << std::endl;
        ss << std::endl;
        
        // Stability analysis
        double stepsStdDev = calculateStandardDeviation(&PerformanceSnapshot::stepsPerSecond);
        double spikesStdDev = calculateStandardDeviation(&PerformanceSnapshot::spikesPerSecond);
        
        ss << "Steps/sec standard deviation: " << stepsStdDev << std::endl;
        ss << "Spikes/sec standard deviation: " << spikesStdDev << std::endl;
        
        if (stepsStdDev / avgStepsPerSec < 0.1) {
            ss << "✓ Steps/sec is stable (coefficient of variation < 10%)" << std::endl;
        } else {
            ss << "⚠ Steps/sec shows high variability" << std::endl;
        }
        
        if (spikesStdDev / avgSpikesPerSec < 0.1) {
            ss << "✓ Spikes/sec is stable (coefficient of variation < 10%)" << std::endl;
        } else {
            ss << "⚠ Spikes/sec shows high variability" << std::endl;
        }
        
        return ss.str();
    }
    
private:
    template<typename Member>
    double getPeakValue(Member ptr) const {
        double peak = 0.0;
        for (const auto& snapshot : snapshots_) {
            peak = std::max(peak, (snapshot.*ptr));
        }
        return peak;
    }
    
    template<typename Member>
    double calculateStandardDeviation(Member ptr) const {
        if (snapshots_.size() < 2) return 0.0;
        
        double mean = 0.0;
        for (const auto& snapshot : snapshots_) {
            mean += (snapshot.*ptr);
        }
        mean /= snapshots_.size();
        
        double sumSquares = 0.0;
        for (const auto& snapshot : snapshots_) {
            double diff = (snapshot.*ptr) - mean;
            sumSquares += diff * diff;
        }
        
        return std::sqrt(sumSquares / (snapshots_.size() - 1));
    }
    
    double getMemoryUsage() const {
        // Simplified memory usage estimation
        // In a real implementation, this would use platform-specific APIs
        return 0.0; // Placeholder
    }
    
    double getCPUUsage() const {
        // Simplified CPU usage estimation
        // In a real implementation, this would use platform-specific APIs
        return 0.0; // Placeholder
    }
    
    std::shared_ptr<Brain> brain_;
    PerformanceMetrics metrics_;
    
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    std::chrono::duration<double> updateInterval_;
    
    size_t lastSpikeCount_ = 0;
    std::vector<PerformanceSnapshot> snapshots_;
};

} // namespace nlm