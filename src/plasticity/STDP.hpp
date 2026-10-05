// STDP.hpp - Enhanced with better biological plausibility

#pragma once

#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class STDP {
public:
    STDP();
    ~STDP();
    
    // Disable copying, enable moving
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;
    
    // Main STDP update function
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    
    // Apply weight change with bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    
    // Getters/setters
    const char* getName() const;
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setSTDPTimeWindows(float tau_plus, float tau_minus);
    float getSTDPTimeWindowPlus() const;
    float getSTDPTimeWindowMinus() const;
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Biological features
    void enableTripletLearning(bool enable);
    bool isTripletLearningEnabled() const;
    void setWeightNormalization(bool enable);
    bool isWeightNormalizationEnabled() const;
    
    // Statistics
    size_t getTotalSTDPUpdates() const;
    float getAverageSTDPChange() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
