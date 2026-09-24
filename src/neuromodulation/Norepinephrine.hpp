#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance (LLC model)
// Based on computational models of NE signaling (Sara & Bouret 2017)
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE signals task salience and promotes cognitive flexibility
    void signalTaskSalience(float salience);
    void signalNoveltySurprise(float surprise);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm