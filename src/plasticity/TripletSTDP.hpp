#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

// Triplet STDP
// Advanced spike-timing dependent plasticity with triplet interactions
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
    void setLTPWeightA(float weight);
    float getLTPWeightA() const;
    
    void setLTDWeightA(float weight);
    float getLTDWeightA() const;
    
    void setLTPWeightB(float weight);
    float getLTPWeightB() const;
    
    void setLTDWeightB(float weight);
    float getLTDWeightB() const;
    
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm