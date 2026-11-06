// Neuron group manager for AgentBrain
// Handles distribution of neurons to different groups
class AgentBrainNeuronManager {
public:
    AgentBrainNeuronManager() = default;
    
    // Add neurons to appropriate groups based on type
    void addNeurons(std::shared_ptr<Brain> brain);
    
    // Get motor neuron groups
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    // Get sensory neuron groups
    const std::vector<Neuron*>& getSensoryVision() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouch() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternal() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioception() const { return sensoryProprioception_; }
    const std::vector<Neuron*>& getInternalNeurons() const { return internalNeurons_; }
    
    // Get total counts
    size_t getMotorNeuronsCount() const {
        return motorForward_.size() + motorBackward_.size() + 
               motorTurnLeft_.size() + motorTurnRight_.size() +
               motorInteract_.size() + motorWait_.size();
    }
    
    size_t getSensoryNeuronsCount() const {
        return sensoryVision_.size() + sensoryTouch_.size() +
               sensoryInternal_.size() + sensoryProprioception_.size();
    }
    
    size_t getInternalNeuronsCount() const { return internalNeurons_.size(); }
    
    // Get all motor neuron groups
    std::vector<const std::vector<Neuron*>*> getAllMotorGroups() const {
        return { &motorForward_, &motorBackward_, &motorTurnLeft_, 
                 &motorTurnRight_, &motorInteract_, &motorWait_ };
    }
    
    // Get all sensory neuron groups
    std::vector<const std::vector<Neuron*>*> getAllSensoryGroups() const {
        return { &sensoryVision_, &sensoryTouch_, &sensoryInternal_,
                 &sensoryProprioception_ };
    }
    
private:
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> internalNeurons_;
};

// State validator for AgentBrain
// Validates internal state consistency
class AgentBrainStateValidator {
public:
    AgentBrainStateValidator() = default;
    
    // Validate brain pointer
    static bool validateBrainPointer(const std::shared_ptr<Brain>& brain);
    
    // Validate neuron groups (non-null for neurons that should exist)
    static bool validateNeuronGroups(const std::vector<Neuron*>& group);
    
    // Validate neuromodulation levels
    static bool validateNeuromodulationLevels(float dopamine, float novelty, 
                                              float curiosity, float predictionError);
    
    // Validate developmental state
    static bool validateDevelopmentalState(double age, float plasticity);
    
    // Validate configuration flags
    static bool validateConfigurationFlags(bool reward, bool plasticity, 
                                            bool development, bool curiosity);
    
    // Validate cognitive state
    static bool validateCognitiveState(const std::vector<float>& vision,
                                       const std::vector<float>& touch);
    
    // Check overall state consistency
    static bool validateOverallState(const AgentBrainNeuronManager& neuronManager,
                                     float dopamine, float novelty, float curiosity,
                                     float predictionError, double developmentalAge,
                                     float plasticityModifier,
                                     bool rewardModulationEnabled,
                                     bool structuralPlasticityEnabled,
                                     bool developmentEnabled, bool curiosityEnabled,
                                     const std::vector<float>& lastVision,
                                     const std::vector<float>& lastTouch,
                                     const std::string& lastText);
};

// Configuration manager for AgentBrain
// Manages AgentBrain-specific configuration
class AgentBrainConfigManager {
public:
    AgentBrainConfigManager() = default;
    
    // Get default configuration values
    static void setDefaultConfig(std::shared_ptr<Config> config);
    
    // Validate configuration for AgentBrain
    static bool validateAgentBrainConfig(const Config& config);
    
    // Apply configuration overrides
    static void applyConfigOverrides(std::shared_ptr<Config> config);
    
    // Get configuration summary
    static std::string getConfigSummary(const Config& config);
    
private:
    // Default configuration values
    static constexpr float DEFAULT_SENSORY_NOVELTY_DECAY = 0.99f;
    static constexpr float DEFAULT_MIN_DOSE = -1.0f;
    static constexpr float DEFAULT_MAX_DOSE = 1.0f;
    static constexpr double DEFAULT_DEVELOPMENTAL_STAGE_DURATION = 900.0;
};