#include <string>
#include <map>

namespace nlm {

// Exploration strategy enumeration
enum class ExplorationStrategy {
    Random,           // Random exploration
    Novelty,         // Explore novel stimuli
    Uncertainty,     // Explore uncertain predictions
    Balanced         // Balance exploration and exploitation
};

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
    
    // Advanced configuration methods
    void setRewardDecay(float decay) { rewardDecay_ = decay; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setNoveltySensitivity(float sensitivity) { noveltySensitivity_ = sensitivity; }
    void setDevelopmentSpeed(float speed) { developmentSpeed_ = speed; }
    void setPlasticityRange(float min, float max) { 
        minPlasticity_ = min; 
        maxPlasticity_ = max; 
    }
    
    // Get advanced configuration
    float getRewardDecay() const { return rewardDecay_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltySensitivity() const { return noveltySensitivity_; }
    float getDevelopmentSpeed() const { return developmentSpeed_; }
    void getPlasticityRange(float& min, float& max) const { min = minPlasticity_; max = maxPlasticity_; }
    
    // Agent statistics and debugging
    float getLearningProgress() const;
    float getExperienceScore() const;
    size_t getTotalExperienceTicks() const;
    void resetExperienceStats();
    
    // Advanced motor control
    MotorCommand getBestMotorCommand() const;
    float getMotorCommandConfidence(MotorCommand cmd) const;
    
    // Enhanced curiosity and exploration
    void setExplorationStrategy(ExplorationStrategy strategy) { explorationStrategy_ = strategy; }
    void enableAdaptiveExploration(bool enable) { adaptiveExplorationEnabled_ = enable; }
    bool isAdaptiveExplorationEnabled() const { return adaptiveExplorationEnabled_; }
    
    // Memory and learning tracking
    void enableMemoryTracking(bool enable) { memoryTrackingEnabled_ = enable; }
    void enableLearningMetrics(bool enable) { learningMetricsEnabled_ = enable; }
    void recordExperience(float reward, float predictionError, float novelty);
    std::string getExperienceReport() const;
    
    // Batch processing for advanced users
    void processSensoryInputs(const std::vector<SensoryPercept>& percepts);
    std::vector<MotorCommand> decodeMotorCommands(size_t steps);
    std::vector<ActionResult> applyRewardModulations(const std::vector<std::pair<float, float>>& rewards);
    
    // Advanced configuration
    struct AgentConfig {
        // Plasticity and learning
        float plasticityMin = 0.1f;
        float plasticityMax = 2.0f;
        float rewardDecay = 0.95f;
        float learningRate = 0.01f;
        
        // Curiosity and exploration
        float curiosityThreshold = 0.3f;
        float explorationRate = 0.2f;
        float noveltySensitivity = 2.0f;
        ExplorationStrategy explorationStrategy = ExplorationStrategy::Balanced;
        
        // Development
        float developmentSpeed = 1.0f;
        float adultPlasticity = 0.2f;
        float criticalPeriodEnd = 300.0;
        
        // Novelty and memory
        float sensoryNoveltyDecay = 0.99f;
        size_t noveltyHistorySize = 100;
        
        // Performance
        size_t motorNeuronGroupSize = 50;
        bool enableAdaptiveExploration = true;
        bool enableMemoryTracking = true;
        bool enableLearningMetrics = true;
    };
    
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
    // Debug and monitoring
    struct AgentMetrics {
        float totalExperienceScore = 0.0f;
        size_t totalExperienceTicks = 0;
        float averagePredictionError = 0.0f;
        float explorationRatio = 0.0f;
        size_t memoryUsage = 0;
        double lastUpdateTime = 0.0;
    };
    
    const AgentMetrics& getMetrics() const { return metrics_; }
    std::string getDebugInfo() const;
    
private:
    // Exploration strategy enumeration
    // (already defined above)
    
    // Advanced configuration methods
    // (private members would be here)
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
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
    
    // Advanced configuration
    float rewardDecay_;
    float curiosityThreshold_;
    float explorationRate_;
    float noveltySensitivity_;
    float developmentSpeed_;
    float minPlasticity_;
    float maxPlasticity_;
    ExplorationStrategy explorationStrategy_;
    bool adaptiveExplorationEnabled_;
    bool memoryTrackingEnabled_;
    bool learningMetricsEnabled_;
    
    // Experience tracking
    float totalExperienceScore_;
    size_t totalExperienceTicks_;
    std::vector<float> experienceRewards_;
    std::vector<float> experienceErrors_;
    std::vector<float> experienceNovelties_;
    std::map<size_t, float> motorCommandConfidences_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Configuration and metrics
    AgentConfig config_;
    mutable AgentMetrics metrics_;
    double lastExperienceRecordTime_;
};

} // namespace nlm
