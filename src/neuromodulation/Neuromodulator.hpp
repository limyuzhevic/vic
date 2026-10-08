class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (normalized 0-1)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get concentration in nM
    virtual float getConcentration() const = 0;
    
    // Get baseline concentration in nM
    virtual float getBaselineConcentration() const = 0;
    
    // Get homeostatic set point in nM
    virtual float getHomeostaticSetPoint() const = 0;
    
    // Check if homeostatic regulation is active
    virtual bool isHomeostaticallyRegulated() const = 0;
    
    // Reset to baseline (for homeostatic regulation)
    virtual void resetToBaseline() = 0;
    
protected:
    Neuromodulator() = default;
};