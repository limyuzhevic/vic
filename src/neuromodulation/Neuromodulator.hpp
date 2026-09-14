//neuromodulator.hpp
#pragma once

#include <memory>
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"

namespace nlm {

class Neuromodulator {
public:
    Neuromodulator(float scale = 1.0f, float decay = 0.1f, float maxLevel = 1.0f);
    virtual ~Neuromodulator() = default;
    
    virtual void initialize(Brain* brain) {
        brain_ = brain;
    }
    
    virtual void update(float timestep) {
        currentLevel_ = std::clamp(currentLevel_ * decay_, 0.0f, maxLevel_);
    }
    
    virtual void setLevel(float level) {
        currentLevel_ = std::clamp(level, 0.0f, maxLevel_);
    }
    
    virtual float getLevel() const { return currentLevel_; }
    
    virtual void setScale(float scale) { scale_ = scale; }
    virtual float getScale() const { return scale_; }
    
    virtual void setDecay(float decay) { decay_ = decay; }
    virtual float getDecay() const { return decay_; }
    
    virtual void setMaxLevel(float maxLevel) { maxLevel_ = maxLevel; }
    virtual float getMaxLevel() const { return maxLevel_; }
    
    virtual void addSignal(float signal) {
        currentLevel_ = std::clamp(currentLevel_ + signal * scale_, 0.0f, maxLevel_);
    }
    
    virtual float getPlasticityFactor() const {
        return 1.0f + (currentLevel_ * 0.5f);  // Positive scaling
    }
    
    // Apply effects to neurons
    virtual void applyToNeuron(Neuron* neuron) {
        if (!neuron || currentLevel_ <= 0.0f) return;
        
        // Generic neuromodulatory effect: adjust excitability
        float effect = currentLevel_ * scale_;
        neuron->injectCurrent(effect);
    }
    
protected:
    Brain* brain_ = nullptr;
    float currentLevel_ = 0.0f;
    float scale_ = 1.0f;
    float decay_ = 0.1f;
    float maxLevel_ = 1.0f;
};

// Enhanced dopamine neuromodulator
class Dopamine : public Neuromodulator {
public:
    Dopamine() : Neuromodulator(1.0f, 0.05f, 2.0f) {}  // Higher max for dopamine
    
    virtual void initialize(Brain* brain) override {
        Neuromodulator::initialize(brain);
        // Dopamine-specific initialization
        plasticityModulation_ = 1.0f;
        rewardPredictionError_ = 0.0f;
    }
    
    virtual void update(float timestep) override {
        Neuromodulator::update(timestep);
        // Decay prediction error over time
        rewardPredictionError_ *= 0.95f;
    }
    
    void setRewardPredictionError(float error) {
        rewardPredictionError_ = error;
        addSignal(error); // Error directly affects dopamine level
    }
    
    float getPredictionError() const { return rewardPredictionError_; }
    
    virtual float getPlasticityFactor() const override {
        // Dopamine has complex effects: promotes LTP when positive, LTD when negative
        float baseFactor = Neuromodulator::getPlasticityFactor();
        if (rewardPredictionError_ > 0.01f) {
            // Strong reward prediction error -> enhanced learning (LTP)
            return baseFactor * 1.5f * std::min(1.0f + rewardPredictionError_, 2.0f);
        } else if (rewardPredictionError_ < -0.01f) {
            // Strong prediction error -> reduced learning, promotes forgetting (LTD)
            return baseFactor * 0.7f * std::max(1.0f + rewardPredictionError_ * 0.5f, 0.3f);
        }
        return baseFactor;
    }
    
    // Apply dopamine-specific effects to neurons
    void applyToNeuron(Neuron* neuron) override {
        if (!neuron) return;
        
        float level = getLevel();
        if (level <= 0.0f) return;
        
        // Dopamine modulates both excitability and plasticity
        // Positive dopamine = increased excitability and LTP promotion
        float excitabilityEffect = level * scale_ * 0.8f; // Stronger excitability effect
        neuron->injectCurrent(excitabilityEffect);
        
        // Also affects learning rate for this neuron
        neuron->setLearningModulation(level);
    }
    
    float getPlasticityModulation() const { return plasticityModulation_; }
    void setPlasticityModulation(float mod) { plasticityModulation_ = mod; }
    
private:
    float rewardPredictionError_ = 0.0f;
    float plasticityModulation_ = 1.0f;
};

// Acetylcholine (ACh) - attention and learning modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine() : Neuromodulator(0.8f, 0.08f, 1.5f) {}
    
    virtual void initialize(Brain* brain) override {
        Neuromodulator::initialize(brain);
        attentionLevel_ = 0.5f;
        learningBoost_ = 1.0f;
    }
    
    virtual void update(float timestep) override {
        Neuromodulator::update(timestep);
        attentionLevel_ = getLevel() * 0.8f;  // ACh strongly affects attention
        learningBoost_ = 1.0f + getLevel() * 0.5f; // Enhances learning
    }
    
    float getAttentionLevel() const { return attentionLevel_; }
    float getLearningBoost() const { return learningBoost_; }
    
    // Apply to neurons with attention effects
    void applyToNeuron(Neuron* neuron) override {
        if (!neuron) return;
        
        float level = getLevel();
        if (level <= 0.0f) return;
        
        // ACh modulates signal-to-noise ratio for attention
        float attentionEffect = level * scale_ * 1.2f;
        neuron->injectCurrent(attentionEffect);
        
        // Boost learning for attended neurons
        neuron->setLearningBoost(learningBoost_);
        neuron->setAttentionLevel(attentionLevel_);
    }
    
private:
    float attentionLevel_ = 0.0f;
    float learningBoost_ = 1.0f;
};

// Norepinephrine (NE) - arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine() : Neuromodulator(0.7f, 0.06f, 2.0f) {}
    
    virtual void update(float timestep) override {
        Neuromodulator::update(timestep);
        // NE affects arousal and response gain
        arousalLevel_ = getLevel() * 1.5f;
    }
    
    float getArousalLevel() const { return arousalLevel_; }
    
    // Apply arousal effects to neurons
    void applyToNeuron(Neuron* neuron) override {
        if (!neuron) return;
        
        float level = getLevel();
        if (level <= 0.0f) return;
        
        // NE increases response gain and baseline firing
        float arousalEffect = level * scale_ * 0.9f;
        neuron->injectCurrent(arousalEffect);
        neuron->setArousalLevel(arousalLevel_);
    }
    
private:
    float arousalLevel_ = 0.0f;
};

// Serotonin (5-HT) - mood and behavioral regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin() : Neuromodulator(0.6f, 0.12f, 1.8f) {}  // Slower decay for serotonin
    
    virtual void initialize(Brain* brain) override {
        Neuromodulator::initialize(brain);
        moodLevel_ = 0.5f;
        behavioralSigmoid_ = 1.0f;
    }
    
    virtual void update(float timestep) override {
        Neuromodulator::update(timestep);
        // Serotonin affects mood and behavioral tendency
        moodLevel_ = getLevel() * 1.2f;
        behavioralSigmoid_ = 1.0f - getLevel() * 0.3f; // High serotonin = more stable behavior
    }
    
    float getMoodLevel() const { return moodLevel_; }
    float getBehavioralSigmoid() const { return behavioralSigmoid_; }
    
    // Apply mood and behavioral effects
    void applyToNeuron(Neuron* neuron) override {
        if (!neuron) return;
        
        float level = getLevel();
        if (level <= 0.0f) return;
        
        // Serotonin modulates emotional state and behavioral threshold
        float moodEffect = level * scale_ * 0.7f;
        neuron->injectCurrent(moodEffect);
        neuron->setMoodLevel(moodLevel_);
        neuron->setBehavioralThreshold(behavioralSigmoid_);
    }
    
private:
    float moodLevel_ = 0.0f;
    float behavioralSigmoid_ = 1.0f;
};

} // namespace nlm