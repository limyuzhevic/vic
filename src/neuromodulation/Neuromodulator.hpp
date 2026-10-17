namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
// Implemented: Full dopamine system with reward prediction error
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
    
    // Get current effects
    float getTotalSignal() const;
    const std::vector<float>& getRecentRewards() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implemented: Enhances attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory effects
    void enhanceAttention();
    void enhanceMemoryConsolidation();
    
    // Get current effects
    float getAttentionBoost() const;
    float getMemoryConsolidationBoost() const;
    
    // Signal processing
    float getTotalSignal() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implemented: Full NE system with locus coeruleus effects
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    void signalArousal(float intensity, float contextRelevance);
    void signalVigilance(float taskDemandingness, float novelty);
    
    // Get current effects
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getPhasicResponse() const;
    
    // Signal processing
    float getTotalSignal() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implemented: Full serotonin system with social effects
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social and mood effects
    void signalPositiveSocialExperience(float reward, float socialCohesion);
    void signalSocialConflict(float isolation, float punishment);
    
    // Get current effects
    float getMoodLevel() const;
    float getSocialEngagementLevel() const;
    float getImpulsivityLevel() const;
    
    // Signal processing
    float getTotalSignal() const;
    const std::vector<float>& getSocialHistory() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
