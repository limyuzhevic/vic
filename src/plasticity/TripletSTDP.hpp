#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Triplet STDP implementation with synaptic history
// Extends standard STDP by considering recent spike triplets
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
    
    // STDP parameters
    void setLTPWeight1(float weight);    // Pre-post LTP weight
    float getLTPWeight1() const;
    void setLTDWeight1(float weight);    // Post-pre LTD weight
    float getLTDWeight1() const;
    void setLTPWeight2(float weight);    // Post-post LTP weight
    float getLTPWeight2() const;
    void setLTDWeight2(float weight);    // Pre-pre LTD weight
    float getLTDWeight2() const;
    
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Configure all parameters
    void configure(float ltpWeight1, float ltdWeight1, float ltpWeight2, float ltdWeight2);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm