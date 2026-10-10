#include "PerformanceMonitor.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

struct PerformanceMonitor::Impl {
    // Configuration
    bool monitoringEnabled = false;
    double samplingInterval = 0.1;  // seconds
    bool realTimeDashboard = false;
    std::shared_ptr<Config> config;
    
    // Data storage
    mutable std::deque<PerformanceSnapshot> historicalSnapshots;
    mutable std::deque<PerformanceSnapshot> realTimeSnapshots;
    mutable std::deque<Bottleneck> bottleneckHistory;
    
    // Timing data
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
    double accumulatedCPUTime = 0.0;
    uint64_t updateCount = 0;
    
    // Brain system performance tracking
    struct NeuronPerformance {
        std::string operation;
        double totalDurationMs = 0.0;
        uint64_t count = 0;
        double avgDurationMs = 0.0;
        double p95DurationMs = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastAccess;
    };
    
    struct SynapsePerformance {
        SynapseId id;
        double totalUpdateDurationMs = 0.0;
        uint64_t updateCount = 0;
        double totalWeightChange = 0.0;
        double avgDurationMs = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
    };
    
    struct PlasticityPerformance {
        std::string ruleType;
        double totalUpdateDurationMs = 0.0;
        uint64_t updateCount = 0;
        double totalLearningRate = 0.0;
        double avgDurationMs = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate;
    };
    
    struct NeuromodulationPerformance {
        std::string signalType;
        double totalDurationMs = 0.0;
        uint64_t applicationCount = 0;
        double totalMagnitude = 0.0;
        double avgDurationMs = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastApplication;
    };
    
    struct MemoryPerformance {
        std::string operation;
        double totalDurationMs = 0.0;
        uint64_t bytesTransferred = 0;
        uint64_t accessCount = 0;
        double avgDurationMs = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastAccess;
    };
    
    // Performance tracking data
    std::unordered_map<NeuronId, NeuronPerformance> neuronPerformances;
    std::unordered_map<SynapseId, SynapsePerformance> synapsePerformances;
    std::unordered_map<std::string, PlasticityPerformance> plasticityPerformances;
    std::unordered_map<std::string, NeuromodulationPerformance> neuromodulationPerformances;
    std::unordered_map<std::string, MemoryPerformance> memoryPerformances;
    
    // Profiling data
    std::map<std::string, std::chrono::duration<double, std::milli>> profileDurations;
    std::map<std::string, uint64_t> profileCounts;
    
    // System metrics
    struct SystemMetricsImpl {
        double peakMemoryUsageMB = 0.0;
        double totalMemoryUsageMB = 0.0;
        double cpuAverage = 0.0;
        double cpuTimePerUpdate = 0.0;
        double readSpeedMBps = 0.0;
        double writeSpeedMBps = 0.0;
        uint64_t threadCount = 0;
        uint64_t totalThreads = std::thread::hardware_concurrency();
    } systemMetrics;
    
    // Alert thresholds
    struct AlertThreshold {
        std::string metric;
        double threshold;
        std::string severity;  // "info", "warning", "error", "critical"
    };
    
    std::vector<AlertThreshold> alertThresholds;
    
    // Bottleneck detection parameters
    struct DetectionParams {
        double cpuThreshold = 80.0;      // Percentage
        double memoryThreshold = 85.0;   // Percentage
        double latencyThresholdMs = 10.0; // Milliseconds
        double spikeThreshold = 3.0;     // Standard deviations
        uint64_t minSamples = 10;        // Minimum samples for trend analysis
    } detectionParams;
    
    // Integration with brain systems
    std::weak_ptr<Brain> brain;
    Timestamp lastBrainMetricsUpdate = 0;
    
