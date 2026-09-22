#include "ExperimentMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

ExperimentMonitor::ExperimentMonitor(std::shared_ptr<Brain> brain, std::shared_ptr<LearningExperiment> experiment)
    : brain_(brain)
    , experiment_(experiment)
    , monitoring_(false)
    , monitoringStep_(0)
    , metricCollectionInterval_(100) // Collect metrics every 100 steps
    , stabilityWindow_(50)
    , convergenceThreshold_(0.01f)
    , maxHistorySize_(10000)
    , previousLearningRate_(0.0f)
    , previousStabilityScore_(0.0f)
    , previousExplorationScore_(0.0f)
    , lastConvergenceCheck_(0)
    , totalCPUTime_(0.0)
    , lastPerformanceMeasure_(std::chrono::steady_clock::now()) {
}

void ExperimentMonitor::startMonitoring() {
    if (monitoring_) {
        return;
    }
    
    monitoring_ = true;
    monitoringStep_ = 0;
    metricHistory_.clear();
    startTime_ = std::chrono::steady_clock::now();
    lastMetricCollection_ = startTime_;
    lastConvergenceCheck_ = 0;
    previousLearningRate_ = 0.0f;
    previousStabilityScore_ = 0.0f;
    previousExplorationScore_ = 0.0f;
    metricBuffers_.clear();
    
    NLM_LOG_INFO("Experiment monitoring started");
}

void ExperimentMonitor::stopMonitoring() {
    if (!monitoring_) {
        return;
    }
    
    monitoring_ = false;
    calculatePerformanceStats(); // Finalize performance stats
    
    NLM_LOG_INFO("Experiment monitoring stopped");
    NLM_LOG_INFO("Collected " + std::to_string(metricHistory_.size()) + " metric samples");
    NLM_LOG_INFO("Performance: " + std::to_string(performanceStats_.stepsPerSecond) + " steps/sec");
}

ExperimentMonitor::ExperimentMetrics ExperimentMonitor::collectMetrics(SimulationStep step, Timestamp timestamp) {
    ExperimentMetrics metrics;
    metrics.step = step;
    metrics.timestamp = timestamp;
    metrics.elapsedTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime_).count();
    
    if (!brain_ || !experiment_) {
        return metrics;
    }
    
    // Basic brain state metrics
    metrics.activeNeuronCount = brain_->getActiveNeuronCount();
    metrics.firingNeuronCount = brain_->getFiringNeuronCount();
    metrics.spikeRate = static_cast<float>(brain_->getTotalSpikeCount()) / std::max<double>(1.0, metrics.elapsedTime);
    
    // Memory system metrics
    auto* workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        metrics.memoryUtilization = workingMem->getUtilization();
    }
    
    // Learning metrics (simplified)
    metrics.learningRate = calculateLearningRate(step);
    
    // Stability calculation
    metrics.stabilityScore = calculateStabilityScore(step);
    
    // Exploration score based on novelty and curiosity
    metrics.explorationScore = calculateExplorationScore(step);
    
    // Prediction error from prediction system
    auto* predSystem = brain_->getPredictionSystem();
    if (predSystem) {
        metrics.predictionError = predSystem->getPredictionError();
    }
    
    // Reward signal from neuromodulation
    auto* rewardMod = brain_->getDopamine();
    if (rewardMod) {
        metrics.rewardSignal = rewardMod->getSignal();
    }
    
    // Synaptic activity (simplified)
    metrics.synapticActivity = static_cast<float>(brain_->getTotalSynapseCount()) / 1000.0f; // Normalize
    
    // Energy consumption (simplified)
    metrics.energyConsumption = metrics.spikeRate * 0.1f; // Estimate
    
    // Weight deltas (sample)
    metrics.weightDeltas = std::vector<float>(10, 0.0f); // Simplified - would need to track actual changes
    
    return metrics;
}

void ExperimentMonitor::processMetrics(const ExperimentMetrics& metrics) {
    // Store metrics
    metricHistory_.push_back(metrics);
    
    // Limit history size
    if (metricHistory_.size() > maxHistorySize_) {
        metricHistory_.erase(metricHistory_.begin(),
                           metricHistory_.begin() + (metricHistory_.size() - maxHistorySize_));
    }
    
    // Buffer metrics for statistical analysis
    metricBuffers_["learningRate"].push_back(metrics.learningRate);
    metricBuffers_["stabilityScore"].push_back(metrics.stabilityScore);
    metricBuffers_["explorationScore"].push_back(metrics.explorationScore);
    
    // Invoke callback
    if (metricCallback_) {
        metricCallback_(metrics);
    }
    
    // Check for convergence
    if (metrics.step > lastConvergenceCheck_ && metrics.step % metricCollectionInterval_ == 0) {
        ConvergenceInfo convInfo = detectConvergence(metricHistory_);
        if (convInfo.converged) {
            convergenceInfo_ = convInfo;
            if (convergenceCallback_) {
                convergenceCallback_(convInfo);
            }
        }
    }
}

ExperimentMonitor::ConvergenceInfo ExperimentMonitor::detectConvergence(const std::vector<ExperimentMetrics>& metrics) const {
    ConvergenceInfo info;
    info.converged = false;
    
    if (metrics.size() < stabilityWindow_) {
        info.reason = "Insufficient data for convergence analysis";
        return info;
    }
    
    // Extract learning rate time series
    std::vector<float> learningRates;
    for (const auto& m : metrics) {
        learningRates.push_back(m.learningRate);
    }
    
    // Calculate final value and compare to threshold
    float finalValue = learningRates.back();
    info.finalValue = finalValue;
    
    // Check if learning has stabilized
    size_t recentWindow = std::min<size_t>(stabilityWindow_, learningRates.size());
    std::vector<float> recentLearningRates(
        learningRates.end() - recentWindow,
        learningRates.end()
    );
    
    float meanRecent = std::accumulate(recentLearningRates.begin(), recentLearningRates.end(), 0.0f) / recentLearningRates.size();
    float variance = 0.0f;
    
    for (float rate : recentLearningRates) {
        float diff = rate - meanRecent;
        variance += diff * diff;
    }
    
    variance /= recentLearningRates.size();
    float stdDev = std::sqrt(variance);
    
    // Convergence criteria: low variance and learning rate below threshold
    bool lowVariance = stdDev < convergenceThreshold_;
    bool smallLearningRate = std::abs(meanRecent - previousLearningRate_) < convergenceThreshold_;
    
    if (lowVariance && smallLearningRate && std::abs(finalValue) < 0.1f) {
        info.converged = true;
        info.convergenceStep = metrics.back().step;
        info.convergenceThreshold = convergenceThreshold_;
        info.reason = "Learning stabilized with low variance and small learning rate";
    } else {
        info.reason = "Learning not yet converged";
    }
    
    return info;
}

bool ExperimentMonitor::checkConvergence(const ExperimentMetrics& current, const ExperimentMetrics& previous) const {
    float learningRateChange = std::abs(current.learningRate - previous.learningRate);
    float stabilityChange = std::abs(current.stabilityScore - previous.stabilityScore);
    float explorationChange = std::abs(current.explorationScore - previous.explorationScore);
    
    // Convergence if all changes are below threshold
    return learningRateChange < convergenceThreshold_ &&
           stabilityChange < convergenceThreshold_ &&
           explorationChange < convergenceThreshold_;
}

float ExperimentMonitor::calculateProgress(SimulationStep step, SimulationStep maxSteps) const {
    if (maxSteps == 0) return 0.0f;
    return static_cast<float>(step) / static_cast<float>(maxSteps);
}

std::string ExperimentMonitor::getProgressStatus() const {
    if (metricHistory_.empty()) {
        return "Not started";
    }
    
    const auto& last = metricHistory_.back();
    std::stringstream ss;
    ss << "Step " << last.step;
    ss << ", Learning Rate: " << std::fixed << std::setprecision(4) << last.learningRate;
    ss << ", Stability: " << last.stabilityScore;
    ss << ", Exploration: " << last.explorationScore;
    ss << ", Time: " << std::fixed << std::setprecision(2) << last.elapsedTime << "s";
    
    return ss.str();
}

ExperimentMonitor::PerformanceStats ExperimentMonitor::calculatePerformanceStats() const {
    PerformanceStats stats;
    
    auto now = std::chrono::steady_clock::now();
    stats.totalTime = std::chrono::duration<double>(now - startTime_).count();
    
    if (stats.totalTime > 0.0) {
        stats.stepsPerSecond = static_cast<double>(metricHistory_.size()) / stats.totalTime;
        stats.operationsPerSecond = stats.stepsPerSecond * 1000.0; // Estimate
    }
    
    // Estimate memory usage (simplified)
    size_t metricMemory = metricHistory_.size() * sizeof(ExperimentMetrics);
    stats.memoryUsage = static_cast<double>(metricMemory) / (1024 * 1024); // MB
    
    stats.cpuTime = totalCPUTime_;
    
    return stats;
}

void ExperimentMonitor::resetPerformanceStats() {
    startTime_ = std::chrono::steady_clock::now();
    lastPerformanceMeasure_ = startTime_;
    totalCPUTime_ = 0.0;
    performanceStats_ = PerformanceStats();
}

