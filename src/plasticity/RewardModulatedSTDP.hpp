#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP): STDP gated by reward signals
// Combines spike timing with reward prediction error for learning
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // R-STDP parameters
    void setBaseLTPWeight(float weight);      // Base LTP weight
    float getBaseLTPWeight() const;
    
    void setBaseLTDWeight(float weight);      // Base LTD weight  
    float getBaseLTDWeight() const;
    
    void setTimeConstant(float tau);          // STDP time constant
    float getTimeConstant() const;
    
    void setLearningRate(float rate);         // Overall learning rate
    float getLearningRate() const;
    
    // Connect to neuromodulator for reward gating
    void connectToNeuromodulator(Neuromodulator* neuromodulator);
    
    // Set reward threshold for gating
    void setRewardThreshold(float threshold);
    
    // Check if reward is sufficient to enable plasticity
    bool isPlasticityEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm