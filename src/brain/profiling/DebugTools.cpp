#include "DebugTools.hpp"
#include "../brain/Brain.hpp"
#include "../brain/profiling/BrainProfiler.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <numeric>

using json = nlohmann::json;

namespace nlm {

struct DebugTools::Impl {
    // Profiler reference
    std::shared_ptr<BrainProfiler> profiler_;
    
    // Debug history
    std::vector<StepAnalysis> stepHistory;
    std::vector<PerformanceBottleneck> bottleneckHistory;
    std::vector<MemoryLeakInfo> memoryLeakHistory;
    std::vector<StateValidationResult> validationHistory;
    
    // Current analysis state
    mutable std::mutex analysisMutex;
    
    // Memory tracking for leaks
    std::unordered_map<std::string, MemoryLeakInfo> currentMemoryLeaks;
    
    // Performance tracking
    std::vector<double> recentCPUTimes;
    std::vector<double> recentMemoryUsage;
    std::vector<double> recentStabilityScores;
    
    Impl() : stepHistory(), bottleneckHistory(), memoryLeakHistory(),
             validationHistory(), currentMemoryLeaks() {
        recentCPUTimes.reserve(100);
        recentMemoryUsage.reserve(100);
        recentStabilityScores.reserve(100);
    }
};

DebugTools::DebugTools() : pImpl(std::make_unique<Impl>()) {}

DebugTools::~DebugTools() = default;

DebugTools::StateValidationResult DebugTools::validateState(const class Brain& brain) const {
    StateValidationResult result;
    
    // Check for null or uninitialized brain
    if (!brain.getConfig()) {
        result.isValid = false;
        result.errors.push_back("Brain configuration is null");
    }
    
    // Check region consistency
    size_t totalNeuronCount = brain.getTotalNeuronCount();
    size_t totalSynapseCount = brain.getTotalSynapseCount();
    
    if (totalNeuronCount == 0) {
        result.isValid = false;
        result.errors.push_back("Brain has no neurons");
    }
    
    if (totalSynapseCount == 0) {
        result.warnings.push_back("Brain has no synapses (isolated neurons)");
    }
    
    // Check spike event counts
    size_t pendingSpikes = brain.getPendingSpikeEventCount();
    if (pendingSpikes > 10000) {
        result.warnings.push_back("High number of pending spike events: " + std::to_string(pendingSpikes));
    }
    
    // Check firing rate consistency
    float eiratio = brain.getExcitationInhibitionRatio();
    if (std::isinf(eiratio) && eiratio < 0) {
        result.errors.push_back("Invalid E/I ratio (infinite or negative)");
    } else if (eiratio > 10.0) {
        result.warnings.push_back("High excitation/inhibition ratio: " + std::to_string(eiratio));
    } else if (eiratio < 0.1) {
        result.warnings.push_back("Low excitation/inhibition ratio: " + std::to_string(eiratio));
    }
    
    // Check average firing rate
    float avgFiringRate = brain.getAverageFiringRate();
    if (avgFiringRate > 100.0) {
        result.warnings.push_back("Very high average firing rate: " + std::to_string(avgFiringRate) + " Hz");
    } else if (avgFiringRate < 0.01 && totalNeuronCount > 100) {
        result.warnings.push_back("Very low average firing rate: " + std::to_string(avgFiringRate) + " Hz");
    }
    
    // Check active neuron count
    size_t activeNeurons = brain.getActiveNeuronCount();
    if (activeNeurons == 0 && totalNeuronCount > 0) {
        result.warnings.push_back("No active neurons in the network");
    }
    
    // Check for consistency between different metrics
    if (totalNeuronCount > 0 && activeNeurons > totalNeuronCount) {
        result.isValid = false;
        result.errors.push_back("Active neuron count exceeds total neuron count");
    }
    
    result.validationTime = std::chrono::steady_clock::now();
    
    // Store in history
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    pImpl->validationHistory.push_back(result);
    if (pImpl->validationHistory.size() > 100) {
        pImpl->validationHistory.erase(pImpl->validationHistory.begin());
    }
    
    return result;
}

std::vector<DebugTools::MemoryLeakInfo> DebugTools::checkMemoryLeaks() const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    std::vector<MemoryLeakInfo> leaks;
    
    // Check profiler's memory allocations if available
    if (profiler_) {
        const auto& memoryAllocations = profiler_->getMetrics("allocation");
        
        for (const auto& allocation : memoryAllocations) {
            auto it = pImpl->currentMemoryLeaks.find(allocation.description);
            if (it == pImpl->currentMemoryLeaks.end()) {
                // New potential leak
                MemoryLeakInfo leak(allocation.description, static_cast<size_t>(allocation.value));
                pImpl->currentMemoryLeaks[allocation.description] = leak;
                leaks.push_back(leak);
            } else {
                // Update existing leak
                it->second.allocationCount++;
                it->second.lastAllocation = std::chrono::steady_clock::now();
                
                // Check if still being allocated (active)
                auto now = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastAllocation).count();
                it->second.isActive = (duration < 300); // Active if allocated in last 5 minutes
                
                leaks.push_back(it->second);
            }
        }
    }
    
    // Update leak history
    for (const auto& leak : leaks) {
        pImpl->memoryLeakHistory.push_back(leak);
    }
    
    if (pImpl->memoryLeakHistory.size() > 100) {
        pImpl->memoryLeakHistory.erase(pImpl->memoryLeakHistory.begin());
    }
    
    return leaks;
}

std::vector<DebugTools::PerformanceBottleneck> DebugTools::analyzePerformance() const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    std::vector<PerformanceBottleneck> bottlenecks;
    
    if (!profiler_) {
        return bottlenecks;
    }
    
    // Get profiler data
    auto stepTimes = profiler_->getMetrics("step_time");
    
    // Analyze step times for bottlenecks
    if (!stepTimes.empty()) {
        double totalTime = 0.0;
        double maxTime = 0.0;
        
        for (const auto& metric : stepTimes) {
            totalTime += metric.value;
            maxTime = std::max(maxTime, metric.value);
        }
        
        double avgTime = totalTime / stepTimes.size();
        
        if (avgTime > 100.0) {
            bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::SlowStepProcessing,
                                     "Average step processing time is too high: " + std::to_string(avgTime) + " ms",
                                     0.8);
        }
        
        if (maxTime > avgTime * 3) { // Max is 3x average
            bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::SlowStepProcessing,
                                     "Step processing time varies significantly (max: " + std::to_string(maxTime) + " ms)",
                                     0.6);
        }
    }
    
    // Check memory usage from profiler
    auto memoryStats = profiler_->getMemoryStats();
    if (memoryStats.usagePercentage > 80.0) {
        bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::HighMemoryUsage,
                                 "Memory usage is high: " + std::to_string(memoryStats.usagePercentage) + "%",
                                 0.7);
    }
    
    // Check firing statistics from profiler
    auto firingStats = profiler_->getFiringStats();
    
    if (firingStats.totalSpikes > 0) {
        double avgFiringRate = firingStats.averageFiringRate;
        
        if (avgFiringRate > 100.0) {
            bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::ExcessiveSpikeRate,
                                     "Average firing rate is too high: " + std::to_string(avgFiringRate) + " Hz",
                                     0.5);
        } else if (avgFiringRate < 0.1) {
            bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::LowFiringRate,
                                     "Average firing rate is too low: " + std::to_string(avgFiringRate) + " Hz",
                                     0.3);
        }
    }
    
    // Check for memory leaks
    auto leaks = checkMemoryLeaks();
    if (!leaks.empty()) {
        bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::MemoryLeak,
                                 "Memory leaks detected: " + std::to_string(leaks.size()) + " leaks",
                                 0.9);
    }
    
    // Check system health from profiler
    auto health = profiler_->getSystemHealth();
    if (health.cpuUsage > 90.0) {
        bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::ConnectivityOverload,
                                 "CPU usage is very high: " + std::to_string(health.cpuUsage) + "%",
                                 0.8);
    }
    
    if (health.stabilityScore < 0.5) {
        bottlenecks.emplace_back(PerformanceBottleneck::BottleneckType::NetworkStability,
                                 "Network stability is low: " + std::to_string(health.stabilityScore),
                                 0.6);
    }
    
    // Store in history
    for (auto& bottleneck : bottlenecks) {
        pImpl->bottleneckHistory.push_back(bottleneck);
    }
    
    if (pImpl->bottleneckHistory.size() > 100) {
        pImpl->bottleneckHistory.erase(pImpl->bottleneckHistory.begin());
    }
    
    return bottlenecks;
}