float ExperimentMonitor::calculateLearningRate(SimulationStep step) const {
    if (step < 10 || metricHistory_.size() < 10) {
        return 0.1f; // Default learning rate
    }
    
    // Look at weight changes over recent steps
    float totalWeightChange = 0.0f;
    int weightChangeCount = 0;
    
    for (size_t i = metricHistory_.size() - 10; i < metricHistory_.size(); ++i) {
        totalWeightChange += std::abs(metricHistory_[i].learningRate);
        weightChangeCount++;
    }
    
    return (weightChangeCount > 0) ? totalWeightChange / weightChangeCount : 0.0f;
}

float ExperimentMonitor::calculateStabilityScore(SimulationStep step) const {
    if (metricHistory_.size() < stabilityWindow_) {
        return 0.5f; // Neutral stability
    }
    
    // Calculate variance of stability over window
    std::vector<float> recentStabilities;
    size_t start = std::max<size_t>(0, metricHistory_.size() - stabilityWindow_);
    
    for (size_t i = start; i < metricHistory_.size(); ++i) {
        recentStabilities.push_back(metricHistory_[i].stabilityScore);
    }
    
    if (recentStabilities.empty()) {
        return 0.5f;
    }
    
    float mean = std::accumulate(recentStabilities.begin(), recentStabilities.end(), 0.0f) / recentStabilities.size();
    
    float variance = 0.0f;
    for (float stability : recentStabilities) {
        float diff = stability - mean;
        variance += diff * diff;
    }
    
    variance /= recentStabilities.size();
    
    // Lower variance = higher stability score (inverted)
    return 1.0f - std::min(1.0f, std::sqrt(variance));
}

float ExperimentMonitor::calculateExplorationScore(SimulationStep step) const {
    if (metricHistory_.size() < stabilityWindow_) {
        return 0.5f; // Neutral exploration
    }
    
    // Base exploration on novelty and curiosity signals
    float noveltyScore = 0.5f;
    float curiosityScore = 0.5f;
    
    // Simplified calculation - would integrate with actual neuromodulation
    return (noveltyScore + curiosityScore) / 2.0f;
}

float ExperimentMonitor::calculateLearningCurve(SimulationStep startStep, SimulationStep endStep) const {
    if (metricHistory_.empty()) return 0.0f;
    
    std::vector<float> learningRates;
    for (const auto& metrics : metricHistory_) {
        if (metrics.step >= startStep && metrics.step <= endStep) {
            learningRates.push_back(metrics.learningRate);
        }
    }
    
    if (learningRates.empty()) return 0.0f;
    
    // Fit exponential decay curve
    float initial = learningRates.front();
    float final = learningRates.back();
    
    if (initial <= 0.0f) return 0.0f;
    
    return final / initial; // Learning retention factor
}

float ExperimentMonitor::calculateStability(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    float variance = 0.0f;
    
    for (float value : values) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    variance /= values.size();
    
    return std::sqrt(variance);
}

float ExperimentMonitor::calculateExploration(const std::vector<ExperimentMetrics>& metrics) const {
    if (metrics.empty()) return 0.0f;
    
    std::vector<float> noveltyScores;
    for (const auto& m : metrics) {
        // Exploration combines novelty and curiosity
        noveltyScores.push_back(m.explorationScore);
    }
    
    float mean = std::accumulate(noveltyScores.begin(), noveltyScores.end(), 0.0f) / noveltyScores.size();
    return mean;
}

