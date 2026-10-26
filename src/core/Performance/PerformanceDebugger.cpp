#include "PerformanceDebugger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

namespace nlm {

// PerformanceProfiler implementation

struct PerformanceProfiler::Impl {
    struct Measurement {
        std::string name;
        std::chrono::duration<double> duration;
        uint64_t samples;
        double minTime;
        double maxTime;
        double avgTime;
    };
    
    std::vector<Measurement> measurements;
    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> timers;
    bool running;
    double totalDuration;
};

PerformanceProfiler::PerformanceProfiler() : running_(false), totalDuration_(0) {
    pImpl = std::make_unique<Impl>();
    pImpl->running = false;
    pImpl->totalDuration = 0;
}

void PerformanceProfiler::start(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->timers.find(name) != pImpl->timers.end()) {
        return; // Timer already running
    }
    
    pImpl->timers[name] = std::chrono::steady_clock::now();
    running_ = true;
}

void PerformanceProfiler::stop(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->timers.find(name);
    if (it == pImpl->timers.end()) {
        return; // Timer not running
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - it->second);
    
    pImpl->totalDuration += duration.count();
    
    // Update or create measurement
    bool found = false;
    for (auto& measurement : pImpl->measurements) {
        if (measurement.name == name) {
            measurement.duration += duration;
            measurement.samples++;
            measurement.minTime = std::min(measurement.minTime, duration.count());
            measurement.maxTime = std::max(measurement.maxTime, duration.count());
            measurement.avgTime = measurement.duration.count() / measurement.samples;
            found = true;
            break;
        }
    }
    
    if (!found) {
        Measurement newMeasurement(name);
        newMeasurement.duration = duration;
        newMeasurement.samples = 1;
        newMeasurement.minTime = duration.count();
        newMeasurement.maxTime = duration.count();
        newMeasurement.avgTime = duration.count();
        pImpl->measurements.push_back(newMeasurement);
    }
    
    pImpl->timers.erase(it);
    running_ = !pImpl->timers.empty();
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->measurements.clear();
    pImpl->timers.clear();
    running_ = false;
    totalDuration_ = 0;
}

std::vector<PerformanceProfiler::Measurement> PerformanceProfiler::getMeasurements() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->measurements;
}

std::string PerformanceProfiler::getReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Performance Profiler Report\n";
    oss << "==========================\n\n";
    oss << "Total profiled time: " << totalDuration_ << "s\n";
    oss << "Active timers: " << pImpl->timers.size() << "\n\n";
    
    if (pImpl->measurements.empty()) {
        oss << "No measurements recorded.\n";
        return oss.str();
    }
    
    oss << "Measurements:\n";
    for (const auto& measurement : pImpl->measurements) {
        oss << "  " << measurement.name << ":\n";
        oss << "    Samples: " << measurement.samples << "\n";
        oss << "    Total time: " << measurement.duration.count() << "s\n";
        oss << "    Min time: " << measurement.minTime << "s\n";
        oss << "    Max time: " << measurement.maxTime << "s\n";
        oss << "    Avg time: " << measurement.avgTime << "s\n";
        oss << "    Percentage: " << (measurement.duration.count() / totalDuration_) * 100 << "%\n\n";
    }
    
    return oss.str();
}

// MemoryMonitor implementation

#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__) || defined(__unix) || defined(__linux__)
#include <sys/resource.h>
#include <unistd.h>
#endif

struct MemoryMonitor::Impl {
    MemoryStats currentStats;
    struct AllocationRecord {
        size_t size;
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        std::string source;
    };
    std::vector<AllocationRecord> allocations;
    std::vector<AllocationRecord> deallocations;
    bool leaksDetected;
    std::vector<std::string> leakSources;
};

MemoryMonitor::MemoryMonitor() {
    pImpl = std::make_unique<Impl>();
    pImpl->currentStats = MemoryStats();
    pImpl->leaksDetected = false;
}

MemoryMonitor::~MemoryMonitor() = default;

void MemoryMonitor::trackAllocation(size_t size, const std::string& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->currentStats.allocatedMemory += size;
    pImpl->currentStats.peakMemory = std::max(pImpl->currentStats.peakMemory, pImpl->currentStats.allocatedMemory);
    pImpl->currentStats.allocationCount++;
    pImpl->currentStats.lastAllocation = std::chrono::steady_clock::now();
    
    pImpl->allocations.push_back({size, pImpl->currentStats.lastAllocation, source});
    
    // Reset leak detection flag
    pImpl->leaksDetected = false;
}

void MemoryMonitor::trackDeallocation(size_t size, const std::string& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->currentStats.allocatedMemory -= std::min(pImpl->currentStats.allocatedMemory, size);
    pImpl->currentStats.deallocationCount++;
    
    pImpl->deallocations.push_back({size, std::chrono::steady_clock::now(), source});
}

MemoryMonitor::MemoryStats MemoryMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->currentStats;
}

std::string MemoryMonitor::getReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Memory Monitor Report\n";
    oss << "=====================\n\n";
    oss << "Current allocated memory: " << pImpl->currentStats.allocatedMemory << " bytes\n";
    oss << "Peak memory usage: " << pImpl->currentStats.peakMemory << " bytes\n";
    oss << "Total allocations: " << pImpl->currentStats.allocationCount << "\n";
    oss << "Total deallocations: " << pImpl->currentStats.deallocationCount << "\n";
    
    if (pImpl->currentStats.allocatedMemory > 0) {
        oss << "\nWARNING: Memory leak detected!\n";
        pImpl->leaksDetected = true;
        
        // Simple leak detection - if there are allocations without matching deallocations
        // This is a simplistic implementation
        oss << "Unmatched allocations: " << pImpl->currentStats.allocationCount - 
            pImpl->currentStats.deallocationCount << "\n";
    }
    
    if (detectLeaks()) {
        oss << "\nDetailed leaks:\n";
        for (const auto& leak : getLeaks()) {
            oss << "  - " << leak << "\n";
        }
    }
    
    return oss.str();
}

size_t MemoryMonitor::getCurrentMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
#elif defined(__unix__) || defined(__unix) || defined(__linux__)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes
#endif
    return 0;
}

size_t MemoryMonitor::getPeakMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.PeakWorkingSetSize;
#elif defined(__unix__) || defined(__unix) || defined(__linux__)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024; // Convert KB to bytes
#endif
    return 0;
}

bool MemoryMonitor::detectLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Simple leak detection based on allocation/deallocation counts
    if (pImpl->currentStats.allocationCount > pImpl->currentStats.deallocationCount) {
        return true;
    }
    
    // Check if we have unmatched allocations
    size_t unmatchedAllocations = pImpl->allocations.size() - pImpl->deallocations.size();
    if (unmatchedAllocations > 0) {
        return true;
    }
    
    return false;
}

std::vector<std::string> MemoryMonitor::getLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> leaks;
    
    // Simple leak source identification
    if (pImpl->currentStats.allocationCount > pImpl->currentStats.deallocationCount) {
        for (const auto& alloc : pImpl->allocations) {
            // Check if this allocation has a corresponding deallocation
            bool found = false;
            for (const auto& dealloc : pImpl->deallocations) {
                // Simple matching based on source and approximate time
                if (alloc.source == dealloc.source && 
                    std::abs(std::chrono::duration<double>(dealloc.timestamp - alloc.timestamp).count()) < 1.0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                leaks.push_back("Allocation at " + std::to_string(
                    std::chrono::duration<double>(alloc.timestamp.time_since_epoch()).count()) + 
                    "s in " + alloc.source + " size " + std::to_string(alloc.size));
            }
        }
    }
    
    return leaks;
}

// SimulationMonitor implementation

struct SimulationMonitor::Impl {
    PerformanceMetrics currentMetrics;
    std::vector<PerformanceMetrics> history;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool monitoring;
    double stepTimeThreshold;
    size_t memoryThreshold;
    double cpuTimeThreshold;
    std::string performanceAlerts;
};

SimulationMonitor::SimulationMonitor() {
    pImpl = std::make_unique<Impl>();
    pImpl->monitoring = false;
    pImpl->stepTimeThreshold = 1.0; // 1 second per step
    pImpl->memoryThreshold = 1024 * 1024 * 1024; // 1 GB
    pImpl->cpuTimeThreshold = 60.0; // 60 seconds
    pImpl->performanceAlerts = "";
}

void SimulationMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->monitoring = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->performanceAlerts = "";
}

void SimulationMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->monitoring = false;
    
    // Generate final report
    if (!pImpl->performanceAlerts.empty()) {
        std::cerr << "Performance alerts during simulation:\n" << pImpl->performanceAlerts << std::endl;
    }
}

void SimulationMonitor::recordStep(double realTime, double cpuTime, 
                                   size_t spikeCount, size_t neuronCount,
                                   size_t synapseCount, size_t steps) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!pImpl->monitoring) {
        return;
    }
    
    auto currentTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(currentTime - pImpl->startTime).count();
    
    pImpl->currentMetrics.simulationTime = elapsed;
    pImpl->currentMetrics.wallClockTime = cpuTime;
    pImpl->currentMetrics.totalSteps = steps;
    pImpl->currentMetrics.spikeCount = spikeCount;
    pImpl->currentMetrics.neuronCount = neuronCount;
    pImpl->currentMetrics.synapseCount = synapseCount;
    
    if (elapsed > 0) {
        pImpl->currentMetrics.avgStepsPerSecond = steps / elapsed;
        pImpl->currentMetrics.spikeRate = spikeCount / elapsed;
    }
    
    pImpl->currentMetrics.memoryUsage = MemoryMonitor::getCurrentMemoryUsage() / (1024.0 * 1024.0); // MB
    pImpl->currentMetrics.cpuUsage = (cpuTime / elapsed) * 100.0; // percentage
    
    // Check for performance thresholds
    std::string alerts;
    
    if (pImpl->currentMetrics.wallClockTime > pImpl->stepTimeThreshold) {
        alerts += "Step time threshold exceeded: " + std::to_string(pImpl->currentMetrics.wallClockTime) + 
                 "s > " + std::to_string(pImpl->stepTimeThreshold) + "s\n";
    }
    
    if (pImpl->currentMetrics.memoryUsage > pImpl->memoryThreshold / (1024.0 * 1024.0)) {
        alerts += "Memory threshold exceeded: " + std::to_string(pImpl->currentMetrics.memoryUsage) + 
                 "MB > " + std::to_string(pImpl->memoryThreshold / (1024.0 * 1024.0)) + "MB\n";
    }
    
    if (pImpl->currentMetrics.cpuUsage > 90.0) {
        alerts += "High CPU usage: " + std::to_string(pImpl->currentMetrics.cpuUsage) + "%\n";
    }
    
    pImpl->performanceAlerts = alerts;
    
    // Add to history
    pImpl->history.push_back(pImpl->currentMetrics);
}

SimulationMonitor::PerformanceMetrics SimulationMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->currentMetrics;
}

std::string SimulationMonitor::getPerformanceReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Simulation Performance Report\n";
    oss << "==========================\n\n";
    oss << "Total simulation time: " << pImpl->currentMetrics.simulationTime << "s\n";
    oss << "Total steps: " << pImpl->currentMetrics.totalSteps << "\n";
    oss << "Total spikes: " << pImpl->currentMetrics.spikeCount << "\n";
    oss << "Average steps per second: " << pImpl->currentMetrics.avgStepsPerSecond << "\n";
    oss << "Average spike rate: " << pImpl->currentMetrics.spikeRate << " spikes/s\n";
    oss << "Average memory usage: " << pImpl->currentMetrics.memoryUsage << " MB\n";
    oss << "Average CPU usage: " << pImpl->currentMetrics.cpuUsage << "%\n\n";
    
    if (!pImpl->performanceAlerts.empty()) {
        oss << "Performance Alerts:\n";
        oss << pImpl->performanceAlerts;
    }
    
    return oss.str();
}

std::vector<SimulationMonitor::PerformanceMetrics> SimulationMonitor::getHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->history;
}

void SimulationMonitor::setStepTimeThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->stepTimeThreshold = threshold;
}

void SimulationMonitor::setMemoryThreshold(size_t threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->memoryThreshold = threshold;
}

void SimulationMonitor::setCPUTimeThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->cpuTimeThreshold = threshold;
}

bool SimulationMonitor::checkPerformanceThresholds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !pImpl->performanceAlerts.empty();
}

std::string SimulationMonitor::getPerformanceAlerts() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->performanceAlerts;
}

// NeuralPerformanceAnalyzer implementation

struct NeuralPerformanceAnalyzer::Impl {
    std::map<size_t, NeuronPerformance> neuronPerformances;
    std::map<size_t, SynapsePerformance> synapsePerformances;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

NeuralPerformanceAnalyzer::NeuralPerformanceAnalyzer() {
    pImpl = std::make_unique<Impl>();
    pImpl->startTime = std::chrono::steady_clock::now();
}

void NeuralPerformanceAnalyzer::updateNeuronPerformance(size_t neuronId, const NeuronState& state, 
                                                      double timestep, uint64_t spikeCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->neuronPerformances.find(neuronId);
    if (it == pImpl->neuronPerformances.end()) {
        it = pImpl->neuronPerformances.emplace(neuronId, NeuronPerformance(neuronId)).first;
    }
    
    NeuronPerformance& perf = it->second;
    perf.spikeCount += spikeCount;
    double firingRate = (state.firingRate != 0.0f) ? state.firingRate : (spikeCount / timestep);
    
    if (perf.samples == 0) {
        perf.avgFiringRate = firingRate;
        perf.maxFiringRate = firingRate;
    } else {
        perf.avgFiringRate = (perf.avgFiringRate * perf.samples + firingRate) / (perf.samples + 1);
    }
    perf.maxFiringRate = std::max(perf.maxFiringRate, firingRate);
    
    perf.avgMembranePotential = (perf.avgMembranePotential * perf.samples + state.membranePotential) / (perf.samples + 1);
    perf.avgThreshold = (perf.avgThreshold * perf.samples + state.threshold) / (perf.samples + 1);
    
    perf.firingHistory.push_back(firingRate);
    if (perf.firingHistory.size() > 100) {
        perf.firingHistory.erase(perf.firingHistory.begin());
    }
    
    perf.samples++;
}

void NeuralPerformanceAnalyzer::updateSynapsePerformance(size_t synapseId, size_t sourceId, size_t targetId,
                                                        SynapticWeight weight, double delay, 
                                                        bool transmissionSuccess) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->synapsePerformances.find(synapseId);
    if (it == pImpl->synapsePerformances.end()) {
        it = pImpl->synapsePerformances.emplace(synapseId, SynapsePerformance(synapseId, sourceId, targetId)).first;
    }
    
    SynapsePerformance& perf = it->second;
    perf.avgWeight = (perf.avgWeight * perf.transmissionCount + weight) / (perf.transmissionCount + 1);
    perf.avgTransmissionDelay = (perf.avgTransmissionDelay * perf.transmissionCount + delay) / (perf.transmissionCount + 1);
    perf.transmissionCount++;
    perf.avgTransmissionSuccessRate = (perf.avgTransmissionSuccessRate * (perf.transmissionCount - 1) + 
                                     (transmissionSuccess ? 1.0 : 0.0)) / perf.transmissionCount;
}

std::vector<NeuralPerformanceAnalyzer::NeuronPerformance> NeuralPerformanceAnalyzer::getTopNeuronsByFiringRate(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<NeuronPerformance> topNeurons;
    for (const auto& pair : pImpl->neuronPerformances) {
        topNeurons.push_back(pair.second);
    }
    
    std::sort(topNeurons.begin(), topNeurons.end(),
              [](const NeuronPerformance& a, const NeuronPerformance& b) {
                  return a.avgFiringRate > b.avgFiringRate;
              });
    
    if (topNeurons.size() > limit) {
        topNeurons.resize(limit);
    }
    
    return topNeurons;
}

std::vector<NeuralPerformanceAnalyzer::NeuronPerformance> NeuralPerformanceAnalyzer::getTopNeuronsByVariability(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<NeuronPerformance> topNeurons;
    for (const auto& pair : pImpl->neuronPerformances) {
        topNeurons.push_back(pair.second);
    }
    
    std::sort(topNeurons.begin(), topNeurons.end(),
              [](const NeuronPerformance& a, const NeuronPerformance& b) {
                  double varA = calculateVariability(a.firingHistory);
                  double varB = calculateVariability(b.firingHistory);
                  return varA > varB;
              });
    
    if (topNeurons.size() > limit) {
        topNeurons.resize(limit);
    }
    
    return topNeurons;
}

std::vector<NeuralPerformanceAnalyzer::SynapsePerformance> NeuralPerformanceAnalyzer::getTopSynapsesByUsage(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<SynapsePerformance> topSynapses;
    for (const auto& pair : pImpl->synapsePerformances) {
        topSynapses.push_back(pair.second);
    }
    
    std::sort(topSynapses.begin(), topSynapses.end(),
              [](const SynapsePerformance& a, const SynapsePerformance& b) {
                  return a.transmissionCount > b.transmissionCount;
              });
    
    if (topSynapses.size() > limit) {
        topSynapses.resize(limit);
    }
    
    return topSynapses;
}

std::string NeuralPerformanceAnalyzer::getPerformanceInsights() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Neural Performance Analysis\n";
    oss << "==========================\n\n";
    
    oss << "Total neurons analyzed: " << pImpl->neuronPerformances.size() << "\n";
    oss << "Total synapses analyzed: " << pImpl->synapsePerformances.size() << "\n\n";
    
    if (!pImpl->neuronPerformances.empty()) {
        double totalFiringRate = 0;
        for (const auto& pair : pImpl->neuronPerformances) {
            totalFiringRate += pair.second.avgFiringRate;
        }
        oss << "Average firing rate across all neurons: " << (totalFiringRate / pImpl->neuronPerformances.size()) << " Hz\n";
        
        double avgMembranePotential = 0;
        double avgThreshold = 0;
        for (const auto& pair : pImpl->neuronPerformances) {
            avgMembranePotential += pair.second.avgMembranePotential;
            avgThreshold += pair.second.avgThreshold;
        }
        oss << "Average membrane potential: " << (avgMembranePotential / pImpl->neuronPerformances.size()) << " mV\n";
        oss << "Average firing threshold: " << (avgThreshold / pImpl->neuronPerformances.size()) << " mV\n\n";
    }
    
    if (!pImpl->synapsePerformances.empty()) {
        double totalWeight = 0;
        double totalDelay = 0;
        for (const auto& pair : pImpl->synapsePerformances) {
            totalWeight += pair.second.avgWeight;
            totalDelay += pair.second.avgTransmissionDelay;
        }
        oss << "Average synaptic weight: " << (totalWeight / pImpl->synapsePerformances.size()) << "\n";
        oss << "Average transmission delay: " << (totalDelay / pImpl->synapsePerformances.size()) << " steps\n";
        oss << "Average transmission success rate: " << calculateSuccessRate(pImpl->synapsePerformances) << "%\n\n";
    }
    
    return oss.str();
}

