#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Implements cholinergic filtering and attentional gain modulation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get ACh level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply attentional modulation to competition
    void modulateAttention(const std::vector<NeuronId>& competitors,
                          std::vector<float>& attentionWeights);
    
    // Apply working memory consolidation
    void consolidateMemory(NeuronId neuron, float activation, float strength = 1.0f);
    
    // Apply cholinergic filtering to sensory input
    void filterSensoryInput(std::vector<float>& inputPattern,
                           const std::vector<float>& baselinePattern);
    
    // Modulate learning rate for memory formation
    float getMemoryConsolidationRate() const;
    
    // Update attentional selection parameters
    void updateAttentionParameters(float gain, float focusWidth, float filteringStrength);
    
    // Reset
    void reset();
    
    // Get cholinergic enhancement factor for neurons
    float getCholinergicGain(NeuronId neuron) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm