#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Neuron distribution strategy enum
enum class DistributionStrategy {
    LOAD_BALANCED,    // Even distribution across groups
    SPATIAL,          // Spatial-based distribution
    TOPOLOGICAL,      // Topological organization
    WEIGHTED          // Weighted by action frequency/importance
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world, std::shared_ptr<const Config> config);
    
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
    
    // Set distribution strategy
    void setDistributionStrategy(DistributionStrategy strategy) { distributionStrategy_ = strategy; }
    void setMotorGroupSizes(const std::unordered_map<MotorCommand, size_t>& sizes) { motorGroupSizes_ = sizes; }
    void setSensoryGroupSizes(const std::unordered_map<std::string, size_t>& sizes) { sensoryGroupSizes_ = sizes; }
    
    // Configuration access methods for API standardization
    std::shared_ptr<const Config> getConfig() const { return config_; }
    void setConfig(std::shared_ptr<const Config> config) { config_ = config; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Resource management methods
    void clearMotorNeuronGroups();
    void clearSensoryNeuronGroups();
    void cleanupResources();
    
    // Distribution strategy access
    DistributionStrategy getDistributionStrategy() const { return distributionStrategy_; }
    const std::unordered_map<MotorCommand, size_t>& getMotorGroupSizes() const { return motorGroupSizes_; }
    const std::unordered_map<std::string, size_t>& getSensoryGroupSizes() const { return sensoryGroupSizes_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Distribution methods
    void distributeMotorNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void distributeSensoryNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void loadBalanceDistribution(std::vector<Neuron*>& targetGroup, const std::vector<Neuron*>& source);
    void spatialDistribution(const std::vector<Neuron*>& sourceNeurons, std::vector<Neuron*>& visionGroup,
                           std::vector<Neuron*>& touchGroup, std::vector<Neuron*>& internalGroup,
                           std::vector<Neuron*>& proprioceptionGroup);
    void topologicalDistribution(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                               const std::vector<Neuron*>& neurons);
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<const Config> config_;  // Added for API standardization
    
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
    
    // Distribution configuration
    DistributionStrategy distributionStrategy_;
    std::unordered_map<MotorCommand, size_t> motorGroupSizes_;
    std::unordered_map<std::string, size_t> sensoryGroupSizes_;
};

} // namespace nlm
