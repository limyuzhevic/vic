#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float receptorDensity;
    float receptorSaturation;
    float cAMPPathway;
    float MAPKinasePathway;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.5f)
        , reuptakeRate(0.12f)
        , receptorDensity(1.0f)
        , receptorSaturation(0.7f)
        , cAMPPathway(0.5f)
        , MAPKinasePathway(0.5f) {}
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
    // Norepinephrine modulates arousal and vigilance-dependent learning
    float effectiveLevel = std::min(pImpl->level / pImpl->receptorSaturation, 1.0f);
    
    // NE enhances signal detection through beta-adrenergic receptors
    // Improves encoding of salient stimuli
    float arousalGain = 1.0f + 1.2f * effectiveLevel;
    
    // NE modulates working memory through locus coeruleus projections
    float memoryStability = 0.8f + 0.4f * effectiveLevel;
    
    // NE enhances encoding of emotionally significant events
    float salienceWeight = 1.0f + 0.6f * effectiveLevel;
    
    // Combined effect on plasticity
    float plasticityFactor = arousalGain * memoryStability * salienceWeight;
    
    return std::max(0.5f, std::min(plasticityFactor, 3.0f));
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Multiple clearance mechanisms
    float decay = pImpl->decayRate * dtFloat;
    
    // Enzymatic degradation (MAO, COMT)
    float degradation = decay * 0.7f;
    
    // Reuptake by norepinephrine transporters (NET)
    float reuptake = pImpl->reuptakeRate * pImpl->level * dtFloat;
    
    // Synaptic vesicle release clearance
    float vesicularRelease = 0.05f * dtFloat;
    
    // Apply clearance
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - degradation - reuptake);
    
    // Update signaling pathways
    pImpl->cAMPPathway = std::max(0.1f, pImpl->cAMPPathway - 0.01f * dtFloat);
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // Arousal signaling through locus coerulean pathways
    // Phasic bursts for novel or challenging stimuli
    
    // Burst magnitude scaled by arousal strength
    float burstSize = arousalStrength * pImpl->releaseRate * 
                      (1.0f + pImpl->cAMPPathway * 0.3f);
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    
    // Activate downstream pathways
    pImpl->cAMPPathway = std::min(1.0f, pImpl->cAMPPathway + 0.2f);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // Vigilance signaling through prefrontal and thalamic projections
    // Enhances attentional focus and monitoring
    
    // Vigilance boost scaled by strength and pathway activation
    float vigilanceBoost = vigilanceStrength * pImpl->releaseRate * 
                           (1.0f + pImpl->MAPKinasePathway * 0.4f);
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceBoost);
    
    // Activate MAPK/ERK pathway for sustained effects
    pImpl->MAPKinasePathway = std::min(1.0f, pImpl->MAPKinasePathway + 0.3f);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->cAMPPathway = 0.5f;
    pImpl->MAPKinasePathway = 0.5f;
}

} // namespace nlm
