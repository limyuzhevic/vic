#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real pharmacological implementation

class Brain;

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get ACh level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update ACh dynamics
    void update(TimestepDuration dt) override;
    
    // Get ACh effects on plasticity
    float getPlasticityFactor() const override;
    
    // Signal events that should trigger ACh release
    void signalAttentionEvent();
    void signalMemoryEvent();
    
    // Get modulated parameters
    void setAttentionGain(float gain);
    float getAttentionGain() const;
    float getMemoryConsolidation() const;
    float getCorticalPlasticity() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
