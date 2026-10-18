#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Connect to brain for neuromodulation operations
    void connectBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
    // Apply reward-based neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Compute dopamine signal from prediction error
    float computeDopamineSignal(float predictionError) const;
    
    // Update eligibility traces for all synapses
    void updateEligibilityTraces(float decayRate = 0.1f);
    
    // Apply reward-modulated weight changes based on eligibility traces
    void applyRewardModulatedWeights(float dopamineLevel, float plasticityFactor);
    
    // Modulate plasticity factors based on dopamine
    void modulatePlasticityFactors(float dopamineLevel, float& plasticityModifier);
    
    // Set neuromodulation parameters
    void setModulationParameters(float eligibilityDecay, float weightChangeRate, 
                                 float plasticitySensitivity, float dopamineClampMin,
                                 float dopamineClampMax) {
        eligibilityDecay_ = eligibilityDecay;
        weightChangeRate_ = weightChangeRate;
        plasticitySensitivity_ = plasticitySensitivity;
        dopamineClampMin_ = dopamineClampMin;
        dopamineClampMax_ = dopamineClampMax;
    }
    
    // Get neuromodulation state
    float getDopamineLevel() const { return dopamineLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Reset neuromodulation state
    void reset();
    
private:
    // Connected brain
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityModifier_;
    
    // Modulation parameters
    float eligibilityDecay_;
    float weightChangeRate_;
    float plasticitySensitivity_;
    float dopamineClampMin_;
    float dopamineClampMax_;
    
    // Last computed values for debugging
    float lastWeightChangeDelta_;
};

} // namespace nlm
