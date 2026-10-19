// Attention Mechanism for NLM
// Implements attention-based processing for focusing on relevant information

#ifndef NLM_ATTENTION_MECHANISM_HPP
#define NLM_ATTENTION_MECHANISM_HPP

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Attention types
enum class AttentionType {
    SoftAttention,
    ScaledDotProduct,
    BahdanauAttention,
    LuongAttention,
    MultiHeadAttention,
    HierarchicalAttention,
    CrossModalAttention,
    GatedAttention
};

// Attention head for multi-head attention
struct AttentionHead {
    size_t headSize;
    size_t embeddingSize;
    float dropoutRate;
    
    // Query, Key, Value projections
    std::vector<float> queryWeights;
    std::vector<float> keyWeights;
    std::vector<float> valueWeights;
    
    // Output projection
    std::vector<float> outputWeights;
    
    // Attention scores
    std::vector<float> attentionScores;
    
    AttentionHead(size_t hs = 64, size_t es = 512, float dr = 0.1)
        : headSize(hs), embeddingSize(es), dropoutRate(dr) {}
};

// Multi-head attention mechanism
class AttentionMechanism {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    AttentionMechanism(size_t numHeads = 8, size_t embeddingSize = 512, 
                     float dropoutRate = 0.1, AttentionType type = AttentionType::MultiHeadAttention);
    ~AttentionMechanism();
    
    // Disable copying, enable moving
    AttentionMechanism(const AttentionMechanism&) = delete;
    AttentionMechanism& operator=(const AttentionMechanism&) = delete;
    AttentionMechanism(AttentionMechanism&&) noexcept;
    AttentionMechanism& operator=(AttentionMechanism&&) noexcept;
    
    // Core attention operations
    std::vector<float> applyAttention(const std::vector<float>& query,
                                     const std::vector<float>& key,
                                     const std::vector<float>& value,
                                     bool training = true);
    
    std::vector<float> applySelfAttention(const std::vector<float>& input,
                                         bool training = true);
    
    std::vector<float> applyCrossAttention(const std::vector<float>& query,
                                          const std::vector<float>& key,
                                          const std::vector<float>& value,
                                          bool training = true);
    
    // Multi-head attention
    std::vector<float> applyMultiHeadAttention(const std::vector<float>& input,
                                              size_t numHeads,
                                              bool training = true);
    
    // Hierarchical attention
    std::vector<float> applyHierarchicalAttention(const std::vector<float>& input,
                                                 size_t levels,
                                                 bool training = true);
    
    // Attention with plasticity modulation
    std::vector<float> applyModulatedAttention(const std::vector<float>& query,
                                              const std::vector<float>& key,
                                              const std::vector<float>& value,
                                              const std::vector<float>& plasticityWeights,
                                              bool training = true);
    
    // Attention for action selection
    std::vector<float> applyActionAttention(const std::vector<float>& state,
                                           const std::vector<float>& actionValue,
                                           bool training = true);
    
    // Attention for memory readout
    std::vector<float> applyMemoryAttention(const std::vector<float>& query,
                                           const std::vector<std::vector<float>>& memoryKeys,
                                           const std::vector<std::vector<float>>& memoryValues,
                                           bool training = true);
    
    // Update attention parameters based on experience
    void updateAttention(const std::vector<float>& experience,
                       float learningRate = 0.001);
    
    // Get attention weights for interpretability
    std::vector<std::vector<float>> getAttentionWeights() const;
    
    // Get attention scores
    std::vector<float> getAttentionScores() const;
    
    // Reset attention mechanism
    void reset();
    
    // Get mechanism statistics
    struct Statistics {
        size_t totalAttentionApplications;
        float averageAttentionScore;
        float attentionEntropy;
        std::vector<float> headAttentionWeights;
        
        Statistics() : totalAttentionApplications(0), averageAttentionScore(0.0f), 
                     attentionEntropy(0.0f) {}
    };
    
    Statistics getStatistics() const;
    
    // Set attention type
    void setAttentionType(AttentionType type);
    AttentionType getAttentionType() const;
    
    // Configure attention parameters
    void setNumHeads(size_t numHeads);
    void setEmbeddingSize(size_t embeddingSize);
    void setDropoutRate(float dropoutRate);
    
    size_t getNumHeads() const;
    size_t getEmbeddingSize() const;
    float getDropoutRate() const;
};

// Hierarchical Processing Network
// Implements multi-scale processing for hierarchical feature extraction
class HierarchicalProcessing {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    HierarchicalProcessing(size_t numLevels = 4, size_t baseFilterSize = 32,
                          float downsampleFactor = 2.0f);
    ~HierarchicalProcessing();
    
    // Disable copying, enable moving
    HierarchicalProcessing(const HierarchicalProcessing&) = delete;
    HierarchicalProcessing& operator=(const HierarchicalProcessing&) = delete;
    HierarchicalProcessing(HierarchicalProcessing&&) noexcept;
    HierarchicalProcessing& operator=(HierarchicalProcessing&&) noexcept;
    
    // Hierarchical feature extraction
    std::vector<std::vector<float>> extractHierarchicalFeatures(const std::vector<float>& input,
                                                               size_t levels);
    
    // Multi-scale processing
    std::vector<float> multiScaleProcessing(const std::vector<float>& input,
                                           const std::vector<size_t>& filterSizes);
    
    // Skip connections for gradient flow
    std::vector<float> hierarchicalSkipConnections(const std::vector<std::vector<float>>& features);
    
    // Upsampling for feature refinement
    std::vector<float> hierarchicalUpsampling(const std::vector<std::vector<float>>& features,
                                             size_t targetLevel);
    
    // Hierarchical attention
    std::vector<float> hierarchicalAttention(const std::vector<float>& input,
                                            size_t numAttentionLevels);
    
    // Scale-invariant processing
    std::vector<float> scaleInvariantProcessing(const std::vector<float>& input,
                                              float minScale = 0.25f, float maxScale = 2.0f,
                                              size_t numScales = 8);
    
    // Hierarchical plasticity
    void applyHierarchicalPlasticity(std::vector<std::vector<float>>& weights,
                                   const std::vector<float>& errorSignals,
                                   float learningRate = 0.001);
    
    // Hierarchical development
    void hierarchicalDevelopment(std::vector<std::vector<float>>& weights,
                                size_t developmentalStage);
    
    // Get hierarchical features
    std::vector<std::vector<float>> getHierarchicalFeatures() const;
    
    // Get level statistics
    struct LevelStatistics {
        size_t level;
        size_t filterSize;
        float activationMean;
        float activationStd;
        float sparsity;
        
        LevelStatistics(size_t l = 0, size_t fs = 0) 
            : level(l), filterSize(fs), activationMean(0.0f), activationStd(0.0f), sparsity(0.0f) {}
    };
    
    std::vector<LevelStatistics> getLevelStatistics() const;
    
    // Reset hierarchical processing
    void reset();
    
    // Configure processing parameters
    void setNumLevels(size_t numLevels);
    void setBaseFilterSize(size_t filterSize);
    void setDownsampleFactor(float factor);
    
    size_t getNumLevels() const;
    size_t getBaseFilterSize() const;
    float getDownsampleFactor() const;
};

// Attention-Enhanced Plasticity
// Plasticity rules modulated by attention mechanisms
class AttentionModulatedPlasticity {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    AttentionModulatedPlasticity();
    ~AttentionModulatedPlasticity();
    
    // Disable copying, enable moving
    AttentionModulatedPlasticity(const AttentionModulatedPlasticity&) = delete;
    AttentionModulatedPlasticity& operator=(const AttentionModulatedPlasticity&) = delete;
    AttentionModulatedPlasticity(AttentionModulatedPlasticity&&) noexcept;
    AttentionModulatedPlasticity& operator=(AttentionModulatedPlasticity&&) noexcept;
    
    // STDP with attention modulation
    void applyAttentionModulatedSTDP(Synapse* synapse,
                                   const std::vector<float>& preSpikeContext,
                                   const std::vector<float>& postSpikeContext,
                                   float learningRate = 0.01f);
    
