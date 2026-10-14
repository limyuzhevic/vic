#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implements locus coeruleus-like burst firing and arousal modulation

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get NE level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply arousal modulation to neural excitability
    void modulateArousal(std::vector<NeuronId>& neurons, std::vector<float>& excitability);
    
    // Simulate locus coeruleus burst firing
    void simulateBurstFiring(float eventIntensity, float burstDuration);
    
    // Apply vigilance modulation to working memory encoding
    void modulateVigilance(float encodingConfidence, float* encodingThreshold);
    
    // Get arousal factor for neural gain modulation
    float getArousalFactor() const;
    
    // Get vigilance enhancement for memory encoding
    float getVigilanceFactor() const;
    
    // Update locus coeruleus parameters
    void updateLocusCoeruleusParameters(float baseline, float gain, float burstProbability);
    
    // Reset
    void reset();
    
    // Get norepinephrine enhancement factor for neurons
    float getNorepinephrineGain(NeuronId neuron) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm