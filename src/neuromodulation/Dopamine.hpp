#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward and prediction error signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced dopamine effects
    // Synaptic plasticity beyond basic scaling
    float getEnhancedPlasticity() const;
    float getMetaPlasticity() const;
    
    // Working memory stability
    float getWorkingMemoryStability() const;
    void setWorkingMemoryRecency(float recency);
    
    // Attention and selection
    float getAttentionGain() const;
    float getSelectionPriority() const;
    void setAttentionContext(const std::string& context);
    
    // Prediction error learning
    float getPredictionError() const;
    void setPredictionError(float error);
    
    // Sensory and motor gating
    float getSensoryGain() const;
    float getMotorGain() const;
    void setSensoryFilter(float filter);
    void setMotorCommandFilter(float filter);
    
    // Cross-system integration
    void integrateWithWorkingMemory(float stability, float recency);
    void integrateWithPredictionSystem(float error);
    void integrateWithPlanningSystem(float value);
    void integrateWithEpisodicMemory(float strength);
    void integrateWithConceptFormation(float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
