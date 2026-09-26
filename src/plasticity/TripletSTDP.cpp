#include "STDP.hpp"

namespace nlm {

// Triplet STDP
// Implements triplet-based spike-timing dependent plasticity
// Captures more complex temporal interactions beyond pairwise spike timing
// Biological basis: Synaptic interactions involving three spikes (pre-post or post-post-pre)
// Mathematical formulation: Considers triplet interactions for both LTP and LTD

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP() : tripletLTPWeight(0.005f), tripletLTDWeight(0.006f),
                   tripletTimeConstant(50.0f), minWeight(-1.0f), maxWeight(1.0f) {}
    ~TripletSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        float totalDelta = 0.0f;
        float tau = tripletTimeConstant;
        
        // Process all spike combinations
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                Timestamp preTime = preSpikes[i];
                Timestamp postTime = postSpikes[j];
                float deltaT = static_cast<float>(postTime - preTime);
                
                // PAIR-BASED STDP (for comparison and baseline)
                if (deltaT > 0) {
                    totalDelta += pairLTPWeight * std::exp(-deltaT / tau);
                } else if (deltaT < 0) {
                    totalDelta += pairLTDWeight * std::exp(deltaT / tau);
                }
                
                // TRIPPLET INTERACTIONS
                // Process triplet combinations: look for patterns like pre-post-pre or post-post-pre
                
                // Check for pre-post-pre triplet (i.e., find another pre spike after post)
                for (size_t k = i + 1; k < preSpikes.size(); ++k) {
                    if (preSpikes[k] > postTime) {
                        // Pre-post-pre triplet: should have different effect than simple pair
                        float pre_post = static_cast<float>(postTime - preTime);  // Δt1
                        float post_pre = static_cast<float>(preSpikes[k] - postTime);  // Δt2
                        
                        if (pre_post > 0 && post_pre > 0) {
                            // Both intervals positive: triplet potentiation
                            totalDelta += tripletLTPWeight * std::exp(-pre_post / tau) * std::exp(-post_pre / tau);
                        }
                    }
                }
                
                // Check for post-post-pre triplet (i.e., find another post spike before pre)
                for (size_t k = j + 1; k < postSpikes.size(); ++k) {
                    if (postSpikes[k] < preTime) {
                        // Post-post-pre triplet: negative interaction
                        float post_pre = static_cast<float>(preTime - postSpikes[k]);  // Δt1
                        float pre_post = static_cast<float>(postTime - preSpikes[j]);  // Δt2
                        
                        if (post_pre > 0 && pre_post < 0) {
                            // Mixed sign intervals: triplet depression
                            totalDelta += tripletLTDWeight * std::exp(-post_pre / tau) * std::exp(-pre_post / tau);
                        }
                    }
                }
            }
        }
        
        // Apply weight change with bounds
        if (std::abs(totalDelta) > 1e-6f) {
            float efficacy = synapse->getEfficacy();
            totalDelta *= efficacy;
            
            synapse->addToWeight(totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet STDP specific parameters
    void setPairLTPWeight(float weight) { pairLTPWeight = std::clamp(weight, 0.0f, 1.0f); }
    float getPairLTPWeight() const { return pairLTPWeight; }
    void setPairLTDWeight(float weight) { pairLTDWeight = std::clamp(weight, 0.0f, 1.0f); }
    float getPairLTDWeight() const { return pairLTDWeight; }
    void setTripletLTPWeight(float weight) { tripletLTPWeight = std::clamp(weight, 0.0f, 1.0f); }
    float getTripletLTPWeight() const { return tripletLTPWeight; }
    void setTripletLTDWeight(float weight) { tripletLTDWeight = std::clamp(weight, 0.0f, 1.0f); }
    float getTripletLTDWeight() const { return tripletLTDWeight; }
    void setTripletTimeConstant(float tau) { tripletTimeConstant = std::clamp(tau, 1.0f, 200.0f); }
    float getTripletTimeConstant() const { return tripletTimeConstant; }
    void setMinWeight(float weight) { minWeight = weight; }
    float getMinWeight() const { return minWeight; }
    void setMaxWeight(float weight) { maxWeight = weight; }
    float getMaxWeight() const { return maxWeight; }
    
    // Update parameters from config
    void configure(float pairLTPWeight, float pairLTDWeight, float tripletLTPWeight, float tripletLTDWeight, float tau) {
        this->pairLTPWeight = std::clamp(pairLTPWeight, 0.0f, 1.0f);
        this->pairLTDWeight = std::clamp(pairLTDWeight, 0.0f, 1.0f);
        this->tripletLTPWeight = std::clamp(tripletLTPWeight, 0.0f, 1.0f);
        this->tripletLTDWeight = std::clamp(tripletLTDWeight, 0.0f, 1.0f);
        this->tripletTimeConstant = std::clamp(tau, 1.0f, 200.0f);
    }

private:
    // Pair-based STDP parameters (baseline for triplet rule)
    float pairLTPWeight = 0.01f;       // Pair-based LTP amplitude
    float pairLTDWeight = 0.012f;      // Pair-based LTD amplitude
    
    // Triplet-specific parameters
    float tripletLTPWeight = 0.005f;   // Triplet LTP amplitude (typically weaker)
    float tripletLTDWeight = 0.006f;   // Triplet LTD amplitude
    float tripletTimeConstant = 50.0f; // Longer time window for triplet interactions
    
    // Weight bounds
    float minWeight = -1.0f;           // Minimum synaptic weight
    float maxWeight = 1.0f;            // Maximum synaptic weight
};
