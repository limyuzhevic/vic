#pragma once

#include "../core/Types/Types.hpp"
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
    
    // Enhanced functionality
    void signalMoodImprovement(float intensity);
    void signalSocialBehavior(float intensity);
    
private:
    struct Impl;
    Impl* pImpl;
};

}