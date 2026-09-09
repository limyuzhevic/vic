#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <deque>

namespace nlm {

/**
 * Working memory state for checkpointing
 */
struct WorkingMemoryCheckpointData {
    std::vector<NeuronId> memoryNeurons;
    std::vector<float> memoryActivations;
    std::vector<size_t> memoryTimestamps;
    size_t capacity;
    float decayRate;
};

/**
 * Episodic memory state for checkpointing
 */
struct EpisodicMemoryCheckpointData {
    std::deque<EpisodicMemoryItem> episodes;
    std::vector<NeuronId> episodeNeurons;
    size_t maxEpisodes;
    bool replayEnabled;
};

/**
 * Associative memory state for checkpointing
 */
struct AssociativeMemoryCheckpointData {
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
};

/**
 * Prediction system state for checkpointing
 */
struct PredictionSystemCheckpointData {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
};

/**
 * Neural planner state for checkpointing
 */
struct NeuralPlannerCheckpointData {
    std::vector<ActionType> recentPlanSuccess;
    size_t planningDepth;
    float planningConfidence;
    std::vector<float> actionQuality;
    std::vector<float> currentGoal;
};

/**
 * Concept formation state for checkpointing
 */
struct ConceptFormationCheckpointData {
    std::vector<float> conceptWeights;
    float formationThreshold;
    float stabilityThreshold;
    size_t stabilityWindow;
    size_t nextConceptId;
};

/**
 * Attentional selection state for checkpointing
 */
struct AttentionalSelectionCheckpointData {
    std::vector<NeuronId> winners;
    std::vector<RegionId> attendedRegions;
    std::vector<float> bottomUpSalience;
    std::vector<float> topDownBias;
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
};

/**
 * Development system state for checkpointing
 */
struct DevelopmentSystemCheckpointData {
    DevelopmentalStage stage;
    double age;
    float plasticityModifier;
    float criticalPeriodProgress;
};

/**
 * Neuromodulation system state for checkpointing
 */
struct NeuromodulationCheckpointData {
    // Dopamine
    float dopamineLevel;
    float dopamineBaseline;
    float dopaminePeak;
    float dopamineDecayRate;
    float dopamineReleaseRate;
    
    // Curiosity
    float curiosityLevel;
    
    // Prediction error
    float predictionErrorLevel;
    
    // Novelty
    float noveltyLevel;
};

/**
 * Structural plasticity state for checkpointing
 */
struct StructuralPlasticityCheckpointData {
    float synaptogenesisRate;
    float pruningRate;
    float minWeightThreshold;
    float activityThreshold;
    size_t maxSynapsesPerNeuron;
    size_t maxTotalSynapses;
    bool useActivityDependency;
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
};

/**
 * Spike system state for checkpointing
 */
struct SpikeSystemCheckpointData {
    std::vector<SpikeEvent> pendingSpikes;
    std::vector<DelayedSpikeEvent> pendingDelayedSpikes;
    size_t totalSpikes;
};

/**
 * Plasticity system state for checkpointing
 */
struct PlasticityCheckpointData {
    // STDP
    std::vector<std::tuple<uint64_t, uint64_t, float, float, float>> stdpTraces;
    
    // Hebbian
    std::vector<std::tuple<uint64_t, uint64_t, float>> hebbianTraces;
};

/**
 * Random generator state for checkpointing
 */
struct RandomStateCheckpointData {
    std::vector<uint64_t> seedHistory;
    size_t stepCount;
};

/**
 * Complete brain state for checkpointing
 */
struct BrainCheckpointData {
    // Core neural data
    NeuronCheckpointData neurons;
    SynapseCheckpointData synapses;
    std::vector<NeuronId> sensoryNeurons;
    std::vector<NeuronId> motorNeurons;
    
    // Integrated systems
    WorkingMemoryCheckpointData workingMemory;
    EpisodicMemoryCheckpointData episodicMemory;
    AssociativeMemoryCheckpointData associativeMemory;
    PredictionSystemCheckpointData predictionSystem;
    NeuralPlannerCheckpointData neuralPlanner;
    ConceptFormationCheckpointData conceptFormation;
    AttentionalSelectionCheckpointData attentionalSelection;
    DevelopmentSystemCheckpointData developmentSystem;
    NeuromodulationCheckpointData neuromodulation;
    StructuralPlasticityCheckpointData structuralPlasticity;
    SpikeSystemCheckpointData spikeSystem;
    PlasticityCheckpointData plasticity;
    
    // Brain metadata
    std::vector<std::string> regionNames;
    std::vector<RegionId> interRegionConnections;
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    DevelopmentalStage developmentalStage;
    RandomStateCheckpointData randomState;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
};

} // namespace nlm
