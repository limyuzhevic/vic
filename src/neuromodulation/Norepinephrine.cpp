#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float clearanceRate;
    float synthesisRate;
    float arousal;
    float vigilance;
    float attention;
    SimulationStep lastReleased;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.2f),
             clearanceRate(0.3f), synthesisRate(0.03f), arousal(0.5f), vigilance(0.5f),
             attention(0.5f), lastReleased(0), stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

Norepinephrine::~Norepinephrine() {
    NLM_LOG_INFO("Norepinephrine neuromodulator destroyed");
}

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
    // Enhanced norepinephrine: sharpens attention and accelerates learning
    float basePlasticity = 0.6f;
    float arousalFactor = 0.5f + 0.5f * pImpl->arousal;
    float vigilanceFactor = 0.7f + 0.6f * pImpl->vigilance;
    float attentionFactor = 0.8f + 0.4f * pImpl->attention;
    float stressFactor = 1.0f + 0.5f * pImpl->stressLevel;
    
    return basePlasticity * arousalFactor * vigilanceFactor * attentionFactor * stressFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Dynamic norepinephrine dynamics with stress response
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Natural clearance
    pImpl->level -= pImpl->clearanceRate * timeFactor;
    
    // Stress-induced release
    if (pImpl->stressLevel > 0.5f) {
        pImpl->level += pImpl->releaseRate * (pImpl->stressLevel - 0.5f) * timeFactor;
    }
    
    // Continuous synthesis
    pImpl->level += pImpl->synthesisRate * timeFactor;
    
    // Ensure level stays in valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update related state variables
    pImpl->arousal = std::min(1.0f, pImpl->arousal + 0.02f * timeFactor);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.01f * timeFactor);
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.03f * timeFactor);
    
    // Stress recovery
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.01f * timeFactor);
    }
}

void Norepinephrine::signalArousalChange(float arousalDelta) {
    pImpl->arousal += arousalDelta;
    pImpl->arousal = std::clamp(pImpl->arousal, 0.0f, 1.0f);
    
    // High arousal improves attention
    if (pImpl->arousal > 0.8f) {
        pImpl->attention = std::min(1.0f, pImpl->attention + 0.15f);
    }
}

void Norepinephrine::signalVigilanceChange(float vigilanceDelta) {
    pImpl->vigilance += vigilanceDelta;
    pImpl->vigilance = std::clamp(pImpl->vigilance, 0.0f, 1.0f);
}

void Norepinephrine::signalAttentionChange(float attentionDelta) {
    pImpl->attention += attentionDelta;
    pImpl->attention = std::clamp(pImpl->attention, 0.0f, 1.0f);
    
    // High attention improves plasticity for salient events
    if (pImpl->attention > 0.7f) {
        setLevel(getLevel() + 0.1f);
    }
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getAttention() const {
    return pImpl->attention;
}

void Norepinephrine::enhanceAlertness(float alertDelta) {
    // Norepinephrine specifically enhances alertness and attention
    pImpl->arousal = std::min(1.0f, pImpl->arousal + 0.25f * alertDelta);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.2f * alertDelta);
    pImpl->attention = std::min(1.0f, pImpl->attention + 0.3f * alertDelta);
    
    // Stress response to alerting events
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + 0.1f * alertDelta);
    
    // Boost plasticity for learning under alert conditions
    setLevel(getLevel() + 0.2f * alertDelta);
}

void Norepinephrine::enhanceMemoryEncoding(float memoryDelta) {
    // Norepinephrine specifically enhances memory encoding and consolidation
    pImpl->level = std::min(1.0f, pImpl->level + 0.3f * memoryDelta);
    pImpl->arousal = std::min(1.0f, pImpl->arousal + 0.15f * memoryDelta);
    
    // Improved memory trace through enhanced plasticity
    if (memoryDelta > 0.5f) {
        signalArousalChange(0.2f);
        setLevel(getLevel() + 0.15f);
    }
}

} // namespace nlm