std::string NeuralPerformanceAnalyzer::getBottleneckAnalysis() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Neural Network Bottleneck Analysis\n";
    oss << "=================================\n\n";
    
    // Find neurons with high firing variability (potential bottleneck)
    std::vector<NeuronPerformance> variableNeurons;
    for (const auto& pair : pImpl->neuronPerformances) {
        double variability = calculateVariability(pair.second.firingHistory);
        if (variability > 1.0) { // High variability threshold
            variableNeurons.push_back(pair.second);
        }
    }
    
    if (!variableNeurons.empty()) {
        oss << "High variability neurons (potential instability): " << variableNeurons.size() << "\n";
        for (const auto& neuron : variableNeurons) {
            oss << "  Neuron " << neuron.neuronId << ": variability = " << calculateVariability(neuron.firingHistory) << "\n";
        }
    }
    
    // Find synapses with low success rate (potential bottleneck)
    std::vector<SynapsePerformance> problematicSynapses;
    for (const auto& pair : pImpl->synapsePerformances) {
        if (pair.second.avgTransmissionSuccessRate < 0.8) { // Low success rate threshold
            problematicSynapses.push_back(pair.second);
        }
    }
    
    if (!problematicSynapses.empty()) {
        oss << "\nProblematic synapses (low success rate): " << problematicSynapses.size() << "\n";
        for (const auto& synapse : problematicSynapses) {
            oss << "  Synapse " << synapse.synapseId << " (" << synapse.sourceId << " -> " << synapse.targetId << ")\n";
            oss << "    Success rate: " << synapse.avgTransmissionSuccessRate * 100 << "%\n";
        }
    }
    
    return oss.str();
}

std::string NeuralPerformanceAnalyzer::getOptimizationRecommendations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Optimization Recommendations\n";
    oss << "==========================\n\n";
    
    // Check for performance issues
    if (pImpl->neuronPerformances.size() > 100) {
        oss << "1. Consider reducing neuron count if firing rates are too high\n";
    }
    
    double avgVariability = 0;
    size_t count = 0;
    for (const auto& pair : pImpl->neuronPerformances) {
        if (!pair.second.firingHistory.empty()) {
            avgVariability += calculateVariability(pair.second.firingHistory);
            count++;
        }
    }
    if (count > 0) {
        avgVariability /= count;
        if (avgVariability > 2.0) {
            oss << "2. Consider adjusting synaptic weights to reduce neural variability\n";
        }
    }
    
    double avgSuccessRate = calculateSuccessRate(pImpl->synapsePerformances);
    if (avgSuccessRate < 0.9) {
        oss << "3. Consider improving synaptic reliability to increase success rate\n";
    }
    
    if (pImpl->neuronPerformances.empty() && pImpl->synapsePerformances.empty()) {
        oss << "No performance data available for optimization recommendations\n";
    }
    
    return oss.str();
}

double NeuralPerformanceAnalyzer::calculateVariability(const std::vector<double>& data) const {
    if (data.size() < 2) return 0.0;
    
    double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    double variance = 0.0;
    for (double val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    return std::sqrt(variance);
}

double NeuralPerformanceAnalyzer::calculateSuccessRate(const std::map<size_t, SynapsePerformance>& synapses) const {
    if (synapses.empty()) return 0.0;
    
    double totalSuccess = 0;
    double totalPossible = 0;
    for (const auto& pair : synapses) {
        totalSuccess += pair.second.avgTransmissionSuccessRate * pair.second.transmissionCount;
        totalPossible += pair.second.transmissionCount;
    }
    
    return totalSuccess / totalPossible;
}

void NeuralPerformanceAnalyzer::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->neuronPerformances.clear();
    pImpl->synapsePerformances.clear();
}

// DebugLogger implementation

struct DebugLogger::Impl {
    std::vector<LogEvent> events;
    std::chrono::time_point<std::chrono::steady_clock> sessionStart;
    std::mutex eventMutex;
};

DebugLogger::DebugLogger() {
    pImpl = std::make_unique<Impl>();
    pImpl->sessionStart = std::chrono::steady_clock::now();
}

void DebugLogger::log(const std::string& level, const std::string& source, 
                     const std::string& message,
                     const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    LogEvent event(level, source, message);
    event.timestamp = std::chrono::steady_clock::now();
    event.metadata = metadata;
    
    pImpl->events.push_back(event);
    
    // Also output to console for real-time debugging
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "
              << level << " [" << source << "] " << message << std::endl;
}

void DebugLogger::debug(const std::string& source, const std::string& message) {
    log("DEBUG", source, message);
}

void DebugLogger::info(const std::string& source, const std::string& message) {
    log("INFO", source, message);
}

void DebugLogger::warn(const std::string& source, const std::string& message) {
    log("WARN", source, message);
}

void DebugLogger::error(const std::string& source, const std::string& message) {
    log("ERROR", source, message);
}

void DebugLogger::logStepPerformance(const std::string& source, double stepTime, 
                                    size_t stepsPerSecond) {
    std::map<std::string, std::string> metadata;
    metadata["step_time"] = std::to_string(stepTime);
    metadata["steps_per_second"] = std::to_string(stepsPerSecond);
    
    log("PERF", source, "Step performance recorded", metadata);
}

void DebugLogger::logMemoryUsage(const std::string& source, size_t memoryBytes) {
    std::map<std::string, std::string> metadata;
    metadata["memory_bytes"] = std::to_string(memoryBytes);
    metadata["memory_mb"] = std::to_string(memoryBytes / (1024.0 * 1024.0));
    
    log("MEM", source, "Memory usage recorded", metadata);
}

void DebugLogger::logNeuralActivity(const std::string& source, size_t activeNeurons,
                                   size_t totalSpikeCount) {
    std::map<std::string, std::string> metadata;
    metadata["active_neurons"] = std::to_string(activeNeurons);
    metadata["total_spike_count"] = std::to_string(totalSpikeCount);
    
    log("NEURAL", source, "Neural activity recorded", metadata);
}

std::vector<DebugLogger::LogEvent> DebugLogger::getEvents(const std::string& level,
                                                         const std::string& source,
                                                         const std::chrono::time_point<std::chrono::steady_clock>& startTime,
                                                         const std::chrono::time_point<std::chrono::steady_clock>& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    std::vector<LogEvent> filteredEvents;
    
    for (const auto& event : pImpl->events) {
        bool matchesLevel = level.empty() || event.level == level;
        bool matchesSource = source.empty() || event.source == source;
        bool matchesTime = (startTime.empty() || event.timestamp >= startTime) &&
                          (endTime.empty() || event.timestamp <= endTime);
        
        if (matchesLevel && matchesSource && matchesTime) {
            filteredEvents.push_back(event);
        }
    }
    
    return filteredEvents;
}

std::string DebugLogger::getLogReport() const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    std::ostringstream oss;
    oss << "Debug Log Report\n";
    oss << "=================\n\n";
    oss << "Total events: " << pImpl->events.size() << "\n";
    oss << "Session duration: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - 
                                                              pImpl->sessionStart).count() << "s\n\n";
    
    // Count events by level
    std::map<std::string, size_t> levelCounts;
    for (const auto& event : pImpl->events) {
        levelCounts[event.level]++;
    }
    
    oss << "Events by level:\n";
    for (const auto& pair : levelCounts) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    // Count events by source
    std::map<std::string, size_t> sourceCounts;
    for (const auto& event : pImpl->events) {
        sourceCounts[event.source]++;
    }
    
    oss << "\nEvents by source:\n";
    for (const auto& pair : sourceCounts) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string DebugLogger::getEventSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    std::ostringstream oss;
    oss << "Event Summary\n";
    oss << "============\n\n";
    
    if (pImpl->events.empty()) {
        oss << "No events recorded.\n";
        return oss.str();
    }
    
    oss << "Recent events (last 10):\n";
    size_t recentCount = std::min(pImpl->events.size(), static_cast<size_t>(10));
    for (size_t i = pImpl->events.size() - recentCount; i < pImpl->events.size(); ++i) {
        const auto& event = pImpl->events[i];
        auto timeSince = std::chrono::duration<double>(std::chrono::steady_clock::now() - 
                                                      event.timestamp).count();
        oss << "  " << std::fixed << std::setprecision(1) << timeSince << "s ago: "
            << event.level << " [" << event.source << "] " << event.message << "\n";
    }
    
    return oss.str();
}

void DebugLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    pImpl->events.clear();
}

void DebugLogger::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Debug Log Export\n";
    file << "=================\n\n";
    
    for (const auto& event : pImpl->events) {
        auto timeSinceEpoch = event.timestamp.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeSinceEpoch);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceEpoch) - seconds;
        
        file << "[" << seconds.count() << "." << milliseconds.count() << "s] "
             << event.level << " [" << event.source << "] " << event.message << "\n";
        
        for (const auto& meta : event.metadata) {
            file << "  Metadata: " << meta.first << " = " << meta.second << "\n";
        }
        file << "\n";
    }
}

void DebugLogger::loadFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    LogEvent currentEvent("", "", "");
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        if (line.find("[.") != std::string::npos) {
            // Timestamp line - start of new event
            if (!currentEvent.message.empty()) {
                pImpl->events.push_back(currentEvent);
            }
            
            // Parse timestamp (simplified)
            size_t bracketEnd = line.find("]");
            if (bracketEnd != std::string::npos) {
                std::string rest = line.substr(bracketEnd + 1);
                size_t bracketStart = rest.find("[") + 1;
                size_t bracketEnd2 = rest.find("]", bracketStart);
                if (bracketEnd2 != std::string::npos) {
                    currentEvent.level = rest.substr(0, bracketStart - 1);
                    currentEvent.source = rest.substr(bracketStart, bracketEnd2 - bracketStart);
                }
                currentEvent.message = rest.substr(bracketEnd2 + 2);
            }
        } else if (line.find("Metadata: ") != std::string::npos) {
            // Metadata line
            size_t colonPos = line.find(": ");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(9, colonPos - 9);
                std::string value = line.substr(colonPos + 2);
                currentEvent.metadata[key] = value;
            }
        }
    }
    
    // Add last event
    if (!currentEvent.message.empty()) {
        pImpl->events.push_back(currentEvent);
    }
}

// PerformanceDebugger implementation

PerformanceDebugger::PerformanceDebugger() {
    pImpl = std::make_unique<Impl>();
    pImpl->monitoring = false;
    pImpl->stepTimeThreshold = 1.0;
    pImpl->memoryThreshold = 1024 * 1024 * 1024; // 1 GB
    pImpl->cpuTimeThreshold = 60.0; // 60 seconds
    pImpl->performanceAlerts = "";
}

PerformanceProfiler& PerformanceDebugger::getProfiler() {
    if (!pImpl->profiler) {
        pImpl->profiler = std::make_unique<PerformanceProfiler>();
    }
    return *pImpl->profiler;
}

MemoryMonitor& PerformanceDebugger::getMemoryMonitor() {
    if (!pImpl->memoryMonitor) {
        pImpl->memoryMonitor = std::make_unique<MemoryMonitor>();
    }
    return *pImpl->memoryMonitor;
}

SimulationMonitor& PerformanceDebugger::getSimulationMonitor() {
    if (!pImpl->simulationMonitor) {
        pImpl->simulationMonitor = std::make_unique<SimulationMonitor>();
    }
    return *pImpl->simulationMonitor;
}

NeuralPerformanceAnalyzer& PerformanceDebugger::getNeuralAnalyzer() {
    if (!pImpl->neuralAnalyzer) {
        pImpl->neuralAnalyzer = std::make_unique<NeuralPerformanceAnalyzer>();
    }
    return *pImpl->neuralAnalyzer;
}

DebugLogger& PerformanceDebugger::getDebugLogger() {
    if (!pImpl->debugLogger) {
        pImpl->debugLogger = std::make_unique<DebugLogger>();
    }
    return *pImpl->debugLogger;
}

void PerformanceDebugger::startPerformanceMonitoring() {
    pImpl->monitoring = true;
    getSimulationMonitor().startMonitoring();
    getDebugLogger().info("PERFORMANCE", "Performance monitoring started");
}

void PerformanceDebugger::stopPerformanceMonitoring() {
    pImpl->monitoring = false;
    getSimulationMonitor().stopMonitoring();
    getDebugLogger().info("PERFORMANCE", "Performance monitoring stopped");
    
    // Generate final reports
    std::string perfReport = getPerformanceReport();
    std::string debugReport = getDebugReport();
    
    std::cout << "=== Performance Report ===\n" << perfReport << std::endl;
    std::cout << "=== Debug Report ===\n" << debugReport << std::endl;
}

void PerformanceDebugger::logSimulationStep(size_t step, double realTime, double cpuTime) {
    if (!pImpl->monitoring) return;
    
    getSimulationMonitor().recordStep(realTime, cpuTime, 
                                      getNeuralAnalyzer().getTopNeuronsByFiringRate().size(),
                                      getNeuralAnalyzer().getTopNeuronsByFiringRate().size(),
                                      getNeuralAnalyzer().getTopSynapsesByUsage().size(),
                                      step);
    
    getDebugLogger().logStepPerformance("SIMULATION", realTime, 1.0 / realTime);
}

void PerformanceDebugger::logNeuralState(size_t step, const Brain& brain) {
    if (!pImpl->monitoring) return;
    
    getNeuralAnalyzer().updateNeuronPerformance(0, brain.getRegion(RegionId(0))->getAllNeurons()[0]->getState(),
                                              0.001, brain.getTotalSpikeCount());
    
    getDebugLogger().logNeuralActivity("NEURAL", brain.getActiveNeuronCount(),
                                      brain.getTotalSpikeCount());
}

