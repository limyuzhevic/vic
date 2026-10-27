#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal, vigilance, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine() : pImpl(new Impl) {}
    ~Norepinephrine() override = default;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl ? pImpl->level : 0.0f; }
    void setLevel(float level) override { if (pImpl) pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    
    float getPlasticityFactor() const override { return pImpl ? pImpl->plasticityFactor : 1.0f; }
    void update(TimestepDuration dt) override { if (pImpl) pImpl->update(dt); }
    
    // Arousal modulation
    void setArousalLevel(float level) { if (pImpl) pImpl->arousalLevel = level; }
    float getArousalLevel() const { return pImpl ? pImpl->arousalLevel : 0.0f; }
    
    // Vigilance control
    void setVigilanceLevel(float level) { if (pImpl) pImpl->vigilanceLevel = level; }
    float getVigilanceLevel() const { return pImpl ? pImpl->vigilanceLevel : 0.0f; }
    
    // Stress response
    void triggerStressResponse() { if (pImpl) pImpl->triggerStressResponse(); }
    bool isStressResponseActive() const { return pImpl ? pImpl->stressResponseActive : false; }
    
    // Alertness
    void setAlertnessLevel(float level) { if (pImpl) pImpl->alertnessLevel = level; }
    float getAlertnessLevel() const { return pImpl ? pImpl->alertnessLevel : 0.0f; }
    
    // Learning under stress
    void stressEnhancesMemory(float factor) { if (pImpl) pImpl->stressEnhancesMemory(factor); }
    float getStressMemoryEnhancement() const { return pImpl ? pImpl->stressMemoryEnhancement : 0.0f; }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        
        // Arousal parameters
        float arousalLevel;
        float arousalThreshold;
        float arousalGain;
        
        // Vigilance parameters
        float vigilanceLevel;
        float alertnessLevel;
        float detectionThreshold;
        
        // Stress parameters
        float stressResponseActive;
        float stressLevel;
        float cortisolLevel;
        float stressMemoryEnhancement;
        
        // Learning parameters
        float stressLearningRate;
        float stressFocus;
        
        Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.02f), releaseRate(2.5f),
                 plasticityFactor(1.0f), arousalLevel(0.5f), arousalThreshold(0.3f), arousalGain(1.0f),
                 vigilanceLevel(0.5f), alertnessLevel(0.5f), detectionThreshold(0.2f),
                 stressResponseActive(0.0f), stressLevel(0.0f), cortisolLevel(0.0f),
                 stressMemoryEnhancement(0.0f), stressLearningRate(0.5f), stressFocus(0.5f) {}
        
        void update(TimestepDuration dt) {
            // Decay norepinephrine level
            level = std::max(baseline, level - decayRate * static_cast<float>(dt));
            
            // Stress response decay
            if (stressResponseActive > 0.0f) {
                stressResponseActive = std::max(0.0f, stressResponseActive - 0.1f * static_cast<float>(dt));
                stressLevel = std::max(0.0f, stressLevel - 0.05f * static_cast<float>(dt));
            }
            
            // Alertness decay
            alertnessLevel = std::max(0.1f, alertnessLevel - 0.05f * static_cast<float>(dt));
        }
        
        void triggerStressResponse() {
            stressResponseActive = 1.0f;
            stressLevel = 1.0f;
            cortisolLevel = 1.0f;
        }
        
        void stressEnhancesMemory(float factor) {
            stressMemoryEnhancement = factor;
            stressFocus = factor * 0.5f;
            stressLearningRate = factor * 0.8f;
        }
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
