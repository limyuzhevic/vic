#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <vector>
#include <memory>
#include <cmath>

namespace nlm {

// Dopamine neuromodulator: Comprehensive implementation of reward signaling,
// reinforcement learning, and neuromodulatory effects on neural plasticity
// Based on biological dopamine dynamics including tonic and phasic release,
// reward prediction error computation, and synaptic modulation

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;

    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;

    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);

    // Real neuromodulation dynamics
    void setTonicLevel(float level);
    void setPhasicGain(float gain);
    float getTonicLevel() const;
    float getPhasicGain() const;

    // Baseline adaptation and homeostatic regulation
    void adaptBaseline(float targetLevel, float rate);
    void applyHomeostaticRegulation(float rewardHistory, float timeConstant);

    // Eligibility traces for temporal difference learning
    void updateEligibilityTrace(float reward, float temporalDifference);
    float getEligibilityTrace() const;
    void clearEligibilityTrace();

    // Spike-based release mechanism
    void simulatePhasicRelease();
    void simulateTonicFiring(TimestepDuration dt);

    // Integration with brain systems
    void integrateWithWorkingMemory(NeuralWorkingMemory* workingMemory);
    void integrateWithCuriosity(Curiosity* curiosity);
    void integrateWithPredictionError(PredictionError* predictionError);
    void integrateWithBrainSystems(Brain* brain);

    // Real plasticity effects
    void modulateSTDP(STDP* stdp, float dopamineLevel, TimestepDuration dt);
    void modulateHebbian(Hebbian* hebbian, float dopamineLevel, TimestepDuration dt);
    void modulateStructuralPlasticity(StructuralPlasticity* structuralPlasticity,
                                      float dopamineLevel, TimestepDuration dt);
    void enhanceNeuralExcitability(std::vector<Neuron*>& neurons, float dopamineLevel);

    // Reward prediction error computation
    void computeRewardPredictionError(float predicted, float actual, float discountFactor = 0.99f);
    float getCurrentRPE() const;
    float getTemporalDifference() const;

    // Biological plausibility features
    void applySTPLTP(float weight, float timeDifference);
    void applySTPLTD(float weight, float timeDifference);
    void applyShortTermPlasticity(Synapse* synapse, TimestepDuration dt);
    void applyLongTermPotentiation(Synapse* synapse, float LTPAmount);
    void applyLongTermDepression(Synapse* synapse, float LTDAmount);

    // State management
    const std::vector<float>& getHistory() const;
    float getPeakLevel() const;
    float getDecayRate() const;
    void setDecayRate(float rate);
    void resetHistory();

    // Integration utilities
    bool isIntegrated() const;
    void markIntegrated(bool integrated);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm