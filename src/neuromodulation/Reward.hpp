// Reward signal for reinforcement learning
// Combines reward prediction and prediction error

class Reward {
public:
    Reward();
    ~Reward();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get reward signal name
    const char* getName() const;
    
    // Get current reward level
    float getLevel() const;
    void setLevel(float level);
    
    // Get reward-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Update reward state
    void update(TimestepDuration dt);
    
    // Signal reward with prediction error
    void signal(float reward, float predictionError);
    
    // Get prediction error component
    float getPredictionErrorSignal() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm