#pragma once

#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class WorkingMemoryTrace;
class FeatureVector;
class AttentionFocus;
class Goal;

// Experience structure for episodic memory
struct Experience {
    // Timestamp information
    uint64_t step;
    double timestamp;
    std::chrono::system_clock::time_point realTime;
    
    // State information
    struct State {
        uint64_t step;
        double timestamp;
        std::vector<std::vector<float>> neuralActivity;
        std::vector<std::vector<float>> sensoryInput;
        std::vector<float> motorOutput;
        
        State() : step(0), timestamp(0.0) {}
    } state;
    
    // Processing information
    struct Processing {
        double sensoryProcessingTime;
        double neuralProcessingTime;
        double motorPlanningTime;
        double attentionAllocation;
        double memoryEncodingStrength;
        
        Processing() : sensoryProcessingTime(0.0), neuralProcessingTime(0.0),
                      motorPlanningTime(0.0), attentionAllocation(0.0),
                      memoryEncodingStrength(0.0) {}
    } processing;
    
    // Outcome information
    struct Outcome {
        float reward;
        bool success;
        std::string outcomeDescription;
        std::vector<float> sensoryChanges;
        std::vector<float> motorChanges;
        
        Outcome() : reward(0.0f), success(false) {}
    } outcome;
    
    // Learning information
    struct Learning {
        float predictionError;
        float noveltyScore;
        float curiosityDrive;
        std::vector<float> synapticChanges;
        std::vector<std::string> conceptsFormed;
        
        Learning() : predictionError(0.0f), noveltyScore(0.0f), curiosityDrive(0.0f) {}
    } learning;
    
    // Metadata
    struct Metadata {
        std::string experienceId;
        std::string agentId;
        uint64_t episodeId;
        std::string contextDescription;
        std::vector<std::string> tags;
        
        Metadata() : episodeId(0) {}
    } metadata;
    
    // Constructors
    Experience() : step(0), timestamp(0.0) {}
    
    Experience(Brain* brain, uint64_t step, double timestamp) {
        this->step = step;
        this->timestamp = timestamp;
        this->realTime = std::chrono::system_clock::now();
        
        if (brain) {
            state = extractStateFromBrain(brain);
        }
    }
    
    // Helper methods
    static State extractStateFromBrain(Brain* brain);
    float calculateSalience() const;
    std::string getSummary() const;
    
    // Operators
    bool operator==(const Experience& other) const {
        return step == other.step && timestamp == other.timestamp;
    }
    
    bool operator!=(const Experience& other) const {
        return !(*this == other);
    }
};

// Brain state structure for checkpointing and working memory
struct BrainState {
    uint64_t step;
    double timestamp;
    
    // Neural state information
    struct NeuralState {
        std::vector<NeuronState> neurons;
        std::vector<SynapseState> synapses;
        std::vector<float> regionActivations;
        
        NeuralState() {}
    } neural;
    
    // Working memory state
    struct WorkingMemoryState {
        std::vector<WorkingMemoryTrace> activeTraces;
        std::vector<std::vector<float>> patterns;
        std::vector<float> traceSalience;
        
        WorkingMemoryState() {}
    } workingMemory;
    
    // Episodic memory state
    struct EpisodicMemoryState {
        std::vector<EpisodicMemoryEntry> episodes;
        std::vector<float> episodeImportance;
        std::vector<uint64_t> episodeSteps;
        
        EpisodicMemoryState() {}
    } episodicMemory;
    
    // Cognitive state
    struct CognitiveState {
        AttentionFocus currentFocus;
        std::vector<Goal> currentGoals;
        std::vector<std::string> currentConcepts;
        std::vector<float> planningConfidence;
        
        CognitiveState() {}
    } cognitive;
    
    // Neuromodulation state
    struct NeuromodulationState {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionErrorLevel;
        std::vector<float> neuromodulatorHistory;
        
        NeuromodulationState() : dopamineLevel(0.0f), curiosityLevel(0.0f),
                                noveltyLevel(0.0f), predictionErrorLevel(0.0f) {}
    } neuromodulation;
    
    // Development state
    struct DevelopmentState {
        DevelopmentalStage currentStage;
        double developmentalAge;
        float plasticityModifier;
        std::vector<float> stageParameters;
        
        DevelopmentState() : currentStage(DevelopmentalStage::Initial),
                           developmentalAge(0.0), plasticityModifier(1.0f) {}
    } development;
    
    // Statistics
    struct Statistics {
        size_t totalSpikes;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        std::vector<float> regionFiringRates;
        
        Statistics() : totalSpikes(0), firingNeurons(0), averageFiringRate(0.0f),
                     excitationInhibitionRatio(0.0f) {}
    } statistics;
    
    BrainState() : step(0), timestamp(0.0) {}
    
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

// Checkpoint structure for persistence
struct Checkpoint {
    uint64_t step;
    double timestamp;
    BrainState brainState;
    std::string checkpointPath;
    bool isValid;
    
    Checkpoint() : step(0), timestamp(0.0), isValid(false) {}
    
    std::string getCheckpointFilename() const {
        return "checkpoint_" + std::to_string(step) + ".bin";
    }
};

// Attention focus structure
struct AttentionFocus {
    enum class TargetType {
        None,
        Visual,
        Tactile,
        Internal,
        Proprioceptive,
        Conceptual,
        Motor
    };
    
    TargetType targetType;
    float priority;
    float duration;
    std::vector<FeatureVector> targetFeatures;
    std::vector<size_t> attendedTraceIndices;
    
    AttentionFocus() : targetType(TargetType::None), priority(0.0f), duration(0.0f) {}
};

// Goal structure for planning
struct Goal {
    std::string goalId;
    std::string description;
    float importance;
    float urgency;
    std::vector<float> targetParameters;
    std::vector<std::string> preconditions;
    std::vector<std::string> effects;
    bool achieved;
    float confidence;
    
    Goal() : importance(0.5f), urgency(0.5f), achieved(false), confidence(0.0f) {}
};

// Experience analysis result
struct ExperienceAnalysis {
    float averageReward;
    float rewardVariance;
    float learningRate;
    float retentionScore;
    std::vector<float> performanceTrajectory;
    std::map<std::string, float> patternCoefficients;
    std::vector<std::string> learnedBehaviors;
    
    ExperienceAnalysis() : averageReward(0.0f), rewardVariance(0.0f), learningRate(0.0f),
                           retentionScore(0.0f) {}
};

} // namespace nlm