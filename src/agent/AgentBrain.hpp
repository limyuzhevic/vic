// Constants for better code maintainability
namespace AgentBrainConstants {
    const size_t DEFAULT_MOTOR_GROUPS = 6;  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
    const size_t DEFAULT_SENSORY_GROUPS = 4;  // Vision, Touch, Internal, Proprioception
    const float DEFAULT_NOVELTY_DECAY = 0.99f;
    const float DEFAULT_PLATSCITY_MODIFIER = 1.0f;
    const float DEFAULT_DEVELOPMENTAL_AGE_FACTOR = 0.5f;
    const float MAX_DOPAMINE_LEVEL = 1.0f;
    const float MIN_DOPAMINE_LEVEL = -1.0f;
    const float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    const float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    const size_t MAX_RANDOM_CHOICE = 7;
    const float DEFAULT_REWARD_EXPONENT = 0.95f;
    const float DEFAULT_NEW_REWARD_WEIGHT = 0.05f;
    const size_t SENSORY_GROUP_MULTIPLIER_VISION = 256;
    const size_t SENSORY_GROUP_MULTIPLIER_TOUCH = 8;
    const size_t SENSORY_GROUP_MULTIPLIER_INTERNAL = 4;
    const size_t SENSORY_GROUP_MULTIPLIER_PROPRIOCEPTION = 6;
    const float VISION_SCALE_FACTOR = 5.0f;
    const float TOUCH_SCALE_FACTOR = 8.0f;
    const float INTERNAL_SCALE_FACTOR = 2.0f;
    const float INTERNAL_AMPLITUDE_FACTOR = 5.0f;
    const float PROPRIOCEPTION_SCALE_FACTOR = 2.0f;
    const float PROPRIOCEPTION_AMPLITUDE_FACTOR = 3.0f;
    const float MINIMUM_ACTIVITY_THRESHOLD = 0.5f;
    const float HIGH_CURIOSITY_THRESHOLD = 0.5f;
    const float ELIGIBILITY_THRESHOLD = 0.001f;
    const float ELIGIBILITY_DECAY_RATE = 0.1f;
    const float BASE_PLASTICITY_FACTOR = 0.5f;
    const float DOPAMINE_PLASTICITY_SENSITIVITY = 0.5f;
    const float MIN_PLASTICITY_FACTOR = 0.1f;
    const float MAX_PLASTICITY_FACTOR = 2.0f;
    const float STDP_LTP_BASE_WEIGHT = 0.01f;
    const float STDP_LTD_BASE_WEIGHT = 0.012f;
    const float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    const float PRUNING_BASE_RATE = 0.00001f;
    const float PRUNING_SENSITIVITY = 2.0f;
    const double INITIAL_DEVELOPMENT_END = 60.0;
    const double CRITICAL_PERIOD_END = 300.0;
    const double MATURATION_END = 900.0;
    const float HIGH_PLASTICITY_FACTOR = 1.0f;
    const float CRITICAL_PERIOD_PLASTICITY_FACTOR = 0.8f;
    const float MATURATION_PLASTICITY_FACTOR = 0.5f;
    const float ADULT_PLASTICITY_FACTOR = 0.2f;
}

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    [[nodiscard]] size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    [[nodiscard]] size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    [[nodiscard]] MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    [[nodiscard]] float getNeuromodulationLevel() const;
    
    // Get curiosity level
    [[nodiscard]] float getCuriosityLevel() const;
    
    // Get novelty level
    [[nodiscard]] float getNoveltyLevel() const;
    
    // Get prediction error
    [[nodiscard]] float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    [[nodiscard]] Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    [[nodiscard]] bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    [[nodiscard]] bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    [[nodiscard]] bool isDevelopmentEnabled() const { return developmentEnabled_; }
    [[nodiscard]] bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Ensure brain is valid before operations
    [[nodiscard]] bool isValid() const { return brain_ != nullptr; }
    
    // Check if any neural groups are populated
    [[nodiscard]] bool hasNeuralGroups() const;
    
    // Const methods for better code safety
    [[nodiscard]] const SimpleWorld& getCurrentWorld() const { return currentWorld_; }
    [[nodiscard]] double getCurrentDevelopmentAge() const { return developmentalAge_; }
    [[nodiscard]] float getCurrentPlasticityModifier() const { return plasticityModifier_; }
    
    // Additional utility methods for advanced usage
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    void setDevelopmentEnabled(bool enabled, double duration = 60.0);
    
    // Diagnostic and debugging methods
    void logNeuralGroupStats() const;
    [[nodiscard]] std::string getNeuralGroupSummary() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    [[nodiscard]] MotorCommand decodeFromMotorNeurons() const;
    
    // Motor command selection with curiosity/exploration
    [[nodiscard]] MotorCommand selectWithCuriosity(MotorCommand defaultCmd) const;
    
    std::shared_ptr<Brain> brain_;
    const SimpleWorld* currentWorld_;  // Pointer to current world for reference
    
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
    
    // Helper functions for improved organization
    void resetNeuromodulationState();
    void resetDevelopmentState();
    void clearPreviousVision();
    void initializeNeuralGroups();
    void clearNeuralGroups();
    
    static void distributeBrainNeurons(AgentBrain& agent, const SimpleWorld& world);
    static void distributeMotorNeurons(std::vector<Neuron*>& target,
                                     const std::vector<Neuron*>& source);
    static void distributeSensoryNeurons(std::vector<Neuron*>& target,
                                         const std::vector<Neuron*>& source);
    
    // Validate vector before access
    [[nodiscard]] bool safeVectorAccess(const std::vector<Neuron*>& vec, size_t index) const;
    
    // Calculate neuron activity safely
    [[nodiscard]] static float calculateNeuronActivity(const std::vector<Neuron*>& neurons);
    
    // Processing methods
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void computeNovelty(const std::vector<float>& vision);
    
    // Plasticity and neuromodulation helpers
    void applyPlasticityModifications();
    void modulatePlasticityBasedOnDopamine();
    
    // Memory management helpers
    void optimizeMemoryUsage();
    void reserveMemoryForGrowth();
    
    // Error handling and logging
    void handleInvalidState(const std::string& operation) const;
    void logInitializationStatus() const;
};

} // namespace nlm
