// NLM Phase 6 Performance Monitoring Implementation
// 
// Implementation of the performance monitoring system for Phase 6
// integrated cognitive systems.

#include "PerformanceMonitoring.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Global performance monitor instance
PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

void PerformanceMonitor::setupRuntimeInfo() {
    metrics.runtime.platform = "Linux";
    metrics.runtime.compiler = "GCC";
    metrics.runtime.buildType = "Release";
    metrics.runtime.enabledFeatures = {
        "LIF Neurons",
        "Event-Driven Spike Processing",
        "STDP Plasticity",
        "Cognitive Integration",
        "Working Memory",
        "Episodic Memory",
        "Prediction Systems",
        "Concept Formation",
        "Neural Planning",
        "Neuromodulation"
    };
}

void PerformanceMonitor::configureForPhase6() {
    metrics.enabled = true;
    metrics.verboseLogging = true;
    metrics.samplingInterval = 0.05;  // Sample every 50ms
    metrics.maxHistorySize = 5000;
    metrics.trackLearningProgress = true;
    metrics.trackSystemIntegration = true;
    metrics.trackResourceUtilization = true;
    
    NLM_LOG_INFO("Performance monitoring configured for Phase 6 integration");
    NLM_LOG_INFO("Sampling interval: " + std::to_string(metrics.samplingInterval) + "s");
    NLM_LOG_INFO("Max history size: " + std::to_string(metrics.maxHistorySize));
}

void PerformanceMonitor::setupPhase6Monitoring() {
    // Initialize Phase 6 specific performance metrics
    metrics.cognitive.systemMetrics["working_memory"] = 0.0;
    metrics.cognitive.systemMetrics["episodic_memory"] = 0.0;
    metrics.cognitive.systemMetrics["prediction_system"] = 0.0;
    metrics.cognitive.systemMetrics["concept_formation"] = 0.0;
    metrics.cognitive.systemMetrics["attention"] = 0.0;
    metrics.cognitive.systemMetrics["planner"] = 0.0;
    metrics.cognitive.systemMetrics["neuromodulation"] = 0.0;
    metrics.cognitive.systemMetrics["integration"] = 0.0;
    
    metrics.learning.learningEfficiency = 0.0;
    metrics.learning.retentionEfficiency = 0.0;
    metrics.learning.generalizationScore = 0.0;
    
    metrics.resources.memoryUsage = 0;
    metrics.resources.cpuUsage = 0;
    metrics.resources.networkUsage = 0;
    metrics.resources.energyConsumption = 0.0;
    
    NLM_LOG_INFO("Phase 6 monitoring initialized");
}

void PerformanceMonitor::recordPerformanceSnapshot(const PerformanceSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    metrics.history.push_back(snapshot);
    
    if (metrics.history.size() > metrics.maxHistorySize) {
        metrics.history.erase(metrics.history.begin());
    }
    
    if (metrics.verboseLogging) {
        NLM_LOG_DEBUG("Performance snapshot recorded: Integration score = " + 
                     std::to_string(snapshot.integrationScore));
    }
}

PerformanceSnapshot PerformanceMonitor::getCurrentSnapshot() {
    PerformanceSnapshot snapshot;
    
    auto currentTime = std::chrono::steady_clock::now();
    snapshot.timestamp = std::chrono::duration<double>(currentTime - metrics.startTime).count();
    
    // Copy current metrics
    snapshot.brain.totalNeurons = metrics.stepPerformance.totalSteps;  // Placeholder
    snapshot.brain.totalSynapses = metrics.stepPerformance.totalSteps;  // Placeholder
    snapshot.brain.activeNeurons = metrics.stepPerformance.totalSteps;  // Placeholder
    snapshot.brain.firingNeurons = metrics.stepPerformance.totalSteps;  // Placeholder
    snapshot.brain.avgFiringRate = metrics.stepPerformance.avgStepTime;  // Placeholder
    snapshot.brain.eToIRatio = 1.0f;  // Placeholder
    
    // Update cognitive system metrics
    for (const auto& pair : metrics.cognitive.systemMetrics) {
        if (pair.first == "working_memory") {
            snapshot.cognitive.workingMemoryUsage = static_cast<size_t>(pair.second * 100);
        } else if (pair.first == "episodic_memory") {
            snapshot.cognitive.episodicMemoryUsage = static_cast<size_t>(pair.second * 1000);
        } else if (pair.first == "prediction_system") {
            snapshot.cognitive.predictionAccuracy = pair.second;
        } else if (pair.first == "concept_formation") {
            snapshot.cognitive.conceptStability = pair.second;
        } else if (pair.first == "attention") {
            snapshot.cognitive.attentionFocus = pair.second;
        } else if (pair.first == "planner") {
            snapshot.cognitive.planningConfidence = pair.second;
        }
    }
    
    // Update neuromodulation
    // (Would need actual values from brain state)
    snapshot.neuromodulation.dopamineLevel = 0.5f;  // Placeholder
    snapshot.neuromodulation.curiosityLevel = 0.3f;   // Placeholder
    snapshot.neuromodulation.noveltyLevel = 0.4f;     // Placeholder
    
    // Update learning metrics
    snapshot.learning.learningRate = metrics.learning.learningEfficiency;
    snapshot.learning.retentionRate = metrics.learning.retentionEfficiency;
    snapshot.learning.generalizationScore = metrics.learning.generalizationScore;
    
    // Update integration metrics
    snapshot.integrationScore = metrics.integration.integrationScore;
    snapshot.integrationStatus = metrics.integration.integrationStatus;
    
    return snapshot;
}

void PerformanceMonitor::recordStepPerformance(double stepTime, bool success) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    metrics.stepPerformance.totalSteps++;
    if (success) {
        metrics.stepPerformance.successfulSteps++;
    } else {
        metrics.stepPerformance.failedSteps++;
    }
    
    // Update average step time
    if (metrics.stepPerformance.totalSteps == 1) {
        metrics.stepPerformance.avgStepTime = stepTime;
        metrics.stepPerformance.minStepTime = stepTime;
        metrics.stepPerformance.maxStepTime = stepTime;
    } else {
        double oldAvg = metrics.stepPerformance.avgStepTime;
        metrics.stepPerformance.avgStepTime = (oldAvg * (metrics.stepPerformance.totalSteps - 1) + stepTime) / 
                                             metrics.stepPerformance.totalSteps;
        metrics.stepPerformance.minStepTime = std::min(metrics.stepPerformance.minStepTime, stepTime);
        metrics.stepPerformance.maxStepTime = std::max(metrics.stepPerformance.maxStepTime, stepTime);
    }
    
    if (metrics.verboseLogging) {
        NLM_LOG_DEBUG("Step performance: time=" + std::to_string(stepTime) + 
                     "s, avg=" + std::to_string(metrics.stepPerformance.avgStepTime) + "s");
    }
}

void PerformanceMonitor::updateBrainMetrics(size_t neurons, size_t synapses, size_t activeNeurons) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    // Update brain metrics with actual values from brain
    metrics.cognitive.systemMetrics["total_neurons"] = neurons;
    metrics.cognitive.systemMetrics["total_synapses"] = synapses;
    metrics.cognitive.systemMetrics["active_neurons"] = activeNeurons;
}

void PerformanceMonitor::recordCognitiveMetric(const std::string& system, double value) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    metrics.cognitive.systemMetrics[system] = value;
    metrics.cognitive.systemHistory[system].push_back(value);
    
    if (metrics.cognitive.systemHistory[system].size() > 100) {
        metrics.cognitive.systemHistory[system].erase(
            metrics.cognitive.systemHistory[system].begin());
    }
}

void PerformanceMonitor::recordCognitiveHistory(const std::string& system, const std::vector<double>& history) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    metrics.cognitive.systemHistory[system] = history;
}

void PerformanceMonitor::recordLearningProgress(double efficiency, double retention, double generalization) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    metrics.learning.learningEfficiency = efficiency;
    metrics.learning.retentionEfficiency = retention;
    metrics.learning.generalizationScore = generalization;
    
    metrics.learning.learningProgress.push_back((efficiency + retention + generalization) / 3.0);
    
    if (metrics.learning.learningProgress.size() > 100) {
        metrics.learning.learningProgress.erase(
            metrics.learning.learningProgress.begin());
    }
}

void PerformanceMonitor::updateLearningHistory(double step, double reward, double error) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    // Learning history is recorded via snapshots
    PerformanceSnapshot snapshot;
    snapshot.learning.rewardHistory.push_back(reward);
    snapshot.learning.predictionErrorHistory.push_back(error);
    
    recordPerformanceSnapshot(snapshot);
}

void PerformanceMonitor::updateIntegrationScore(double score, const std::string& status) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    metrics.integration.integrationScore = score;
    metrics.integration.integrationStatus = status;
    
    if (metrics.verboseLogging) {
        NLM_LOG_DEBUG("Integration score updated: " + std::to_string(score) + ", status: " + status);
    }
}

void PerformanceMonitor::addIntegrationIssue(const std::string& issue) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    metrics.integration.integrationIssues.push_back(issue);
    
    NLM_LOG_WARNING("Integration issue detected: " + issue);
}

void PerformanceMonitor::updateResourceUsage(size_t memory, size_t cpu, size_t network, double energy) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    metrics.resources.memoryUsage = memory;
    metrics.resources.cpuUsage = cpu;
    metrics.resources.networkUsage = network;
    metrics.resources.energyConsumption = energy;
}

double PerformanceMonitor::calculateIntegrationEfficiency() const {
    double efficiency = metrics.integration.integrationScore;
    
    // Factor in step success rate
    double successRate = metrics.stepPerformance.totalSteps > 0 ?
        static_cast<double>(metrics.stepPerformance.successfulSteps) / metrics.stepPerformance.totalSteps : 0.0;
    
    // Factor in learning efficiency
    double learningEff = metrics.learning.learningEfficiency;
    
    // Combined efficiency score
    return (efficiency * 0.5 + successRate * 0.3 + learningEff * 0.2);
}

double PerformanceMonitor::calculateLearningEfficiency() const {
    if (metrics.learning.learningProgress.empty()) {
        return 0.0;
    }
    
    // Calculate learning efficiency based on progress
    double avgProgress = std::accumulate(metrics.learning.learningProgress.begin(),
                                        metrics.learning.learningProgress.end(), 0.0) /
                       metrics.learning.learningProgress.size();
    
    return avgProgress;
}

double PerformanceMonitor::calculateSystemEfficiency() const {
    double totalSystemScore = 0.0;
    size_t systemCount = 0;
    
    for (const auto& pair : metrics.cognitive.systemMetrics) {
        totalSystemScore += pair.second;
        systemCount++;
    }
    
    return systemCount > 0 ? totalSystemScore / systemCount : 0.0;
}

std::string PerformanceMonitor::generatePerformanceReport() const {
    std::ostringstream report;
    
    report << "=== NLM Phase 6 Performance Report ===\n";
    report << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    report << "System Performance:" << std::endl;
    report << "  Total Steps: " << metrics.stepPerformance.totalSteps << std::endl;
    report << "  Successful Steps: " << metrics.stepPerformance.successfulSteps << std::endl;
    report << "  Failed Steps: " << metrics.stepPerformance.failedSteps << std::endl;
    report << "  Success Rate: " << std::fixed << std::setprecision(2)
           << (metrics.stepPerformance.totalSteps > 0 ?
               static_cast<double>(metrics.stepPerformance.successfulSteps) / metrics.stepPerformance.totalSteps * 100 : 0.0)
           << "%" << std::endl;
    report << "  Average Step Time: " << std::setprecision(4) << metrics.stepPerformance.avgStepTime * 1000 << " ms" << std::endl;
    report << "  Min Step Time: " << std::setprecision(4) << metrics.stepPerformance.minStepTime * 1000 << " ms" << std::endl;
    report << "  Max Step Time: " << std::setprecision(4) << metrics.stepPerformance.maxStepTime * 1000 << " ms" << std::endl;
    report << "  Steps per Second: " << std::setprecision(2) << 
           (metrics.stepPerformance.totalSteps > 0 && metrics.stepPerformance.maxStepTime > 0 ?
            metrics.stepPerformance.totalSteps / (metrics.stepPerformance.maxStepTime * metrics.stepPerformance.totalSteps) : 0.0)
           << std::endl;
    
    report << "\nCognitive System Performance:" << std::endl;
    for (const auto& pair : metrics.cognitive.systemMetrics) {
        report << "  " << pair.first << ": " << std::setprecision(3) << pair.second << std::endl;
    }
    
    report << "\nLearning Performance:" << std::endl;
    report << "  Learning Efficiency: " << std::setprecision(4) << metrics.learning.learningEfficiency << std::endl;
    report << "  Retention Efficiency: " << std::setprecision(4) << metrics.learning.retentionEfficiency << std::endl;
    report << "  Generalization Score: " << std::setprecision(4) << metrics.learning.generalizationScore << std::endl;
    
    report << "\nIntegration Status:" << std::endl;
    report << "  Integration Score: " << std::setprecision(1) << metrics.integration.integrationScore << "/100" << std::endl;
    report << "  Status: " << metrics.integration.integrationStatus << std::endl;
    report << "  Issues: " << metrics.integration.integrationIssues.size() << std::endl;
    
    report << "\nResource Utilization:" << std::endl;
    report << "  Memory Usage: " << formatPerformanceValue(metrics.resources.memoryUsage, "MB") << std::endl;
    report << "  CPU Usage: " << formatPerformanceValue(metrics.resources.cpuUsage, "%") << std::endl;
    report << "  Network Usage: " << formatPerformanceValue(metrics.resources.networkUsage, "MB/s") << std::endl;
    report << "  Energy Consumption: " << std::setprecision(4) << metrics.resources.energyConsumption << " J" << std::endl;
    
    return report.str();
}

std::string PerformanceMonitor::generateIntegrationReport() const {
    std::ostringstream report;
    
    report << "=== NLM Phase 6 Integration Report ===\n";
    report << "Integration Assessment:" << std::endl;
    report << "  Overall Integration Score: " << std::setprecision(1) << metrics.integration.integrationScore << "/100" << std::endl;
    report << "  Integration Status: " << metrics.integration.integrationStatus << std::endl;
    
    if (!metrics.integration.integrationIssues.empty()) {
        report << "  Integration Issues:" << std::endl;
        for (const auto& issue : metrics.integration.integrationIssues) {
            report << "    - " << issue << std::endl;
        }
    } else {
        report << "  Integration Issues: None" << std::endl;
    }
    
    report << "\nCognitive System Integration:" << std::endl;
    for (const auto& pair : metrics.cognitive.systemMetrics) {
        std::string status = (pair.second > 0.7) ? "GOOD" : (pair.second > 0.4) ? "FAIR" : "NEEDS_IMPROVEMENT";
        report << "  " << pair.first << ": " << std::setprecision(3) << pair.second << " (" << status << ")" << std::endl;
    }
    
    return report.str();
}

std::string PerformanceMonitor::generateLearningReport() const {
    std::ostringstream report;
    
    report << "=== NLM Phase 6 Learning Report ===\n";
    report << "Learning Progress:" << std::endl;
    report << "  Learning Efficiency: " << std::setprecision(4) << metrics.learning.learningEfficiency << std::endl;
    report << "  Retention Efficiency: " << std::setprecision(4) << metrics.learning.retentionEfficiency << std::endl;
    report << "  Generalization Score: " << std::setprecision(4) << metrics.learning.generalizationScore << std::endl;
    
    if (!metrics.learning.learningProgress.empty()) {
        double avgProgress = calculateAverage(metrics.learning.learningProgress);
        double stdDev = calculateStdDev(metrics.learning.learningProgress);
        report << "  Average Learning Progress: " << std::setprecision(4) << avgProgress << std::endl;
        report << "  Learning Progress Std Dev: " << std::setprecision(4) << stdDev << std::endl;
    }
    
    return report.str();
}

std::string PerformanceMonitor::generateResourceReport() const {
    std::ostringstream report;
    
    report << "=== NLM Phase 6 Resource Report ===\n";
    report << "Resource Utilization:" << std::endl;
    report << "  Memory Usage: " << formatPerformanceValue(metrics.resources.memoryUsage, "MB") << std::endl;
    report << "  CPU Usage: " << formatPerformanceValue(metrics.resources.cpuUsage, "%") << std::endl;
    report << "  Network Usage: " << formatPerformanceValue(metrics.resources.networkUsage, "MB/s") << std::endl;
    report << "  Energy Consumption: " << std::setprecision(4) << metrics.resources.energyConsumption << " J" << std::endl;
    
    return report.str();
}

bool PerformanceMonitor::validatePerformanceThresholds() const {
    bool valid = true;
    
    // Check integration score threshold
    if (metrics.integration.integrationScore < 60.0) {
        valid = false;
    }
    
    // Check step success rate threshold
    double successRate = metrics.stepPerformance.totalSteps > 0 ?
        static_cast<double>(metrics.stepPerformance.successfulSteps) / metrics.stepPerformance.totalSteps : 0.0;
    
    if (successRate < 0.8) {
        valid = false;
    }
    
    // Check learning efficiency threshold
    if (metrics.learning.learningEfficiency < 0.3) {
        valid = false;
    }
    
    return valid;
}

std::vector<std::string> PerformanceMonitor::getPerformanceIssues() const {
    std::vector<std::string> issues;
    
    if (metrics.integration.integrationScore < 60.0) {
        issues.push_back("Integration score below threshold (60.0)");
    }
    
    if (metrics.stepPerformance.totalSteps > 0) {
        double successRate = static_cast<double>(metrics.stepPerformance.successfulSteps) / metrics.stepPerformance.totalSteps;
        if (successRate < 0.8) {
            issues.push_back("Step success rate below threshold (80.0%)");
        }
    }
    
    if (metrics.learning.learningEfficiency < 0.3) {
        issues.push_back("Learning efficiency below threshold (0.3)");
    }
    
    return issues;
}

bool PerformanceMonitor::exportPerformanceData(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generatePerformanceReport();
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export performance data: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceMonitor::importPerformanceData(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // In a full implementation, you would parse the exported file
        // and restore the performance metrics
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import performance data: " + std::string(e.what()));
        return false;
    }
}

double PerformanceMonitor::calculateAverage(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceMonitor::calculateStdDev(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    double avg = calculateAverage(values);
    double sumSquared = 0.0;
    
    for (double val : values) {
        double diff = val - avg;
        sumSquared += diff * diff;
    }
    
    return std::sqrt(sumSquared / (values.size() - 1));
}

std::string PerformanceMonitor::formatPerformanceValue(double value, const std::string& unit) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value << " " << unit;
    return oss.str();
}

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%d %H:%M:%S UTC");
    return ss.str();
}

} // namespace nlm
