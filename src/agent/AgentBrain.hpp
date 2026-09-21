#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Integration with disconnected systems
    void setNeuralPlanner(std::unique_ptr<NeuralPlanner> planner) {
        neuralPlanner_ = std::move(planner);
    }
    void setPredictionSystem(std::unique_ptr<PredictionSystem> predictor) {
        predictionSystem_ = std::move(predictor);
    }
    void setConceptFormation(std::unique_ptr<ConceptFormation> conceptForm) {
        conceptFormation_ = std::move(conceptForm);
    }
    void setEpisodicMemory(std::unique_ptr<NeuralEpisodicMemory> episodicMem) {
        episodicMemory_ = std::move(episodeMem);
    }
    void setWorkingMemory(std::unique_ptr<NeuralWorkingMemory> workingMem) {
        workingMemory_ = std::move(workingMem);
    }
    void setNeuromodulator(std::unique_ptr<Neuromodulator> neuromod) {
        neuromodulator_ = std::move(neuromod);
    }
    void setDevelopmentSystem(std::unique_ptr<DevelopmentSystem> devSystem) {
        developmentSystem_ = std::move(devSystem);
    }
    
    // Integration methods
    void integrateNeuromodulation(double timestep);
    void updateMemoryConsolidation();
    void updateAdvancedSensoryProcessing();
    void updateMemoryReplay();
    
    // Get brain pointer (const version)
    const Brain* getBrain() const { return brain_.get(); }
    
    // Non-const version for backward compatibility
    Brain* getBrain() { return brain_.get(); }
    
    // Update all integrated systems
    void update(double timestep);
    
    // Main processing pipeline
    void processFrame(const SimpleWorld& world, const SensoryPercept& percept);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Simulation clock reference
    const SimulationClock* simulation_clock_ = nullptr;
    
    // Integrated systems
    std::unique_ptr<NeuralPlanner> neuralPlanner_;
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<Neuromodulator> neuromodulator_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
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
