#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    // Disable moving
    AgentBrain(AgentBrain&&) = delete;
    AgentBrain& operator=(AgentBrain&&) = delete;
    
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
    
    // Brain access methods
    Brain* getBrain() const { return brain_.get(); }
    const std::shared_ptr<const Config>& getBrainConfig() const;
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Statistics and monitoring
    float getDopamineLevel() const { return dopamineLevel_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    
private:
    // Core initialization and setup
    void initializeNeuronGroups();
    
    // Motor system components
    struct MotorGroups {
        std::vector<Neuron*> forward;
        std::vector<Neuron*> backward;
        std::vector<Neuron*> turnLeft;
        std::vector<Neuron*> turnRight;
        std::vector<Neuron*> interact;
        std::vector<Neuron*> wait;
        
        MotorGroups() : forward(), backward(), turnLeft(), turnRight(), interact(), wait() {}
        
        size_t totalCount() const {
            return forward.size() + backward.size() + turnLeft.size() + 
                   turnRight.size() + interact.size() + wait.size();
        }
    };
    
    struct SensoryGroups {
        std::vector<Neuron*> vision;
        std::vector<Neuron*> touch;
        std::vector<Neuron*> internal;
        std::vector<Neuron*> proprioception;
        
        SensoryGroups() : vision(), touch(), internal(), proprioception() {}
        
        size_t totalCount() const {
            return vision.size() + touch.size() + internal.size() + proprioception.size();
        }
    };
    
    // Motor decoding
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Processing helper methods
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void computeNovelty(const std::vector<float>& vision);
    void updateCuriosityBasedOnNovelty();
    
    // Reward and neuromodulation
    void updateRewardModulatedWeights();
    void modulatePlasticityBasedOnDopamine();
    
    // Development system
    void updateDevelopmentalStage();
    void updateDevelopmentPlasticityRates();
    
    // Neuron group distribution
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons);
    
    // Validation
    void validateBrainState() const;
    
    // Configuration
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<const Config> brainConfig_;
    
    // Neuron groups (encapsulated for efficient access)
    MotorGroups motorGroups_;
    SensoryGroups sensoryGroups_;
    
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
    
    // Performance optimization fields
    bool initialized_;
    std::string lastErrorMessage_;
};

} // namespace nlm
