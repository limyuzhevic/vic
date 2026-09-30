#include "StructuralPlasticityExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>

namespace nlm {

StructuralPlasticityExperiment::StructuralPlasticityExperiment()
    : currentNeuronCount(1000), currentSynapseCount(0), 
      currentConnectionProbability(0.1f), currentStep(0), rng(std::random_device()()),
      weightDist(-1.0, 1.0), plasticityDist(0.0, 1.0) {
    setName("Structural Plasticity Experiment");
    setDescription("Tests structural plasticity and brain development with dynamic synaptogenesis and pruning");
}

bool StructuralPlasticityExperiment::initialize() {
    log("Initializing Structural Plasticity Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    if (config.maxNeurons < config.initialMaxNeurons) {
        log("ERROR: maxNeurons must be >= initialMaxNeurons");
        return false;
    }
    
    // Initialize structural elements
    currentNeuronCount = config.initialMaxNeurons;
    currentSynapseCount = static_cast<size_t>(currentNeuronCount * config.initialConnectionProbability);
    currentConnectionProbability = config.initialConnectionProbability;
    currentStep = 0;
    
    synapticConnections.clear();
    developmentStages.clear();
    
    log("Structural Plasticity Experiment initialized successfully");
    log("  - Initial Neurons: " + std::to_string(currentNeuronCount));
    log("  - Initial Synapses: " + std::to_string(currentSynapseCount));
    log("  - Synaptogenesis Rate: " + std::to_string(config.synaptogenesisRate));
    log("  - Pruning Rate: " + std::to_string(config.pruningRate));
    
    return true;
}

void StructuralPlasticityExperiment::setup() {
    log("Setting up Structural Plasticity Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Synaptogenesis Rate: " + std::to_string(config.synaptogenesisRate));
    log("  - Pruning Rate: " + std::to_string(config.pruningRate));
    log("  - Initial Connection Probability: " + std::to_string(config.initialConnectionProbability));
    log("  - Maximum Neurons: " + std::to_string(config.maxNeurons));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void StructuralPlasticityExperiment::run() {
    log("Starting Structural Plasticity Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runStructuralPlasticityTrial(trial);
        
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
    log("Structural Plasticity Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void StructuralPlasticityExperiment::analyzeResults() {
    log("Analyzing Structural Plasticity Experiment Results");
    
    if (synapticConnections.empty()) {
        log("ERROR: No synaptic connections to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalWeight = 0.0;
    double totalPlasticity = 0.0;
    size_t activeConnections = 0;
    
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            totalWeight += conn.weight;
            totalPlasticity += conn.plasticity;
            activeConnections++;
        }
    }
    
    size_t numConnections = synapticConnections.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_weight", totalWeight);
    results.recordMetric("average_weight", totalWeight / numConnections);
    results.recordMetric("average_plasticity", totalPlasticity / numConnections);
    results.recordMetric("active_connections", static_cast<double>(activeConnections));
    results.recordMetric("connection_density", static_cast<double>(activeConnections) / numConnections);
    
    // Calculate structural metrics
    double synapticStability = calculateSynapticStability();
    double networkEfficiency = calculateNetworkEfficiency();
    double structuralComplexity = calculateStructuralComplexity();
    
    results.recordMetric("synaptic_stability", synapticStability);
    results.recordMetric("network_efficiency", networkEfficiency);
    results.recordMetric("structural_complexity", structuralComplexity);
    
    // Record results
    recordResults(results);
    
    log("Structural Plasticity Experiment analysis complete");
    log("  - Synaptic Stability: " + std::to_string(synapticStability));
    log("  - Network Efficiency: " + std::to_string(networkEfficiency));
    log("  - Structural Complexity: " + std::to_string(structuralComplexity));
}

void StructuralPlasticityExperiment::cleanup() {
    log("Cleaning up Structural Plasticity Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    synapticConnections.clear();
    developmentStages.clear();
    
    log("Structural Plasticity Experiment cleanup complete");
}

void StructuralPlasticityExperiment::setPlasticityConfig(const PlasticityConfig& config) {
    this->config = config;
    currentConnectionProbability = config.initialConnectionProbability;
}

const StructuralPlasticityExperiment::PlasticityConfig& StructuralPlasticityExperiment::getPlasticityConfig() const {
    return config;
}

void StructuralPlasticityExperiment::setNumNeurons(size_t neurons) {
    config.initialMaxNeurons = neurons;
    config.maxNeurons = std::max(config.maxNeurons, neurons);
}

void StructuralPlasticityExperiment::setNumSynapses(size_t synapses) {
    config.initialConnectionProbability = static_cast<float>(synapses) / config.initialMaxNeurons;
}

void StructuralPlasticityExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void StructuralPlasticityExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void StructuralPlasticityExperiment::runStructuralPlasticityTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Simulate development stages
    NeuralDevelopmentStage stage;
    stage.stageName = (trialNumber < config.synaptogenesisPhase) ? "Synaptogenesis" :
                     (trialNumber < config.pruningPhase) ? "Pruning" : "Stabilization";
    stage.startStep = currentStep;
    stage.endStep = currentStep + 100;
    stage.neuronCount = currentNeuronCount;
    stage.synapseCount = currentSynapseCount;
    
    // Run developmental phase
    if (trialNumber < config.synaptogenesisPhase) {
        simulateSynaptogenesis(trialNumber);
    } else if (trialNumber < config.pruningPhase) {
        simulatePruning(trialNumber);
    } else {
        simulateStabilization(trialNumber);
    }
    
    stage.averageFiringRate = 10.0f;  // Placeholder
    stage.averageWeight = calculateAverageConnectionStrength();
    stage.connectionDensity = static_cast<float>(currentSynapseCount) / 
                             (static_cast<float>(currentNeuronCount) * currentNeuronCount);
    
    developmentStages.push_back(stage);
    
    // Log trial results
    log("Development Stage: " + stage.stageName);
    log("  - Neurons: " + std::to_string(stage.neuronCount));
    log("  - Synapses: " + std::to_string(stage.synapseCount));
    log("  - Connection Density: " + std::to_string(stage.connectionDensity));
    
    logEpisodeEnd(trialNumber, 0.0, 1, 0.0);  // Placeholder metrics
}

void StructuralPlasticityExperiment::simulateSynaptogenesis(size_t trialNumber) {
    // Generate new synaptic connections
    size_t newConnections = static_cast<size_t>(config.synaptogenesisRate * 
                                                currentNeuronCount * 100);
    
    for (size_t i = 0; i < newConnections && currentNeuronCount < config.maxNeurons; ++i) {
        SynapticConnection conn;
        conn.preNeuronId = rand() % currentNeuronCount;
        conn.postNeuronId = rand() % currentNeuronCount;
        conn.weight = weightDist(rng);
        conn.plasticity = plasticityDist(rng);
        conn.lastUpdate = currentStep;
        conn.isActive = true;
        
        synapticConnections.push_back(conn);
    }
    
    currentSynapseCount += newConnections;
    currentNeuronCount = std::min(currentNeuronCount + 100, config.maxNeurons);
}

void StructuralPlasticityExperiment::simulatePruning(size_t trialNumber) {
    // Remove weak or redundant connections
    size_t connectionsToRemove = static_cast<size_t>(config.pruningRate * currentSynapseCount);
    
    std::vector<SynapticConnection> activeConnections;
    for (auto& conn : synapticConnections) {
        if (conn.isActive) {
            // Determine if connection should be pruned
            double pruningProbability = config.pruningThreshold * 
                                        std::abs(conn.weight) * conn.plasticity;
            
            if (std::rand() / (double)RAND_MAX < pruningProbability) {
                conn.isActive = false;
                connectionsToRemove--;
            } else {
                activeConnections.push_back(conn);
            }
        }
    }
    
    synapticConnections = activeConnections;
    currentSynapseCount = synapticConnections.size();
}

void StructuralPlasticityExperiment::simulateStabilization(size_t trialNumber) {
    // Stabilize remaining connections through homeostatic plasticity
    for (auto& conn : synapticConnections) {
        if (conn.isActive) {
            // Apply homeostatic plasticity
            conn.weight += (0.5f - conn.weight) * config.homeostaticPlasticity;
            conn.plasticity *= 0.95f;  // Reduce plasticity over time
            conn.lastUpdate = currentStep;
        }
    }
    
    // Update structural connectivity
    updateStructuralConnectivity();
}

void StructuralPlasticityExperiment::updateStructuralConnectivity() {
    // Update global structural properties
    currentStep++;
    
    // Calculate current connection probability
    if (currentNeuronCount > 0) {
        currentConnectionProbability = static_cast<float>(currentSynapseCount) / 
                                        (static_cast<float>(currentNeuronCount) * currentNeuronCount);
    }
}

const std::vector<StructuralPlasticityExperiment::SynapticConnection>& StructuralPlasticityExperiment::getSynapticConnections() const {
    return synapticConnections;
}

const std::vector<StructuralPlasticityExperiment::NeuralDevelopmentStage>& StructuralPlasticityExperiment::getDevelopmentStages() const {
    return developmentStages;
}

double StructuralPlasticityExperiment::calculateSynapticStability() const {
    if (synapticConnections.empty()) return 0.0;
    
    double totalStability = 0.0;
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            totalStability += conn.plasticity * (1.0 - std::abs(conn.weight));
        }
    }
    
    return totalStability / synapticConnections.size();
}

double StructuralPlasticityExperiment::calculateNetworkEfficiency() const {
    if (currentNeuronCount == 0) return 0.0;
    
    // Network efficiency based on connection density and weight distribution
    double density = static_cast<double>(currentSynapseCount) / 
                     (static_cast<double>(currentNeuronCount) * currentNeuronCount);
    
    double avgWeight = calculateAverageConnectionStrength();
    double weightStability = 1.0 - std::abs(avgWeight - 0.0);
    
    return density * weightStability;
}

double StructuralPlasticityExperiment::calculateStructuralComplexity() const {
    // Structural complexity based on number of active connections and diversity
    if (synapticConnections.empty()) return 0.0;
    
    double activeConnections = 0;
    double weightVariance = 0.0;
    double meanWeight = 0.0;
    
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            activeConnections++;
            meanWeight += conn.weight;
        }
    }
    
    if (activeConnections == 0) return 0.0;
    
    meanWeight /= activeConnections;
    
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            double diff = conn.weight - meanWeight;
            weightVariance += diff * diff;
        }
    }
    
    weightVariance /= activeConnections;
    double stdDev = std::sqrt(weightVariance);
    
    // Complexity = (active connections * weight diversity) / neuron count
    return (activeConnections * stdDev) / currentNeuronCount;
}

std::shared_ptr<Brain> StructuralPlasticityExperiment::createBrain() {
    // TODO: Create appropriate brain for structural plasticity experiment
    return nullptr;
}

std::shared_ptr<Environment> StructuralPlasticityExperiment::createEnvironment() {
    // TODO: Create appropriate environment for structural plasticity experiment
    return nullptr;
}

bool StructuralPlasticityExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                                  std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !synapticConnections.empty();
}

bool StructuralPlasticityExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Structural Plasticity Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Synaptogenesis Rate: " << config.synaptogenesisRate << std::endl;
    file << "Pruning Rate: " << config.pruningRate << std::endl;
    file << "Initial Max Neurons: " << config.initialMaxNeurons << std::endl;
    file << "Maximum Neurons: " << config.maxNeurons << std::endl;
    file << std::endl;
    
    // Save development stages
    file << "Development Stages:" << std::endl;
    for (const auto& stage : developmentStages) {
        file << "  Stage: " << stage.stageName
             << " (Steps " << stage.startStep << "-" << stage.endStep << ")"
             << " Neurons: " << stage.neuronCount
             << " Synapses: " << stage.synapseCount
             << " Connection Density: " << stage.connectionDensity
             << std::endl;
    }
    
    // Save synaptic connections
    file << std::endl;
    file << "Synaptic Connections:" << std::endl;
    file << "Count: " << synapticConnections.size() << std::endl;
    for (const auto& conn : synapticConnections) {
        file << "  Connection: " << conn.preNeuronId << " -> " << conn.postNeuronId
             << " Weight: " << conn.weight
             << " Plasticity: " << conn.plasticity
             << " Active: " << (conn.isActive ? "Yes" : "No")
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Final Neuron Count: " << currentNeuronCount << std::endl;
    file << "  Final Synapse Count: " << currentSynapseCount << std::endl;
    file << "  Final Connection Probability: " << currentConnectionProbability << std::endl;
    file << "  Synaptic Stability: " << calculateSynapticStability() << std::endl;
    file << "  Network Efficiency: " << calculateNetworkEfficiency() << std::endl;
    file << "  Structural Complexity: " << calculateStructuralComplexity() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool StructuralPlasticityExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double StructuralPlasticityExperiment::getProgress() const {
    return developmentStages.empty() ? 0.0 : 
           static_cast<double>(developmentStages.size()) / config.numTrials;
}

std::string StructuralPlasticityExperiment::getStatus() const {
    if (developmentStages.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

size_t StructuralPlasticityExperiment::getNeuronCount() const {
    return currentNeuronCount;
}

size_t StructuralPlasticityExperiment::getSynapseCount() const {
    return currentSynapseCount;
}

float StructuralPlasticityExperiment::getAverageConnectionStrength() const {
    if (synapticConnections.empty()) return 0.0f;
    
    double total = 0.0;
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            total += conn.weight;
        }
    }
    
    return static_cast<float>(total) / synapticConnections.size();
}

size_t StructuralPlasticityExperiment::getActiveConnections() const {
    size_t count = 0;
    for (const auto& conn : synapticConnections) {
        if (conn.isActive) {
            count++;
        }
    }
    return count;
}

float StructuralPlasticityExperiment::calculateAverageConnectionStrength() const {
    return getAverageConnectionStrength();
}

} // namespace nlm