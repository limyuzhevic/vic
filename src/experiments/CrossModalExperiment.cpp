#include "CrossModalExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

CrossModalExperiment::CrossModalExperiment()
    : sensoryIntegrationStrength(1.0f), multisensoryBindingQuality(1.0f),
      rng(std::random_device()()), sensoryDist(0.0, 1.0),
      integrationDist(0.0, 1.0), featureDist(0.0, 1.0) {
    setName("Cross-Modal Experiment");
    setDescription("Tests cross-modal learning and sensory integration with multiple sensory modalities");
}

bool CrossModalExperiment::initialize() {
    log("Initializing Cross-Modal Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    // Initialize cross-modal state
    sensoryIntegrationStrength = config.crossModalIntegrationStrength;
    multisensoryBindingQuality = config.multimodalBindingStrength;
    
    sensoryInputs.clear();
    multisensoryRepresentations.clear();
    
    log("Cross-Modal Experiment initialized successfully");
    log("  - Visual Sensory Weight: " + std::to_string(config.visualSensoryWeight));
    log("  - Auditory Sensory Weight: " + std::to_string(config.auditorySensoryWeight));
    log("  - Cross-Modal Integration Strength: " + std::to_string(config.crossModalIntegrationStrength));
    log("  - Multisensory Binding Strength: " + std::to_string(config.multimodalBindingStrength));
    
    return true;
}

void CrossModalExperiment::setup() {
    log("Setting up Cross-Modal Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Cross-Modal Integration Strength: " + std::to_string(config.crossModalIntegrationStrength));
    log("  - Sensory Conflict Resolution: " + std::to_string(config.sensoryConflictResolution));
    log("  - Multisensory Binding Strength: " + std::to_string(config.multimodalBindingStrength));
    log("  - Cross-Modal Learning Rate: " + std::to_string(config.crossModalLearningRate));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void CrossModalExperiment::run() {
    log("Starting Cross-Modal Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runCrossModalTrial(trial);
        
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
    log("Cross-Modal Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void CrossModalExperiment::analyzeResults() {
    log("Analyzing Cross-Modal Experiment Results");
    
    if (sensoryInputs.empty()) {
        log("ERROR: No sensory inputs to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalIntegrationScore = 0.0;
    double totalBindingQuality = 0.0;
    
    for (const auto& input : sensoryInputs) {
        totalIntegrationScore += input.intensity * input.reliability;
        totalBindingQuality += input.reliability;
    }
    
    size_t numInputs = sensoryInputs.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_integration_score", totalIntegrationScore);
    results.recordMetric("average_integration_score", totalIntegrationScore / numInputs);
    results.recordMetric("average_sensory_reliability", totalBindingQuality / numInputs);
    results.recordMetric("sensory_integration_strength", sensoryIntegrationStrength);
    results.recordMetric("multisensory_binding_quality", multisensoryBindingQuality);
    
    // Calculate cross-modal metrics
    double sensoryIntegrationAccuracy = calculateSensoryIntegrationAccuracy();
    double crossModalLearningEfficiency = calculateCrossModalLearningEfficiency();
    double multisensoryBindingStrength = calculateMultisensoryBindingStrength();
    
    results.recordMetric("sensory_integration_accuracy", sensoryIntegrationAccuracy);
    results.recordMetric("cross_modal_learning_efficiency", crossModalLearningEfficiency);
    results.recordMetric("multisensory_binding_strength", multisensoryBindingStrength);
    
    // Record results
    recordResults(results);
    
    log("Cross-Modal Experiment analysis complete");
    log("  - Sensory Integration Accuracy: " + std::to_string(sensoryIntegrationAccuracy));
    log("  - Cross-Modal Learning Efficiency: " + std::to_string(crossModalLearningEfficiency));
    log("  - Multisensory Binding Strength: " + std::to_string(multisensoryBindingStrength));
}

void CrossModalExperiment::cleanup() {
    log("Cleaning up Cross-Modal Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    sensoryInputs.clear();
    multisensoryRepresentations.clear();
    
    log("Cross-Modal Experiment cleanup complete");
}

void CrossModalExperiment::setCrossModalConfig(const CrossModalConfig& config) {
    this->config = config;
    sensoryIntegrationStrength = config.crossModalIntegrationStrength;
    multisensoryBindingQuality = config.multimodalBindingStrength;
}

const CrossModalExperiment::CrossModalConfig& CrossModalExperiment::getCrossModalConfig() const {
    return config;
}

void CrossModalExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void CrossModalExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void CrossModalExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void CrossModalExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void CrossModalExperiment::runCrossModalTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Collect sensory inputs
    collectSensoryInput(trialNumber);
    
    // Integrate sensory inputs
    MultisensoryRepresentation representation = integrateSensoryInputs(sensoryInputs);
    
    // Update cross-modal learning
    updateCrossModalLearning(representation);
    
    // Resolve sensory conflicts
    resolveSensoryConflicts(sensoryInputs);
    
    // Record multisensory representation
    multisensoryRepresentations.push_back(representation);
    
    log("Cross-modal trial complete: integrated_features=" + std::to_string(representation.integratedFeatures.size()) +
        " confidence=" + std::to_string(representation.integrationConfidence));
    
    logEpisodeEnd(trialNumber, 0.0, 1, 0.0);  // Placeholder metrics
}

void CrossModalExperiment::collectSensoryInput(size_t trialNumber) {
    // Simulate collecting sensory inputs from different modalities
    std::vector<SensoryInput> inputs;
    
    // Visual input
    SensoryInput visualInput;
    visualInput.modality = "visual";
    visualInput.features = generateFeatures(5);  // Visual features
    visualInput.intensity = sensoryDist(rng);
    visualInput.reliability = sensoryDist(rng);
    visualInput.timestamp = static_cast<float>(trialNumber);
    inputs.push_back(visualInput);
    
    // Auditory input
    SensoryInput auditoryInput;
    auditoryInput.modality = "auditory";
    auditoryInput.features = generateFeatures(3);  // Auditory features
    auditoryInput.intensity = sensoryDist(rng);
    auditoryInput.reliability = sensoryDist(rng);
    auditoryInput.timestamp = static_cast<float>(trialNumber);
    inputs.push_back(auditoryInput);
    
    // Tactile input
    SensoryInput tactileInput;
    tactileInput.modality = "tactile";
    tactileInput.features = generateFeatures(4);  // Tactile features
    tactileInput.intensity = sensoryDist(rng);
    tactileInput.reliability = sensoryDist(rng);
    tactileInput.timestamp = static_cast<float>(trialNumber);
    inputs.push_back(tactileInput);
    
    // Add inputs to collection
    for (const auto& input : inputs) {
        sensoryInputs.push_back(input);
    }
}

CrossModalExperiment::MultisensoryRepresentation CrossModalExperiment::integrateSensoryInputs(
    const std::vector<SensoryInput>& inputs) {
    MultisensoryRepresentation representation;
    
    if (inputs.empty()) {
        return representation;
    }
    
    // Integrate features based on modality weights
    std::map<std::string, float> modalityWeights = {
        {"visual", config.visualSensoryWeight},
        {"auditory", config.auditorySensoryWeight},
        {"tactile", config.tactileSensoryWeight},
        {"olfactory", config.olfactorySensoryWeight},
        {"proprioceptive", config.proprioceptiveSensoryWeight}
    };
    
    // Calculate total weight
    float totalWeight = 0.0f;
    for (const auto& weight : modalityWeights) {
        totalWeight += weight.second;
    }
    
    // Normalize weights
    for (auto& weight : modalityWeights) {
        weight.second /= totalWeight;
    }
    
    // Integrate features
    std::vector<float> integratedFeatures;
    std::vector<std::string> contributingModalities;
    
    for (const auto& input : inputs) {
        auto it = modalityWeights.find(input.modality);
        if (it != modalityWeights.end()) {
            float weight = it->second;
            for (size_t i = 0; i < input.features.size() && i < config.multimodalRepresentationSize; ++i) {
                if (i < static_cast<size_t>(integratedFeatures.size())) {
                    integratedFeatures[i] += input.features[i] * weight;
                } else {
                    integratedFeatures.push_back(input.features[i] * weight);
                }
            }
            contributingModalities.push_back(input.modality);
        }
    }
    
    // Calculate integration confidence
    float integrationConfidence = 0.0f;
    for (const auto& input : inputs) {
        integrationConfidence += input.reliability;
    }
    integrationConfidence /= inputs.size();
    
    // Normalize integrated features
    for (float& feature : integratedFeatures) {
        feature = std::max(0.0f, std::min(1.0f, feature));
    }
    
    representation.integratedFeatures = integratedFeatures;
    representation.contributingModalities = contributingModalities;
    representation.integrationConfidence = integrationConfidence;
    representation.bindingStrength = config.multimodalBindingStrength;
    
    return representation;
}

void CrossModalExperiment::updateCrossModalLearning(const MultisensoryRepresentation& representation) {
    // Update cross-modal learning based on integrated representation
    float learningStrength = config.crossModalLearningRate * representation.integrationConfidence;
    
    // Update sensory integration strength
    sensoryIntegrationStrength *= (1.0f + learningStrength);
    
    // Update multisensory binding quality
    multisensoryBindingQuality *= (1.0f + representation.bindingStrength * learningStrength);
    
    // Clamp values
    sensoryIntegrationStrength = std::min(2.0f, std::max(0.5f, sensoryIntegrationStrength));
    multisensoryBindingQuality = std::min(2.0f, std::max(0.5f, multisensoryBindingQuality));
}

void CrossModalExperiment::resolveSensoryConflicts(const std::vector<SensoryInput>& inputs) {
    // Resolve conflicts between sensory inputs
    std::map<std::string, float> conflictScores;
    
    for (const auto& input : inputs) {
        conflictScores[input.modality] += (1.0f - input.reliability) * input.intensity;
    }
    
    // Adjust modality weights based on conflicts
    for (auto& pair : conflictScores) {
        float weight = config.visualSensoryWeight;  // Default
        if (pair.first == "visual") weight = config.visualSensoryWeight;
        else if (pair.first == "auditory") weight = config.auditorySensoryWeight;
        else if (pair.first == "tactile") weight = config.tactileSensoryWeight;
        else if (pair.first == "olfactory") weight = config.olfactorySensoryWeight;
        else if (pair.first == "proprioceptive") weight = config.proprioceptiveSensoryWeight;
        
        // Reduce weight for conflicting modalities
        weight *= (1.0f - pair.second * config.sensoryConflictResolution);
    }
}

const std::vector<CrossModalExperiment::SensoryInput>& CrossModalExperiment::getSensoryInputs() const {
    return sensoryInputs;
}

const std::vector<CrossModalExperiment::MultisensoryRepresentation>& CrossModalExperiment::getMultisensoryRepresentations() const {
    return multisensoryRepresentations;
}

double CrossModalExperiment::calculateSensoryIntegrationAccuracy() const {
    if (sensoryInputs.empty()) return 0.0;
    
    double totalAccuracy = 0.0;
    for (const auto& input : sensoryInputs) {
        totalAccuracy += input.reliability * input.intensity;
    }
    
    return totalAccuracy / sensoryInputs.size();
}

double CrossModalExperiment::calculateCrossModalLearningEfficiency() const {
    if (multisensoryRepresentations.empty()) return 0.0;
    
    double totalEfficiency = 0.0;
    for (const auto& representation : multisensoryRepresentations) {
        totalEfficiency += representation.integrationConfidence * representation.bindingStrength;
    }
    
    return totalEfficiency / multisensoryRepresentations.size();
}

double CrossModalExperiment::calculateMultisensoryBindingStrength() const {
    return multisensoryBindingQuality;
}

std::shared_ptr<Brain> CrossModalExperiment::createBrain() {
    // TODO: Create appropriate brain for cross-modal experiment
    return nullptr;
}

std::shared_ptr<Environment> CrossModalExperiment::createEnvironment() {
    // TODO: Create appropriate environment for cross-modal experiment
    return nullptr;
}

bool CrossModalExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                         std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !sensoryInputs.empty();
}

bool CrossModalExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Cross-Modal Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Final Sensory Integration Strength: " << sensoryIntegrationStrength << std::endl;
    file << "Final Multisensory Binding Quality: " << multisensoryBindingQuality << std::endl;
    file << "Number of Sensory Inputs: " << sensoryInputs.size() << std::endl;
    file << std::endl;
    
    // Save sensory inputs
    file << "Sensory Inputs:" << std::endl;
    file << "Count: " << sensoryInputs.size() << std::endl;
    for (const auto& input : sensoryInputs) {
        file << "  " << input.modality << ":"
             << " Intensity=" << input.intensity
             << " Reliability=" << input.reliability
             << " Features=" << input.features.size()
             << std::endl;
    }
    
    // Save multisensory representations
    file << std::endl;
    file << "Multisensory Representations:" << std::endl;
    file << "Count: " << multisensoryRepresentations.size() << std::endl;
    for (const auto& representation : multisensoryRepresentations) {
        file << "  Integrated Features=" << representation.integratedFeatures.size()
             << " Contributing Modalities=" << representation.contributingModalities.size()
             << " Confidence=" << representation.integrationConfidence
             << " BindingStrength=" << representation.bindingStrength
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Sensory Integration Strength: " << sensoryIntegrationStrength << std::endl;
    file << "  Multisensory Binding Quality: " << multisensoryBindingQuality << std::endl;
    file << "  Sensory Integration Accuracy: " << calculateSensoryIntegrationAccuracy() << std::endl;
    file << "  Cross-Modal Learning Efficiency: " << calculateCrossModalLearningEfficiency() << std::endl;
    file << "  Multisensory Binding Strength: " << calculateMultisensoryBindingStrength() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool CrossModalExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double CrossModalExperiment::getProgress() const {
    return sensoryInputs.empty() ? 0.0 : 
           static_cast<double>(sensoryInputs.size()) / config.numTrials;
}

std::string CrossModalExperiment::getStatus() const {
    if (sensoryInputs.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

float CrossModalExperiment::getSensoryIntegrationStrength() const {
    return sensoryIntegrationStrength;
}

float CrossModalExperiment::getMultisensoryBindingQuality() const {
    return multisensoryBindingQuality;
}

std::vector<float> CrossModalExperiment::generateFeatures(size_t count) {
    std::vector<float> features;
    for (size_t i = 0; i < count; ++i) {
        features.push_back(featureDist(rng));
    }
    return features;
}

} // namespace nlm