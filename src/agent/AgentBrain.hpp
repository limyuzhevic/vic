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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

// Low-level motor commands that the brain can produce
// These are innate muscle-like signals, not semantic actions
enum class MotorCommand {
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};

// Helper function to get random motor command (excludes Wait)
inline MotorCommand getRandomMotorCommand() {
    static const MotorCommand commands[] = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::LookLeft,
        MotorCommand::LookRight,
        MotorCommand::Interact
    };
    return commands[rand() % (sizeof(commands) / sizeof(commands[0]))];
}

// Forward declarations for factory functions
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
std::shared_ptr<SimpleWorld> createSimpleWorld();
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);

} // namespace nlm

// Advanced factory functions
std::shared_ptr<Brain> createBrainWithSeed(uint64_t seed, std::shared_ptr<Config> config);

// Batch operations for efficiency
void batchStep(std::vector<std::shared_ptr<AgentBrain>>& agents,
               std::vector<std::shared_ptr<SimpleWorld>>& worlds,
               int numSteps);

// Neural network statistics
struct BrainStats {
    size_t totalNeurons;
    size_t activeNeurons;
    size_t totalSynapses;
    float avgFiringRate;
    float excitationInhibitionRatio;
    float totalSpikeCount;
};

BrainStats getBrainStats(std::shared_ptr<Brain> brain);

// Configuration validation
void validateBrainConfig(const std::shared_ptr<Config>& config);

// Performance monitoring
struct PerformanceMonitor {
    double startTime;
    double lastStepTime;
    size_t stepsCompleted;
    double totalTime;
    
    PerformanceMonitor();
    
    void start();
    void stepComplete();
    double getAverageStepTime() const;
    double getTotalTime() const;
    size_t getStepsCompleted() const;
};

} // namespace nlm

// Factory function implementations
namespace nlm {

std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain with null config");
        return nullptr;
    }
    return std::make_shared<Brain>(config);
}

std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot create AgentBrain with null brain");
        return nullptr;
    }
    return std::make_shared<AgentBrain>(brain);
}
