#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float rewardWeight;
    float punishmentWeight;
    float socialWeight;
    float moodWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , rewardWeight(0.3f)
        , punishmentWeight(0.3f)
        , socialWeight(0.2f)
        , moodWeight(0.2f)
        , decayRate(0.03f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (5-HT) system initialized");
}

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = level;
}

void Serotonin::update(float reward, float punishment, float social, float mood, TimestepDuration dt) {
    // 5-HT increases with reward, social interaction, and mood
    float target = pImpl->rewardWeight * reward + 
                   pImpl->punishmentWeight * punishment +
                   pImpl->socialWeight * social + 
                   pImpl->moodWeight * mood;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on reward/punishment balance
    float rewardPunishmentBalance = pImpl->rewardWeight - pImpl->punishmentWeight;
    if (rewardPunishmentBalance > 0.3f) {
        // High reward bias - enhances plasticity
        return 1.0f + pImpl->level * 0.4f;
    } else if (rewardPunishmentBalance < -0.3f) {
        // High punishment bias - reduces plasticity
        return std::max(0.1f, 1.0f - pImpl->level * 0.3f);
    }
    return 1.0f;  // Balanced
}

void Serotonin::setRewardWeight(float weight) {
    pImpl->rewardWeight = weight;
}

void Serotonin::setPunishmentWeight(float weight) {
    pImpl->punishmentWeight = weight;
}

void Serotonin::setSocialWeight(float weight) {
    pImpl->socialWeight = weight;
}

void Serotonin::setMoodWeight(float weight) {
    pImpl->moodWeight = weight;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
