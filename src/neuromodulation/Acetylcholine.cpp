// Acetylcholine implementation - attention and memory modulation
#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryReconciliation;
    float corticalClearance;
    bool isActive;
    float workingMemoryTarget;
    float episodicTarget;
    float attentionWeight;
    
    Impl() 
        : level(0.0f)
        , baseline(0.02f)          // Low tonic baseline for attention
        , peak(1.0f)
        , decayRate(0.08f)         // Faster decay than dopamine
        , releaseRate(1.5f)        // Quick response to stimuli
        , attentionGain(0.8f)      // Strong attention modulation
        , memoryReconciliation(0.6f)  // Memory consolidation strength
        , corticalClearance(0.5f)     // Attention filtering
        , isActive(false)
        , workingMemoryTarget(0.7f)   // Target for working memory updating
        , episodicTarget(0.8f)       // Target for episodic memory
        , attentionWeight(1.0f)       // Current attention level
    {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh has specific effects on attention and memory:
    // - Enhances signal-to-noise ratio in cortical processing
    // - Facilitates memory encoding and consolidation
    // - Modulates working memory updating
    // - Promotes attention to salient stimuli
    
    float level = getLevel();
    
    if (level < 0.2f) {
        // Low ACh: poor attention, weak memory encoding
        return std::max(0.3f, 0.5f + level * 0.5f);
    } else if (level < 0.6f) {
        // Moderate ACh: optimal for attention and working memory
        return 0.7f + (level - 0.2f) * 0.45f;
    } else {
        // High ACh: strong attention but potential for over-focusing
        return std::min(1.5f, 1.0f + (level - 0.6f) * 1.5f);
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics:
    // - Builds up with stimulus salience
    // - Decays between stimuli
    // - Modulates cortical arousal and attention
    
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Modulate attention based on current level
    pImpl->attentionWeight = std::clamp(0.5f + (getLevel() - 0.1f) * pImpl->attentionGain, 
                                       0f, 2.0f);
    
    // Update memory targets based on cholinergic tone
    pImpl->workingMemoryTarget = std::clamp(0.5f + getLevel() * 0.3f, 0f, 1.0f);
    pImpl->episodicTarget = std::clamp(0.6f + getLevel() * 0.2f, 0f, 1.0f);
}

void Acetylcholine::signalStimulus(float salience, float predictionError) {
    // ACh responds to stimulus salience and prediction errors
    // High salience or positive prediction error triggers ACh release
    
    float response = salience * predictionError;
    pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
    
    // Focus attention on salient stimuli
    pImpl->isActive = (salience > 0.5f);
}

void Acetylcholine::applyToMemory(NeuralWorkingMemory* workingMemory, NeuralEpisodicMemory* episodicMemory) {
    if (!workingMemory || !episodicMemory) return;
    
    // ACh effects on working memory:
    // - Increases maintenance of relevant information
    // - Enhances updating with new information
    // - Reduces interference from irrelevant items
    
    float level = getLevel();
    
    // Modulate working memory parameters based on ACh level
    // Higher ACh = better working memory performance up to optimal level
    if (level > 0.3f && level < 0.8f) {
        // In the optimal range: enhance working memory stability
        workingMemory->setDecayRate(0.01f * (1.0f - (level - 0.3f) * 0.67f));
        
        // Strengthen memory traces
        workingMemory->strengthenMemory(level * 0.5f);
    }
    
    // ACh effects on episodic memory:
    // - Facilitates memory encoding
    // - Enhances consolidation during rest periods
    // - Promotes memory replay during sleep
    
    // Update consolidation targets
    float consolidationStrength = level * 0.8f;
    
    // Apply attention modulation to episodic memory
    // Similar implementation as in concept formation
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->isActive = false;
    pImpl->attentionWeight = 1.0f;
}

} // namespace nlm
