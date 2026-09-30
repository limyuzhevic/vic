#include "NeuromodulationExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

NeuromodulationExperiment::NeuromodulationExperiment()
    : currentDopamineLevel(0.5f), currentSerotoninLevel(0.3f),
      currentNorepinephrineLevel(0.4f), currentAcetylcholineLevel(0.6f),
      rng(std::random_device()()), rewardDist(0.0, 10.0),
      noveltyDist(0.0, 1.0), neuromodulatorDist(0.0, 1.0) {
    setName("Neuromodulation Experiment");
    setDescription("Tests neuromodulation and learning signals with dopamine, serotonin, and other neuromodulators");
}

bool NeuromodulationExperiment::initialize() {
    log("Initializing Neuromodulation Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    // Initialize neuromodulation state
    currentDopamineLevel = config.dopamineLevel;
    currentSerotoninLevel = config.serotoninLevel;
    currentNorepinephrineLevel = config.norepinephrineLevel;
    currentAcetylcholineLevel = config.acetylcholineLevel;
    
    neuromodulationEvents.clear();
    learningSignals.clear();
    
    log("Neuromodulation Experiment initialized successfully");
    log("  - Initial Dopamine: " + std::to_string(currentDopamineLevel));
    log("  - Initial Serotonin: " + std::to_string(currentSerotoninLevel));
    log("  - Initial Norepinephrine: " + std::to_string(currentNorepinephrineLevel));
    log("  - Initial Acetylcholine: " + std::to_string(currentAcetylcholineLevel));
    
    return true;
}

void NeuromodulationExperiment::setup() {
    log("Setting up Neuromodulation Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Reward Prediction Error: " + std::to_string(config.rewardPredictionError));
    log("  - Novelty Signal: " + std::to_string(config.noveltySignal));
    log("  - Curiosity Signal: " + std::to_string(config.curiositySignal));
    log("  - Uncertainty: " + std::to_string(config.uncertainty));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void NeuromodulationExperiment::run() {
    log("Starting Neuromodulation Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runNeuromodulationTrial(trial);
        
        // Update progress
        double progress = static_cast<double>(trial + 1) / config.numTrials;
        reportProgress(progress, "Running trial " + std::to_string(trial + 1));
        
        // Check time limit
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        if (elapsed > config.timeLimit) {
            log("Time limit reached, stopping experiment");
            break;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalTime = std::chrono::duration<double>(endTime - startTime).count();
    log("Neuromodulation Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void NeuromodulationExperiment::analyzeResults() {
    log("Analyzing Neuromodulation Experiment Results");
    
    if (learningSignals.empty()) {
        log("ERROR: No learning signals to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalRPE = 0.0;
    double totalNovelty = 0.0;
    double totalCuriosity = 0.0;
    double totalUncertainty = 0.0;
    double totalLearningRate = 0.0;
    
    for (const auto& signal : learningSignals) {
        totalRPE += signal.rewardPredictionError;
        totalNovelty += signal.novelty;
        totalCuriosity += signal.curiosity;
        totalUncertainty += signal.uncertainty;
        totalLearningRate += signal.learningRate;
    }
    
    size_t numSignals = learningSignals.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_reward_prediction_error", totalRPE);
    results.recordMetric("average_reward_prediction_error", totalRPE / numSignals);
    results.recordMetric("average_novelty", totalNovelty / numSignals);
    results.recordMetric("average_curiosity", totalCuriosity / numSignals);
    results.recordMetric("average_uncertainty", totalUncertainty / numSignals);
    results.recordMetric("average_learning_rate", totalLearningRate / numSignals);
    
    // Calculate neuromodulation effects
    double dopamineEffect = calculateNeuromodulationEffect("dopamine");
    double serotoninEffect = calculateNeuromodulationEffect("serotonin");
    double norepinephrineEffect = calculateNeuromodulationEffect("norepinephrine");
    double acetylcholineEffect = calculateNeuromodulationEffect("acetylcholine");
    
    results.recordMetric("dopamine_effect", dopamineEffect);
    results.recordMetric("serotonin_effect", serotoninEffect);
    results.recordMetric("norepinephrine_effect", norepinephrineEffect);
    results.recordMetric("acetylcholine_effect", acetylcholineEffect);
    
    // Calculate learning efficiency
    double learningEfficiency = calculateLearningEfficiency();
    results.recordMetric("learning_efficiency", learningEfficiency);
    
    // Record results
    recordResults(results);
    
    log("Neuromodulation Experiment analysis complete");
    log("  - Average Reward Prediction Error: " + std::to_string(totalRPE / numSignals));
    log("  - Average Novelty: " + std::to_string(totalNovelty / numSignals));
    log("  - Average Curiosity: " + std::to_string(totalCuriosity / numSignals));
    log("  - Learning Efficiency: " + std::to_string(learningEfficiency));
}

void NeuromodulationExperiment::cleanup() {
    log("Cleaning up Neuromodulation Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    neuromodulationEvents.clear();
    learningSignals.clear();
    
    log("Neuromodulation Experiment cleanup complete");
}

void NeuromodulationExperiment::setNeuromodulationConfig(const NeuromodulationConfig& config) {
    this->config = config;
    currentDopamineLevel = config.dopamineLevel;
    currentSerotoninLevel = config.serotoninLevel;
    currentNorepinephrineLevel = config.norepinephrineLevel;
    currentAcetylcholineLevel = config.acetylcholineLevel;
}

const NeuromodulationExperiment::NeuromodulationConfig& NeuromodulationExperiment::getNeuromodulationConfig() const {
    return config;
}

void NeuromodulationExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void NeuromodulationExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void NeuromodulationExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void NeuromodulationExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void NeuromodulationExperiment::runNeuromodulationTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Generate learning signals
    LearningSignal signal;
    signal.step = trialNumber;
    signal.rewardPredictionError = rewardDist(rng);
    signal.novelty = noveltyDist(rng);
    signal.curiosity = signal.novelty * 0.5 + signal.rewardPredictionError * 0.3;
    signal.uncertainty = std::abs(signal.rewardPredictionError - signal.novelty) * 0.5;
    signal.learningRate = config.dopamineLearningRate * signal.rewardPredictionError;
    
    // Update neuromodulators based on learning signals
    updateNeuromodulators(signal);
    
    // Apply neuromodulation effects
    applyNeuromodulationEffects();
    
    // Record neuromodulation events
    recordNeuromodulationEvent("dopamine", currentDopamineLevel, "reward_prediction_error");
    recordNeuromodulationEvent("serotonin", currentSerotoninLevel, "novelty");
    recordNeuromodulationEvent("norepinephrine", currentNorepinephrineLevel, "uncertainty");
    recordNeuromodulationEvent("acetylcholine", currentAcetylcholineLevel, "curiosity");
    
    // Record learning signal
    learningSignals.push_back(signal);
    
    logStep(trialNumber, signal.rewardPredictionError, signal.curiosity, 
            signal.novelty, signal.uncertainty, "neuromodulation");
    logEpisodeEnd(trialNumber, signal.rewardPredictionError, 1, signal.novelty);
}

void NeuromodulationExperiment::updateNeuromodulators(const LearningSignal& signal) {
    // Update neuromodulator levels based on learning signals
    currentDopamineLevel *= (1.0f + signal.rewardPredictionError * config.dopamineLearningRate);
    currentSerotoninLevel *= (1.0f + signal.novelty * config.serotoninPlasticityModulator);
    currentNorepinephrineLevel *= (1.0f + signal.uncertainty * config.norepinephrineAttentionModulator);
    currentAcetylcholineLevel *= (1.0f + signal.curiosity * config.acetylcholineMemoryModulator);
    
    // Clamp neuromodulator levels
    currentDopamineLevel = std::max(0.0f, std::min(1.0f, currentDopamineLevel));
    currentSerotoninLevel = std::max(0.0f, std::min(1.0f, currentSerotoninLevel));
    currentNorepinephrineLevel = std::max(0.0f, std::min(1.0f, currentNorepinephrineLevel));
    currentAcetylcholineLevel = std::max(0.0f, std::min(1.0f, currentAcetylcholineLevel));
}

void NeuromodulationExperiment::applyNeuromodulationEffects() {
    // Apply neuromodulator effects to brain (placeholder)
    // This would modify brain dynamics based on neuromodulator levels
}

void NeuromodulationExperiment::recordNeuromodulationEvent(const std::string& neuromodulator, 
                                                          float level, const std::string& trigger) {
    NeuromodulationEvent event;
    event.timestamp = std::chrono::steady_clock::now();
    event.neuromodulator = neuromodulator;
    event.level = level;
    event.effect = level * 0.1f;  // Simple effect calculation
    event.trigger = trigger;
    
    neuromodulationEvents.push_back(event);
}

const std::vector<NeuromodulationExperiment::NeuromodulationEvent>& NeuromodulationExperiment::getNeuromodulationEvents() const {
    return neuromodulationEvents;
}

const std::vector<NeuromodulationExperiment::LearningSignal>& NeuromodulationExperiment::getLearningSignals() const {
    return learningSignals;
}

double NeuromodulationExperiment::calculateNeuromodulationEffect(const std::string& neuromodulator) const {
    double totalEffect = 0.0;
    for (const auto& event : neuromodulationEvents) {
        if (event.neuromodulator == neuromodulator) {
            totalEffect += event.effect;
        }
    }
    
    return neuromodulationEvents.empty() ? 0.0 : totalEffect / neuromodulationEvents.size();
}

double NeuromodulationExperiment::calculateLearningEfficiency() const {
    if (learningSignals.empty()) return 0.0;
    
    double totalRPE = 0.0;
    double totalNovelty = 0.0;
    
    for (const auto& signal : learningSignals) {
        totalRPE += signal.rewardPredictionError;
        totalNovelty += signal.novelty;
    }
    
    double avgRPE = totalRPE / learningSignals.size();
    double avgNovelty = totalNovelty / learningSignals.size();
    
    // Learning efficiency based on balanced RPE and novelty
    double efficiency = (1.0 - std::abs(avgRPE - 0.5)) * (1.0 - std::abs(avgNovelty - 0.5));
    return efficiency;
}

std::shared_ptr<Brain> NeuromodulationExperiment::createBrain() {
    // TODO: Create appropriate brain for neuromodulation experiment
    return nullptr;
}

std::shared_ptr<Environment> NeuromodulationExperiment::createEnvironment() {
    // TODO: Create appropriate environment for neuromodulation experiment
    return nullptr;
}

bool NeuromodulationExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                              std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !learningSignals.empty();
}

bool NeuromodulationExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Neuromodulation Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Final Dopamine Level: " << currentDopamineLevel << std::endl;
    file << "Final Serotonin Level: " << currentSerotoninLevel << std::endl;
    file << "Final Norepinephrine Level: " << currentNorepinephrineLevel << std::endl;
    file << "Final Acetylcholine Level: " << currentAcetylcholineLevel << std::endl;
    file << std::endl;
    
    // Save learning signals
    file << "Learning Signals:" << std::endl;
    file << "Count: " << learningSignals.size() << std::endl;
    for (const auto& signal : learningSignals) {
        file << "  Step " << signal.step << ":"
             << " RPE=" << signal.rewardPredictionError
             << " Novelty=" << signal.novelty
             << " Curiosity=" << signal.curiosity
             << " Uncertainty=" << signal.uncertainty
             << " LearningRate=" << signal.learningRate
             << std::endl;
    }
    
    // Save neuromodulation events
    file << std::endl;
    file << "Neuromodulation Events:" << std::endl;
    file << "Count: " << neuromodulationEvents.size() << std::endl;
    for (const auto& event : neuromodulationEvents) {
        file << "  " << event.timestamp.time_since_epoch().count() << ":"
             << " " << event.neuromodulator
             << " Level=" << event.level
             << " Effect=" << event.effect
             << " Trigger=" << event.trigger
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Average Reward Prediction Error: " << getAverageRewardPredictionError() << std::endl;
    file << "  Average Novelty: " << getAverageNovelty() << std::endl;
    file << "  Average Curiosity: " << getAverageCuriosity() << std::endl;
    file << "  Learning Efficiency: " << calculateLearningEfficiency() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool NeuromodulationExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double NeuromodulationExperiment::getProgress() const {
    return learningSignals.empty() ? 0.0 : 
           static_cast<double>(learningSignals.size()) / config.numTrials;
}

std::string NeuromodulationExperiment::getStatus() const {
    if (learningSignals.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

float NeuromodulationExperiment::getAverageDopamineLevel() const {
    // Calculate average dopamine level from events
    if (neuromodulationEvents.empty()) return 0.0f;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& event : neuromodulationEvents) {
        if (event.neuromodulator == "dopamine") {
            total += event.level;
            count++;
        }
    }
    
    return count > 0 ? static_cast<float>(total / count) : 0.0f;
}

float NeuromodulationExperiment::getAverageSerotoninLevel() const {
    // Similar implementation for other neuromodulators
    if (neuromodulationEvents.empty()) return 0.0f;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& event : neuromodulationEvents) {
        if (event.neuromodulator == "serotonin") {
            total += event.level;
            count++;
        }
    }
    
    return count > 0 ? static_cast<float>(total / count) : 0.0f;
}

float NeuromodulationExperiment::getAverageNorepinephrineLevel() const {
    if (neuromodulationEvents.empty()) return 0.0f;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& event : neuromodulationEvents) {
        if (event.neuromodulator == "norepinephrine") {
            total += event.level;
            count++;
        }
    }
    
    return count > 0 ? static_cast<float>(total / count) : 0.0f;
}

float NeuromodulationExperiment::getAverageAcetylcholineLevel() const {
    if (neuromodulationEvents.empty()) return 0.0f;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& event : neuromodulationEvents) {
        if (event.neuromodulator == "acetylcholine") {
            total += event.level;
            count++;
        }
    }
    
    return count > 0 ? static_cast<float>(total / count) : 0.0f;
}

double NeuromodulationExperiment::getAverageRewardPredictionError() const {
    if (learningSignals.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& signal : learningSignals) {
        total += signal.rewardPredictionError;
    }
    
    return total / learningSignals.size();
}

double NeuromodulationExperiment::getAverageNovelty() const {
    if (learningSignals.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& signal : learningSignals) {
        total += signal.novelty;
    }
    
    return total / learningSignals.size();
}

double NeuromodulationExperiment::getAverageCuriosity() const {
    if (learningSignals.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& signal : learningSignals) {
        total += signal.curiosity;
    }
    
    return total / learningSignals.size();
}

} // namespace nlm