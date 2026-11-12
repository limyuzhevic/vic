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
