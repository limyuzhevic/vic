#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Triplet STDP includes contributions from previous spikes by maintaining
// traces of both pre- and post-synaptic activity
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
    void setLTPWeight(float weight);                    // Potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);                    // Depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);                    // Primary time constant (ms)
    float getTimeConstant() const;
    void setSecondaryTimeConstant(float tau2);          // Secondary time constant for triplet traces (ms)
    float getSecondaryTimeConstant() const;
    void setMaxTrace(float max);                         // Maximum trace value
    float getMaxTrace() const;

    // Trace management for triplet interactions
    void updatePreTrace(float trace, Timestamp spikeTime, Timestamp currentTime);
    void updatePostTrace(float trace, Timestamp spikeTime, Timestamp currentTime);
    float getPreTrace() const;
    float getPostTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm