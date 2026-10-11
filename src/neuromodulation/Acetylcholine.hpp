#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

struct Acetylcholine::Impl;

class Acetylcholine : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Enhance attention and memory encoding
    void enhanceAttention(float enhancement);
    void enhanceMemoryEncoding(float strength);
    void setMemoryReverie(bool reverie);
};

} // namespace nlm