// Learning rate adaptation algorithm types
enum class AdaptationAlgorithm {
    Fixed,              // No adaptation - use fixed learning rate
    RMSprop,            // Root mean square propagation adaptation
    Adam,               // Adaptive moment estimation
    Homeostatic         // Homeostatic regulation based on stability
    // Can add more algorithms later
};

// LearningRateAdapter class implements synaptic learning rate adaptation
// based on learning history, stability measures, and performance metrics
class LearningRateAdapter {
public:
    // Constructor
    LearningRateAdapter();
    
    // Destructor
    ~LearningRateAdapter();
    
    // Initialize adapter with configuration parameters
    void initialize(
        bool enabled,
        AdaptationAlgorithm algorithm,
        float adaptationRate,
        float stabilityThreshold,
        float maxLearningRate,
        float minLearningRate,
        float baseLearningRate
    );
    
    // Adapt learning rate based on synaptic history and stability
    void update(SynapseId synapseId, 
                const std::vector<SynapticWeight>& weightHistory,
                const std::vector<float>& stabilityHistory,
                const std::vector<float>& performanceHistory,
                TimestepDuration dt);
    
    // Get current learning rate for a synapse
    float getLearningRate(SynapseId synapseId) const;
    
    // Get learning rate adaptation statistics
    float getAverageWeightChange(SynapseId synapseId) const;
    float getStabilityMeasure(SynapseId synapseId) const;
    float getPerformanceMetric(SynapseId synapseId) const;
    
    // Reset learning history for a synapse
    void reset(SynapseId synapseId);
    
    // Check if learning rate adaptation is enabled
    bool isEnabled() const;
    
    // Clear all learning history
    void clearHistory();
    
    // Get algorithm type
    AdaptationAlgorithm getAlgorithm() const;
    
    // Set algorithm type
    void setAlgorithm(AdaptationAlgorithm algorithm);
    
    // Get configuration parameters
    float getAdaptationRate() const;
    float getStabilityThreshold() const;
    float getMaxLearningRate() const;
    float getMinLearningRate() const;
    float getBaseLearningRate() const;
    
    // Set configuration parameters
    void setAdaptationRate(float rate);
    void setStabilityThreshold(float threshold);
    void setMaxLearningRate(float maxRate);
    void setMinLearningRate(float minRate);
    void setBaseLearningRate(float baseRate);
    
    // Get algorithm name as string
    const char* getAlgorithmName() const;
    
    // Get adapter status
    std::string getStatus() const;

private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
