#pragma once

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

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
    
    // Trace management
    void updateTraces(float dt);              // Update trace values
    float getPreTrace() const;                // Get current pre trace value
    float getPostTrace() const;               // Get current post trace value
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
