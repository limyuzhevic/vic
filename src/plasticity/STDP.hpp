#pragma once

#include "PlasticityRule.hpp"
#include <string>

namespace nlm {

// Forward declarations for developmental stages
enum class DevelopmentalStage;

// Advanced STDP variants

// Base STDP class with configurable parameters
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Advanced STDP variants
    void enableTripletSTDP(bool enable) { tripletEnabled_ = enable; }
    bool isTripletSTDPEnabled() const { return tripletEnabled_; }
    
    void setTripletA1(float a1) { tripletA1_ = std::clamp(a1, 0.0f, 1.0f); }
    void setTripletA2(float a2) { tripletA2_ = std::clamp(a2, 0.0f, 1.0f); }
    void setTripletB(float b) { tripletB_ = std::clamp(b, 0.0f, 1.0f); }
    
    void enableAntiHebbian(bool enable) { antiHebbianEnabled_ = enable; }
    bool isAntiHebbianEnabled() const { return antiHebbianEnabled_; }
    
    void enableVersionBased(bool enable) { versionBasedEnabled_ = enable; }
    bool isVersionBasedEnabled() const { return versionBasedEnabled_; }
    
    // Get STDP statistics
    size_t getPairwiseUpdateCount() const { return pairwiseUpdates_; }
    size_t getTripletUpdateCount() const { return tripletUpdates_; }
    size_t getAntiHebbianUpdateCount() const { return antiHebbianUpdates_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Statistics
    mutable size_t pairwiseUpdates_;
    mutable size_t tripletUpdates_;
    mutable size_t antiHebbianUpdates_;
    
    // Advanced features
    bool tripletEnabled_;
    bool antiHebbianEnabled_;
    bool versionBasedEnabled_;
    
    // Triplet STDP parameters
    float tripletA1_;  // First-order potentiation
    float tripletA2_;  // Second-order potentiation  
    float tripletB_;   // Depression
};

// Reward-modulated STDP (R-STDP)
// Enhances STDP with dopamine prediction error signals
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
    
    // Neuromodulation parameters
    void setDopamineModulation(float modulation) { 
        dopamineModulation_ = std::clamp(modulation, 0.0f, 2.0f); 
    }
    void setCuriosityModulation(float modulation) { 
        curiosityModulation_ = std::clamp(modulation, 0.0f, 1.0f); 
    }
    void setNoveltyModulation(float modulation) { 
        noveltyModulation_ = std::clamp(modulation, 0.0f, 1.0f); 
    }
    
    // Get current neuromodulation level
    float getCurrentNeuromodulationLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    float dopamineModulation_;
    float curiosityModulation_;
    float noveltyModulation_;
    float predictionError_;
    float expectedReward_;
};

// Spike-timing dependent plasticity with triplet interactions
// Includes second-order spike interactions for more realistic learning
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
    void setA1(float a1) { a1_ = std::clamp(a1, 0.0f, 1.0f); }
    void setA2(float a2) { a2_ = std::clamp(a2, 0.0f, 1.0f); }
    void setB(float b) { b_ = std::clamp(b, 0.0f, 1.0f); }
    void setW1(float w1) { w1_ = std::clamp(w1, 0.0f, 1.0f); }
    void setW2(float w2) { w2_ = std::clamp(w2, 0.0f, 1.0f); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float a1_;  // Pre-before-post potentiation
    float a2_;  // Post-before-pre potentiation
    float b_;   // Depression
    float w1_;  // Trace for first-order spikes
    float w2_;  // Trace for second-order spikes
};

// Anti-Hebbian STDP for inhibitory learning and pattern separation
// Implements anti-correlation learning for orthogonalization
class AntiHebbianSTDP : public PlasticityRule {
public:
    AntiHebbianSTDP();
    ~AntiHebbianSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Anti-Hebbian parameters
    void setLearningRate(float lr) { learningRate_ = std::clamp(lr, 0.0f, 1.0f); }
    void setAntiCorrelationThreshold(float threshold) { antiThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float learningRate_;
    float antiThreshold_;
};

// Version-based STDP for developmental trajectories
// Implements age-dependent learning rules for developmental stages
class VersionBasedSTDP : public PlasticityRule {
public:
    VersionBasedSTDP();
    ~VersionBasedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Developmental parameters
    void setDevelopmentalStage(DevelopmentalStage stage);
    void setPlasticityTrajectory(const std::string& trajectory);
    
    // Get current developmental influence
    float getDevelopmentalModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    DevelopmentalStage currentStage_;
    std::string trajectoryName_;
    float plasticityModulation_;
};

} // namespace nlm
