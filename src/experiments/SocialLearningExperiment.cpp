#include "SocialLearningExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

SocialLearningExperiment::SocialLearningExperiment()
    : currentGroupSize(5), socialCooperationLevel(0.6f), socialConformityPressure(0.5f),
      rng(std::random_device()()), socialDist(0.0, 1.0), interactionDist(0.0, 1.0),
      adoptionDist(0.0, 1.0) {
    setName("Social Learning Experiment");
    setDescription("Tests social learning and interaction with other agents, including imitation and cultural transmission");
}

bool SocialLearningExperiment::initialize() {
    log("Initializing Social Learning Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    if (config.groupSize == 0) {
        log("ERROR: groupSize must be greater than 0");
        return false;
    }
    
    // Initialize social state
    currentGroupSize = config.groupSize;
    socialCooperationLevel = config.cooperativeBehavior;
    socialConformityPressure = config.conformityPressure;
    
    socialInteractions.clear();
    culturalTransmissions.clear();
    
    log("Social Learning Experiment initialized successfully");
    log("  - Group Size: " + std::to_string(currentGroupSize));
    log("  - Imitation Accuracy: " + std::to_string(config.imitationAccuracy));
    log("  - Social Influence: " + std::to_string(config.socialInfluence));
    log("  - Cooperative Behavior: " + std::to_string(config.cooperativeBehavior));
    
    return true;
}

