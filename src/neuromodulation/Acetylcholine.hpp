#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Controls attention allocation and memory encoding processes

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory encoding enhancement
    void signalSensoryInput(float inputStrength, float novelty);
    void signalWorkingMemoryAccess(class Brain* brain, NeuronId neuron, float activation);
    void updateEncoding(float inputPatternStrength);
    
    // Integration with attention
    void updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance);
    
    // Getters for derived effects
    float getMemoryEnhancement() const;
    float getEncodingBoost() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm