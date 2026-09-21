// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
//
// Phase 6 Integration: 
// - Connects sensory processing to working memory
// - Integrates higher cognition (planning, concept formation)
// - Links prediction system with action selection
// - Uses neuromodulation for adaptive behavior

// Sensory input types for AgentBrain
enum class SensoryType {
    Vision = 0,
    Touch = 1,
    Internal = 2,
    Proprioception = 3
};

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
    
    // Cognitive integration
    bool isCognitiveEnabled() const { return cognitiveEnabled_; }
    void enableCognitiveProcessing(bool enable) { cognitiveEnabled_ = enable; }
    
    // Get cognitive statistics
    float getCognitiveActivity() const { return cognitiveActive_; }
    float getConceptActivation() const { return conceptActivation_; }
    float getPlanProgress() const { return planProgress_; }
    float getAttentionFocus() const { return attentionFocus_; }
    float getCognitiveNovelty() const { return cognitiveNovelty_; }
    
    // Get current self state for cognitive systems
    std::vector<float> getCurrentSelfState() const;
    
    // Process cognitive input and update internal state
    void processCognitiveInput(const std::vector<float>& input);
    
private:
    // Helper methods for cognitive state
    float getMotorCommandActivity(MotorCommand cmd) const;
    float getSensoryActivity(SensoryType type) const;
    
    // Update cognitive processing
    void updateHigherCognition(double timestep);
    
    std::shared_ptr<Brain> brain_;
    
    // BRAIN INTEGRATION SYSTEMS
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<PredictionSystem> predictionSystem_;
    std::shared_ptr<DevelopmentSystem> developmentSystem_;
    
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
    bool cognitiveEnabled_;
    
    // COGNITIVE PROCESSING STATE
    float cognitiveActive_;           // Overall cognitive activity level
    float conceptActivation_;         // Concept formation activity
    float planProgress_;              // Planning progress
    float attentionFocus_;            // Attention focus level
    
    // Cognitive novelty and tracking
    float cognitiveNovelty_;
    float cognitiveNoveltyDecay_;
    std::vector<float> previousCognitiveState_;
    
    // Cognitive integration points (signals to cognition systems)
    std::vector<std::function<void(const std::vector<float>&)>> cognitiveIntegration_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Reward prediction error for cognitive learning
    std::vector<float> rewardPredictionErrors_;
};
