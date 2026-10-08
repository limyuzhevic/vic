#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

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
    
    // Update autonomous development system
    void updateAutonomousDevelopment(double timestep);
    
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
    
    // Get combined neuromodulation score
    float getNeuromodulationScore() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get current environmental temperature
    float getEnvironmentalTemperature() const;
    
    // Get current environmental humidity
    float getEnvironmentalHumidity() const;
    
    // Get current environmental pressure
    float getEnvironmentalPressure() const;
    
    // Reset environmental context
    void resetEnvironmentalContext();
    
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
    
    // Advanced learning control
    void enableAdvancedLearning(bool enable);
    void setLearningRate(float rate);
    void enableMetaControl(bool enable);
    void setExplorationRate(float rate);
    void enableSelfMonitor(bool enable);
    void enableAutonomousDevelopment(bool enable);
    void enableSelfOptimization(bool enable);
    
    // Performance tracking
    void recordSuccess();
    void recordStep();
    float getSuccessRate() const;
    float getAverageReward() const;
    
    // Exploration and behavior
    void executeExplorationPattern();
    void setEnvironmentalContext(float temperature, float humidity, float pressure);
    void addReward(float reward, float predictedReward);
    void addError(float error);
    
    // Reset methods
    void reset();
    
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
    
    // Advanced learning features
    bool advancedLearningEnabled_;
    bool metaControlEnabled_;
    bool selfMonitorEnabled_;
    bool autonomousDevelopmentEnabled_;
    
    // Learning parameters
    float learningRate_;
    float explorationRate_;
    
    // Environmental sensors
    float environmentalTemperature_;
    float environmentalHumidity_;
    float environmentalPressure_;
    
    // Performance tracking
    uint64_t totalStepsExecuted_;
    uint64_t successfulActions_;
    float cumulativeReward_;
    
    // Internal processing
    struct Impl;
    Impl* internal_;
};

} // namespace nlm
