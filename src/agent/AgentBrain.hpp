#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
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
    
    // Advanced agent control
    void setExplorationWeight(float weight) { explorationWeight_ = weight; }
    void setExploitationWeight(float weight) { exploitationWeight_ = weight; }
    void setNoveltyThreshold(float threshold) { noveltyThreshold_ = threshold; }
    void setRiskTolerance(float tolerance) { riskTolerance_ = tolerance; }
    
    float getExplorationWeight() const { return explorationWeight_; }
    float getExploitationWeight() const { return exploitationWeight_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getRiskTolerance() const { return riskTolerance_; }
    
    // Advanced neuromodulation
    void setDopamineGain(float gain) { dopamineGain_ = gain; }
    void setCuriosityGain(float gain) { curiosityGain_ = gain; }
    void setNoveltyGain(float gain) { noveltyGain_ = gain; }
    
    float getDopamineGain() const { return dopamineGain_; }
    float getCuriosityGain() const { return curiosityGain_; }
    float getNoveltyGain() const { return noveltyGain_; }
    
    // Advanced development
    void setDevelopmentRate(float rate) { developmentRate_ = rate; }
    void setMaturationSpeed(float speed) { maturationSpeed_ = speed; }
    
    float getDevelopmentRate() const { return developmentRate_; }
    float getMaturationSpeed() const { return maturationSpeed_; }
    
    // Statistical analysis
    std::vector<float> getActivityDistribution() const;
    std::vector<float> getWeightDistribution() const;
    float calculateFiringCoherence() const;
    float calculateSynapticStrength() const;
    
    // State inspection
    size_t countActiveNeurons() const;
    size_t countConnectedSynapses() const;
    float getAverageMembranePotential() const;
    float getAverageSynapticWeight() const;
    
    // Learning statistics
    float getLearningRate() const { return learningRate_; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Performance metrics
    float getPerformanceScore() const;
    std::vector<float> getPerformanceHistory() const;
    void recordPerformance(float score);
    
    // Memory management
    void optimizeMemoryUsage();
    void compressMemory();
    size_t getMemoryUsage() const;
    
    // Advanced reset options
    void resetDevelopment();
    void resetNeuromodulation();
    void resetLearning();
    void resetAll();
    
    // Configuration presets
    void loadPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Debug and diagnostic
    void enableDebugMode(bool enable) { debugMode_ = enable; }
    bool isDebugMode() const { return debugMode_; }
    
    void enableProfiling(bool enable) { profilingEnabled_ = enable; }
    bool isProfilingEnabled() const { return profilingEnabled_; }
    
    std::string getDebugInfo() const;
    std::vector<float> getProfilingData() const;
    
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
