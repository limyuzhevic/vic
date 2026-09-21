// Prediction system: predicts future sensory states and consequences

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Record sensory state for learning
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get brain reference
    class Brain* getBrain() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    class Brain* parentBrain;
};
