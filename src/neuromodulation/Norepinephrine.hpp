#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Enhanced with locus coeruleus dynamics, attention modulation, and stress response

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Arousal level
    float getArousalLevel() const;
    void setArousalLevel(float level);
    
    // Vigilance modulation
    float getVigilanceFactor() const;
    void setVigilanceFactor(float factor);
    
    // Stress response
    void signalStress(float stressLevel);
    float getStressResponse() const;
    
    // Locus coeruleus dynamics
    void setLocusCoeruleusGain(float gain);
    float getLocusCoeruleusSensitivity() const;
    
    // Fight-or-flight response
    float getFightOrFlightLevel() const;
    void updateFightOrFlight(float threatLevel, TimestepDuration dt);
    
    // Sleep-wake cycle modulation
    void setWakeDrive(float drive);
    float getWakeDrive() const;
    void setSleepDrive(float drive);
    float getSleepDrive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
