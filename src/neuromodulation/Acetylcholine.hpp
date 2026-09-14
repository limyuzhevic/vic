#pragma once

#include "Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <boost/optional.hpp>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Biologically plausible model with attention modulation, working memory enhancement,
// and memory consolidation functions based on ACh release patterns in the brain

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void signalAttention(float attentionStrength);
    void enableMemoryConsolidation(bool enable);
    
    // Working memory enhancement
    void enhanceWorkingMemory(NeuralWorkingMemory* workingMemory);
    
    // State access
    float getCholinergicTone() const;
    float getMemoryBoost() const;
    float getAttentionGain() const;
    float getSensoryGain() const;
    float getNoiseReduction() const;
    
    // Enhanced neuromodulation effects
    void modulateSensoryProcessing(std::vector<float>& sensoryInput, float gainFactor);
    void enhanceMemoryEncoding(const std::vector<float>& neuralActivity, float learningRate);
    void suppressIrrelevantActivity(std::vector<float>& neuralActivity, float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm