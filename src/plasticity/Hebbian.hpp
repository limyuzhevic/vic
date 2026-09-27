#pragma once

#include "PlasticityRule.hpp"
#include <unordered_map>

namespace nlm {

// Helper struct for synapse handle
struct SynapseHandle {
    uint64_t source;
    uint64_t destination;
    
    SynapseHandle() : source(0), destination(0) {}
    SynapseHandle(uint64_t src, uint64_t dst) : source(src), destination(dst) {}
    
    bool operator==(const SynapseHandle& other) const {
        return source == other.source && destination == other.destination;
    }
    
    bool operator!=(const SynapseHandle& other) const {
        return !(*this == other);
    }
};

// Hebbian learning implementation
// Advanced implementation with Classical Hebbian, BCM theory, and memory modulation

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on spike correlation
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with biological constraints
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Classical Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // BCM (Bienenstock-Cooper-Munro) theory parameters
    void setBCMParameters(float threshold, float learningRate, float targetWeight);
    
    // Neuromodulation parameters
    void setNeuromodulation(float predErrorMod, float dopamineMod, 
                            float curiosityMod, float noveltyMod);
    
    // Temporal kernel parameters
    void setTemporalKernel(float width);
    
    // Eligibility trace parameters
    void setTraceParams(float decayRate, float timeConstant);
    
    // Memory influence parameters
    void setMemoryInfluence(float influence);
    
    // Attention parameters
    void setAttentionMod(float mod);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Classical Hebbian learning ("fire together, wire together")
class ClassicalHebbian : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "Classical Hebbian"; }
};

// BCM Hebbian learning (Bienenstock-Cooper-Munro theory)
class BCMHebbian : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM Hebbian"; }
};

// Memory-modulated Hebbian learning
class MemoryHebbian : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "Memory-Modulated Hebbian"; }
};

} // namespace nlm
