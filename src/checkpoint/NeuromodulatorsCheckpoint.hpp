// Neuromodulators checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "../core/Types/Types.hpp"

namespace nlm {

// NeuromodulatorBaseCheckpoint for common neuromodulator state
struct NeuromodulatorBaseCheckpoint {
    // Core neuromodulator properties
    std::string name;
    
    // Concentration/state
    float level;
    float baselineLevel;
    float maxLevel;
    float minLevel;
    
    // Plasticity effects
    float plasticityFactor;
    float learningRate;
    float attentionModulation;
    
    // Temporal dynamics
    float accumulation;
    float decayRate;
    float halfLife;
    
    // Integration state
    float predictionErrorInput;
    float rewardPredictionError;
    float noveltySignal;
    
    // System interactions
    std::unordered_map<std::string, float> systemModulations;
    std::vector<float> integratedState;
    
    NeuromodulatorBaseCheckpoint()
        : level(0), baselineLevel(0), maxLevel(1), minLevel(0)
        , plasticityFactor(1.0f), learningRate(0.01f), attentionModulation(1.0f)
        , accumulation(0), decayRate(0.1f), halfLife(10)
        , predictionErrorInput(0), rewardPredictionError(0), noveltySignal(0) {}
};

// Dopamine checkpoint state
struct DopamineCheckpoint : public NeuromodulatorBaseCheckpoint {
    // Reward-specific properties
    float rewardPrediction;
    float rewardHistory;
    float reinforcementHistory;
    
    // Behavioral effects
    float actionSelectionBias;
    float explorationBonus;
    float motivationLevel;
    
    // Temporal aspects
    float rewardDelay;
    float rewardPersistence;
    
    // Integration with other systems
    float workingMemoryModulation;
    float attentionModulation;
    float decisionModulation;
    
    DopamineCheckpoint() : NeuromodulatorBaseCheckpoint() {
        name = "Dopamine";
        maxLevel = 2.0f;
        minLevel = 0.0f;
        learningRate = 0.05f;
    }
};

// Curiosity checkpoint state
struct CuriosityCheckpoint : public NeuromodulatorBaseCheckpoint {
    // Exploration properties
    float explorationLevel;
    float uncertaintyEstimate;
    float informationGain;
    
    // Novelty detection
    float noveltySensitivity;
    float predictionErrorAmplification;
    float behavioralFlexibility;
    
    // Learning aspects
    float intrinsicMotivation;
    float extrinsicMotivation;
    float goalRelevance;
    
    // Integration with other systems
    float conceptFormationModulation;
    float predictionSystemModulation;
    float memoryConsolidationModulation;
    
    CuriosityCheckpoint() : NeuromodulatorBaseCheckpoint() {
        name = "Curiosity";
        maxLevel = 1.5f;
        minLevel = 0.0f;
        learningRate = 0.03f;
    }
};

// Novelty checkpoint state
struct NoveltyCheckpoint : public NeuromodulatorBaseCheckpoint {
    // Novelty detection properties
    float noveltyDetectionThreshold;
    float noveltySensitivity;
    float predictionSurprise;
    
    // Cognitive effects
    float attentionCapture;
    float memoryEncoding;
    float learningAmplification;
    
    // Behavioral aspects
    float explorationDrive;
    float noveltySeeking;
    float adaptationSpeed;
    
    // Integration with other systems
    float workingMemoryModulation;
    float episodicMemoryModulation;
    float conceptFormationModulation;
    
    NoveltyCheckpoint() : NeuromodulatorBaseCheckpoint() {
        name = "Novelty";
        maxLevel = 1.2f;
        minLevel = 0.0f;
        learningRate = 0.02f;
    }
};

// PredictionError checkpoint state
struct PredictionErrorCheckpoint : public NeuromodulatorBaseCheckpoint {
    // Prediction error properties
    float predictionErrorMagnitude;
    float predictionErrorHistory;
    float predictionErrorSign;
    
    // Learning effects
    float learningSignalStrength;
    float creditAssignment;
    float temporalDifference;
    
    // Integration aspects
    float reinforcementLearningModulation;
    float attentionModulation;
    float memoryUpdateModulation;
    
    // State tracking
    SimulationStep lastPredictionError;
    float predictionErrorBias;
    float predictionErrorPersistence;
    
    PredictionErrorCheckpoint() : NeuromodulatorBaseCheckpoint() {
        name = "PredictionError";
        maxLevel = 1.0f;
        minLevel = 0.0f;
        learningRate = 0.1f;
    }
};

// Neuromodulators checkpoint state
struct NeuromodulatorsCheckpoint {
    // Individual neuromodulator states
    DopamineCheckpoint dopamine;
    CuriosityCheckpoint curiosity;
    NoveltyCheckpoint novelty;
    PredictionErrorCheckpoint predictionError;
    
    // Neuromodulator interactions
    std::vector<float> neuromodulatorInteractions;
    
    // System-wide neuromodulation state
    float globalModulation;
    float arousalLevel;
    float stressLevel;
    
    // Temporal dynamics
    SimulationStep lastNeuromodulationUpdate;
    float neuromodulationIntegrator;
    
    // Integration with other systems
    std::vector<float> integratedAttentionState;
    std::vector<float> integratedPredictionState;
    std::vector<float> integratedLearningState;
    
    // Performance metrics
    float overallModulationBalance;
    float systemStability;
    float adaptiveCapacity;
    
    // Environmental adaptation
    float environmentalAdaptation;
    float behavioralFlexibility;
    float learningRate;
    
    NeuromodulatorsCheckpoint()
        : globalModulation(1.0f), arousalLevel(0.5f), stressLevel(0)
        , lastNeuromodulationUpdate(0), neuromodulationIntegrator(0)
        , overallModulationBalance(1.0f), systemStability(1.0f), adaptiveCapacity(1.0f)
        , environmentalAdaptation(1.0f), behavioralFlexibility(1.0f), learningRate(0.05f) {}
};

// Neuromodulators save/load interface
class NeuromodulatorsSaverLoader {
public:
    virtual ~NeuromodulatorsSaverLoader() = default;
    
    // Save neuromodulators state
    virtual bool save(const NeuromodulatorsCheckpoint& checkpoint) const = 0;
    
    // Load neuromodulators state
    virtual bool load(NeuromodulatorsCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual NeuromodulatorsCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get neuromodulator level for validation
    virtual float getNeuromodulatorLevel(const NeuromodulatorBaseCheckpoint& neuromodulator) const = 0;
};

} // namespace nlm
