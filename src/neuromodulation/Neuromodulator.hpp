#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced for Phase 6 integration

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Enhanced for Phase 6
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced for Phase 6 integration
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Dopamine for Phase 6
class DopamineEnhanced : public Neuromodulator {
public:
    DopamineEnhanced();
    ~DopamineEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced reward signaling with adaptive learning
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error for learning
    float getPredictionError() const;
    
    // Get recent experience for exploration
    float getRecentExperience() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced for Phase 6 integration
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Enhanced Acetylcholine for Phase 6
class AcetylcholineEnhanced : public Neuromodulator {
public:
    AcetylcholineEnhanced();
    ~AcetylcholineEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced attention modulation
    void signalAttention(float attentionLevel);
    void modulateMemoryConsolidation(float consolidationFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced for Phase 6 integration
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Enhanced Norepinephrine for Phase 6
class NorepinephrineEnhanced : public Neuromodulator {
public:
    NorepinephrineEnhanced();
    ~NorepinephrineEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced arousal modulation
    void signalArousal(float arousalLevel);
    void modulateVigilance(float vigilanceFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced for Phase 6 integration
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Enhanced Serotonin for Phase 6
class SerotoninEnhanced : public Neuromodulator {
public:
    SerotoninEnhanced();
    ~SerotoninEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced mood and social modulation
    void signalMood(float moodLevel);
    void modulateSocialBehavior(float socialFactor);
    void modulateImpulseControl(float impulseControlFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation
// Enhanced for Phase 6 integration
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty, float predictionError, float dtTime);
    
    // Enhanced curiosity dynamics
    void signalNovelty(float noveltyLevel);
    void signalPredictionError(float predictionError);
    float getNoveltyLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Curiosity for Phase 6
class CuriosityEnhanced : public Neuromodulator {
public:
    CuriosityEnhanced();
    ~CuriosityEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty, float predictionError, float dtTime);
    
    // Enhanced exploration dynamics
    void signalExploration(float explorationLevel);
    void adaptBasedOnExperience(float experienceLevel);
    void modulateExplorationThreshold(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection: Novelty detection
// Enhanced for Phase 6 integration
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced novelty detection
    void signalNovelty(float noveltyLevel);
    void updatePatternHistory(const std::vector<float>& pattern);
    float getPatternNovelty(const std::vector<float>& pattern) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Novelty for Phase 6
class NoveltyEnhanced : public Neuromodulator {
public:
    NoveltyEnhanced();
    ~NoveltyEnhanced() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced novelty detection with pattern matching
    void signalNovelty(float noveltyLevel);
    void updatePatternDatabase(const std::vector<float>& pattern);
    float detectPatternNovelty(const std::vector<float>& pattern) const;
    void clearPatternDatabase();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
