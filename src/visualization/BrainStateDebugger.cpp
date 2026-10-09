// Implementation of brain state debugger
#include "BrainStateDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_set>

namespace nlm {

BrainStateDebugger::BrainStateDebugger()
    : maxSnapshots_(1000), anomalyDetectionThreshold_(0.7f), memoryLeakDetectionThreshold_(100),
      performanceAnomalyThreshold_(2.0f), enableDevelopmentTracking_(true), 
      enableNeuromodulationTracking_(true), enableMemoryLeakDetection_(true),
      enablePerformanceMonitoring_(true) {
}

BrainStateDebugger::~BrainStateDebugger() {
    shutdown();
}

bool BrainStateDebugger::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize BrainStateDebugger: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    snapshots_.clear();
    debugReports_.clear();
    comparisonResults_.clear();
    stateChanges_.clear();
    
    NLM_LOG_INFO("BrainStateDebugger initialized successfully");
    return true;
}

void BrainStateDebugger::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    brain_.reset();
    snapshots_.clear();
    debugReports_.clear();
    comparisonResults_.clear();
    stateChanges_.clear();
    
    // Clear callbacks
    stateChangeCallbacks_.clear();
    anomalyCallbacks_.clear();
    memoryLeakCallbacks_.clear();
    performanceAnomalyCallbacks_.clear();
    
    NLM_LOG_INFO("BrainStateDebugger shutdown complete");
}

void BrainStateDebugger::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain_) return;
    
    // Capture snapshot on key events
    SimulationStep currentStep = brain_->getCurrentStep();
    
    // Capture snapshot every N steps or on significant changes
    static SimulationStep lastSnapshotStep = 0;
    if (currentStep % 100 == 0 || currentStep == 0) {
        BrainSnapshot snapshot = captureSnapshot("Step " + std::to_string(currentStep));
        snapshots_.push_back(snapshot);
        
        // Maintain snapshot history
        if (snapshots_.size() > maxSnapshots_) {
            snapshots_.erase(snapshots_.begin());
        }
    }
    
    // Detect and report anomalies
    if (enableDevelopmentTracking_) {
        BrainSnapshot currentSnapshot = captureSnapshot("Anomaly Detection");
        DebugReport report = generateAnomalyReport(currentSnapshot);
        if (report.hasCriticalIssues) {
            debugReports_.push_back(report);
            
            // Notify callbacks
            for (const auto& callback : anomalyCallbacks_) {
                callback(report);
            }
        }
    }
}

BrainSnapshot BrainStateDebugger::captureSnapshot(const std::string& description) const {
    BrainSnapshot snapshot;
    
    if (!brain_) return snapshot;
    
    snapshot.step = brain_->getCurrentStep();
    snapshot.timestamp = brain_->getCurrentTime();
    snapshot.description = description;
    
    // Capture neural state
    size_t neuronCount = brain_->getTotalNeuronCount();
    snapshot.neuronMembranePotentials.resize(neuronCount);
    snapshot.neuronFiringRates.resize(neuronCount);
    
    // Extract data from brain (placeholder - real implementation would access brain components)
    for (size_t i = 0; i < neuronCount && i < snapshot.neuronMembranePotentials.size(); ++i) {
        snapshot.neuronMembranePotentials[i] = -70.0f + static_cast<float>(rand()) / RAND_MAX * 50.0f;
        snapshot.neuronFiringRates[i] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
    }
    
    // Capture synaptic state
    size_t synapseCount = brain_->getTotalSynapseCount();
    snapshot.synapticWeights.resize(synapseCount);
    
    for (size_t i = 0; i < synapseCount && i < snapshot.synapticWeights.size(); ++i) {
        snapshot.synapticWeights[i] = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
    }
    
    // Capture memory states
    if (brain_->getWorkingMemory()) {
        snapshot.memoryStates.resize(3); // working, episodic, associative
        snapshot.memoryStates[0] = brain_->getWorkingMemory()->getActiveTraces();
    } else {
        snapshot.memoryStates = {0, 0, 0};
    }
    
    // Capture development stages
    if (enableDevelopmentTracking_) {
        snapshot.developmentStages.resize(neuronCount);
        for (size_t i = 0; i < neuronCount && i < snapshot.developmentStages.size(); ++i) {
            snapshot.developmentStages[i] = static_cast<int>(rand()) % 4;
        }
    }
    
    // Capture global metrics
    snapshot.globalExcitatoryInhibitionRatio = brain_->getExcitationInhibitionRatio();
    snapshot.totalSpikeCount = brain_->getTotalSpikeCount();
    snapshot.activeNeuronCount = brain_->getActiveNeuronCount();
    snapshot.simulationTime = brain_->getCurrentTime();
    
    return snapshot;
}

bool BrainStateDebugger::restoreSnapshot(const BrainSnapshot& snapshot) {
    // Restore brain state from snapshot
    // This is a placeholder - real implementation would restore actual brain state
    NLM_LOG_INFO("Restoring brain state from snapshot at step " + std::to_string(snapshot.step));
    return true;
}

void BrainStateDebugger::clearSnapshots() {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshots_.clear();
    NLM_LOG_INFO("Cleared all snapshots");
}

ComparisonResult BrainStateDebugger::compareSnapshots(const BrainSnapshot& snapshot1, 
                                                     const BrainSnapshot& snapshot2) const {
    ComparisonResult result;
    result.step1 = snapshot1.step;
    result.step2 = snapshot2.step;
    
    // Compute differences
    result.totalDifference = computeStateDifference(snapshot1, snapshot2);
    result.neuronDifference = computeNeuronStateDifference(snapshot1, snapshot2);
    result.synapticDifference = computeSynapticStateDifference(snapshot1, snapshot2);
    result.memoryDifference = computeMemoryStateDifference(snapshot1, snapshot2);
    result.activityDifference = computeActivityStateDifference(snapshot1, snapshot2);
    
    // Detect significant differences
    result.isSignificant = (result.totalDifference > anomalyDetectionThreshold_);
    result.similarityScore = 1.0f - std::min(1.0f, result.totalDifference);
    
    // Generate state changes
    result.changes = detectDifferences(snapshot1, snapshot2);
    
    return result;
}

ComparisonResult BrainStateDebugger::compareWithPrevious(const BrainSnapshot& currentSnapshot) const {
    if (snapshots_.empty()) {
        return ComparisonResult{};
    }
    
    BrainSnapshot previousSnapshot = snapshots_.back();
    ComparisonResult result = compareSnapshots(previousSnapshot, currentSnapshot);
    
    // Add to comparison results
    comparisonResults_.push_back(result);
    
    // Notify callbacks
    for (const auto& callback : stateChangeCallbacks_) {
        for (const auto& change : result.changes) {
            callback(change);
        }
    }
    
    return result;
}

std::vector<ComparisonResult> BrainStateDebugger::compareSnapshots(const std::vector<BrainSnapshot>& snapshots) const {
    std::vector<ComparisonResult> results;
    
    for (size_t i = 1; i < snapshots.size(); ++i) {
        ComparisonResult result = compareSnapshots(snapshots[i-1], snapshots[i]);
        results.push_back(result);
    }
    
    return results;
}

std::vector<BrainStateChange> BrainStateDebugger::detectDifferences(const BrainSnapshot& snapshot1,
                                                                  const BrainSnapshot& snapshot2) const {
    std::vector<BrainStateChange> changes;
    
    if (snapshot1.step == snapshot2.step) {
        return changes; // Same snapshot
    }
    
    // Compare neuron states
    size_t minNeurons = std::min(snapshot1.neuronMembranePotentials.size(), 
                                 snapshot2.neuronMembranePotentials.size());
    
    for (size_t i = 0; i < minNeurons; ++i) {
        float oldValue = snapshot1.neuronMembranePotentials[i];
        float newValue = snapshot2.neuronMembranePotentials[i];
        
        if (std::abs(newValue - oldValue) > anomalyDetectionThreshold_ * 10.0f) {
            BrainStateChange change;
            change.step = snapshot2.step;
            change.timestamp = snapshot2.timestamp;
            change.changeType = "neuron_state";
            change.component = "neuron_" + std::to_string(i);
            change.oldValue = oldValue;
            change.newValue = newValue;
            change.changeMagnitude = std::abs(newValue - oldValue);
            change.description = "Membrane potential change from " + std::to_string(oldValue) + 
                               " to " + std::to_string(newValue) + " mV";
            change.affectedNeurons.push_back(static_cast<float>(i));
            changes.push_back(change);
        }
    }
    
    // Compare synaptic weights
    size_t minSynapses = std::min(snapshot1.synapticWeights.size(), 
                                  snapshot2.synapticWeights.size());
    
    for (size_t i = 0; i < minSynapses; ++i) {
        float oldWeight = snapshot1.synapticWeights[i];
        float newWeight = snapshot2.synapticWeights[i];
        
        if (std::abs(newWeight - oldWeight) > anomalyDetectionThreshold_ * 0.5f) {
            BrainStateChange change;
            change.step = snapshot2.step;
            change.timestamp = snapshot2.timestamp;
            change.changeType = "synaptic_weight";
            change.component = "synapse_" + std::to_string(i);
            change.oldValue = oldWeight;
            change.newValue = newWeight;
            change.changeMagnitude = std::abs(newWeight - oldWeight);
            change.description = "Synaptic weight change from " + std::to_string(oldWeight) + 
                               " to " + std::to_string(newWeight);
            change.affectedSynapses.push_back(static_cast<float>(i));
            changes.push_back(change);
        }
    }
    
    // Compare memory states
    if (snapshot1.memoryStates.size() == snapshot2.memoryStates.size()) {
        for (size_t i = 0; i < snapshot1.memoryStates.size(); ++i) {
            int oldMemory = snapshot1.memoryStates[i];
            int newMemory = snapshot2.memoryStates[i];
            
            if (std::abs(newMemory - oldMemory) > 10) {
                BrainStateChange change;
                change.step = snapshot2.step;
                change.timestamp = snapshot2.timestamp;
                change.changeType = "memory_state";
                change.component = "memory_" + std::to_string(i);
                change.oldValue = static_cast<float>(oldMemory);
                change.newValue = static_cast<float>(newMemory);
                change.changeMagnitude = static_cast<float>(std::abs(newMemory - oldMemory));
                change.description = "Memory state change from " + std::to_string(oldMemory) + 
                                   " to " + std::to_string(newMemory);
                changes.push_back(change);
            }
        }
    }
    
    return changes;
}

std::vector<BrainStateChange> BrainStateDebugger::detectDifferences(const BrainSnapshot& previousSnapshot) const {
    if (snapshots_.empty()) {
        return std::vector<BrainStateChange>();
    }
    
    BrainSnapshot currentSnapshot = snapshots_.back();
    return detectDifferences(previousSnapshot, currentSnapshot);
}

std::vector<std::vector<BrainSnapshot>> BrainStateDebugger::detectStateTrajectories(size_t minTrajectoryLength) const {
    std::vector<std::vector<BrainSnapshot>> trajectories;
    
    if (snapshots_.size() < minTrajectoryLength) {
        return trajectories;
    }
    
    // Simple trajectory detection based on clustering similar states
    std::vector<size_t> currentTrajectory;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        if (currentTrajectory.empty()) {
            currentTrajectory.push_back(i);
        } else {
            // Check if this state is similar to previous ones
            bool isSimilar = false;
            for (size_t j : currentTrajectory) {
                float similarity = computeStateDifference(snapshots_[j], snapshots_[i]);
                if (similarity < 0.3f) { // Similarity threshold
                    isSimilar = true;
                    break;
                }
            }
            
            if (isSimilar) {
                currentTrajectory.push_back(i);
            } else {
                // Start new trajectory
                if (currentTrajectory.size() >= minTrajectoryLength) {
                    trajectories.push_back(getSnapshotsFromIndices(currentTrajectory));
                }
                currentTrajectory.clear();
                currentTrajectory.push_back(i);
            }
        }
    }
    
    // Add final trajectory if it's long enough
    if (currentTrajectory.size() >= minTrajectoryLength) {
        trajectories.push_back(getSnapshotsFromIndices(currentTrajectory));
    }
    
    return trajectories;
}

std::vector<std::string> BrainStateDebugger::analyzeStateTransitions(const BrainSnapshot& snapshot) const {
    std::vector<std::string> transitions;
    
    if (snapshot.neuronMembranePotentials.empty()) {
        return transitions;
    }
    
    // Analyze membrane potential transitions
    float avgPotential = computeMean(snapshot.neuronMembranePotentials);
    
    if (avgPotential > -50.0f) {
        transitions.push_back("hyper_excitability");
    } else if (avgPotential < -80.0f) {
        transitions.push_back("hyper_polarization");
    }
    
    // Analyze firing rate transitions
    float avgFiringRate = computeMean(snapshot.neuronFiringRates);
    
    if (avgFiringRate > 50.0f) {
        transitions.push_back("high_activity");
    } else if (avgFiringRate < 5.0f) {
        transitions.push_back("low_activity");
    }
    
    // Analyze development stage transitions
    if (enableDevelopmentTracking_) {
        int currentStage = 0;
        if (!snapshot.developmentStages.empty()) {
            currentStage = snapshot.developmentStages[0];
        }
        
        if (currentStage >= 2) { // Maturation or Adult
            transitions.push_back("advanced_development");
        } else if (currentStage == 1) { // Critical period
            transitions.push_back("critical_period");
        }
    }
    
    return transitions;
}

bool BrainStateDebugger::detectAnomalies(const BrainSnapshot& snapshot) const {
    // Check for various types of anomalies
    bool hasAnomalies = false;
    
    // Check for neuron state anomalies
    if (!snapshot.neuronMembranePotentials.empty()) {
        float minPotential = *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        float maxPotential = *std::max_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        
        if (minPotential < -100.0f || maxPotential > 50.0f) {
            hasAnomalies = true;
        }
    }
    
    // Check for synaptic anomalies
    if (!snapshot.synapticWeights.empty()) {
        float minWeight = *std::min_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        float maxWeight = *std::max_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        
        if (minWeight < -2.0f || maxWeight > 2.0f) {
            hasAnomalies = true;
        }
    }
    
    // Check for memory anomalies
    for (int memoryState : snapshot.memoryStates) {
        if (memoryState < 0 || memoryState > 10000) {
            hasAnomalies = true;
            break;
        }
    }
    
    return hasAnomalies;
}

DebugReport BrainStateDebugger::generateAnomalyReport(const BrainSnapshot& snapshot) const {
    DebugReport report;
    report.step = snapshot.step;
    report.timestamp = snapshot.timestamp;
    report.reportType = "anomaly_detection";
    report.title = "Brain State Anomaly Report";
    report.description = "Analysis of brain state for anomalies at step " + std::to_string(snapshot.step);
    
    // Check for anomalies
    bool hasAnomalies = detectAnomalies(snapshot);
    
    if (hasAnomalies) {
        report.hasCriticalIssues = true;
        report.requiresIntervention = true;
        
        // Generate specific findings
        if (!snapshot.neuronMembranePotentials.empty()) {
            float minPotential = *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                                  snapshot.neuronMembranePotentials.end());
            float maxPotential = *std::max_element(snapshot.neuronMembranePotentials.begin(), 
                                                  snapshot.neuronMembranePotentials.end());
            
            if (minPotential < -100.0f) {
                report.errors.push_back("Neurons with dangerously low membrane potential (< -100mV)");
            }
            if (maxPotential > 50.0f) {
                report.errors.push_back("Neurons with dangerously high membrane potential (> 50mV)");
            }
        }
        
        // Generate recommendations
        report.recommendations.push_back("Check neural excitability and synaptic stability");
        report.recommendations.push_back("Review neuromodulation levels");
        report.recommendations.push_back("Analyze recent learning events");
        
        // Set metrics
        report.metrics["total_neuron_count"] = static_cast<float>(snapshot.neuronMembranePotentials.size());
        report.metrics["avg_membrane_potential"] = computeMean(snapshot.neuronMembranePotentials);
        report.metrics["potential_range"] = *std::max_element(snapshot.neuronMembranePotentials.begin(), 
                                                            snapshot.neuronMembranePotentials.end()) -
                                           *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                                            snapshot.neuronMembranePotentials.end());
    } else {
        report.hasCriticalIssues = false;
        report.requiresIntervention = false;
        
        report.findings.push_back("Brain state appears normal");
        report.findings.push_back("All neurons within physiological ranges");
        
        if (enableDevelopmentTracking_) {
            report.findings.push_back("Development stage consistent with expected progression");
        }
    }
    
    return report;
}

std::vector<MemoryLeak> BrainStateDebugger::detectMemoryLeaks() const {
    std::vector<MemoryLeak> leaks;
    
    if (!enableMemoryLeakDetection_) {
        return leaks;
    }
    
    // Track memory usage over time
    if (snapshots_.size() < memoryLeakDetectionThreshold_) {
        return leaks; // Not enough data to detect leaks
    }
    
    // Analyze memory usage patterns
    std::vector<size_t> memoryUsage;
    for (const auto& snapshot : snapshots_) {
        memoryUsage.push_back(snapshot.memoryStates.empty() ? 0 : snapshot.memoryStates[0]);
    }
    
    // Detect increasing memory trend
    if (memoryUsage.size() >= 2) {
        // Calculate linear regression trend
        float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
        size_t n = memoryUsage.size();
        
        for (size_t i = 0; i < n; ++i) {
            float x = static_cast<float>(i);
            float y = static_cast<float>(memoryUsage[i]);
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }
        
        float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        float intercept = (sumY - slope * sumX) / n;
        
        // If slope is positive and significant, we might have a leak
        if (slope > 0.1f && slope > memoryLeakDetectionThreshold_) {
            MemoryLeak leak;
            leak.leakType = "working_memory_growth";
            leak.sourceComponent = "working_memory";
            leak.firstDetected = static_cast<SimulationStep>(n - 10);
            leak.lastObserved = static_cast<SimulationStep>(n - 1);
            leak.estimatedLeakSize = static_cast<size_t>(slope * 10.0f);
            leak.memoryPool = "working_memory_pool";
            leak.isPersistent = slope > 0.5f;
            leak.description = "Memory usage is increasing over time, suggesting a potential leak";
            leaks.push_back(leak);
        }
    }
    
    return leaks;
}

bool BrainStateDebugger::hasMemoryLeaks() const {
    return !detectMemoryLeaks().empty();
}

DebugReport BrainStateDebugger::generateMemoryLeakReport(const std::vector<MemoryLeak>& leaks) const {
    DebugReport report;
    report.step = snapshots_.empty() ? 0 : snapshots_.back().step;
    report.reportType = "memory_leak_detection";
    report.title = "Memory Leak Analysis Report";
    report.description = "Analysis of memory usage for potential leaks";
    
    if (leaks.empty()) {
        report.findings.push_back("No memory leaks detected");
        report.findings.push_back("Memory usage patterns are stable");
    } else {
        report.hasCriticalIssues = true;
        report.requiresIntervention = true;
        
        for (const auto& leak : leaks) {
            std::string message = "Memory leak detected: " + leak.description + 
                                " (estimated " + std::to_string(leak.estimatedLeakSize) + " elements)";
            report.errors.push_back(message);
        }
        
        report.recommendations.push_back("Clear working memory buffers regularly");
        report.recommendations.push_back("Optimize memory allocation patterns");
        report.recommendations.push_back("Implement memory pooling");
        
        report.metrics["total_leaks"] = static_cast<float>(leaks.size());
        report.metrics["estimated_total_leak_size"] = 0.0f;
        
        for (const auto& leak : leaks) {
            report.metrics["estimated_total_leak_size"] += static_cast<float>(leak.estimatedLeakSize);
        }
    }
    
    return report;
}

std::vector<PerformanceAnomaly> BrainStateDebugger::detectPerformanceAnomalies(const PerformanceMetrics& metrics) const {
    std::vector<PerformanceAnomaly> anomalies;
    
    if (!enablePerformanceMonitoring_) {
        return anomalies;
    }
    
    // Check for performance anomalies
    if (metrics.simulationTime > performanceAnomalyThreshold_) {
        PerformanceAnomaly anomaly;
        anomaly.anomalyType = "simulation_performance";
        anomaly.component = "simulation_engine";
        anomaly.detectionStep = metrics.step;
        anomaly.baselinePerformance = 100.0f; // Placeholder
        anomaly.currentPerformance = 1.0f / metrics.simulationTime; // Throughput
        anomaly.performanceDelta = anomaly.currentPerformance - anomaly.baselinePerformance;
        anomaly.thresholdExceeded = metrics.simulationTime;
        anomaly.description = "Simulation time exceeded threshold";
        anomaly.isCritical = metrics.simulationTime > performanceAnomalyThreshold_ * 2.0f;
        anomaly.potentialCauses.push_back("Inefficient neural updates");
        anomaly.potentialCauses.push_back("Memory allocation issues");
        anomaly.potentialCauses.push_back("Inefficient visualization rendering");
        anomalies.push_back(anomaly);
    }
    
    // Check for memory usage anomalies
    if (metrics.memoryUsage > memoryLeakDetectionThreshold_) {
        PerformanceAnomaly anomaly;
        anomaly.anomalyType = "memory_usage";
        anomaly.component = "memory_manager";
        anomaly.detectionStep = metrics.step;
        anomaly.baselinePerformance = 100.0f; // Placeholder
        anomaly.currentPerformance = 1.0f / (metrics.memoryUsage + 1.0f); // Inverse usage
        anomaly.performanceDelta = anomaly.currentPerformance - anomaly.baselinePerformance;
        anomaly.thresholdExceeded = static_cast<float>(metrics.memoryUsage);
        anomaly.description = "Memory usage exceeded threshold";
        anomaly.isCritical = metrics.memoryUsage > memoryLeakDetectionThreshold_ * 2;
        anomaly.potentialCauses.push_back("Memory leak");
        anomaly.potentialCauses.push_back("Inefficient data structures");
        anomaly.potentialCauses.push_back("Excessive visualization data");
        anomalies.push_back(anomaly);
    }
    
    return anomalies;
}

DebugReport BrainStateDebugger::generatePerformanceReport(const std::vector<PerformanceAnomaly>& anomalies) const {
    DebugReport report;
    report.step = snapshots_.empty() ? 0 : snapshots_.back().step;
    report.reportType = "performance_analysis";
    report.title = "Performance Analysis Report";
    report.description = "Analysis of system performance for anomalies";
    
    if (anomalies.empty()) {
        report.findings.push_back("Performance within normal ranges");
        report.findings.push_back("All metrics within expected thresholds");
    } else {
        report.hasCriticalIssues = false;
        report.requiresIntervention = false;
        
        for (const auto& anomaly : anomalies) {
            if (anomaly.isCritical) {
                report.errors.push_back("CRITICAL: " + anomaly.description);
                report.hasCriticalIssues = true;
                report.requiresIntervention = true;
            } else {
                report.warnings.push_back("WARNING: " + anomaly.description);
            }
        }
        
        report.recommendations.push_back("Optimize neural computation efficiency");
        report.recommendations.push_back("Implement memory management optimizations");
        report.recommendations.push_back("Consider parallel processing");
        
        report.metrics["total_anomalies"] = static_cast<float>(anomalies.size());
        report.metrics["critical_anomalies"] = 0.0f;
        
        for (const auto& anomaly : anomalies) {
            if (anomaly.isCritical) {
                report.metrics["critical_anomalies"]++;
            }
        }
    }
    
    return report;
}

std::vector<ConfigurationIssue> BrainStateDebugger::validateConfiguration() const {
    std::vector<ConfigurationIssue> issues;
    
    // Validate configuration parameters
    if (!brain_ || !brain_->getConfig()) {
        ConfigurationIssue issue;
        issue.issueType = "brain_config";
        issue.parameterName = "config";
        issue.currentValue = 0.0f;
        issue.validRangeMin = 1.0f;
        issue.validRangeMax = 1.0f;
        issue.description = "Brain configuration is not available";
        issue.severity = "critical";
        issue.isBlocking = true;
        issue.suggestedFixes.push_back("Initialize brain with valid configuration");
        issues.push_back(issue);
        return issues;
    }
    
    auto config = brain_->getConfig();
    
    // Check simulation timestep
    if (auto timestep = config->getOr<float>("simulation_timestep", 0.001f)) {
        if (*timestep <= 0.0f || *timestep > 1.0f) {
            ConfigurationIssue issue;
            issue.issueType = "simulation_parameters";
            issue.parameterName = "simulation_timestep";
            issue.currentValue = *timestep;
            issue.validRangeMin = 0.0001f;
            issue.validRangeMax = 1.0f;
            issue.description = "Simulation timestep is out of valid range";
            issue.severity = "high";
            issue.isBlocking = false;
            issue.suggestedFixes.push_back("Set timestep between 0.0001s and 1.0s");
            issues.push_back(issue);
        }
    }
    
    // Check neuron count
    if (auto neuronCount = config->getOr<size_t>("neuron_count", 1000)) {
        if (*neuronCount == 0) {
            ConfigurationIssue issue;
            issue.issueType = "brain_size";
            issue.parameterName = "neuron_count";
            issue.currentValue = static_cast<float>(*neuronCount);
            issue.validRangeMin = 1.0f;
            issue.validRangeMax = 1000000.0f;
            issue.description = "Neuron count cannot be zero";
            issue.severity = "critical";
            issue.isBlocking = true;
            issue.suggestedFixes.push_back("Set neuron count to at least 1");
            issues.push_back(issue);
        } else if (*neuronCount > 1000000) {
            ConfigurationIssue issue;
            issue.issueType = "brain_size";
            issue.parameterName = "neuron_count";
            issue.currentValue = static_cast<float>(*neuronCount);
            issue.validRangeMin = 1.0f;
            issue.validRangeMax = 1000000.0f;
            issue.description = "Neuron count exceeds recommended maximum";
            issue.severity = "medium";
            issue.isBlocking = false;
            issue.suggestedFixes.push_back("Consider reducing neuron count for better performance");
            issues.push_back(issue);
        }
    }
    
    // Check connectivity probability
    if (auto connProb = config->getOr<float>("connection_probability", 0.1f)) {
        if (*connProb < 0.0f || *connProb > 1.0f) {
            ConfigurationIssue issue;
            issue.issueType = "connectivity";
            issue.parameterName = "connection_probability";
            issue.currentValue = *connProb;
            issue.validRangeMin = 0.0f;
            issue.validRangeMax = 1.0f;
            issue.description = "Connection probability must be between 0 and 1";
            issue.severity = "high";
            issue.isBlocking = false;
            issue.suggestedFixes.push_back("Set connection probability between 0.0 and 1.0");
            issues.push_back(issue);
        }
    }
    
    return issues;
}

DebugReport BrainStateDebugger::generateConfigurationReport(const std::vector<ConfigurationIssue>& issues) const {
    DebugReport report;
    report.step = snapshots_.empty() ? 0 : snapshots_.back().step;
    report.reportType = "configuration_validation";
    report.title = "Configuration Validation Report";
    report.description = "Analysis of configuration parameters for issues";
    
    if (issues.empty()) {
        report.findings.push_back("Configuration parameters are valid");
        report.findings.push_back("All settings within recommended ranges");
    } else {
        report.hasCriticalIssues = false;
        report.requiresIntervention = false;
        
        for (const auto& issue : issues) {
            if (issue.severity == "critical" || issue.isBlocking) {
                report.errors.push_back("CRITICAL: " + issue.description);
                report.hasCriticalIssues = true;
                report.requiresIntervention = true;
            } else if (issue.severity == "high") {
                report.errors.push_back("ERROR: " + issue.description);
            } else {
                report.warnings.push_back("WARNING: " + issue.description);
            }
        }
        
        report.recommendations.push_back("Review and fix configuration issues");
        report.recommendations.push_back("Consider running with default configuration");
        report.recommendations.push_back("Test simulation with modified parameters");
        
        report.metrics["total_issues"] = static_cast<float>(issues.size());
        report.metrics["critical_issues"] = 0.0f;
        
        for (const auto& issue : issues) {
            if (issue.severity == "critical" || issue.isBlocking) {
                report.metrics["critical_issues"]++;
            }
        }
    }
    
    return report;
}

DevelopmentStageReport BrainStateDebugger::analyzeDevelopmentStage(const BrainSnapshot& snapshot) const {
    DevelopmentStageReport report;
    report.step = snapshot.step;
    report.timestamp = snapshot.timestamp;
    
    // Determine development stage
    if (!enableDevelopmentTracking_) {
        report.stage = DevelopmentalStage::Adult; // Default
    } else {
        // Simple stage determination based on simulation step
        if (snapshot.step < 1000) {
            report.stage = DevelopmentalStage::Initial;
        } else if (snapshot.step < 10000) {
            report.stage = DevelopmentalStage::CriticalPeriod;
        } else if (snapshot.step < 50000) {
            report.stage = DevelopmentalStage::Maturation;
        } else {
            report.stage = DevelopmentalStage::Adult;
        }
    }
    
    // Calculate plasticity rate
    report.plasticityRate = 1.0f / (1.0f + std::log10(static_cast<float>(snapshot.step) + 1.0f));
    
    // Calculate development progress
    report.developmentProgress = std::min(1.0f, static_cast<float>(snapshot.step) / 100000.0f);
    
    // Calculate maturity level
    report.maturityLevel = 1.0f - report.developmentProgress;
    
    // Check developmental milestones
    if (snapshot.step >= 1000) {
        report.developmentMilestones.push_back("Initial development complete");
        report.hasCompletedMilestone = true;
    }
    
    if (snapshot.step >= 10000) {
        report.developmentMilestones.push_back("Critical period completed");
        report.hasCompletedMilestone = true;
    }
    
    if (snapshot.step >= 50000) {
        report.developmentMilestones.push_back("Maturation completed");
        report.hasCompletedMilestone = true;
    }
    
    report.nextMilestoneDescription = "Reach adult development stage with stable neural connections";
    
    return report;
}

bool BrainStateDebugger::isDevelopmentOnTrack() const {
    if (!enableDevelopmentTracking_) {
        return true; // Cannot determine without tracking
    }
    
    if (snapshots_.size() < 2) {
        return true; // Not enough data
    }
    
    // Check if development stage is progressing normally
    size_t oldStage = static_cast<size_t>(snapshots_[0].developmentStages.empty() ? 
                                         DevelopmentStage::Initial : 
                                         static_cast<DevelopmentalStage>(snapshots_[0].developmentStages[0]));
    size_t newStage = static_cast<size_t>(snapshots_.back().developmentStages.empty() ? 
                                         DevelopmentStage::Initial : 
                                         static_cast<DevelopmentalStage>(snapshots_.back().developmentStages[0]));
    
    return newStage >= oldStage; // Development should not regress
}

NeuromodulationReport BrainStateDebugger::analyzeNeuromodulation(const BrainSnapshot& snapshot) const {
    NeuromodulationReport report;
    report.step = snapshot.step;
    report.timestamp = snapshot.timestamp;
    
    // This is a placeholder - real implementation would extract neuromodulation data
    // For now, we'll simulate neuromodulation levels
    report.dopamineLevel = 0.5f;
    report.curiosityLevel = 0.3f;
    report.noveltyLevel = 0.2f;
    report.predictionErrorLevel = 0.4f;
    
    // Calculate overall modulation
    report.overallModulation = (report.dopamineLevel + report.curiosityLevel + 
                               report.noveltyLevel + report.predictionErrorLevel) / 4.0f;
    
    // Determine active modulators
    if (report.dopamineLevel > 0.3f) report.activeModulators.push_back("dopamine");
    if (report.curiosityLevel > 0.2f) report.activeModulators.push_back("curiosity");
    if (report.noveltyLevel > 0.1f) report.activeModulators.push_back("novelty");
    if (report.predictionErrorLevel > 0.3f) report.activeModulators.push_back("prediction_error");
    
    report.isModulationActive = report.overallModulation > 0.2f;
    
    // Calculate modulation effects
    report.modulationEffects["learning_rate"] = report.dopamineLevel * 0.5f;
    report.modulationEffects["exploration"] = report.curiosityLevel * 0.8f;
    report.modulationEffects["adaptation"] = report.noveltyLevel * 0.6f;
    report.modulationEffects["prediction_adjustment"] = report.predictionErrorLevel * 0.7f;
    
    return report;
}

bool BrainStateDebugger::isNeuromodulationBalanced() const {
    if (!enableNeuromodulationTracking_) {
        return true; // Cannot determine without tracking
    }
    
    // Simple balance check
    float totalModulation = 0.0f;
    size_t numSnapshots = std::min(static_cast<size_t>(10), snapshots_.size());
    
    for (size_t i = 0; i < numSnapshots; ++i) {
        size_t idx = snapshots_.size() - 1 - i;
        const auto& snapshot = snapshots_[idx];
        
        // Simulate neuromodulation calculation
        totalModulation += (static_cast<float>(rand()) / RAND_MAX) * 0.5f + 0.1f;
    }
    
    float avgModulation = totalModulation / static_cast<float>(numSnapshots);
    return std::abs(avgModulation - 0.5f) < 0.2f; // Within reasonable range of 0.5
}

std::vector<std::string> BrainStateDebugger::getDebugInfo(const BrainSnapshot& snapshot) const {
    std::vector<std::string> debugInfo;
    
    debugInfo.push_back("Step: " + std::to_string(snapshot.step));
    debugInfo.push_back("Time: " + std::to_string(snapshot.simulationTime) + " s");
    debugInfo.push_back("Neurons: " + std::to_string(snapshot.neuronMembranePotentials.size()));
    debugInfo.push_back("Synapses: " + std::to_string(snapshot.synapticWeights.size()));
    debugInfo.push_back("Active neurons: " + std::to_string(snapshot.activeNeuronCount));
    debugInfo.push_back("Total spikes: " + std::to_string(snapshot.totalSpikeCount));
    
    if (!snapshot.neuronMembranePotentials.empty()) {
        float avgPotential = computeMean(snapshot.neuronMembranePotentials);
        float minPotential = *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        float maxPotential = *std::max_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        
        debugInfo.push_back("Avg membrane potential: " + std::to_string(avgPotential) + " mV");
        debugInfo.push_back("Potential range: " + std::to_string(minPotential) + " to " + std::to_string(maxPotential) + " mV");
    }
    
    if (!snapshot.synapticWeights.empty()) {
        float avgWeight = computeMean(snapshot.synapticWeights);
        float weightRange = *std::max_element(snapshot.synapticWeights.begin(), 
                                            snapshot.synapticWeights.end()) -
                           *std::min_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        
        debugInfo.push_back("Avg synaptic weight: " + std::to_string(avgWeight));
        debugInfo.push_back("Synaptic weight range: " + std::to_string(weightRange));
    }
    
    debugInfo.push_back("E/I ratio: " + std::to_string(snapshot.globalExcitatoryInhibitionRatio));
    
    return debugInfo;
}

std::vector<std::string> BrainStateDebugger::getRecommendations(const BrainSnapshot& snapshot) const {
    std::vector<std::string> recommendations;
    
    // Generate recommendations based on current state
    if (!snapshot.neuronMembranePotentials.empty()) {
        float avgPotential = computeMean(snapshot.neuronMembranePotentials);
        
        if (avgPotential < -80.0f) {
            recommendations.push_back("Increase overall excitability (e.g., increase neuromodulation)");
        } else if (avgPotential > -50.0f) {
            recommendations.push_back("Decrease overall excitability (e.g., increase inhibition)");
        }
    }
    
    if (!snapshot.synapticWeights.empty()) {
        float avgWeight = computeMean(snapshot.synapticWeights);
        
        if (std::abs(avgWeight) > 0.5f) {
            recommendations.push_back("Consider synaptic scaling to maintain stability");
        }
    }
    
    if (snapshot.totalSpikeCount > 1000) {
        recommendations.push_back("Spike rate is high - consider reducing input or increasing inhibition");
    } else if (snapshot.totalSpikeCount < 10) {
        recommendations.push_back("Spike rate is low - consider increasing input or reducing inhibition");
    }
    
    if (snapshot.activeNeuronCount < 10) {
        recommendations.push_back("Low neural activity - check sensory inputs or neuromodulation");
    } else if (snapshot.activeNeuronCount > snapshot.neuronMembranePotentials.size() * 0.8f) {
        recommendations.push_back("High neural activity - consider implementing competition or pruning");
    }
    
    recommendations.push_back("Save brain state checkpoint for recovery");
    recommendations.push_back("Consider replaying important episodes");
    
    return recommendations;
}

std::vector<std::string> BrainStateDebugger::getWarnings(const BrainSnapshot& snapshot) const {
    std::vector<std::string> warnings;
    
    if (!snapshot.neuronMembranePotentials.empty()) {
        float minPotential = *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        float maxPotential = *std::max_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        
        if (minPotential < -90.0f) {
            warnings.push_back("Some neurons are hyperpolarized (near resting potential)");
        }
        if (maxPotential > -40.0f) {
            warnings.push_back("Some neurons are depolarized (close to firing threshold)");
        }
    }
    
    if (!snapshot.synapticWeights.empty()) {
        float minWeight = *std::min_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        float maxWeight = *std::max_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        
        if (std::abs(minWeight) > 1.5f) {
            warnings.push_back("Extremely strong inhibitory synapses detected");
        }
        if (maxWeight > 1.5f) {
            warnings.push_back("Extremely strong excitatory synapses detected");
        }
    }
    
    if (snapshot.totalSpikeCount > 10000) {
        warnings.push_back("Very high total spike count - potential instability");
    }
    
    return warnings;
}

std::vector<std::string> BrainStateDebugger::getErrors(const BrainSnapshot& snapshot) const {
    std::vector<std::string> errors;
    
    if (!snapshot.neuronMembranePotentials.empty()) {
        float minPotential = *std::min_element(snapshot.neuronMembranePotentials.begin(), 
                                              snapshot.neuronMembranePotentials.end());
        
        if (minPotential < -120.0f) {
            errors.push_back("Critical: Neurons with dangerously low membrane potential (< -120mV)");
        }
    }
    
    if (!snapshot.synapticWeights.empty()) {
        float minWeight = *std::min_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        float maxWeight = *std::max_element(snapshot.synapticWeights.begin(), 
                                           snapshot.synapticWeights.end());
        
        if (minWeight < -2.5f || maxWeight > 2.5f) {
            errors.push_back("Critical: Synaptic weights outside safe bounds");
        }
    }
    
    return errors;
}

void BrainStateDebugger::visualizeStateTransition(const BrainSnapshot& snapshot1, 
                                                 const BrainSnapshot& snapshot2) const {
    // Visualize state transition between two snapshots
    // This would use a graphics library to render the changes
    ComparisonResult comparison = compareSnapshots(snapshot1, snapshot2);
    
    if (comparison.isSignificant) {
        // Render significant differences
        // This is a placeholder - real implementation would render visualization
    }
}

void BrainStateDebugger::visualizeMemoryLeaks(const std::vector<MemoryLeak>& leaks) const {
    // Visualize memory leaks
    // This would use a graphics library to render memory leak information
    // This is a placeholder - real implementation would render visualization
}

void BrainStateDebugger::visualizePerformanceAnomalies(const std::vector<PerformanceAnomaly>& anomalies) const {
    // Visualize performance anomalies
    // This would use a graphics library to render performance information
    // This is a placeholder - real implementation would render visualization
}

void BrainStateDebugger::visualizeConfigurationIssues(const std::vector<ConfigurationIssue>& issues) const {
    // Visualize configuration issues
    // This would use a graphics library to render configuration validation results
    // This is a placeholder - real implementation would render visualization
}

void BrainStateDebugger::exportSnapshots(const std::string& directory, ExportFormat format) const {
    // Export snapshots to files
    std::filesystem::create_directories(directory);
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        std::string filename;
        switch (format) {
            case ExportFormat::JSON:
                filename = directory + "/snapshot_" + std::to_string(i) + ".json";
                exportSnapshotToJSON(snapshots_[i], filename);
                break;
            case ExportFormat::Binary:
                filename = directory + "/snapshot_" + std::to_string(i) + ".bin";
                exportSnapshotToBinary(snapshots_[i], filename);
                break;
            default:
                filename = directory + "/snapshot_" + std::to_string(i) + ".txt";
                // Export to text format
                break;
        }
    }
}

void BrainStateDebugger::exportDebugReport(const DebugReport& report, const std::string& filepath) const {
    // Export debug report to file
    switch (getFileExtension(filepath)) {
        case ExportFormat::JSON:
            exportDebugReportToJSON(report, filepath);
            break;
        case ExportFormat::Binary:
            // Export to binary format
            break;
        default:
            // Export to text format
            break;
    }
}

void BrainStateDebugger::exportComparisonResults(const ComparisonResult& result, const std::string& filepath) const {
    // Export comparison results to file
    switch (getFileExtension(filepath)) {
        case ExportFormat::JSON:
            exportComparisonResultToJSON(result, filepath);
            break;
        case ExportFormat::Binary:
            // Export to binary format
            break;
        default:
            // Export to text format
            break;
    }
}

void BrainStateDebugger::exportSnapshotToJSON(const BrainSnapshot& snapshot, const std::string& filepath) const {
    // Export snapshot to JSON format
    // This would use a JSON library to serialize snapshot data
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"step\": " << snapshot.step << ",\n";
        file << "  \"timestamp\": " << snapshot.timestamp << ",\n";
        file << "  \"description\": \"" << snapshot.description << "\",\n";
        file << "  \"neuron_count\": " << snapshot.neuronMembranePotentials.size() << ",\n";
        file << "  \"synapse_count\": " << snapshot.synapticWeights.size() << ",\n";
        file << "  \"active_neurons\": " << snapshot.activeNeuronCount << ",\n";
        file << "  \"total_spikes\": " << snapshot.totalSpikeCount << ",\n";
        file << "  \"e_i_ratio\": " << snapshot.globalExcitatoryInhibitionRatio << ",\n";
        file << "  \"simulation_time\": " << snapshot.simulationTime << "\n";
        file << "}\n";
        file.close();
    }
}

void BrainStateDebugger::exportSnapshotToBinary(const BrainSnapshot& snapshot, const std::string& filepath) const {
    // Export snapshot to binary format
    std::ofstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        // Write binary data
        file.write(reinterpret_cast<const char*>(&snapshot.step), sizeof(snapshot.step));
        file.write(reinterpret_cast<const char*>(&snapshot.timestamp), sizeof(snapshot.timestamp));
        file.write(snapshot.description.c_str(), snapshot.description.size() + 1);
        
        // Write arrays
        auto writeArray = [&file](const auto& arr) {
            uint64_t size = arr.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            file.write(reinterpret_cast<const char*>(arr.data()), size * sizeof(arr[0]));
        };
        
        writeArray(snapshot.neuronMembranePotentials);
        writeArray(snapshot.neuronFiringRates);
        writeArray(snapshot.synapticWeights);
        writeArray(snapshot.memoryStates);
        writeArray(snapshot.developmentStages);
        
        file.close();
    }
}

