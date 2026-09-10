#pragma once

#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <random>

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
    
    // Real dopamine dynamics for reward and reinforcement learning
    void signalReward(float reward, float predictionError = 0.0f);
    void signalUnexpectedOutcome(float outcome, float expected);
    void signalTemporalDifference(float tdError);
    
    // Dopamine regulation mechanisms
    void engageVolumeTransmission();      // Diffuse modulatory signal
    void engagePointToPoint();           // Targeted synaptic release
    void decayToBaseline(TimestepDuration dt);
    
    // Computational model of dopamine dynamics
    void setTimeConstant(float tc) { timeConstant_ = tc; }
    void setBaseline(float baseline) { baseline_ = baseline; }
    void setLearningRate(float lr) { learningRate_ = lr; }
    
    // Get dopamine-related statistics
    float getBurstFrequency() const { return burstFrequency_; }
    float getAdaptationLevel() const { return adaptation_; }
    float getHabituation() const { return habituation_; }
    
    // Reset and clear
    void reset() override {
        setLevel(baseline_);
        burstFrequency_ = 0.0f;
        adaptation_ = 0.0f;
        habituation_ = 0.0f;
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Computational parameters
    float timeConstant_;
    float baseline_;
    float learningRate_;
    
    // Dynamic state
    float burstFrequency_;
    float adaptation_;
    float habituation_;
    
    // Internal clocks for temporal processing
    float lastBurstTime_;
    float integrationTime_;
};

} // namespace nlm
