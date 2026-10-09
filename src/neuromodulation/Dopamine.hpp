#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include <unordered_map>
#include <memory>

namespace nlm {

// Enhanced Dopamine neuromodulation system
// Implements biologically realistic reward prediction error signaling
// with temporal dynamics and synaptic plasticity modulation

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward prediction error (RPE) signaling
    // Implements temporal difference (TD) learning
    void signalReward(float reward, float predictedReward);
    void signalImmediateReward(float reward);
    void signalPredictionError(float error);
    
    // Reward computation and prediction
    // For reinforcement learning
    void updateValuePrediction(float currentValue);
    void learnFromOutcome(float actualOutcome);
    
    // Temporal dynamics control
    void setPhasicGain(float gain);
    void setTonicBaseline(float baseline);
    void setTimeConstants(float phasicTau, float tonicTau);
    void setAdaptationParameters(float adaptationRate, float decayRate);
    
    // Plasticity modulation
    void modulateSTDP(float rpe, const Synapse* synapse, float eligibilityTrace);
    void setSTDPModulationParameters(float ltpMod, float ltdMod, float threshold);
    
    // Integration with learning systems
    void reset();
    void initialize(float initialLevel = 0.05f);
    
    // State and history
    float getPhasicComponent() const;
    float getTonicComponent() const;
    float getPredictionError() const;
    float getValueFunction() const;
    
    // Configuration and parameter access
    const std::unordered_map<std::string, float>& getParameters() const;
    void setParameter(const std::string& name, float value);
    
    // Learning statistics
    struct LearningStats {
        float totalRewards;
        float totalPredictionErrors;
        float learningRate;
        float predictionAccuracy;
        float explorationBonus;
        float exploitationValue;
    };
    
    const LearningStats& getLearningStats() const;
    
    // Advanced methods
    void applyNeuralPlasticity(const std::vector<std::shared_ptr<Synapse>>& synapses,
                             const std::vector<float>& eligibilityTraces,
                             float learningRateMultiplier = 1.0f);
    void integrateWithPredictionSystem(float predictedValue, float actualValue);
    void temporalAdaptation(float environmentComplexity);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm