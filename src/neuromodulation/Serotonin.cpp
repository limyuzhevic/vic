#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float receptorDensity;
    float receptorSaturation;
    float rapheActivity;
    float tryptophanPool;
    float MAOActivity;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.06f)
        , releaseRate(1.2f)
        , reuptakeRate(0.15f)
        , receptorDensity(1.0f)
        , receptorSaturation(0.8f)
        , rapheActivity(0.5f)
        , tryptophanPool(1.0f)
        , MAOActivity(0.5f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior-dependent learning
    float effectiveLevel = std::min(pImpl->level / pImpl->receptorSaturation, 1.0f);
    
    // 5-HT enhances mood stabilization and reduces impulsivity
    // Improves reinforcement learning and decision making
    float moodStability = 1.0f + 0.8f * effectiveLevel;
    
    // 5-HT modulates social behavior and reward processing
    float socialReward = 1.0f + 0.6f * effectiveLevel;
    
    // 5-HT regulates aggression and anxiety
    float emotionalRegulation = 0.9f + 0.3f * effectiveLevel;
    
    // 5-HT affects learning rate adaptation
    float learningRateAdaptation = 1.0f + 0.4f * (1.0f - effectiveLevel); // Inverse relationship
    
    // Combined effect on plasticity
    float plasticityFactor = moodStability * socialReward * emotionalRegulation * learningRateAdaptation;
    
    return std::max(0.3f, std::min(plasticityFactor, 2.5f));
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Multiple clearance mechanisms
    float decay = pImpl->decayRate * dtFloat;
    
    // Monoamine oxidase (MAO) breakdown
    float degradation = decay * 0.8f;
    
    // Reuptake by serotonin transporters (SERT)
    float reuptake = pImpl->reuptakeRate * pImpl->level * dtFloat;
    
    // Synaptic vesicle release clearance
    float vesicularRelease = 0.03f * dtFloat;
    
    // Apply clearance
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - degradation - reuptake);
    
    // Update precursor availability
    pImpl->tryptophanPool = std::min(1.0f, pImpl->tryptophanPool + 0.01f * dtFloat);
    
    // Update MAO activity
    pImpl->MAOActivity = std::max(0.3f, pImpl->MAOActivity - 0.02f * dtFloat);
}

void Serotonin::signalMood(float moodStrength) {
    // Mood signaling through dorsal and median raphe nuclei
    // Phasic bursts for positive emotional states
    
    // Burst magnitude scaled by mood strength and precursor availability
    float burstSize = moodStrength * pImpl->releaseRate * 
                      (1.0f + pImpl->tryptophanPool * 0.4f);
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    
    // Enhance raphe activity
    pImpl->rapheActivity = std::min(1.0f, pImpl->rapheActivity + 0.3f);
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // Social behavior signaling through limbic system projections
    // Enhances social reward processing and bonding
    
    // Social boost scaled by strength and raphe activity
    float socialBoost = socialStrength * pImpl->releaseRate * 
                        (1.0f + pImpl->rapheActivity * 0.5f);
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialBoost);
    
    // Enhance social responsiveness
    pImpl->rapheActivity = std::min(1.0f, pImpl->rapheActivity + 0.5f);
    
    // Reduce MAO activity to maintain elevated levels
    pImpl->MAOActivity = std::max(0.2f, pImpl->MAOActivity - 0.3f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rapheActivity = 0.5f;
    pImpl->tryptophanPool = 1.0f;
    pImpl->MAOActivity = 0.5f;
}

} // namespace nlm