std::string ExperimentMonitor::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Experiment Report ===" << std::endl;
    ss << std::endl;
    
    if (metricHistory_.empty()) {
        ss << "No data collected" << std::endl;
        return ss.str();
    }
    
    // Basic statistics
    ss << "=== Summary ===" << std::endl;
    ss << "Total steps: " << metricHistory_.back().step << std::endl;
    ss << "Total time: " << std::fixed << std::setprecision(2) 
       << metricHistory_.back().elapsedTime << " seconds" << std::endl;
    ss << "Samples collected: " << metricHistory_.size() << std::endl;
    ss << std::endl;
    
    // Learning statistics
    ss << "=== Learning Statistics ===" << std::endl;
    ss << "Initial learning rate: " << std::fixed << std::setprecision(4)
       << (metricHistory_.size() > 0 ? metricHistory_.front().learningRate : 0.0f) << std::endl;
    ss << "Final learning rate: " << std::fixed << std::setprecision(4)
       << metricHistory_.back().learningRate << std::endl;
    ss << "Learning retention: " << std::fixed << std::setprecision(4)
       << calculateLearningCurve(0, metricHistory_.back().step) << std::endl;
    ss << std::endl;
    
    // Stability statistics
    ss << "=== Stability Statistics ===" << std::endl;
    ss << "Final stability: " << std::fixed << std::setprecision(4)
       << metricHistory_.back().stabilityScore << std::endl;
    ss << std::endl;
    
    // Exploration statistics
    ss << "=== Exploration Statistics ===" << std::endl;
    ss << "Final exploration: " << std::fixed << std::setprecision(4)
       << metricHistory_.back().explorationScore << std::endl;
    ss << std::endl;
    
    // Performance statistics
    ss << "=== Performance ===" << std::endl;
    ss << "Steps per second: " << std::fixed << std::setprecision(2)
       << performanceStats_.stepsPerSecond << std::endl;
    ss << "Memory usage: " << std::fixed << std::setprecision(2)
       << performanceStats_.memoryUsage << " MB" << std::endl;
    ss << std::endl;
    
    // Convergence
    ss << "=== Convergence ===" << std::endl;
    if (convergenceInfo_.converged) {
        ss << "Converged: YES" << std::endl;
        ss << "Convergence step: " << convergenceInfo_.convergenceStep << std::endl;
        ss << "Reason: " << convergenceInfo_.reason << std::endl;
    } else {
        ss << "Converged: NO" << std::endl;
        ss << "Reason: " << convergenceInfo_.reason << std::endl;
    }
    
    return ss.str();
}

