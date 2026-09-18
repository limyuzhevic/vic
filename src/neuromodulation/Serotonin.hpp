#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Modulate learning and decision making
    void modulateLearning(float reward, float predictionError);
    void affectDecisionMaking(float uncertainty);
    
    // Get learning rate and mood modifiers
    float getLearningRatePositive() const;
    float getLearningRateNegative() const;
    float getMoodLevel() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm