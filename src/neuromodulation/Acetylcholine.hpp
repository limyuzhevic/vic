#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// TODO PHASE 2: Implement real acetylcholine dynamics

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override = default;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// TODO PHASE 2: Implement real norepinephrine dynamics

class Norepinephrine : public Neuromodulator {
public:
    ~Norepinephrine() override = default;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// TODO PHASE 2: Implement real serotonin dynamics

class Serotonin : public Neuromodulator {
public:
    ~Serotonin() override = default;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm