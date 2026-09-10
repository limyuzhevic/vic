#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float lastRewardTime;
    float rewardHistory[10];  // Simple reward history for prediction error
    size_t historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , lastRewardTime(0.0f), historyIndex(0) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on neuroscience research
    // Dopamine modulates synaptic strength through D1 and D2 receptors
    // Higher dopamine generally enhances LTP (learning) but can also enhance LTD
    float factor = 0.5f + 0.5f * pImpl->level;
    // Add some non-linearity for realistic dynamics
    return factor * factor; // Quadratic relationship
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on reinforcement learning
    // Dopamine tracks reward prediction errors
    // Decay rate depends on baseline activity
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Store previous reward for prediction error calculation
    std::rotate(std::begin(pImpl->rewardHistory), std::begin(pImpl->rewardHistory) + 1, std::end(pImpl->rewardHistory));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on neuroscience research
    // Dopamine neurons show phasic responses to unexpected rewards
    // Response magnitude depends on reward prediction error
    
    // Add reward to history
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Calculate average reward for prediction error
    float avgReward = 0.0f;
    for (int i = 0; i < 10; ++i) avgReward += pImpl->rewardHistory[i];
    avgReward /= 10.0f;
    
    // Burst of dopamine on reward (magnitude proportional to reward size)
    float burst = std::min(pImpl->peak, reward * pImpl->releaseRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    pImpl->lastRewardTime = 0.0f; // Reset timing
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Dopamine responds to unexpected rewards or omissions
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    
    // Response depends on error magnitude and sign
    float response = error * pImpl->releaseRate;
    
    // Cap response magnitude and ensure non-negative
    pImpl->level = std::max(0.0f, pImpl->level + response);
    pImpl->level = std::min(pImpl->peak, pImpl->level);
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), learningRate(0.8f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates attention and memory consolidation
    // Higher ACh enhances working memory and attentional focus
    // Influences plasticity through nicotinic and muscarinic receptors
    float factor = 0.3f + 0.7f * pImpl->level;
    // Non-linear relationship for realistic attention dynamics
    return factor * factor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics based on attentional modulation
    // Tracks environmental uncertainty and attention demands
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::signalStimulus(float salience) {
    // Signal novel or salient stimuli
    float burst = std::min(pImpl->peak, salience * pImpl->learningRate);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Acetylcholine::signalAttentionShift(float bias) {
    // Modulate attention based on top-down signals
    float response = bias * pImpl->learningRate;
    pImpl->level = std::clamp(pImpl->level + response, 0.0f, pImpl->peak);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), arousalLevel(0.5f), vigilanceLevel(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and vigilance
    // Affects learning under stress and in novel environments
    // Enhances memory consolidation for salient events
    float factor = 0.4f + 0.6f * pImpl->level;
    // Arousal-dependent plasticity
    return factor * factor * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on arousal and stress
    // Tracks environmental demands and vigilance requirements
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::signalStress(float stressLevel) {
    // Respond to stress and challenge
    float response = stressLevel * pImpl->arousalLevel;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

void Norepinephrine::signalNovelty(float novelty) {
    // Respond to novel and unpredictable stimuli
    float response = novelty * pImpl->vigilanceLevel;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulseControl;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), moodLevel(0.5f), impulseControl(0.7f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood, impulsivity, and social behavior
    // Affects decision-making and behavioral flexibility
    // Low serotonin associated with impulsivity and aggression
    float factor = 0.2f + 0.8f * pImpl->level;
    // Mood-dependent plasticity
    return factor * pImpl->moodLevel;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics based on mood and social context
    // Tracks emotional state and social feedback
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::signalReward(float reward) {
    // Response to rewarding stimuli (especially social rewards)
    float response = reward * pImpl->impulseControl;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

void Serotonin::signalSocialFeedback(float feedback) {
    // Response to social interaction and feedback
    float response = feedback * pImpl->moodLevel;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response);
}

} // namespace nlm
