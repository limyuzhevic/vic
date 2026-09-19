#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Hebbian learning implementation
// Combines covariance-based learning, Oja's rule, BCM theory, and more
// for biologically accurate and stable synaptic plasticity

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Implements multiple Hebbian learning rules for realistic learning
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Core Hebbian parameters
    void setLearningRate(float rate);          // Base learning rate η
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);        // Maximum weight for normalization
    float getMaxWeight() const;
    void setMinWeight(float minWeight);        // Minimum weight
    float getMinWeight() const;
    
    // Covariance-based learning parameters
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    
    // Oja's rule parameters
    void setOjaAlpha(float alpha);             // Oja learning rate
    float getOjaAlpha() const;
    void setOjaBeta(float beta);               // Oja normalization factor
    float getOjaBeta() const;
    
    // BCM theory parameters
    void setBcmThreshold(float threshold);     // BCM sliding threshold θ
    float getBcmThreshold() const;
    void setBcmTau(float tau);                 // BCM time constant
    float getBcmTau() const;
    void setBcmTheta(float theta);             // BCM threshold θ̂
    float getBcmTheta() const;
    
    // Temporal learning parameters
    void setTemporalWindow(float window);     // Temporal window for spike pairing
    float getTemporalWindow() const;
    void setTemporalDecay(float decay);       // Temporal trace decay rate
    float getTemporalDecay() const;
    
    // Synaptic competition parameters
    void setCompetitionStrength(float strength);    // Competition strength α
    float getCompetitionStrength() const;
    void setGlobalNormalization(float norm);        // Global normalization factor
    float getGlobalNormalization() const;
    void setCompetitionRange(size_t range);         // Competition range N
    size_t getCompetitionRange() const;
    
    // Synaptic tagging and capture
    void enableTagging(bool enable);                 // Enable/disable synaptic tagging
    bool isTaggingEnabled() const;
    void setTagDecayRate(float rate);               // Tag decay rate
    float getTagDecayRate() const;
    void setTagCaptureThreshold(float threshold);   // Tag capture threshold
    float getTagCaptureThreshold() const;
    
    // Activity-dependent learning
    void setBaselineLearningRate(float rate);       // Baseline learning rate
    float getBaselineLearningRate() const;
    void setActivityModulation(float modulation);   // Activity modulation factor
    float getActivityModulation() const;
    void setMaxActivityLevel(float level);          // Maximum activity level
    float getMaxActivityLevel() const;
    
    // Configuration from global parameters
    void configure(float learningRate, float maxWeight, float minWeight,
                   float ojaAlpha, float ojaBeta, float bcmThreshold,
                   float covarianceThreshold, float temporalWindow,
                   float competitionStrength);
    
    // Statistics and debugging
    size_t getTotalUpdates() const;
    float getCumulativeWeightChange() const;
    size_t getSampleCount() const;
    float getRunningMeanPre() const;
    float getRunningMeanPost() const;
    float getRunningCovariance() const;
    
    // Get synaptic tag for a synapse
    float getSynapticTag(SynapseId synapseId) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
