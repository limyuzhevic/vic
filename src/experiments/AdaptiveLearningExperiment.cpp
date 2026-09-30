#include "AdaptiveLearningExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>

namespace nlm {

AdaptiveLearningExperiment::AdaptiveLearningExperiment()
    : currentLearningRate(0.01f), currentEnvironmentalComplexity(5.0f),
      successfulAdaptations(0), totalAdaptations(0), rng(std::random_device()()),
      rewardDist(0.0, 10.0), noveltyDist(0.0, 1.0), actionDist(0.0, 1.0) {
    setName("Adaptive Learning Experiment");
    setDescription("Tests adaptive learning with changing environments and dynamic parameters");
}

bool AdaptiveLearningExperiment::initialize() {
    log("Initializing Adaptive Learning Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    if (config.learningRate <= 0.0f || config.learningRate > 1.0f) {
        log("ERROR: learningRate must be between 0.0 and 1.0");
        return false;
    }
    
    currentLearningRate = config.learningRate;
    currentEnvironmentalComplexity = config.environmentComplexity;
    successfulAdaptations = 0;
    totalAdaptations = 0;
    trialResults.clear();
    
    log("Adaptive Learning Experiment initialized successfully");
    return true;
}

void AdaptiveLearningExperiment::setup() {
    log("Setting up Adaptive Learning Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Learning Rate: " + std::to_string(config.learningRate));
    log("  - Environment Complexity: " + std::to_string(config.environmentComplexity));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void AdaptiveLearningExperiment::run() {
    log("Starting Adaptive Learning Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runAdaptiveTrial(trial);
        
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
    log("Adaptive Learning Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void AdaptiveLearningExperiment::analyzeResults() {
    log("Analyzing Adaptive Learning Experiment Results");
    
    if (trialResults.empty()) {
        log("ERROR: No trial results to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalReward = 0.0;
    double totalNovelty = 0.0;
    double totalCuriosity = 0.0;
    double totalPredictionError = 0.0;
    double totalAdaptationFactor = 0.0;
    
    for (const auto& result : trialResults) {
        totalReward += result.reward;
        totalNovelty += result.novelty;
        totalCuriosity += result.curiosity;
        totalPredictionError += result.predictionError;
        totalAdaptationFactor += result.adaptationFactor;
    }
    
    size_t numResults = trialResults.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_reward", totalReward);
    results.recordMetric("average_reward", totalReward / numResults);
    results.recordMetric("average_novelty", totalNovelty / numResults);
    results.recordMetric("average_curiosity", totalCuriosity / numResults);
    results.recordMetric("average_prediction_error", totalPredictionError / numResults);
    results.recordMetric("average_adaptation_factor", totalAdaptationFactor / numResults);
    
    // Calculate learning metrics
    double adaptationRate = calculateAdaptationRate();
    double explorationRate = calculateExplorationRate();
    double learningEfficiency = calculateLearningEfficiency();
    
    results.recordMetric("adaptation_rate", adaptationRate);
    results.recordMetric("exploration_rate", explorationRate);
    results.recordMetric("learning_efficiency", learningEfficiency);
    
    // Record results
    recordResults(results);
    
    log("Adaptive Learning Experiment analysis complete");
    log("  - Average Reward: " + std::to_string(totalReward / numResults));
    log("  - Average Novelty: " + std::to_string(totalNovelty / numResults));
    log("  - Average Curiosity: " + std::to_string(totalCuriosity / numResults));
    log("  - Adaptation Rate: " + std::to_string(adaptationRate));
    log("  - Exploration Rate: " + std::to_string(explorationRate));
    log("  - Learning Efficiency: " + std::to_string(learningEfficiency));
}

void AdaptiveLearningExperiment::cleanup() {
    log("Cleaning up Adaptive Learning Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    trialResults.clear();
    
    log("Adaptive Learning Experiment cleanup complete");
}

void AdaptiveLearningExperiment::setAdaptiveConfig(const AdaptiveConfig& config) {
    this->config = config;
    currentLearningRate = config.learningRate;
    currentEnvironmentalComplexity = config.environmentComplexity;
}

const AdaptiveLearningExperiment::AdaptiveConfig& AdaptiveLearningExperiment::getAdaptiveConfig() const {
    return config;
}

void AdaptiveLearningExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void AdaptiveLearningExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void AdaptiveLearningExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void AdaptiveLearningExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void AdaptiveLearningExperiment::runAdaptiveTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Generate trial parameters
    double reward = rewardDist(rng);
    double novelty = noveltyDist(rng);
    double curiosity = novelty * 0.5 + reward * 0.3;
    double energy = std::max(0.0, 1.0 - novelty * 0.2 - curiosity * 0.3);
    double predictionError = std::abs(reward * 0.1 - novelty * 0.3);
    
    // Select action based on exploration vs exploitation
    std::string action;
    double actionValue = actionDist(rng);
    
    if (actionValue < config.explorationProbability / 100.0) {
        action = "EXPLORATION";
    } else {
        action = "EXPLOITATION";
    }
    
    // Calculate adaptation factor
    float adaptationFactor = currentLearningRate * currentEnvironmentalComplexity;
    
    // Update environmental complexity
    updateEnvironmentalComplexity(trialNumber);
    
    // Adapt parameters
    adaptParameters(trialNumber);
    
    // Record trial result
    AdaptiveTrialResult result;
    result.trialNumber = trialNumber;
    result.reward = reward;
    result.novelty = novelty;
    result.curiosity = curiosity;
    result.energy = energy;
    result.predictionError = predictionError;
    result.action = action;
    result.adaptationFactor = adaptationFactor;
    result.environmentalComplexity = currentEnvironmentalComplexity;
    
    trialResults.push_back(result);
    
    logStep(trialNumber, reward, curiosity, novelty, energy, action);
    logEpisodeEnd(trialNumber, reward, 1, novelty);
}

void AdaptiveLearningExperiment::adaptParameters(size_t trialNumber) {
    totalAdaptations++;
    
    // Adapt based on trial performance
    double recentReward = trialResults.back().reward;
    double recentNovelty = trialResults.back().novelty;
    
    // Adjust learning rate based on performance
    if (recentReward > 5.0 && recentNovelty < 0.3) {
        currentLearningRate *= 1.1f;  // Improve learning
        successfulAdaptations++;
    } else if (recentReward < 2.0 || recentNovelty > 0.7) {
        currentLearningRate *= 0.9f;  // Reduce learning
    }
    
    // Adapt exploration rate
    double explorationRate = calculateExplorationRate();
    if (explorationRate < 0.2) {
        // Increase exploration
    } else if (explorationRate > 0.5) {
        // Decrease exploration
    }
}

void AdaptiveLearningExperiment::updateEnvironmentalComplexity(size_t trialNumber) {
    // Dynamic environmental complexity changes
    double complexityChange = (trialNumber % 10 == 0) ? 0.5 : 
                              (trialNumber % 5 == 0) ? -0.3 : 0.0;
    
    currentEnvironmentalComplexity += static_cast<float>(complexityChange);
    currentEnvironmentalComplexity = std::max(1.0f, std::min(10.0f, currentEnvironmentalComplexity));
}

const std::vector<AdaptiveLearningExperiment::AdaptiveTrialResult>& AdaptiveLearningExperiment::getTrialResults() const {
    return trialResults;
}

double AdaptiveLearningExperiment::calculateAdaptationRate() const {
    if (totalAdaptations == 0) return 0.0;
    return static_cast<double>(successfulAdaptations) / totalAdaptations;
}

double AdaptiveLearningExperiment::calculateExplorationRate() const {
    size_t explorationCount = 0;
    for (const auto& result : trialResults) {
        if (result.action == "EXPLORATION") {
            explorationCount++;
        }
    }
    
    return trialResults.empty() ? 0.0 : static_cast<double>(explorationCount) / trialResults.size();
}

double AdaptiveLearningExperiment::calculateLearningEfficiency() const {
    if (trialResults.empty()) return 0.0;
    
    double totalReward = 0.0;
    double totalPredictionError = 0.0;
    
    for (const auto& result : trialResults) {
        totalReward += result.reward;
        totalPredictionError += result.predictionError;
    }
    
    double avgReward = totalReward / trialResults.size();
    double avgPredictionError = totalPredictionError / trialResults.size();
    
    return (avgReward > 0.0) ? (avgReward / (1.0 + avgPredictionError)) : 0.0;
}

std::shared_ptr<Brain> AdaptiveLearningExperiment::createBrain() {
    // TODO: Create appropriate brain for adaptive learning experiment
    return nullptr;
}

std::shared_ptr<Environment> AdaptiveLearningExperiment::createEnvironment() {
    // TODO: Create appropriate environment for adaptive learning experiment
    return nullptr;
}

bool AdaptiveLearningExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                               std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !trialResults.empty();
}

bool AdaptiveLearningExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Adaptive Learning Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Learning Rate: " << config.learningRate << std::endl;
    file << "Environment Complexity: " << config.environmentComplexity << std::endl;
    file << "Number of Trials: " << config.numTrials << std::endl;
    file << std::endl;
    
    // Save trial results
    file << "Trial Results:" << std::endl;
    for (const auto& result : trialResults) {
        file << "Trial " << result.trialNumber << ":"
             << " Reward=" << result.reward
             << " Novelty=" << result.novelty
             << " Curiosity=" << result.curiosity
             << " Energy=" << result.energy
             << " PredictionError=" << result.predictionError
             << " Action=" << result.action
             << " AdaptationFactor=" << result.adaptationFactor
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Average Reward: " << getAverageReward() << std::endl;
    file << "  Average Novelty: " << getAverageNovelty() << std::endl;
    file << "  Average Curiosity: " << getAverageCuriosity() << std::endl;
    file << "  Adaptation Rate: " << calculateAdaptationRate() << std::endl;
    file << "  Exploration Rate: " << calculateExplorationRate() << std::endl;
    file << "  Learning Efficiency: " << calculateLearningEfficiency() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool AdaptiveLearningExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double AdaptiveLearningExperiment::getProgress() const {
    return trialResults.empty() ? 0.0 : 
           static_cast<double>(trialResults.size()) / config.numTrials;
}

std::string AdaptiveLearningExperiment::getStatus() const {
    if (trialResults.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

double AdaptiveLearningExperiment::getAverageReward() const {
    if (trialResults.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : trialResults) {
        total += result.reward;
    }
    return total / trialResults.size();
}

double AdaptiveLearningExperiment::getAverageNovelty() const {
    if (trialResults.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : trialResults) {
        total += result.novelty;
    }
    return total / trialResults.size();
}

double AdaptiveLearningExperiment::getAverageCuriosity() const {
    if (trialResults.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : trialResults) {
        total += result.curiosity;
    }
    return total / trialResults.size();
}

size_t AdaptiveLearningExperiment::getSuccessfulAdaptations() const {
    return successfulAdaptations;
}

} // namespace nlm