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
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionSignal;
    float memorySignal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f)
        , attentionSignal(0.0f), memorySignal(0.0f) {}
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
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    pImpl->attentionSignal = std::clamp(attention, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->attentionSignal * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::signalLearningEvent(float predictionError) {
    pImpl->memorySignal = std::clamp(std::abs(predictionError), 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->memorySignal * pImpl->releaseRate * 0.3f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalSignal;
    float noveltySignal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.2f)
        , arousalSignal(0.0f), noveltySignal(0.0f) {}
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
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelty(float novelty) {
    pImpl->noveltySignal = std::clamp(novelty, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->noveltySignal * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::signalRewardPredictionError(float error) {
    pImpl->arousalSignal = std::clamp(std::abs(error), 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->arousalSignal * pImpl->releaseRate * 0.6f);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousalSignal = std::clamp(arousalLevel, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->arousalSignal * pImpl->releaseRate * 0.4f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialSignal;
    float rewardSignal;
    float punishmentSignal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f)
        , socialSignal(0.0f), rewardSignal(0.0f), punishmentSignal(0.0f) {}
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
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialContext(float socialCohesion) {
    pImpl->socialSignal = std::clamp(socialCohesion, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->socialSignal * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalPunishment(float punishment) {
    pImpl->punishmentSignal = std::clamp(punishment, 0.0f, 1.0f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->punishmentSignal * pImpl->releaseRate * 0.4f);
}

void Serotonin::signalReward(float reward) {
    pImpl->rewardSignal = std::clamp(reward, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->rewardSignal * pImpl->releaseRate * 0.3f);
}

} // namespace nlm
