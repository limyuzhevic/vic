#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../motor/MotorSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <string>
#include <cmath>

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
    
    // Mood and impulse regulation
    void modulateMood(float moodLevel);
    void regulateImpulsivity(float impulsivityLevel);
    void modulateSocialBehavior(const Brain* brain);
    
    // Learning modulation
    void facilitatePatternLearning(const Brain* brain);
    void modulatePunishmentLearning(const Brain* brain);
    void enhanceMemoryPrecision(const Brain* brain);
    
    // Decision making and behavioral control
    void facilitatePlanEvaluation(const Brain* brain);
    void modulateRiskAssessment(const Brain* brain);
    
    // State management
    void reset() override;
    float getMoodLevel() const;
    float getImpulseControl() const;
    float getSocialDominance() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm