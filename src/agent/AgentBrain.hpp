#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced features
    
    // Get all concepts discovered by brain
    std::vector<size_t> getDiscoveredConceptIds() const;
    
    // Get concept stability for planning
    float getConceptStability(size_t conceptId) const;
    
    // Get prediction error history
    const std::vector<float>& getPredictionErrorHistory() const;
    
    // Get neural planner if available
    NeuralPlanner* getPlanner() const { return brain_->getPlanner(); }
    
    // Get concept formation system if available  
    ConceptFormation* getConceptFormation() const { return brain_->getConceptFormation(); }
    
    // Get prediction system if available
    PredictionSystem* getPredictionSystem() const { return brain_->getPredictionSystem(); }
    
    // Get working memory if available
    NeuralWorkingMemory* getWorkingMemory() const { return brain_->getWorkingMemory(); }
    
    // Get episodic memory if available
    NeuralEpisodicMemory* getEpisodicMemory() const { return brain_->getEpisodicMemory(); }
    
    // Simulation control
    
    // Set goal for planning (can be vision pattern or abstract goal)
    void setPlanningGoal(const std::vector<float>& goal) { currentGoal_ = goal; }
    
    // Set planning depth (how many steps ahead to plan)
    void setPlanningDepth(size_t depth) { 
        if (auto* planner = getPlanner()) {
            planner->setPlanningDepth(depth);
        }
    }
    
    // Enable/disable exploration behavior
    void enableExploration(bool enable) { explorationEnabled_ = enable; }
    
    // Set exploration threshold (curiosity level needed for random actions)
    void setExplorationThreshold(float threshold) { explorationThreshold_ = threshold; }
    
    // Set reward scaling factor for neuromodulation
    void setRewardScale(float scale) { rewardScale_ = scale; }
    
    // Development control
    
    // Skip to a specific developmental stage
    void skipToDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Set developmental timeline speed (for faster/slower development)
    void setDevelopmentSpeed(float speed) { developmentSpeed_ = speed; }
    
    // Get current developmental metrics
    float getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Memory statistics
    
    // Get working memory usage statistics
    size_t getWorkingMemoryActiveTraces() const {
        auto* workingMem = getWorkingMemory();
        return workingMem ? workingMem->getActiveTraces() : 0;
    }
    
    // Get episodic memory episode count
    size_t getEpisodicMemoryEpisodeCount() const {
        auto* episodicMem = getEpisodicMemory();
        return episodicMem ? episodicMem->getEpisodeCount() : 0;
    }
    
    // Get concept formation statistics
    size_t getConceptCount() const {
        auto* conceptForm = getConceptFormation();
        return conceptForm ? conceptForm->getConceptCount() : 0;
    }
    
    // Get prediction system statistics
    size_t getPredictionErrorHistorySize() const {
        auto* predSys = getPredictionSystem();
        return predSys ? predSys->getErrorHistory().size() : 0;
    }
    
    // Advanced neuromodulation
    
    // Set custom neuromodulation factors (dopamine, curiosity, novelty weights)
    void setNeuromodulationFactors(float dopamineWeight, float curiosityWeight, float noveltyWeight) {
        neuromodulationWeights_[0] = dopamineWeight;
        neuromodulationWeights_[1] = curiosityWeight; 
        neuromodulationWeights_[2] = noveltyWeight;
    }
    
    // Get current neuromodulation vector
    std::vector<float> getNeuromodulationVector() const {
        return {
            getNeuromodulationLevel() * neuromodulationWeights_[0],
            getCuriosityLevel() * neuromodulationWeights_[1],
            getNoveltyLevel() * neuromodulationWeights_[2]
        };
    }
    
    // Advanced learning
    
    // Trigger learning episode for all systems
    void triggerLearningEpisode() {
        // Update concept formation with recent experience
        auto* conceptForm = getConceptFormation();
        if (conceptForm && conceptForm->getConceptCount() > 0) {
            // This would use stored sensory patterns from recent simulation
            // For now, just clear for next episode
            NLM_LOG_INFO("Learning episode triggered - concepts will update from experience");
        }
        
        // Train prediction system with recent observations
        auto* predictionSys = getPredictionSystem();
        if (predictionSys) {
            predictionSys->clearHistory(); // Reset for new learning
            NLM_LOG_INFO("Prediction system cleared for new learning episode");
        }
    }
    
    // Get performance metrics
    
    // Get simulation statistics
    struct SimulationStats {
        size_t stepCount;
        float avgFiringRate;
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        float plasticityLevel;
        float developmentProgress;
        float memoryUtilization;
        float learningRate;
    };
    
    SimulationStats getSimulationStats() const {
        SimulationStats stats;
        stats.stepCount = brain_->getTotalSpikeCount();
        stats.avgFiringRate = brain_->getAverageFiringRate();
        stats.totalNeurons = brain_->getTotalNeuronCount();
        stats.totalSynapses = brain_->getTotalSynapseCount();
        stats.activeNeurons = brain_->getActiveNeuronCount();
        stats.plasticityLevel = plasticityModifier_;
        stats.developmentProgress = std::min(1.0f, developmentalAge_ / 900.0f);
        stats.memoryUtilization = 0.5f; // Simplified
        stats.learningRate = getNeuromodulationLevel();
        return stats;
    }
    
    // State management
    
    // Save current brain state (checkpoint)
    bool saveBrainState(const std::string& filepath) const {
        if (brain_) {
            return brain_->save(filepath);
        }
        return false;
    }
    
    // Load brain state from checkpoint
    bool loadBrainState(const std::string& filepath) {
        if (brain_) {
            return brain_->load(filepath);
        }
        return false;
    }
    
    // Get current developmental stage as string
    std::string getDevelopmentalStageString() const {
        switch (getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            default: return "Unknown";
        }
    }
    
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
    
    // Advanced features
    bool explorationEnabled_;
    float explorationThreshold_;
    float rewardScale_;
    float developmentSpeed_;
    std::vector<float> neuromodulationWeights_;
    std::vector<float> currentGoal_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