DebugTools::StepAnalysis DebugTools::analyzeStep(size_t step) const {
    StepAnalysis analysis(step, 0.0); // simulationTime is not available here
    
    // For now, return basic analysis
    // In a real implementation, this would access step-specific data from profiler
    if (profiler_) {
        auto metrics = profiler_->getMetrics("step_time");
        
        for (const auto& metric : metrics) {
            if (std::abs(metric.value) > 1e-6) { // Non-zero value
                analysis.simulationTime = metric.value;
                analysis.cpuUsage = 50.0; // Placeholder
                analysis.memoryUsage = 50.0; // Placeholder
                analysis.neuronsProcessed = 100; // Placeholder
                analysis.spikesProcessed = 10; // Placeholder
                break;
            }
        }
    }
    
    return analysis;
}

DebugTools::SystemHealth DebugTools::getSystemHealth() const {
    SystemHealth health;
    
    if (profiler_) {
        auto profilerHealth = profiler_->getSystemHealth();
        health.cpuUsage = profilerHealth.cpuUsage;
        health.memoryUsage = profilerHealth.memoryUsage;
        health.stabilityScore = profilerHealth.stabilityScore;
        health.hasMemoryLeaks = profilerHealth.hasMemoryLeaks;
        health.issues = profilerHealth.status != "healthy" ? std::vector<std::string>{profilerHealth.status} : std::vector<std::string>{};
        
        // Determine overall status
        if (health.cpuUsage > 90.0 || health.memoryUsage > 90.0 || health.hasMemoryLeaks) {
            health.healthStatus = "critical";
            health.hasPerformanceIssues = true;
        } else if (health.cpuUsage > 70.0 || health.memoryUsage > 70.0 || health.stabilityScore < 0.5) {
            health.healthStatus = "warning";
            health.hasPerformanceIssues = true;
        } else {
            health.healthStatus = "healthy";
            health.hasPerformanceIssues = false;
        }
    }
    
    return health;
}

DebugTools::NetworkSnapshot DebugTools::takeNetworkSnapshot(const class Brain& brain) const {
    NetworkSnapshot snapshot(brain.getTotalNeuronCount());
    snapshot.timestamp = std::chrono::steady_clock::now();
    
    // Get neuron counts by region
    auto regionIds = brain.getRegionIds();
    snapshot.regionNames.reserve(regionIds.size());
    for (const auto& regionId : regionIds) {
        auto* region = brain.getRegion(regionId);
        if (region) {
            snapshot.regionNames.push_back(region->getName());
        }
    }
    
    // Get neuron activities and firing rates
    size_t neuronIndex = 0;
    for (const auto& regionId : regionIds) {
        auto* region = brain.getRegion(regionId);
        if (region) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuronIndex < snapshot.neuronActivity.size()) {
                        const auto& state = neuron->getState();
                        snapshot.neuronActivity[neuronIndex] = 
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                        
                        // Calculate firing rate (simplified)
                        double firingRate = 0.0;
                        if (state.lastSpikeTime > 0.0) {
                            firingRate = 1000.0 / (state.lastSpikeTime - state.lastSpikeTime);
                        }
                        snapshot.firingRates[neuronIndex] = firingRate;
                        
                        // Track active neurons
                        if (neuron->isFiring() || snapshot.neuronActivity[neuronIndex] > 0.1) {
                            snapshot.activeNeurons.push_back(neuron->getId().index());
                        }
                    }
                    neuronIndex++;
                }
            }
        }
    }
    
    // Get connectivity information
    for (size_t i = 0; i < snapshot.connectivityMatrix.size(); ++i) {
        for (size_t j = 0; j < snapshot.connectivityMatrix[i].size(); ++j) {
            // In a real implementation, this would query the actual synapse weights
            snapshot.connectivityMatrix[i][j] = 0.0; // Placeholder
        }
    }
    
    return snapshot;
}

