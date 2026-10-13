#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Enhanced Spike-Timing-Dependent Plasticity
// Implements biological STDP with multiple time windows, triplet interactions, and neuromodulation

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing differences
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with bounds checking
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // ===== BASIC STDP PARAMETERS =====
    
    // Long-term potentiation weight (A+)
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    
    // Long-term depression weight (A-)
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    
    // STDP time constant (tau) for exponential decay
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // ===== ADVANCED STDP PARAMETERS =====
    
    // Learning rate modifier (global scaling factor)
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // NMDA receptor saturation level
    void setNMDACompetence(float competence);
    float getNMDACompetence() const;
    
    // Weight normalization bounds
    void setWeightBounds(float minWeight, float maxWeight);
    void getWeightBounds(float& minWeight, float& maxWeight) const;
    
    // ===== TRIPLET INTERACTIONS =====
    
    // Enable/disable triplet interactions (Ruvinen model)
    void setTripletEnabled(bool enabled);
    bool getTripletEnabled() const;
    
    // Triplet interaction time constants
    void setTripletTimeConstants(float tau_plus, float tau_minus);
    void getTripletTimeConstants(float& tau_plus, float& tau_minus) const;
    
    // Triplet amplitudes
    void setTripletAmplitudes(float u_plus, float u_minus, float w_plus, float w_minus);
    void getTripletAmplitudes(float& u_plus, float& u_minus, float& w_plus, float& w_minus) const;
    
    // ===== BIOLOGICAL MODELS =====
    
    // Select STDP model type
    enum class STDPModel {
        Pairwise,      // Simple pairwise STDP
        Vilnius,       // Vilnius model with synaptic tags
        Ruvinen,       // Ruvinen triplet model
        Tsodyks,       // Tsodyks-Markram model
        Combined       // Combined model with all features
    };
    
    void setSTDPModel(STDPModel model);
    STDPModel getSTDPModel() const;
    
    // ===== NEUROMODULATION =====
    
    // Enable dopamine modulation of STDP
    void setDopamineModulation(bool enabled, float learningRateFactor = 1.0f);
    bool getDopamineModulation() const;
    float getDopamineLearningRateFactor() const;
    
    // Enable reward prediction error modulation
    void setRewardModulation(bool enabled, float rewardAmplitude = 1.0f);
    bool getRewardModulation() const;
    float getRewardAmplitude() const;
    
    // Update parameters from configuration
    void configure(float ltpWeight, float ltdWeight, float tau,
                   STDPModel model = STDPModel::Ruvinen);
    
    // Get current STDP state for debugging
    std::string getDebugInfo() const;
    
    // ===== EDUCATIONAL FEATURES =====
    
    // Enable/disable educational logging
    void setEducationalLogging(bool enabled);
    bool getEducationalLogging() const;
    
    // Get example spike pairings for learning
    std::vector<std::pair<Timestamp, Timestamp>> getExampleSpikePairs() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
