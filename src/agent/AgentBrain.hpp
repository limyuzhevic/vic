#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration methods
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = std::max(0.0f, std::min(threshold, 1.0f)); }
    void setExplorationRate(float rate) { explorationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    void setNoveltyDecay(float decay) { sensoryNoveltyDecay_ = std::clamp(decay, 0.0f, 1.0f); }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = std::clamp(modifier, 0.0f, 10.0f); }
    
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationRate() const { return explorationRate_; }
    float getNoveltyDecay() const { return sensoryNoveltyDecay_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Performance optimization methods
    void enableCaching(bool enable) { useCaching_ = enable; }
    bool isCachingEnabled() const { return useCaching_; }
    
    // Statistics and debugging
    size_t getMotorNeuronsCount() const;
    size_t getSensoryNeuronsCount() const;
    float getLastNovelty() const { return lastNoveltyComputed_; }
    
    // Advanced control methods
    void setAgentName(const std::string& name) { agentName_ = name; }
    const std::string& getAgentName() const { return agentName_; }
    
    // Performance monitoring
    double getProcessingTime() const { return processingTime_; }
    uint64_t getTotalSensoryProcessed() const { return totalSensoryProcessed_; }
    uint64_t getTotalMotorDecoded() const { return totalMotorDecoded_; }
    
    // State management
    std::string getStateSummary() const;
    
    // Debug and analysis
    void printDebugInfo() const;
    void enableDebugOutput(bool enable) { debugOutputEnabled_ = enable; }
    bool isDebugOutputEnabled() const { return debugOutputEnabled_; }
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Optimized motor decoding with pre-computed indices
    MotorCommand decodeOptimized();
    
    // Helper methods for performance
    float computeMotorActivation(const std::vector<Neuron*>& neurons) const;
    MotorCommand findMaxActivityCommand(const std::vector<float>& activities) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - now with smart indexing
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Pre-computed motor neuron group indices for fast access
    std::array<size_t, 6> motorGroupSizes_;
    bool motorGroupsInitialized_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Pre-computed sensory neuron group indices
    std::array<size_t, 4> sensoryGroupSizes_;
    bool sensoryGroupsInitialized_;
    
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
    bool useCaching_;
    
    // Advanced configuration
    float curiosityThreshold_;
    float explorationRate_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance optimization fields
    float lastNoveltyComputed_;
    std::vector<float> cachedVisionDifferences_;
    mutable bool noveltyCacheValid_;
    
    // Validation and utility methods
    void validateNeuronGroups() const;
    void validateBrainPtr() const;
    void logConfiguration() const;
};

} // namespace nlm