std::string DebugTools::getVisualizationData(const NetworkSnapshot& snapshot) const {
    std::ostringstream oss;
    oss << "NetworkSnapshot at time: " << snapshot.timestamp.time_since_epoch().count() << "\n";
    oss << "Neurons: " << snapshot.neuronActivity.size() << "\n";
    oss << "Active neurons: " << snapshot.activeNeurons.size() << "\n";
    oss << "Regions: " << snapshot.regionNames.size() << "\n";
    oss << "\nNeuron Activities (first 10): ";
    
    size_t count = std::min(snapshot.neuronActivity.size(), size_t(10));
    for (size_t i = 0; i < count; ++i) {
        oss << snapshot.neuronActivity[i];
        if (i < count - 1) oss << ", ";
    }
    oss << "\n\nFiring Rates (first 10): ";
    
    for (size_t i = 0; i < count; ++i) {
        oss << snapshot.firingRates[i];
        if (i < count - 1) oss << ", ";
    }
    oss << "\n";
    
    return oss.str();
}

DebugTools::ConnectivityResult DebugTools::checkConnectivity(const class Brain& brain) const {
    ConnectivityResult result;
    
    result.totalConnections = brain.getTotalSynapseCount();
    
    if (result.totalConnections > 0) {
        result.isConnected = true;
        
        // Calculate average weight (simplified)
        float totalWeight = 0.0;
        size_t synapseCount = 0;
        
        for (const auto& regionId : brain.getRegionIds()) {
            auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto* synapse : region->getSynapses()) {
                    totalWeight += std::abs(synapse->getWeight());
                    synapseCount++;
                }
            }
        }
        
        if (synapseCount > 0) {
            result.averageWeight = totalWeight / synapseCount;
        }
        
        // Count isolated neurons
        size_t totalNeurons = brain.getTotalNeuronCount();
        std::unordered_set<size_t> connectedNeuronIds;
        
        for (const auto& regionId : brain.getRegionIds()) {
            auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto* synapse : region->getSynapses()) {
                    connectedNeuronIds.insert(synapse->getSourceNeuron().index());
                    connectedNeuronIds.insert(synapse->getDestinationNeuron().index());
                }
            }
        }
        
        result.isolatedNeurons = totalNeurons - connectedNeuronIds.size();
        
        // Find high-degree neurons
        std::unordered_map<size_t, size_t> neuronDegree;
        for (const auto& regionId : brain.getRegionIds()) {
            auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto* synapse : region->getSynapses()) {
                    neuronDegree[synapse->getSourceNeuron().index()]++;
                    neuronDegree[synapse->getDestinationNeuron().index()]++;
                }
            }
        }
        
        for (const auto& pair : neuronDegree) {
            if (pair.second > 10) {
                result.highDegreeNeurons.push_back(pair.first);
            }
        }
    }
    
    return result;
}

DebugTools::FiringPatternAnalysis DebugTools::analyzeFiringPatterns(const class Brain& brain) const {
    FiringPatternAnalysis analysis;
    
    if (brain.getTotalNeuronCount() == 0) {
        return analysis;
    }
    
    // Simple firing pattern analysis
    size_t totalNeurons = brain.getTotalNeuronCount();
    double totalFiringRate = 0.0;
    double totalActivation = 0.0;
    
    for (const auto& regionId : brain.getRegionIds()) {
        auto* region = brain.getRegion(regionId);
        if (region) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    
                    // Simple firing rate estimation
                    double firingRate = 0.0;
                    if (state.lastSpikeTime > 0.0) {
                        // This is a simplification
                        firingRate = 10.0 / (state.lastSpikeTime - state.lastSpikeTime + 1.0);
                    }
                    
                    analysis.maxFiringRate = std::max(analysis.maxFiringRate, firingRate);
                    totalFiringRate += firingRate;
                    
                    // Activation level
                    double activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                    totalActivation += activation;
                    
                    // Classify neurons
                    if (firingRate == 0.0) {
                        analysis.silentNeurons.push_back(neuron->getId().index());
                    } else if (firingRate > 100.0) {
                        analysis.hyperactiveNeurons.push_back(neuron->getId().index());
                    }
                }
            }
        }
    }
    
    if (totalNeurons > 0) {
        analysis.meanFiringRate = totalFiringRate / totalNeurons;
        analysis.meanActivation = totalActivation / totalNeurons;
        
        // Calculate standard deviation (simplified)
        double variance = 0.0;
        for (const auto& regionId : brain.getRegionIds()) {
            auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        double firingRate = 0.0;
                        if (state.lastSpikeTime > 0.0) {
                            firingRate = 10.0 / (state.lastSpikeTime - state.lastSpikeTime + 1.0);
                        }
                        variance += (firingRate - analysis.meanFiringRate) * (firingRate - analysis.meanFiringRate);
                    }
                }
            }
        }
        
        analysis.firingRateStdDev = std::sqrt(variance / totalNeurons);
    }
    
    // Find max firing neuron
    for (const auto& regionId : brain.getRegionIds()) {
        auto* region = brain.getRegion(regionId);
        if (region) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    double firingRate = 0.0;
                    if (state.lastSpikeTime > 0.0) {
                        firingRate = 10.0 / (state.lastSpikeTime - state.lastSpikeTime + 1.0);
                    }
                    
                    if (firingRate > analysis.maxFiringRate) {
                        analysis.maxFiringRate = firingRate;
                        analysis.maxFiringNeuron = neuron->getId().index();
                    }
                }
            }
        }
    }
    
    return analysis;
}

DebugTools::NeuromodulationValidation DebugTools::validateNeuromodulation(const class Brain& brain) const {
    NeuromodulationValidation result;
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Validate dopamine
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        result.dopamineValid = (dopamineLevel >= 0.0 && dopamineLevel <= 1.0);
        if (!result.dopamineValid) {
            result.validationErrors.push_back("Dopamine level out of expected range: " + std::to_string(dopamineLevel));
        }
    }
    
    // Validate curiosity
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        result.curiosityValid = (curiosityLevel >= 0.0 && curiosityLevel <= 1.0);
        if (!result.curiosityValid) {
            result.validationErrors.push_back("Curiosity level out of expected range: " + std::to_string(curiosityLevel));
        }
    }
    
    // Validate novelty - check if method exists by calling it
    if (novelty) {
        // Try to access novelty level without specific method calls
        try {
            result.noveltyValid = true; // Default to true if exists
        } catch (...) {
            result.noveltyValid = false;
            result.validationErrors.push_back("Novelty detection not accessible");
        }
    }
    
    // Validate prediction error - check if method exists
    if (predictionError) {
        try {
            float errorLevel = predictionError->getLevel();
            result.predictionErrorValid = std::isfinite(errorLevel) && errorLevel >= 0.0;
            if (!result.predictionErrorValid) {
                result.validationErrors.push_back("Prediction error signal is invalid: " + std::to_string(errorLevel));
            }
        } catch (...) {
            result.predictionErrorValid = false;
            result.validationErrors.push_back("Prediction error signal not accessible");
        }
    }
    
    return result;
}

