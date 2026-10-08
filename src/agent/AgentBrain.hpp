#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

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
    
    // ========== CONVENIENCE METHODS ==========
    
    // Common agent behavior: take action based on sensory input
    void takeAction(const SensoryPercept& percept, MotorCommand& chosenAction);
    
    // Common agent behavior: process sensory input and get motor command
    MotorCommand processAndAct(const SensoryPercept& percept);
    
    // Common agent behavior: evaluate if should explore vs exploit
    MotorCommand exploreOrExploit(MotorCommand bestAction, float threshold);
    
    // Get agent status for debugging/monitoring
    std::string getStatusString() const;
    
    // Check if agent is in a stable state (not learning or developing)
    bool isStable() const;
    
    // Get current agent capabilities
    struct AgentCapabilities {
        bool rewardLearning;      // Can learn from rewards
        bool curiosityDriven;     // Uses curiosity for exploration
        bool developmentalGrowth; // Develops over time
        bool memoryBased;         // Uses memory for behavior
        float plasticityRate;     // How fast it learns
    };
    
    AgentCapabilities getCapabilities() const;
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // ========== ADVANCED CONFIGURATION ==========
    
    // Set neuromodulation sensitivity
    void setNeuromodulationSensitivity(float sensitivity);
    
    // Set curiosity threshold for exploration
    void setCuriosityThreshold(float threshold);
    
    // Set developmental rate
    void setDevelopmentalRate(float rate);
    
    // Enable/disable specific agent capabilities
    void setCapability(AgentCapabilities::RewardLearning enabled);
    void setCapability(AgentCapabilities::CuriosityDriven enabled);
    void setCapability(AgentCapabilities::DevelopmentalGrowth enabled);
    void setCapability(AgentCapabilities::MemoryBased enabled);
    void setCapability(AgentCapabilities::PlasticityRate rate);
    
    // Get current values
    float getNeuromodulationSensitivity() const;
    float getCuriosityThreshold() const;
    float getDevelopmentalRate() const;
    float getPlasticityRate() const;
    
    // ========== EPISODE MANAGEMENT ==========
    
    // Start new episode
    void startEpisode(double initialTime = 0.0);
    
    // End current episode
    void endEpisode();
    
    // Check if episode is active
    bool isEpisodeActive() const;
    
    // Get episode statistics
    struct EpisodeStats {
        double duration;           // How long the episode lasted
        float totalReward;         // Total reward received
        size_t actionCount;        // Number of actions taken
        float explorationRate;    // How much exploration occurred
        float learningRate;       // How much was learned
    };
    
    EpisodeStats getEpisodeStats() const;
    
    // Get episode start time
    double getEpisodeStartTime() const;
    
    // ========== STATE MANAGEMENT ==========
    
    // Save agent state for checkpointing
    bool saveState(const std::string& filepath) const;
    
    // Load agent state from checkpoint
    bool loadState(const std::string& filepath);
    
    // Create backup of current state
    bool createBackup(const std::string& backupPath) const;
    
    // Restore from backup
    bool restoreFromBackup(const std::string& backupPath);
    
    // Reset all state to initial values
    void resetToDefaults();
    
    // ========== MONITORING AND DEBUGGING ==========
    
    // Get spike count for monitoring
    size_t getSpikeCount() const;
    
    // Get active neuron count
    size_t getActiveNeuronCount() const;
    
    // Get recent activity statistics
    struct ActivityStats {
        float averageFiringRate;
        float peakActivity;
        size_t timeStepsActive;
        std::vector<float> recentActivity;  // Last 10 time steps
    };
    
    ActivityStats getActivityStats() const;
    
    // Check if agent is processing sensory input
    bool hasSensoryInput() const;
    
    // Get last sensory percept
    SensoryPercept getLastSensoryPercept() const;
    
    // ========== HIGH-LEVEL AGENT BEHAVIORS ==========
    
    // Habituation: reduce response to frequent stimuli
    void applyHabituation(const SensoryPercept& percept, float decayRate);
    
    // Sensitization: increase response to novel stimuli
    void applySensitization(const SensoryPercept& percept, float amplification);
    
    // Fatigue: reduce motor output after prolonged activity
    void applyFatigue(float decayRate, float recoveryRate);
    
    // Rest: allow agent to recover and reset state
    void applyRest(double restDuration);
    
    // Emergency reset: stop all activity and reset completely
    void emergencyReset();
    
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
    
    // Advanced configuration
    float neuromodulationSensitivity_;
    float curiosityThreshold_;
    float developmentalRate_;
    
    // Episode management
    bool episodeActive_;
    double episodeStartTime_;
    double currentEpisodeDuration_;
    float episodeTotalReward_;
    size_t episodeActionCount_;
    float episodeExplorationRate_;
    float episodeLearningRate_;
    
    // State management
    std::vector<float> recentActivity_;
    SensoryPercept lastPercept_;
    double lastUpdateTime_;
    
    // Fatigue and recovery state
    float fatigueLevel_;
    float recoveryRate_;
    double lastRestTime_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
