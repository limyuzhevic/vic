#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include <algorithm>

namespace nlm {

// Implement Dopamine's applyPlasticityModulation method
void Dopamine::applyPlasticityModulation(const Neuromodulator& source) const {
    // Apply plasticity modulation based on dopamine interaction with other neuromodulators
    // Dopamine signaling interacts with other neuromodulators to regulate plasticity
    
    // Get other neuromodulator's level for interaction
    float otherLevel = source.getLevel();
    
    if (otherLevel > 0.0f) {
        // Dopamine modulates plasticity based on other signals
        // For example, serotonin can modulate dopamine release
        float interaction = std::min(1.0f, otherLevel * 0.5f);
        
        // Adjust dopamine's effect on plasticity
        pImpl->baseline = std::max(0.0f, pImpl->baseline + interaction * 0.05f);
        
        // Modulate release rate based on interaction
        pImpl->releaseRate = std::min(2.0f, pImpl->releaseRate + interaction * 0.1f);
    }
}

// Implement Acetylcholine's applyPlasticityModulation method (Neuromodulator.hpp)
void Acetylcholine::applyPlasticityModulation(const Neuromodulator& source) const {
    // Implement ACh effects on plasticity
    // ACh enhances attention and modulates memory consolidation
    
    // ACh modulates plasticity based on attention state
    float level = getLevel();
    if (level > 0.0f) {
        // Acetylcholine enhances synaptic strengthening during attention
        // This would normally affect LTP/LTD balance
        pImpl->baseline = std::min(1.0f, pImpl->baseline + level * 0.1f);
    }
}

// Implement Norepinephrine's applyPlasticityModulation method (Neuromodulator.hpp)
void Norepinephrine::applyPlasticityModulation(const Neuromodulator& source) const {
    // Implement NE effects on plasticity
    // NE increases arousal and modulates working memory updating
    
    // NE modulates plasticity based on arousal state
    float level = getLevel();
    if (level > 0.0f) {
        // Norepinephrine enhances plasticity for salient events
        // Increases learning rate for novel or important information
        pImpl->baseline = std::min(1.0f, pImpl->baseline + level * 0.08f);
    }
}

// Implement Serotonin's applyPlasticityModulation method (Neuromodulator.hpp)
void Serotonin::applyPlasticityModulation(const Neuromodulator& source) const {
    // Implement 5-HT effects on plasticity
    // 5-HT modulates mood and impulsivity, affecting learning
    
    // Serotonin modulates plasticity based on mood state
    float level = getLevel();
    if (level > 0.0f) {
        // Serotonin typically reduces impulsivity and modulates learning rate
        // Complex effects: high 5-HT can either increase or decrease plasticity
        // depending on receptor type and brain region
        pImpl->baseline = std::min(1.0f, pImpl->baseline + level * 0.06f);
    }
}

} // namespace nlm
