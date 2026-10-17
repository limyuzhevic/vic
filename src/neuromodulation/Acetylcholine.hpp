#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Biologically inspired neuromodulator that enhances attention and memory formation
// Mechanisms:
// - Signal to prioritize salient sensory inputs
// - Rehearsal of memory traces to strengthen connections
// - Enhance working memory capacity and stability

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    void setLevel(float level);
    
    // Update acetylcholine dynamics
    void update(TimestepDuration dt);
    
    // Signal attention boost from novel stimuli
    void signalAttention(float attentionSignal);
    
    // Enhance memory through rehearsal
    void rehearseMemory(NeuronId neuron, float activation);
    
    // Get current attention boost level
    float getAttentionBoost() const;
    
    // Get plasticity factor for this neuromodulator
    float getPlasticityFactor() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm