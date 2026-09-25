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
    
    // Configuration methods
    void enablePhysics(bool enable) { physicsEnabled_ = enable; }
    void enableActionLogging(bool enable) { actionLoggingEnabled_ = enable; }
    bool isPhysicsEnabled() const { return physicsEnabled_; }
    bool isActionLoggingEnabled() const { return actionLoggingEnabled_; }
    
    // Motor threshold and behavior control
    void setMotorThreshold(float threshold) { motorThreshold_ = threshold; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setDevelopmentSpeed(float speed) { developmentSpeed_ = speed; }
    void setSensoryGain(float gain) { sensoryGain_ = gain; }
    
    // Getters for configuration
    float getMotorThreshold() const { return motorThreshold_; }
    float getExplorationRate() const { return explorationRate_; }
    float getDevelopmentSpeed() const { return developmentSpeed_; }
    float getSensoryGain() const { return sensoryGain_; }
    
    // Neural activity monitoring
    size_t getActiveMotorNeurons() const;
    size_t getActiveSensoryNeurons() const;
    std::string getNeuralActivityStats() const;
    
    // Physics and movement control
    void updatePhysics(double timestep);
    void applyForce(float fx, float fy);
    void setPosition(float x, float y);
    std::pair<float, float> getPosition() const;
    float getDirection() const;
    void setDirection(float heading);
    std::pair<float, float> getVelocity() const;
    void setMaxSpeed(float speed);
    float getMaxSpeed() const;
    
    // State tracking
    float getHealth() const { return health_; }
    void setHealth(float health) { health_ = health; }
    float getEnergy() const { return energy_; }
    void setEnergy(float energy) { energy_ = energy; }
    double getAge() const { return developmentalAge_; }
    double getActiveTime() const { return activeTime_; }
    
    // Action and sensory history
    std::vector<Action> getActionHistory(size_t maxHistory = 100) const;
    std::vector<SensoryPercept> getSensoryHistory(size_t maxHistory = 100) const;
    
    // Performance metrics
    struct PerformanceMetrics {
        float totalReward = 0.0f;
        float efficiency = 0.0f;
        float explorationScore = 0.0f;
        float learningProgress = 0.0f;
        size_t actionCount = 0;
        double lastActionTime = 0.0;
        double sessionDuration = 0.0;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetMetrics();
    
    // Serialization and persistence
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    std::string serialize() const;
    bool deserialize(const std::string& data);
    std::vector<uint8_t> serializeBinary() const;
    bool deserializeBinary(const std::vector<uint8_t>& data);
    bool hasStateChanged() const;
    void markStateChanged();
    void resetStateChangedFlag();
    std::string createSnapshot() const;
    bool restoreSnapshot(const std::string& snapshot);
    std::string serializeConfig() const;
    bool deserializeConfig(const std::string& configData);
    std::string serializePerformance() const;
    bool deserializePerformance(const std::string& performanceData);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Physics and state
    bool physicsEnabled_;
    bool actionLoggingEnabled_;
    float x_;
    float y_;
    float orientation_;
    float velocityX_;
    float velocityY_;
    float angularVelocity_;
    float maxSpeed_;
    float health_;
    float energy_;
    double activeTime_;
    double lastPositionUpdate_;
    
    // Behavior configuration
    float motorThreshold_;
    float explorationRate_;
    float developmentSpeed_;
    float sensoryGain_;
    
    // History tracking
    std::vector<Action> actionHistory_;
    std::vector<SensoryPercept> sensoryHistory_;
    
    // Performance metrics
    PerformanceMetrics metrics_;
    mutable size_t spikeCount_;
    mutable double lastSpikeTime_;
    mutable float cumulativeScore_;
    
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

} // namespace nlm
