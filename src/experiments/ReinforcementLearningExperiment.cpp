#include "ReinforcementLearningExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <unordered_map>

namespace nlm {

ReinforcementLearningExperiment::ReinforcementLearningExperiment()
    : currentQValue(0.0f), currentValue(0.0f), episodeReward(0.0f),
      rng(std::random_device()()), rewardDist(0.0, 10.0),
      qValueDist(-10.0, 10.0), policyDist(0.0, 1.0) {
    setName("Reinforcement Learning Experiment");
    setDescription("Tests reinforcement learning and reward prediction with Q-learning and policy gradients");
}

bool ReinforcementLearningExperiment::initialize() {
    log("Initializing Reinforcement Learning Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    if (config.learningRate <= 0.0f || config.learningRate > 1.0f) {
        log("ERROR: learningRate must be between 0.0 and 1.0");
        return false;
    }
    
    if (config.discountFactor <= 0.0f || config.discountFactor > 1.0f) {
        log("ERROR: discountFactor must be between 0.0 and 1.0");
        return false;
    }
    
    // Initialize RL state
    currentQValue = 0.0f;
    currentValue = 0.0f;
    episodeReward = 0.0f;
    
    qTable.clear();
    policyGradients.clear();
    valueFunctions.clear();
    
    log("Reinforcement Learning Experiment initialized successfully");
    log("  - Learning Rate: " + std::to_string(config.learningRate));
    log("  - Discount Factor: " + std::to_string(config.discountFactor));
    log("  - Exploration Rate: " + std::to_string(config.explorationRate));
    log("  - Reward Scale: " + std::to_string(config.rewardScale));
    
    return true;
}

void ReinforcementLearningExperiment::setup() {
    log("Setting up Reinforcement Learning Experiment");
    
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
    log("  - Discount Factor: " + std::to_string(config.discountFactor));
    log("  - Temperature: " + std::to_string(config.temperature));
    log("  - Reward Scale: " + std::to_string(config.rewardScale));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void ReinforcementLearningExperiment::run() {
    log("Starting Reinforcement Learning Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runReinforcementLearningTrial(trial);
        
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
    log("Reinforcement Learning Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void ReinforcementLearningExperiment::analyzeResults() {
    log("Analyzing Reinforcement Learning Experiment Results");
    
    if (qTable.empty()) {
        log("ERROR: No Q-table entries to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalQValue = 0.0;
    double totalPolicyGradient = 0.0;
    double totalValueError = 0.0;
    
    for (const auto& entry : qTable) {
        totalQValue += entry.value;
    }
    
    for (const auto& grad : policyGradients) {
        totalPolicyGradient += grad.gradient;
    }
    
    for (const auto& valueFunc : valueFunctions) {
        totalValueError += std::abs(valueFunc.estimatedValue - valueFunc.targetValue);
    }
    
    size_t numQEntries = qTable.size();
    size_t numGradients = policyGradients.size();
    size_t numValueFuncs = valueFunctions.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_q_value", totalQValue);
    results.recordMetric("average_q_value", totalQValue / numQEntries);
    results.recordMetric("average_policy_gradient", totalPolicyGradient / numGradients);
    results.recordMetric("average_value_error", totalValueError / numValueFuncs);
    results.recordMetric("value_function_mse", totalValueError / numValueFuncs);
    
    // Calculate RL metrics
    double qValueConvergence = calculateQValueConvergence();
    double policyGradientEfficiency = calculatePolicyGradientEfficiency();
    double valueFunctionAccuracy = calculateValueFunctionAccuracy();
    
    results.recordMetric("q_value_convergence", qValueConvergence);
    results.recordMetric("policy_gradient_efficiency", policyGradientEfficiency);
    results.recordMetric("value_function_accuracy", valueFunctionAccuracy);
    
    // Record results
    recordResults(results);
    
    log("Reinforcement Learning Experiment analysis complete");
    log("  - Q Value Convergence: " + std::to_string(qValueConvergence));
    log("  - Policy Gradient Efficiency: " + std::to_string(policyGradientEfficiency));
    log("  - Value Function Accuracy: " + std::to_string(valueFunctionAccuracy));
}

void ReinforcementLearningExperiment::cleanup() {
    log("Cleaning up Reinforcement Learning Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    qTable.clear();
    policyGradients.clear();
    valueFunctions.clear();
    
    log("Reinforcement Learning Experiment cleanup complete");
}

void ReinforcementLearningExperiment::setReinforcementConfig(const ReinforcementConfig& config) {
    this->config = config;
    currentQValue = 0.0f;
    currentValue = 0.0f;
}

const ReinforcementLearningExperiment::ReinforcementConfig& ReinforcementLearningExperiment::getReinforcementConfig() const {
    return config;
}

void ReinforcementLearningExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void ReinforcementLearningExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void ReinforcementLearningExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void ReinforcementLearningExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void ReinforcementLearningExperiment::runReinforcementLearningTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Reset episode reward
    episodeReward = 0.0f;
    
    // Run RL trial (placeholder - simulate RL algorithm)
    currentState = "state_" + std::to_string(trialNumber % 10);
    currentAction = selectAction(currentState);
    
    // Calculate reward
    float extrinsicReward = static_cast<float>(rewardDist(rng));
    float intrinsicReward = config.intrinsicReward * (1.0f - config.explorationRate);
    float totalReward = calculateReward(extrinsicReward, intrinsicReward);
    
    episodeReward = totalReward;
    
    // Update Q-table
    updateQTable(currentState, currentAction, totalReward, "next_state");
    
    // Compute policy gradient
    PolicyGradientResult pgResult = computePolicyGradient(currentState, currentAction);
    policyGradients.push_back(pgResult);
    
    // Compute value function
    ValueFunctionResult vfResult = computeValueFunction(currentState, totalReward);
    valueFunctions.push_back(vfResult);
    
    log("Reinforcement learning trial complete: state=" + currentState +
        " action=" + currentAction +
        " reward=" + std::to_string(totalReward) +
        " q_value=" + std::to_string(currentQValue));
    
    logEpisodeEnd(trialNumber, totalReward, 1, 0.0);  // Placeholder metrics
}

void ReinforcementLearningExperiment::updateQTable(const std::string& state, const std::string& action, float reward, const std::string& nextState) {
    // Find or create Q-table entry
    bool found = false;
    for (auto& entry : qTable) {
        if (entry.state == state && entry.action == action) {
            // Q-learning update
            float maxNextQ = qValueDist(rng);  // Placeholder
            entry.value = entry.value + config.learningRate * 
                         (reward + config.discountFactor * maxNextQ - entry.value);
            entry.visitationCount++;
            currentQValue = entry.value;
            found = true;
            break;
        }
    }
    
    if (!found) {
        QTableEntry entry;
        entry.state = state;
        entry.action = action;
        entry.value = reward;
        entry.visitationCount = 1;
        qTable.push_back(entry);
        currentQValue = reward;
    }
}

ReinforcementLearningExperiment::PolicyGradientResult ReinforcementLearningExperiment::computePolicyGradient(const std::string& state, const std::string& action) {
    PolicyGradientResult result;
    
    // Find Q-value for this state-action pair
    float qValue = qValueDist(rng);  // Placeholder
    float policyProbability = policyDist(rng);
    
    result.action = action;
    result.probability = policyProbability;
    result.advantage = qValue - currentValue;  // Placeholder
    result.gradient = config.learningRate * result.advantage * policyProbability;
    
    return result;
}

ReinforcementLearningExperiment::ValueFunctionResult ReinforcementLearningExperiment::computeValueFunction(const std::string& state, float targetValue) {
    ValueFunctionResult result;
    
    // Estimate value function (placeholder)
    result.state = state;
    result.estimatedValue = currentValue;
    result.targetValue = targetValue;
    result.temporalDifferenceError = targetValue - currentValue;
    
    return result;
}

std::string ReinforcementLearningExperiment::selectAction(const std::string& state) {
    // Epsilon-greedy action selection (placeholder)
    double explorationProbability = config.explorationRate;
    double randomValue = policyDist(rng);
    
    if (randomValue < explorationProbability) {
        return "exploration_action";
    } else {
        return "exploitation_action";
    }
}

float ReinforcementLearningExperiment::calculateReward(float extrinsicReward, float intrinsicReward) {
    // Combine extrinsic and intrinsic rewards
    return extrinsicReward * config.extrinsicReward + intrinsicReward * config.intrinsicReward;
}

const std::vector<ReinforcementLearningExperiment::QTableEntry>& ReinforcementLearningExperiment::getQTable() const {
    return qTable;
}

const std::vector<ReinforcementLearningExperiment::PolicyGradientResult>& ReinforcementLearningExperiment::getPolicyGradients() const {
    return policyGradients;
}

const std::vector<ReinforcementLearningExperiment::ValueFunctionResult>& ReinforcementLearningExperiment::getValueFunctions() const {
    return valueFunctions;
}

double ReinforcementLearningExperiment::calculateQValueConvergence() const {
    if (qTable.empty()) return 0.0;
    
    double totalVariance = 0.0;
    double meanValue = 0.0;
    
    for (const auto& entry : qTable) {
        meanValue += entry.value;
    }
    
    meanValue /= qTable.size();
    
    for (const auto& entry : qTable) {
        double diff = entry.value - meanValue;
        totalVariance += diff * diff;
    }
    
    return totalVariance / qTable.size();
}

double ReinforcementLearningExperiment::calculatePolicyGradientEfficiency() const {
    if (policyGradients.empty()) return 0.0;
    
    double totalEfficiency = 0.0;
    for (const auto& grad : policyGradients) {
        totalEfficiency += std::abs(grad.gradient);
    }
    
    return totalEfficiency / policyGradients.size();
}

double ReinforcementLearningExperiment::calculateValueFunctionAccuracy() const {
    if (valueFunctions.empty()) return 0.0;
    
    double totalError = 0.0;
    for (const auto& valueFunc : valueFunctions) {
        totalError += std::abs(valueFunc.estimatedValue - valueFunc.targetValue);
    }
    
    return 1.0 - (totalError / valueFunctions.size());  // Normalize to [0, 1]
}

std::shared_ptr<Brain> ReinforcementLearningExperiment::createBrain() {
    // TODO: Create appropriate brain for reinforcement learning experiment
    return nullptr;
}

std::shared_ptr<Environment> ReinforcementLearningExperiment::createEnvironment() {
    // TODO: Create appropriate environment for reinforcement learning experiment
    return nullptr;
}

bool ReinforcementLearningExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                                   std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !qTable.empty();
}

bool ReinforcementLearningExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Reinforcement Learning Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Final Q Value: " << currentQValue << std::endl;
    file << "Final Value: " << currentValue << std::endl;
    file << "Number of Q-table Entries: " << qTable.size() << std::endl;
    file << std::endl;
    
    // Save Q-table
    file << "Q-Table:" << std::endl;
    file << "Count: " << qTable.size() << std::endl;
    for (const auto& entry : qTable) {
        file << "  State: " << entry.state
             << " Action: " << entry.action
             << " Value: " << entry.value
             << " Visited: " << entry.visitationCount
             << std::endl;
    }
    
    // Save policy gradients
    file << std::endl;
    file << "Policy Gradients:" << std::endl;
    file << "Count: " << policyGradients.size() << std::endl;
    for (const auto& grad : policyGradients) {
        file << "  Action: " << grad.action
             << " Probability: " << grad.probability
             << " Advantage: " << grad.advantage
             << " Gradient: " << grad.gradient
             << std::endl;
    }
    
    // Save value functions
    file << std::endl;
    file << "Value Functions:" << std::endl;
    file << "Count: " << valueFunctions.size() << std::endl;
    for (const auto& valueFunc : valueFunctions) {
        file << "  State: " << valueFunc.state
             << " EstimatedValue: " << valueFunc.estimatedValue
             << " TargetValue: " << valueFunc.targetValue
             << " TDError: " << valueFunc.temporalDifferenceError
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Average Q Value: " << getAverageQValue() << std::endl;
    file << "  Policy Gradient Norm: " << getPolicyGradientNorm() << std::endl;
    file << "  Value Function MSE: " << getValueFunctionMSE() << std::endl;
    file << "  Q Value Convergence: " << calculateQValueConvergence() << std::endl;
    file << "  Policy Gradient Efficiency: " << calculatePolicyGradientEfficiency() << std::endl;
    file << "  Value Function Accuracy: " << calculateValueFunctionAccuracy() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool ReinforcementLearningExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double ReinforcementLearningExperiment::getProgress() const {
    return qTable.empty() ? 0.0 : 
           static_cast<double>(qTable.size()) / config.numTrials;
}

std::string ReinforcementLearningExperiment::getStatus() const {
    if (qTable.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

float ReinforcementLearningExperiment::getAverageQValue() const {
    if (qTable.empty()) return 0.0f;
    
    double total = 0.0;
    for (const auto& entry : qTable) {
        total += entry.value;
    }
    
    return static_cast<float>(total / qTable.size());
}

float ReinforcementLearningExperiment::getPolicyGradientNorm() const {
    if (policyGradients.empty()) return 0.0f;
    
    double totalNorm = 0.0;
    for (const auto& grad : policyGradients) {
        totalNorm += grad.gradient * grad.gradient;
    }
    
    return static_cast<float>(std::sqrt(totalNorm / policyGradients.size()));
}

float ReinforcementLearningExperiment::getValueFunctionMSE() const {
    if (valueFunctions.empty()) return 0.0f;
    
    double totalMSE = 0.0;
    for (const auto& valueFunc : valueFunctions) {
        totalMSE += (valueFunc.estimatedValue - valueFunc.targetValue) * 
                   (valueFunc.estimatedValue - valueFunc.targetValue);
    }
    
    return static_cast<float>(totalMSE / valueFunctions.size());
}

} // namespace nlm