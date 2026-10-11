#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet parameters
    void setA1(float weight);        // First trace amplitude
    float getA1() const;
    void setA2(float weight);        // Second trace amplitude
    float getA2() const;
    void setA3(float weight);        // Third trace amplitude
    float getA3() const;
    void setTau1(float tau);         // First trace time constant
    float getTau1() const;
    void setTau2(float tau);         // Second trace time constant
    float getTau2() const;
    void setTheta(float theta);      // Threshold for triplet interactions
    float getTheta() const;
    
    // Configuration helper
    void configure(float a1, float a2, float a3, float tau1, float tau2);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
