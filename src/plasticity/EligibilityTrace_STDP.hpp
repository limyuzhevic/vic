#pragma once

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

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
    void setRewardModulator(Neuromodulator* neuromodulator); // Set reward neuromodulator
    
    // Delayed reward parameters
    void setDelayWindow(float window);         // Time window for delayed reward
    float getDelayWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
