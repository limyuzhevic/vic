#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Enhanced dopamine-modulated plasticity factor
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

float Dopamine::getRewardPredictionError() const {
    // Return current reward prediction error (could be stored separately)
    return pImpl->level * 0.5f; // Simplified calculation
}

void Dopamine::setRewardPredictionError(float error) {
    pImpl->level = std::clamp(error, 0.0f, 1.0f);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float attentionStrength;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), attentionStrength(1.0f), memoryBoost(1.5f) {}
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
    // ACh enhances plasticity for attention and memory
    return 0.7f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline but can be boosted by attention signals
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Real attention signaling - ACh spikes on focused attention
    pImpl->level = std::min(1.0f, pImpl->level + attentionSignal * pImpl->attentionStrength);
}

void Acetylcholine::signalMemoryConsolidation() {
    // Signal memory consolidation event
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f * pImpl->memoryBoost);
}

void Acetylcholine::enhanceWorkingMemory(NeuralWorkingMemory* wm) {
    if (wm && pImpl->level > 0.5f) {
        // ACh significantly enhances working memory capacity and persistence
        wm->setDecayRate(wm->getDecayRate() * (1.0f - 0.5f * pImpl->level));
        // Boost memory trace strength
        wm->strengthenMemory(1.0f + pImpl->level * 0.5f);
    }
}

void Acetylcholine::modulateAttention(AttentionalSelection* attention) {
    if (attention && pImpl->level > 0.3f) {
        // ACh reduces inhibition strength, allowing more focused attention
        float currentInhibition = attention->getInhibitionStrength();
        attention->setInhibitionStrength(currentInhibition * (1.0f - pImpl->level * 0.4f));
        
        // ACh increases excitation strength
        float currentExcitation = attention->getExcitationStrength();
        attention->setExcitationStrength(currentExcitation * (1.0f + pImpl->level * 0.3f));
    }
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

float Acetylcholine::getDecayRate() const {
    return pImpl->decayRate;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float arousalStrength;
    float vigilanceStrength;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.08f), arousalStrength(1.0f), vigilanceStrength(0.8f) {}
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
    // NE modulates plasticity based on arousal state
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics with arousal and vigilance effects
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Real arousal signaling - NE spikes on salient events
    pImpl->level = std::min(1.0f, pImpl->level + arousalSignal * pImpl->arousalStrength);
}

void Norepinephrine::signalVigilance() {
    // Signal vigilance - increases NE level to maintain alertness
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f * pImpl->vigilanceStrength);
}

void Norepinephrine::modulateExcitability(Brain* brain) {
    if (brain && pImpl->level > 0.3f) {
        // NE increases neural excitability across the brain
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE injects additional current to increase excitability
                    neuron->injectCurrent(pImpl->level * 0.3f);
                }
            }
        }
    }
}

void Norepinephrine::enhanceSignalDetection(class SensoryInput* input) {
    if (input && pImpl->level > 0.5f) {
        // NE enhances sensory signal detection and processing
        // This would involve strengthening sensory neuron responses
        // For now, just note that enhancement would occur
    }
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

void Norepinephrine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

float Norepinephrine::getDecayRate() const {
    return pImpl->decayRate;
}

void Norepinephrine::setArousalStrength(float strength) {
    pImpl->arousalStrength = strength;
}

float Norepinephrine::getArousalStrength() const {
    return pImpl->arousalStrength;
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float moodStrength;
    float socialStrength;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.06f), moodStrength(0.8f), socialStrength(0.7f) {}
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
    // 5-HT modulates plasticity for mood and behavior control
    return 0.8f - 0.3f * pImpl->level;  // High 5-HT = lower plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics with mood stabilization
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodSignal) {
    // Real mood signaling - 5-HT affects mood and emotional state
    pImpl->level = std::min(1.0f, pImpl->level + moodSignal * pImpl->moodStrength);
}

void Serotonin::signalImpulsivityControl() {
    // Signal impulsivity control event
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f * pImpl->socialStrength);
}

void Serotonin::modulateSocialBehavior() {
    // Enhance social behavior modulation
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f * pImpl->socialStrength);
}

void Serotonin::modulatePlasticity(STDP* stdp, Hebbian* hebbian) {
    if (stdp && hebbian && pImpl->level > 0.5f) {
        // High 5-HT reduces plasticity for behavioral stability
        stdp->setLTPWeight(stdp->getLTPWeight() * (1.0f - 0.5f * pImpl->level));
        stdp->setLTDWeight(stdp->getLTDWeight() * (1.0f - 0.5f * pImpl->level));
        hebbian->setLearningRate(hebbian->getLearningRate() * (1.0f - 0.4f * pImpl->level));
    }
}

void Serotonin::reduceImpulsivity(NeuralPlanner* planner) {
    if (planner && pImpl->level > 0.3f) {
        // High 5-HT reduces impulsive actions and promotes planned behavior
        planner->setPlanningDepth(planner->getPlanningDepth() * (1.0f + pImpl->level));
    }
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

void Serotonin::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

float Serotonin::getDecayRate() const {
    return pImpl->decayRate;
}

void Serotonin::setSocialStrength(float strength) {
    pImpl->socialStrength = strength;
}

float Serotonin::getSocialStrength() const {
    return pImpl->socialStrength;
}

} // namespace nlm