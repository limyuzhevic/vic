// NeuralAssociativeMemory: Forms associations between neural representations
// NOT simple pattern matching - creates Hebbian-like connections between concepts
//
// Key mechanisms:
// - Hebbian learning for association formation
// - Pattern completion through attractor dynamics
// - Bidirectional and unidirectional associations
// - Strength-based association weighting

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float initialStrength = 1.0f);

    // Get associated pattern
    // Returns associated pattern if strong association exists
    std::vector<float> getAssociation(const std::vector<float>& pattern) const;

    // Get all associations for a pattern
    std::vector<std::pair<std::vector<float>, float>> getAllAssociations(
        const std::vector<float>& pattern) const;

    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Get association strength
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;

    // Check if strong association exists
    bool hasStrongAssociation(const std::vector<float>& patternA,
                             const std::vector<float>& patternB,
                             float threshold = 0.5f) const;

    // Get association count
    size_t getAssociationCount() const { return associations_.size(); }

    // Pattern completion - recall associated patterns
    std::vector<float> patternCompletion(const std::vector<float>& partialPattern,
                                         size_t maxResults = 3) const;

    // Clear all associations
    void clear();

    // Get all patterns in memory
    std::vector<std::vector<float>> getAllPatterns() const;

private:
    struct Association {
        std::vector<float> patternA;
        std::vector<float> patternB;
        float strength;
        size_t accessCount;
        SimulationStep lastAccessed;
        
        Association(const std::vector<float>& a, const std::vector<float>& b, float s)
            : patternA(a), patternB(b), strength(s), accessCount(0), lastAccessed(0) {}
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float decayRate_;
    
    // All associations
    std::vector<Association> associations_;
    
    // All patterns for quick lookup
    std::vector<std::vector<float>> patterns_;
};

// NeuralPrediction: Temporal sequence prediction and forward models
// NOT simple prediction - learns forward dynamics through neural plasticity
//
// Key mechanisms:
// - Learned forward models for action consequences
// - Prediction error minimization
// - Temporal sequence completion
// - Confidence-based action selection

class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Observe action and outcome
    void observeActionOutcome(ActionType action,
                             const std::vector<float>& stateBefore,
                             const std::vector<float>& stateAfter,
                             float reward);

    // Predict next state
    std::vector<float> predictNextState(ActionType action,
                                        const std::vector<float>& currentState) const;

    // Get prediction confidence
    float getPredictionConfidence(ActionType action,
                                 const std::vector<float>& currentState) const;

    // Get prediction error
    float getPredictionError(ActionType action,
                            const std::vector<float>& predictedState,
                            const std::vector<float>& actualState) const;

    // Clear all predictions
    void clear();

    // Get number of learned predictions
    size_t getPredictionCount() const;

private:
    struct Prediction {
        ActionType action;
        std::vector<float> statePattern;
        std::vector<float> outcomePattern;
        float confidence;
        float error;
        size_t observationCount;
        
        Prediction(ActionType a, const std::vector<float>& before,
                  const std::vector<float>& after)
            : action(a), statePattern(before), outcomePattern(after),
              confidence(0.5f), error(0.0f), observationCount(0) {}
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<Prediction> predictions_;
};

} // namespace nlm