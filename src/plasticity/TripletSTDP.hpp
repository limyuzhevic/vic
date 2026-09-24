#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Triplet STDP implementation
// Implements spike-timing dependent plasticity with triplet interactions
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
    
    // Triplet STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setTraceDecay(float tauPre, float tauPost);
    void getTraceDecay(float& tauPre, float& tauPost) const;
    
private:
    void updateTraces(const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt);
    float computeWeightChange() const;
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm