// Forward declarations
class Brain;
class NeuralPrediction;
class ActionConsequencePredictor;
class PredictionErrorSignal;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Update prediction system with current sensory state
    void updateWithSensoryInput(const SensoryInput& currentInput);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get prediction neurons for integration
    const std::vector<NeuronId>& getPredictionNeurons() const;
    
    // Get sequence neurons for temporal predictions
    const std::vector<NeuronId>& getSequenceNeurons() const;
    
    // Get integrated prediction system for brain integration
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
    // Get action consequence predictor for planning
    ActionConsequencePredictor* getActionConsequencePredictor() { return actionConsequencePredictor_.get(); }
    
    // Get prediction error signal for neuromodulation
    PredictionErrorSignal* getPredictionErrorSignal() { return predictionErrorSignal_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Brain reference for integration
    Brain* brain_;
    
    // Integrated prediction components
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor_;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal_;
};

} // namespace nlm
