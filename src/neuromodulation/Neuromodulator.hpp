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
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get baseline level
    virtual float getBaselineLevel() const = 0;
    
    // Get peak level
    virtual float getPeakLevel() const = 0;
    
    // Get decay rate
    virtual float getDecayRate() const = 0;
    
    // Get release rate
    virtual float getReleaseRate() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
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
    
    // Get baseline level
    float getBaselineLevel() const override;
    
    // Get peak level
    float getPeakLevel() const override;
    
    // Get decay rate
    float getDecayRate() const override;
    
    // Get release rate
    float getReleaseRate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get baseline level
    float getBaselineLevel() const override;
    
    // Get peak level
    float getPeakLevel() const override;
    
    // Get decay rate
    float getDecayRate() const override;
    
    // Get release rate
    float getReleaseRate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get baseline level
    float getBaselineLevel() const override;
    
    // Get peak level
    float getPeakLevel() const override;
    
    // Get decay rate
    float getDecayRate() const override;
    
    // Get release rate
    float getReleaseRate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get baseline level
    float getBaselineLevel() const override;
    
    // Get peak level
    float getPeakLevel() const override;
    
    // Get decay rate
    float getDecayRate() const override;
    
    // Get release rate
    float getReleaseRate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
