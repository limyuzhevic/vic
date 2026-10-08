#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>
#include <memory>

namespace nlm {

// Neuromodulation-based plasticity modifier
struct NeuromodulationModifier {
    float baseFactor;           // Base plasticity factor
    float neuromodulatorMod;     // Neuromodulator influence
    float eligibilityTrace;      // Eligibility trace value
    float learningProgress;      // Learning progress factor
    float attentionMod;          // Attention modulation
    float motivationMod;         // Motivation modulation
    float confidence;            // Prediction confidence
    
    NeuromodulationModifier()
        : baseFactor(1.0f), neuromodulatorMod(1.0f), eligibilityTrace(0.0f),
          learningProgress(0.0f), attentionMod(1.0f), motivationMod(1.0f),
          confidence(1.0f) {}
};

// Abstract base class for plasticity rules with neuromodulation support
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity with neuromodulation
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt,
                        const NeuromodulationModifier& modifier) = 0;
    
    // Apply weight change with neuromodulation integration
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                                  const NeuromodulationModifier& modifier) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Get rule type
    virtual const char* getType() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Set neuromodulator for this rule
    void setNeuromodulator(const Neuromodulator& neuromodulator);
    
    // Get neuromodulator influence
    float getNeuromodulatorInfluence() const;
    
    // Calculate eligibility trace for spike-based learning
    virtual float calculateEligibilityTrace(const Timestamp& spikeTime, 
                                           const Timestamp& currentTime,
                                           float decayRate) const = 0;
    
    // Get plasticity factor with neuromodulation
    virtual float getPlasticityFactor(const NeuromodulationModifier& modifier) const = 0;
    
protected:
    PlasticityRule() : enabled_(true), neuromodulatorInfluence_(1.0f) {}
    
    // Apply common neuromodulation factors to weight change
    void applyCommonNeuromodulationFactors(SynapticWeight& delta,
                                          const NeuromodulationModifier& modifier) const;
    
    // Calculate reward-prediction error based modulation
    float calculateRewardPredictionErrorModulation(const NeuromodulationModifier& modifier) const;
    
private:
    bool enabled_;
    const Neuromodulator* neuromodulator_;
    mutable float neuromodulatorInfluence_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together" with neuromodulation
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                const NeuromodulationModifier& modifier) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                          const NeuromodulationModifier& modifier) override;
    const char* getName() const override;
    const char* getType() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Neuromodulation-specific parameters
    void setNoveltySensitivity(float sensitivity);
    float getNoveltySensitivity() const;
    void setCuriositySensitivity(float sensitivity);
    float getCuriositySensitivity() const;
    
    // Calculate eligibility trace
    float calculateEligibilityTrace(const Timestamp& spikeTime,
                                   const Timestamp& currentTime,
                                   float decayRate) const override;
    
    // Get plasticity factor with neuromodulation
    float getPlasticityFactor(const NeuromodulationModifier& modifier) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                const NeuromodulationModifier& modifier) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                          const NeuromodulationModifier& modifier) override;
    const char* getName() const override { return "AntiHebbian"; }
    const char* getType() const override { return "AntiHebbian"; }
    
    // Calculate eligibility trace
    float calculateEligibilityTrace(const Timestamp& spikeTime,
                                   const Timestamp& currentTime,
                                   float decayRate) const override;
    
    // Get plasticity factor with neuromodulation
    float getPlasticityFactor(const NeuromodulationModifier& modifier) const override;
};

// Bienenstock-Cooper-Munro (BCM) rule with neuromodulation
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                const NeuromodulationModifier& modifier) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                          const NeuromodulationModifier& modifier) override;
    const char* getName() const override { return "BCM"; }
    const char* getType() const override { return "BCM"; }
    
    // BCM specific parameters
    void setTheta(float theta);        // Threshold
    void setSigma(float sigma);        // Modulation strength
    void setLearningRate(float rate);  // Learning rate
    
    // Calculate eligibility trace
    float calculateEligibilityTrace(const Timestamp& spikeTime,
                                   const Timestamp& currentTime,
                                   float decayRate) const override;
    
    // Get plasticity factor with neuromodulation
    float getPlasticityFactor(const NeuromodulationModifier& modifier) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP) rule
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                const NeuromodulationModifier& modifier) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                          const NeuromodulationModifier& modifier) override;
    const char* getName() const override { return "R-STDP"; }
    const char* getType() const override { return "RewardModulatedSTDP"; }
    
    // Reward modulation parameters
    void setRewardWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setLearningProgressWeight(float weight);
    
    // Calculate eligibility trace
    float calculateEligibilityTrace(const Timestamp& spikeTime,
                                   const Timestamp& currentTime,
                                   float decayRate) const override;
    
    // Get plasticity factor with neuromodulation
    float getPlasticityFactor(const NeuromodulationModifier& modifier) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP with neuromodulation
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt,
                const NeuromodulationModifier& modifier) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta,
                          const NeuromodulationModifier& modifier) override;
    const char* getName() const override { return "TripletSTDP"; }
    const char* getType() const override { return "TripletSTDP"; }
    
    // Calculate eligibility trace
    float calculateEligibilityTrace(const Timestamp& spikeTime,
                                   const Timestamp& currentTime,
                                   float decayRate) const override;
    
    // Get plasticity factor with neuromodulation
    float getPlasticityFactor(const NeuromodulationModifier& modifier) const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
