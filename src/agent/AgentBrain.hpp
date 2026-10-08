#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "AdvancedAgent.hpp"
#include "AdvancedAgentBrain.hpp"
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
    
    // Advanced command interface
    CommandResult executeCommand(const std::string& commandLine);
    CommandResult executeComplexAction(ComplexAction action);
    
    // Script management
    bool loadScript(const std::string& filename);
    bool executeScript(const std::string& scriptName);
    bool scheduleScript(const std::string& scriptName, double delay = 0.0);
    
    // Command history and completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory(size_t limit = 10) const;
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    
    // Command registration
    void registerCommand(const std::string& name, std::unique_ptr<Command> command);
    bool hasCommand(const std::string& name) const;
    
    // State management
    void saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Configuration
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
    // Script variables and context
    void setVariable(const std::string& name, float value);
    float getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    void setContextVariable(const std::string& name, const std::string& value);
    std::string getContextVariable(const std::string& name) const;
    
    // Advanced agent brain access
    std::shared_ptr<AdvancedAgentBrain> getAdvancedAgentBrain() const { return advancedAgentBrain_; }
    
    // Configuration flags
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
    std::shared_ptr<AdvancedAgentBrain> advancedAgentBrain_;
    
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
    bool verbose_;
    size_t maxHistorySize_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Scripting and advanced features
    bool explorationMode_;
    bool learningMode_;
};

} // namespace nlm
