#include "Norepinephrine.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float tonicNE;
    float phasicNE;
    float arousalBuffer;
    float attentionalLoad;
    float attentionalStability;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float recoveryRate;
    
    // Current attentional allocation
    std::vector<float> attentionAllocation;
    
    // Event history for sustained attention
    std::vector<float> eventHistory;
    
    // Focus mechanisms
    std::vector<std::pair<RegionId, float>> focusRegions;
    
    // Physiological parameters
    float noveltySensitivity;
    float predictionErrorSensitivity;
    
    // Adaptive mechanisms
    float adaptationRate;
    float fatigueRate;
    
    Impl() : tonicNE(0.0f)
        , phasicNE(0.0f)
        , arousalBuffer(0.0f)
        , attentionalLoad(0.0f)
        , attentionalStability(1.0f)
        , baselineLevel(0.05f)
        , peakLevel(1.0f)
        , decayRate(0.1f)
        , recoveryRate(0.05f)
        , noveltySensitivity(0.7f)
        , predictionErrorSensitivity(0.3f)
        , adaptationRate(0.02f)
        , fatigueRate(0.01f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl)
    , brain_(nullptr)
    , currentLevel_(0.0f)
    , attentionalLoad_(0.0f)
    , attentionalStability_(1.0f)
{
    pImpl->attentionAllocation.resize(1024, 0.0f);  // Will be resized when brain is set
}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->attentionAllocation.resize(brain_ ? brain->getTotalNeuronCount() : 1024);
    NLM_LOG_INFO("Norepinephrine (NE) initialized - attention modulation system ready");
}

float Norepinephrine::getLevel() const {
    return currentLevel_;
}

void Norepinephrine::setLevel(float level) {
    currentLevel_ = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getAttentionEnhancement() const {
    // NE enhances attention proportionally to level with saturation
    return 1.0f + currentLevel_ * 1.5f;  // 1.0x to 2.5x enhancement
}

float Norepinephrine::getWorkingMemoryBenefit() const {
    // NE supports working memory storage and retrieval
    return 1.0f + currentLevel_ * 0.5f;  // 1.0x to 1.5x benefit
}

void Norepinephrine::update(float novelty, float predictionError, TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Compute NE response based on inputs
    float noveltyResponse = novelty * pImpl->noveltySensitivity;
    float predictionErrorResponse = std::abs(predictionError) * pImpl->predictionErrorSensitivity;
    
    // Phasic NE bursts for novel events
    float phasicInput = std::max(noveltyResponse, predictionErrorResponse);
    if (phasicInput > 0.1f) {
        pImpl->phasicNE = std::min(pImpl->peakLevel, pImpl->phasicNE + phasicInput * 2.0f);
        eventHistory_.push_back(phasicInput);
        if (eventHistory_.size() > 50) eventHistory_.erase(eventHistory_.begin());
    }
    
    // Update arousal buffer
    pImpl->arousalBuffer = std::max(0.0f, pImpl->arousalBuffer - pImpl->decayRate * dtFloat);
    pImpl->phasicNE = std::max(0.0f, pImpl->phasicNE - pImpl->recoveryRate * dtFloat);
    
    // Compute current NE level
    float tonicNE = pImpl->tonicNE + pImpl->phasicNE;
    tonicNE = std::clamp(tonicNE, pImpl->baselineLevel, pImpl->peakLevel);
    
    // Update attentional load and stability
    attentionalLoad_ = std::min(1.0f, attentionalLoad_ + dtFloat * 0.2f);
    attentionalStability_ = 1.0f - attentionalLoad_ * 0.5f;  // More load = less stability
    
    // Update current level
    setLevel(tonicNE);
    
    // Update focus regions based on NE level
    updateFocusRegions(dtFloat);
    
    // Update neuronal attention allocation
    if (brain_ && focusRegions_.size() > 0) {
        updateAttentionAllocation(dtFloat);
    }
}

void Norepinephrine::signalEvent(float importance) {
    pImpl->phasicNE = std::min(pImpl->peakLevel, pImpl->phasicNE + importance * 0.5f);
    eventHistory_.push_back(importance);
    if (eventHistory_.size() > 50) eventHistory_.erase(eventHistory_.begin());
    
    NLM_LOG_INFO("Norepinephrine: Event signaled with importance " + std::to_string(importance));
}

void Norepinephrine::signalSustainedAttention(float focusDuration) {
    // Sustained attention increases baseline NE
    pImpl->tonicNE = std::min(pImpl->peakLevel, pImpl->tonicNE + focusDuration * 0.001f);
    attentionalLoad_ = std::max(0.1f, attentionalLoad_ - 0.2f * focusDuration);
    NLM_LOG_INFO("Norepinephrine: Sustained attention for " + std::to_string(focusDuration) + "s");
}

float Norepinephrine::getAttentionBias(class NeuronId neuron) const {
    if (neuron.value < pImpl->attentionAllocation.size()) {
        return pImpl->attentionAllocation[neuron.value];
    }
    return 0.0f;
}

void Norepinephrine::setFocusRegion(class RegionId region, float strength) {
    // Remove existing focus on this region
    for (auto it = focusRegions_.begin(); it != focusRegions_.end(); ++it) {
        if (it->first == region) {
            focusRegions_.erase(it);
            break;
        }
    }
    
    // Add new focus
    focusRegions_.push_back({region, strength});
    NLM_LOG_INFO("Norepinephrine: Focus set on Region " + std::to_string(region.index()) + " (strength: " + std::to_string(strength) + ")");
}

void Norepinephrine::reset() {
    pImpl->tonicNE = 0.0f;
    pImpl->phasicNE = 0.0f;
    pImpl->arousalBuffer = 0.0f;
    pImpl->attentionalLoad = 0.0f;
    pImpl->attentionalStability = 1.0f;
    pImpl->eventHistory.clear();
    focusRegions_.clear();
    setLevel(0.0f);
    
    // Reset attention allocation
    for (float& allocation : pImpl->attentionAllocation) {
        allocation = 0.0f;
    }
    
    NLM_LOG_INFO("Norepinephrine reset complete");
}

void Norepinephrine::updateFocusRegions(float dt) {
    // Adjust focus region strengths based on current NE level
    for (auto& regionFocus : focusRegions_) {
        float targetStrength = regionFocus.second * getLevel();
        float currentStrength = regionFocus.second;
        
        // Adapt focus strength toward target
        float adaptation = (targetStrength - currentStrength) * pImpl->adaptationRate * dt;
        regionFocus.second += adaptation;
        
        // Decay weak focus regions
        if (regionFocus.second < 0.05f) {
            regionFocus.second = 0.0f;
        }
    }
    
    // Remove empty focus regions
    focusRegions_.erase(
        std::remove_if(focusRegions_.begin(), focusRegions_.end(),
            [](const std::pair<RegionId, float>& rf) { return rf.second < 0.01f; }),
        focusRegions_.end()
    );
}

void Norepinephrine::updateAttentionAllocation(float dt) {
    // Allocate attention to neurons based on focus regions
    std::fill(pImpl->attentionAllocation.begin(), pImpl->attentionAllocation.end(), 0.0f);
    
    if (!brain_ || focusRegions_.empty()) return;
    
    for (const auto& regionFocus : focusRegions_) {
        RegionId region = regionFocus.first;
        float strength = regionFocus.second;
        
        // Get all neurons in this region
        NeuralRegion* regionPtr = brain_->getRegion(region);
        if (!regionPtr) continue;
        
        // Apply attention bias to all neurons in region
        for (const auto& pop : regionPtr->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                NeuronId neuronId = neuron->getId();
                if (neuronId.value < pImpl->attentionAllocation.size()) {
                    pImpl->attentionAllocation[neuronId.value] += strength * 0.1f;
                }
            }
        }
    }
    
    // Normalize attention allocation
    float maxAttention = 0.0f;
    for (float alloc : pImpl->attentionAllocation) {
        if (alloc > maxAttention) maxAttention = alloc;
    }
    
    if (maxAttention > 0.0f) {
        for (float& alloc : pImpl->attentionAllocation) {
            alloc /= maxAttention;
        }
    }
}

} // namespace nlm