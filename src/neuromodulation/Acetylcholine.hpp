#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effects
    float getMemoryConsolidationFactor() const;
    void signalNovelExperience(const std::vector<float>& input);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