    // Hebbian learning with attention
    void applyAttentionModulatedHebbian(Neuron* preNeuron,
                                      Neuron* postNeuron,
                                      const std::vector<float>& attentionWeights,
                                      float learningRate = 0.01f);
    
    // Reward-modulated learning with attention
    void applyRewardModulatedLearningWithAttention(Synapse* synapse,
                                                 const std::vector<float>& rewardPredictionError,
                                                 const std::vector<float>& attentionContext,
                                                 float learningRate = 0.01f);
    
    // Curiosity-driven exploration with attention
    void applyCuriosityDrivenPlasticity(Neuron* neuron,
                                      const std::vector<float>& curiositySignal,
                                      const std::vector<float>& attentionWeights,
                                      float learningRate = 0.01f);
    
    // Novelty detection with attention
    void applyNoveltyDrivenPlasticity(Synapse* synapse,
                                    const std::vector<float>& noveltySignal,
                                    const std::vector<float>& attentionContext,
                                    float learningRate = 0.01f);
    
    // Structural plasticity with attention
    void applyStructuralPlasticityWithAttention(NeuralRegion* region,
                                              const std::vector<float>& attentionDistribution,
                                              float synaptogenesisRate = 0.001f,
                                              float pruningRate = 0.0001f);
    
    // Development with attention
    void applyDevelopmentalPlasticity(NeuralRegion* region,
                                      size_t developmentalStage,
                                      const std::vector<float>& attentionWeights);
    
    // Get plasticity statistics
    struct PlasticityStatistics {
        size_t totalSTDPChanges;
        size_t totalHebbianChanges;
        size_t totalRewardModulatedChanges;
        size_t totalCuriosityDrivenChanges;
        size_t totalNoveltyDrivenChanges;
        size_t totalStructuralChanges;
        float averageAttentionModulation;
        
        PlasticityStatistics() : totalSTDPChanges(0), totalHebbianChanges(0),
                               totalRewardModulatedChanges(0), totalCuriosityDrivenChanges(0),
                               totalNoveltyDrivenChanges(0), totalStructuralChanges(0),
                               averageAttentionModulation(0.0f) {}
    };
    
    PlasticityStatistics getStatistics() const;
    
    // Reset plasticity statistics
    void reset();
    
    // Configure plasticity parameters
    void setSTDPAttentionWeight(float weight);
    void setHebbianAttentionWeight(float weight);
    void setRewardModulatedAttentionWeight(float weight);
    void setCuriosityAttentionWeight(float weight);
    void setNoveltyAttentionWeight(float weight);
    
    float getSTDPAttentionWeight() const;
    float getHebbianAttentionWeight() const;
    float getRewardModulatedAttentionWeight() const;
    float getCuriosityAttentionWeight() const;
    float getNoveltyAttentionWeight() const;
};

// Multi-Modal Attention Integration
// Integrates different sensory modalities using attention mechanisms
class MultiModalAttention {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MultiModalAttention();
    ~MultiModalAttention();
    
    // Disable copying, enable moving
    MultiModalAttention(const MultiModalAttention&) = delete;
    MultiModalAttention& operator=(const MultiModalAttention&) = delete;
    MultiModalAttention(MultiModalAttention&&) noexcept;
    MultiModalAttention& operator=(MultiModalAttention&&) noexcept;
    
    // Cross-modal attention
    std::vector<float> applyCrossModalAttention(const std::vector<float>& visualInput,
                                               const std::vector<float>& auditoryInput,
                                               const std::vector<float>& proprioceptiveInput,
                                               const std::vector<float>& tactileInput,
                                               bool training = true);
    
    // Modal-specific attention
    std::vector<float> applyModalSpecificAttention(const std::vector<float>& input,
                                                   NeuronType modality,
                                                   bool training = true);
    
    // Attention-based multimodal integration
    std::vector<float> integrateModalities(const std::vector<std::vector<float>>& modalityFeatures,
                                          const std::vector<NeuronType>& modalities,
                                          bool training = true);
    
    // Attention for feature binding
    std::vector<float> bindFeatures(const std::vector<float>& featureA,
                                   const std::vector<float>& featureB,
                                   const std::vector<float>& bindingAttention,
                                   bool training = true);
    
    // Multi-modal prediction with attention
    std::vector<float> predictMultimodal(const std::vector<float>& predictionQuery,
                                        const std::vector<std::vector<float>>& modalityPredictions,
                                        bool training = true);
    
    // Cross-modal interference detection
    std::vector<float> detectCrossModalInterference(const std::vector<float>& modalityA,
                                                   const std::vector<float>& modalityB,
                                                   bool training = true);
    
    // Get multimodal attention weights
    std::vector<std::vector<float>> getMultimodalAttentionWeights() const;
    
    // Get modality attention distributions
    std::unordered_map<NeuronType, std::vector<float>> getModalityAttentionDistributions() const;
    
    // Reset multimodal attention
    void reset();
    
    // Configure modality weights
    void setModalAttentionWeight(NeuronType modality, float weight);
    float getModalAttentionWeight(NeuronType modality) const;
    
    // Add modality
    void addModality(NeuronType modality, size_t featureSize);
    
    // Remove modality
    void removeModality(NeuronType modality);
};

// Advanced Action Selection with Attention
// Uses attention mechanisms for action selection and planning
class ActionSelectionWithAttention {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ActionSelectionWithAttention(size_t actionSpaceSize = 100, size_t embeddingSize = 256);
    ~ActionSelectionWithAttention();
    
    // Disable copying, enable moving
    ActionSelectionWithAttention(const ActionSelectionWithAttention&) = delete;
    ActionSelectionWithAttention& operator=(const ActionSelectionWithAttention&) = delete;
    ActionSelectionWithAttention(ActionSelectionWithAttention&&) noexcept;
    ActionSelectionWithAttention& operator=(ActionSelectionWithAttention&&) noexcept;
    
    // Attention-based action selection
    std::pair<size_t, float> selectActionWithAttention(const std::vector<float>& state,
                                                     const std::vector<float>& actionValues,
                                                     bool training = true);
    
    // Hierarchical action planning
    std::vector<size_t> planActionSequence(const std::vector<float>& goalState,
                                          size_t planningHorizon,
                                          bool training = true);
    
    // Attention for action monitoring
    void monitorActionWithAttention(size_t actionId,
                                   const std::vector<float>& actionContext,
                                   float rewardPrediction);
    
    // Action-outcome attention
    float computeActionOutcomeAttention(size_t actionId,
                                      const std::vector<float>& actionFeatures,
                                      const std::vector<float>& outcomeFeatures);
    
    // Get action attention weights
    std::vector<float> getActionAttentionWeights(size_t actionId) const;
    
    // Get action selection statistics
    struct ActionSelectionStatistics {
        size_t totalActionsSelected;
        float averageAttentionScore;
        std::vector<size_t> actionSelectionCounts;
        std::vector<float> actionAttentionWeights;
        
        ActionSelectionStatistics() : totalActionsSelected(0), averageAttentionScore(0.0f) {}
    };
    
    ActionSelectionStatistics getStatistics() const;
    
    // Reset action selection
    void reset();
    
    // Configure action space
    void setActionSpaceSize(size_t size);
    void setEmbeddingSize(size_t size);
    
    size_t getActionSpaceSize() const;
    size_t getEmbeddingSize() const;
};

// Factory functions for creating advanced components
inline AttentionMechanism makeAttentionMechanism(AttentionType type = AttentionType::MultiHeadAttention) {
    return AttentionMechanism(8, 512, 0.1, type);
}

inline HierarchicalProcessing makeHierarchicalProcessing(size_t levels = 4) {
    return HierarchicalProcessing(levels);
}

inline AttentionModulatedPlasticity makeAttentionModulatedPlasticity() {
    return AttentionModulatedPlasticity();
}

inline MultiModalAttention makeMultiModalAttention() {
    return MultiModalAttention();
}

inline ActionSelectionWithAttention makeActionSelectionWithAttention(size_t actionSpaceSize = 100) {
    return ActionSelectionWithAttention(actionSpaceSize);
}

} // namespace nlm

#endif // NLM_ATTENTION_MECHANISM_HPP