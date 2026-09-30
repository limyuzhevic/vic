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
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper method to calculate activity in neuron groups
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    
    // Configuration
    void setMotorDistributionGroups(size_t groups) { motorDistributionGroups_ = groups; }
    void setSensoryDistributionGroups(size_t groups) { sensoryDistributionGroups_ = groups; }
    
    // Sensory and motor sizing configuration
    void setSensoryInputSize(size_t size) { sensoryInputSize_ = size; }
    void setMotorOutputSize(size_t size) { motorOutputSize_ = size; }
    void setVisionWidth(size_t width) { visionWidth_ = width; }
    void setVisionHeight(size_t height) { visionHeight_ = height; }
    void setTouchInputSize(size_t size) { touchInputSize_ = size; }
    void setInternalInputSize(size_t size) { internalInputSize_ = size; }
    void setProprioceptionInputSize(size_t size) { proprioceptionInputSize_ = size; }
    
    // Get configuration values
    size_t getSensoryInputSize() const { return sensoryInputSize_; }
    size_t getMotorOutputSize() const { return motorOutputSize_; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getTouchInputSize() const { return touchInputSize_; }
    size_t getInternalInputSize() const { return internalInputSize_; }
    size_t getProprioceptionInputSize() const { return proprioceptionInputSize_; }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Curiosity configuration
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationThreshold(float threshold) { explorationThreshold_ = threshold; }
    void setExplorationChanceMultiplier(float multiplier) { explorationChanceMultiplier_ = multiplier; }
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setCuriosityCalculationCoefficients(float noveltyCoeff, float errorCoeff) {
        curiosityNoveltyCoefficient_ = noveltyCoeff;
        curiosityErrorCoefficient_ = errorCoeff;
    }
    
    // Get configuration values
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationThreshold() const { return explorationThreshold_; }
    float getExplorationChanceMultiplier() const { return explorationChanceMultiplier_; }
    float getActivityThreshold() const { return activityThreshold_; }
    float getCuriosityNoveltyCoefficient() const { return curiosityNoveltyCoefficient_; }
    float getCuriosityErrorCoefficient() const { return curiosityErrorCoefficient_; }
    
    std::shared_ptr<Brain> brain_;
    
    // Configuration parameters
    size_t motorDistributionGroups_;
    size_t sensoryDistributionGroups_;
    size_t motorDistributionCounter_;
    size_t sensoryDistributionCounter_;
    
    // Sensory and motor sizing configuration
    size_t sensoryInputSize_;
    size_t motorOutputSize_;
    size_t visionWidth_;
    size_t visionHeight_;
    size_t touchInputSize_;
    size_t internalInputSize_;
    size_t proprioceptionInputSize_;
    
    // Curiosity configuration parameters
    float curiosityThreshold_;
    float explorationThreshold_;
    float explorationChanceMultiplier_;
    float activityThreshold_;
    float curiosityNoveltyCoefficient_;
    float curiosityErrorCoefficient_;
    
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
