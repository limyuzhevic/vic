#include "ExperimentRunner.hpp"

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // PLACEHOLDER: In real implementation, would clone from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

#include "ExperimentRunner.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

namespace nlm {

// ExperimentResult implementation
std::string ExperimentResult::toJSON() const {
    json j;
    j["experiment_name"] = experimentName;
    j["seed"] = seed;
    j["start_step"] = startStep;
    j["end_step"] = endStep;
    j["duration"] = duration;
    j["total_neurons"] = totalNeurons;
    j["total_synapses"] = totalSynapses;
    j["max_firing_neurons"] = maxFiringNeurons;
    j["min_firing_rate"] = minFiringRate;
    j["max_firing_rate"] = maxFiringRate;
    j["avg_firing_rate"] = avgFiringRate;
    j["total_time"] = totalTime;
    j["total_steps"] = totalSteps;
    j["successful_steps"] = successfulSteps;
    j["learning_detected"] = learningDetected;
    j["learning_magnitude"] = learningMagnitude;
    j["total_spikes"] = totalSpikes;
    j["max_spikes_per_step"] = maxSpikesPerStep;
    j["spike_rate"] = spikeRate;
    j["success"] = success;
    
    if (!errorMessage.empty()) j["error_message"] = errorMessage;
    
    if (!initialWeights.empty()) {
        j["initial_weights"] = initialWeights;
    }
    if (!finalWeights.empty()) {
        j["final_weights"] = finalWeights;
    }
    
    j["weight_change_mean"] = weightChangeMean;
    j["weight_change_std"] = weightChangeStd;
    j["weights_strengthened"] = weightsStrengthened;
    j["weights_weakened"] = weightsWeakened;
    j["weights_unchanged"] = weightsUnchanged;
    
    return j.dump(2);
}

std::string ExperimentResult::toCSV() const {
    std::ostringstream oss;
    oss << "\"" << experimentName << "\""."""";
    oss << seed << "."""";
    oss << startStep << "."""";
    oss << endStep << "."""";
    oss << duration << "."""";
    oss << totalNeurons << "."""";
    oss << totalSynapses << "."""";
    oss << maxFiringNeurons << "."""";
    oss << minFiringRate << "."""";
    oss << maxFiringRate << "."""";
    oss << avgFiringRate << "."""";
    oss << totalTime << "."""";
    oss << totalSteps << "."""";
    oss << successfulSteps << "."""";
    oss << (learningDetected ? "true" : "false") << "."""";
    oss << learningMagnitude << "."""";
    oss << totalSpikes << "."""";
    oss << maxSpikesPerStep << "."""";
    oss << spikeRate << "."""";
    oss << (success ? "true" : "false") << "."""";
    
    if (!errorMessage.empty()) {
        oss << "\"" << errorMessage << "\"";
    }
    
    return oss.str();
}

std::string ExperimentResult::toTextReport() const {
    std::ostringstream oss;
    oss << "=== Experiment Result ===" << std::endl;
    oss << "Name: " << experimentName << std::endl;
    oss << "Seed: " << seed << std::endl;
    oss << "Duration: " << duration << " steps" << std::endl;
    oss << "Neurons: " << totalNeurons << ", Synapses: " << totalSynapses << std::endl;
    oss << "Firing Rate: " << avgFiringRate << " Hz (min: " << minFiringRate << ", max: " << maxFiringRate << ")" << std::endl;
    oss << "Total Spikes: " << totalSpikes << ", Spike Rate: " << spikeRate << " Hz" << std::endl;
    oss << "Learning Detected: " << (learningDetected ? "Yes" : "No");
    if (learningDetected) {
        oss << " (magnitude: " << learningMagnitude << ")";
    }
    oss << std::endl;
    oss << "Success: " << (success ? "Yes" : "No");
    if (!success && !errorMessage.empty()) {
        oss << " - " << errorMessage;
    }
    oss << std::endl;
    oss << "========================" << std::endl;
    
    return oss.str();
}

double ExperimentResult::getCoefficientOfVariation() const {
    if (avgFiringRate == 0) return 0.0;
    return (maxFiringRate - minFiringRate) / avgFiringRate;
}

bool ExperimentResult::isWithinNormalRange(float threshold) const {
    return (avgFiringRate > threshold) && (weightChangeMean < threshold);
}

std::string ExperimentResult::getPerformanceGrade() const {
    double score = (avgFiringRate * 0.4) + (learningMagnitude * 0.3) + (spikeRate * 0.3);
    
    if (score >= 0.8) return "A+ (Excellent)";
    if (score >= 0.7) return "A (Very Good)";
    if (score >= 0.6) return "B+ (Good)";
    if (score >= 0.5) return "B (Average)";
    if (score >= 0.4) return "C+ (Below Average)";
    if (score >= 0.3) return "C (Poor)";
    if (score >= 0.2) return "D (Very Poor)";
    return "F (Failed)";
}

std::string ExperimentComparison::getComparisonReport() const {
    std::ostringstream oss;
    oss << "=== Experiment Comparison ===" << std::endl;
    oss << "Experiments compared: " << experimentNames.size() << std::endl;
    oss << "Best experiment: " << bestExperiment << std::endl;
    oss << "Worst experiment: " << worstExperiment << std::endl;
    oss << std::endl;
    oss << "Metric rankings:" << std::endl;
    for (const auto& pair : metricRanks) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    oss << "============================" << std::endl;
    
    return oss.str();
}

// ExperimentRunner implementation
struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    std::map<std::string, std::string> experimentMetadata;
    
    Impl() : globalSeed(42) {}
};

ExperimentRunner::ExperimentRunner() : pImpl(std::make_unique<Impl>()) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

ExperimentResult ExperimentRunner::runExperimentAndCollectResults(
    std::shared_ptr<Experiment> experiment,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    SimulationStep maxSteps) {
    
    ExperimentResult result;
    result.experimentName = experiment->getName();
    result.seed = experiment->getSeed();
    result.startTime = std::chrono::system_clock::now();
    
    try {
        if (!runExperiment(experiment, brain, environment, maxSteps)) {
            result.success = false;
            result.errorMessage = "Experiment failed to run";
        } else {
            result.success = true;
            
            // Collect comprehensive results
            result.endTime = std::chrono::system_clock::now();
            result.duration = std::chrono::duration<double>(
                result.endTime - result.startTime).count();
            
            result.totalSteps = experiment->getEndStep() - experiment->getStartStep();
            
            // Get neural metrics from experiment step data
            const auto& stepData = experiment->getStepData();
            if (!stepData.empty()) {
                result.totalNeurons = stepData.back().neuronCount;
                result.totalSynapses = stepData.back().synapseCount;
                result.maxFiringNeurons = stepData.back().firingCount;
                result.minFiringRate = stepData.back().avgFiringRate;
                result.maxFiringRate = stepData.back().avgFiringRate;
                result.avgFiringRate = stepData.back().avgFiringRate;
                
                for (const auto& step : stepData) {
                    if (step.avgFiringRate > result.maxFiringRate) {
                        result.maxFiringRate = step.avgFiringRate;
                    }
                    if (step.avgFiringRate < result.minFiringRate) {
                        result.minFiringRate = step.avgFiringRate;
                    }
                    result.totalSpikes += step.firingCount;
                }
                
                if (!stepData.empty()) {
                    result.spikeRate = static_cast<float>(result.totalSpikes) / result.totalSteps;
                }
            }
            
            // Analyze plasticity
            const auto& metrics = experiment->getMetrics();
            for (const auto& metric : metrics) {
                if (metric.first == "weight_change_mean") {
                    result.weightChangeMean = metric.second;
                } else if (metric.first == "weight_change_std") {
                    result.weightChangeStd = metric.second;
                }
            }
            
            // Determine learning
            result.learningDetected = (result.weightChangeMean > 0.001f);
            result.learningMagnitude = std::abs(result.weightChangeMean);
            
            // Determine success based on criteria
            result.success = (result.learningDetected || result.totalSpikes > 0) && !experiment->hasMetric("error");
            
            if (result.success) {
                result.errorMessage = "";
            } else {
                result.errorMessage = "Experiment did not meet success criteria";
            }
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    return result;
}

std::vector<ExperimentResult> ExperimentRunner::runBatchAndCollectResults(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<ExperimentResult> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        auto environment = std::make_shared<Environment>(*environmentTemplate.get());
        
        ExperimentResult result = runExperimentAndCollectResults(exp, brain, environment, maxSteps);
        results.push_back(result);
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath, const std::vector<ExperimentResult>& results) const {
    return saveResultsJSON(filepath, results);
}

bool ExperimentRunner::saveResultsJSON(const std::string& filepath, const std::vector<ExperimentResult>& results) const {
    json j;
    j["experiments"] = json::array();
    j["timestamp"] = std::chrono::system_clock::now();
    j["total_experiments"] = results.size();
    
    for (const auto& result : results) {
        j["experiments"].push_back(json::parse(result.toJSON()));
    }
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << std::setw(2) << j << std::endl;
        return true;
    }
    return false;
}

bool ExperimentRunner::saveResultsCSV(const std::string& filepath, const std::vector<ExperimentResult>& results) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "experiment_name,seed,start_step,end_step,duration,total_neurons,total_synapses," << std::endl
         << "max_firing_neurons,min_firing_rate,max_firing_rate,avg_firing_rate,total_time,total_steps," << std::endl
         << "successful_steps,learning_detected,learning_magnitude,total_spikes,max_spikes_per_step,spike_rate,success" << std::endl;
    
    // Write data
    for (const auto& result : results) {
        file << "\"" << result.experimentName << "\""."""";
        file << result.seed << "."""";
        file << result.startStep << "."""";
        file << result.endStep << "."""";
        file << result.duration << "."""";
        file << result.totalNeurons << "."""";
        file << result.totalSynapses << "."""";
        file << result.maxFiringNeurons << "."""";
        file << result.minFiringRate << "."""";
        file << result.maxFiringRate << "."""";
        file << result.avgFiringRate << "."""";
        file << result.totalTime << "."""";
        file << result.totalSteps << "."""";
        file << result.successfulSteps << "."""";
        file << (result.learningDetected ? "true" : "false") << "."""";
        file << result.learningMagnitude << "."""";
        file << result.totalSpikes << "."""";
        file << result.maxSpikesPerStep << "."""";
        file << result.spikeRate << "."""";
        file << (result.success ? "true" : "false") << "."""";
        file << std::endl;
    }
    
    return true;
}

bool ExperimentRunner::saveResultsText(const std::string& filepath, const std::vector<ExperimentResult>& results) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Comprehensive Experiment Report ===" << std::endl;
    file << "Generated: " << std::chrono::system_clock::now() << std::endl;
    file << "Total Experiments: " << results.size() << std::endl;
    file << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << results[i].toTextReport();
        if (i < results.size() - 1) {
            file << "---" << std::endl;
        }
    }
    
    return true;
}

std::vector<ExperimentResult> ExperimentRunner::loadResults(const std::string& filepath) const {
    std::vector<ExperimentResult> results;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return results;
    }
    
    json j;
    try {
        file >> j;
        
        for (const auto& expJson : j["experiments"]) {
            ExperimentResult result;
            result.experimentName = expJson["experiment_name"];
            result.seed = expJson["seed"];
            result.startStep = expJson["start_step"];
            result.endStep = expJson["end_step"];
            result.duration = expJson["duration"];
            result.totalNeurons = expJson["total_neurons"];
            result.totalSynapses = expJson["total_synapses"];
            result.maxFiringNeurons = expJson["max_firing_neurons"];
            result.minFiringRate = expJson["min_firing_rate"];
            result.maxFiringRate = expJson["max_firing_rate"];
            result.avgFiringRate = expJson["avg_firing_rate"];
            result.totalTime = expJson["total_time"];
            result.totalSteps = expJson["total_steps"];
            result.successfulSteps = expJson["successful_steps"];
            result.learningDetected = expJson["learning_detected"];
            result.learningMagnitude = expJson["learning_magnitude"];
            result.totalSpikes = expJson["total_spikes"];
            result.maxSpikesPerStep = expJson["max_spikes_per_step"];
            result.spikeRate = expJson["spike_rate"];
            result.success = expJson["success"];
            
            if (expJson.contains("error_message")) {
                result.errorMessage = expJson["error_message"];
            }
            
            results.push_back(result);
        }
    } catch (const std::exception& e) {
        // If parsing fails, return empty results
        return results;
    }
    
    return results;
}

ExperimentResult ExperimentRunner::analyzeExperiment(std::shared_ptr<Experiment> experiment) const {
    ExperimentResult result;
    result.experimentName = experiment->getName();
    result.seed = experiment->getSeed();
    
    // Analyze step data
    const auto& stepData = experiment->getStepData();
    if (!stepData.empty()) {
        result.totalSteps = stepData.back().step;
        
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t maxFiringNeurons = 0;
        float sumFiringRates = 0.0f;
        
        for (const auto& step : stepData) {
            totalNeurons += step.neuronCount;
            totalSynapses += step.synapseCount;
            maxFiringNeurons += step.firingCount;
            sumFiringRates += step.avgFiringRate;
        }
        
        result.totalNeurons = totalNeurons / stepData.size();
        result.totalSynapses = totalSynapses / stepData.size();
        result.maxFiringNeurons = maxFiringNeurons / stepData.size();
        result.avgFiringRate = sumFiringRates / stepData.size();
    }
    
    // Analyze metrics
    const auto& metrics = experiment->getMetrics();
    for (const auto& metric : metrics) {
        if (metric.first == "weight_change_mean") {
            result.weightChangeMean = metric.second;
            result.learningDetected = (std::abs(metric.second) > 0.001f);
            result.learningMagnitude = std::abs(metric.second);
        }
    }
    
    result.success = !experiment->hasMetric("error");
    
    return result;
}

ExperimentComparison ExperimentRunner::compareExperiments(const std::vector<std::shared_ptr<Experiment>>& experiments) const {
    ExperimentComparison comparison;
    
    for (auto& exp : experiments) {
        comparison.experimentNames.push_back(exp->getName());
    }
    
    // For demonstration, use simple metrics
    // In a real implementation, this would be more sophisticated
    for (size_t i = 0; i < experiments.size(); ++i) {
        const auto& exp = experiments[i];
        std::string name = exp->getName();
        
        // Simple scoring based on experiment name length (placeholder)
        double score = static_cast<double>(name.length()) / 10.0;
        comparison.metricValues[name] = score;
        comparison.metricRanks[name] = score;
    }
    
    if (!comparison.experimentNames.empty()) {
        comparison.bestExperiment = comparison.experimentNames[0];
        comparison.worstExperiment = comparison.experimentNames.back();
    }
    
    return comparison;
}

double ExperimentRunner::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double ExperimentRunner::calculateStd(const std::vector<double>& values, double mean) const {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& val : values) {
        sum += std::pow(val - mean, 2);
    }
    return std::sqrt(sum / values.size());
}

double ExperimentRunner::calculateCoefficientOfVariation(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double mean = calculateMean(values);
    if (mean == 0.0) return 0.0;
    double std = calculateStd(values, mean);
    return (std / mean) * 100.0;  // As percentage
}

std::vector<double> ExperimentRunner::normalizeMetrics(const std::map<std::string, double>& metrics) const {
    std::vector<double> normalizedValues;
    
    for (const auto& pair : metrics) {
        normalizedValues.push_back(pair.second);
    }
    
    if (normalizedValues.empty()) return normalizedValues;
    
    double minVal = *std::min_element(normalizedValues.begin(), normalizedValues.end());
    double maxVal = *std::max_element(normalizedValues.begin(), normalizedValues.end());
    
    if (maxVal == minVal) return std::vector<double>(normalizedValues.size(), 0.5);
    
    std::vector<double> normalized;
    for (const auto& val : normalizedValues) {
        normalized.push_back((val - minVal) / (maxVal - minVal));
    }
    
    return normalized;
}

double ExperimentRunner::calculatePerformanceScore(const ExperimentResult& result) const {
    double score = 0.0;
    
    // Firing rate component (0-40%)
    score += (result.avgFiringRate / 100.0) * 0.4;
    
    // Learning component (0-30%)
    score += result.learningMagnitude * 0.3;
    
    // Spike activity component (0-20%)
    score += (result.spikeRate / 10.0) * 0.2;
    
    // Success component (0-10%)
    if (result.success) score += 0.1;
    
    return std::min(score, 1.0);  // Cap at 1.0
}

std::string ExperimentRunner::assessExperimentQuality(const ExperimentResult& result) const {
    double score = calculatePerformanceScore(result);
    
    if (score >= 0.9) return "Excellent";
    if (score >= 0.8) return "Very Good";
    if (score >= 0.7) return "Good";
    if (score >= 0.6) return "Average";
    if (score >= 0.5) return "Below Average";
    if (score >= 0.4) return "Poor";
    return "Very Poor";
}

bool ExperimentRunner::validateExperimentResult(const ExperimentResult& result) const {
    if (result.experimentName.empty()) return false;
    if (result.totalSteps == 0) return false;
    if (result.avgFiringRate < 0.0 || result.avgFiringRate > 100.0) return false;
    if (result.weightChangeMean < -10.0 || result.weightChangeMean > 10.0) return false;
    return true;
}

std::vector<std::string> ExperimentRunner::getValidationErrors(const ExperimentResult& result) const {
    std::vector<std::string> errors;
    
    if (result.experimentName.empty()) {
        errors.push_back("Experiment name is empty");
    }
    
    if (result.totalSteps == 0) {
        errors.push_back("No steps recorded");
    }
    
    if (result.avgFiringRate < 0.0 || result.avgFiringRate > 100.0) {
        errors.push_back("Firing rate out of expected range");
    }
    
    if (result.weightChangeMean < -10.0 || result.weightChangeMean > 10.0) {
        errors.push_back("Weight change magnitude is unrealistic");
    }
    
    return errors;
}

std::string ExperimentRunner::generateComprehensiveReport(const std::vector<ExperimentResult>& results) const {
    std::ostringstream oss;
    oss << "=== Comprehensive Experiment Analysis Report ===" << std::endl;
    oss << "Generated: " << std::chrono::system_clock::now() << std::endl;
    oss << "Total Experiments: " << results.size() << std::endl;
    oss << std::endl;
    
    oss << "=== Summary Statistics ===" << std::endl;
    oss << "Average Performance: " << getAveragePerformance(results) << std::endl;
    oss << "Successful Experiments: " << getSuccessfulExperiments(results) << std::endl;
    oss << std::endl;
    
    oss << "=== Detailed Results ===" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "Experiment " << i+1 << ": " << result.experimentName << std::endl;
        oss << "  Performance Grade: " << result.getPerformanceGrade() << std::endl;
        oss << "  Quality Assessment: " << assessExperimentQuality(result) << std::endl;
        oss << "  Learning Detected: " << (result.learningDetected ? "Yes" : "No");
        if (result.learningDetected) oss << " (magnitude: " << result.learningMagnitude << ")";
        oss << std::endl;
        oss << "  Success: " << (result.success ? "Yes" : "No");
        if (!result.success && !result.errorMessage.empty()) {
            oss << " - " << result.errorMessage;
        }
        oss << std::endl;
        if (i < results.size() - 1) oss << std::endl;
    }
    
    oss << "========================" << std::endl;
    
    return oss.str();
}

