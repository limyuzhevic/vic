#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <mutex>

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
    
    // Advanced configuration
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationBase(float base) { explorationBase_ = base; }
    void setMotorCommandTimeout(float timeout) { motorCommandTimeout_ = timeout; }
    void setNeuromodulationGain(float gain) { neuromodulationGain_ = gain; }
    void setDevelopmentRate(double rate) { developmentRate_ = rate; }
    
    // Statistics and debugging
    struct AgentStats {
        size_t totalSteps = 0;
        size_t motorCommandsIssued = 0;
        float averageFiringRate = 0.0f;
        float totalRewardReceived = 0.0f;
        double averageDevelopmentAge = 0.0;
        size_t episodeCount = 0;
    };
    
    AgentStats getStats() const { return stats_; }
    void resetStats() { stats_ = AgentStats(); }
    
    // Utility methods
    std::string getDebugInfo() const;
    bool validateState() const;
    
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
    
    // Thread safety
    mutable std::mutex stateMutex_;
    
    // Advanced configuration parameters
    float curiosityThreshold_;
    float explorationBase_;
    float motorCommandTimeout_;
    float neuromodulationGain_;
    double developmentRate_;
    
    // Statistics and debugging
    mutable AgentStats stats_;
    
    // Constants for motor commands
    static constexpr size_t NUM_MOTOR_COMMANDS = 6;
    static constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    static constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_CURIOUSNESS_THRESHOLD = 0.3f;
    static constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.5f;
    static constexpr float DEFAULT_NOVLEDY_DECAY = 0.99f;
    static constexpr float DEFAULT_PLASTICITY_MODIFIER = 1.0f;
    static constexpr double DEFAULT_DEVELOPMENT_RATE = 0.001;
    static constexpr float DEFAULT_NEUROMODULATION_GAIN = 1.0f;

} // namespace nlm
