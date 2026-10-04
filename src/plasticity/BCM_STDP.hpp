#pragma once

#include "STDP.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

// Enhanced STDP with BCM theory
class BCMSTDP : public STDP {
public:
    BCMSTDP();
    ~BCMSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // BCM theory parameters
    void setBCMThreshold(float threshold);    // BCM threshold (theta_m)
    float getBCMThreshold() const;
    void setBCMLearningRate(float rate);      // BCM learning rate (eta)
    float getBCMLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP with presynaptic and postsynaptic traces
class TripletSTDP : public STDP {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Triplet parameters
    void setTraceDecayTime(float traceTau);   // Trace decay time constant
    float getTraceDecayTime() const;
    void setPreTraceWeight(float weight);     // Presynaptic trace weight
    float getPreTraceWeight() const;
    void setPostTraceWeight(float weight);    // Postsynaptic trace weight
    float getPostTraceWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic STDP to maintain stability
class HomeostaticSTDP : public STDP {
public:
    HomeostaticSTDP();
    ~HomeostaticSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Homeostatic parameters
    void setTargetActivity(float target);     // Target firing rate
    float getTargetActivity() const;
    void setHomeostaticRate(float rate);      // Homeostatic learning rate
    float getHomeostaticRate() const;
    void setHomeostaticTimeConstant(float tau); // Homeostatic time constant
    float getHomeostaticTimeConstant() const;
    
    // Metaplasticity - modulate plasticity itself
    void setMetaplasticityFactor(float factor); // Metaplasticity modulation
    float getMetaplasticityFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Eligibility traces for delayed reward learning
class EligibilityTraceSTDP : public STDP {
public:
    EligibilityTraceSTDP();
    ~EligibilityTraceSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Eligibility trace parameters
    void setTraceDecayRate(float rate);        // Trace decay rate
    float getTraceDecayRate() const;
    void setMaxTraceValue(float max);          // Maximum trace value
    float getMaxTraceValue() const;
    
    // Reward modulation integration
    void modulateWithReward(float reward, float time); // Apply reward modulation
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
