#include "MemoryIntegrationExperiment.h"
#include "../experiments/ExperimentResults.h"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

MemoryIntegrationExperiment::MemoryIntegrationExperiment()
    : nextEpisodeId(0), memoryRetention(0.6f), rng(std::random_device()()),
      rewardDist(0.0, 10.0), spatialDist(0.0, 10.0) {
    setName("Memory Integration Experiment");
    setDescription("Tests memory system integration with neural processing and episodic memory formation");
}

bool MemoryIntegrationExperiment::initialize() {
    log("Initializing Memory Integration Experiment");
    
    // Validate configuration
    if (config.numTrials == 0) {
        log("ERROR: numTrials must be greater than 0");
        return false;
    }
    
    // Initialize memory state
    nextEpisodeId = 0;
    memoryRetention = config.workingMemoryRetention;
    currentPosition = {5.0f, 5.0f};  // Start at center
    
    memoryEpisodes.clear();
    
    log("Memory Integration Experiment initialized successfully");
    log("  - Episodic Memory Capacity: " + std::to_string(config.episodicMemoryCapacity));
    log("  - Working Memory Capacity: " + std::to_string(config.workingMemoryCapacity));
    log("  - Memory Consolidation Rate: " + std::to_string(config.memoryConsolidationRate));
    log("  - Spatial Map Resolution: " + std::to_string(config.spatialMapResolution));
    
    return true;
}

void MemoryIntegrationExperiment::setup() {
    log("Setting up Memory Integration Experiment");
    
    // Initialize brain and environment
    currentBrain = createBrain();
    currentEnvironment = createEnvironment();
    
    if (!currentBrain || !currentEnvironment) {
        log("ERROR: Failed to create brain or environment");
        return;
    }
    
    // Log experiment setup
    log("Experiment setup complete:")
        log("  - Episodic Recall Accuracy: " + std::to_string(config.episodicRecallAccuracy));
    log("  - Spatial Map Resolution: " + std::to_string(config.spatialMapResolution));
    log("  - Working Memory Capacity: " + std::to_string(config.workingMemoryCapacity));
    log("  - Number of Trials: " + std::to_string(config.numTrials));
    log("  - Time Limit: " + std::to_string(config.timeLimit) + " seconds");
}

void MemoryIntegrationExperiment::run() {
    log("Starting Memory Integration Experiment");
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t trial = 0; trial < config.numTrials && !shouldStop(); ++trial) {
        logStep(trial, 0.0, 0.0, 0.0, 0.0, "Initializing");
        runMemoryIntegrationTrial(trial);
        
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
    log("Memory Integration Experiment completed in " + std::to_string(totalTime) + " seconds");
}

void MemoryIntegrationExperiment::analyzeResults() {
    log("Analyzing Memory Integration Experiment Results");
    
    if (memoryEpisodes.empty()) {
        log("ERROR: No memory episodes to analyze");
        return;
    }
    
    // Calculate summary statistics
    double totalReward = 0.0;
    double totalRecallAccuracy = 0.0;
    
    for (const auto& episode : memoryEpisodes) {
        totalReward += episode.reward;
        // Simple recall accuracy calculation (placeholder)
        totalRecallAccuracy += config.episodicRecallAccuracy * (1.0 - episode.reward * 0.1);
    }
    
    size_t numEpisodes = memoryEpisodes.size();
    
    // Create experiment results
    ExperimentResults results;
    results.recordMetric("total_reward", totalReward);
    results.recordMetric("average_reward", totalReward / numEpisodes);
    results.recordMetric("episodic_memory_accuracy", totalRecallAccuracy / numEpisodes);
    results.recordMetric("memory_retention_rate", memoryRetention);
    
    // Calculate memory integration metrics
    double episodicMemoryAccuracy = calculateEpisodicMemoryAccuracy();
    double spatialMemoryAccuracy = calculateSpatialMemoryAccuracy();
    double workingMemoryCapacity = calculateWorkingMemoryCapacity();
    
    results.recordMetric("episodic_memory_accuracy", episodicMemoryAccuracy);
    results.recordMetric("spatial_memory_accuracy", spatialMemoryAccuracy);
    results.recordMetric("working_memory_capacity", workingMemoryCapacity);
    
    // Record results
    recordResults(results);
    
    log("Memory Integration Experiment analysis complete");
    log("  - Episodic Memory Accuracy: " + std::to_string(episodicMemoryAccuracy));
    log("  - Spatial Memory Accuracy: " + std::to_string(spatialMemoryAccuracy));
    log("  - Working Memory Capacity: " + std::to_string(workingMemoryCapacity));
}

void MemoryIntegrationExperiment::cleanup() {
    log("Cleaning up Memory Integration Experiment");
    
    currentBrain.reset();
    currentEnvironment.reset();
    memoryEpisodes.clear();
    
    log("Memory Integration Experiment cleanup complete");
}

void MemoryIntegrationExperiment::setMemoryConfig(const MemoryConfig& config) {
    this->config = config;
    memoryRetention = config.workingMemoryRetention;
}

const MemoryIntegrationExperiment::MemoryConfig& MemoryIntegrationExperiment::getMemoryConfig() const {
    return config;
}

void MemoryIntegrationExperiment::setNumNeurons(size_t neurons) {
    // TODO: Set brain configuration for specific neuron count
}

void MemoryIntegrationExperiment::setNumSynapses(size_t synapses) {
    // TODO: Set brain configuration for specific synapse count
}

void MemoryIntegrationExperiment::setNumTrials(size_t trials) {
    config.numTrials = trials;
}

void MemoryIntegrationExperiment::setTimeLimit(double limit) {
    config.timeLimit = limit;
}

void MemoryIntegrationExperiment::runMemoryIntegrationTrial(size_t trialNumber) {
    logEpisodeStart(trialNumber);
    
    // Generate trial parameters
    float reward = static_cast<float>(rewardDist(rng));
    
    // Update position
    std::vector<float> movement(2);
    movement[0] = static_cast<float>(spatialDist(rng)) - 5.0f;
    movement[1] = static_cast<float>(spatialDist(rng)) - 5.0f;
    
    currentPosition[0] += movement[0];
    currentPosition[1] += movement[1];
    
    // Update spatial memory
    updateSpatialMemory(currentPosition);
    
    // Create memory episode
    std::string context = "trial_" + std::to_string(trialNumber) + 
                          "_pos_" + std::to_string(currentPosition[0]) + "_" + 
                          std::to_string(currentPosition[1]);
    
    storeEpisodicMemory(nextEpisodeId++, reward, context);
    
    // Perform memory replay
    if (trialNumber % static_cast<size_t>(1.0 / config.memoryReplayFrequency) == 0) {
        performMemoryReplay();
    }
    
    // Integrate memory with learning
    integrateMemoryWithLearning();
    
    // Decay memory
    memoryRetention *= (1.0f - config.memoryDecayRate);
    
    log("Memory integration trial complete: reward=" + std::to_string(reward) + 
        " position=" + std::to_string(currentPosition[0]) + "," + std::to_string(currentPosition[1]));
    
    logEpisodeEnd(trialNumber, reward, 1, 0.0);  // Placeholder metrics
}

void MemoryIntegrationExperiment::storeEpisodicMemory(uint64_t episodeId, float reward, 
                                                     const std::string& context) {
    MemoryEpisode episode;
    episode.episodeId = episodeId;
    episode.timestamp = std::chrono::steady_clock::now();
    episode.reward = reward;
    episode.context = context;
    episode.spatialRepresentation = currentPosition;
    
    memoryEpisodes.push_back(episode);
    
    // Consolidate memory if capacity allows
    if (memoryEpisodes.size() > config.episodicMemoryCapacity * 10) {
        // Remove oldest episodes
        size_t targetSize = static_cast<size_t>(config.episodicMemoryCapacity * 10);
        if (memoryEpisodes.size() > targetSize) {
            memoryEpisodes.erase(memoryEpisodes.begin(), 
                                memoryEpisodes.begin() + (memoryEpisodes.size() - targetSize));
        }
    }
}

void MemoryIntegrationExperiment::recallEpisodicMemory(uint64_t episodeId) {
    for (const auto& episode : memoryEpisodes) {
        if (episode.episodeId == episodeId) {
            // Recall episode (placeholder)
            break;
        }
    }
}

void MemoryIntegrationExperiment::updateSpatialMemory(const std::vector<float>& position) {
    // Update spatial memory map with new position
    if (spatialMemoryMap.gridSize > 0 && position.size() >= 2) {
        size_t x = static_cast<size_t>((position[0] + 5.0f) * spatialMemoryMap.gridSize / 10.0f);
        size_t y = static_cast<size_t>((position[1] + 5.0f) * spatialMemoryMap.gridSize / 10.0f);
        
        if (x < spatialMemoryMap.gridSize && y < spatialMemoryMap.gridSize) {
            spatialMemoryMap.occupancyGrid[x][y] = 1.0f;
            spatialMemoryMap.pathMap[x][y] = 1.0f;
        }
    }
}

void MemoryIntegrationExperiment::performMemoryReplay() {
    // Replay recent memories to strengthen connections
    for (int i = std::min(static_cast<int>(memoryEpisodes.size()) - 5, 4); i >= 0; --i) {
        uint64_t episodeId = memoryEpisodes[static_cast<size_t>(i)].episodeId;
        // Replay memory (placeholder)
    }
}

void MemoryIntegrationExperiment::integrateMemoryWithLearning() {
    // Integrate memory with learning process
    // This would modify brain dynamics based on memory recall
}

const std::vector<MemoryIntegrationExperiment::MemoryEpisode>& MemoryIntegrationExperiment::getMemoryEpisodes() const {
    return memoryEpisodes;
}

const MemoryIntegrationExperiment::SpatialMemoryMap& MemoryIntegrationExperiment::getSpatialMemoryMap() const {
    return spatialMemoryMap;
}

double MemoryIntegrationExperiment::calculateEpisodicMemoryAccuracy() const {
    if (memoryEpisodes.empty()) return 0.0;
    
    double accuracy = 0.0;
    for (const auto& episode : memoryEpisodes) {
        // Simple accuracy calculation based on reward
        accuracy += (1.0 - std::abs(episode.reward / 10.0 - config.episodicRecallAccuracy));
    }
    
    return accuracy / memoryEpisodes.size();
}

double MemoryIntegrationExperiment::calculateSpatialMemoryAccuracy() const {
    if (spatialMemoryMap.gridSize == 0) return 0.0;
    
    double totalAccuracy = 0.0;
    for (size_t x = 0; x < spatialMemoryMap.gridSize; ++x) {
        for (size_t y = 0; y < spatialMemoryMap.gridSize; ++y) {
            totalAccuracy += spatialMemoryMap.occupancyGrid[x][y];
        }
    }
    
    return totalAccuracy / (spatialMemoryMap.gridSize * spatialMemoryMap.gridSize);
}

double MemoryIntegrationExperiment::calculateWorkingMemoryCapacity() const {
    // Calculate working memory capacity based on memory retention
    return std::min(1.0, memoryRetention * config.workingMemoryCapacity / 10.0);
}

std::shared_ptr<Brain> MemoryIntegrationExperiment::createBrain() {
    // TODO: Create appropriate brain for memory integration experiment
    return nullptr;
}

std::shared_ptr<Environment> MemoryIntegrationExperiment::createEnvironment() {
    // TODO: Create appropriate environment for memory integration experiment
    return nullptr;
}

bool MemoryIntegrationExperiment::runExperiment(std::shared_ptr<Brain> brain, 
                                                 std::shared_ptr<Environment> environment) {
    currentBrain = brain;
    currentEnvironment = environment;
    
    run(); // This calls the run() method which runs all trials
    analyzeResults(); // Analyze results after running
    
    return !memoryEpisodes.empty();
}

bool MemoryIntegrationExperiment::saveResults(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        log("ERROR: Could not open file for writing: " + filepath);
        return false;
    }
    
    // Save experiment configuration
    file << "Memory Integration Experiment Results" << std::endl;
    file << "===============================" << std::endl;
    file << "Experiment: " << getName() << std::endl;
    file << "Description: " << getDescription() << std::endl;
    file << "Final Memory Retention: " << memoryRetention << std::endl;
    file << "Number of Episodes: " << memoryEpisodes.size() << std::endl;
    file << std::endl;
    
    // Save memory episodes
    file << "Memory Episodes:" << std::endl;
    file << "Count: " << memoryEpisodes.size() << std::endl;
    for (const auto& episode : memoryEpisodes) {
        file << "  Episode " << episode.episodeId << ":"
             << " Reward=" << episode.reward
             << " Context=" << episode.context
             << " Position=(" << episode.spatialRepresentation[0] << "," << episode.spatialRepresentation[1] << ")"
             << std::endl;
    }
    
    // Save spatial memory map
    file << std::endl;
    file << "Spatial Memory Map:" << std::endl;
    file << "Grid Size: " << spatialMemoryMap.gridSize << std::endl;
    
    // Save summary statistics
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    file << "  Memory Retention Rate: " << memoryRetention << std::endl;
    file << "  Episodic Memory Accuracy: " << calculateEpisodicMemoryAccuracy() << std::endl;
    file << "  Spatial Memory Accuracy: " << calculateSpatialMemoryAccuracy() << std::endl;
    file << "  Working Memory Capacity: " << calculateWorkingMemoryCapacity() << std::endl;
    
    file.close();
    log("Results saved to: " + filepath);
    return true;
}

bool MemoryIntegrationExperiment::loadResults(const std::string& filepath) {
    // TODO: Implement result loading
    log("Loading results from: " + filepath);
    return false;
}

double MemoryIntegrationExperiment::getProgress() const {
    return memoryEpisodes.empty() ? 0.0 : 
           static_cast<double>(memoryEpisodes.size()) / config.numTrials;
}

std::string MemoryIntegrationExperiment::getStatus() const {
    if (memoryEpisodes.empty()) {
        return "Not started";
    }
    
    double progress = getProgress();
    if (progress >= 1.0) {
        return "Completed";
    }
    
    return "Running";
}

size_t MemoryIntegrationExperiment::getEpisodicMemorySize() const {
    return memoryEpisodes.size();
}

float MemoryIntegrationExperiment::getMemoryRetentionRate() const {
    return memoryRetention;
}

} // namespace nlm