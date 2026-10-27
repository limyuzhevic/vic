#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention modulation and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine() : pImpl(new Impl) {}
    ~Acetylcholine() override = default;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl ? pImpl->level : 0.0f; }
    void setLevel(float level) override { if (pImpl) pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    
    float getPlasticityFactor() const override { return pImpl ? pImpl->plasticityFactor : 1.0f; }
    void update(TimestepDuration dt) override { if (pImpl) pImpl->update(dt); }
    
    // Attention modulation
    void setAttentionLevel(float level) { if (pImpl) pImpl->attentionLevel = level; }
    float getAttentionLevel() const { return pImpl ? pImpl->attentionLevel : 0.0f; }
    
    // Memory encoding
    void triggerMemoryEncoding() { if (pImpl) pImpl->triggerMemoryEncoding(); }
    bool isMemoryEncodingEnabled() const { return pImpl ? pImpl->memoryEncodingEnabled : false; }
    
    // Novelty detection
    void setNoveltyLevel(float level) { if (pImpl) pImpl->noveltyLevel = level; }
    float getNoveltyLevel() const { return pImpl ? pImpl->noveltyLevel : 0.0f; }
    
    // Learning enhancement
    void enhanceLearning(float factor) { if (pImpl) pImpl->enhanceLearning(factor); }
    float getLearningEnhancement() const { return pImpl ? pImpl->learningEnhancement : 0.0f; }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        
        // Attention parameters
        float attentionLevel;
        float attentionGain;
        
        // Memory parameters
        float memoryEncodingEnabled;
        float memoryConsolidation;
        
        // Novelty parameters
        float noveltyLevel;
        float noveltyThreshold;
        
        // Learning parameters
        float learningEnhancement;
        float cholinergicLearning;
        
        Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.03f), releaseRate(3.0f),
                 plasticityFactor(1.0f), attentionLevel(0.5f), attentionGain(1.0f),
                 memoryEncodingEnabled(0.0f), memoryConsolidation(1.0f),
                 noveltyLevel(0.0f), noveltyThreshold(0.3f),
                 learningEnhancement(0.0f), cholinergicLearning(1.0f) {}
        
        void update(TimestepDuration dt) {
            // Decay acetylcholine level
            level = std::max(baseline, level - decayRate * static_cast<float>(dt));
        }
        
        void triggerMemoryEncoding() {
            // Strong acetylcholine triggers memory encoding
            memoryEncodingEnabled = 1.0f;
        }
        
        void enhanceLearning(float factor) {
            cholinergicLearning = factor;
            learningEnhancement = factor * 0.5f;
        }
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
