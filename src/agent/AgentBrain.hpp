#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Integrates with all brain systems: memory, prediction, cognition, neuromodulation
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and prepare all integrated systems
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // Also stores percept in working memory, updates prediction, triggers attention
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // Integrates with planner and attentional selection
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    // Updates dopamine, prediction error, curiosity
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    // Advances developmental stage and affects all system parameters
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
    void enableMemoryStorage(bool enable) { memoryStorageEnabled_ = enable; }
    void enablePrediction(bool enable) { predictionEnabled_ = enable; }
    void enableCognition(bool enable) { cognitionEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isMemoryStorageEnabled() const { return memoryStorageEnabled_; }
    bool isPredictionEnabled() const { return predictionEnabled_; }
    bool isCognitionEnabled() const { return cognitionEnabled_; }
    
    // Integrated system access
    NeuralWorkingMemory* getWorkingMemory() const;
    NeuralEpisodicMemory* getEpisodicMemory() const;
    NeuralAssociativeMemory* getAssociativeMemory() const;
    PredictionSystem* getPredictionSystem() const;
    NeuralPlanner* getPlanner() const;
    ConceptFormation* getConceptFormation() const;
    AttentionalSelection* getAttention() const;
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionErrorSignal() const;
    
    // Development and plasticity management
    void advanceDevelopment();
    void triggerMemoryConsolidation();
    void triggerMemoryReplay();
    void updateSystemParameters();
    
    // Experience management
    void storeExperience(const SensoryPercept& percept, MotorCommand action, float reward);
    void retrieveMemoryPattern(const NeuralWorkingMemory::MemoryPattern& pattern);
    void updateConceptFromExperience(const SensoryPercept& percept);
    
    // Action planning integration
    std::vector<MotorCommand> planSequence(int depth);
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Current active state
    bool episodeActive_;
    size_t currentStepInEpisode_;
    
    // Integrated system references
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralAssociativeMemory> associativeMemory_;
    std::shared_ptr<PredictionSystem> predictionSystem_;
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<AttentionalSelection> attention_;
    std::shared_ptr<Dopamine> dopamine_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<Novelty> novelty_;
    std::shared_ptr<PredictionError> predictionError_;
    
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
    bool memoryStorageEnabled_;
    bool predictionEnabled_;
    bool cognitionEnabled_;
    
    // Planning and attention state
    std::vector<MotorCommand> plannedSequence_;
    size_t currentPlanStep_;
    float attentionLevel_;
    float explorationDrive_;
};

} // namespace nlm
