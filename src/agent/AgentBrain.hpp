#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Forward declarations
class AgentBrain;

// Strategy pattern interfaces for different implementations
class SensoryProcessingStrategy {
public:
    virtual ~SensoryProcessingStrategy() = default;
    virtual void processSensoryInput(const SensoryPercept& percept) = 0;
    virtual size_t getSensoryInputSize() const = 0;
    virtual std::string getStrategyName() const = 0;
};

class MotorDecodingStrategy {
public:
    virtual ~MotorDecodingStrategy() = default;
    virtual MotorCommand decodeMotorCommand() = 0;
    virtual size_t getMotorOutputSize() const = 0;
    virtual std::vector<MotorGroupActivity> getMotorGroupActivities() const = 0;
    virtual std::string getStrategyName() const = 0;
};

class NeuromodulationStrategy {
public:
    virtual ~NeuromodulationStrategy() = default;
    virtual void applyRewardModulation(float reward, float predictedReward) = 0;
    virtual float getNeuromodulationLevel() const = 0;
    virtual std::string getStrategyName() const = 0;
};

class DevelopmentStrategy {
public:
    virtual ~DevelopmentStrategy() = default;
    virtual void updateDevelopment(double timestep) = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual std::string getStrategyName() const = 0;
};

// Default strategy implementations
class DefaultSensoryProcessingStrategy : public SensoryProcessingStrategy {
public:
    DefaultSensoryProcessingStrategy(std::shared_ptr<AgentBrain> agentBrain);
    void processSensoryInput(const SensoryPercept& percept) override;
    size_t getSensoryInputSize() const override;
    std::string getStrategyName() const override;
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

class DefaultMotorDecodingStrategy : public MotorDecodingStrategy {
public:
    DefaultMotorDecodingStrategy(std::shared_ptr<AgentBrain> agentBrain);
    MotorCommand decodeMotorCommand() override;
    size_t getMotorOutputSize() const override;
    std::vector<MotorGroupActivity> getMotorGroupActivities() const override;
    std::string getStrategyName() const override;
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

class DefaultNeuromodulationStrategy : public NeuromodulationStrategy {
public:
    DefaultNeuromodulationStrategy(std::shared_ptr<AgentBrain> agentBrain);
    void applyRewardModulation(float reward, float predictedReward) override;
    float getNeuromodulationLevel() const override;
    std::string getStrategyName() const override;
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

class DefaultDevelopmentStrategy : public DevelopmentStrategy {
public:
    DefaultDevelopmentStrategy(std::shared_ptr<AgentBrain> agentBrain);
    void updateDevelopment(double timestep) override;
    DevelopmentalStage getDevelopmentalStage() const override;
    std::string getStrategyName() const override;
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// AgentBrain: Connects NLM brain to the world using strategy pattern
// Handles sensory transduction and motor decoding with pluggable strategies
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
    
    // Strategy management
    void setSensoryProcessingStrategy(std::unique_ptr<SensoryProcessingStrategy> strategy);
    void setMotorDecodingStrategy(std::unique_ptr<MotorDecodingStrategy> strategy);
    void setNeuromodulationStrategy(std::unique_ptr<NeuromodulationStrategy> strategy);
    void setDevelopmentStrategy(std::unique_ptr<DevelopmentStrategy> strategy);
    
    std::string getStrategyInfo() const;
    
    // Diagnostic: per motor group, in decode order
    struct MotorGroupActivity {
        const char* name;
        float activity;
        size_t refractoryCount;
        size_t size = 0;
        float firingRate = 0.0f;
    };
    std::vector<MotorGroupActivity> getMotorGroupActivities() const;
    
    // Thought trace (Phase 5): a one-line, human-readable record of the
    // most recent loop
    std::string getThoughtTrace() const;
    
    // Learned preference trait, stored on the Brain so it survives a
    // checkpoint reload
    float getLearnedPreference() const;
    
    // Phase 6: text-as-a-sense. Encode a teacher word into the sparse
    // pattern of text-neuron indices it drives
    static std::vector<size_t> encodeText(const std::string& text);
    
    // Internal (non-motor, non-sensory) firing rates
    std::vector<float> getInternalFiringRates() const;
    
    // Phase 6 thought/planning counters
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }
    
    // Phase 6 wired systems counters
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    size_t getSocialObservationCount() const;
    
    // Social learning interface
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision);
    
    // Configuration getters
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Core components
    std::shared_ptr<Brain> brain_;
    
    // Strategy components
    std::unique_ptr<SensoryProcessingStrategy> sensoryStrategy_;
    std::unique_ptr<MotorDecodingStrategy> motorStrategy_;
    std::unique_ptr<NeuromodulationStrategy> neuromodulationStrategy_;
    std::unique_ptr<DevelopmentStrategy> developmentStrategy_;
    
    // Legacy members (for backward compatibility and fallback)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> internalNeurons_;
    
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
    
    // Slowly adapting prediction of the visual scene
    std::vector<float> predictedVision_;
    float sensoryNoveltyDecay_;
    float maxTouch_;
    float forwardTouch_;
    float forwardObjectVision_;
    int lastWinnerGroup_;
    float lastWinnerMargin_;
    int consecutiveWinnerCount_;
    int stepsSinceTurn_;
    int repeatCount_;
    int reflexBonusCount_;
    int reflexSuppressCount_;
    int wtaPulseCount_;
    MotorCommand lastDecoded_;
    bool hasLastDecoded_;
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_;
    bool socialReady_;
    std::vector<float> lastTouch_;
    std::string lastText_;
    int plannerOverrideCount_;
    int plannerAgreementCount_;
    int plannerDecisionCount_;
    int conceptBiasCount_;
    int selfBiasCount_;
    int socialBiasCount_;
    std::vector<float> lastVision_;
    std::string thoughtPercept_;
    MotorCommand thoughtPlanned_;
    bool hasThoughtPlanned_;
    float thoughtReward_;
    bool hasThoughtOutcome_;
};

} // namespace nlm