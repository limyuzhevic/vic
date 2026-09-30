#include "Performance.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <string>

namespace nlm {

PerformanceMetrics::PerformanceMetrics()
    : totalSteps(0), totalSpikes(0), averageFiringRate(0.0f), 
      averageSynapticWeight(0.0f), memoryUsage(0), 
      peakMemoryUsage(0), cpuUsage(0.0f), simulationTime(0.0),
      neuronsProcessed(0), synapsesProcessed(0), memoryEvents(0),
      checkpointsCreated(0), replayEvents(0), developmentUpdates(0),
      noveltyEvents(0), curiosityEvents(0), dopamineEvents(0) {}

void PerformanceMetrics::update(const PerformanceMetrics& other) {
    totalSteps += other.totalSteps;
    totalSpikes += other.totalSpikes;
    memoryUsage = std::max(memoryUsage, other.memoryUsage);
    peakMemoryUsage = std::max(peakMemoryUsage, other.peakMemoryUsage);
    simulationTime += other.simulationTime;
    neuronsProcessed += other.neuronsProcessed;
    synapsesProcessed += other.synapsesProcessed;
    memoryEvents += other.memoryEvents;
    checkpointsCreated += other.checkpointsCreated;
    replayEvents += other.replayEvents;
    developmentUpdates += other.developmentUpdates;
    noveltyEvents += other.noveltyEvents;
    curiosityEvents += other.curiosityEvents;
    dopamineEvents += other.dopamineEvents;
    
    // Update derived metrics
    if (totalSteps > 0) {
        averageFiringRate = static_cast<float>(totalSpikes) / totalSteps;
    }
    if (neuronsProcessed > 0) {
        averageSynapticWeight = totalWeightSum / neuronsProcessed;
    }
}

std::string PerformanceMetrics::toString() const {
    std::ostringstream oss;
    oss << "Performance Metrics:\n";
    oss << "  Total Steps: " << totalSteps << "\n";
    oss << "  Total Spikes: " << totalSpikes << "\n";
    oss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << averageFiringRate << " Hz\n";
    oss << "  Average Synaptic Weight: " << std::fixed << std::setprecision(4) 
        << averageSynapticWeight << "\n";
    oss << "  Memory Usage: " << memoryUsage << " bytes\n";
    oss << "  Peak Memory: " << peakMemoryUsage << " bytes\n";
    oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << cpuUsage << "%\n";
    oss << "  Simulation Time: " << std::fixed << std::setprecision(2) 
        << simulationTime << " s\n";
    oss << "  Neurons Processed: " << neuronsProcessed << "\n";
    oss << "  Synapses Processed: " << synapsesProcessed << "\n";
    oss << "  Memory Events: " << memoryEvents << "\n";
    oss << "  Checkpoints Created: " << checkpointsCreated << "\n";
    oss << "  Replay Events: " << replayEvents << "\n";
    oss << "  Development Updates: " << developmentUpdates << "\n";
    oss << "  Novelty Events: " << noveltyEvents << "\n";
    oss << "  Curiosity Events: " << curiosityEvents << "\n";
    oss << "  Dopamine Events: " << dopamineEvents << "\n";
    return oss.str();
}

std::string PerformanceMetrics::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"totalSteps\": " << totalSteps << ",\n";
    oss << "  \"totalSpikes\": " << totalSpikes << ",\n";
    oss << "  \"averageFiringRate\": " << std::fixed << std::setprecision(4) 
        << averageFiringRate << ",\n";
    oss << "  \"averageSynapticWeight\": " << std::fixed << std::setprecision(6) 
        << averageSynapticWeight << ",\n";
    oss << "  \"memoryUsage\": " << memoryUsage << ",\n";
    oss << "  \"peakMemoryUsage\": " << peakMemoryUsage << ",\n";
    oss << "  \"cpuUsage\": " << std::fixed << std::setprecision(2) << cpuUsage << ",\n";
    oss << "  \"simulationTime\": " << std::fixed << std::setprecision(3) 
        << simulationTime << ",\n";
    oss << "  \"neuronsProcessed\": " << neuronsProcessed << ",\n";
    oss << "  \"synapsesProcessed\": " << synapsesProcessed << ",\n";
    oss << "  \"memoryEvents\": " << memoryEvents << ",\n";
    oss << "  \"checkpointsCreated\": " << checkpointsCreated << ",\n";
    oss << "  \"replayEvents\": " << replayEvents << ",\n";
    oss << "  \"developmentUpdates\": " << developmentUpdates << ",\n";
    oss << "  \"noveltyEvents\": " << noveltyEvents << ",\n";
    oss << "  \"curiosityEvents\": " << curiosityEvents << ",\n";
    oss << "  \"dopamineEvents\": " << dopamineEvents << "\n";
    oss << "}\n";
    return oss.str();
}

PerformanceMonitor::PerformanceMonitor() : metrics_(), startTime_(std::chrono::steady_clock::now()) {
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.totalSteps = step;
    metrics_.neuronsProcessed += neuronCount;
    metrics_.synapsesProcessed += synapseCount;
    
    // Estimate weight sum (in real implementation, would track this properly)
    metrics_.totalWeightSum += static_cast<float>(synapseCount) * 0.1f;
    
    updateMemoryUsage();
}

void PerformanceMonitor::recordSpike(NeuronId neuronId) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.totalSpikes++;
    
    // Update spike count per neuron
    spikeCountPerNeuron_[neuronId]++;
    
    updateMemoryUsage();
}

void PerformanceMonitor::recordMemoryEvent(MemoryEventType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.memoryEvents++;
    
    // Track memory event details
    memoryEvents_.push_back({std::chrono::steady_clock::now(), type});
}

void PerformanceMonitor::recordCheckpoint(size_t neurons, size_t synapses, size_t memorySize) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.checkpointsCreated++;
    
    // Track checkpoint details
    checkpointEvents_.push_back({std::chrono::steady_clock::now(), neurons, synapses, memorySize});
    
    updateMemoryUsage();
}

void PerformanceMonitor::recordReplay(size_t episodeCount, size_t neuronsRestored) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.replayEvents++;
    
    // Track replay details
    replayEvents_.push_back({std::chrono::steady_clock::now(), episodeCount, neuronsRestored});
}

void PerformanceMonitor::recordDevelopmentUpdate(DevelopmentalStage stage, size_t neuronsAffected) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.developmentUpdates++;
    
    // Track development details
    developmentEvents_.push_back({std::chrono::steady_clock::now(), stage, neuronsAffected});
}

void PerformanceMonitor::recordNeuromodulationEvent(NeuromodulatorType type, float level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (type) {
        case NeuromodulatorType::Dopamine:
            metrics_.dopamineEvents++;
            dopamineLevelHistory_.push_back(level);
            break;
        case NeuromodulatorType::Novelty:
            metrics_.noveltyEvents++;
            noveltyLevelHistory_.push_back(level);
            break;
        case NeuromodulatorType::Curiosity:
            metrics_.curiosityEvents++;
            curiosityLevelHistory_.push_back(level);
            break;
        default:
            break;
    }
}

void PerformanceMonitor::updateMemoryUsage() {
    // Simple memory estimation - in real implementation would use system calls
    static size_t estimatedNeuronSize = sizeof(Neuron) + sizeof(NeuronState);
    static size_t estimatedSynapseSize = sizeof(Synapse);
    
    // Rough estimate based on processing counts
    metrics_.memoryUsage = metrics_.neuronsProcessed * estimatedNeuronSize + 
                          metrics_.synapsesProcessed * estimatedSynapseSize;
    
    // Add memory for pools, queues, etc.
    metrics_.memoryUsage += metrics_.memoryEvents * 1024; // ~1KB per memory event
    metrics_.memoryUsage += metrics_.checkpointsCreated * 10 * 1024 * 1024; // ~10MB per checkpoint
    
    metrics_.peakMemoryUsage = std::max(metrics_.peakMemoryUsage, metrics_.memoryUsage);
}

void PerformanceMonitor::updateCpuUsage() {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_).count();
    
    if (elapsed > 0) {
        // Simple CPU usage estimation - in real implementation would use system calls
        metrics_.cpuUsage = std::min(100.0f, metrics_.totalSteps * 100.0f / elapsed);
    }
}

void PerformanceMonitor::recordSimulationTime(double time) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.simulationTime += time;
}

PerformanceMetrics PerformanceMonitor::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    updateCpuUsage();
    return metrics_;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_ = PerformanceMetrics();
    spikeCountPerNeuron_.clear();
    memoryEvents_.clear();
    checkpointEvents_.clear();
    replayEvents_.clear();
    developmentEvents_.clear();
    dopamineLevelHistory_.clear();
    noveltyLevelHistory_.clear();
    curiosityLevelHistory_.clear();
}

std::vector<PerformanceEvent> PerformanceMonitor::getMemoryEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryEvents_;
}

std::vector<CheckpointEvent> PerformanceMonitor::getCheckpointEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return checkpointEvents_;
}

std::vector<ReplayEvent> PerformanceMonitor::getReplayEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return replayEvents_;
}

std::vector<DevelopmentEvent> PerformanceMonitor::getDevelopmentEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return developmentEvents_;
}

std::vector<NeuromodulationEvent> PerformanceMonitor::getNeuromodulationEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<NeuromodulationEvent> result;
    
    for (float level : dopamineLevelHistory_) {
        result.push_back({std::chrono::steady_clock::now(), NeuromodulatorType::Dopamine, level});
    }
    for (float level : noveltyLevelHistory_) {
        result.push_back({std::chrono::steady_clock::now(), NeuromodulatorType::Novelty, level});
    }
    for (float level : curiosityLevelHistory_) {
        result.push_back({std::chrono::steady_clock::now(), NeuromodulatorType::Curiosity, level});
    }
    
    return result;
}

PerformanceBenchmark::PerformanceBenchmark() : results_() {
}

PerformanceBenchmark::~PerformanceBenchmark() = default;

void PerformanceBenchmark::addBenchmark(const std::string& name, const PerformanceMetrics& metrics) {
    BenchmarkResult result;
    result.name = name;
    result.metrics = metrics;
    result.timestamp = std::chrono::steady_clock::now();
    results_.push_back(result);
}

PerformanceMetrics PerformanceBenchmark::getAverage() const {
    PerformanceMetrics avg;
    if (results_.empty()) return avg;
    
    for (const auto& result : results_) {
        avg.update(result.metrics);
    }
    
    avg.totalSteps /= results_.size();
    avg.neuronsProcessed /= results_.size();
    avg.synapsesProcessed /= results_.size();
    
    return avg;
}

PerformanceMetrics PerformanceBenchmark::getBestPerformance() const {
    if (results_.empty()) return PerformanceMetrics();
    
    size_t bestIndex = 0;
    float bestScore = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < results_.size(); ++i) {
        // Simple scoring - lower is better (fewer steps, less time, etc.)
        float score = static_cast<float>(results_[i].metrics.totalSteps) +
                     results_[i].metrics.simulationTime * 100.0f +
                     results_[i].metrics.memoryUsage / 1024.0f; // Convert to KB
        
        if (score < bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }
    
    return results_[bestIndex].metrics;
}

std::vector<BenchmarkResult> PerformanceBenchmark::getResults() const {
    return results_;
}

void PerformanceBenchmark::clear() {
    results_.clear();
}

} // namespace nlm
