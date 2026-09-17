#include "BehavioralDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct BehavioralDevelopment::Impl {
    double lastUpdate;
    float developmentRate;
    
    Impl() : lastUpdate(0.0), developmentRate(0.01f) {}
};

BehavioralDevelopment::BehavioralDevelopment() : pImpl(new Impl) {}

BehavioralDevelopment::~BehavioralDevelopment() = default;

void BehavioralDevelopment::update(Brain* brain, RandomGenerator& rng, double age, double dt) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    developExploration(brain, rng, age);
    developAttention(brain, rng, age);
    developStressResponse(brain, rng, age);
    developSocialBehavior(brain, rng, age);
}

void BehavioralDevelopment::developExploration(Brain* brain, RandomGenerator& rng, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Exploration develops early and then stabilizes
    profile.curiosityDrive = 0.3f + 0.6f * static_cast<float>(ageFraction);
    
    // Novelty seeking develops with age
    profile.noveltySeeking = 0.4f + 0.5f * static_cast<float>(ageFraction);
}

void BehavioralDevelopment::developAttention(Brain* brain, RandomGenerator& rng, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Attention span develops with age
    profile.habituationRate = 0.2f + 0.6f * static_cast<float>(ageFraction);
    profile.attentionSpan = 5.0f + 15.0f * static_cast<float>(ageFraction);
}

void BehavioralDevelopment::developStressResponse(Brain* brain, RandomGenerator& rng, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Stress response develops and then decreases
    profile.stressResponsiveness = 0.8f - 0.6f * static_cast<float>(ageFraction);
}

void BehavioralDevelopment::developSocialBehavior(Brain* brain, RandomGenerator& rng, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Social tendencies develop with age
    profile.socialTendencies = 0.3f + 0.6f * static_cast<float>(ageFraction);
    
    // Aggression decreases with age
    profile.aggressionLevel = 0.3f - 0.25f * static_cast<float>(ageFraction);
}

BehavioralProfile BehavioralDevelopment::getProfile() const {
    return profile;
}

float BehavioralDevelopment::getBehavioralTendency(const char* behaviorName) const {
    if (strcmp(behaviorName, "curiosity") == 0) return profile.curiosityDrive;
    if (strcmp(behaviorName, "noveltySeeking") == 0) return profile.noveltySeeking;
    if (strcmp(behaviorName, "habituation") == 0) return profile.habituationRate;
    if (strcmp(behaviorName, "stress") == 0) return profile.stressResponsiveness;
    if (strcmp(behaviorName, "social") == 0) return profile.socialTendencies;
    if (strcmp(behaviorName, "aggression") == 0) return profile.aggressionLevel;
    return 0.5f;
}

} // namespace nlm