    Impl() {
        lastUpdate = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() {
        // Cleanup
    }
};

bool PerformanceMonitor::initialize(std::shared_ptr<Config> config) {
    pImpl->config = config;
    
    if (pImpl->config) {
        pImpl->monitoringEnabled = pImpl->config->getOr<bool>("performance.monitoring.enabled", true);
        pImpl->samplingInterval = pImpl->config->getOr<double>("performance.sampling_interval", 0.1);
        pImpl->realTimeDashboard = pImpl->config->getOr<bool>("performance.dashboard.enabled", false);
        pImpl->detectionParams.cpuThreshold = pImpl->config->getOr<double>("performance.bottleneck.cpu_threshold", 80.0);
        pImpl->detectionParams.memoryThreshold = pImpl->config->getOr<double>("performance.bottleneck.memory_threshold", 85.0);
        pImpl->detectionParams.latencyThresholdMs = pImpl->config->getOr<double>("performance.bottleneck.latency_threshold", 10.0);
    }
    
    pImpl->historicalSnapshots.clear();
    pImpl->historicalSnapshots.reserve(1000);
    
    pImpl->neuronPerformances.clear();
    pImpl->synapsePerformances.clear();
    pImpl->plasticityPerformances.clear();
    pImpl->neuromodulationPerformances.clear();
    pImpl->memoryPerformances.clear();
    
    NLM_LOG_INFO("Performance monitoring initialized");
    if (pImpl->config) {
        NLM_LOG_INFO("  Monitoring enabled: " + std::string(pImpl->monitoringEnabled ? "yes" : "no"));
        NLM_LOG_INFO("  Sampling interval: " + std::to_string(pImpl->samplingInterval) + "s");
    }
    
    return true;
}

void PerformanceMonitor::shutdown() {
    if (!pImpl) return;
    
    NLM_LOG_INFO("Performance monitoring shutdown");
}

void PerformanceMonitor::update() {
    if (!pImpl->monitoringEnabled) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastUpdate).count();
    
    if (elapsed < pImpl->samplingInterval && !pImpl->realTimeDashboard) {
        return;  // Not time to update yet
    }
    
    // Create performance snapshot
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Update system metrics
    snapshot.system = getSystemMetrics();
    
    // Update brain metrics if brain is available
    if (auto brainPtr = pImpl->brain.lock()) {
        snapshot.brain = getBrainMetrics(brainPtr.get());
    }
    
    // Update integration metrics
    snapshot.integration = getIntegrationMetrics();
    
    // Update plasticity metrics
    snapshot.plasticity = getPlasticityMetrics();
    
    // Store snapshot
    pImpl->historicalSnapshots.push_back(snapshot);
    if (pImpl->historicalSnapshots.size() > 1000) {
        pImpl->historicalSnapshots.pop_front();
    }
    
    // Real-time snapshots for dashboard
    if (pImpl->realTimeDashboard) {
        pImpl->realTimeSnapshots.push_back(snapshot);
        if (pImpl->realTimeSnapshots.size() > 100) {
            pImpl->realTimeSnapshots.pop_front();
        }
    }
    
    // Detect bottlenecks
    auto bottlenecks = detectBottlenecksImpl();
    for (const auto& bottleneck : bottlenecks) {
        pImpl->bottleneckHistory.push_back(bottleneck);
    }
    
    // Clean old bottleneck history
    auto cutoff = snapshot.timestamp - 3600000;  // Last hour
    while (!pImpl->bottleneckHistory.empty() && 
           pImpl->bottleneckHistory.front().detectionTime < cutoff) {
        pImpl->bottleneckHistory.pop_front();
    }
    
    pImpl->lastUpdate = now;
    pImpl->updateCount++;
}

