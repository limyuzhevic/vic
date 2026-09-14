// Forward declarations for internal classes
struct EpisodicMemoryEntry {
    double timestamp;
    std::vector<float> visionPattern;
    MotorCommand action;
    float reward;
    float importance;
    std::string context;
};

struct LearnedSequence {
    MotorCommand action;
    float frequency;
    std::string context;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrainConfig> config);
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
    
    // Get energy level
    float getEnergyLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
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
    
    // Configuration access
    void setConfig(std::shared_ptr<AgentBrainConfig> config) { config_ = config; }
    std::shared_ptr<AgentBrainConfig> getConfig() const { return config_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration (enhanced version)
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced neuron organization
    void organizeNeuronsImproved();
    void distributeMotorNeuronsBiased(const std::vector<Neuron*>& motorNeurons);
    void distributeSensoryNeuronsHierarchical(const std::vector<Neuron*>& sensoryNeurons);
    
    // Enhanced curiosity computation
    float computeCuriosityEnhanced(const std::vector<float>& vision, float predictionError);
    float computeNoveltyEnhanced(const std::vector<float>& vision);
    
    // Enhanced development system
    void updateDevelopmentEnhanced(double timestep);
    
    // Learning and memory integration
    void learnFromExplorationHistory();
    void updateMemoryIntegration(const std::vector<float>& vision, const MotorCommand& action, float reward);
    
    // Enhanced neuromodulation with stress and energy factors
    void applyRewardModulationEnhanced(float reward, float predictedReward);
    
    // Helper functions
    const char* motorCommandToStringEnhanced(MotorCommand cmd);
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrainConfig> config_;
    
    // Random number generator for exploration
    std::mt19937 randomEngine_;
    
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
    
    // Stress and energy state
    float stressLevel_;
    float energyLevel_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Exploration history for learning
    std::vector<MotorCommand> explorationHistory_;
    
    // Learned sequences and patterns
    std::vector<LearnedSequence> learnedSequences_;
};

} // namespace nlm