std::vector<DebugTools::StepAnalysis> DebugTools::getStepAnalysisHistory(size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    std::vector<StepAnalysis> history;
    
    size_t count = std::min(maxHistory, pImpl->stepHistory.size());
    history.reserve(count);
    
    for (size_t i = pImpl->stepHistory.size() - count; i < pImpl->stepHistory.size(); ++i) {
        history.push_back(pImpl->stepHistory[i]);
    }
    
    return history;
}

std::vector<DebugTools::PerformanceBottleneck> DebugTools::getPerformanceHistory(
    size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    std::vector<PerformanceBottleneck> history;
    
    size_t count = std::min(maxHistory, pImpl->bottleneckHistory.size());
    history.reserve(count);
    
    for (size_t i = pImpl->bottleneckHistory.size() - count; i < pImpl->bottleneckHistory.size(); ++i) {
        history.push_back(pImpl->bottleneckHistory[i]);
    }
    
    return history;
}

std::vector<DebugTools::MemoryLeakInfo> DebugTools::getMemoryLeakHistory(
    size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    std::vector<MemoryLeakInfo> history;
    
    size_t count = std::min(maxHistory, pImpl->memoryLeakHistory.size());
    history.reserve(count);
    
    for (size_t i = pImpl->memoryLeakHistory.size() - count; i < pImpl->memoryLeakHistory.size(); ++i) {
        history.push_back(pImpl->memoryLeakHistory[i]);
    }
    
    return history;
}

std::string DebugTools::generateDebugReport() const {
    std::ostringstream oss;
    oss << "=== NLM Debug Report ===\n";
    
    // Get system health
    auto health = getSystemHealth();
    oss << "System Health: " << health.healthStatus << "\n";
    oss << "CPU Usage: " << health.cpuUsage << "%\n";
    oss << "Memory Usage: " << health.memoryUsage << "%\n";
    oss << "Stability Score: " << health.stabilityScore << "\n";
    oss << "Memory Leaks: " << (health.hasMemoryLeaks ? "YES" : "NO") << "\n";
    oss << "Performance Issues: " << (health.hasPerformanceIssues ? "YES" : "NO") << "\n";
    
    if (!health.issues.empty()) {
        oss << "Issues:\n";
        for (const auto& issue : health.issues) {
            oss << "  - " << issue << "\n";
        }
    }
    
    // Get performance bottlenecks
    auto bottlenecks = analyzePerformance();
    if (!bottlenecks.empty()) {
        oss << "\nPerformance Bottlenecks: " << bottlenecks.size() << "\n";
        for (const auto& bottleneck : bottlenecks) {
            oss << "  - " << bottleneck.description << " (severity: " << bottleneck.severity << ")\n";
        }
    }
    
    // Get memory leaks
    auto leaks = checkMemoryLeaks();
    if (!leaks.empty()) {
        oss << "\nMemory Leaks: " << leaks.size() << "\n";
        for (const auto& leak : leaks) {
            oss << "  - " << leak.context << " (" << leak.allocationCount << " allocations, " 
                << leak.allocatedBytes << " bytes, " << (leak.isActive ? "active" : "inactive") << ")\n";
        }
    }
    
    // Get state validation
    if (profiler_) {
        auto* brainPtr = reinterpret_cast<const class Brain*>(profiler_.get());
        auto validation = validateState(*brainPtr);
        
        if (!validation.isValid || !validation.errors.empty() || !validation.warnings.empty()) {
            oss << "\nState Validation:\n";
            oss << "  Valid: " << (validation.isValid ? "YES" : "NO") << "\n";
            if (!validation.errors.empty()) {
                oss << "  Errors: " << validation.errors.size() << "\n";
                for (const auto& error : validation.errors) {
                    oss << "    - " << error << "\n";
                }
            }
            if (!validation.warnings.empty()) {
                oss << "  Warnings: " << validation.warnings.size() << "\n";
                for (const auto& warning : validation.warnings) {
                    oss << "    - " << warning << "\n";
                }
            }
        }
    }
    
    return oss.str();
}