void PerformanceMonitor::trackNeuronOperation(NeuronId neuronId, const std::string& operation,
                                              double durationMs, double memoryMB) {
    if (!pImpl->monitoringEnabled) return;
    
    auto& perf = pImpl->neuronPerformances[neuronId];
    perf.operation = operation;
    perf.totalDurationMs += durationMs;
    perf.count++;
    perf.avgDurationMs = perf.totalDurationMs / perf.count;
    perf.lastAccess = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::trackSynapseUpdate(SynapseId synapseId, double durationMs,
                                           float weightChange) {
    if (!pImpl->monitoringEnabled) return;
    
    auto& perf = pImpl->synapsePerformances[synapseId];
    perf.id = synapseId;
    perf.totalUpdateDurationMs += durationMs;
    perf.updateCount++;
    perf.totalWeightChange += std::abs(weightChange);
    perf.avgDurationMs = perf.totalUpdateDurationMs / perf.updateCount;
    perf.lastUpdate = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::trackPlasticityOperation(const std::string& ruleType,
                                                 double durationMs, float learningRate) {
    if (!pImpl->monitoringEnabled) return;
    
    auto& perf = pImpl->plasticityPerformances[ruleType];
    perf.ruleType = ruleType;
    perf.totalUpdateDurationMs += durationMs;
    perf.updateCount++;
    perf.totalLearningRate += learningRate;
    perf.avgDurationMs = perf.totalUpdateDurationMs / perf.updateCount;
    perf.lastUpdate = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::trackNeuromodulation(const std::string& signalType,
                                              double durationMs, float magnitude) {
    if (!pImpl->monitoringEnabled) return;
    
    auto& perf = pImpl->neuromodulationPerformances[signalType];
    perf.signalType = signalType;
    perf.totalDurationMs += durationMs;
    perf.applicationCount++;
    perf.totalMagnitude += std::abs(magnitude);
    perf.avgDurationMs = perf.totalDurationMs / perf.applicationCount;
    perf.lastApplication = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::trackMemoryOperation(const std::string& operation,
                                              double durationMs, uint64_t bytesTransferred) {
    if (!pImpl->monitoringEnabled) return;
    
    auto& perf = pImpl->memoryPerformances[operation];
    perf.operation = operation;
    perf.totalDurationMs += durationMs;
    perf.bytesTransferred += bytesTransferred;
    perf.accessCount++;
    perf.avgDurationMs = perf.totalDurationMs / perf.accessCount;
    perf.lastAccess = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::trackSystemUsage(double cpuPercent, double memoryMB, double memoryPercent) {
    if (!pImpl->monitoringEnabled) return;
    
    pImpl->systemMetrics.cpuAverage = (pImpl->systemMetrics.cpuAverage * 
                                       (pImpl->updateCount - 1) + cpuPercent) / 
                                      pImpl->updateCount;
    
    pImpl->systemMetrics.totalMemoryUsageMB = (pImpl->systemMetrics.totalMemoryUsageMB * 
                                               (pImpl->updateCount - 1) + memoryMB) / 
                                              pImpl->updateCount;
    
    if (memoryMB > pImpl->systemMetrics.peakMemoryUsageMB) {
        pImpl->systemMetrics.peakMemoryUsageMB = memoryMB;
    }
}

void PerformanceMonitor::trackDiskIO(double readSpeedMBps, double writeSpeedMBps) {
    if (!pImpl->monitoringEnabled) return;
    
    pImpl->systemMetrics.readSpeedMBps = (pImpl->systemMetrics.readSpeedMBps * 
                                          (pImpl->updateCount - 1) + readSpeedMBps) / 
                                         pImpl->updateCount;
    
    pImpl->systemMetrics.writeSpeedMBps = (pImpl->systemMetrics.writeSpeedMBps * 
                                           (pImpl->updateCount - 1) + writeSpeedMBps) / 
                                          pImpl->updateCount;
}

PerformanceSnapshot PerformanceMonitor::getCurrentMetrics() const {
    PerformanceSnapshot snapshot;
    snapshot.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    snapshot.system = getSystemMetrics();
    
    if (auto brainPtr = pImpl->brain.lock()) {
        snapshot.brain = getBrainMetrics(brainPtr.get());
    }
    
    snapshot.integration = getIntegrationMetrics();
    snapshot.plasticity = getPlasticityMetrics();
    
    return snapshot;
}

SystemMetrics PerformanceMonitor::Impl::getSystemMetrics() {
    SystemMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    metrics.cpuUsagePercent = cpuAverage;
    metrics.memoryUsageMB = totalMemoryUsageMB;
    metrics.memoryUsagePercent = (totalMemoryUsageMB / 1024.0) * 100.0;  // Assuming 1GB total
    metrics.diskIOReadMBps = readSpeedMBps;
    metrics.diskIOWriteMBps = writeSpeedMBps;
    metrics.activeThreads = threadCount;
    metrics.totalThreads = totalThreads;
    
    return metrics;
}

BrainMetrics PerformanceMonitor::Impl::getBrainMetrics(const Brain* brain) {
    BrainMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    metrics.totalNeurons = brain->getTotalNeuronCount();
    metrics.activeNeurons = brain->getActiveNeuronCount();
    metrics.firingNeurons = brain->getFiringNeuronCount();
    metrics.totalSynapses = brain->getTotalSynapseCount();
    metrics.averageFiringRate = brain->getAverageFiringRate();
    metrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    metrics.totalSpikes = brain->getTotalSpikeCount();
    metrics.synapticOperationsPerStep = calculateTotalSynapticOperations();
    
    // Calculate memory usage per neuron
    metrics.memoryUsagePerNeuronMB = (totalMemoryUsageMB * 1024.0) / 
                                     (totalNeurons > 0 ? totalNeurons : 1);
    
    return metrics;
}

IntegrationMetrics PerformanceMonitor::Impl::getIntegrationMetrics() {
    IntegrationMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Calculate integration metrics based on performance data
    metrics.sensoryInputLatencyMs = calculateAverageSensoryLatency();
    metrics.actionOutputLatencyMs = calculateAverageActionLatency();
    metrics.plasticityUpdateLatencyMs = calculateAveragePlasticityLatency();
    metrics.memoryAccessLatencyMs = calculateAverageMemoryLatency();
    metrics.neuromodulationLatencyMs = calculateAverageNeuromodulationLatency();
    metrics.systemInteractionCount = getTotalOperationCount();
    metrics.integrationEfficiencyScore = calculateIntegrationEfficiency();
    
    return metrics;
}

PlasticityMetrics PerformanceMonitor::Impl::getPlasticityMetrics() {
    PlasticityMetrics metrics;
    metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (const auto& [ruleType, perf] : plasticityPerformances) {
        if (perf.ruleType == "Hebbian") {
            metrics.hebbianUpdateLatencyMs = perf.avgDurationMs;
        } else if (perf.ruleType == "STDP") {
            metrics.stdpUpdateLatencyMs = perf.avgDurationMs;
        }
        
        metrics.plasticityOperations += perf.updateCount;
        metrics.totalLearningRate += perf.totalLearningRate / perf.updateCount;
        
        if (perf.updateCount > 0) {
            metrics.weightChangeMagnitude += perf.totalLearningRate / perf.updateCount;
        }
    }
    
    metrics.learningRate = metrics.totalLearningRate / 
                          (metrics.plasticityOperations > 0 ? metrics.plasticityOperations : 1);
    metrics.convergenceRate = calculateConvergenceRate();
    
    return metrics;
}

std::vector<Bottleneck> PerformanceMonitor::detectBottlenecks() const {
    return pImpl->detectBottlenecksImpl();
}

std::vector<Bottleneck> PerformanceMonitor::Impl::detectBottlenecksImpl() const {
    std::vector<Bottleneck> bottlenecks;
    
    // Check CPU bottleneck
    if (systemMetrics.cpuAverage > detectionParams.cpuThreshold) {
        Bottleneck bottleneck;
        bottleneck.type = Bottleneck::Type::CPU_BOUND;
        bottleneck.description = "High CPU utilization detected";
        bottleneck.impact = systemMetrics.cpuAverage / 100.0;
        bottleneck.affectedComponents = {"neuron_updates", "plasticity", "system_operations"};
        bottleneck.metrics = {systemMetrics.cpuAverage, systemMetrics.cpuTimePerUpdate};
        bottleneck.detectionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bottleneck.requiresOptimization = true;
        bottlenecks.push_back(bottleneck);
    }
    
    // Check memory bottleneck
    double memoryPercent = (totalMemoryUsageMB / 1024.0) * 100.0;
    if (memoryPercent > detectionParams.memoryThreshold) {
        Bottleneck bottleneck;
        bottleneck.type = Bottleneck::Type::MEMORY_BOUND;
        bottleneck.description = "High memory usage detected";
        bottleneck.impact = memoryPercent / 100.0;
        bottleneck.affectedComponents = {"neuron_states", "synapse_storage", "plasticity_cache"};
        bottleneck.metrics = {totalMemoryUsageMB, memoryPercent};
        bottleneck.detectionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bottleneck.requiresOptimization = true;
        bottlenecks.push_back(bottleneck);
    }
    
    // Check synaptic performance bottleneck
    double avgSynapseLatency = calculateAverageSynapseLatency();
    if (avgSynapseLatency > detectionParams.latencyThresholdMs) {
        Bottleneck bottleneck;
        bottleneck.type = Bottleneck::Type::SYNAPTIC_THROTTLE;
        bottleneck.description = "High synaptic operation latency";
        bottleneck.impact = std::min(avgSynapseLatency / 50.0, 1.0);
        bottleneck.affectedComponents = {"synapse_updates", "spike_propagation"};
        bottleneck.metrics = {avgSynapseLatency, calculateSynapseOperationCount()};
        bottleneck.detectionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bottleneck.requiresOptimization = true;
        bottlenecks.push_back(bottleneck);
    }
    
    // Check plasticity bottleneck
    double avgPlasticityLatency = calculateAveragePlasticityLatency();
    if (avgPlasticityLatency > detectionParams.latencyThresholdMs) {
        Bottleneck bottleneck;
        bottleneck.type = Bottleneck::Type::PLASTICITY_THROTTLE;
        bottleneck.description = "High plasticity operation latency";
        bottleneck.impact = std::min(avgPlasticityLatency / 50.0, 1.0);
        bottleneck.affectedComponents = {"hebbian_updates", "stdp_updates", "structural_plasticity"};
        bottleneck.metrics = {avgPlasticityLatency, calculateTotalPlasticityOperations()};
        bottleneck.detectionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bottleneck.requiresOptimization = true;
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::string PerformanceMonitor::generateReport(const std::string& format) const {
    if (format == "json") {
        return generateJSONReport();
    } else {
        return generateTextReport();
    }
}

std::string PerformanceMonitor::generateTextReport() const {
    std::stringstream ss;
    auto current = getCurrentMetrics();
    
    ss << "=== NLM Performance Report ===\n\n";
    ss << "Timestamp: " << current.timestamp << "\n\n";
    
    ss << "--- System Performance ---\n";
    ss << "CPU Usage: " << std::fixed << std::setprecision(1) 
       << current.system.cpuUsagePercent << "%\n";
    ss << "Memory Usage: " << std::fixed << std::setprecision(1)
       << current.system.memoryUsageMB << " MB ("
       << std::fixed << std::setprecision(1)
       << current.system.memoryUsagePercent << "%)\n";
    ss << "Disk I/O Read: " << std::fixed << std::setprecision(2)
       << current.system.diskIOReadMBps << " MB/s\n";
    ss << "Disk I/O Write: " << std::fixed << std::setprecision(2)
       << current.system.diskIOWriteMBps << " MB/s\n\n";
    
    ss << "--- Brain Performance ---\n";
    ss << "Neurons: " << current.brain.totalNeurons << " total, "
       << current.brain.activeNeurons << " active, "
       << current.brain.firingNeurons << " firing\n";
    ss << "Synapses: " << current.brain.totalSynapses << "\n";
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(2)
       << current.brain.averageFiringRate << " Hz\n";
    ss << "E/I Ratio: " << std::fixed << std::setprecision(2)
       << current.brain.excitationInhibitionRatio << "\n";
    ss << "Total Spikes: " << current.brain.totalSpikes << "\n";
    ss << "Memory per Neuron: " << std::fixed << std::setprecision(2)
       << current.brain.memoryUsagePerNeuronMB << " MB\n\n";
    
    ss << "--- Integration Performance ---\n";
    ss << "Sensory Input Latency: " << std::fixed << std::setprecision(2)
       << current.integration.sensoryInputLatencyMs << " ms\n";
    ss << "Action Output Latency: " << std::fixed << std::setprecision(2)
       << current.integration.actionOutputLatencyMs << " ms\n";
    ss << "Plasticity Update Latency: " << std::fixed << std::setprecision(2)
       << current.integration.plasticityUpdateLatencyMs << " ms\n";
    ss << "System Interaction Count: " << current.integration.systemInteractionCount << "\n";
    ss << "Integration Efficiency: " << std::fixed << std::setprecision(2)
       << current.integration.integrationEfficiencyScore << "\n\n";
    
    ss << "--- Plasticity Performance ---\n";
    ss << "Hebbian Update Latency: " << std::fixed << std::setprecision(2)
       << current.plasticity.hebbianUpdateLatencyMs << " ms\n";
    ss << "STDP Update Latency: " << std::fixed << std::setprecision(2)
       << current.plasticity.stdpUpdateLatencyMs << " ms\n";
    ss << "Structural Plasticity Latency: " << std::fixed << std::setprecision(2)
       << current.plasticity.structuralPlasticityLatencyMs << " ms\n";
    ss << "Plasticity Operations: " << current.plasticity.plasticityOperations << "\n";
    ss << "Average Learning Rate: " << std::fixed << std::setprecision(4)
       << current.plasticity.learningRate << "\n";
    ss << "Convergence Rate: " << std::fixed << std::setprecision(4)
       << current.plasticity.convergenceRate << "\n";
    
    return ss.str();
}

std::string PerformanceMonitor::generateDashboard() const {
    std::stringstream ss;
    
    ss << "=== NLM Performance Dashboard ===\n\n";
    
    auto current = getCurrentMetrics();
    
    // Current status
    ss << "Current Status:\n";
    ss << "  Time: " << current.timestamp << " ms\n";
    ss << "  CPU: " << std::fixed << std::setprecision(1)
       << current.system.cpuUsagePercent << "%\n";
    ss << "  Memory: " << std::fixed << std::setprecision(1)
       << current.system.memoryUsagePercent << "%\n";
    ss << "  Neurons: " << current.brain.totalNeurons << "\n";
    ss << "  Synapses: " << current.brain.totalSynapses << "\n";
    ss << "  Spikes/sec: " << (current.brain.totalSpikes / 
                              (current.timestamp / 1000.0)) << "\n\n";
    
    // Performance alerts
    auto alerts = checkAlerts(current);
    if (!alerts.empty()) {
        ss << "Alerts:";
        for (const auto& alert : alerts) {
            ss << "\n  " << alert;
        }
        ss << "\n\n";
    }
    
    // Bottlenecks
    auto bottlenecks = getRecentBottlenecks(current.timestamp - 300000);  // Last 5 minutes
    if (!bottlenecks.empty()) {
        ss << "Active Bottlenecks:";
        for (const auto& bottleneck : bottlenecks) {
            ss << "\n  - " << getBottleneckTypeString(bottleneck.type) << 
               " (Impact: " << std::fixed << std::setprecision(2) 
               << bottleneck.impact << ")";
        }
        ss << "\n\n";
    }
    
    return ss.str();
}

std::vector<std::string> PerformanceMonitor::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    auto current = getCurrentMetrics();
    
    // CPU optimization recommendations
    if (current.system.cpuUsagePercent > 80.0) {
        recommendations.push_back(
            "Reduce neuron update frequency or enable parallel processing for CPU-bound operations");
    }
    
    // Memory optimization recommendations
    if (current.system.memoryUsagePercent > 85.0) {
        recommendations.push_back(
            "Implement memory pool optimization or enable sparse connectivity");
    }
    
    // Synaptic optimization recommendations
    double synapticLatency = calculateAverageSynapseLatency();
    if (synapticLatency > 10.0) {
        recommendations.push_back(
            "Optimize synaptic connectivity or enable SIMD vectorization");
    }
    
    // Plasticity optimization recommendations
    if (current.plasticity.hebbianUpdateLatencyMs > 10.0) {
        recommendations.push_back(
            "Optimize Hebbian update algorithm or reduce learning rate frequency");
    }
    
    // Integration optimization recommendations
    if (current.integration.sensoryInputLatencyMs > 20.0) {
        recommendations.push_back(
            "Optimize sensory input processing or enable event-driven updates");
    }
    
    return recommendations;
}

std::string PerformanceMonitor::generateOptimizationPlan() const {
    std::stringstream ss;
    
    ss << "=== NLM Performance Optimization Plan ===\n\n";
    
    auto recommendations = getOptimizationRecommendations();
    
    if (recommendations.empty()) {
        ss << "No optimization recommendations at this time.\n";
        return ss.str();
    }
    
    ss << "Priority Actions:\n";
    for (size_t i = 0; i < recommendations.size(); ++i) {
        ss << (i + 1) << ". " << recommendations[i] << "\n";
    }
    
    ss << "\nQuick Wins:\n";
    for (const auto& recommendation : recommendations) {
        if (recommendation.find("reduce") != std::string::npos ||
            recommendation.find("optimize") != std::string::npos) {
            ss << "  - " << recommendation << "\n";
        }
    }
    
    return ss.str();
}

void PerformanceMonitor::setMonitoringEnabled(bool enabled) {
    pImpl->monitoringEnabled = enabled;
}

bool PerformanceMonitor::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

void PerformanceMonitor::setSamplingInterval(double intervalSeconds) {
    pImpl->samplingInterval = intervalSeconds;
}

void PerformanceMonitor::setMonitoringEnabled(bool enabled) {
    pImpl->monitoringEnabled = enabled;
}

bool PerformanceMonitor::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

void PerformanceMonitor::setSamplingInterval(double intervalSeconds) {
    pImpl->samplingInterval = intervalSeconds;
}

double PerformanceMonitor::getSamplingInterval() const {
    return pImpl->samplingInterval;
}

void PerformanceMonitor::enableRealTimeDashboard(bool enabled) {
    pImpl->realTimeDashboard = enabled;
}

bool PerformanceMonitor::isRealTimeDashboardEnabled() const {
    return pImpl->realTimeDashboard;
}

void PerformanceMonitor::startProfile(const std::string& profileName) {
    if (!pImpl->monitoringEnabled) return;
    pImpl->profileDurations[profileName] = std::chrono::duration<double, std::milli>(0.0);
    pImpl->profileCounts[profileName] = 0;
}

void PerformanceMonitor::stopProfile(const std::string& profileName) {
    if (!pImpl->monitoringEnabled) return;
    pImpl->profileDurations[profileName] = std::chrono::duration<double, std::milli>(
        pImpl->profileDurations[profileName].count());
    pImpl->profileCounts[profileName]++;
}

std::map<std::string, double> PerformanceMonitor::getProfileResults() const {
    std::map<std::string, double> results;
    for (const auto& [name, duration] : pImpl->profileDurations) {
        uint64_t count = pImpl->profileCounts[name];
        results[name] = duration.count() / (count > 0 ? count : 1);
    }
    return results;
}

void PerformanceMonitor::addAlertThreshold(const std::string& metric, double threshold,
                                          const std::string& severity) {
    Impl::AlertThreshold alert;
    alert.metric = metric;
    alert.threshold = threshold;
    alert.severity = severity;
    pImpl->alertThresholds.push_back(alert);
}

std::vector<std::string> PerformanceMonitor::checkAlerts(const PerformanceSnapshot& snapshot) const {
    std::vector<std::string> alerts;
    
    for (const auto& threshold : pImpl->alertThresholds) {
        if (threshold.metric == "cpu" && snapshot.system.cpuUsagePercent > threshold.threshold) {
            std::stringstream ss;
            ss << "High CPU usage: " << std::fixed << std::setprecision(1)
               << snapshot.system.cpuUsagePercent << "% (threshold: "
               << threshold.threshold << "%) - " << threshold.severity;
            alerts.push_back(ss.str());
        }
        else if (threshold.metric == "memory" && snapshot.system.memoryUsagePercent > threshold.threshold) {
            std::stringstream ss;
            ss << "High memory usage: " << std::fixed << std::setprecision(1)
               << snapshot.system.memoryUsagePercent << "% (threshold: "
               << threshold.threshold << "%) - " << threshold.severity;
            alerts.push_back(ss.str());
        }
        else if (threshold.metric == "latency" && 
                 snapshot.integration.sensoryInputLatencyMs > threshold.threshold) {
            std::stringstream ss;
            ss << "High latency: " << std::fixed << std::setprecision(1)
               << snapshot.integration.sensoryInputLatencyMs << "ms (threshold: "
               << threshold.threshold << "ms) - " << threshold.severity;
            alerts.push_back(ss.str());
        }
    }
    
    return alerts;
}

double PerformanceMonitor::analyzeTrend(const std::vector<double>& values, size_t windowSize) const {
    if (values.size() < 2) return 0.0;
    
    size_t start = std::max<size_t>(0, values.size() - windowSize);
    std::vector<double> window(values.begin() + start, values.end());
    
    if (window.size() < 2) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < window.size(); ++i) {
        double x = static_cast<double>(i);
        double y = window[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double n = static_cast<double>(window.size());
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    return slope;
}

double PerformanceMonitor::getMovingAverage(const std::vector<double>& values, size_t windowSize) const {
    if (values.empty()) return 0.0;
    
    size_t start = std::max<size_t>(0, values.size() - windowSize);
    double sum = std::accumulate(values.begin() + start, values.end(), 0.0);
    return sum / (values.size() - start);
}

uint64_t PerformanceMonitor::getPeakMemoryUsageMB() const {
    return static_cast<uint64_t>(pImpl->systemMetrics.peakMemoryUsageMB);
}

uint64_t PerformanceMonitor::getAverageMemoryUsageMB() const {
    return static_cast<uint64_t>(pImpl->systemMetrics.totalMemoryUsageMB);
}

double PerformanceMonitor::getCPUAverage() const {
    return pImpl->systemMetrics.cpuAverage;
}

uint64_t PerformanceMonitor::getTotalOperationCount() const {
    uint64_t total = 0;
    for (const auto& [neuronId, perf] : pImpl->neuronPerformances) {
        total += perf.count;
    }
    for (const auto& [synapseId, perf] : pImpl->synapsePerformances) {
        total += perf.updateCount;
    }
    for (const auto& [ruleType, perf] : pImpl->plasticityPerformances) {
        total += perf.updateCount;
    }
    for (const auto& [signalType, perf] : pImpl->neuromodulationPerformances) {
        total += perf.applicationCount;
    }
    return total;
}

// Helper methods
double PerformanceMonitor::Impl::calculateAverageSynapseLatency() const {
    double totalLatency = 0.0;
    uint64_t totalUpdates = 0;
    
    for (const auto& [synapseId, perf] : synapsePerformances) {
        totalLatency += perf.totalUpdateDurationMs;
        totalUpdates += perf.updateCount;
    }
    
    return totalUpdates > 0 ? totalLatency / totalUpdates : 0.0;
}

uint64_t PerformanceMonitor::Impl::calculateSynapseOperationCount() const {
    uint64_t total = 0;
    for (const auto& [synapseId, perf] : synapsePerformances) {
        total += perf.updateCount;
    }
    return total;
}

double PerformanceMonitor::Impl::calculateAveragePlasticityLatency() const {
    double totalLatency = 0.0;
    uint64_t totalUpdates = 0;
    
    for (const auto& [ruleType, perf] : plasticityPerformances) {
        totalLatency += perf.totalUpdateDurationMs;
        totalUpdates += perf.updateCount;
    }
    
    return totalUpdates > 0 ? totalLatency / totalUpdates : 0.0;
}

uint64_t PerformanceMonitor::Impl::calculateTotalPlasticityOperations() const {
    uint64_t total = 0;
    for (const auto& [ruleType, perf] : plasticityPerformances) {
        total += perf.updateCount;
    }
    return total;
}

double PerformanceMonitor::Impl::calculateConvergenceRate() const {
    if (plasticityPerformances.empty()) return 0.0;
    
    std::vector<double> learningRates;
    for (const auto& [ruleType, perf] : plasticityPerformances) {
        if (perf.updateCount > 0) {
            learningRates.push_back(perf.totalLearningRate / perf.updateCount);
        }
    }
    
    if (learningRates.size() < 2) return 0.0;
    
    double sum = std::accumulate(learningRates.begin(), learningRates.end(), 0.0);
    double mean = sum / learningRates.size();
    
    double variance = 0.0;
    for (double rate : learningRates) {
        variance += (rate - mean) * (rate - mean);
    }
    variance /= learningRates.size();
    
    double stddev = std::sqrt(variance);
    
    return stddev > 0.0 ? (mean / stddev) : 0.0;
}

std::string PerformanceMonitor::getBottleneckTypeString(Bottleneck::Type type) const {
    switch (type) {
        case Bottleneck::Type::CPU_BOUND: return "CPU Bound";
        case Bottleneck::Type::MEMORY_BOUND: return "Memory Bound";
        case Bottleneck::Type::IO_BOUND: return "I/O Bound";
        case Bottleneck::Type::SYNAPTIC_THROTTLE: return "Synaptic Throttled";
        case Bottleneck::Type::PLASTICITY_THROTTLE: return "Plasticity Throttled";
        case Bottleneck::Type::MEMORY_PRESSURE: return "Memory Pressure";
        case Bottleneck::Type::SPATIAL_DISTRIBUTION: return "Spatial Distribution";
        default: return "Unknown";
    }
}

std::string PerformanceMonitor::generateJSONReport() const {
    // Placeholder for JSON report generation
    return "{\"status\": \"Performance monitoring system initialized but requires implementation of JSON report generation\"}";
}

// Global functions
void initializePerformanceMonitoring(std::shared_ptr<Brain> brain) {
    PerformanceMonitor::getInstance().initialize();
    PerformanceMonitor::getInstance().setMonitoringEnabled(true);
    
    if (brain) {
        PerformanceMonitor::getInstance().setBrain(brain);
    }
    
    NLM_LOG_INFO("Performance monitoring system initialized");
}

// Performance macro helper implementation
void PerformanceMonitor::Impl::trackTiming(const std::string& operation,
                                          const std::chrono::duration<double, std::milli>& duration,
                                          const std::string& metric) {
    if (!monitoringEnabled) return;
    
    double durationMs = duration.count();
    
    if (operation.find("neuron") != std::string::npos) {
        // Parse neuron ID and operation type
        // Implementation would parse and update neuron performance metrics
    } else if (operation.find("synapse") != std::string::npos) {
        // Parse synapse ID and update synapse performance metrics
    } else if (operation.find("plasticity") != std::string::npos) {
        // Parse plasticity rule type and update plasticity performance metrics
    }
}

// Set brain reference
void PerformanceMonitor::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

} // namespace nlm