void ExperimentMonitor::exportMetricsToCSV(const std::string& filepath) const {
    if (metricHistory_.empty()) {
        return;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    // Write header
    file << "step,timestamp,learning_rate,stability_score,exploration_score,prediction_error,reward_signal,spike_rate,synaptic_activity,memory_utilization,energy_consumption,active_neurons,firing_neurons,elapsed_time" << std::endl;
    
    // Write data
    for (const auto& metrics : metricHistory_) {
        file << metrics.step << ","
             << metrics.timestamp << ","
             << metrics.learningRate << ","
             << metrics.stabilityScore << ","
             << metrics.explorationScore << ","
             << metrics.predictionError << ","
             << metrics.rewardSignal << ","
             << metrics.spikeRate << ","
             << metrics.synapticActivity << ","
             << metrics.memoryUtilization << ","
             << metrics.energyConsumption << ","
             << metrics.activeNeuronCount << ","
             << metrics.firingNeuronCount << ","
             << metrics.elapsedTime << std::endl;
    }
    
    file.close();
}

void ExperimentMonitor::exportMetricsToJSON(const std::string& filepath) const {
    if (metricHistory_.empty()) {
        return;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    file << "  \"experiment_metrics\": [\n";
    
    for (size_t i = 0; i < metricHistory_.size(); ++i) {
        const auto& metrics = metricHistory_[i];
        
        file << "    {\n";
        file << "      \"step\": " << metrics.step << ",\n";
        file << "      \"timestamp\": " << metrics.timestamp << ",\n";
        file << "      \"learning_rate\": " << metrics.learningRate << ",\n";
        file << "      \"stability_score\": " << metrics.stabilityScore << ",\n";
        file << "      \"exploration_score\": " << metrics.explorationScore << ",\n";
        file << "      \"prediction_error\": " << metrics.predictionError << ",\n";
        file << "      \"reward_signal\": " << metrics.rewardSignal << ",\n";
        file << "      \"spike_rate\": " << metrics.spikeRate << ",\n";
        file << "      \"synaptic_activity\": " << metrics.synapticActivity << ",\n";
        file << "      \"memory_utilization\": " << metrics.memoryUtilization << ",\n";
        file << "      \"energy_consumption\": " << metrics.energyConsumption << ",\n";
        file << "      \"active_neuron_count\": " << metrics.activeNeuronCount << ",\n";
        file << "      \"firing_neuron_count\": " << metrics.firingNeuronCount << ",\n";
        file << "      \"elapsed_time\": " << metrics.elapsedTime << "\n";
        
        if (i < metricHistory_.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ],\n";
    
    // Add summary statistics
    file << "  \"summary_statistics\": {\n";
    
    // Calculate summary statistics
    if (!metricHistory_.empty()) {
        float sumLearningRate = 0.0f;
        float sumStability = 0.0f;
        float sumExploration = 0.0f;
        float sumSpikeRate = 0.0f;
        
        for (const auto& metrics : metricHistory_) {
            sumLearningRate += metrics.learningRate;
            sumStability += metrics.stabilityScore;
            sumExploration += metrics.explorationScore;
            sumSpikeRate += metrics.spikeRate;
        }
        
        size_t count = metricHistory_.size();
        
        file << "    \"avg_learning_rate\": " << (sumLearningRate / count) << ",\n";
        file << "    \"avg_stability_score\": " << (sumStability / count) << ",\n";
        file << "    \"avg_exploration_score\": " << (sumExploration / count) << ",\n";
        file << "    \"avg_spike_rate\": " << (sumSpikeRate / count) << ",\n";
        file << "    \"total_steps\": " << metricHistory_.back().step << ",\n";
        file << "    \"total_time\": " << metricHistory_.back().elapsedTime << ",\n";
        file << "    \"convergence\": " << (convergenceInfo_.converged ? "true" : "false") << "\n";
    }
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
}

void ExperimentMonitor::performStatisticalAnalysis() const {
    // Perform detailed statistical analysis on collected metrics
    if (metricHistory_.size() < 10) {
        return;
    }
    
    // Analyze learning rate stability
    std::vector<float> learningRates;
    for (const auto& metrics : metricHistory_) {
        learningRates.push_back(metrics.learningRate);
    }
    
    StatisticalSummary learningStats = calculateStatistics(learningRates);
    
    // Log key statistics
    NLM_LOG_INFO("Statistical Analysis:");
    NLM_LOG_INFO("  Learning Rate - Mean: " + std::to_string(learningStats.mean) +
                ", Std Dev: " + std::to_string(learningStats.standardDeviation) +
                ", Min: " + std::to_string(learningStats.min) +
                ", Max: " + std::to_string(learningStats.max));
    
    // Analyze learning curve
    float learningCurve = calculateLearningCurve(0, metricHistory_.back().step);
    NLM_LOG_INFO("Learning curve retention factor: " + std::to_string(learningCurve));
    
    // Check for significant patterns
    size_t stableWindow = std::min<size_t>(stabilityWindow_, metricHistory_.size());
    std::vector<float> recentLearningRates;
    if (metricHistory_.size() > stableWindow) {
        recentLearningRates.assign(metricHistory_.end() - stableWindow, metricHistory_.end());
    } else {
        recentLearningRates = metricHistory_;
    }
    
    float recentMean = std::accumulate(recentLearningRates.begin(), recentLearningRates.end(), 0.0f) / recentLearningRates.size();
    NLM_LOG_INFO("Recent learning rate (last " + std::to_string(recentLearningRates.size()) + " steps): " + std::to_string(recentMean));
}

float ExperimentMonitor::calculateLearningStability() const {
    if (metricHistory_.size() < 10) {
        return 0.0f;
    }
    
    std::vector<float> learningRates;
    for (const auto& metrics : metricHistory_) {
        learningRates.push_back(metrics.learningRate);
    }
    
    return calculateStability(learningRates);
}

float ExperimentMonitor::calculateBehavioralFlexibility() const {
    if (metricHistory_.size() < 10) {
        return 0.0f;
    }
    
    // Calculate flexibility as variance in behavior over time
    std::vector<float> explorationScores;
    for (const auto& metrics : metricHistory_) {
        explorationScores.push_back(metrics.explorationScore);
    }
    
    // High variance indicates flexibility (willingness to try different behaviors)
    return calculateStability(explorationScores);
}
}

ExperimentMonitor::StatisticalSummary ExperimentMonitor::calculateStatistics(const std::vector<float>& values) const {
    StatisticalSummary summary;
    
    if (values.empty()) {
        return summary; // All zeros
    }
    
    summary.mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    
    // Calculate variance and standard deviation
    float variance = 0.0f;
    for (float value : values) {
        float diff = value - summary.mean;
        variance += diff * diff;
    }
    variance /= values.size();
    
    summary.variance = variance;
    summary.standardDeviation = std::sqrt(variance);
    
    // Find min and max
    summary.min = *std::min_element(values.begin(), values.end());
    summary.max = *std::max_element(values.begin(), values.end());
    
    // Calculate median
    std::vector<float> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t middle = sorted.size() / 2;
    if (sorted.size() % 2 == 0) {
        summary.median = (sorted[middle - 1] + sorted[middle]) / 2.0f;
    } else {
        summary.median = sorted[middle];
    }
    
    // Calculate percentiles
    auto percentile = [sorted](float percentile) {
        if (sorted.empty()) return 0.0f;
        size_t index = static_cast<size_t>(percentile * (sorted.size() - 1));
        if (index >= sorted.size()) index = sorted.size() - 1;
        return sorted[index];
    };
    
    summary.percentile95 = percentile(0.95f);
    summary.percentile99 = percentile(0.99f);
        ss << "    " << recentMean << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm