// Phase 6 Integration Results
// 
// Data structure for storing results from Phase 6 integration experiments

#pragma once

#include <chrono>

namespace nlm {

struct Phase6Results {
    // Basic metrics
    double totalWallClockTime;
    double totalSimulationTime;
    
    // Brain performance
    double avgFiringRate;
    size_t totalSpikesGenerated;
    float eiRatio;
    
    // Memory metrics
    size_t memoryWorkingMemoryIntegrated;
    size_t memoryEpisodicMemoryIntegrated;
    size_t memoryAssociativeMemoryIntegrated;
    size_t memoryEpisodicMemoryStored;
    size_t memoryReplayEvents;
    
    // Neuromodulation metrics
    size_t neuromodulationIntegrated;
    double dopamineLevel;
    double curiosityLevel;
    double noveltyLevel;
    double predictionErrorLevel;
    
    // Cognitive metrics
    size_t predictionIntegrated;
    size_t planningIntegrated;
    size_t conceptFormationIntegrated;
    size_t attentionIntegrated;
    size_t selfModelIntegrated;
    size_t socialLearningIntegrated;
    
    // Development metrics
    size_t developmentIntegrated;
    size_t synaptogenesisEvents;
    size_t pruningEvents;
    size_t maturationEvents;
    
    // Learning metrics
    double totalReward;
    double learningRate;
    size_t episodesLearned;
    size_t conceptsDiscovered;
    
    // Persistence metrics
    size_t checkpointingWorks;
    size_t checkpointsSaved;
    size_t checkpointsLoaded;
    size_t replayEvents;
    
    // System health
    bool integrationComplete;
    bool noMemoryLeak;
    bool allSystemsConnected;
    
    // Advanced metrics
    size_t patternCompletionAccuracy;
    size_t predictionAccuracy;
    size_t planningSuccessRate;
    size_t attentionFocusScore;
};

} // namespace nlm