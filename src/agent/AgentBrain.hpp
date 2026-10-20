// AgentBrain.hpp - Enhanced AgentBrain interface with configurable distribution
#ifndef NLM_AGENTBRAIN_HPP
#define NLM_AGENTBRAIN_HPP

#include "brain/Brain.hpp"
#include "neuron/NeuronId.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

enum class MotorCommand {
    Wait,
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact
};

class AgentBrain {
public:
    // Configuration structure for neuron distribution
    struct NeuronDistributionConfig {
        // Motor neuron distribution configuration
        struct MotorConfig {
            std::vector<std::string> motorGroupNames;
            std::vector<size_t> neuronsPerGroup;
            bool useModularDistribution = true;
        } motor;
        
        // Sensory neuron distribution configuration
        struct SensoryConfig {
            std::vector<std::string> sensoryGroupNames;
            std::vector<size_t> neuronsPerGroup;
            bool useModularDistribution = true;
        } sensory;
        
        // Advanced distribution options
        bool enableDynamicRebalancing = false;
        float groupBalanceThreshold = 0.2f;
        size_t maxRedistributionSteps = 10;
    };

    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Configuration management
    void setNeuronDistributionConfig(const NeuronDistributionConfig& config) {
        distributionConfig_ = config;
        redistributeNeurons();
    }
    
    const NeuronDistributionConfig& getNeuronDistributionConfig() const {
        return distributionConfig_;
    }
    
    // Core processing methods
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void updateDevelopment(double timestep);
    void applyRewardModulation(float reward, float predictedReward);
    
    // Neuron access methods (enhanced)
    const std::vector<Neuron*>& getMotorForwardNeurons() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackwardNeurons() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeftNeurons() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRightNeurons() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteractNeurons() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWaitNeurons() const { return motorWait_; }
    
    const std::vector<Neuron*>& getSensoryVisionNeurons() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouchNeurons() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternalNeurons() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioceptionNeurons() const { return sensoryProprioception_; }
    
    // Enhanced utility methods
    size_t getTotalMotorNeurons() const;
    size_t getTotalSensoryNeurons() const;
    size_t getNeuronsInGroup(const std::string& groupName) const;
    std::vector<std::string> getMotorGroupNames() const;
    std::vector<std::string> getSensoryGroupNames() const;
    
    // Configuration generation methods
    static NeuronDistributionConfig createDefaultConfig();
    static NeuronDistributionConfig createBalancedConfig();
    static NeuronDistributionConfig createExplorationConfig();
    static NeuronDistributionConfig createNavigationConfig();
    
    // Development and state management
    void reset();
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
private:
    std::shared_ptr<Brain> brain_;
    NeuronDistributionConfig distributionConfig_;
    
    // Neuron group storage (enhanced)
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
    
    // Dynamic state tracking
    std::vector<float> previousVision_;
    double developmentalAge_;
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityModifier_;
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    
    // Private helper methods
    void redistributeNeurons();
    void initializeMotorGroups();
    void initializeSensoryGroups();
    void balanceGroupSizes();
    void updateDynamicDistribution();
    std::vector<Neuron*> getAllMotorNeurons() const;
    std::vector<Neuron*> getAllSensoryNeurons() const;
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    float calculateGroupActivity(const std::vector<Neuron*>& neurons) const;
    std::string getGroupNameForIndex(size_t index, bool isMotor) const;
};

} // namespace nlm

#endif // NLM_AGENTBRAIN_HPP
