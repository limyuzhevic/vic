// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
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

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->attentionLevel; }
    void update(TimestepDuration dt) override;
    
    // Attention-related functions
    float getAttentionLevel() const { return pImpl->attentionLevel; }
    float getMemoryConsolidationStrength() const { return pImpl->memoryConsolidation; }
    
    // Set parameters
    void setAttentionGain(float gain) { pImpl->attentionGain = gain; }
    void setMemoryConsolidation(float strength) { pImpl->memoryConsolidation = strength; }
    void setReleaseRate(float rate) { pImpl->releaseRate = rate; }
    void setDecayRate(float rate) { pImpl->decayRate = rate; }
    
    // Signal-based modulation
    void signalAttention(float salience, const std::vector<float>& sensoryPattern);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->arousalLevel; }
    void update(TimestepDuration dt) override;
    
    // Arousal-related functions
    float getArousalLevel() const { return pImpl->arousalLevel; }
    float getSignalToNoiseRatio() const { return pImpl->signalToNoise; }
    
    // Set parameters
    void setArousalGain(float gain) { pImpl->arousalGain = gain; }
    void setSignalToNoise(float snr) { pImpl->signalToNoise = snr; }
    void setThreshold(float threshold) { pImpl->arousalThreshold = threshold; }
    void setDecayRate(float rate) { pImpl->decayRate = rate; }
    
    // Signal-based modulation
    void signalNovelty(float novelty, float predictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->moodLevel; }
    void update(TimestepDuration dt) override;
    
    // Mood-related functions
    float getMoodLevel() const { return pImpl->moodLevel; }
    float getImpulseControl() const { return pImpl->impulseControl; }
    float getSocialBehaviorModulation() const { return pImpl->socialModulation; }
    
    // Set parameters
    void setMoodGain(float gain) { pImpl->moodGain = gain; }
    void setImpulseControl(float control) { pImpl->impulseControl = control; }
    void setSocialModulation(float mod) { pImpl->socialModulation = mod; }
    void setDecayRate(float rate) { pImpl->decayRate = rate; }
    
    // Signal-based modulation
    void signalSocialContext(float socialCohesion, float frustration);
    
private:
    struct Impl;
    Impl* pImpl;
};