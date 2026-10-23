#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float vigilance;
    float arousal;
    float workingMemoryEnhancement;
    float attentionShifting;
    
    Impl() : level(0.0f), vigilance(0.0f), arousal(0.0f), workingMemoryEnhancement(0.0f), attentionShifting(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 3.0f);
    
    // NE affects vigilance and arousal
    pImpl->vigilance = pImpl->level * 0.4f;
    pImpl->arousal = pImpl->level * 0.6f;
    pImpl->workingMemoryEnhancement = 1.0f + pImpl->level * 0.2f;
    pImpl->attentionShifting = pImpl->level * 0.3f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE increases plasticity for learning and attention
    return 1.0f + pImpl->level * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Slow decay of NE signal
    pImpl->level *= 0.93f;
    
    // Apply arousal and vigilance effects
    if (pImpl->level > 0.1f) {
        // NE enhances alertness and working memory
        // This would modulate neural gain in prefrontal and memory areas
        NLM_LOG_INFO("Norepinephrine level: " + std::to_string(pImpl->level) + 
                    ", vigilance: " + std::to_string(pImpl->vigilance) + 
                    ", arousal: " + std::to_string(pImpl->arousal));
    }
}

void Norepinephrine::signalAlertness(float intensity) {
    // NE signals increased alertness in response to novel or important stimuli
    setLevel(getLevel() + intensity * 0.15f);
}

void Norepinephrine::signalWorkingMemoryUpdate(const std::vector<NeuronId>& memoryNeurons, float strength) {
    // NE enhances working memory updating and consolidation
    if (strength > 0.0f) {
        setLevel(getLevel() + strength * 0.03f);
        
        // Enhance working memory traces
        for (NeuronId neuron : memoryNeurons) {
            NLM_LOG_INFO("NE enhancing working memory in neuron " + 
                        std::to_string(neuron.getId()) + " with strength " + std::to_string(strength));
        }
    }
}

void Norepinephrine::signalAttentionShifting(float shiftStrength) {
    // NE helps shift attention between tasks
    setLevel(getLevel() + shiftStrength * 0.08f);
}

} // namespace nlm