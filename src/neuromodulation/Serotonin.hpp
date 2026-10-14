#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin {
public:
    const char* getName() const { return "5-HT"; }
    float getLevel() const { return level; }
    void setLevel(float lvl) { level = std::clamp(lvl, 0.0f, 1.0f); }
    float getPlasticityFactor() const { return 1.0f; }
    void update(double dt) {}
    void signalMood(float mood) { level = std::min(1.0f, level + mood * 0.3f); }
    void signalSocial(const std::vector<float>& socialInputs) {
        float socialSum = 0.0f;
        for (float input : socialInputs) socialSum += input;
        level = std::min(1.0f, level + socialSum * 0.3f / socialInputs.size());
    }
    void signalImpulsivity(float impulsivity) { level = std::min(1.0f, level + impulsivity * 0.2f); }
    void signalImpulseControl(float control) { level = std::max(0.0f, level - control * 0.6f); }
    
    float getSocialModulation() const { return level * 0.7f; }
    float getImpulseControl() const { return 1.0f - level * 0.8f; }
    
private:
    float level = 0.0f;
};

} // namespace nlm