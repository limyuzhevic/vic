// MetricsCollector implementation
#include "MetricsCollector.hpp"
#include "../Errors/Error.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>
#include <numeric>
#include <sstream>

using json = nlohmann::json;

namespace nlm {

MetricsCollector::MetricsCollector(const MetricsConfig& config) : config_(config) {
    validateConfig();
    NLM_LOG_INFO("MetricsCollector initialized with snapshot interval: " + 
                 std::to_string(config_.snapshotInterval));
}

void MetricsCollector::configure(const MetricsConfig& config) {
    config_ = config;
    validateConfig();
    NLM_LOG_INFO("MetricsCollector configured with new settings");
}

void MetricsCollector::recordSnapshot(const Brain& brain, const AgentBrain& agent,
                                     const SimpleWorld& world, SimulationStep step,
                                     Timestamp timestamp, double wallClockTime) {
    if (recordingPaused_) return;
    if (step < config_.startStep) return;
    if (step >= config_.endStep) return;
    
    MetricSnapshot snapshot;
    snapshot.step = step;
    snapshot.timestamp = timestamp;
    snapshot.elapsedTime = wallClockTime;
    
    // Record performance metrics
    if (config_.enablePerformanceMetrics) {
        snapshot.cpuUsage = calculateCPUUsage(); // TODO: implement
        snapshot.memoryUsage = calculateMemoryUsage(); // TODO: implement
        snapshot.simulationSpeed = (wallClockTime > 0) ? 
            static_cast<double>(step) / wallClockTime : 0.0;
    }
    
    // Record learning metrics
    if (config_.enableLearningMetrics) {
        snapshot.learningRate = brain.getAverageLearningRate(); // TODO: implement
        snapshot.predictionError = agent.getPredictionError();
        snapshot.noveltyLevel = agent.getNoveltyLevel();
        snapshot.dopamineLevel = agent.getNeuromodulationLevel();
    }
    
    // Record network metrics
    if (config_.enableNetworkMetrics) {
        snapshot.activeNeuronCount = brain.getActiveNeuronCount(); // TODO: implement
        snapshot.firingNeuronCount = brain.getFiringNeuronCount(); // TODO: implement
        snapshot.averageFiringRate = brain.getAverageFiringRate();
        snapshot.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
        snapshot.totalSpikeCount = brain.getTotalSpikeCount();
    }
    
    // Record behavioral metrics
    if (config_.enableBehavioralMetrics) {
        snapshot.rewardReceived = brain.getLastStepReward(); // TODO: implement
        snapshot.cumulativeReward = brain.getCumulativeReward(); // TODO: implement
        snapshot.actionCount = 1; // TODO: implement
        // TODO: track action frequency
    }
    
    // Record development metrics
    if (config_.enableDevelopmentMetrics) {
        snapshot.plasticityModifier = agent.getPlasticityModifier();
        snapshot.developmentalStage = agent.getDevelopmentalStage();
        snapshot.developmentalAge = agent.getDevelopmentalAge(); // TODO: implement
    }
    
    // Record memory metrics
    if (config_.enableMemoryMetrics) {
        snapshot.workingMemoryCapacity = brain.getWorkingMemoryCapacity(); // TODO: implement
        snapshot.episodicMemoryCount = brain.getEpisodicMemoryCount(); // TODO: implement
        snapshot.memoryConsolidationRate = brain.getMemoryConsolidationRate(); // TODO: implement
    }
    
    // Record energy metrics
    if (config_.enableEnergyMetrics) {
        snapshot.energyConsumed = world.getAgentEnergyConsumed(); // TODO: implement
        snapshot.healthLevel = world.getAgentHealth(); // TODO: implement
        snapshot.environmentChanged = world.hasEnvironmentChanged(); // TODO: implement
    }
    
    // Add to collection
    snapshots_.push_back(snapshot);
    
    // Maintain maximum size
    if (snapshots_.size() > config_.maxHistorySize) {
        size_t excess = snapshots_.size() - config_.maxHistorySize;
        snapshots_.erase(snapshots_.begin(), snapshots_.begin() + excess);
    }
    
    // Notify progress callback
    updateProgress(step, config_.endStep - config_.startStep);
}

const MetricSnapshot* MetricsCollector::getSnapshot(SimulationStep step) const {
    for (const auto& snapshot : snapshots_) {
        if (snapshot.step == step) {
            return &snapshot;
        }
    }
    return nullptr;
}

MetricsAnalysis MetricsCollector::analyze() const {
    MetricsAnalysis analysis;
    
    if (snapshots_.empty()) {
        return analysis;
    }
    
    // Calculate performance metrics
    std::vector<double> speeds;
    std::vector<double> memoryUsages;
    for (const auto& snapshot : snapshots_) {
        speeds.push_back(snapshot.simulationSpeed);
        memoryUsages.push_back(snapshot.memoryUsage);
    }
    
    analysis.averageSimulationSpeed = calculateAverage(speeds);
    analysis.minSimulationSpeed = *std::min_element(speeds.begin(), speeds.end());
    analysis.maxSimulationSpeed = *std::max_element(speeds.begin(), speeds.end());
    analysis.memoryUsageAverage = calculateAverage(memoryUsages);
    
    // Calculate learning metrics
    std::vector<float> learningRates;
    for (const auto& snapshot : snapshots_) {
        if (snapshot.learningRate > 0) learningRates.push_back(snapshot.learningRate);
    }
    
    if (!learningRates.empty()) {
        analysis.initialLearningRate = learningRates.front();
        analysis.finalLearningRate = learningRates.back();
        analysis.maxLearningRate = *std::max_element(learningRates.begin(), learningRates.end());
        analysis.totalLearningProgress = analysis.finalLearningRate - analysis.initialLearningRate;
    }
    
    // Calculate behavioral metrics
    std::vector<float> rewards;
    for (const auto& snapshot : snapshots_) {
        if (snapshot.rewardReceived > 0) rewards.push_back(snapshot.rewardReceived);
    }
    
    if (!rewards.empty()) {
        analysis.totalReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f);
        analysis.averageRewardPerStep = analysis.totalReward / rewards.size();
        
        // Calculate variance
        double mean = analysis.averageRewardPerStep;
        double variance = 0.0;
        for (float reward : rewards) {
            variance += (reward - mean) * (reward - mean);
        }
        analysis.rewardVariance = static_cast<float>(variance / rewards.size());
    }
    
    // Calculate stability metrics
    analysis.stabilityIndex = calculateStabilityIndex(); // TODO: implement
    analysis.burstingActivity = calculateBurstingActivity(); // TODO: implement
    analysis.activeNeuronVariability = calculateVariability(
        getSnapshotsOfMetric(&MetricSnapshot::activeNeuronCount) // TODO: implement helper
    );
    
    // Generate insights and recommendations
    generateInsights(analysis);
    
    return analysis;
}

bool MetricsCollector::exportToCSV(const std::string& filename) const {
    if (snapshots_.empty()) {
        NLM_LOG_ERROR("Cannot export metrics: no data collected");
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filename);
        return false;
    }
    
    // Write header
    file << "Step,Timestamp,ElapsedTime,CPUM Usage,Memory Usage,Simulation Speed,";
    file << "Learning Rate,Prediction Error,Novelty Level,Dopamine Level,";
    file << "Active Neurons,Firing Neurons,Average Firing Rate,";
    file << "Excitation Inhibition Ratio,Total Spikes,Reward Received,Cumulative Reward;\n";
    
    // Write data
    for (const auto& snapshot : snapshots_) {
        file << snapshot.step << "," << snapshot.timestamp << "," << snapshot.elapsedTime << ",";
        file << snapshot.cpuUsage << "," << snapshot.memoryUsage << "," << snapshot.simulationSpeed << ",";
        file << snapshot.learningRate << "," << snapshot.predictionError << "," << snapshot.noveltyLevel << ",";
        file << snapshot.dopamineLevel << ",\" << snapshot.activeNeuronCount << ",";
        file << snapshot.firingNeuronCount << "," << snapshot.averageFiringRate << ",\";
        file << snapshot.excitationInhibitionRatio << "," << snapshot.totalSpikeCount << ",";
        file << snapshot.rewardReceived << "," << snapshot.cumulativeReward << ";\n";
    }
    
    file.close();
    NLM_LOG_INFO("Metrics exported to CSV: " + filename);
    return true;
}

bool MetricsCollector::exportToJSON(const std::string& filename) const {
    if (snapshots_.empty()) {
        NLM_LOG_ERROR("Cannot export metrics: no data collected");
        return false;
    }
    
    json j;
    j["metadata"] = {
        {"snapshotCount", snapshots_.size()},
        {"startStep", snapshots_.front().step},
        {"endStep", snapshots_.back().step},
        {"exportTimestamp", generateTimestamp()}
    };
    
    j["snapshots"] = json::array();
    for (const auto& snapshot : snapshots_) {
        json snapshotJson;
        snapshotJson["step"] = snapshot.step;
        snapshotJson["timestamp"] = snapshot.timestamp;
        snapshotJson["elapsedTime"] = snapshot.elapsedTime;
        snapshotJson["cpuUsage"] = snapshot.cpuUsage;
        snapshotJson["memoryUsage"] = snapshot.memoryUsage;
        snapshotJson["simulationSpeed"] = snapshot.simulationSpeed;
        snapshotJson["learningRate"] = snapshot.learningRate;
        snapshotJson["predictionError"] = snapshot.predictionError;
        snapshotJson["noveltyLevel"] = snapshot.noveltyLevel;
        snapshotJson["dopamineLevel"] = snapshot.dopamineLevel;
        snapshotJson["activeNeuronCount"] = snapshot.activeNeuronCount;
        snapshotJson["firingNeuronCount"] = snapshot.firingNeuronCount;
        snapshotJson["averageFiringRate"] = snapshot.averageFiringRate;
        snapshotJson["excitationInhibitionRatio"] = snapshot.excitationInhibitionRatio;
        snapshotJson["totalSpikeCount"] = snapshot.totalSpikeCount;
        snapshotJson["rewardReceived"] = snapshot.rewardReceived;
        snapshotJson["cumulativeReward"] = snapshot.cumulativeReward;
        j["snapshots"].push_back(snapshotJson);
    }
    
    // Add analysis
    auto analysis = analyze();
    j["analysis"] = {
        {"averageSimulationSpeed", analysis.averageSimulationSpeed},
        {"minSimulationSpeed", analysis.minSimulationSpeed},
        {"maxSimulationSpeed", analysis.maxSimulationSpeed},
        {"memoryUsageAverage", analysis.memoryUsageAverage},
        {"initialLearningRate", analysis.initialLearningRate},
        {"finalLearningRate", analysis.finalLearningRate},
        {"maxLearningRate", analysis.maxLearningRate},
        {"totalLearningProgress", analysis.totalLearningProgress},
        {"totalReward", analysis.totalReward},
        {"averageRewardPerStep", analysis.averageRewardPerStep},
        {"rewardVariance", analysis.rewardVariance}
    };
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filename);
        return false;
    }
    
    file << j.dump(2);
    file.close();
    NLM_LOG_INFO("Metrics exported to JSON: " + filename);
    return true;
}

double MetricsCollector::calculateRunningAverage(size_t numSamples) const {
    if (snapshots_.empty()) return 0.0;
    
    size_t start = std::max<size_t>(0, static_cast<long>(snapshots_.size()) - numSamples);
    double sum = 0.0;
    for (size_t i = start; i < snapshots_.size(); ++i) {
        sum += snapshots_[i].simulationSpeed;
    }
    
    return sum / static_cast<double>(snapshots_.size() - start);
}

double MetricsCollector::calculatePerformanceScore() const {
    if (snapshots_.empty()) return 0.0;
    
    // Composite performance score based on multiple factors
    double speedScore = calculateRunningAverage() / 1000.0; // Normalize
    double learningScore = calculateAverageLearningRate() / 0.1; // Normalize
    
    return std::min(1.0, (speedScore + learningScore) / 2.0);
}

std::string MetricsCollector::generateReport() const {
    std::ostringstream report;
    report << "NLM Metrics Report\n";
    report << "===================\n\n";
    
    if (snapshots_.empty()) {
        report << "No metrics collected.\n";
        return report.str();
    }
    
    auto analysis = analyze();
    
    report << "Summary Statistics:\n";
    report << "  Total snapshots: " << snapshots_.size() << "\n";
    report << "  Simulation steps: " << (snapshots_.back().step - snapshots_.front().step + 1) << "\n";
    report << "  Total time: " << (snapshots_.back().elapsedTime - snapshots_.front().elapsedTime) << "s\n";
    report << "  Average speed: " << analysis.averageSimulationSpeed << " steps/s\n";
    report << "  Memory usage: " << analysis.memoryUsageAverage << " MB\n";
    report << "  Learning progress: " << analysis.totalLearningProgress << "\n";
    report << "  Total reward: " << analysis.totalReward << "\n";
    report << "  Average reward/step: " << analysis.averageRewardPerStep << "\n\n";
    
    report << "Insights:\n";
    auto insights = analyze().insights;
    for (const auto& insight : insights) {
        report << "  - " << insight << "\n";
    }
    
    return report.str();
}

void MetricsCollector::updateProgress(SimulationStep step, size_t totalSteps) const {
    if (progressCallback_) {
        double progress = (totalSteps > 0) ? 
            static_cast<double>(step) / static_cast<double>(totalSteps) : 0.0;
        progressCallback_(snapshots_.back(), step, totalSteps);
    }
}

void MetricsCollector::validateConfig() const {
    if (config_.snapshotInterval <= 0) {
        throw ConfigValidationError("Snapshot interval must be positive");
    }
    if (config_.maxHistorySize == 0) {
        throw ConfigValidationError("Max history size must be positive");
    }
    if (config_.endStep <= config_.startStep) {
        throw ConfigValidationError("End step must be greater than start step");
    }
}

void MetricsCollector::normalizeMetricValues(MetricSnapshot& snapshot) const {
    // Normalize metric values to 0-1 range for comparison
    // Implementation depends on specific metrics
}

} // namespace nlm