#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Placeholder - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Apply neuromodulation to neuron excitability
    virtual void applyToNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Reset neuromodulator state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Neuromodulators with placeholder implementations
// TODO PHASE 2: Implement real neuromodulation effects for each neurotransmitter

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override {}  // ACh modulates attention
    void reset() override {};
    
    // Attention modulation for learning
    void enableAttention(bool enable) { attentionEnabled_ = enable; }
    
private:
    struct Impl;
    Impl* pImpl;
    float level_;
    bool attentionEnabled_;
};

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override {}  // NE affects arousal
    void reset() override {};
    
    // Arousal and vigilance modulation
    void increaseArousal(float amount) { level_ = std::min(1.0f, level_ + amount); }
    
private:
    struct Impl;
    Impl* pImpl;
    float level_;
};

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override {}  // 5-HT affects mood
    void reset() override {};
    
    // Mood and impulsivity modulation
    void enhanceMood(float amount) { level_ = std::min(1.0f, level_ + amount); }
    
private:
    struct Impl;
    Impl* pImpl;
    float level_;
};
