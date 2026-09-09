#ifndef NLM_AGENTBRAIN_HPP
#define NLM_AGENTBRAIN_HPP

#include "Brain.hpp"
#include "SimpleWorld.hpp"
#include "Action.hpp"
#include "DevelopmentSystem.hpp"

namespace nlm {

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    void initialize(const SimpleWorld& world);
    void reset();
    
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    
    // Subsystem control
    void enableRewardModulation(bool enabled);
    void enableStructuralPlasticity(bool enabled);
    void enableDevelopment(bool enabled);
    void enableCuriosity(bool enabled);
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // State
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    std::vector<float> previousVision_;
};

} // namespace nlm

#endif // NLM_AGENTBRAIN_HPP