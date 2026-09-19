// Neuromodulator: Abstract base for neuromodulatory signals
// Implements real neuromodulation effects for Phase 6 integration

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
    
    // For attention and memory modulation
    virtual void applyToAttention() = 0;
    virtual void applyToMemoryConsolidation() = 0;
    
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
    
    // Modulate attention and memory
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention and memory
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate impulsivity and attention
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Detection of new stimuli
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate plasticity for novel patterns
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
    // Novelty detection
    void signalNovelty(float noveltyScore);
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError: Error between predicted and actual outcomes
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate learning
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
    // Set error from prediction system
    void setError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Drive to explore and learn
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention and exploration
    void applyToAttention() override;
    void applyToMemoryConsolidation() override;
    
    // Curiosity-driven exploration
    void signalNovelty(float noveltyScore);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
