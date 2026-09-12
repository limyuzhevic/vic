#pragma once

// Checkpoint data structures for the CheckpointSystem
// These match the structures referenced in Brain.cpp

namespace nlm {

// Connectivity checkpoint data
struct ConnectivityCheckpointData {
    std::vector<uint64_t> sourceRegion;
    std::vector<uint64_t> targetRegion;
    std::vector<float> weight;
    std::vector<uint32_t> delay;
    std::vector<uint8_t> plasticityFlags;
};

// Spike history checkpoint data
struct SpikeRecord {
    uint64_t neuronId;
    double time;
    uint64_t step;
    bool isDelayed;
    uint64_t synapseId;
};

struct SpikeHistoryCheckpointData {
    std::vector<SpikeRecord> spikes;
};

// Plasticity state checkpoint data
struct STDPState {
    float weightChange;
    float adaptation;
};

struct HebbianState {
    float eligibilityTrace;
};

struct PlasticityCheckpointData {
    bool hasSTDP;
    bool hasHebbian;
    STDPState stdpState;
    HebbianState hebbianState;
};

// Development checkpoint data
struct DevelopmentCheckpointData {
    DevelopmentalStage stage;
    float plasticityFactor;
};

// Random generator state checkpoint data
struct RandomCheckpointData {
    std::vector<uint8_t> state;
};

// Neuromodulation checkpoint data
struct NeuromodulationCheckpointData {
    float dopamineLevel;
    float dopaminePlasticityFactor;
    float curiosityLevel;
    float predictionErrorLevel;
    float noveltyLevel;
};

// Memory system checkpoint data
struct MemoryCheckpointData {
    size_t workingMemoryActiveTraces;
    size_t episodicMemoryEpisodeCount;
};

// Simulation checkpoint data
struct SimulationCheckpointData {
    uint64_t currentStep;
    double currentTime;
    DevelopmentalStage developmentalStage;
};

// Environment checkpoint data
struct EnvironmentCheckpointData {
    bool hasSensoryInput;
    // Additional environment state can be added here
};

} // namespace nlm
