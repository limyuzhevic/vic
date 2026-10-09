#pragma once

#include "PIMPL.hpp"

namespace nlm {

class Neuromodulator {
public:
    PIMPL_FORWARD(Neuromodulator)
    
    Neuromodulator();
    ~Neuromodulator();
    
    virtual const char* getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
    
    // Exception safety
    void reset();
    bool isValid() const;
    
    // Logging
    void logStatus() const;
    
protected:
    // Allow derived classes to access pImpl
    template<typename T>
    T* getImpl() { return static_cast<T*>(pImpl); }
    
    template<typename T>
    const T* getImpl() const { return static_cast<const T*>(pImpl); }
};

class Dopamine : public Neuromodulator {
public:
    PIMPL_IMPLEMENTATION_DEFAULT(Dopamine)
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Advanced dopamine dynamics
    void applyInhibition(float inhibition);
    void applyFacilitation(float facilitation);
    float computeModulationFactor(float baseline, float input) const;
};

class Acetylcholine : public Neuromodulator {
public:
    PIMPL_IMPLEMENTATION_DEFAULT(Acetylcholine)
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    void enhanceMemoryConsolidation(float intensity);
};

class Norepinephrine : public Neuromodulator {
public:
    PIMPL_IMPLEMENTATION_DEFAULT(Norepinephrine)
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    void modulateArousal(float arousalLevel);
};

class Serotonin : public Neuromodulator {
public:
    PIMPL_IMPLEMENTATION_DEFAULT(Serotonin)
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    void regulateMood(float moodLevel);
};

} // namespace nlm
