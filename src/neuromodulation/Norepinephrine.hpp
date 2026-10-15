#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include <string>
#include <cmath>

namespace nlm {

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance modulation
    void modulateArousal(float arousalLevel);
    void modulateVigilance(float vigilance);
    
    // Signal detection and attentional focus
    void enhanceSensoryProcessing(const Brain* brain, float stimulus salience);
    void filterDistractorInputs(const Brain* brain, const std::vector<float>& sensoryInputs);
    
    // Decision making modulation
    void facilitateDecisionMaking(const Brain* brain);
    void modulateResponseSelection(const Brain* brain);
    
    // State management
    void reset() override;
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getSignalToNoiseRatio() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm