#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain configuration parameters
struct AgentBrainConfig {
    // Motor system configuration
    size_t numMotorGroups;           // Number of motor neuron groups (default: 6)
    std::vector<MotorCommand> motorGroupCommands; // Commands for each group
    
    // Sensory system configuration  
    size_t numSensoryGroups;         // Number of sensory neuron groups (default: 4)
    std::vector<std::string> sensoryGroupNames; // Names for each group
    
    // Novelty detection configuration
    float noveltyThreshold;          // Threshold for significant novelty (default: 0.01f)
    float sensoryNoveltyDecay;       // Decay factor for novelty (default: 0.99f)
    
    // Curiosity configuration
    bool curiosityEnabled;           // Whether curiosity-based exploration is enabled (default: true)
    float curiosityThreshold;        // Minimum curiosity level to trigger exploration (default: 0.3f)
    float maxExplorationChance;      // Maximum random exploration chance (default: 0.3f)
    float curiosityNoiseLevel;       // Noise added to curiosity signal (default: 0.05f)
    
    // Reward modulation configuration
    bool rewardModulationEnabled;    // Whether reward modulation is enabled (default: true)
    float rewardPredictionWindow;    // Time window for reward prediction (default: 1.0f)
    float dopamineGain;              // Dopamine signal gain (default: 1.0f)
    
    // Developmental configuration
    bool developmentEnabled;         // Whether development system is enabled (default: true)
    bool structuralPlasticityEnabled; // Whether structural plasticity is enabled (default: true)
    float developmentalStages[3];    // Age thresholds for developmental stages
    float plasticityModifiers[4];    // Plasticity modifier for each developmental stage
    
    // Performance configuration
    size_t minNeuronsForDistribution; // Minimum neurons needed before distribution (default: 10)
    float motorActivityThreshold;     // Minimum activity to consider motor command (default: 0.5f)
    float sensoryInjectionScale;      // Scale factor for sensory current injection (default: 5.0f)
    
    // Exploration configuration
    size_t numMotorCommands;         // Number of motor commands for random exploration (default: 8)
    float explorationTemperature;    // Temperature for exploration randomness (default: 1.0f)
    float explorationDecay;          // Decay rate for exploration over time (default: 0.995f)
    
    // Configuration loading
    void loadFromConfig(const Config& config);
    void applyToAgentBrain(AgentBrain& brain) const;
    
    // Default configuration
    static AgentBrainConfig getDefaultConfig() {
        AgentBrainConfig config;
        
        // Motor system
        config.numMotorGroups = 6;
        config.motorGroupCommands = {
            MotorCommand::MoveForward,
            MotorCommand::MoveBackward,
            MotorCommand::TurnLeft,
            MotorCommand::TurnRight,
            MotorCommand::Interact,
            MotorCommand::Wait
        };
        
        // Sensory system
        config.numSensoryGroups = 4;
        config.sensoryGroupNames = {"Vision", "Touch", "Internal", "Proprioception"};
        
        // Novelty detection
        config.noveltyThreshold = 0.01f;
        config.sensoryNoveltyDecay = 0.99f;
        
        // Curiosity
        config.curiosityEnabled = true;
        config.curiosityThreshold = 0.3f;
        config.maxExplorationChance = 0.3f;
        config.curiosityNoiseLevel = 0.05f;
        
        // Reward modulation
        config.rewardModulationEnabled = true;
        config.rewardPredictionWindow = 1.0f;
        config.dopamineGain = 1.0f;
        
        // Development
        config.developmentEnabled = true;
        config.structuralPlasticityEnabled = true;
        config.developmentalStages = {60.0, 300.0, 900.0}; // Age thresholds
        config.plasticityModifiers = {1.0f, 0.8f, 0.5f, 0.2f}; // Stage modifiers
        
        // Performance
        config.minNeuronsForDistribution = 10;
        config.motorActivityThreshold = 0.5f;
        config.sensoryInjectionScale = 5.0f;
        
        // Exploration
        config.numMotorCommands = 8;
        config.explorationTemperature = 1.0f;
        config.explorationDecay = 0.995f;
        
        return config;
    }
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig::getDefaultConfig());
    ~AgentBrain();
    
    // Initialize with world and configuration
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
    void setConfig(const AgentBrainConfig& config) { config_ = config; applyToConfig(); }
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Enable/disable systems
    void enableRewardModulation(bool enable) { config_.rewardModulationEnabled = enable; }
    void enableStructuralPlasticity(bool enable) { config_.structuralPlasticityEnabled = enable; }
    void enableDevelopment(bool enable) { config_.developmentEnabled = enable; }
    void enableCuriosity(bool enable) { config_.curiosityEnabled = enable; }
    
    bool isRewardModulationEnabled() const { return config_.rewardModulationEnabled; }
    bool isStructuralPlasticityEnabled() const { return config_.structuralPlasticityEnabled; }
    bool isDevelopmentEnabled() const { return config_.developmentEnabled; }
    bool isCuriosityEnabled() const { return config_.curiosityEnabled; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Apply configuration to internal state
    void applyToConfig();
    
    // Internal state
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    
    // Motor neuron groups (organized by action)
    std::vector<MotorCommand> motorCommandMap_; // Maps group index to command
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (organized by type)
    std::vector<std::string> sensoryGroupNames_;
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float explorationState_; // Current exploration tendency
    
    // Helper methods
    MotorCommand randomMotorCommand() const;
    float calculateExplorationProbability() const;
    float computeNovelty(const std::vector<float>& currentVision) const;
    void updateExploration(double timestep);
    std::vector<float> getWeightedSensoryInput(const SensoryPercept& percept) const;
};

} // namespace nlm