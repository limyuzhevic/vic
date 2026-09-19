#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Subsystem types for comprehensive management
enum class SubsystemType {
    RewardModulation,
    StructuralPlasticity,
    Development,
    Curiosity,
    Prediction,
    ConceptFormation,
    Planning,
    All
};

// System status snapshot for monitoring
struct SystemStatus {
    float curiosity;
    float novelty;
    float predictionError;
    float dopamineLevel;
    float expectedReward;
    double developmentalAge;
    DevelopmentalStage developmentalStage;
    float plasticityModifier;
    bool isHealthy; // Overall system health check
    
    SystemStatus()
        : curiosity(0.0f), novelty(0.0f), predictionError(0.0f), dopamineLevel(0.0f)
        , expectedReward(0.0f), developmentalAge(0.0), developmentalStage(DevelopmentalStage::Initial)
        , plasticityModifier(1.0f), isHealthy(true) {}
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with enhanced functionality
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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Enhanced subsystem management
    void enableSubsystem(SubsystemType type);
    void disableSubsystem(SubsystemType type);
    void toggleSubsystem(SubsystemType type);
    bool isSubsystemEnabled(SubsystemType type) const;
    
    // Get comprehensive system status
    SystemStatus getSystemStatus() const;
    
    // Convenience methods for common operations
    bool isActive() const;
    std::string getStatusSummary() const;
    MotorCommand getRecommendedAction() const;
    
    // Motor command decoding improvements
    MotorCommand decodeMotorCommandRobust();
    float getMotorConflictLevel() const;
    
    // Agent-brain integration
    void syncAgentBrain(const AgentBody& agentBody);
    const AgentBody& getAgentBody() const;
    void updateAgentFromWorld(const SimpleWorld& world);
    
    // Development and learning control
    void advanceDevelopment(double timestep);
    void resetForEpisode();
    void optimizeForTask(const std::string& taskType);
    
    // Health monitoring
    bool checkSystemHealth() const;
    std::vector<std::string> getHealthIssues() const;
    
    // Serialization/Reset control
    void checkpoint();
    void restore();
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced motor decoding with conflict resolution
    MotorCommand decodeWithConflictResolution();
    
    std::shared_ptr<Brain> brain_;
    
    // Agent body state for integration
    AgentBody agentBody_;
    
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
    
    // Individual subsystem states
    bool predictionEnabled_;
    bool conceptFormationEnabled_;
    bool planningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
