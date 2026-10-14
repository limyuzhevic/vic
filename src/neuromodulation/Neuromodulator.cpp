#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float attentionWeight;
    float memoryWeight;
    float behavioralWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), brain(nullptr),
             attentionWeight(0.3f), memoryWeight(0.4f), behavioralWeight(0.3f) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        // Set baseline from brain configuration
        // In real implementation, this would be more sophisticated
        pImpl->baseline = 0.1f;
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

float Dopamine::getAttentionModulation() const {
    // Dopamine modulates attention towards rewarded stimuli
    return 1.0f + pImpl->attentionWeight * getLevel();
}

float Dopamine::getMemoryModulation() const {
    // Dopamine enhances memory consolidation for rewarding events
    return 1.0f + pImpl->memoryWeight * getLevel();
}

float Dopamine::getBehavioralModulation() const {
    // Dopamine drives goal-directed behavior
    return 1.0f + pImpl->behavioralWeight * getLevel();
}

// Acetylcholine Implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float attentionGain;
    float memoryConsolidationRate;
    std::vector<float> attentionFocus;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f), brain(nullptr),
             attentionGain(1.5f), memoryConsolidationRate(0.8f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances synaptic plasticity for attended information
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.05f;
    }
}

void Acetylcholine::focusAttention(const std::vector<float>& features) {
    // Normalize features to determine attention focus
    pImpl->attentionFocus = features;
    float maxFeature = 0.0f;
    for (float feat : features) {
        maxFeature = std::max(maxFeature, feat);
    }
    if (maxFeature > 0.0f) {
        for (float& feat : pImpl->attentionFocus) {
            feat /= maxFeature;
        }
    }
    
    // Increase ACh level based on attention focus
    float attentionStrength = std::accumulate(pImpl->attentionFocus.begin(), 
                                            pImpl->attentionFocus.end(), 0.0f) / 
                            pImpl->attentionFocus.size();
    setLevel(std::min(1.0f, getLevel() + attentionStrength * pImpl->releaseRate * 0.5f));
}

void Acetylcholine::consolidateMemory() {
    if (!pImpl->brain) return;
    
    // ACh enhances working memory consolidation
    NeuralWorkingMemory* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        // Increase consolidation rate based on ACh level
        float consolidationBoost = getLevel() * pImpl->memoryConsolidationRate;
        // In real implementation, this would affect memory consolidation timing
        NLM_LOG_INFO("Acetylcholine: Enhancing memory consolidation (boost: " + 
                    std::to_string(consolidationBoost) + ")");
    }
}

float Acetylcholine::getAttentionModulation() const {
    // ACh directly enhances attentional processes
    return 1.0f + pImpl->attentionGain * getLevel();
}

float Acetylcholine::getMemoryModulation() const {
    // ACh enhances memory encoding and consolidation
    return 1.0f + pImpl->memoryConsolidationRate * getLevel();
}

// Norepinephrine Implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float arousalSensitivity;
    float vigilanceGain;
    std::vector<float> noveltySignature;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f), brain(nullptr),
             arousalSensitivity(1.2f), vigilanceGain(1.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for salient/novel events
    return 0.2f + 0.8f * getLevel();
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.05f;
    }
}

void Norepinephrine::signalNovelty(const std::vector<float>& input) {
    // Calculate novelty based on input statistics
    float mean = std::accumulate(input.begin(), input.end(), 0.0f) / input.size();
    float variance = 0.0f;
    for (float val : input) {
        variance += (val - mean) * (val - mean);
    }
    variance /= input.size();
    
    float novelty = std::sqrt(variance) / (mean + 0.1f);  // Normalize by mean
    pImpl->noveltySignature = input;
    
    // Increase NE level based on novelty
    setLevel(std::min(1.0f, getLevel() + novelty * pImpl->releaseRate * 0.8f));
}

void Norepinephrine::increaseArousal(float salience) {
    // NE increases arousal based on stimulus salience
    setLevel(std::min(1.0f, getLevel() + salience * pImpl->releaseRate * 1.0f));
}

float Norepinephrine::getArousalModulation() const {
    // NE directly modulates arousal/alertness
    return 1.0f + pImpl->arousalSensitivity * getLevel();
}

float Norepinephrine::getAttentionModulation() const {
    // NE enhances alertness and selective attention
    return 1.0f + pImpl->vigilanceGain * getLevel();
}

// Serotonin Implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float moodWeight;
    float impulsivityControl;
    float socialLearningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f), brain(nullptr),
             moodWeight(1.1f), impulsivityControl(1.4f), socialLearningRate(0.9f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on mood/state
    float modulation = getLevel();
    if (modulation > 0.7f) {
        // High serotonin reduces certain types of plasticity (prevents over-learning)
        return 0.6f - 0.4f * (modulation - 0.7f) * 2.0f;
    } else {
        // Moderate serotonin enhances adaptive plasticity
        return 0.7f + 0.3f * modulation;
    }
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.03f;
    }
}

void Serotonin::regulateMood(bool socialPositive, float socialValue) {
    // Serotonin regulates mood based on social feedback
    float moodChange = socialValue;
    if (!socialPositive) {
        moodChange -= 0.5f;  // Social defeat reduces serotonin
    }
    
    setLevel(std::min(1.0f, getLevel() + moodChange * pImpl->releaseRate * 0.7f));
}

void Serotonin::controlImpulsivity(bool hasWaitOption) {
    // Serotonin helps control impulsive behavior
    if (hasWaitOption) {
        // Having a wait option reduces impulsivity
        setLevel(getLevel() * 0.95f);
    } else {
        // No wait option may increase impulsivity
        setLevel(std::min(1.0f, getLevel() + 0.2f * pImpl->releaseRate));
    }
}

float Serotonin::getBehavioralModulation() const {
    // Serotonin modulates behavioral responses (patience, mood)
    return 1.0f + pImpl->moodWeight * getLevel() - pImpl->impulsivityControl * (1.0f - getLevel()) * 0.5f;
}

float Serotonin::getAttentionModulation() const {
    // Serotonin modulates attentional focus and impulsivity
    return 1.0f - 0.3f * getLevel();  // High serotonin reduces distractibility
}

} // namespace nlm
