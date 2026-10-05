namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Constants for neuron grouping
    static constexpr size_t MAX_MOTOR_GROUPS = 6;
    static constexpr size_t MAX_SENSORY_GROUPS = 4;
    static constexpr size_t MOTOR_GROUP_SIZE = 6;
    static constexpr size_t SENSORY_GROUP_SIZE = 4;
    
    // Create neuron groups configuration structure
    struct NeuronGroupConfig {
        std::vector<Neuron*> neurons;
        std::string name;
        NeuronType type;
    };
    
public:
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
    
    // Enhanced initialization methods
    void initializeMotorGroups();
    void initializeSensoryGroups();
    bool validateBrainStructure() const;
    void distributeNeuronsToGroups();
    
    // Configuration with defaults
    void configureNeuronGroups(size_t motorGroups = MAX_MOTOR_GROUPS, 
                              size_t sensoryGroups = MAX_SENSORY_GROUPS);
    
    // Get group statistics for debugging
    size_t getMotorGroupCount() const { return motorForward_.size() + motorBackward_.size() + 
                                               motorTurnLeft_.size() + motorTurnRight_.size() + 
                                               motorInteract_.size() + motorWait_.size(); }
    size_t getSensoryGroupCount() const { return sensoryVision_.size() + sensoryTouch_.size() + 
                                              sensoryInternal_.size() + sensoryProprioception_.size(); }
    
    // Statistical helper methods
    float calculateNeuronActivity(const std::vector<Neuron*>& neurons) const;
    size_t countActiveNeurons(const std::vector<Neuron*>& neurons) const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Neuron group configuration
    size_t motorGroups_;
    size_t sensoryGroups_;
    
    // Motor neuron groups (distributed)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (distributed)
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
