#include "ExperimentMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

struct ExperimentMetrics::Impl {
    std::vector<StepMetrics> stepMetrics;
    
    // Aggregate metrics
    float totalReward;
    float totalSpikes;
    size_t totalActiveNeurons;
    size_t totalFiringNeurons;
    float curiosityLevel;
    float noveltyLevel;
    float dopamineLevel;
    
    // Statistical accumulators
    float rewardSum;
    float rewardSumSq;
    float firingRateSum;
    float firingRateSumSq;
    float rewardPerStepSum;
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastStepTime;
    double totalSimulationTime;
    
    // Peak values
    size_t peakActiveNeurons;
    size_t peakFiringNeurons;
    float peakFiringRate;
    
    Impl() : totalReward(0.0f), totalSpikes(0.0f), totalActiveNeurons(0), 
             totalFiringNeurons(0), curiosityLevel(0.0f), noveltyLevel(0.0f),
             dopamineLevel(0.0f), rewardSum(0.0f), rewardSumSq(0.0f),
             firingRateSum(0.0f), firingRateSumSq(0.0f), rewardPerStepSum(0.0f),
             totalSimulationTime(0.0), peakActiveNeurons(0), 
             peakFiringNeurons(0), peakFiringRate(0.0f) {
        startTime = std::chrono::high_resolution_clock::now();
        lastStepTime = startTime;
    }
};

ExperimentMetrics::ExperimentMetrics() : pImpl(new Impl) {
    initializeInternalStructures();
}

ExperimentMetrics::~ExperimentMetrics() = default;

ExperimentMetrics::ExperimentMetrics(const ExperimentMetrics& other) : pImpl(new Impl(*other.pImpl)) {}

ExperimentMetrics& ExperimentMetrics::operator=(const ExperimentMetrics& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

ExperimentMetrics::ExperimentMetrics(ExperimentMetrics&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ExperimentMetrics& ExperimentMetrics::operator=(ExperimentMetrics&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void ExperimentMetrics::recordStep(SimulationStep step, float reward, float firingRate,
                                   size_t activeNeurons, size_t firingNeurons,
                                   float totalSpikes, float curiosityLevel,
                                   float noveltyLevel, float dopamineLevel) {
    StepMetrics metrics;
    metrics.step = step;
    metrics.reward = reward;
    metrics.firingRate = firingRate;
    metrics.activeNeurons = activeNeurons;
    metrics.firingNeurons = firingNeurons;
    metrics.totalSpikes = totalSpikes;
    metrics.curiosityLevel = curiosityLevel;
    metrics.noveltyLevel = noveltyLevel;
    metrics.dopamineLevel = dopamineLevel;
    metrics.timestamp = std::chrono::high_resolution_clock::now();
    
    // Update aggregates
    pImpl->stepMetrics.push_back(metrics);
    pImpl->totalReward += reward;
    pImpl->totalSpikes += totalSpikes;
    pImpl->totalActiveNeurons += activeNeurons;
    pImpl->totalFiringNeurons += firingNeurons;
    
    // Update running statistics
    pImpl->rewardSum += reward;
    pImpl->rewardSumSq += reward * reward;
    pImpl->firingRateSum += firingRate;
    pImpl->firingRateSumSq += firingRate * firingRate;
    pImpl->rewardPerStepSum += reward;
    
    // Update neuromodulation levels (use latest)
    pImpl->curiosityLevel = curiosityLevel;
    pImpl->noveltyLevel = noveltyLevel;
    pImpl->dopamineLevel = dopamineLevel;
    
    // Update peaks
    if (activeNeurons > pImpl->peakActiveNeurons) {
        pImpl->peakActiveNeurons = activeNeurons;
    }
    if (firingNeurons > pImpl->peakFiringNeurons) {
        pImpl->peakFiringNeurons = firingNeurons;
    }
    if (firingRate > pImpl->peakFiringRate) {
        pImpl->peakFiringRate = firingRate;
    }
    
    // Update timing
    auto now = std::chrono::high_resolution_clock::now();
    pImpl->totalSimulationTime = std::chrono::duration<double>(now - pImpl->startTime).count();
    auto stepDuration = std::chrono::duration<double>(now - pImpl->lastStepTime).count();
    pImpl->lastStepTime = now;
    
    NLM_LOG_DEBUG("Recorded step " + std::to_string(step) + ": reward=" + 
                  std::to_string(reward) + ", firing_rate=" + std::to_string(firingRate));
}

void ExperimentMetrics::recordReward(float reward) {
    recordStep(getTotalSteps(), reward, getAverageFiringRate(),
               getTotalActiveNeurons(), getTotalFiringNeurons(), 0.0f);
}

void ExperimentMetrics::recordFiringRate(float firingRate) {
    recordStep(getTotalSteps(), getTotalReward(), firingRate,
               getTotalActiveNeurons(), getTotalFiringNeurons(), 0.0f);
}

void ExperimentMetrics::recordActiveNeurons(size_t count) {
    recordStep(getTotalSteps(), getTotalReward(), getAverageFiringRate(),
               count, getTotalFiringNeurons(), 0.0f);
}

void ExperimentMetrics::recordFiringNeurons(size_t count) {
    recordStep(getTotalSteps(), getTotalReward(), getAverageFiringRate(),
               getTotalActiveNeurons(), count, 0.0f);
}

void ExperimentMetrics::recordTotalSpikes(float spikes) {
    recordStep(getTotalSteps(), getTotalReward(), getAverageFiringRate(),
               getTotalActiveNeurons(), getTotalFiringNeurons(), spikes);
}

void ExperimentMetrics::recordNeuromodulation(float curiosity, float novelty, float dopamine) {
    recordStep(getTotalSteps(), getTotalReward(), getAverageFiringRate(),
               getTotalActiveNeurons(), getTotalFiringNeurons(), getTotalSpikes(),
               curiosity, novelty, dopamine);
}

void ExperimentMetrics::recordSteps(const std::vector<StepMetrics>& steps) {
    for (const auto& step : steps) {
        recordStep(step.step, step.reward, step.firingRate,
                  step.activeNeurons, step.firingNeurons, step.totalSpikes,
                  step.curiosityLevel, step.noveltyLevel, step.dopamineLevel);
    }
}

void ExperimentMetrics::recordEpisode(float totalReward, float avgFiringRate,
                                      size_t totalActiveNeurons, size_t totalFiringNeurons,
                                      float totalSpikes) {
    recordStep(getTotalSteps(), totalReward, avgFiringRate,
               totalActiveNeurons, totalFiringNeurons, totalSpikes);
}

float ExperimentMetrics::getTotalReward() const {
    return pImpl->totalReward;
}

float ExperimentMetrics::getAverageFiringRate() const {
    return pImpl->stepMetrics.empty() ? 0.0f : 
           pImpl->firingRateSum / pImpl->stepMetrics.size();
}

float ExperimentMetrics::getAverageRewardPerStep() const {
    return pImpl->stepMetrics.empty() ? 0.0f : 
           pImpl->rewardPerStepSum / pImpl->stepMetrics.size();
}

size_t ExperimentMetrics::getTotalSteps() const {
    return pImpl->stepMetrics.size();
}

size_t ExperimentMetrics::getTotalActiveNeurons() const {
    return pImpl->totalActiveNeurons;
}

size_t ExperimentMetrics::getTotalFiringNeurons() const {
    return pImpl->totalFiringNeurons;
}

float ExperimentMetrics::getTotalSpikes() const {
    return pImpl->totalSpikes;
}

float ExperimentMetrics::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

float ExperimentMetrics::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float ExperimentMetrics::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

ExperimentMetrics::StepMetrics ExperimentMetrics::getStepMetrics(SimulationStep step) const {
    for (const auto& metrics : pImpl->stepMetrics) {
        if (metrics.step == step) {
            return metrics;
        }
    }
    return StepMetrics();
}

const std::vector<ExperimentMetrics::StepMetrics>& ExperimentMetrics::getAllStepMetrics() const {
    return pImpl->stepMetrics;
}

float ExperimentMetrics::getRewardMean() const {
    if (pImpl->stepMetrics.empty()) return 0.0f;
    return pImpl->rewardSum / pImpl->stepMetrics.size();
}

float ExperimentMetrics::getRewardStdDev() const {
    if (pImpl->stepMetrics.size() < 2) return 0.0f;
    float mean = getRewardMean();
    float variance = pImpl->rewardSumSq / pImpl->stepMetrics.size() - mean * mean;
    return std::sqrt(std::max(0.0f, variance));
}

float ExperimentMetrics::getFiringRateMean() const {
    if (pImpl->stepMetrics.empty()) return 0.0f;
    return pImpl->firingRateSum / pImpl->stepMetrics.size();
}

float ExperimentMetrics::getFiringRateStdDev() const {
    if (pImpl->stepMetrics.size() < 2) return 0.0f;
    float mean = getFiringRateMean();
    float variance = pImpl->firingRateSumSq / pImpl->stepMetrics.size() - mean * mean;
    return std::sqrt(std::max(0.0f, variance));
}

float ExperimentMetrics::getRewardPerStepMean() const {
    if (pImpl->stepMetrics.empty()) return 0.0f;
    return pImpl->rewardPerStepSum / pImpl->stepMetrics.size();
}

double ExperimentMetrics::getAverageStepDuration() const {
    if (pImpl->stepMetrics.size() < 2) return 0.0;
    return pImpl->totalSimulationTime / pImpl->stepMetrics.size();
}

double ExperimentMetrics::getTotalSimulationTime() const {
    if (pImpl->stepMetrics.empty()) return 0.0;
    return pImpl->totalSimulationTime;
}

float ExperimentMetrics::getStepsPerSecond() const {
    return static_cast<float>(pImpl->stepMetrics.size() / std::max(1.0, pImpl->totalSimulationTime));
}

size_t ExperimentMetrics::getMemoryUsage() const {
    size_t memory = sizeof(Impl);
    for (const auto& step : pImpl->stepMetrics) {
        memory += sizeof(StepMetrics);
    }
    return memory;
}

size_t ExperimentMetrics::getMetricCount() const {
    return pImpl->stepMetrics.size();
}

ExperimentMetrics::MetricsSnapshot ExperimentMetrics::createSnapshot() const {
    MetricsSnapshot snapshot;
    snapshot.totalReward = getTotalReward();
    snapshot.avgFiringRate = getAverageFiringRate();
    snapshot.avgRewardPerStep = getAverageRewardPerStep();
    snapshot.stepsRecorded = getTotalSteps();
    snapshot.peakActiveNeurons = pImpl->peakActiveNeurons;
    snapshot.peakFiringNeurons = pImpl->peakFiringNeurons;
    snapshot.totalSpikes = getTotalSpikes();
    snapshot.curiosityLevel = getCuriosityLevel();
    snapshot.noveltyLevel = getNoveltyLevel();
    snapshot.dopamineLevel = getDopamineLevel();
    snapshot.simulationTime = getTotalSimulationTime();
    snapshot.throughput = getStepsPerSecond();
    return snapshot;
}

std::string ExperimentMetrics::MetricsSnapshot::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << "=== Experiment Metrics Snapshot ===\n";
    ss << "Total Reward: " << totalReward << "\n";
    ss << "Average Firing Rate: " << avgFiringRate << "\n";
    ss << "Average Reward Per Step: " << avgRewardPerStep << "\n";
    ss << "Steps Recorded: " << stepsRecorded << "\n";
    ss << "Peak Active Neurons: " << peakActiveNeurons << "\n";
    ss << "Peak Firing Neurons: " << peakFiringNeurons << "\n";
    ss << "Total Spikes: " << totalSpikes << "\n";
    ss << "Curiosity Level: " << curiosityLevel << "\n";
    ss << "Novelty Level: " << noveltyLevel << "\n";
    ss << "Dopamine Level: " << dopamineLevel << "\n";
    ss << "Simulation Time: " << simulationTime << " seconds\n";
    ss << "Throughput: " << throughput << " steps/second\n";
    
    return ss.str();
}

std::string ExperimentMetrics::getSummaryReport() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << "=== Experiment Metrics Summary ===\n";
    ss << "Total Reward: " << getTotalReward() << "\n";
    ss << "Average Reward Per Step: " << getAverageRewardPerStep() << "\n";
    ss << "Average Firing Rate: " << getAverageFiringRate() << "\n";
    ss << "Reward Mean: " << getRewardMean() << "\n";
    ss << "Reward Standard Deviation: " << getRewardStdDev() << "\n";
    ss << "Firing Rate Mean: " << getFiringRateMean() << "\n";
    ss << "Firing Rate Standard Deviation: " << getFiringRateStdDev() << "\n";
    ss << "Total Steps: " << getTotalSteps() << "\n";
    ss << "Peak Active Neurons: " << pImpl->peakActiveNeurons << "\n";
    ss << "Peak Firing Neurons: " << pImpl->peakFiringNeurons << "\n";
    ss << "Total Spikes: " << getTotalSpikes() << "\n";
    ss << "Curiosity Level: " << getCuriosityLevel() << "\n";
    ss << "Novelty Level: " << getNoveltyLevel() << "\n";
    ss << "Dopamine Level: " << getDopamineLevel() << "\n";
    ss << "Simulation Time: " << getTotalSimulationTime() << " seconds\n";
    ss << "Average Step Duration: " << getAverageStepDuration() << " seconds\n";
    ss << "Throughput: " << getStepsPerSecond() << " steps/second\n";
    
    return ss.str();
}

double ExperimentMetrics::compareWith(const ExperimentMetrics& other, const std::string& metricName) const {
    if (metricName == "total_reward") {
        return static_cast<double>(getTotalReward()) / static_cast<double>(other.getTotalReward());
    } else if (metricName == "avg_firing_rate") {
        return static_cast<double>(getAverageFiringRate()) / static_cast<double>(other.getAverageFiringRate());
    } else if (metricName == "reward_mean") {
        return static_cast<double>(getRewardMean()) / static_cast<double>(other.getRewardMean());
    }
    return 0.0;
}

void ExperimentMetrics::reset() {
    pImpl->stepMetrics.clear();
    pImpl->totalReward = 0.0f;
    pImpl->totalSpikes = 0.0f;
    pImpl->totalActiveNeurons = 0;
    pImpl->totalFiringNeurons = 0;
    pImpl->curiosityLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->dopamineLevel = 0.0f;
    pImpl->rewardSum = 0.0f;
    pImpl->rewardSumSq = 0.0f;
    pImpl->firingRateSum = 0.0f;
    pImpl->firingRateSumSq = 0.0f;
    pImpl->rewardPerStepSum = 0.0f;
    pImpl->peakActiveNeurons = 0;
    pImpl->peakFiringNeurons = 0;
    pImpl->peakFiringRate = 0.0f;
    
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->lastStepTime = pImpl->startTime;
    pImpl->totalSimulationTime = 0.0;
    
    NLM_LOG_INFO("ExperimentMetrics reset");
}

void ExperimentMetrics::clear() {
    reset();
}

bool ExperimentMetrics::exportToFile(const std::string& filepath) const {
    try {
        json j;
        
        // Export step metrics
        json stepMetrics = json::array();
        for (const auto& step : pImpl->stepMetrics) {
            json stepJson;
            stepJson["step"] = step.step;
            stepJson["reward"] = step.reward;
            stepJson["firing_rate"] = step.firingRate;
            stepJson["active_neurons"] = step.activeNeurons;
            stepJson["firing_neurons"] = step.firingNeurons;
            stepJson["total_spikes"] = step.totalSpikes;
            stepJson["curiosity_level"] = step.curiosityLevel;
            stepJson["novelty_level"] = step.noveltyLevel;
            stepJson["dopamine_level"] = step.dopamineLevel;
            stepJson["timestamp"] = std::chrono::duration<double>(step.timestamp - pImpl->startTime).count();
            stepMetrics.push_back(stepJson);
        }
        j["step_metrics"] = stepMetrics;
        
        // Export aggregate metrics
        json aggregates;
        aggregates["total_reward"] = pImpl->totalReward;
        aggregates["total_spikes"] = pImpl->totalSpikes;
        aggregates["total_active_neurons"] = pImpl->totalActiveNeurons;
        aggregates["total_firing_neurons"] = pImpl->totalFiringNeurons;
        aggregates["curiosity_level"] = pImpl->curiosityLevel;
        aggregates["novelty_level"] = pImpl->noveltyLevel;
        aggregates["dopamine_level"] = pImpl->dopamineLevel;
        aggregates["peak_active_neurons"] = pImpl->peakActiveNeurons;
        aggregates["peak_firing_neurons"] = pImpl->peakFiringNeurons;
        aggregates["peak_firing_rate"] = pImpl->peakFiringRate;
        aggregates["total_simulation_time"] = pImpl->totalSimulationTime;
        
        j["aggregates"] = aggregates;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for writing: " + filepath);
        }
        
        file << j.dump(4);
        file.close();
        
        NLM_LOG_INFO("ExperimentMetrics exported to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export ExperimentMetrics: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentMetrics::importFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw NLMRuntimeError("Cannot open file for reading: " + filepath);
        }
        
        json j;
        file >> j;
        file.close();
        
        // Clear existing metrics
        clear();
        
        // Import step metrics
        if (j.contains("step_metrics")) {
            for (const auto& stepJson : j["step_metrics"]) {
                StepMetrics step;
                step.step = stepJson.value("step", 0);
                step.reward = stepJson.value("reward", 0.0f);
                step.firingRate = stepJson.value("firing_rate", 0.0f);
                step.activeNeurons = stepJson.value("active_neurons", 0);
                step.firingNeurons = stepJson.value("firing_neurons", 0);
                step.totalSpikes = stepJson.value("total_spikes", 0.0f);
                step.curiosityLevel = stepJson.value("curiosity_level", 0.0f);
                step.noveltyLevel = stepJson.value("novelty_level", 0.0f);
                step.dopamineLevel = stepJson.value("dopamine_level", 0.0f);
                
                // Reconstruct timestamp
                double timestamp = stepJson.value("timestamp", 0.0);
                step.timestamp = pImpl->startTime + std::chrono::duration<double>(timestamp);
                
                pImpl->stepMetrics.push_back(step);
                
                // Update aggregates
                pImpl->totalReward += step.reward;
                pImpl->totalSpikes += step.totalSpikes;
                pImpl->totalActiveNeurons += step.activeNeurons;
                pImpl->totalFiringNeurons += step.firingNeurons;
                
                // Update statistics
                pImpl->rewardSum += step.reward;
                pImpl->rewardSumSq += step.reward * step.reward;
                pImpl->firingRateSum += step.firingRate;
                pImpl->firingRateSumSq += step.firingRate * step.firingRate;
                pImpl->rewardPerStepSum += step.reward;
                
                // Update peaks
                if (step.activeNeurons > pImpl->peakActiveNeurons) {
                    pImpl->peakActiveNeurons = step.activeNeurons;
                }
                if (step.firingNeurons > pImpl->peakFiringNeurons) {
                    pImpl->peakFiringNeurons = step.firingNeurons;
                }
                if (step.firingRate > pImpl->peakFiringRate) {
                    pImpl->peakFiringRate = step.firingRate;
                }
            }
        }
        
        // Import aggregate metrics
        if (j.contains("aggregates")) {
            const auto& aggregates = j["aggregates"];
            pImpl->curiosityLevel = aggregates.value("curiosity_level", 0.0f);
            pImpl->noveltyLevel = aggregates.value("novelty_level", 0.0f);
            pImpl->dopamineLevel = aggregates.value("dopamine_level", 0.0f);
        }
        
        NLM_LOG_INFO("ExperimentMetrics imported from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import ExperimentMetrics: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentMetrics::isValid() const {
    if (pImpl->stepMetrics.empty()) {
        NLM_LOG_WARNING("No step metrics recorded");
        return true; // Empty is valid
    }
    
    if (pImpl->totalReward < 0.0f) {
        NLM_LOG_ERROR("Total reward cannot be negative");
        return false;
    }
    
    return true;
}

std::string ExperimentMetrics::validateAndGetError() const {
    if (pImpl->stepMetrics.empty()) {
        return "No step metrics recorded";
    }
    
    if (pImpl->totalReward < 0.0f) {
        return "Total reward cannot be negative";
    }
    
    return "Metrics are valid";
}

ExperimentMetrics::StepIterator ExperimentMetrics::begin() const {
    return pImpl->stepMetrics.begin();
}

ExperimentMetrics::StepIterator ExperimentMetrics::end() const {
    return pImpl->stepMetrics.end();
}

void ExperimentMetrics::initializeInternalStructures() {
    // Initialize with default values
    pImpl->totalReward = 0.0f;
    pImpl->totalSpikes = 0.0f;
    pImpl->totalActiveNeurons = 0;
    pImpl->totalFiringNeurons = 0;
    pImpl->curiosityLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->dopamineLevel = 0.0f;
    pImpl->rewardSum = 0.0f;
    pImpl->rewardSumSq = 0.0f;
    pImpl->firingRateSum = 0.0f;
    pImpl->firingRateSumSq = 0.0f;
    pImpl->rewardPerStepSum = 0.0f;
    pImpl->peakActiveNeurons = 0;
    pImpl->peakFiringNeurons = 0;
    pImpl->peakFiringRate = 0.0f;
    
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->lastStepTime = pImpl->startTime;
    pImpl->totalSimulationTime = 0.0;
}

void ExperimentMetrics::updateDerivedMetrics() {
    // Update any derived metrics as needed
}

void ExperimentMetrics::validateMetricsConsistency() const {
    // Check for consistency issues
    if (pImpl->stepMetrics.empty()) {
        return;
    }
    
    // Check that total aggregates match step metrics
    float calculatedTotalReward = 0.0f;
    for (const auto& step : pImpl->stepMetrics) {
        calculatedTotalReward += step.reward;
    }
    
    if (std::abs(calculatedTotalReward - pImpl->totalReward) > 0.001f) {
        NLM_LOG_WARNING("Reward aggregation inconsistency detected");
    }
}

} // namespace nlm