std::string PerformanceDebugger::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "=== Comprehensive Performance Report ===\n\n";
    
    if (pImpl->profiler) {
        oss << "--- Profiler Report ---\n";
        oss << pImpl->profiler->getReport() << "\n";
    }
    
    if (pImpl->memoryMonitor) {
        oss << "--- Memory Monitor Report ---\n";
        oss << pImpl->memoryMonitor->getReport() << "\n";
    }
    
    if (pImpl->simulationMonitor) {
        oss << "--- Simulation Monitor Report ---\n";
        oss << pImpl->simulationMonitor->getPerformanceReport() << "\n";
    }
    
    if (pImpl->neuralAnalyzer) {
        oss << "--- Neural Performance Analysis ---\n";
        oss << pImpl->neuralAnalyzer->getPerformanceInsights() << "\n";
        oss << pImpl->neuralAnalyzer->getBottleneckAnalysis() << "\n";
        oss << pImpl->neuralAnalyzer->getOptimizationRecommendations() << "\n";
    }
    
    if (pImpl->debugLogger) {
        oss << "--- Debug Logger Report ---\n";
        oss << pImpl->debugLogger->getLogReport() << "\n";
    }
    
    return oss.str();
}

std::string PerformanceDebugger::generateDebugReport() const {
    std::ostringstream oss;
    oss << "=== Debug Report ===\n\n";
    
    if (pImpl->debugLogger) {
        oss << "--- Event Summary ---\n";
        oss << pImpl->debugLogger->getEventSummary() << "\n";
    }
    
    oss << "--- Performance Alerts ---\n";
    oss << "Simulation alerts: " << (pImpl->simulationMonitor ? pImpl->simulationMonitor->getPerformanceAlerts() : "N/A") << "\n";
    
    return oss.str();
}

std::string PerformanceDebugger::generateOptimizationReport() const {
    std::ostringstream oss;
    oss << "=== Optimization Report ===\n\n";
    
    if (pImpl->neuralAnalyzer) {
        oss << pImpl->neuralAnalyzer->getOptimizationRecommendations() << "\n";
    }
    
    oss << "=== Key Performance Metrics ===\n";
    
    if (pImpl->profiler) {
        double totalTime = pImpl->profiler->getTotalTime();
        oss << "Total profiled time: " << totalTime << "s\n";
    }
    
    if (pImpl->memoryMonitor) {
        auto stats = pImpl->memoryMonitor->getStats();
        oss << "Current memory usage: " << stats.allocatedMemory << " bytes\n";
        oss << "Peak memory usage: " << stats.peakMemory << " bytes\n";
    }
    
    if (pImpl->simulationMonitor) {
        auto metrics = pImpl->simulationMonitor->getCurrentMetrics();
        oss << "Average steps per second: " << metrics.avgStepsPerSecond << "\n";
        oss << "Average spike rate: " << metrics.spikeRate << " spikes/s\n";
        oss << "Current memory usage: " << metrics.memoryUsage << " MB\n";
        oss << "Current CPU usage: " << metrics.cpuUsage << "%\n";
    }
    
    return oss.str();
}

void PerformanceDebugger::enableRealTimeMonitoring(bool enable) {
    pImpl->monitoring = enable;
    if (enable) {
        startPerformanceMonitoring();
    } else {
        stopPerformanceMonitoring();
    }
}

void PerformanceDebugger::setReportInterval(size_t interval) {
    pImpl->reportInterval = interval;
}

void PerformanceDebugger::setAlertThresholds(double stepTime, size_t memory, double cpu) {
    if (pImpl->simulationMonitor) {
        pImpl->simulationMonitor->setStepTimeThreshold(stepTime);
        pImpl->simulationMonitor->setMemoryThreshold(memory);
        pImpl->simulationMonitor->setCPUTimeThreshold(cpu);
    }
}

void PerformanceDebugger::reset() {
    if (pImpl->profiler) pImpl->profiler->reset();
    if (pImpl->neuralAnalyzer) pImpl->neuralAnalyzer->reset();
    if (pImpl->debugLogger) pImpl->debugLogger->clear();
    pImpl->monitoring = false;
    pImpl->performanceAlerts = "";
}

void PerformanceDebugger::pause() {
    pImpl->monitoring = false;
    if (pImpl->simulationMonitor) {
        pImpl->simulationMonitor->stopMonitoring();
    }
}

void PerformanceDebugger::resume() {
    pImpl->monitoring = true;
    if (pImpl->simulationMonitor) {
        pImpl->simulationMonitor->startMonitoring();
    }
}

bool PerformanceDebugger::isMonitoring() const {
    return pImpl->monitoring;
}

} // namespace nlm
