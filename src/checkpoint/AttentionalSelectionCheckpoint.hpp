// AttentionalSelection checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../core/Types/Types.hpp"

namespace nlm {

// AttentionalBias for checkpointing
struct AttentionalBiasCheckpoint {
    // Target of attention
    NeuronId targetNeuron;
    
    // Bias strength and duration
    float biasStrength;
    float biasDuration;
    SimulationStep biasStartTime;
    
    // Bias type
    bool isTopDown;
    bool isBottomUp;
    
    // Source of bias
    std::string biasSource;
    float sourceConfidence;
    
    // Modulation factors
    float neuromodulationInfluence;
    float workingMemoryInfluence;
    float predictionInfluence;
    
    AttentionalBiasCheckpoint()
        : targetNeuron(0), biasStrength(0), biasDuration(0), biasStartTime(0)
        , isTopDown(false), isBottomUp(false), sourceConfidence(0)
        , neuromodulationInfluence(1.0f), workingMemoryInfluence(1.0f), predictionInfluence(1.0f) {}
};

// AttentionalSelection checkpoint state
struct AttentionalSelectionCheckpoint {
    // Current attentional focus
    std::vector<NeuronId> winners;
    
    // Biases currently active
    std::vector<AttentionalBiasCheckpoint> activeBiases;
    
    // Attended regions
    std::vector<RegionId> attendedRegions;
    
    // Salience values
    std::vector<float> neuronSalience;
    std::unordered_map<NeuronId, float> biasMap;
    std::unordered_map<NeuronId, float> salienceMap;
    
    // System parameters
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
    
    // State tracking
    bool isFocused;
    float attentionalScope;
    SimulationStep lastSelection;
    
    // Integration state
    std::vector<float> integratedWorkingMemoryState;
    std::vector<float> integratedPredictionState;
    std::vector<float> integratedConceptState;
    std::vector<float> integratedNeuromodulationState;
    
    // Neuromodulation levels
    float dopamineLevel;
    float noveltyLevel;
    float curiosityLevel;
    
    // Advanced parameters
    bool winnerTakeAllMode;
    float attentionalFatigue;
    float attentionalRecoveryRate;
    
    // Biological realism
    bool attentionBlinkEnabled;
    float attentionBlinkDuration;
    
    // Selection timing
    float selectionTimeWindow;
    float temporalIntegrationDecay;
    
    // Statistics
    size_t totalSelections;
    float averageSelectionConfidence;
    
    AttentionalSelectionCheckpoint()
        : inhibitionStrength(0.5f), excitationStrength(1.5f), competitionThreshold(0.3f)
        , isFocused(false), attentionalScope(1.0f), lastSelection(0)
        , dopamineLevel(0), noveltyLevel(0), curiosityLevel(0)
        , winnerTakeAllMode(false), attentionalFatigue(0), attentionalRecoveryRate(0.1f)
        , attentionBlinkEnabled(false), attentionBlinkDuration(0)
        , selectionTimeWindow(0.5f), temporalIntegrationDecay(0.9f)
        , totalSelections(0), averageSelectionConfidence(0) {}
};

// AttentionalSelection save/load interface
class AttentionalSelectionSaverLoader {
public:
    virtual ~AttentionalSelectionSaverLoader() = default;
    
    // Save attentional selection state
    virtual bool save(const AttentionalSelectionCheckpoint& checkpoint) const = 0;
    
    // Load attentional selection state
    virtual bool load(AttentionalSelectionCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual AttentionalSelectionCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get attentional confidence
    virtual float getAttentionalConfidence(const AttentionalSelectionCheckpoint& checkpoint) const = 0;
};

} // namespace nlm
