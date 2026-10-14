#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

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
    
    // Mood modulation
    void improveMood(float strength);
    
    // Social behavior
    void facilitateSocialBehavior(float strength);
    
    // Impulse control
    void enhanceImpulseControl(float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm