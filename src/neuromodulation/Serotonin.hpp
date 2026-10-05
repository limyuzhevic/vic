#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void signalSatiety(float satietyLevel);
    void signalSocialStatus(float statusLevel);
    
    // Additional serotonin methods
    float get5HT1AEffect() const;
    float get5HT2AEffect() const;
    std::string getStateDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm