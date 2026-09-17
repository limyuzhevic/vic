// Phase 6 Integration - Main Configuration
// 
// This file contains the configuration for Phase 6 integration experiments
// It provides a unified interface for all integrated systems

#pragma once

#include <string>

namespace nlm {

struct Phase6Config {
    // Brain parameters
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Memory system parameters
    size_t maxWorkingMemoryCapacity = 100;
    size_t maxEpisodicMemorySize = 1000;
    bool enableEpisodicReplay = true;
    
    // Prediction system parameters
    bool enablePrediction = true;
    size_t predictionHistorySize = 100;
    
    // Cognitive system parameters
    bool enablePlanning = true;
    size_t planningDepth = 5;
    bool enableConceptFormation = true;
    bool enableAttention = true;
    
    // Neuromodulation parameters
    bool enableDopamine = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool enablePredictionError = true;
    
    // Development parameters
    bool enableDevelopment = true;
    bool enableSynaptogenesis = true;
    bool enablePruning = true;
    bool enableMaturation = true;
    
    // Performance parameters
    bool enableCheckpointing = true;
    std::string checkpointDir = "./checkpoints";
    size_t maxCheckpoints = 10;
    bool enableReplay = true;
    size_t replayInterval = 100;
    
    // Experiment parameters
    size_t maxSteps = 10000;
    size_t episodeLimit = 1000;
    float explorationRate = 0.1f;
    
    // Debug/Logging
    bool verboseLogging = false;
    bool enableMetrics = true;
    std::string metricsFile = "metrics.csv";
};

} // namespace nlm