void BrainStateDebugger::exportDebugReportToJSON(const DebugReport& report, const std::string& filepath) const {
    // Export debug report to JSON format
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"step\": " << report.step << ",\n";
        file << "  \"timestamp\": \"" << report.title << "\",\n";
        file << "  \"report_type\": \"" << report.reportType << "\",\n";
        file << "  \"title\": \"" << report.title << "\",\n";
        file << "  \"description\": \"" << report.description << "\",\n";
        file << "  \"has_critical_issues\": " << (report.hasCriticalIssues ? "true" : "false") << ",\n";
        file << "  \"requires_intervention\": " << (report.requiresIntervention ? "true" : "false") << ",\n";
        file << "  \"findings\": [\n";
        for (size_t i = 0; i < report.findings.size(); ++i) {
            file << "    \"" << report.findings[i] << "\"";
            if (i < report.findings.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";
        file << "  \"warnings\": [\n";
        for (size_t i = 0; i < report.warnings.size(); ++i) {
            file << "    \"" << report.warnings[i] << "\"";
            if (i < report.warnings.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";
        file << "  \"errors\": [\n";
        for (size_t i = 0; i < report.errors.size(); ++i) {
            file << "    \"" << report.errors[i] << "\"";
            if (i < report.errors.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";
        file << "  \"recommendations\": [\n";
        for (size_t i = 0; i < report.recommendations.size(); ++i) {
            file << "    \"" << report.recommendations[i] << "\"";
            if (i < report.recommendations.size() - 1) file << ",";
            file << "\n";
        }
        file << "  ],\n";
        file << "  \"metrics\": {\n";
        size_t metricCount = 0;
        for (const auto& pair : report.metrics) {
            file << "    \"" << pair.first << "\": " << pair.second;
            if (++metricCount < report.metrics.size()) file << ",";
            file << "\n";
        }
        file << "  }\n";
        file << "}\n";
        file.close();
    }
}

void BrainStateDebugger::exportComparisonResultToJSON(const ComparisonResult& result, const std::string& filepath) const {
    // Export comparison result to JSON format
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"step1\": " << result.step1 << ",\n";
        file << "  \"step2\": " << result.step2 << ",\n";
        file << "  \"total_difference\": " << result.totalDifference << ",\n";
        file << "  \"neuron_difference\": " << result.neuronDifference << ",\n";
        file << "  \"synaptic_difference\": " << result.synapticDifference << ",\n";
        file << "  \"memory_difference\": " << result.memoryDifference << ",\n";
        file << "  \"activity_difference\": " << result.activityDifference << ",\n";
        file << "  \"is_significant\": " << (result.isSignificant ? "true" : "false") << ",\n";
        file << "  \"similarity_score\": " << result.similarityScore << ",\n";
        file << "  \"changes\": [\n";
        
        for (size_t i = 0; i < result.changes.size(); ++i) {
            file << "    {\n";
            file << "      \"step\": " << result.changes[i].step << ",\n";
            file << "      \"change_type\": \"" << result.changes[i].changeType << "\",\n";
            file << "      \"component\": \"" << result.changes[i].component << "\",\n";
            file << "      \"old_value\": " << result.changes[i].oldValue << ",\n";
            file << "      \"new_value\": " << result.changes[i].newValue << ",\n";
            file << "      \"change_magnitude\": " << result.changes[i].changeMagnitude << ",\n";
            file << "      \"description\": \"" << result.changes[i].description << "\"\n";
            file << "    }";
            if (i < result.changes.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
    }
}

std::vector<BrainSnapshot> BrainStateDebugger::getSnapshots() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return snapshots_;
}

BrainSnapshot BrainStateDebugger::getLatestSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (snapshots_.empty()) {
        return BrainSnapshot{};
    }
    
    return snapshots_.back();
}

BrainSnapshot BrainStateDebugger::getPreviousSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (snapshots_.size() < 2) {
        return BrainSnapshot{};
    }
    
    return snapshots_[snapshots_.size() - 2];
}

std::vector<DebugReport> BrainStateDebugger::getDebugReports() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return debugReports_;
}

std::vector<ComparisonResult> BrainStateDebugger::getComparisonResults() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return comparisonResults_;
}

void BrainStateDebugger::registerStateChangeCallback(std::function<void(const BrainStateChange&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    stateChangeCallbacks_.push_back(callback);
}

void BrainStateDebugger::registerAnomalyCallback(std::function<void(const DebugReport&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    anomalyCallbacks_.push_back(callback);
}

void BrainStateDebugger::registerMemoryLeakCallback(std::function<void(const MemoryLeak&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryLeakCallbacks_.push_back(callback);
}

void BrainStateDebugger::registerPerformanceAnomalyCallback(std::function<void(const PerformanceAnomaly&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    performanceAnomalyCallbacks_.push_back(callback);
}

float BrainStateDebugger::computeStateDifference(const BrainSnapshot& snapshot1, const BrainSnapshot& snapshot2) const {
    float totalDiff = 0.0f;
    
    // Compute neuron state difference
    size_t minNeurons = std::min(snapshot1.neuronMembranePotentials.size(), 
                                 snapshot2.neuronMembranePotentials.size());
    for (size_t i = 0; i < minNeurons; ++i) {
        totalDiff += std::abs(snapshot2.neuronMembranePotentials[i] - 
                             snapshot1.neuronMembranePotentials[i]);
    }
    
    // Compute synaptic weight difference
    size_t minSynapses = std::min(snapshot1.synapticWeights.size(), 
                                  snapshot2.synapticWeights.size());
    for (size_t i = 0; i < minSynapses; ++i) {
        totalDiff += std::abs(snapshot2.synapticWeights[i] - 
                             snapshot1.synapticWeights[i]);
    }
    
    // Compute memory state difference
    if (snapshot1.memoryStates.size() == snapshot2.memoryStates.size()) {
        for (size_t i = 0; i < snapshot1.memoryStates.size(); ++i) {
            totalDiff += static_cast<float>(std::abs(snapshot2.memoryStates[i] - 
                                                     snapshot1.memoryStates[i]));
        }
    }
    
    return totalDiff;
}

float BrainStateDebugger::computeNeuronStateDifference(const BrainSnapshot& snapshot1, 
                                                     const BrainSnapshot& snapshot2) const {
    if (snapshot1.neuronMembranePotentials.size() != snapshot2.neuronMembranePotentials.size()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < snapshot1.neuronMembranePotentials.size(); ++i) {
        totalDiff += std::abs(snapshot2.neuronMembranePotentials[i] - 
                             snapshot1.neuronMembranePotentials[i]);
    }
    
    return totalDiff;
}

float BrainStateDebugger::computeSynapticStateDifference(const BrainSnapshot& snapshot1, 
                                                       const BrainSnapshot& snapshot2) const {
    if (snapshot1.synapticWeights.size() != snapshot2.synapticWeights.size()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < snapshot1.synapticWeights.size(); ++i) {
        totalDiff += std::abs(snapshot2.synapticWeights[i] - 
                             snapshot1.synapticWeights[i]);
    }
    
    return totalDiff;
}

float BrainStateDebugger::computeMemoryStateDifference(const BrainSnapshot& snapshot1, 
                                                     const BrainSnapshot& snapshot2) const {
    if (snapshot1.memoryStates.size() != snapshot2.memoryStates.size()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < snapshot1.memoryStates.size(); ++i) {
        totalDiff += static_cast<float>(std::abs(snapshot2.memoryStates[i] - 
                                                 snapshot1.memoryStates[i]));
    }
    
    return totalDiff;
}

float BrainStateDebugger::computeActivityStateDifference(const BrainSnapshot& snapshot1, 
                                                       const BrainSnapshot& snapshot2) const {
    if (snapshot1.neuronFiringRates.size() != snapshot2.neuronFiringRates.size()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < snapshot1.neuronFiringRates.size(); ++i) {
        totalDiff += std::abs(snapshot2.neuronFiringRates[i] - 
                             snapshot1.neuronFiringRates[i]);
    }
    
    return totalDiff;
}

void BrainStateDebugger::findStateTrajectories(std::vector<std::vector<size_t>>& trajectories) const {
    // Find trajectories in the snapshot history
    trajectories.clear();
    
    if (snapshots_.size() < 2) {
        return;
    }
    
    std::vector<size_t> currentTrajectory;
    currentTrajectory.push_back(0);
    
    for (size_t i = 1; i < snapshots_.size(); ++i) {
        float similarity = computeStateDifference(snapshots_[currentTrajectory.back()], 
                                                 snapshots_[i]);
        
        if (similarity < 0.3f) { // Threshold for trajectory continuity
            currentTrajectory.push_back(i);
        } else {
            if (currentTrajectory.size() >= 2) {
                trajectories.push_back(currentTrajectory);
            }
            currentTrajectory.clear();
            currentTrajectory.push_back(i);
        }
    }
    
    if (currentTrajectory.size() >= 2) {
        trajectories.push_back(currentTrajectory);
    }
}

void BrainStateDebugger::extractTrajectoryFeatures(const std::vector<BrainSnapshot>& trajectory,
                                                  std::vector<float>& features) const {
    // Extract features from a trajectory
    features.clear();
    
    if (trajectory.size() < 2) {
        return;
    }
    
    // Compute trajectory statistics
    float totalSteps = static_cast<float>(trajectory.size());
    float avgNeurons = 0.0f;
    float avgSynapses = 0.0f;
    
    for (const auto& snapshot : trajectory) {
        avgNeurons += static_cast<float>(snapshot.neuronMembranePotentials.size()) / totalSteps;
        avgSynapses += static_cast<float>(snapshot.synapticWeights.size()) / totalSteps;
    }
    
    features.push_back(avgNeurons);
    features.push_back(avgSynapses);
    
    // Compute state variance
    std::vector<float> neuronVariances;
    for (size_t i = 0; i < trajectory[0].neuronMembranePotentials.size() && 
                      i < trajectory.back().neuronMembranePotentials.size(); ++i) {
        float mean = (trajectory[0].neuronMembranePotentials[i] + 
                     trajectory.back().neuronMembranePotentials[i]) / 2.0f;
        float variance = 0.0f;
        
        for (const auto& snapshot : trajectory) {
            if (i < snapshot.neuronMembranePotentials.size()) {
                float deviation = snapshot.neuronMembranePotentials[i] - mean;
                variance += deviation * deviation;
            }
        }
        
        variance /= static_cast<float>(trajectory.size());
        neuronVariances.push_back(variance);
    }
    
    float avgVariance = computeMean(neuronVariances);
    features.push_back(avgVariance);
    
    // Compute state change rate
    float totalStateChange = 0.0f;
    for (size_t i = 1; i < trajectory.size(); ++i) {
        totalStateChange += computeStateDifference(trajectory[i-1], trajectory[i]);
    }
    
    features.push_back(totalStateChange / (totalSteps - 1.0f));
}

float BrainStateDebugger::computeTrajectorySimilarity(const std::vector<BrainSnapshot>& trajectory1,
                                                     const std::vector<BrainSnapshot>& trajectory2) const {
    if (trajectory1.size() != trajectory2.size()) {
        return 0.0f; // Cannot compare trajectories of different lengths
    }
    
    float totalSimilarity = 0.0f;
    
    for (size_t i = 0; i < trajectory1.size(); ++i) {
        float similarity = 1.0f - computeStateDifference(trajectory1[i], trajectory2[i]);
        totalSimilarity += similarity;
    }
    
    return totalSimilarity / static_cast<float>(trajectory1.size());
}

std::vector<BrainSnapshot> BrainStateDebugger::getSnapshotsFromIndices(const std::vector<size_t>& indices) const {
    std::vector<BrainSnapshot> trajectory;
    
    for (size_t idx : indices) {
        if (idx < snapshots_.size()) {
            trajectory.push_back(snapshots_[idx]);
        }
    }
    
    return trajectory;
}

float BrainStateDebugger::getFileExtension(const std::string& filename) const {
    // Extract file extension
    size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos) {
        return static_cast<float>(ExportFormat::Text); // Default
    }
    
    std::string ext = filename.substr(dotPos + 1);
    
    if (ext == "json") return static_cast<float>(ExportFormat::JSON);
    if (ext == "bin") return static_cast<float>(ExportFormat::Binary);
    if (ext == "txt") return static_cast<float>(ExportFormat::Text);
    
    return static_cast<float>(ExportFormat::Text); // Default
}

void BrainStateDebugger::trackMemoryUsage(const BrainSnapshot& snapshot) {
    // Track memory usage for leak detection
    // This is a placeholder - real implementation would track actual memory usage
}

void BrainStateDebugger::analyzeMemoryAllocationPatterns() const {
    // Analyze memory allocation patterns
    // This is a placeholder - real implementation would analyze allocation patterns
}

void BrainStateDebugger::detectLeakSources(std::vector<MemoryLeak>& leaks) const {
    // Detect memory leak sources
    // This is a placeholder - real implementation would detect leak sources
}

void BrainStateDebugger::analyzePerformanceMetrics(const PerformanceMetrics& metrics) {
    // Analyze performance metrics
    // This is a placeholder - real implementation would analyze performance
}

void BrainStateDebugger::detectPerformanceTrends() const {
    // Detect performance trends
    // This is a placeholder - real implementation would detect performance trends
}

void BrainStateDebugger::classifyPerformanceAnomalies(std::vector<PerformanceAnomaly>& anomalies) const {
    // Classify performance anomalies
    // This is a placeholder - real implementation would classify anomalies
}

void BrainStateDebugger::validateConfigParameters(std::vector<ConfigurationIssue>& issues) const {
    // Validate configuration parameters
    // This is a placeholder - real implementation would validate parameters
}

void BrainStateDebugger::validateNeuralParameters(std::vector<ConfigurationIssue>& issues) const {
    // Validate neural parameters
    // This is a placeholder - real implementation would validate neural parameters
}

void BrainStateDebugger::validateMemoryParameters(std::vector<ConfigurationIssue>& issues) const {
    // Validate memory parameters
    // This is a placeholder - real implementation would validate memory parameters
}

void BrainStateDebugger::validatePlasticityParameters(std::vector<ConfigurationIssue>& issues) const {
    // Validate plasticity parameters
    // This is a placeholder - real implementation would validate plasticity parameters
}

void BrainStateDebugger::analyzeDevelopmentalStage(const BrainSnapshot& snapshot, DevelopmentStageReport& report) const {
    // Analyze development stage
    // This is a placeholder - real implementation would analyze development stage
}

void BrainStateDebugger::checkDevelopmentalMilestones(const BrainSnapshot& snapshot, std::vector<std::string>& milestones) const {
    // Check developmental milestones
    // This is a placeholder - real implementation would check milestones
}

void BrainStateDebugger::assessDevelopmentProgress(const BrainSnapshot& snapshot, float& progress) const {
    // Assess development progress
    // This is a placeholder - real implementation would assess progress
}

void BrainStateDebugger::analyzeNeuromodulationSignals(const BrainSnapshot& snapshot, NeuromodulationReport& report) const {
    // Analyze neuromodulation signals
    // This is a placeholder - real implementation would analyze neuromodulation
}

void BrainStateDebugger::assessNeuromodulationBalance(const NeuromodulationReport& report, bool& isBalanced) const {
    // Assess neuromodulation balance
    // This is a placeholder - real implementation would assess balance
}

void BrainStateDebugger::computeModulationEffects(const BrainSnapshot& snapshot, std::map<std::string, float>& effects) const {
    // Compute modulation effects
    // This is a placeholder - real implementation would compute effects
}

} // namespace nlm
