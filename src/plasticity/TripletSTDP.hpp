#pragma once

#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>

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
    void setAPlus(float weight);
    float getAPlus() const;
    void setAMinus(float weight);
    float getAMinus() const;
    void setTimeConstants(float tau_plus, float tau_minus);
    float getTauPlus() const;
    float getTauMinus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm