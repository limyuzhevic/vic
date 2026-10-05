#pragma once

#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class AgentBrain;

// Configuration system for AgentBrain parameters
struct AgentBrainConfig {
    // Motor neuron distribution
    static constexpr size_t motorForwardRatio = 1;
    static constexpr size_t motorBackwardRatio = 1;
    static constexpr size_t motorTurnLeftRatio = 1;
    static constexpr size_t motorTurnRightRatio = 1;
    static constexpr size_t motorInteractRatio = 1;
    static constexpr size_t motorWaitRatio = 1;
    static constexpr size_t motorLookLeftRatio = 1;
    static constexpr size_t motorLookRightRatio = 1;
    
    // Sensory neuron distribution
    static constexpr size_t sensoryVisionRatio = 4;
    static constexpr size_t sensoryTouchRatio = 2;
    static constexpr size_t sensoryInternalRatio = 2;
    static constexpr size_t sensoryProprioceptionRatio = 2;
    static constexpr size_t sensoryAudioRatio = 1;
    
    // Neuromodulation thresholds
    static constexpr float noveltyThreshold = 0.5f;
    static constexpr float curiosityThreshold = 0.3f;
    static constexpr float curiosityExplorationThreshold = 0.5f;
    static constexpr float predictionErrorClamping = 1.0f;
    
    // Development stages
    static constexpr double initialStageDuration = 60.0;      // ~1 minute
    static constexpr double criticalPeriodDuration = 240.0;   // ~4 minutes
    static constexpr double maturationDuration = 540.0;       // ~9 minutes
    
    // Performance and stability
    static constexpr float motorActivityThreshold = 0.5f;
    static constexpr float eligibilityTraceDecay = 0.1f;
    static constexpr float noveltyDecayRate = 0.99f;
    static constexpr float maxNormaliztion = 1.0f;
    
    // Configuration flags
    static constexpr bool enableRewardModulationDefault = true;
    static constexpr bool enableStructuralPlasticityDefault = true;
    static constexpr bool enableDevelopmentDefault = true;
    static constexpr bool enableCuriosityDefault = true;
    static constexpr bool enableSafetyChecksDefault = true;
};

// Event system for agent state changes
class AgentEventListener {
public:
    virtual ~AgentEventListener() = default;
    virtual void onMotorCommandSelected(MotorCommand cmd, float confidence) = 0;
    virtual void onNeuromodulationUpdated(float dopamine, float predictionError) = 0;
    virtual void onDevelopmentStageChanged(DevelopmentalStage stage) = 0;
    virtual void onSensoryProcessing(const std::vector<float>& vision) = 0;
};

// Motor neuron group manager
class MotorNeuronManager {
public:
    MotorNeuronManager(std::shared_ptr<Brain> brain);
    ~MotorNeuronManager() = default;
    
    void updateDistribution();
    MotorCommand decodeCommand(const std::vector<Neuron*>& motorNeurons, float threshold) const;
    
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    const std::vector<Neuron*>& getMotorLookLeft() const { return motorLookLeft_; }
    const std::vector<Neuron*>& getMotorLookRight() const { return motorLookRight_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    
    void distributeMotorNeurons();
};

// Sensory processor for handling different sensory modalities
class SensoryProcessor {
public:
    SensoryProcessor(AgentBrain* agentBrain);
    ~SensoryProcessor() = default;
    
    void processVision(const std::vector<float>& vision);
    void processTouch(const std::vector<float>& touch);
    void processInternal(const std::vector<float>& internal);
    void processProprioception(const std::vector<float>& proprioception);
    void processAudio(const std::vector<float>& audio);
    
    float getNoveltyLevel() const { return noveltyLevel_; }
    void setNoveltyDecayRate(float decay) { noveltyDecayRate_ = decay; }
    
private:
    AgentBrain* agentBrain_;
    std::vector<float> previousVision_;
    float noveltyLevel_;
    float noveltyDecayRate_;
    
    void computeNovelty(const std::vector<float>& currentVision);
};

// Neuromodulation system
class NeuromodulationSystem {
public:
    NeuromodulationSystem(AgentBrain* agentBrain);
    ~NeuromodulationSystem() = default;
    
    void applyRewardModulation(float reward, float predictedReward);
    void updateDopamine(float predictionError);
    void adjustPlasticity(float dopamineLevel);
    
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
private:
    AgentBrain* agentBrain_;
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityModifier_;
    bool rewardModulationEnabled_;
    
    void updateCuriosity(float novelty, float predictionError);
};

// Development system
class DevelopmentSystem {
public:
    DevelopmentSystem(AgentBrain* agentBrain);
    ~DevelopmentSystem() = default;
    
    void update(double timestep);
    DevelopmentalStage getStage() const { return currentStage_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
private:
    AgentBrain* agentBrain_;
    double developmentalAge_;
    float plasticityModifier_;
    DevelopmentalStage currentStage_;
    
    void updateStage(double timestep);
    void updateStructuralPlasticity(float plasticity);
};

// Curiosity and exploration system
class CuriositySystem {
public:
    CuriositySystem(AgentBrain* agentBrain);
    ~CuriositySystem() = default;
    
    MotorCommand selectWithCuriosity(MotorCommand defaultCommand);
    void updateCuriosity(float novelty, float predictionError);
    bool isCuriosityEnabled() const { return enabled_; }
    void setCuriosityEnabled(bool enabled) { enabled_ = enabled; }
    
    float getCuriosityLevel() const { return curiosityLevel_; }
    
private:
    AgentBrain* agentBrain_;
    bool enabled_;
    float curiosityLevel_;
    std::shared_ptr<Brain> brain_;
};

// Main AgentBrain class - now focused on coordination
class AgentBrain {
friend class MotorNeuronManager;
friend class SensoryProcessor;
friend class NeuromodulationSystem;
friend class DevelopmentSystem;
friend class CuriositySystem;

public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { neuromodulation_->rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { development_->structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { development_->developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosity_->setCuriosityEnabled(enable); }
    
    bool isRewardModulationEnabled() const { return neuromodulation_->rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return development_->structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return development_->developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosity_->isCuriosityEnabled(); }
    
    // Event system
    void addEventListener(AgentEventListener* listener) { eventListeners_.push_back(listener); }
    void removeEventListener(AgentEventListener* listener);
    
    // Debugging and monitoring
    void printDebugInfo() const;
    std::string getDebugInfo() const;
    
    // Statistical information
    size_t getActiveMotorNeurons() const;
    size_t getFiringNeuronCount() const;
    
private:
    std::shared_ptr<Brain> brain_;
    const AgentBrainConfig config_;
    
    // Subsystems
    std::unique_ptr<MotorNeuronManager> motorManager_;
    std::unique_ptr<SensoryProcessor> sensoryProcessor_;
    std::unique_ptr<NeuromodulationSystem> neuromodulation_;
    std::unique_ptr<DevelopmentSystem> development_;
    std::unique_ptr<CuriositySystem> curiosity_;
    
    // Event listeners
    std::vector<AgentEventListener*> eventListeners_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Cache for frequently accessed data
    mutable size_t cachedMotorCount_;
    mutable bool motorCountValid_;
    
    // Initialize all subsystems
    void initializeSubsystems(const SimpleWorld& world);
    
    // Notify event listeners
    void notifyEventListeners(const std::string& eventType, const std::string& data) const;
    
    // Safety checks
    bool validateBrainState() const;
    void safetyCheck() const;
};

} // namespace nlm