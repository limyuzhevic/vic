// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration constants for improved maintainability
    static constexpr float ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float CURIOSITY_LEVEL_THRESHOLD = 0.3f;
    static constexpr float SENSORY_NOVELTY_DECAY = 0.99f;
    static constexpr float EXPECTED_REWARD_ALPHA = 0.95f;
    static constexpr float EXPECTED_REWARD_BETA = 0.05f;
    static constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    static constexpr float ELIGIBILITY_DECAY_RATE = 0.1f;
    static constexpr float STDP_LTP_BASE = 0.01f;
    static constexpr float STDP_LTD_BASE = 0.012f;
    static constexpr float PLASTICITY_BASE = 0.5f;
    static constexpr float NOVELTY_WEIGHT_FACTOR = 2.0f;
    static constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
    static constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;
    static constexpr float PLASMODICITY_ENHANCEMENT = 0.5f;
    static constexpr float ACTIVITY_SCALE_FACTOR_VISION = 5.0f;
    static constexpr float ACTIVITY_SCALE_FACTOR_TOUCH = 8.0f;
    static constexpr float ACTIVITY_SCALE_FACTOR_INTERNAL = 5.0f;
    static constexpr float ACTIVITY_SCALE_FACTOR_PROPRIO = 3.0f;
    static constexpr float INTERNAL_CENTER_OFFSET = 1.0f;
    static constexpr float PROPRIO_CENTER_OFFSET = 1.0f;
    
    static constexpr size_t MOTOR_GROUP_COUNT = 6;
    static constexpr size_t SENSORY_GROUP_COUNT = 4;
    
    static constexpr double DEVELOPMENT_STAGE_1 = 60.0;
    static constexpr double DEVELOPMENT_STAGE_2 = 300.0;
    static constexpr double DEVELOPMENT_STAGE_3 = 900.0;
    static constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    static constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    static constexpr float ACTIVITY_THRESHOLD_DEVELOPMENT_INITIAL = 1.0f;
    static constexpr float ACTIVITY_THRESHOLD_DEVELOPMENT_CRITICAL = 0.8f;
    static constexpr float ACTIVITY_THRESHOLD_DEVELOPMENT_MATURATION = 0.5f;
    static constexpr float ACTIVITY_THRESHOLD_DEVELOPMENT_ADULT = 0.2f;

    // Public interface
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
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
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    // Helper methods for neuron group distribution
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons);
    MotorCommand getMotorCommandFromIndex(size_t index) const;
    
    // Initialization methods
    void initializeMotorGroups();
    void initializeSensoryGroups();
    void initializeDevelopmentStage();
    void resetState();
    
    // Validation methods
    bool validateInputRanges() const;
    void safeLogInitialization() const;

private:
    // Core implementation
    Brain* getBrainUnsafe() const { return brain_.get(); }
    bool hasBrain() const { return brain_ != nullptr; }
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Neuromodulation and state management
    void updateDopamineSignal();
    void updateCuriosityLevel();
    void applyPredictionError(float reward, float predictedReward);
    
    // Development and plasticity management
    void updateDevelopmentStages();
    void updateStructuralPlasticity();
    
    // Sensory processing helpers
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& intern);
    void processProprioceptionInput(const std::vector<float>& proprio);
    
    // Novelty detection
    void computeSensoryNovelty(const std::vector<float>& vision);

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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    
};

} // namespace nlm