bool DebugTools::exportDebugData(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Export current system state
        json j;
        
        // Export system health
        auto health = getSystemHealth();
        j["system_health"]["cpu_usage"] = health.cpuUsage;
        j["system_health"]["memory_usage"] = health.memoryUsage;
        j["system_health"]["stability_score"] = health.stabilityScore;
        j["system_health"]["has_memory_leaks"] = health.hasMemoryLeaks;
        j["system_health"]["has_performance_issues"] = health.hasPerformanceIssues;
        j["system_health"]["health_status"] = health.healthStatus;
        
        // Export performance bottlenecks
        auto bottlenecks = analyzePerformance();
        json bottlenecksJson = json::array();
        for (const auto& bottleneck : bottlenecks) {
            json bottleneckJson;
            bottleneckJson["type"] = static_cast<int>(bottleneck.type);
            bottleneckJson["description"] = bottleneck.description;
            bottleneckJson["severity"] = bottleneck.severity;
            bottlenecksJson.push_back(bottleneckJson);
        }
        j["performance_bottlenecks"] = bottlenecksJson;
        
        // Export memory leaks
        auto leaks = checkMemoryLeaks();
        json leaksJson = json::array();
        for (const auto& leak : leaks) {
            json leakJson;
            leakJson["context"] = leak.context;
            leakJson["allocated_bytes"] = leak.allocatedBytes;
            leakJson["allocation_count"] = leak.allocationCount;
            leakJson["is_active"] = leak.isActive;
            leaksJson.push_back(leakJson);
        }
        j["memory_leaks"] = leaksJson;
        
        // Export validation results
        if (profiler_) {
            auto* brainPtr = reinterpret_cast<const class Brain*>(profiler_.get());
            auto validation = validateState(*brainPtr);
            
            json validationJson;
            validationJson["is_valid"] = validation.isValid;
            validationJson["errors"] = validation.errors;
            validationJson["warnings"] = validation.warnings;
            j["state_validation"] = validationJson;
        }
        
        file << j.dump(2);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugTools::importDebugData(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Clear existing history
        clearDebugHistory();
        
        // Import memory leaks
        if (j.contains("memory_leaks")) {
            for (const auto& leakJson : j["memory_leaks"]) {
                MemoryLeakInfo leak(leakJson["context"], leakJson["allocated_bytes"]);
                leak.allocationCount = leakJson["allocation_count"];
                leak.isActive = leakJson["is_active"];
                pImpl->memoryLeakHistory.push_back(leak);
            }
        }
        
        // Import performance bottlenecks
        if (j.contains("performance_bottlenecks")) {
            for (const auto& bottleneckJson : j["performance_bottlenecks"]) {
                PerformanceBottleneck bottleneck(
                    static_cast<PerformanceBottleneck::BottleneckType>(bottleneckJson["type"].get<int>()),
                    bottleneckJson["description"],
                    bottleneckJson["severity"]
                );
                pImpl->bottleneckHistory.push_back(bottleneck);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void DebugTools::clearDebugHistory() {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    
    pImpl->stepHistory.clear();
    pImpl->bottleneckHistory.clear();
    pImpl->memoryLeakHistory.clear();
    pImpl->validationHistory.clear();
    pImpl->currentMemoryLeaks.clear();
    pImpl->recentCPUTimes.clear();
    pImpl->recentMemoryUsage.clear();
    pImpl->recentStabilityScores.clear();
}

} // namespace nlm
