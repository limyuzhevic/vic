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
