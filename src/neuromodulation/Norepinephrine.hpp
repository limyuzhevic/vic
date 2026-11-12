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
