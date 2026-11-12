// Neuromodulator: Abstract base for neuromodulatory signals
// 
// Abstract base class defining the interface for neuromodulatory signals
// that modulate neural plasticity and dynamics based on behavioral context
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (normalized 0.0-1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns multiplicative factor to apply to learning rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on time elapsed
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation with burst dynamics and plasticity modulation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling with biologically realistic burst dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get current burst state for debugging
    bool isInBurst() const;
    float getBurstDuration() const;
    
    // Get dopamine dynamics
    float getDALevel() const;
    float getDABaseline() const;
    void setDABaseline(float baseline);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Modulates attention, learning, and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention control
    float getAttentionGain() const;
    void setAttentionGain(float gain);
    
    // Memory modulation
    void triggerMemoryConsolidation();
    
    // Get acetylcholine dynamics
    float getAChLevel() const;
    float getAChBaseline() const;
    void setAChBaseline(float baseline);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Controls alertness, attention, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance control
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    void triggerAlertness(float intensity);
    
    // Get norepinephrine dynamics
    float getNELevel() const;
    float getNEBaseline() const;
    void setNEBaseline(float baseline);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, impulse control, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and impulse regulation
    float getMoodLevel() const;
    float getImpulseControl() const;
    void signalSocialFeedback(float socialValue);
    
    // Get serotonin dynamics
    float get5HTLevel() const;
    float get5HTBaseline() const;
    void set5HTBaseline(float baseline);
    
private:
    struct Impl;
    Impl* pImpl;
};