std::string ExperimentRunner::generateStatisticalReport(const std::vector<ExperimentResult>>& results) const {
    std::ostringstream oss;
    oss << "=== Statistical Analysis Report ===" << std::endl;
    oss << "Generated: " << std::chrono::system_clock::now() << std::endl;
    oss << "Number of experiments: " << results.size() << std::endl;
    oss << std::endl;
    
    if (results.empty()) {
        oss << "No results to analyze." << std::endl;
        return oss.str();
    }
    
    // Collect metrics for analysis
    std::vector<double> firingRates, weightChanges, spikeRates, learningMagnitudes;
    
    for (const auto& result : results) {
        firingRates.push_back(result.avgFiringRate);
        weightChanges.push_back(result.weightChangeMean);
        spikeRates.push_back(result.spikeRate);
        learningMagnitudes.push_back(result.learningMagnitude);
    }
    
    oss << "=== Firing Rate Analysis ===" << std::endl;
    oss << "Mean: " << calculateMean(firingRates) << " Hz" << std::endl;
    oss << "Std: " << calculateStd(firingRates, calculateMean(firingRates)) << " Hz" << std::endl;
    oss << "Coefficient of Variation: " << calculateCoefficientOfVariation(firingRates) << "%" << std::endl;
    oss << "Min: " << *std::min_element(firingRates.begin(), firingRates.end()) << " Hz" << std::endl;
    oss << "Max: " << *std::max_element(firingRates.begin(), firingRates.end()) << " Hz" << std::endl;
    oss << std::endl;
    
    oss << "=== Weight Change Analysis ===" << std::endl;
    oss << "Mean: " << calculateMean(weightChanges) << std::endl;
    oss << "Std: " << calculateStd(weightChanges, calculateMean(weightChanges)) << std::endl;
    oss << "Coefficient of Variation: " << calculateCoefficientOfVariation(weightChanges) << "%" << std::endl;
    oss << "Learning Detected: " << (calculateMean(learningMagnitudes) > 0.001 ? "Yes" : "No") << std::endl;
    oss << std::endl;
    
    oss << "=== Spike Rate Analysis ===" << std::endl;
    oss << "Mean: " << calculateMean(spikeRates) << " Hz" << std::endl;
    oss << "Std: " << calculateStd(spikeRates, calculateMean(spikeRates)) << " Hz" << std::endl;
    oss << "Coefficient of Variation: " << calculateCoefficientOfVariation(spikeRates) << "%" << std::endl;
    oss << std::endl;
    
    oss << "=== Learning Analysis ===" << std::endl;
    oss << "Average Learning Magnitude: " << calculateMean(learningMagnitudes) << std::endl;
    oss << "Learning Detected in " << std::count_if(learningMagnitudes.begin(), learningMagnitudes.end(),
                                                  [](double x) { return x > 0.001; }) << " experiments" << std::endl;
    oss << std::endl;
    
    oss << "==============================" << std::endl;
    
    return oss.str();
}

std::string ExperimentRunner::generateComparisonReport(const ExperimentComparison& comparison) const {
    return comparison.getComparisonReport();
}

size_t ExperimentRunner::getTotalExperiments() const {
    return pImpl->experiments.size();
}

double ExperimentRunner::getAveragePerformance(const std::vector<ExperimentResult>& results) const {
    if (results.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& result : results) {
        sum += calculatePerformanceScore(result);
    }
    return sum / results.size();
}

size_t ExperimentRunner::getSuccessfulExperiments(const std::vector<ExperimentResult>& results) const {
    return std::count_if(results.begin(), results.end(),
                        [](const ExperimentResult& r) { return r.success; });
}

std::vector<ExperimentResult> ExperimentRunner::filterResultsByPerformance(
    const std::vector<ExperimentResult>& results,
    double minScore,
    double maxScore) const {
    std::vector<ExperimentResult> filtered;
    
    for (const auto& result : results) {
        double score = calculatePerformanceScore(result);
        if (score >= minScore && score <= maxScore) {
            filtered.push_back(result);
        }
    }
    
    return filtered;
}

std::vector<ExperimentResult> ExperimentRunner::filterResultsByLearning(
    const std::vector<ExperimentResult>& results,
    bool learningDetected,
    float minLearningMagnitude) const {
    std::vector<ExperimentResult> filtered;
    
    for (const auto& result : results) {
        if (result.learningDetected == learningDetected && 
            result.learningMagnitude >= minLearningMagnitude) {
            filtered.push_back(result);
        }
    }
    
    return filtered;
}

std::vector<ExperimentResult> ExperimentRunner::filterResultsByDomain(
    const std::vector<ExperimentResult>& results,
    const std::string& domain) const {
    // Note: This is a placeholder implementation since ExperimentResult doesn't have domain information
    // In a real implementation, you would add domain information to ExperimentResult
    return results;
}

} // namespace nlm
