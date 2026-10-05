#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Controls arousal levels and attentional focus

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float stimulus);
    void signalAttention(float attentionSignal);
    
    // Integration with prediction systems
    void updateArousal(float baseline, float novelty, float predictionError);
    void updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance);
    
    // Getters for derived effects
    float getArousal() const;
    float getAttention() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm