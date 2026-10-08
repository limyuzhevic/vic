#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float currentLevel;
    float arousal;
    float vigilance;
    float attentionBias;
    float alertness;
    float plasticityFactor;
    
    // Neural dynamics
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    
    // Time constants for dynamics
    float arousalDecayRate;
    float vigilanceDecayRate;
    float attentionModulation;
    
    // Stress and activation
    float stressLevel;
    float activationEnergy;
    
    // Temporal dynamics
    TimestepDuration timeSinceLastEvent;
    
    Impl() 
        : currentLevel(0.3f)
        , arousal(0.3f)
        , vigilance(0.5f)
        , attentionBias(1.0f)
        , alertness(0.5f)
        , plasticityFactor(1.0f)
        , arousalDecayRate(0.1f)
        , vigilanceDecayRate(0.05f)
        , attentionModulation(1.0f)
        , stressLevel(0.0f)
        , activationEnergy(0.0f)
        , timeSinceLastEvent(0.0f)
    {
    }
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {
}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity based on arousal and attention
    // Moderate arousal enhances plasticity, high arousal reduces it
    // (inverted U-curve relationship)
    
    // Arousal-dependent modulation (inverted U)
    float arousalOptimum = 0.5f;
    float arousalModulation = std::exp(-std::pow((pImpl->arousal - arousalOptimum) / 0.2f, 2.0f));
    
    // Vigilance-dependent enhancement
    float vigilanceModulation = 1.0f + 0.3f * pImpl->vigilance;
    
    // Alertness improves efficiency
    float alertnessModulation = 1.0f + 0.5f * pImpl->alertness;
    
    // Combined effect
    float factor = 1.0f * arousalModulation * vigilanceModulation * alertnessModulation;
    
    // Clamp to reasonable range
    return std::clamp(factor, 0.1f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->timeSinceLastEvent += dt;
    
    // Natural decay toward baseline
    pImpl->arousal = std::max(0.1f, pImpl->arousal - pImpl->arousalDecayRate * dt);
    pImpl->vigilance = std::max(0.1f, pImpl->vigilance - pImpl->vigilanceDecayRate * dt * 2.0f);
    
    // Alertness recovery
    pImpl->alertness = std::min(1.0f, pImpl->alertness + 0.05f * dt);
    
    // Decay stress and activation energy
    pImpl->stressLevel *= (1.0f - 0.001f * dt);
    pImpl->activationEnergy *= (1.0f - 0.0005f * dt);
    
    // Add to history
    pImpl->arousalHistory.push_back(pImpl->arousal);
    pImpl->vigilanceHistory.push_back(pImpl->vigilance);
    
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin());
    }
}

void Norepinephrine::signalArousalChange(float arousalDelta) {
    // Apply arousal change
    pImpl->arousal = std::clamp(pImpl->arousal + arousalDelta, 0.1f, 2.0f);
    
    // Arousal affects vigilance and alertness
    if (arousalDelta > 0.0f) {
        // Increased arousal increases vigilance
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + arousalDelta * 0.5f);
        pImpl->alertness = std::min(1.0f, pImpl->alertness + arousalDelta * 0.3f);
    } else {
        // Decreased arousal reduces vigilance
        pImpl->vigilance = std::max(0.1f, pImpl->vigilance + arousalDelta * 0.3f);
    }
    
    // Arousal affects attention bias
    pImpl->attentionBias = 1.0f + 0.5f * (pImpl->arousal - 0.5f);
    
    NLM_LOG_INFO(std::string("Norepinephrine: Arousal change to ") + std::to_string(pImpl->arousal));
}

void Norepinephrine::signalAttentionNeed(float attentionDemand) {
    // Attention demand increases norepinephrine
    pImpl->arousal = std::min(2.0f, pImpl->arousal + attentionDemand * 0.5f);
    
    // Increases vigilance for sustained attention
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + attentionDemand * 0.3f);
    
    // Modulates attention bias
    pImpl->attentionBias = 1.0f + attentionDemand * 0.4f;
    
    NLM_LOG_INFO(std::string("Norepinephrine: Attention demand = ") + std::to_string(attentionDemand));
}

void Norepinephrine::signalStressResponse(float stressLevel) {
    // Stress affects norepinephrine system
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Stress increases arousal
    pImpl->arousal = std::min(2.0f, pImpl->arousal + stressLevel * 0.8f);
    
    // Stress increases vigilance (fight-or-flight)
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + stressLevel * 0.6f);
    
    // Stress increases alertness
    pImpl->alertness = std::min(1.0f, pImpl->alertness + stressLevel * 0.5f);
    
    // Stress reduces efficiency, affecting plasticity
    pImpl->plasticityFactor *= (1.0f - stressLevel * 0.3f);
    
    NLM_LOG_INFO(std::string("Norepinephrine: Stress level = ") + std::to_string(stressLevel));
}

void Norepinephrine::signalMemorySalience(float memorySalience) {
    // Salient memories trigger norepinephrine release
    if (memorySalience > 0.0f) {
        pImpl->arousal = std::min(2.0f, pImpl->arousal + memorySalience * 0.4f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + memorySalience * 0.3f);
        
        // Enhances memory consolidation through plasticity effects
        pImpl->plasticityFactor = std::min(3.0f, pImpl->plasticityFactor + memorySalience * 0.2f);
        
        NLM_LOG_INFO(std::string("Norepinephrine: Salient memory = ") + std::to_string(memorySalience));
    }
}

} // namespace nlm
