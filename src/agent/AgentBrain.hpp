namespace nlm {

// AgentBrain - Agent-brain interface
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // Subsystem enable/disable
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Statistics
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Text processing
    std::vector<size_t> encodeText(const std::string& text);
    
    // Social learning
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision, const std::vector<float>& afterVision);
    
    // State management
    void reset();
    void logStatus() const;
    
    // Get reference to the brain
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
    
    // Agent state
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Neuromodulation state
    float dopamineLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    
    // Development
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
    
    // Sensory processing
    std::vector<float> predictedVision_;
    std::vector<float> lastVision_;
    std::vector<float> lastTouch_;
    
    // Motor command distribution
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron distribution
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Internal neuron group
    std::vector<Neuron*> internalNeurons_;
    
    // Winner-take-all state
    int lastWinnerGroup_ = -1;
    float lastWinnerMargin_ = 0.0f;
    int consecutiveWinnerCount_ = 0;
    int wtaPulseCount_ = 0;
    
    // Novelty and curiosity
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float sensoryNoveltyDecay_ = 0.99f;
    
    // Thought trace
    std::string thoughtPercept_;
    bool hasThoughtOutcome_ = false;
    float thoughtReward_ = 0.0f;
    
    // Planning
    MotorCommand thoughtPlanned_;
    bool hasThoughtPlanned_ = false;
    
    // Last decoded action
    MotorCommand lastDecoded_;
    bool hasLastDecoded_ = false;
    
    // Diversity counters
    int stepsSinceTurn_ = 0;
    int repeatCount_ = 1;
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int conceptBiasCount_ = 0;
    int selfBiasCount_ = 0;
    int socialBiasCount_ = 0;
    int plannerOverrideCount_ = 0;
    
    // Self-model
    bool selfModelReady_ = false;
    SelfModel selfModel_;
    
    // Social learning
    bool socialReady_ = false;
    SocialLearning socialLearning_;
    
    // State for self-model recording
    MotorCommand lastDecoded_;
    bool hasLastDecoded_ = false;
};

} // namespace nlm