void SocialLearningExperiment::setup() {
    log("Setting up Social Learning Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Group Size: " + std::to_string(currentGroupSize));
    log("  - Social Influence: " + std::to_string(config.socialInfluence));
    log("  - Peer Pressure: " + std::to_string(config.peerPressure));
    log("  - Cultural Transmission Rate: " + std::to_string(config.culturalTransmissionRate));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void SocialLearningExperiment::run() {
    log("Starting Social Learning Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runSocialLearningTrial(trial);
        
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
    log("Social Learning Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void SocialLearningExperiment::analyzeResults() {
    log("Analyzing Social Learning Experiment Results");
    
    if (socialInteractions.empty()) {
        log("ERROR: No social interactions to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalInteractionScore = 0.0;
    double totalLearningOutcome = 0.0;
    double totalCulturalTransmission = 0.0;
    
    for (const auto& interaction : socialInteractions) {
        totalInteractionScore += interaction.interactionStrength;
        totalLearningOutcome += interaction.learningOutcome;
    }
    
    for (const auto& transmission : culturalTransmissions) {
        totalCulturalTransmission += transmission.transmissionStrength;
    }
    
    size_t numInteractions = socialInteractions.size();
    size_t numTransmissions = culturalTransmissions.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_interaction_score", totalInteractionScore);
    results.recordMetric("average_interaction_score", totalInteractionScore / numInteractions);
    results.recordMetric("average_learning_outcome", totalLearningOutcome / numInteractions);
    results.recordMetric("total_cultural_transmission", totalCulturalTransmission);
    results.recordMetric("average_cultural_transmission", totalCulturalTransmission / numTransmissions);
    results.recordMetric("social_cooperation_level", socialCooperationLevel);
    results.recordMetric("social_conformity_pressure", socialConformityPressure);
    
    // Calculate social learning metrics
    double imitationAccuracy = calculateImitationAccuracy();
    double socialLearningEfficiency = calculateSocialLearningEfficiency();
    double culturalTransmissionStrength = calculateCulturalTransmissionStrength();
    
    results.recordMetric("imitation_accuracy", imitationAccuracy);
    results.recordMetric("social_learning_efficiency", socialLearningEfficiency);
    results.recordMetric("cultural_transmission_strength", culturalTransmissionStrength);
    
    // Record results
    recordResults(results);
    
    log("Social Learning Experiment analysis complete");
    log("  - Imitation Accuracy: " + std::to_string(imitationAccuracy));
    log("  - Social Learning Efficiency: " + std::to_string(socialLearningEfficiency));
    log("  - Cultural Transmission Strength: " + std::to_string(culturalTransmissionStrength));
}

void SocialLearningExperiment::cleanup() {
    log("Cleaning up Social Learning Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    socialInteractions.clear();
    culturalTransmissions.clear();
    
    log("Social Learning Experiment cleanup complete");
}

void SocialLearningExperiment::setSocialConfig(const SocialConfig& config) {
    this->config = config;
    currentGroupSize = config.groupSize;
    socialCooperationLevel = config.cooperativeBehavior;
    socialConformityPressure = config.conformityPressure;
}

const SocialLearningExperiment::SocialConfig& SocialLearningExperiment::getSocialConfig() const {
    return config;
}

void SocialLearningExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void SocialLearningExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void SocialLearningExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void SocialLearningExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void SocialLearningExperiment::runSocialLearningTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Simulate social interactions
    SocialInteraction interaction;
    interaction.step = trialNumber;
    
    // Determine interaction type
    double interactionType = socialDist(rng);
    if (interactionType < 0.4) {
        interaction.interactionType = "imitation";
        processImitationLearning(trialNumber);
    } else if (interactionType < 0.7) {
        interaction.interactionType = "observation";
        processObservationLearning(trialNumber);
    } else if (interactionType < 0.85) {
        interaction.interactionType = "cooperation";
        processCooperativeLearning(trialNumber);
    } else {
        interaction.interactionType = "competition";
        processCompetitiveLearning(trialNumber);
    }
    
    // Simulate interaction
    interaction.interactionStrength = interactionDist(rng);
    interaction.learningOutcome = std::min(1.0, interaction.interactionStrength * adoptionDist(rng));
    interaction.agentBehavior = "adapted_behavior";
    interaction.peerBehavior = "peer_behavior";
    
    socialInteractions.push_back(interaction);
    
    updateCulturalTransmission(trialNumber);
    
    log("Social learning trial complete: interaction_type=" + interaction.interactionType +
        " strength=" + std::to_string(interaction.interactionStrength) +
        " outcome=" + std::to_string(interaction.learningOutcome));
    
    logEpisodeEnd(trialNumber, interaction.learningOutcome, 1, 0.0);  // Placeholder metrics
}

void SocialLearningExperiment::processImitationLearning(size_t trialNumber) {
    // Simulate imitation learning from peer behaviors
    double imitationSuccess = std::min(1.0, config.imitationAccuracy * socialDist(rng));
    // Update social cooperation based on successful imitation
    socialCooperationLevel = std::min(1.0f, socialCooperationLevel + static_cast<float>(imitationSuccess) * 0.1f);
}

void SocialLearningExperiment::processObservationLearning(size_t trialNumber) {
    // Simulate observation learning from peer behaviors
    double observationLearning = config.observationLearningRate * socialDist(rng);
    // Update social influence based on observation learning
    // socialInfluence = std::min(1.0f, socialInfluence + static_cast<float>(observationLearning) * 0.1f);
}

void SocialLearningExperiment::processCooperativeLearning(size_t trialNumber) {
    // Simulate cooperative learning and mutual benefit
    double cooperationBenefit = config.cooperativeBehavior * socialDist(rng);
    // Increase social cooperation level
    socialCooperationLevel = std::min(1.0f, socialCooperationLevel + static_cast<float>(cooperationBenefit) * 0.05f);
}

void SocialLearningExperiment::processCompetitiveLearning(size_t trialNumber) {
    // Simulate competitive learning and competition
    double competitionEffect = config.competitiveBehavior * socialDist(rng);
    // May decrease social cooperation due to competition
    socialCooperationLevel = std::max(0.0f, socialCooperationLevel - static_cast<float>(competitionEffect) * 0.05f);
}

void SocialLearningExperiment::updateCulturalTransmission(size_t trialNumber) {
    // Update cultural transmission based on social learning
    CulturalTransmission transmission;
    transmission.behaviorPattern = "learned_behavior";
    transmission.transmissionStrength = socialDist(rng);
    transmission.adoptionRate = adoptionDist(rng);
    transmission.persistenceDuration = 10;  // Placeholder
    transmission.lastObserved = trialNumber;
    
    culturalTransmissions.push_back(transmission);
}

const std::vector<SocialLearningExperiment::SocialInteraction>& SocialLearningExperiment::getSocialInteractions() const {
    return socialInteractions;
}

const std::vector<SocialLearningExperiment::CulturalTransmission>& SocialLearningExperiment::getCulturalTransmissions() const {
    return culturalTransmissions;
}

double SocialLearningExperiment::calculateImitationAccuracy() const {
    if (socialInteractions.empty()) return 0.0;
    
    double totalAccuracy = 0.0;
    size_t imitationCount = 0;
    
    for (const auto& interaction : socialInteractions) {
        if (interaction.interactionType == "imitation") {
            imitationCount++;
            totalAccuracy += interaction.learningOutcome;
        }
    }
    
    return imitationCount > 0 ? totalAccuracy / imitationCount : 0.0;
}

double SocialLearningExperiment::calculateSocialLearningEfficiency() const {
    if (socialInteractions.empty()) return 0.0;
    
    double totalEfficiency = 0.0;
    for (const auto& interaction : socialInteractions) {
        totalEfficiency += interaction.learningOutcome * interaction.interactionStrength;
    }
    
    return totalEfficiency / socialInteractions.size();
}

double SocialLearningExperiment::calculateCulturalTransmissionStrength() const {
    if (culturalTransmissions.empty()) return 0.0;
    
    double totalStrength = 0.0;
    for (const auto& transmission : culturalTransmissions) {
        totalStrength += transmission.transmissionStrength;
    }
    
    return totalStrength / culturalTransmissions.size();
}

std::shared_ptr<Brain> SocialLearningExperiment::createBrain() {
    // TODO: Create appropriate brain for social learning experiment
    return nullptr;
}

std::shared_ptr<Environment> SocialLearningExperiment::createEnvironment() {
    // TODO: Create appropriate environment for social learning experiment
    return nullptr;
}

bool SocialLearningExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                             std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !socialInteractions.empty();
}

bool SocialLearningExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Social Learning Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Final Social Cooperation Level: " << socialCooperationLevel << std::endl;
    file << "Final Social Conformity Pressure: " << socialConformityPressure << std::endl;
    file << "Group Size: " << currentGroupSize << std::endl;
    file << "Number of Social Interactions: " << socialInteractions.size() << std::endl;
    file << std::endl;
    
    // Save social interactions
    file << "Social Interactions:" << std::endl;
    file << "Count: " << socialInteractions.size() << std::endl;
    for (const auto& interaction : socialInteractions) {
        file << "  Step " << interaction.step << ":"
             << " Type=" << interaction.interactionType
             << " Strength=" << interaction.interactionStrength
             << " Outcome=" << interaction.learningOutcome
             << std::endl;
    }
    
    // Save cultural transmissions
    file << std::endl;
    file << "Cultural Transmissions:" << std::endl;
    file << "Count: " << culturalTransmissions.size() << std::endl;
    for (const auto& transmission : culturalTransmissions) {
        file << "  Pattern=" << transmission.behaviorPattern
             << " Strength=" << transmission.transmissionStrength
             << " AdoptionRate=" << transmission.adoptionRate
             << std::endl;
    }
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Social Cooperation Level: " << socialCooperationLevel << std::endl;
    file << "  Social Conformity Pressure: " << socialConformityPressure << std::endl;
    file << "  Imitation Accuracy: " << calculateImitationAccuracy() << std::endl;
    file << "  Social Learning Efficiency: " << calculateSocialLearningEfficiency() << std::endl;
    file << "  Cultural Transmission Strength: " << calculateCulturalTransmissionStrength() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool SocialLearningExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double SocialLearningExperiment::getProgress() const {
    return socialInteractions.empty() ? 0.0 : 
           static_cast<double>(socialInteractions.size()) / config.numTrials;
}

std::string SocialLearningExperiment::getStatus() const {
    if (socialInteractions.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

float SocialLearningExperiment::getSocialCooperationLevel() const {
    return socialCooperationLevel;
}

float SocialLearningExperiment::getSocialConformityPressure() const {
    return socialConformityPressure;
}

size_t SocialLearningExperiment::getGroupSize() const {
    return currentGroupSize;
}

} // namespace nlm