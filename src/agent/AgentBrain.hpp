#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Action;
class Neuromodulator;
class DevelopmentalStage;
class NeuronType;
class NeuronId;
class RegionId;
class SimulationStep;
class Timestamp;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with integrated neuromodulation
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and configure sensory/motor mappings
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain (from agent's perspective)
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected (number of possible actions)
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // Extracts features from sensory input and applies to neural activity
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // Selects action based on motor neuron population activity
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward prediction
    // Uses prediction error to modulate learning and plasticity
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system based on experience
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get current neuromodulation level (dopamine)
    float getNeuromodulationLevel() const;
    
    // Get curiosity level (exploration motivation)
    float getCuriosityLevel() const;
    
    // Get novelty level (new information detection)
    float getNoveltyLevel() const;
    
    // Get prediction error signal
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration methods for agent behavior
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Set exploration vs exploitation balance
    void setExplorationWeight(float weight) { explorationWeight_ = weight; }
    void setExploitationWeight(float weight) { exploitationWeight_ = weight; }
    
    // Get current neuromodulation statistics
    float getAverageNeuromodulation() const;
    float getMaxNeuromodulation() const;
    float getMinNeuromodulation() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration bias
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Extract and process sensory features
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    
    // Update neuromodulatory signals
    void updateNeuromodulation(double timestep);
    void updateCuriosity(double timestep);
    void updateNovelty(double timestep);
    
    // Validate sensory data before processing
    bool validateSensoryInput(const SensoryPercept& percept) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups for action selection
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups for feature extraction
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state for learning and plasticity
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float explorationWeight_;
    float exploitationWeight_;
    
    // Development state tracking
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags for agent subsystems
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection and change detection
    std::vector<float> previousVision_;
    std::vector<float> previousTouch_;
    std::vector<float> previousInternal_;
    float sensoryNoveltyDecay_;
    
    // Statistics for monitoring agent performance
    float totalNeuromodulationSum_;
    float maxNeuromodulation_;
    float minNeuromodulation_;
    size_t neuromodulationCount_;
    
    // Helper methods
    static float clamp(float value, float min, float max);
    static float normalize(float value, float min, float max);
};

} // namespace nlm
