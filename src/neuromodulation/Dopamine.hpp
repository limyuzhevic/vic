namespace nlm {

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
    
    // Reward accumulation and decay
    void addToHistory(float reward);
    const std::vector<float>& getRewardHistory() const;
    void clearHistory();
    
    // Phasic vs tonic modulation
    float getPhasicModulation() const;
    float getTonicModulation() const;
    void setModulationMode(bool phasic);
    
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
    
    // Attention modulation
    void modulateAttention(float attentionLevel);
    float getAttentionLevel() const;
    
    // Memory consolidation
    void consolidateMemory(float consolidationStrength);
    float getMemoryConsolidation() const;
    
    // Cholinergic neuron activity
    void recordCholinergicActivity(float activity);
    const std::vector<float>& getActivityHistory() const;
    
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
    
    // Arousal modulation
    void setArousal(float arousal);
    float getArousal() const;
    
    // Vigilance modulation
    void setVigilance(float vigilance);
    float getVigilance() const;
    
    // Stress response
    void triggerStressResponse(float intensity);
    float getStressLevel() const;
    
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
    
    // Mood modulation
    void modulateMood(float mood);
    float getMood() const;
    
    // Impulsivity modulation
    void setImpulsivity(float impulsivity);
    float getImpulsivity() const;
    
    // Social behavior modulation
    void setSocialBehavior(float sociality);
    float getSocialBehavior() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
