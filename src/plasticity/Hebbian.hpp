#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Get network activity ratio for learning modulation
    float getNetworkActivityRatio() const;
    
    // Set network activity ratio for testing/debugging
    void setNetworkActivityRatio(float ratio);
    
    // Phase 3: reference implementation for evaluating learning efficiency
    struct LearningMetrics {
        float totalReward;
        float averageFiringRate;
        float weightSelectivity;
        float patternStability;
        float learningEfficiency;
        
        LearningMetrics()
            : totalReward(0.0f), averageFiringRate(0.0f), weightSelectivity(0.0f),
              patternStability(0.0f), learningEfficiency(0.0f) {}
    };
    
    // Phase 3: analyze learning patterns and metrics
    LearningMetrics analyzeLearningPatterns(const std::vector<float>& rewardHistory) const;
    
    // Phase 3: generate synthetic training data for debugging
    static std::vector<float> generateSyntheticTrainingData(int numSamples = 100);
    
    // Phase 3: benchmark performance against baseline implementations
    static float benchmarkLearningEfficiency(const std::vector<float>& rewards);
    
    // Phase 4: advanced Hebbian variants
    void enableTripletSTDP(bool enable) { tripletStdpEnabled_ = enable; }
    bool isTripletSTDPebled() const { return tripletStdpEnabled_; }
    
    void enableBCM(bool enable) { bcmEnabled_ = enable; }
    bool isBCMenabled() const { return bcmEnabled_; }
    
    // Phase 4: eligibility traces for reward-modulated learning
    void setEligibilityTraceDecay(float decay) { eligibilityTraceDecay_ = decay; }
    float getEligibilityTraceDecay() const { return eligibilityTraceDecay_; }
    
    // Phase 4: batch learning support
    void startLearningBatch() { batchLearning_ = true; }
    void endLearningBatch() { batchLearning_ = false; }
    bool isInLearningBatch() const { return batchLearning_; }
    
    // Phase 4: cross-time consolidation
    void enableConsolidation(bool enable) { consolidationEnabled_ = enable; }
    bool isConsolidationEnabled() const { return consolidationEnabled_; }
    
    // Phase 5: metaplasticity (learning to learn)
    void enableMetaplasticity(bool enable) { metaplasticityEnabled_ = enable; }
    bool isMetaplasticityEnabled() const { return metaplasticityEnabled_; }
    
    void setMetaplasticThreshold(float threshold) { metaplasticThreshold_ = threshold; }
    float getMetaplasticThreshold() const { return metaplasticThreshold_; }
    
    // Phase 5: synaptic masking (winner-take-all at synapse level)
    void enableSynapticMasking(bool enable) { maskingEnabled_ = enable; }
    bool isSynapticMaskingEnabled() const { return maskingEnabled_; }
    
    // Phase 5: soft weight normalization
    void enableSoftNormalization(bool enable) { softNormEnabled_ = enable; }
    bool isSoftNormalizationEnabled() const { return softNormEnabled_; }
    
    void setSoftNormStrength(float strength) { softNormStrength_ = strength; }
    float getSoftNormStrength() const { return softNormStrength_; }
    
    // Phase 6: structural plasticity integration
    void enableStructuralHebbian(bool enable) { structuralHebbianEnabled_ = enable; }
    bool isStructuralHebbianEnabled() const { return structuralHebbianEnabled_; }
    
    // Phase 6: homeostatic plasticity
    void enableHomeostaticPlasticity(bool enable) { homeostaticEnabled_ = enable; }
    bool isHomeostaticPlasticityEnabled() const { return homeostaticEnabled_; }
    
    void setHomeostaticSetpoint(float setpoint) { homeostaticSetpoint_ = setpoint; }
    float getHomeostaticSetpoint() const { return homeostaticSetpoint_; }
    
    void setHomeostaticLearningRate(float rate) { homeostaticRate_ = rate; }
    float getHomeostaticLearningRate() const { return homeostaticRate_; }
    
    // Phase 6: activity-dependent structural plasticity
    void enableActivityDependentStructural(bool enable) { activityDepStructEnabled_ = enable; }
    bool isActivityDependentStructuralEnabled() const { return activityDepStructEnabled_; }
    
    // Phase 6: competition-driven wiring
    void enableSynapseCompetition(bool enable) { competitionEnabled_ = enable; }
    bool isSynapseCompetitionEnabled() const { return competitionEnabled_; }
    
    void setCompetitionThreshold(float threshold) { competitionThreshold_ = threshold; }
    float getCompetitionThreshold() const { return competitionThreshold_; }
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
