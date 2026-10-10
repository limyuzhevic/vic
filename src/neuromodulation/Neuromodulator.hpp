// Neuromodulator: Abstract base for neuromodulatory signals
// Integrates with neural computation for motivation, attention, and learning

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to neural processes
    // TODO PHASE 2: Implement real modulation effects
    virtual float getPlasticityFactor() const = 0;
    virtual float getAttentionModulator() const { return 1.0f; }  // Default no effect
    virtual float getMemoryModulator() const { return 1.0f; }     // Default no effect
    virtual float getLearningRateModulator() const { return 1.0f; }  // Default no effect
    
    // Update neuromodulator state based on neural activity and context
    virtual void update(TimestepDuration dt) = 0;
    
    // Integration with brain systems
    virtual void onSpiking(Brain* brain, NeuronId neuronId, float current) = 0;
    virtual void onMemoryConsolidation(Brain* brain, float consolidationSignal) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error and reinforcement learning
// High dopamine = better than expected -> strengthen useful pathways
// Low dopamine = worse than expected -> weaken harmful pathways
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void recordOutcome(float outcomeValue);
    
    // Integration with learning systems
    void onLearning(float learningSignal);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine (ACh): Attention and memory consolidation
// Modulates attention, strengthens memory encoding, supports working memory
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    void onSpiking(class Brain* brain, NeuronId neuronId, float current) override;
    void onMemoryConsolidation(class Brain* brain, float consolidationSignal) override;
    
    // Specific cholinergic functions
    void enhanceAttention(class Brain* brain, const std::vector<NeuronId>& neurons);
    void facilitateMemoryEncoding(class Brain* brain, const std::vector<NeuronId>& neurons);
    void modulateThalamicReticular nucleus(class Brain* brain);
};

// Norepinephrine (NE): Arousal, vigilance, and stress response
// Increases neural gain, enhances signal-to-noise ratio, modulates attention
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    void onSpiking(class Brain* brain, NeuronId neuronId, float current) override;
    void onMemoryConsolidation(class Brain* brain, float consolidationSignal) override;
    
    // Specific noradrenergic functions
    void increaseAlertness(class Brain* brain);
    void enhanceLTM consolidation(class Brain* brain);
    void modulateMonoaminergic modulation(class Brain* brain);
};

// Serotonin (5-HT): Mood regulation, impulsivity, and social behavior
// Modulates overall excitability, regulates aggression, supports learning patterns
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    void onSpiking(class Brain* brain, NeuronId neuronId, float current) override;
    void onMemoryConsolidation(class Brain* brain, float consolidationSignal) override;
    
    // Specific serotonergic functions
    void modulateAggression(class Brain* brain);
    void supportPatternLearning(class Brain* brain);
    void regulateSleepWakeCycle(class Brain* brain);
};

// Novelty detection: Detects and signals novel or surprising stimuli
// Integrates with curiosity and prediction error systems
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override { return "Novelty"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    void onSpiking(class Brain* brain, NeuronId neuronId, float current) override;
    void onMemoryConsolidation(class Brain* brain, float consolidationSignal) override;
    
    // Novelty detection mechanisms
    void computeNovelty(Brain* brain, const std::vector<float>& sensoryInput);
    void updateNoveltyRate(class Brain* brain, float learningRate);
    
    // Get novelty statistics
    float getNoveltyRate() const;
    float getPredictionSurprise() const;
};

// Reward signal: Immediate reward prediction for reinforcement learning
// Simple reward signal that integrates with dopamine system
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override { return "Reward"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    float getLearningRateModulator() const override;
    void update(TimestepDuration dt) override;
    void onSpiking(class Brain* brain, NeuronId neuronId, float current) override;
    void onMemoryConsolidation(class Brain* brain, float consolidationSignal) override;
    
    // Reward signaling
    void signalImmediateReward(float reward);
    void signalDelayedReward(float reward, float delay);
    void updateRewardHistory(float reward);
};
