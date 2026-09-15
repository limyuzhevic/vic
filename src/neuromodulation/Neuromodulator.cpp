#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Acetylcholine Implementation
struct AcetylcholineImpl {
    float level;
    float baseline;
    float peak;
    float memoryBoost;
    
    AcetylcholineImpl() : level(0.0f), baseline(0.0f), peak(1.0f), memoryBoost(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new AcetylcholineImpl) {}

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
    // ACh modulates memory consolidation and attention
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh supports memory consolidation and attention
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

// Norepinephrine Implementation
struct NorepinephrineImpl {
    float level;
    float baseline;
    float peak;
    float arousalBoost;
    float attentionBoost;
    
    NorepinephrineImpl() : level(0.0f), baseline(0.0f), peak(1.0f), arousalBoost(0.3f), attentionBoost(0.4f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new NorepinephrineImpl) {}

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
    // NE modulates arousal and attention
    return 0.7f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE supports arousal and attention
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.08f * static_cast<float>(dt));
}

// Serotonin Implementation
struct SerotoninImpl {
    float level;
    float baseline;
    float peak;
    float moodBoost;
    float impulsivityMod;
    
    SerotoninImpl() : level(0.0f), baseline(0.0f), peak(1.0f), moodBoost(0.2f), impulsivityMod(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new SerotoninImpl) {}

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
    // Serotonin modulates mood and social behavior
    return 0.6f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin supports mood regulation and social behavior
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.03f * static_cast<float>(dt));
}

// Novelty Implementation
struct NoveltyImpl {
    float level;
    float baseline;
    float peak;
    float noveltyBoost;
    float curiosityMod;
    
    NoveltyImpl() : level(0.0f), baseline(0.0f), peak(1.0f), noveltyBoost(0.6f), curiosityMod(0.4f) {}
};

Novelty::Novelty() : pImpl(new NoveltyImpl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty modulates exploration and curiosity
    return 0.4f + 0.6f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Novelty supports novelty detection and exploration
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.15f * static_cast<float>(dt));
}

// PredictionError Implementation
struct PredictionErrorImpl {
    float level;
    float baseline;
    float learningSignal;
    float predictionGain;
    
    PredictionErrorImpl() : level(0.0f), baseline(0.0f), learningSignal(1.0f), predictionGain(1.0f) {}
};

PredictionError::PredictionError() : pImpl(new PredictionErrorImpl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PE";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error directly modulates learning
    return pImpl->level;
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error supports learning and adaptation
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.2f * static_cast<float>(dt));
}

void PredictionError::signalPredictionError(float error) {
    // Prediction error signal from the environment
    pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->learningSignal);
}

// Curiosity Implementation
struct CuriosityImpl {
    float level;
    float baseline;
    float peak;
    float explorationGain;
    float noveltyThreshold;
    
    CuriosityImpl() : level(0.0f), baseline(0.0f), peak(1.0f), explorationGain(1.0f), noveltyThreshold(0.3f) {}
};

Curiosity::Curiosity() : pImpl(new CuriosityImpl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity is driven by novelty and prediction error
    float curiosityInput = novelty * 0.6f + predictionError * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + curiosityInput * pImpl->explorationGain);
    
    // Decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

// Reward Implementation
struct RewardImpl {
    float level;
    float baseline;
    float peak;
    float rewardGain;
    float predictionErrorGain;
    
    RewardImpl() : level(0.0f), baseline(0.0f), peak(1.0f), rewardGain(1.0f), predictionErrorGain(0.8f) {}
};

Reward::Reward() : pImpl(new RewardImpl) {}

Reward::~Reward() = default;

const char* Reward::getName() const {
    return "Reward";
}

float Reward::getLevel() const {
    return pImpl->level;
}

void Reward::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Reward::getPlasticityFactor() const {
    // Reward directly modulates dopamine
    return pImpl->level;
}

void Reward::update(TimestepDuration dt) {
    // Reward signal decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f * static_cast<float>(dt));
}

void Reward::signalReward(float reward) {
    // Reward signal from the environment
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->rewardGain);
}

void Reward::signalRewardPredictionError(float error) {
    // Reward prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->predictionErrorGain);
}

// AttentionalSelection Implementation
struct AttentionalSelectionImpl {
    float currentAttention;
    float baseline;
    float peak;
    float allocationRate;
    float decayRate;
    
    AttentionalSelectionImpl() : currentAttention(0.0f), baseline(0.0f), peak(1.0f), allocationRate(1.0f), decayRate(0.05f) {}
};

AttentionalSelection::AttentionalSelection() : pImpl(new AttentionalSelectionImpl) {}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Update attention level based on competition
    float totalActivity = 0.0f;
    for (NeuronId neuron : competitors) {
        // Simulate attention allocation based on activity
        totalActivity += 1.0f;  // Simplified
    }
    
    if (totalActivity > 0.0f) {
        pImpl->currentAttention = std::min(pImpl->peak, totalActivity * 0.3f);
    } else {
        pImpl->currentAttention = std::max(pImpl->baseline, pImpl->currentAttention - pImpl->decayRate);
    }
    
    // Apply inhibition to competitors based on attention level
    for (NeuronId neuron : competitors) {
        if (pImpl->currentAttention > 0.5f) {
            // Apply global inhibition if attention is high
            if (brain_) {
                brain_->injectCurrent(neuron, -globalInhibition * 2.0f);
            }
        }
    }
    
    // Select winners based on attention level
    for (NeuronId neuron : competitors) {
        if (pImpl->currentAttention > 0.5f && rand() % 100 < 70) {  // Simplified winner selection
            winners_.push_back(neuron);
        }
    }
    
    if (winners_.empty() && !competitors.empty()) {
        // Ensure at least one winner if competition is happening
        winners_.push_back(competitors[0]);
    }
    
    return winners_;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay attention over time
    pImpl->currentAttention = std::max(pImpl->baseline, pImpl->currentAttention - pImpl->decayRate);
    
    // Apply attention-based modulation to neural activity
    if (brain_ && pImpl->currentAttention > 0.1f) {
        for (NeuronId winner : winners_) {
            // Boost activity of attended neurons
            brain_->injectCurrent(winner, pImpl->currentAttention * 3.0f);
        }
    }
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    // Mark a region as being attended
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    pImpl->currentAttention = 0.0f;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return 1.0f - pImpl->currentAttention;  // Non-winners receive reduced inhibition
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return pImpl->currentAttention * 2.0f;  // Winners receive excitation
    }
    return 0.0f;
}
