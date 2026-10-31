#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "AgentBrainSensory.hpp"
#include "AgentBrainMotor.hpp"
#include "AgentBrainLearning.hpp"
#include "AgentBrainDevelopment.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class AgentBrainCore {
public:
    AgentBrainCore(std::shared_ptr<Brain> brain);
    ~AgentBrainCore();
    
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
    
    // Thought trace (Phase 5): a one-line, human-readable record of the
    // most recent loop — what was perceived, what the planner suggested
    // from learned action quality, what the brain actually decoded, and
    // the outcome reward.
    std::string getThoughtTrace() const;
    
    // Learned preference trait, stored on the Brain so it survives a
    // checkpoint reload
    float getLearnedPreference() const;
    
    // Phase 5 diagnostic counters
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }
    
        // Get thought planned
    MotorCommand getThoughtPlanned() const { return thoughtPlanned_; }
    
    // Set thought planned
    void setThoughtPlanned(MotorCommand cmd) { thoughtPlanned_ = cmd; }
    
    // Check if thought planned
    bool hasThoughtPlanned() const { return hasThoughtPlanned_; }
    
    // Set has thought planned
    void setHasThoughtPlanned(bool hasPlanned) { hasThoughtPlanned_ = hasPlanned; }
    
    // Get thought reward
    float getThoughtReward() const { return thoughtReward_; }
    
    // Set thought reward
    void setThoughtReward(float reward) { thoughtReward_ = reward; }
    
    // Check if has thought outcome
    bool hasThoughtOutcome() const { return hasThoughtOutcome_; }
    
    // Set has thought outcome
    void setHasThoughtOutcome(bool hasOutcome) { hasThoughtOutcome_ = hasOutcome; }
    
    // Get last decoded command
    MotorCommand getLastDecoded() const { return lastDecoded_; }
    
    // Set last decoded command
    void setLastDecoded(MotorCommand cmd) { lastDecoded_ = cmd; }
    
    // Check if has last decoded
    bool hasLastDecoded() const { return hasLastDecoded_; }
    
    // Set has last decoded
    void setHasLastDecoded(bool has) { hasLastDecoded_ = has; }
    
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
    
    // Thought trace (Phase 5)
    MotorCommand thoughtPlanned_;
    bool hasThoughtPlanned_;
    float thoughtReward_;
    bool hasThoughtOutcome_;
    
    // Motor state
    MotorCommand lastDecoded_;
    bool hasLastDecoded_;
};