// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory input and inject into brain
    bool processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward = 0.0f);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enablePrediction(bool enable) { predictionEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isPredictionEnabled() const { return predictionEnabled_; }
    
    // Convenience methods for common operations
    bool simulateEpisode(SimpleWorld& world, size_t numSteps, bool computeReward = true);
    std::map<std::string, float> getAgentState() const;
    std::string getAgentStatus() const;
    
    // Development management
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Learning management
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }
    
    // Performance monitoring
    size_t getStepCount() const { return stepCount_; }
    double getTotalSimulationTime() const { return totalSimulationTime_; }
    
    // Error handling
    bool hasErrors() const { return !errorMessages_.empty(); }
    std::string getErrors() const { return errorMessages_; }
    void clearErrors() { errorMessages_.clear(); }
    
    // Statistics and metrics
    struct AgentMetrics {
        float averageFiringRate;
        float totalReward;
        size_t totalSteps;
        size_t activeNeurons;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float neuromodulationLevel;
        
        AgentMetrics() : averageFiringRate(0.0f), totalReward(0.0f), totalSteps(0),
                       activeNeurons(0), curiosityLevel(0.0f), noveltyLevel(0.0f),
                       predictionError(0.0f), neuromodulationLevel(0.0f) {}
    };
    
    AgentMetrics getMetrics() const;
    std::string metricsToString() const;
    
private:
    // Core functionality
    void processSensoryInput(const SensoryPercept& percept, bool& validInput);
    MotorCommand decodeMotorCommandWithCuriosity(MotorCommand defaultCmd);
    void applyRewardModulationInternal(float reward, float predictedReward);
    
    // Error handling
    void reportError(const std::string& error);
    void reportWarning(const std::string& warning);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool predictionEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance tracking
    size_t stepCount_;
    double totalSimulationTime_;
    float accumulatedReward_;
    
    // Error tracking
    std::string errorMessages_;
    std::vector<std::string> warningMessages_;
    
    // Learning parameters
    float learningRate_;
    
    // Previous motor commands for change detection
    MotorCommand previousMotorCommand_;
    
    // Statistical tracking
    mutable std::vector<float> firingRateHistory_;
    mutable std::vector<float> rewardHistory_;
};
