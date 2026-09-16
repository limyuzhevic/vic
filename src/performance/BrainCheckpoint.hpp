#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Neuromodulation checkpoint data
struct NeuromodulationCheckpointData {
    std::vector<float> dopamineLevels;
    std::vector<float> curiosityLevels;
    std::vector<float> predictionErrorLevels;
    std::vector<float> noveltyLevels;
    std::vector<float> plasticityFactors;
};

// Memory checkpoint data
struct MemoryCheckpointData {
    std::vector<float> workingMemoryTraces;
    std::vector<std::vector<float>> episodicMemoryEpisodes;
    std::vector<std::vector<float>> associativeMemoryConnections;
    std::vector<std::string> semanticMemoryFacts;
};

// Prediction system checkpoint data
struct PredictionCheckpointData {
    std::vector<float> predictionErrors;
    std::vector<float> predictionConfidences;
    std::vector<float> predictionWeights;
    std::vector<float> predictionBiases;
};

// Cognition checkpoint data
struct CognitionCheckpointData {
    std::vector<float> plannerActionProbabilities;
    std::vector<float> conceptFormationWeights;
    std::vector<float> attentionalWeights;
    std::vector<float> attentionGains;
    std::vector<float> attentionThresholds;
    std::vector<float> planningDepths;
    std::vector<float> planningConfidences;
    std::vector<float> goalStates;
};

// Development system checkpoint data
struct DevelopmentCheckpointData {
    std::vector<float> developmentalStage;
    std::vector<float> plasticityRates;
    std::vector<float> synaptogenesisRates;
    std::vector<float> pruningRates;
};

// Simulation state checkpoint data
struct SimulationCheckpointData {
    std::vector<float> neuromodulationLevels;
    std::vector<float> rewardSignals;
    std::vector<float> predictionErrors;
    std::vector<float> attentionWeights;
    std::vector<float> conceptActivation;
    std::vector<float> memoryActivation;
};

// Complete brain state checkpoint data
struct BrainCheckpointData {
    NeuromodulationCheckpointData neuromodulation;
    MemoryCheckpointData memory;
    PredictionCheckpointData prediction;
    CognitionCheckpointData cognition;
    DevelopmentCheckpointData development;
    SimulationCheckpointData simulation;
    float currentTime;
    uint64_t currentStep;
    float totalSpikeCount;
};

} // namespace nlm