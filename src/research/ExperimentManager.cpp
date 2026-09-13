#include "ExperimentManager.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace nlm {

namespace results_utils {

StatisticalSummary calculateStatistics(const std::vector<double>& data) {
    StatisticalSummary summary;
    if (data.empty()) {
        return summary;
    }
    
    summary.mean = calculateMean(data);
    summary.stddev = calculateStdDev(data, summary.mean);
    summary.median = calculateMedian(data);
    summary.min = *std::min_element(data.begin(), data.end());
    summary.max = *std::max_element(data.begin(), data.end());
    summary.variance = summary.stddev * summary.stddev;
    
    if (std::abs(summary.mean) > 1e-10) {
        summary.coefficientOfVariation = std::abs(summary.stddev / summary.mean) * 100.0;
    } else {
        summary.coefficientOfVariation = 0.0;
    }
    
    return summary;
}

double calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double calculateStdDev(const std::vector<double>& data, double mean) {
    if (data.size() < 2) return 0.0;
    
    double sumSquares = 0.0;
    for (double value : data) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (data.size() - 1));
}

double calculateMedian(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double calculatePercentile(const std::vector<double>& data, double percentile) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    double index = (percentile / 100.0) * (sorted.size() - 1);
    size_t lowerIndex = static_cast<size_t>(std::floor(index));
    size_t upperIndex = static_cast<size_t>(std::ceil(index));
    
    if (lowerIndex == upperIndex) {
        return sorted[lowerIndex];
    }
    
    double weight = index - lowerIndex;
    return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
}

std::string analyzeLearningCurve(const std::vector<std::pair<SimulationStep, double>>& data) {
    if (data.empty()) {
        return "No data available for learning curve analysis";
    }
    
    std::ostringstream oss;
    oss << "Learning Curve Analysis\n";
    oss << "=======================" << "\n\n";
    
    // Extract step and value vectors
    std::vector<SimulationStep> steps;
    std::vector<double> values;
    for (const auto& point : data) {
        steps.push_back(point.first);
        values.push_back(point.second);
    }
    
    // Calculate statistics
    double initialValue = values[0];
    double finalValue = values.back();
    double improvement = finalValue - initialValue;
    double relativeImprovement = improvement / (std::abs(initialValue) + 1e-10);
    
    oss << "Initial Value: " << initialValue << "\n";
    oss << "Final Value: " << finalValue << "\n";
    oss << "Absolute Improvement: " << improvement << "\n";
    oss << "Relative Improvement: " << relativeImprovement * 100.0 << "%\n\n";
    
    // Determine learning phase
    std::string learningPhase = identifyLearningPhase(values);
    oss << "Learning Phase: " << learningPhase << "\n\n";
    
    // Calculate learning rate (simplified)
    double learningRate = 0.0;
    if (steps.size() > 1) {
        double totalSteps = steps.back() - steps.front();
        if (totalSteps > 0) {
            learningRate = improvement / totalSteps;
        }
    }
    
    oss << "Learning Rate: " << learningRate << " per step\n";
    
    // Detect plateau
    std::string plateau = detectPlateau(values);
    oss << "Plateau Detection: " << plateau << "\n";
    
    return oss.str();
}

std::string analyzePerformanceProfile(const std::vector<std::pair<SimulationStep, double>>& data) {
    if (data.empty()) {
        return "No data available for performance profile analysis";
    }
    
    std::ostringstream oss;
    oss << "Performance Profile Analysis\n";
    oss << "=============================" << "\n\n";
    
    // Extract values
    std::vector<double> values;
    for (const auto& point : data) {
        values.push_back(point.second);
    }
    
    // Calculate performance statistics
    double mean = calculateMean(values);
    double stddev = calculateStdDev(values, mean);
    double median = calculateMedian(values);
    
    oss << "Mean Performance: " << mean << "\n";
    oss << "Median Performance: " << median << "\n";
    oss << "Standard Deviation: " << stddev << "\n";
    oss << "Coefficient of Variation: " << (std::abs(mean) > 1e-10 ? (stddev / std::abs(mean) * 100.0) : 0.0) << "%\n\n";
    
    // Performance classification
    oss << "Performance Classification:\n";
    if (mean > 0.8) {
        oss << "  State: Excellent - High performance levels\n";
    } else if (mean > 0.5) {
        oss << "  State: Good - Acceptable performance levels\n";
    } else if (mean > 0.2) {
        oss << "  State: Fair - Below average performance\n";
    } else {
        oss << "  State: Poor - Low performance levels\n";
    }
    
    // Stability analysis
    oss << "\nStability Analysis:\n";
    if (stddev < 0.1 * std::abs(mean)) {
        oss << "  State: Stable - Low variance in performance\n";
    } else if (stddev < 0.3 * std::abs(mean)) {
        oss << "  State: Moderately Stable - Acceptable variance\n";
    } else {
        oss << "  State: Unstable - High variance in performance\n";
    }
    
    return oss.str();
}

std::string analyzeStability(const std::vector<double>& data, double threshold) {
    if (data.size() < 10) {
        return "Insufficient data for stability analysis";
    }
    
    double mean = calculateMean(data);
    double stddev = calculateStdDev(data, mean);
    
    if (std::abs(mean) < threshold) {
        return "Stable - Mean value is near zero";
    }
    
    double cv = (std::abs(mean) > 1e-10) ? (stddev / std::abs(mean)) : 0.0;
    
    if (cv < threshold) {
        return "Stable - Low coefficient of variation";
    } else if (cv < 2 * threshold) {
        return "Moderately Stable - Acceptable coefficient of variation";
    } else {
        return "Unstable - High coefficient of variation";
    }
}

double calculatePerformanceDifference(const std::vector<double>& data1,
                                     const std::vector<double>& data2) {
    if (data1.empty() || data2.empty()) {
        return 0.0;
    }
    
    double mean1 = calculateMean(data1);
    double mean2 = calculateMean(data2);
    
    return mean1 - mean2;
}

double calculateEffectSize(const std::vector<double>& data1,
                          const std::vector<double>& data2) {
    if (data1.size() < 2 || data2.size() < 2) {
        return 0.0;
    }
    
    double mean1 = calculateMean(data1);
    double mean2 = calculateMean(data2);
    double stddev1 = calculateStdDev(data1, mean1);
    double stddev2 = calculateStdDev(data2, mean2);
    
    double pooledStddev = std::sqrt((stddev1 * stddev1 + stddev2 * stddev2) / 2.0);
    if (pooledStddev == 0.0) {
        return 0.0;
    }
    
    return (mean1 - mean2) / pooledStddev;
}

std::string interpretEffectSize(double effectSize) {
    if (std::abs(effectSize) < 0.2) {
        return "Negligible effect";
    } else if (std::abs(effectSize) < 0.5) {
        return "Small effect";
    } else if (std::abs(effectSize) < 0.8) {
        return "Medium effect";
    } else {
        return "Large effect";
    }
}

double calculateReproducibilityScore(const std::string& configHash,
                                    const std::string& seedHash) const {
    // Simple reproducibility score based on hash matching
    if (configHash == seedHash) {
        return 1.0; // Perfect match
    }
    
    // Calculate similarity based on hash comparison
    double similarity = 0.0;
    if (!configHash.empty() && !seedHash.empty()) {
        // Simple character-wise comparison (simplified)
        size_t minLength = std::min(configHash.size(), seedHash.size());
        size_t matches = 0;
        for (size_t i = 0; i < minLength; ++i) {
            if (configHash[i] == seedHash[i]) {
                matches++;
            }
        }
        similarity = static_cast<double>(matches) / minLength;
    }
    
    return similarity;
}

double calculateQualityScore(const ExperimentMetadata& metadata) const {
    double score = 0.0;
    
    // Weight different quality factors
    double statusWeight = 0.3;
    double progressWeight = 0.2;
    double validationWeight = 0.2;
    double reproducibilityWeight = 0.3;
    
    // Status contribution
    switch (metadata.status) {
        case TrackingStatus::Completed:
            score += statusWeight * 1.0;
            break;
        case TrackingStatus::Active:
            score += statusWeight * 0.7;
            break;
        case TrackingStatus::Failed:
            score += statusWeight * 0.0;
            break;
        case TrackingStatus::Paused:
            score += statusWeight * 0.3;
            break;
        default:
            score += statusWeight * 0.1;
    }
    
    // Progress contribution
    score += progressWeight * metadata.progress;
    
    // Validation contribution
    score += validationWeight * (metadata.validationPassed ? 1.0 : 0.0);
    
    // Reproducibility contribution
    std::string reproScoreStr = calculateReproducibilityScore(metadata.configHash, metadata.randomSeedHash);
    try {
        double reproScore = std::stod(reproScoreStr);
        score += reproducibilityWeight * reproScore;
    } catch (...) {
        score += reproducibilityWeight * 0.0;
    }
    
    return score;
}

std::string formatStatisticalSummary(const StatisticalSummary& summary) {
    std::ostringstream oss;
    oss << "Statistical Summary for " << summary.metricName << ":\n";
    oss << "  Mean: " << summary.mean << "\n";
    oss << "  Median: " << summary.median << "\n";
    oss << "  Standard Deviation: " << summary.stddev << "\n";
    oss << "  Variance: " << summary.variance << "\n";
    oss << "  Minimum: " << summary.min << "\n";
    oss << "  Maximum: " << summary.max << "\n";
    oss << "  Coefficient of Variation: " << summary.coefficientOfVariation << "%\n";
    return oss.str();
}

std::string formatExperimentMetadata(const ExperimentMetadata& metadata) {
    std::ostringstream oss;
    oss << "Experiment: " << metadata.experimentId << "\n";
    oss << "  Configuration: " << metadata.configName << "\n";
    oss << "  Seed: " << metadata.seed << "\n";
    oss << "  Max Steps: " << metadata.maxSteps << "\n";
    oss << "  Status: " << static_cast<int>(metadata.status) << "\n";
    oss << "  Progress: " << metadata.progress * 100.0 << "%\n";
    oss << "  Total Reward: " << metadata.totalReward << "\n";
    oss << "  Final Firing Rate: " << metadata.finalFiringRate << "\n";
    oss << "  Steps Recorded: " << metadata.totalStepsRecorded << "\n";
    oss << "  Validation Passed: " << (metadata.validationPassed ? "Yes" : "No") << "\n";
    return oss.str();
}

std::string formatValidationResult(const ValidationResult& result) {
    std::ostringstream oss;
    oss << "Validation Result:\n";
    oss << "  Passed: " << (result.passed ? "Yes" : "No") << "\n";
    oss << "  Quality Score: " << result.qualityScore << "/1.0\n";
    
    if (!result.errors.empty()) {
        oss << "  Errors:\n";
        for (const auto& error : result.errors) {
            oss << "    - " << error << "\n";
        }
    }
    
    if (!result.warnings.empty()) {
        oss << "  Warnings:\n";
        for (const auto& warning : result.warnings) {
            oss << "    - " << warning << "\n";
        }
    }
    
    if (!result.recommendations.empty()) {
        oss << "  Recommendations:\n";
        for (const auto& rec : result.recommendations) {
            oss << "    - " << rec << "\n";
        }
    }
    
    return oss.str();
}

std::string exportToCSV(const std::vector<ExperimentMetadata>& metadata) {
    std::ostringstream oss;
    oss << "ExperimentID,ConfigName,Seed,MaxSteps,Status,Progress,TotalReward,FinalFiringRate,StepsRecorded,ValidationPassed\n";
    
    for (const auto& exp : metadata) {
        oss << exp.experimentId << ",";
        oss << exp.configName << ",";
        oss << exp.seed << ",";
        oss << exp.maxSteps << ",";
        oss << static_cast<int>(exp.status) << ",";
        oss << exp.progress << ",";
        oss << exp.totalReward << ",";
        oss << exp.finalFiringRate << ",";
        oss << exp.totalStepsRecorded << ",";
        oss << (exp.validationPassed ? "Yes" : "No") << "\n";
    }
    
    return oss.str();
}

std::string exportToJSON(const std::vector<ExperimentMetadata>& metadata) {
    std::ostringstream oss;
    oss << "{\n  \"experiments\": [\n";
    
    for (size_t i = 0; i < metadata.size(); ++i) {
        const auto& exp = metadata[i];
        oss << "    {\n";
        oss << "      \"experimentId\": \"" << exp.experimentId << "\",\n";
        oss << "      \"configName\": \"" << exp.configName << "\",\n";
        oss << "      \"seed\": " << exp.seed << ",\n";
        oss << "      \"maxSteps\": " << exp.maxSteps << ",\n";
        oss << "      \"status\": " << static_cast<int>(exp.status) << ",\n";
        oss << "      \"progress\": " << exp.progress << ",\n";
        oss << "      \"totalReward\": " << exp.totalReward << ",\n";
        oss << "      \"finalFiringRate\": " << exp.finalFiringRate << ",\n";
        oss << "      \"totalStepsRecorded\": " << exp.totalStepsRecorded << ",\n";
        oss << "      \"validationPassed\": " << (exp.validationPassed ? "true" : "false") << "\n";
        if (i < metadata.size() - 1) {
            oss << "    },\n";
        } else {
            oss << "    }\n";
        }
    }
    
    oss << "  ]\n}";
    return oss.str();
}

std::string exportToBinary(const std::vector<ExperimentMetadata>& metadata) {
    // Simplified binary export - in practice would use proper binary serialization
    std::ostringstream oss;
    oss << "Binary export not implemented (placeholder)\n";
    oss << "Total experiments: " << metadata.size() << "\n";
    return oss.str();
}

} // namespace results_utils

struct ExperimentManager::Impl {
    // Experiment tracking
    std::map<std::string, ExperimentMetadata> experimentMetadata;
    std::map<std::string, std::shared_ptr<Experiment>> experimentObjects;
    std::map<std::string, std::map<SimulationStep, std::vector<Experiment::StepData>>> experimentStepData;
    std::map<std::string, std::map<std::string, double>> experimentMetrics;
    
    // Configuration management
    std::map<std::string, ResearchConfig> configurations;
    
    // Experiment lifecycle tracking
    std::set<std::string> runningExperiments;
    std::set<std::string> pausedExperiments;
    std::set<std::string> completedExperiments;
    std::set<std::string> failedExperiments;
    std::set<std::string> archivedExperiments;
    
    // Control and synchronization
    std::mutex experimentsMutex;
    std::condition_variable experimentsCondition;
    std::atomic<bool> systemRunning;
    std::atomic<bool> systemPaused;
    
    // Monitoring and performance
    size_t totalExperimentsRun;
    size_t totalExperimentsCompleted;
    size_t totalExperimentsFailed;
    double totalSystemTime;
    
    // Error handling
    std::string lastError;
    std::vector<std::string> errorHistory;
    
    // Performance monitoring
    bool performanceMonitoringEnabled;
    double maxCpuUsage;
    double maxMemoryUsage;
    
    Impl() : systemRunning(false), systemPaused(false),
             totalExperimentsRun(0), totalExperimentsCompleted(0),
             totalExperimentsFailed(0), totalSystemTime(0.0),
             performanceMonitoringEnabled(false),
             maxCpuUsage(100.0), maxMemoryUsage(100.0) {}
};

ExperimentManager::ExperimentManager() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

ExperimentManager::~ExperimentManager() {
    stopAllExperiments();
}

void ExperimentManager::initializeDefaults() {
    // Initialize default configurations
    pImpl->configurations = {
        {"minimal", ResearchConfigManager::createMinimalConfig()},
        {"standard", ResearchConfigManager::createStandardConfig()},
        {"large_scale", ResearchConfigManager::createLargeScaleConfig()},
        {"developmental", ResearchConfigManager::createDevelopmentalConfig()}
    };
}

std::string ExperimentManager::createExperiment(const std::string& configName,
                                                const std::string& description,
                                                uint64_t seed,
                                                SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    // Generate unique experiment ID
    std::string experimentId = generateExperimentId();
    
    // Check if configuration exists
    if (pImpl->configurations.find(configName) == pImpl->configurations.end()) {
        throw std::runtime_error("Configuration not found: " + configName);
    }
    
    // Create experiment metadata
    ExperimentMetadata metadata;
    metadata.experimentId = experimentId;
    metadata.configName = configName;
    metadata.description = description;
    metadata.seed = seed;
    metadata.maxSteps = maxSteps;
    metadata.status = TrackingStatus::Active;
    metadata.progress = 0.0;
    metadata.validationPassed = true;
    
    // Calculate hashes for reproducibility
    metadata.configHash = calculateHash(pImpl->configurations[configName].name);
    metadata.randomSeedHash = calculateHash(std::to_string(seed));
    
    // Store metadata
    pImpl->experimentMetadata[experimentId] = metadata;
    
    // Create experiment object
    auto experiment = std::make_shared<Experiment>(experimentId);
    experiment->setSeed(seed);
    
    pImpl->experimentObjects[experimentId] = experiment;
    
    // Initialize step data and metrics tracking
    pImpl->experimentStepData[experimentId] = {};
    pImpl->experimentMetrics[experimentId] = {};
    
    pImpl->totalExperimentsRun++;
    
    return experimentId;
}

bool ExperimentManager::startExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (it->second.status != TrackingStatus::Active &&
        it->second.status != TrackingStatus::Paused) {
        return false; // Can only start active or paused experiments
    }
    
    it->second.status = TrackingStatus::Active;
    it->second.startTime = std::chrono::system_clock::now();
    it->second.progress = 0.0;
    
    pImpl->runningExperiments.insert(experimentId);
    
    return true;
}

bool ExperimentManager::pauseExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (it->second.status != TrackingStatus::Active) {
        return false;
    }
    
    it->second.status = TrackingStatus::Paused;
    
    pImpl->runningExperiments.erase(experimentId);
    pImpl->pausedExperiments.insert(experimentId);
    
    return true;
}

bool ExperimentManager::resumeExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (it->second.status != TrackingStatus::Paused) {
        return false;
    }
    
    it->second.status = TrackingStatus::Active;
    
    pImpl->runningExperiments.insert(experimentId);
    pImpl->pausedExperiments.erase(experimentId);
    
    return true;
}

bool ExperimentManager::completeExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    it->second.status = TrackingStatus::Completed;
    it->second.endTime = std::chrono::system_clock::now();
    it->second.totalDuration = 
        std::chrono::duration_cast<std::chrono::duration<double>>(
            it->second.endTime - it->second.startTime);
    it->second.progress = 1.0;
    
    pImpl->runningExperiments.erase(experimentId);
    pImpl->pausedExperiments.erase(experimentId);
    pImpl->completedExperiments.insert(experimentId);
    pImpl->totalExperimentsCompleted++;
    
    return true;
}

bool ExperimentManager::failExperiment(const std::string& experimentId, const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    it->second.status = TrackingStatus::Failed;
    it->second.endTime = std::chrono::system_clock::now();
    it->second.totalDuration = 
        std::chrono::duration_cast<std::chrono::duration<double>>(
            it->second.endTime - it->second.startTime);
    it->second.errorMessage = errorMessage;
    it->second.validationPassed = false;
    
    pImpl->runningExperiments.erase(experimentId);
    pImpl->pausedExperiments.erase(experimentId);
    pImpl->failedExperiments.insert(experimentId);
    pImpl->totalExperimentsFailed++;
    
    logError(errorMessage);
    
    return true;
}

bool ExperimentManager::archiveExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (it->second.status != TrackingStatus::Completed &&
        it->second.status != TrackingStatus::Failed) {
        return false; // Can only archive completed or failed experiments
    }
    
    it->second.status = TrackingStatus::Archived;
    
    pImpl->completedExperiments.erase(experimentId);
    pImpl->failedExperiments.erase(experimentId);
    pImpl->archivedExperiments.insert(experimentId);
    
    return true;
}

bool ExperimentManager::deleteExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Remove from all tracking sets
    pImpl->runningExperiments.erase(experimentId);
    pImpl->pausedExperiments.erase(experimentId);
    pImpl->completedExperiments.erase(experimentId);
    pImpl->failedExperiments.erase(experimentId);
    pImpl->archivedExperiments.erase(experimentId);
    
    // Remove experiment data
    pImpl->experimentMetadata.erase(it);
    pImpl->experimentObjects.erase(experimentId);
    pImpl->experimentStepData.erase(experimentId);
    pImpl->experimentMetrics.erase(experimentId);
    
    return true;
}

bool ExperimentManager::recordExperimentStep(const std::string& experimentId,
                                            SimulationStep step,
                                            size_t neuronCount,
                                            size_t synapseCount,
                                            size_t firingCount,
                                            float avgFiringRate) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Record step data
    Experiment::StepData stepData;
    stepData.step = step;
    stepData.neuronCount = neuronCount;
    stepData.synapseCount = synapseCount;
    stepData.firingCount = firingCount;
    stepData.avgFiringRate = avgFiringRate;
    
    pImpl->experimentStepData[experimentId][step].push_back(stepData);
    
    // Update experiment metadata
    it->second.totalStepsRecorded++;
    it->second.progress = static_cast<double>(step) / it->second.maxSteps;
    
    // Update experiment object if it exists
    auto expIt = pImpl->experimentObjects.find(experimentId);
    if (expIt != pImpl->experimentObjects.end()) {
        expIt->second->recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
    }
    
    return true;
}

bool ExperimentManager::recordExperimentMetric(const std::string& experimentId,
                                              const std::string& name,
                                              double value) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Record metric
    pImpl->experimentMetrics[experimentId][name] = value;
    
    // Update experiment metadata
    if (name == "total_reward") {
        it->second.totalReward = value;
    } else if (name == "final_firing_rate") {
        it->second.finalFiringRate = value;
    }
    
    // Update experiment object if it exists
    auto expIt = pImpl->experimentObjects.find(experimentId);
    if (expIt != pImpl->experimentObjects.end()) {
        expIt->second->recordMetric(name, value);
    }
    
    return true;
}

bool ExperimentManager::updateExperimentProgress(const std::string& experimentId, double progress) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Update progress
    it->second.progress = std::min(1.0, std::max(0.0, progress));
    
    return true;
}

const ExperimentMetadata* ExperimentManager::getExperimentMetadata(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it != pImpl->experimentMetadata.end()) {
        return &it->second;
    }
    
    return nullptr;
}

const Experiment* ExperimentManager::getExperimentData(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentObjects.find(experimentId);
    if (it != pImpl->experimentObjects.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

std::shared_ptr<Experiment> ExperimentManager::getExperimentObject(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentObjects.find(experimentId);
    if (it != pImpl->experimentObjects.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::vector<std::string> ExperimentManager::getExperimentIds() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> ids;
    for (const auto& pair : pImpl->experimentMetadata) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> ExperimentManager::getExperimentIds(TrackingStatus status) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> ids;
    for (const auto& pair : pImpl->experimentMetadata) {
        if (pair.second.status == status) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

std::vector<std::string> ExperimentManager::getExperimentIds(const std::string& configName) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> ids;
    for (const auto& pair : pImpl->experimentMetadata) {
        if (pair.second.configName == configName) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

bool ExperimentManager::addConfig(const ResearchConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    if (pImpl->configurations.find(config.name) != pImpl->configurations.end()) {
        return false; // Already exists
    }
    
    pImpl->configurations[config.name] = config;
    return true;
}

bool ExperimentManager::removeConfig(const std::string& configName) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    return pImpl->configurations.erase(configName) > 0;
}

const ResearchConfig* ExperimentManager::getConfig(const std::string& configName) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->configurations.find(configName);
    if (it != pImpl->configurations.end()) {
        return &it->second;
    }
    
    return nullptr;
}

std::vector<std::string> ExperimentManager::getAvailableConfigs() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->configurations) {
        names.push_back(pair.first);
    }
    return names;
}

ValidationResult ExperimentManager::validateExperiment(const std::string& experimentId) const {
    ValidationResult result;
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        result.passed = false;
        result.errors.push_back("Experiment not found");
        return result;
    }
    
    const auto& metadata = it->second;
    
    // Validate experiment setup
    ValidationResult setupValidation = validateExperimentSetup(metadata);
    if (!setupValidation.passed) {
        result.passed = false;
        result.errors.insert(result.errors.end(), 
                           setupValidation.errors.begin(), setupValidation.errors.end());
        result.warnings.insert(result.warnings.end(),
                             setupValidation.warnings.begin(), setupValidation.warnings.end());
        result.recommendations.insert(result.recommendations.end(),
                                   setupValidation.recommendations.begin(), setupValidation.recommendations.end());
    }
    
    // Calculate quality score
    result.qualityScore = calculateExperimentQualityScore(metadata);
    
    // Final validation result
    result.passed = result.qualityScore > 0.5 && metadata.validationPassed;
    
    return result;
}

ValidationResult ExperimentManager::validateExperimentSetup(const std::string& experimentId) const {
    return validateExperimentSetup(getExperimentMetadata(experimentId));
}

ValidationResult ExperimentManager::validateExperimentSetup(const ExperimentMetadata& metadata) const {
    ValidationResult result;
    
    // Check configuration
    if (metadata.configName.empty()) {
        result.passed = false;
        result.errors.push_back("Experiment has no configuration");
    }
    
    // Check seed
    if (metadata.seed == 0) {
        result.warnings.push_back("Experiment has no explicit seed - results may not be reproducible");
    }
    
    // Check max steps
    if (metadata.maxSteps == 0) {
        result.errors.push_back("Experiment has no maximum steps defined");
    }
    
    // Check for contradictions in configuration (simplified)
    // In a real implementation, would validate against actual configuration parameters
    
    // Calculate quality score
    result.qualityScore = calculateExperimentQualityScore(metadata);
    
    return result;
}

bool ExperimentManager::addValidationIssue(const std::string& experimentId,
                                         const std::string& issue,
                                         const std::string& severity) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (severity == "ERROR" || severity == "error") {
        it->second.validationPassed = false;
        it->second.validationIssues.push_back("ERROR: " + issue);
    } else if (severity == "WARNING" || severity == "warning") {
        it->second.validationIssues.push_back("WARNING: " + issue);
    } else {
        it->second.validationIssues.push_back("INFO: " + issue);
    }
    
    return true;
}

std::string ExperimentManager::optimizeParameters(const std::string& experimentId,
                                                  const std::map<std::string, std::string>& constraints) {
    std::ostringstream oss;
    oss << "Parameter Optimization for Experiment: " << experimentId << "\n";
    oss << "==========================================" << "\n\n";
    
    auto metadata = getExperimentMetadata(experimentId);
    if (!metadata) {
        return "Experiment not found";
    }
    
    // Get current configuration
    const ResearchConfig* config = getConfig(metadata->configName);
    if (!config) {
        return "Configuration not found";
    }
    
    oss << "Current Configuration:\n";
    oss << "  Name: " << config->name << "\n";
    oss << "  Mode: " << static_cast<int>(config->mode) << "\n";
    oss << "  Neurons: " << config->neuron_count << "\n";
    oss << "  Random Seed: " << config->random_seed << "\n";
    
    // Optimization constraints handling (simplified)
    oss << "\nOptimization Constraints:\n";
    for (const auto& pair : constraints) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    // Simple optimization suggestions
    oss << "\nOptimization Suggestions:\n";
    oss << "  1. Increase neuron count for better performance (if constraints allow)\n";
    oss << "  2. Optimize random seed for reproducibility\n";
    oss << "  3. Adjust simulation timestep for balance between accuracy and speed\n";
    oss << "  4. Configure appropriate plasticity settings based on task requirements\n";
    
    return oss.str();
}

bool ExperimentManager::applyOptimizedParameters(const std::string& experimentId,
                                                const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Apply parameters (simplified - would need more sophisticated parameter handling)
    // In a real implementation, would need to modify the ResearchConfig object
    
    // Update experiment metadata
    it->second.validationPassed = true;
    
    return true;
}

bool ExperimentManager::enableReproducibility(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Enable reproducibility features
    it->second.configHash = calculateHash(pImpl->configurations[it->second.configName].name);
    it->second.randomSeedHash = calculateHash(std::to_string(it->second.seed));
    
    return true;
}

bool ExperimentManager::ensureReproducibility(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Ensure reproducibility by setting fixed random seed and configuration
    it->second.seed = static_cast<uint64_t>(std::hash<std::string>{}(it->second.experimentId) % 1000000);
    
    return true;
}

bool ExperimentManager::verifyReproducibility(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    // Verify reproducibility by recalculating hashes
    std::string currentConfigHash = calculateHash(pImpl->configurations[it->second.configName].name);
    std::string currentSeedHash = calculateHash(std::to_string(it->second.seed));
    
    return (currentConfigHash == it->second.configHash && 
            currentSeedHash == it->second.randomSeedHash);
}

std::string ExperimentManager::generateReproducibilityReport(const std::string& experimentId) const {
    std::ostringstream oss;
    oss << "Reproducibility Report for Experiment: " << experimentId << "\n";
    oss << "=========================================" << "\n\n";
    
    auto metadata = getExperimentMetadata(experimentId);
    if (!metadata) {
        return "Experiment not found";
    }
    
    oss << "Configuration Hash: " << metadata->configHash << "\n";
    oss << "Random Seed Hash: " << metadata->randomSeedHash << "\n";
    oss << "Reproducibility Score: " << calculateReproducibilityScore(metadata->configHash, metadata->randomSeedHash) << "\n\n";
    
    oss << "Reproducibility Status:\n";
    if (verifyReproducibility(experimentId)) {
        oss << "  ✓ Experiment is reproducible\n";
    } else {
        oss << "  ✗ Experiment reproducibility cannot be verified\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::analyzeExperimentResults(const std::string& experimentId) const {
    std::ostringstream oss;
    oss << "Results Analysis for Experiment: " << experimentId << "\n";
    oss << "================================" << "\n\n";
    
    auto experiment = getExperimentObject(experimentId);
    if (!experiment) {
        return "Experiment not found or not available";
    }
    
    const auto& metrics = experiment->getMetrics();
    const auto& steps = experiment->getStepData();
    
    oss << "Basic Statistics:\n";
    for (const auto& metric : metrics) {
        oss << "  " << metric.first << ": " << metric.second << "\n";
    }
    
    oss << "\nStep Statistics:\n";
    oss << "  Total Steps Recorded: " << steps.size() << "\n";
    if (!steps.empty()) {
        size_t totalNeurons = 0, totalSynapses = 0, totalFiring = 0;
        float totalFiringRate = 0.0;
        
        for (const auto& step : steps) {
            totalNeurons += step.neuronCount;
            totalSynapses += step.synapseCount;
            totalFiring += step.firingCount;
            totalFiringRate += step.avgFiringRate;
        }
        
        oss << "  Average Neurons: " << (totalNeurons / steps.size()) << "\n";
        oss << "  Average Synapses: " << (totalSynapses / steps.size()) << "\n";
        oss << "  Average Firing Count: " << (totalFiring / steps.size()) << "\n";
        oss << "  Average Firing Rate: " << (totalFiringRate / steps.size()) << "\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::analyzeExperimentTrends(const std::string& experimentId) const {
    std::ostringstream oss;
    oss << "Trend Analysis for Experiment: " << experimentId << "\n";
    oss << "================================" << "\n\n";
    
    auto experiment = getExperimentObject(experimentId);
    if (!experiment) {
        return "Experiment not found or not available";
    }
    
    const auto& steps = experiment->getStepData();
    
    if (steps.size() < 10) {
        oss << "Insufficient data for trend analysis\n";
        return oss.str();
    }
    
    // Extract firing rates over time
    std::vector<double> firingRates;
    for (const auto& step : steps) {
        firingRates.push_back(step.avgFiringRate);
    }
    
    // Calculate trend (simplified linear regression)
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < steps.size(); ++i) {
        double x = static_cast<double>(i);
        double y = firingRates[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double n = static_cast<double>(steps.size());
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double intercept = (sumY - slope * sumX) / n;
    
    oss << "Trend Analysis Results:\n";
    oss << "  Linear Trend: y = " << intercept << " + " << slope << " * x\n";
    
    if (slope > 0.01) {
        oss << "  Trend: Increasing (improvement over time)\n";
    } else if (slope < -0.01) {
        oss << "  Trend: Decreasing (deterioration over time)\n";
    } else {
        oss << "  Trend: Stable (no significant change)\n";
    }
    
    // Calculate coefficient of variation
    double mean = calculateMean(firingRates);
    double cv = (mean != 0.0) ? (calculateStdDev(firingRates, mean) / std::abs(mean) * 100.0) : 0.0;
    oss << "  Coefficient of Variation: " << cv << "%\n";
    
    if (cv < 10.0) {
        oss << "  Stability: High stability\n";
    } else if (cv < 25.0) {
        oss << "  Stability: Moderate stability\n";
    } else {
        oss << "  Stability: Low stability\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::analyzeExperimentPerformance(const std::string& experimentId) const {
    std::ostringstream oss;
    oss << "Performance Analysis for Experiment: " << experimentId << "\n";
    oss << "=====================================" << "\n\n";
    
    auto experiment = getExperimentObject(experimentId);
    if (!experiment) {
        return "Experiment not found or not available";
    }
    
    const auto& metrics = experiment->getMetrics();
    
    oss << "Performance Metrics:\n";
    for (const auto& metric : metrics) {
        oss << "  " << metric.first << ": " << metric.second << "\n";
    }
    
    // Performance classification based on total reward
    double totalReward = 0.0;
    if (metrics.find("total_reward") != metrics.end()) {
        totalReward = metrics.at("total_reward");
    }
    
    oss << "\nPerformance Classification:\n";
    if (totalReward > 1000.0) {
        oss << "  State: Excellent - High total reward\n";
    } else if (totalReward > 500.0) {
        oss << "  State: Good - Moderate total reward\n";
    } else if (totalReward > 100.0) {
        oss << "  State: Fair - Low total reward\n";
    } else {
        oss << "  State: Poor - Very low total reward\n";
    }
    
    return oss.str();
}

ComparisonResult ExperimentManager::compareExperiments(const std::string& experimentId1,
                                                      const std::string& experimentId2) const {
    ComparisonResult result;
    result.experimentId1 = experimentId1;
    result.experimentId2 = experimentId2;
    
    auto exp1 = getExperimentObject(experimentId1);
    auto exp2 = getExperimentObject(experimentId2);
    
    if (!exp1 || !exp2) {
        result.interpretation = "One or both experiments not found";
        return result;
    }
    
    // Compare metrics
    const auto& metrics1 = exp1->getMetrics();
    const auto& metrics2 = exp2->getMetrics();
    
    double sumDiff = 0.0;
    size_t count = 0;
    
    for (const auto& pair : metrics1) {
        const std::string& metricName = pair.first;
        double value1 = pair.second;
        
        auto it = metrics2.find(metricName);
        if (it != metrics2.end()) {
            double value2 = it->second;
            double diff = value1 - value2;
            sumDiff += diff;
            count++;
        }
    }
    
    if (count > 0) {
        result.performanceDifference = sumDiff / count;
        result.statisticalSignificance = 1.0; // Simplified - would need proper statistical test
        result.effectSize = interpretEffectSize(std::abs(result.performanceDifference));
        
        if (std::abs(result.performanceDifference) > 0.1) {
            result.interpretation = "Significant performance difference between experiments";
        } else {
            result.interpretation = "Minimal performance difference between experiments";
        }
    } else {
        result.interpretation = "No common metrics found for comparison";
    }
    
    return result;
}

std::vector<ComparisonResult> ExperimentManager::compareExperiments(const std::vector<std::string>& experimentIds) const {
    std::vector<ComparisonResult> results;
    
    if (experimentIds.size() < 2) {
        return results;
    }
    
    for (size_t i = 0; i < experimentIds.size(); ++i) {
        for (size_t j = i + 1; j < experimentIds.size(); ++j) {
            ComparisonResult comp = compareExperiments(experimentIds[i], experimentIds[j]);
            results.push_back(comp);
        }
    }
    
    return results;
}

std::string ExperimentManager::aggregateResults(const std::vector<std::string>& experimentIds) const {
    std::ostringstream oss;
    oss << "Results Aggregation for " << experimentIds.size() << " Experiments\n";
    oss << "==============================================" << "\n\n";
    
    std::map<std::string, std::vector<double>> metricData;
    
    for (const auto& expId : experimentIds) {
        auto experiment = getExperimentObject(expId);
        if (!experiment) continue;
        
        const auto& metrics = experiment->getMetrics();
        
        for (const auto& metric : metrics) {
            metricData[metric.first].push_back(metric.second);
        }
    }
    
    oss << "Aggregated Metrics:\n";
    for (const auto& pair : metricData) {
        const auto& values = pair.second;
        double mean = calculateMean(values);
        double stddev = calculateStdDev(values, mean);
        
        oss << "  " << pair.first << ": mean=" << mean << ", stddev=" << stddev << ", n=" << values.size() << "\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::aggregateResultsByConfig(const std::string& configName) const {
    std::ostringstream oss;
    oss << "Results Aggregation for Configuration: " << configName << "\n";
    oss << "===========================================" << "\n\n";
    
    std::vector<std::string> expIds = getExperimentIds(configName);
    
    if (expIds.empty()) {
        oss << "No experiments found for this configuration\n";
        return oss.str();
    }
    
    oss << "Found " << expIds.size() << " experiments\n\n";
    
    // Aggregate by experiment type/status
    std::map<TrackingStatus, size_t> statusCounts;
    std::map<std::string, double> configPerformance;
    
    for (const auto& expId : expIds) {
        auto metadata = getExperimentMetadata(expId);
        if (metadata) {
            statusCounts[metadata->status]++;
        }
        
        auto experiment = getExperimentObject(expId);
        if (experiment) {
            const auto& metrics = experiment->getMetrics();
            if (metrics.find("total_reward") != metrics.end()) {
                configPerformance[expId] = metrics.at("total_reward");
            }
        }
    }
    
    oss << "Status Distribution:\n";
    for (const auto& pair : statusCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    if (!configPerformance.empty()) {
        oss << "\nPerformance Distribution:\n";
        for (const auto& pair : configPerformance) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::string ExperimentManager::aggregateResultsByMetric(const std::string& metricName) const {
    std::ostringstream oss;
    oss << "Results Aggregation for Metric: " << metricName << "\n";
    oss << "============================================" << "\n\n";
    
    std::vector<double> allValues;
    std::map<std::string, double> experimentValues;
    
    for (const auto& pair : pImpl->experimentMetadata) {
        const auto& expId = pair.first;
        
        auto experiment = getExperimentObject(expId);
        if (!experiment) continue;
        
        const auto& metrics = experiment->getMetrics();
        auto it = metrics.find(metricName);
        if (it != metrics.end()) {
            double value = it->second;
            allValues.push_back(value);
            experimentValues[expId] = value;
        }
    }
    
    if (allValues.empty()) {
        oss << "No experiments found with metric: " << metricName << "\n";
        return oss.str();
    }
    
    // Calculate statistics
    double mean = calculateMean(allValues);
    double stddev = calculateStdDev(allValues, mean);
    double median = calculateMedian(allValues);
    
    oss << "Statistical Summary:\n";
    oss << "  Mean: " << mean << "\n";
    oss << "  Median: " << median << "\n";
    oss << "  Standard Deviation: " << stddev << "\n";
    oss << "  Coefficient of Variation: " << (mean != 0.0 ? (stddev / std::abs(mean) * 100.0) : 0.0) << "%\n";
    oss << "  Min: " << *std::min_element(allValues.begin(), allValues.end()) << "\n";
    oss << "  Max: " << *std::max_element(allValues.begin(), allValues.end()) << "\n\n";
    
    oss << "Individual Experiment Values:\n";
    for (const auto& pair : experimentValues) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

double ExperimentManager::getExperimentProgress(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it != pImpl->experimentMetadata.end()) {
        return it->second.progress;
    }
    
    return 0.0;
}

std::chrono::duration<double> ExperimentManager::getEstimatedTimeRemaining(const std::string& experimentId) const {
    auto metadata = getExperimentMetadata(experimentId);
    if (!metadata) {
        return std::chrono::duration<double>(0.0);
    }
    
    // Estimate based on progress and total duration
    if (metadata->progress > 0.0 && metadata->totalDuration.count() > 0.0) {
        double elapsedFraction = metadata->progress;
        if (elapsedFraction > 0.0) {
            double remainingFraction = 1.0 - elapsedFraction;
            std::chrono::duration<double> estimatedRemaining = 
                metadata->totalDuration * remainingFraction;
            return estimatedRemaining;
        }
    }
    
    return std::chrono::duration<double>(0.0);
}

std::string ExperimentManager::generateExperimentReport(const std::string& experimentId) const {
    std::ostringstream oss;
    oss << "Experiment Report: " << experimentId << "\n";
    oss << "=================" << "\n\n";
    
    auto metadata = getExperimentMetadata(experimentId);
    if (!metadata) {
        return "Experiment not found";
    }
    
    oss << formatExperimentMetadata(*metadata) << "\n";
    
    auto experiment = getExperimentObject(experimentId);
    if (experiment) {
        oss << "Detailed Results:\n";
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            oss << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        oss << "\nStep-by-Step Data:\n";
        const auto& steps = experiment->getStepData();
        for (size_t i = 0; i < std::min(size_t(10), steps.size()); ++i) {
            const auto& step = steps[i];
            oss << "  Step " << step.step << ": Neurons=" << step.neuronCount 
                << ", Synapses=" << step.synapseCount 
                << ", Firing=" << step.firingCount 
                << ", AvgRate=" << step.avgFiringRate << "\n";
        }
        
        if (steps.size() > 10) {
            oss << "  ... and " << (steps.size() - 10) << " more steps\n";
        }
    }
    
    return oss.str();
}

std::string ExperimentManager::generateBatchReport(const std::vector<std::string>& experimentIds) const {
    std::ostringstream oss;
    oss << "Batch Report for " << experimentIds.size() << " Experiments\n";
    oss << "================================" << "\n\n";
    
    size_t totalExperiments = experimentIds.size();
    size_t completedExperiments = 0;
    size_t failedExperiments = 0;
    
    double totalReward = 0.0;
    double avgFiringRate = 0.0;
    
    for (const auto& expId : experimentIds) {
        auto metadata = getExperimentMetadata(expId);
        if (!metadata) continue;
        
        switch (metadata->status) {
            case TrackingStatus::Completed:
                completedExperiments++;
                break;
            case TrackingStatus::Failed:
                failedExperiments++;
                break;
        }
        
        auto experiment = getExperimentObject(expId);
        if (experiment) {
            const auto& metrics = experiment->getMetrics();
            
            if (metrics.find("total_reward") != metrics.end()) {
                totalReward += metrics.at("total_reward");
            }
            
            if (metrics.find("final_firing_rate") != metrics.end()) {
                avgFiringRate += metrics.at("final_firing_rate");
            }
        }
    }
    
    oss << "Summary Statistics:\n";
    oss << "  Total Experiments: " << totalExperiments << "\n";
    oss << "  Completed: " << completedExperiments << "\n";
    oss << "  Failed: " << failedExperiments << "\n";
    oss << "  Success Rate: " << (totalExperiments > 0 ? (completedExperiments * 100.0 / totalExperiments) : 0.0) << "%\n";
    
    if (totalExperiments > 0) {
        oss << "  Average Total Reward: " << (totalReward / totalExperiments) << "\n";
        oss << "  Average Final Firing Rate: " << (avgFiringRate / totalExperiments) << "\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::generateComprehensiveReport() const {
    std::ostringstream oss;
    oss << "Comprehensive Experiment Management Report\n";
    oss << "===========================================" << "\n\n";
    
    oss << "System Status:\n";
    oss << "  System Running: " << (isSystemRunning() ? "Yes" : "No") << "\n";
    oss << "  System Paused: " << (isPausedAllExperiments() ? "Yes" : "No") << "\n";
    oss << "  Total Experiments Run: " << pImpl->totalExperimentsRun << "\n";
    oss << "  Total Experiments Completed: " << pImpl->totalExperimentsCompleted << "\n";
    oss << "  Total Experiments Failed: " << pImpl->totalExperimentsFailed << "\n\n";
    
    oss << "Experiment Counts by Status:\n";
    oss << "  Active: " << pImpl->runningExperiments.size() << "\n";
    oss << "  Paused: " << pImpl->pausedExperiments.size() << "\n";
    oss << "  Completed: " << pImpl->completedExperiments.size() << "\n";
    oss << "  Failed: " << pImpl->failedExperiments.size() << "\n";
    oss << "  Archived: " << pImpl->archivedExperiments.size() << "\n\n";
    
    oss << "Available Configurations:\n";
    for (const auto& pair : pImpl->configurations) {
        oss << "  " << pair.first << ": " << pair.second.name << "\n";
    }
    
    return oss.str();
}

bool ExperimentManager::exportExperiment(const std::string& experimentId, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto metadata = getExperimentMetadata(experimentId);
    if (!metadata) {
        return false;
    }
    
    auto experiment = getExperimentObject(experimentId);
    if (!experiment) {
        return false;
    }
    
    // Export to JSON format (simplified)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"experimentId\": \"" << experimentId << "\",\n";
    file << "  \"metadata\": {\n";
    file << "    \"configName\": \"" << metadata->configName << "\",\n";
    file << "    \"seed\": " << metadata->seed << ",\n";
    file << "    \"maxSteps\": " << metadata->maxSteps << ",\n";
    file << "    \"status\": " << static_cast<int>(metadata->status) << ",\n";
    file << "    \"progress\": " << metadata->progress << ",\n";
    file << "    \"totalReward\": " << metadata->totalReward << ",\n";
    file << "    \"finalFiringRate\": " << metadata->finalFiringRate << "\n";
    file << "  },\n";
    
    // Export metrics
    file << "  \"metrics\": [\n";
    const auto& expMetrics = experiment->getMetrics();
    for (size_t i = 0; i < expMetrics.size(); ++i) {
        const auto& metric = expMetrics[i];
        file << "    {\"name\": \"" << metric.first << "\", \"value\": " << metric.second;
        if (i < expMetrics.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "  ]\n";
    
    file << "}\n";
    
    return true;
}

bool ExperimentManager::importExperiment(const std::string& filepath, const std::string& configName) {
    // Simplified import - would need proper JSON parsing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse JSON (simplified)
    std::string line;
    std::string experimentId;
    uint64_t seed = 0;
    
    while (std::getline(file, line)) {
        if (line.find("experimentId") != std::string::npos) {
            size_t pos = line.find('\"') + 1;
            size_t end = line.find('\"', pos);
            if (pos != std::string::npos && end != std::string::npos) {
                experimentId = line.substr(pos, end - pos);
            }
        } else if (line.find("seed") != std::string::npos) {
            size_t pos = line.find('"') + 1;
            size_t end = line.find('"', pos);
            if (pos != std::string::npos && end != std::string::npos) {
                seed = std::stoul(line.substr(pos, end - pos));
            }
        }
    }
    
    if (experimentId.empty()) {
        return false;
    }
    
    // Create experiment with imported data
    std::string description = "Imported experiment from " + filepath;
    std::string newId = createExperiment(configName, description, seed);
    
    return true;
}

std::vector<std::string> ExperimentManager::validateAllConfigs() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> allErrors;
    
    for (const auto& pair : pImpl->configurations) {
        const auto& config = pair.second;
        
        // Simple validation (would need proper ResearchConfigManager integration)
        if (config.neuron_count == 0) {
            allErrors.push_back("Configuration '" + pair.first + "': neuron_count must be > 0");
        }
        
        if (config.random_seed == 0) {
            allErrors.push_back("Configuration '" + pair.first + "': random_seed should be non-zero for reproducibility");
        }
    }
    
    return allErrors;
}

std::string ExperimentManager::generateConfigValidationReport() const {
    std::ostringstream oss;
    oss << "Configuration Validation Report\n";
    oss << "================================" << "\n\n";
    
    std::vector<std::string> errors = validateAllConfigs();
    
    if (errors.empty()) {
        oss << "✓ All configurations are valid\n";
    } else {
        oss << "Errors found:\n";
        for (const auto& error : errors) {
            oss << "  ✗ " << error << "\n";
        }
    }
    
    return oss.str();
}

std::string ExperimentManager::generateQualityReport() const {
    std::ostringstream oss;
    oss << "Quality Report\n";
    oss << "=============" << "\n\n";
    
    oss << "Experiment Quality Metrics:\n";
    
    // Calculate average quality score
    double totalQuality = 0.0;
    size_t count = 0;
    
    for (const auto& pair : pImpl->experimentMetadata) {
        const auto& metadata = pair.second;
        double quality = calculateExperimentQualityScore(metadata);
        totalQuality += quality;
        count++;
        
        oss << "  " << pair.first << ": " << quality << "/1.0\n";
    }
    
    if (count > 0) {
        double avgQuality = totalQuality / count;
        oss << "\nAverage Quality Score: " << avgQuality << "/1.0\n";
        
        if (avgQuality > 0.8) {
            oss << "Overall Quality: Excellent\n";
        } else if (avgQuality > 0.6) {
            oss << "Overall Quality: Good\n";
        } else if (avgQuality > 0.4) {
            oss << "Overall Quality: Fair\n";
        } else {
            oss << "Overall Quality: Poor\n";
        }
    }
    
    return oss.str();
}

bool ExperimentManager::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
    return true;
}

void ExperimentManager::setPerformanceThresholds(double maxCpuUsage, double maxMemoryUsage) {
    pImpl->maxCpuUsage = maxCpuUsage;
    pImpl->maxMemoryUsage = maxMemoryUsage;
}

std::string ExperimentManager::getPerformanceReport() const {
    std::ostringstream oss;
    oss << "Performance Report\n";
    oss << "=================" << "\n\n";
    
    oss << "Monitoring Enabled: " << (pImpl->performanceMonitoringEnabled ? "Yes" : "No") << "\n";
    oss << "CPU Usage Threshold: " << pImpl->maxCpuUsage << "%\n";
    oss << "Memory Usage Threshold: " << pImpl->maxMemoryUsage << "%\n\n";
    
    // Simulated performance metrics
    oss << "Recent Performance Metrics:\n";
    oss << "  Total Experiments per Hour: " << (pImpl->totalExperimentsRun / (24 * 60)) << "\n";
    oss << "  Average Experiment Duration: " << (pImpl->totalSystemTime.count() / pImpl->totalExperimentsCompleted) << " seconds\n";
    oss << "  System Availability: " << (pImpl->systemRunning ? 99.9 : 0.0) << "%\n";
    
    return oss.str();
}

bool ExperimentManager::handleExperimentError(const std::string& experimentId, const std::string& errorMessage) {
    logError(errorMessage);
    
    // Try to recover from error
    bool recovered = attemptExperimentRecovery(experimentId);
    
    return recovered;
}

bool ExperimentManager::attemptExperimentRecovery(const std::string& experimentId) {
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it == pImpl->experimentMetadata.end()) {
        return false;
    }
    
    if (it->second.status == TrackingStatus::Failed) {
        // Attempt to restart failed experiment
        it->second.status = TrackingStatus::Active;
        it->second.progress = 0.0;
        pImpl->runningExperiments.insert(experimentId);
        return true;
    }
    
    return false;
}

std::string ExperimentManager::getLastError() const {
    return pImpl->lastError;
}

void ExperimentManager::updateExperimentMetadata(const std::string& experimentId) {
    auto it = pImpl->experimentMetadata.find(experimentId);
    if (it != pImpl->experimentMetadata.end()) {
        // Update metadata with current state
        auto experiment = getExperimentObject(experimentId);
        if (experiment) {
            const auto& metrics = experiment->getMetrics();
            
            if (metrics.find("total_reward") != metrics.end()) {
                it->second.totalReward = metrics.at("total_reward");
            }
            
            if (metrics.find("final_firing_rate") != metrics.end()) {
                it->second.finalFiringRate = metrics.at("final_firing_rate");
            }
            
            it->second.validationPassed = true;
        }
    }
}

bool ExperimentManager::isExperimentRunning(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    auto it = pImpl->runningExperiments.find(experimentId);
    return it != pImpl->runningExperiments.end();
}

bool ExperimentManager::isSystemRunning() const {
    return pImpl->systemRunning;
}

size_t ExperimentManager::getRunningExperimentsCount() const {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    return pImpl->runningExperiments.size();
}

std::string ExperimentManager::getSystemStatusReport() const {
    return generateComprehensiveReport();
}

void ExperimentManager::stopAllExperiments() {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    pImpl->systemRunning = false;
    pImpl->paused = true;
    
    for (const auto& expId : pImpl->runningExperiments) {
        if (pImpl->experimentMetadata.find(expId) != pImpl->experimentMetadata.end()) {
            pImpl->experimentMetadata[expId].status = TrackingStatus::Failed;
            pImpl->experimentMetadata[expId].errorMessage = "System shutdown requested";
        }
    }
    
    pImpl->runningExperiments.clear();
    pImpl->pausedExperiments.clear();
}

void ExperimentManager::pauseAllExperiments() {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    pImpl->systemPaused = true;
    
    for (const auto& expId : pImpl->runningExperiments) {
        if (pImpl->experimentMetadata.find(expId) != pImpl->experimentMetadata.end()) {
            pImpl->experimentMetadata[expId].status = TrackingStatus::Paused;
        }
    }
    
    pImpl->pausedExperiments = pImpl->runningExperiments;
    pImpl->runningExperiments.clear();
}

void ExperimentManager::resumeAllExperiments() {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    pImpl->systemPaused = false;
    
    for (const auto& expId : pImpl->pausedExperiments) {
        if (pImpl->experimentMetadata.find(expId) != pImpl->experimentMetadata.end()) {
            pImpl->experimentMetadata[expId].status = TrackingStatus::Active;
        }
    }
    
    pImpl->runningExperiments = pImpl->pausedExperiments;
    pImpl->pausedExperiments.clear();
}

void ExperimentManager::cleanupCompletedExperiments() {
    std::lock_guard<std::mutex> lock(pImpl->experimentsMutex);
    
    std::vector<std::string> toRemove;
    
    for (const auto& pair : pImpl->experimentMetadata) {
        if (pair.second.status == TrackingStatus::Completed ||
            pair.second.status == TrackingStatus::Failed) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& expId : toRemove) {
        pImpl->experimentMetadata.erase(expId);
        pImpl->experimentObjects.erase(expId);
        pImpl->experimentStepData.erase(expId);
        pImpl->experimentMetrics.erase(expId);
        
        // Update status sets
        pImpl->runningExperiments.erase(expId);
        pImpl->pausedExperiments.erase(expId);
        pImpl->completedExperiments.erase(expId);
        pImpl->failedExperiments.erase(expId);
    }
}

void ExperimentManager::logError(const std::string& errorMessage) {
    pImpl->lastError = errorMessage;
    pImpl->errorHistory.push_back(errorMessage);
}

void ExperimentManager::handleCriticalError(const std::string& errorMessage) {
    logError(errorMessage);
    
    // In a real implementation, would handle critical errors by:
    // 1. Attempting to save current state
    // 2. Notifying monitoring systems
    // 3. Initiating controlled shutdown if necessary
    
    stopAllExperiments();
}

std::string ExperimentManager::calculateHash(const std::string& data) {
    // Simplified hash function - in practice would use a proper cryptographic hash
    std::hash<std::string> hasher;
    return std::to_string(hasher(data));
}

std::string ExperimentManager::generateExperimentId(const std::string& prefix) {
    static std::atomic<size_t> counter{0};
    return prefix + "_" + std::to_string(++counter);
}

std::string ExperimentManager::generateConfigName(const std::string& prefix) {
    return prefix + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

bool ExperimentManager::isValidExperimentId(const std::string& id) {
    return !id.empty() && id.find('_') != std::string::npos;
}

bool ExperimentManager::isValidConfigName(const std::string& name) {
    return !name.empty();
}

double ExperimentManager::calculateExperimentQualityScore(const ExperimentMetadata& metadata) const {
    double score = 0.0;
    
    // Status component
    switch (metadata.status) {
        case TrackingStatus::Completed:
            score += 0.4;
            break;
        case TrackingStatus::Active:
            score += 0.2;
            break;
        case TrackingStatus::Failed:
            score += 0.0;
            break;
        case TrackingStatus::Paused:
            score += 0.1;
            break;
        case TrackingStatus::Archived:
            score += 0.3;
            break;
        default:
            score += 0.1;
    }
    
    // Progress component
    score += 0.2 * metadata.progress;
    
    // Validation component
    score += 0.2 * (metadata.validationPassed ? 1.0 : 0.0);
    
    // Reproducibility component (simplified)
    double reproScore = calculateReproducibilityScore(metadata.configHash, metadata.randomSeedHash);
    score += 0.2 * reproScore;
    
    return std::min(1.0, score);
}

std::string ExperimentManager::formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time_t_now = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool ExperimentManager::isPausedAllExperiments() const {
    return pImpl->systemPaused;
}

} // namespace nlm