#pragma once

#include "../plasticity/PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// Neuromodulator types
enum class NeuromodulatorType {
    Dopamine,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
    Oxytocin,
    Vasopressin,
    GABA,
    Glutamate
};

// Neuromodulator state
struct NeuromodulatorState {
    float concentration;     // Current concentration (normalized 0-1)
    float baseline;          // Baseline concentration
    float releaseRate;       // Rate of release
    float decayRate;         // Rate of decay
    float modulationStrength; // Strength of modulation effect
    float history[10];        // Recent history for temporal integration
    size_t historyIndex;      // Current index in history array
    
    NeuromodulatorState() : concentration(0.0f), baseline(0.1f),
                           releaseRate(0.01f), decayRate(0.05f),
                           modulationStrength(1.0f), historyIndex(0) {
        for (size_t i = 0; i < 10; ++i) history[i] = baseline;
    }
};

// Reward-modulated STDP
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
    
    // Reward modulation parameters
    void setRewardPredictionError(float rpe);
    void setLearningRate(float rate);
    void setRewardDecayRate(float decay);
    void setBaselineReward(float baseline);
    
    // Neuromodulatory integration
    void setNeuromodulator(NeuromodulatorType type, float concentration);
    void setNeuromodulatorEffect(NeuromodulatorType type, float effect);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homosynaptic and heterosynaptic plasticity
class HomosynapticPlasticity : public PlasticityRule {
public:
    HomosynapticPlasticity();
    ~HomosynapticPlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homosynaptic parameters
    void setHomosynapticStrength(float strength);
    void setTargetWeight(float target);
    void setOutputDependency(float outputDep);
    
    // Heterosynaptic parameters
    void setHeterosynapticRange(float range);
    void setHeterosynapticStrength(float strength);
    void enableHeterosynaptic(bool enable);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Synaptic tagging and capture (STC)
class SynapticTaggingAndCapture : public PlasticityRule {
public:
    SynapticTaggingAndCapture();
    ~SynapticTaggingAndCapture() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STC parameters
    void setTagPersistence(float persistence);
    void setCaptureThreshold(float threshold);
    void setTagDecayRate(float decay);
    void setCaptureRange(float range);
    
    // Multi-neuromodulator integration
    void setMultiNeuromodulatorIntegration(bool enable);
    void setModulatorWeights(const std::vector<float>& weights);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Metaplasticity (plasticity of plasticity)
class Metaplasticity : public PlasticityRule {
public:
    Metaplasticity();
    ~Metaplasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplasticity parameters
    void setPlasticityThreshold(float threshold);
    void setPlasticityHistorySize(size_t size);
    void setMetaplasticLearningRate(float rate);
    void setHomeostasisTarget(float target);
    
    // Plasticity history management
    void recordPlasticityHistory(Synapse* synapse, float delta);
    float getPlasticityHistory(SynapseId synapseId) const;
    void resetPlasticityHistory(SynapseId synapseId);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Short-term plasticity (STP)
class ShortTermPlasticity : public PlasticityRule {
public:
    ShortTermPlasticity();
    ~ShortTermPlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STP parameters
    void setDepressionParameters(float u, float tauD);
    void setFacilitationParameters(float U, float tauF);
    void enableDepression(bool enable);
    void enableFacilitation(bool enable);
    
    // Use-dependent modulation
    void setUseDependentModulation(bool enable);
    void setModulationParameters(float maxModulation);
    
    // Dynamic efficacy
    void updateSynapticEfficacy(Synapse* synapse, Timestamp currentTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulatory integration
class NeuromodulatoryIntegration : public PlasticityRule {
public:
    NeuromodulatoryIntegration();
    ~NeuromodulatoryIntegration() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Neuromodulator management
    void addNeuromodulator(NeuromodulatorType type, float baseline,
                          float releaseRate, float decayRate);
    void updateNeuromodulator(NeuromodulatorType type, float value);
    float getNeuromodulatorLevel(NeuromodulatorType type) const;
    
    // Integration weights
    void setModulationWeights(const std::vector<float>& weights);
    void setInteractionTerms(const std::vector<std::vector<float>>& interactions);
    
    // Multi-neuromodulator dynamics
    void updateMultiNeuromodulatorState(Timestamp currentTime);
    float computeModulatedPlasticity(float basePlasticity) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm