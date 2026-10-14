#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Triplet STDP: implements triplet spike interaction rules
// Extends STDP with first-order and second-order spike interactions
// Captures more complex spike timing correlations in neural activity
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
    
    // Parameters
    void setPreTraceTimeConstant(float tau1);
    float getPreTraceTimeConstant() const;
    void setPostTraceTimeConstant(float tau2);
    float getPostTraceTimeConstant() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setThirdOrderLearningRate(float rate);
    float getThirdOrderLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm