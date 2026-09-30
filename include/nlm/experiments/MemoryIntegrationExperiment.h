#pragma once

#include "AdvancedExperiment.h"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <unordered_map>

namespace nlm {

/**
 * Memory Integration Experiment
 * 
 * Advanced experiment that tests memory system integration with neural processing,
 * episodic memory formation, spatial representation, and memory replay systems.
 * Demonstrates how memory systems influence learning and behavior.
 */
class MemoryIntegrationExperiment : public AdvancedExperiment {
public:
    // Memory integration configuration
    struct MemoryConfig {
        // Episodic memory parameters
        float episodicMemoryCapacity;
        float episodicRecallAccuracy;
        uint64_t memoryConsolidationRate;
        float memoryDecayRate;
        
        // Spatial memory parameters
        float spatialRepresentationSize;
        float spatialMapResolution;
        float pathIntegrationAccuracy;
        
        // Working memory parameters
        size_t workingMemoryCapacity;
        float workingMemoryRetention;
        float activeMaintenanceThreshold;
        
        // Memory integration parameters
        float memoryInfluencesLearning;
        float learningInfluencesMemory;
        float memoryReplayFrequency;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackMemoryFormation;
        bool trackMemoryRecall;
        bool trackSpatialMemory;
        
        MemoryConfig()
            : episodicMemoryCapacity(0.8f), episodicRecallAccuracy(0.7f),
              memoryConsolidationRate(1000), memoryDecayRate(0.001f),
              spatialRepresentationSize(10.0f), spatialMapResolution(100),
              pathIntegrationAccuracy(0.9f), workingMemoryCapacity(7),
              workingMemoryRetention(0.6f), activeMaintenanceThreshold(0.8f),
              memoryInfluencesLearning(1.0f), learningInfluencesMemory(1.0f),
              memoryReplayFrequency(0.1f), numTrials(50), timeLimit(300.0f),
              trackMemoryFormation(true), trackMemoryRecall(true),
              trackSpatialMemory(true) {}
    };
    
    // Memory episode
    struct MemoryEpisode {
        uint64_t episodeId;
        std::chrono::steady_clock::time_point timestamp;
        float reward;
        std::string context;
        std::vector<float> spatialRepresentation;
        
        MemoryEpisode() : episodeId(0), reward(0.0f) {}
    };
    
    // Spatial memory map
    struct SpatialMemoryMap {
        size_t gridSize;
        std::vector<std::vector<float>> occupancyGrid;
        std::vector<std::vector<float>> landmarkMap;
        std::vector<std::vector<float>> pathMap;
        
        SpatialMemoryMap() : gridSize(100) {
            occupancyGrid.resize(gridSize, std::vector<float>(gridSize, 0.0f));
            landmarkMap.resize(gridSize, std::vector<float>(gridSize, 0.0f));
            pathMap.resize(gridSize, std::vector<float>(gridSize, 0.0f));
        }
    };
    
    MemoryIntegrationExperiment();
    ~MemoryIntegrationExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setMemoryConfig(const MemoryConfig& config);
    const MemoryConfig& getMemoryConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Memory integration specific methods
    void runMemoryIntegrationTrial(size_t trialNumber);
    void storeEpisodicMemory(uint64_t episodeId, float reward, const std::string& context);
    void recallEpisodicMemory(uint64_t episodeId);
    void updateSpatialMemory(const std::vector<float>& position);
    void performMemoryReplay();
    void integrateMemoryWithLearning();
    
    // Results management
    const std::vector<MemoryEpisode>& getMemoryEpisodes() const;
    const SpatialMemoryMap& getSpatialMemoryMap() const;
    double calculateEpisodicMemoryAccuracy() const;
    double calculateSpatialMemoryAccuracy() const;
    double calculateWorkingMemoryCapacity() const;
    
    // Brain and environment setup
    std::shared_ptr<Brain> createBrain() override;
    std::shared_ptr<Environment> createEnvironment() override;
    
    // Experiment execution
    bool runExperiment(std::shared_ptr<Brain> brain, 
                       std::shared_ptr<Environment> environment) override;
    
    // Results saving/loading
    bool saveResults(const std::string& filepath) override;
    bool loadResults(const std::string& filepath) override;
    
    // Progress reporting
    double getProgress() const override;
    std::string getStatus() const override;
    
    // Memory integration specific getters
    size_t getEpisodicMemorySize() const;
    float getMemoryRetentionRate() const;
    
private:
    // Configuration
    MemoryConfig config;
    
    // State tracking
    std::vector<MemoryEpisode> memoryEpisodes;
    SpatialMemoryMap spatialMemoryMap;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Memory integration state
    uint64_t nextEpisodeId;
    std::vector<float> currentPosition;
    float memoryRetention;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> rewardDist;
    std::uniform_real_distribution<double> spatialDist;
};

} // namespace nlm