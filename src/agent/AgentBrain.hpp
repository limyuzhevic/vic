// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with enhanced functionality
class AgentBrain {
public:
    // Enhanced constructor with optional configuration
    AgentBrain(std::shared_ptr<Brain> brain, bool enableAdvancedFeatures = false);
    ~AgentBrain();
    
    // Standard initialization
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward (original method for compatibility)
    void applyRewardModulation(float reward, float predictedReward);
    
    // Enhanced neuromodulation with multiple signals
    void applyMultiNeuromodulation(const std::vector<float>& signals);
    
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
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration for power users
    void setActionSelectionMode(int mode); // 0=direct, 1=planned, 2=exploratory
    void setCuriosityThreshold(float threshold);
    void setDevelopmentSpeed(float speed);
    void enableSleepCycles(bool enable) { enableSleepCycles_ = enable; }
    void enableMemoryReplay(bool enable) { enableMemoryReplay_ = enable; }
    
    // Advanced query methods
    float getActionQuality(int actionIndex) const;
    bool wasPlanSuccessful() const;
    size_t getObservationCount() const { return observationCount_; }
    float getAverageReward() const { return averageReward_; }
    
    // Memory-related methods for advanced users
    void recordExperience(const std::vector<float>& state, ActionType action, 
                         const std::vector<float>& nextState, float reward);
    std::vector<std::vector<float>> retrieveSimilarExperiences(
        const std::vector<float>& state, size_t maxResults) const;
    
    // Callback support for advanced users
    void setRewardCallback(std::function<void(float, float)> callback) {
        rewardCallback_ = callback;
    }
    
    void setActionCallback(std::function<MotorCommand()> callback) {
        actionCallback_ = callback;
    }
    
    // Performance monitoring
    double getAverageProcessingTime() const { return averageProcessingTime_; }
    void resetPerformanceCounters() {
        processingTimes_.clear();
        averageProcessingTime_ = 0.0;
    }
    
    // Debug and development methods
    void logAgentState() const;
    void exportNeuralState(std::vector<float>& state) const;
    void importNeuralState(const std::vector<float>& state);
    
    // Safe methods with error handling
    Brain* getBrainSafe() { 
        return brain_ ? brain_.get() : nullptr; 
    }
    
    SimpleWorld* getWorldSafe() { 
        return world_ ? world_.get() : nullptr; 
    }
    
    // Action type for experience recording
    enum class ActionType {
        MoveForward,
        MoveBackward,
        TurnLeft,
        TurnRight,
        LookLeft,
        LookRight,
        Interact,
        Wait
    };
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
// Error handling for safety
    void handleError(const std::string& errorMessage) const;
    
    // Performance tracking
    void updatePerformanceCounter() {
        auto now = std::chrono::high_resolution_clock::now();
        processingTimes_.push_back(
            std::chrono::duration<double>(now - lastTime_).total_seconds()
        );
        if (!processingTimes_.empty()) {
            double total = 0.0;
            for (double t : processingTimes_) total += t;
            averageProcessingTime_ = total / processingTimes_.size();
        }
        lastTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Random generator for advanced features
    std::mt19937 randomDevice_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
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
    
    // Enhanced neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Advanced state for power users
    int actionSelectionMode_;  // 0=direct, 1=planned, 2=exploratory
    float curiosityThreshold_;
    float developmentSpeed_;
    bool enableSleepCycles_;
    bool enableMemoryReplay_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced memory system
    std::vector<std::tuple<std::vector<float>, ActionType, std::vector<float>, float>> experienceBuffer_;
    size_t observationCount_;
    float averageReward_;
    
    // Callbacks for advanced customization
    std::function<void(float, float)> rewardCallback_;
    std::function<MotorCommand()> actionCallback_;
    
    // Performance monitoring
    std::vector<double> processingTimes_;
    double averageProcessingTime_;
    std::chrono::high_resolution_clock::time_point lastTime_;
    
    // Safety state
    bool initialized_;
    bool brainNullChecked_;
    std::string lastError_;
    
    // Prevent memory leaks and improve safety
    void clearNeuronVectors();
    void cleanupResources();
};

} // namespace nlm
