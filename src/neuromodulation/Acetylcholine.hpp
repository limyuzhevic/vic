#pragma once

#include "Neuromodulator.hpp"
#include "../../brain/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation neuromodulator
// Biological basis:
// - Basal forebrain cholinergic system projects throughout cortex
// - ACh enhances cortical excitability and attentional focus
// - Promotes LTP and memory encoding
// - Modulates neural synchrony for perception
// - Reduces spontaneous activity (attenuation)
// 
// Functions:
// 1. Attentional selection: enhances responses to attended stimuli
// 2. Memory encoding: strengthens new memory traces
// 3. Memory consolidation: stabilizes memories during rest
// 4. Cognitive flexibility: reduces habitual responses
// 5. Sensory processing: enhances signal-to-noise ratio

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attentional signaling
    void signalAttention(float attentionStrength, float memoryPriority);
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryPattern, float strength);
    
    // Getters for specialized functions
    float getAttentionScore() const;
    float getMemoryPriority() const;
    const std::vector<float>& getMemoryTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm