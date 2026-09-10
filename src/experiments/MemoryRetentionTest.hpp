// MemoryRetentionTest.hpp - Comprehensive memory retention test

#pragma once

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../memory/Memory.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

namespace nlm {

// Structure to track memory retention metrics over time
struct MemoryRetentionMetrics {
    SimulationStep step;
    
    // Working memory metrics
    size_t workingMemoryTraces;
    float workingMemoryActivity;
    std::vector<float> workingMemoryPattern;
    float workingMemoryDecayRate;
    
    // Episodic memory metrics
    size_t episodicMemoryEpisodes;
    std::vector<float> episodicMemoryAgeDistribution;
    std::vector<float> episodicMemoryRewardDistribution;
    std::vector<std::vector<float>> episodicMemoryPatterns;
    
    // Memory system health
    float consolidationEfficiency;
    float replayEffectiveness;
    float patternStability;
    
    // Integration metrics
    float memoryNetworkCohesion;
    float crossSystemSynergy;
    
    MemoryRetentionMetrics()
        : step(0)
        , workingMemoryTraces(0)
        , workingMemoryActivity(0.0f)
        , workingMemoryDecayRate(0.01f)
        , episodicMemoryEpisodes(0)
        , consolidationEfficiency(0.0f)
        , replayEffectiveness(0.0f)
        , patternStability(0.0f)
        , memoryNetworkCohesion(0.0f)
        , crossSystemSynergy(0.0f) {}
};

// Comprehensive memory retention test
class MemoryRetentionTest {
public:
    MemoryRetentionTest();
    ~MemoryRetentionTest();
    
    // Run the memory retention test
    bool runTest();
    
    // Get detailed retention statistics
    std::string getRetentionStatistics() const;
    
    // Get memory retention metrics over time
    const std::vector<MemoryRetentionMetrics>& getMetrics() const { return metrics_; }
    
    // Get summary statistics
    struct Summary {
        float avgWorkingMemoryRetention;
        float avgEpisodicMemoryRetention;
        float peakWorkingMemoryActivity;
        float peakEpisodicMemoryActivity;
        float totalWorkingMemoryDecay;
        float totalEpisodicMemoryGrowth;
        float avgConsolidationEfficiency;
        float avgReplayEffectiveness;
        float avgPatternStability;
        float avgNetworkCohesion;
        float avgSystemSynergy;
        size_t totalSteps;
        
        Summary()
            : avgWorkingMemoryRetention(0.0f)
            , avgEpisodicMemoryRetention(0.0f)
            , peakWorkingMemoryActivity(0.0f)
            , peakEpisodicMemoryActivity(0.0f)
            , totalWorkingMemoryDecay(0.0f)
            , totalEpisodicMemoryGrowth(0.0f)
            , avgConsolidationEfficiency(0.0f)
            , avgReplayEffectiveness(0.0f)
            , avgPatternStability(0.0f)
            , avgNetworkCohesion(0.0f)
            , avgSystemSynergy(0.0f)
            , totalSteps(0) {}
    };
    
    Summary getSummary() const;
    
private:
    // Test helper methods
    void setupTestEnvironment();
    void initializeBrain();
    void initializeAgent();
    void runExtendedSimulation();
    void trackMemoryRetention();
    void analyzePatternStability();
    void analyzeConsolidation();
    void analyzeReplay();
    void analyzeIntegration();
    
    // Memory-specific tracking methods
    void trackWorkingMemoryRetention(const std::shared_ptr<Brain>& brain, SimulationStep step);
    void trackEpisodicMemoryRetention(const std::shared_ptr<Brain>& brain, SimulationStep step);
    void trackMemoryDecay(const std::shared_ptr<Brain>& brain, SimulationStep step, float decayRate);
    void trackMemoryReplay(const std::shared_ptr<Brain>& brain, SimulationStep step);
    
    // Metrics collection
    std::vector<MemoryRetentionMetrics> metrics_;
    Metrics metricsCollector_;
    
    // Test configuration
    struct Config {
        uint64_t totalSteps;
        uint64_t memoryTrackingInterval;
        uint64_t consolidationInterval;
        uint64_t replayInterval;
        uint64_t decayInterval;
        uint64_t patternAnalysisInterval;
        size_t neuronCount;
        float workingMemoryDecayRate;
        float episodicMemoryDecayRate;
        bool enableVerboseLogging;
        
        Config()
            : totalSteps(5000)
            , memoryTrackingInterval(10)
            , consolidationInterval(500)
            , replayInterval(100)
            , decayInterval(200)
            , patternAnalysisInterval(250)
            , neuronCount(500)
            , workingMemoryDecayRate(0.005f)
            , episodicMemoryDecayRate(0.002f)
            , enableVerboseLogging(true) {}
    } config_;
    
    // Test state
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> brainConfig_;
    SimpleWorld world_;
    AgentBrain agent_;
    bool testInitialized_;
    
    // Pattern tracking for retention analysis
    std::unordered_map<SimulationStep, std::vector<float>> patternHistory_;
    std::vector<float> patternStabilityScores_;
    std::vector<float> consolidationEfficiencyScores_;
    std::vector<float> replayEffectivenessScores_;
};