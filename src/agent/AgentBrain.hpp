#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced features
class AgentBrain {
public:
    // Configuration structure for fine-tuning behavior
    struct Configuration {
        // Sensory processing parameters
        float visionScale_ = 5.0f;           // Vision signal scaling
        float touchScale_ = 8.0f;           // Touch signal scaling  
        float internalScale_ = 5.0f;        // Internal signal scaling
        float proprioceptionScale_ = 3.0f; // Proprioception scaling
        
        // Novelty detection parameters
        float visionNoveltyWeight_ = 0.4f;  // Vision contribution to novelty
        float touchNoveltyWeight_ = 0.3f;   // Touch contribution to novelty
        float internalNoveltyWeight_ = 0.2f; // Internal contribution to novelty
        float proprioceptionNoveltyWeight_ = 0.1f; // Proprioception contribution
        float noveltyDecay_ = 0.99f;        // Novelty decay rate
        
        // Motor decoding parameters
        float actionThreshold_ = 0.5f;      // Minimum activity for action
        float curiosityThreshold_ = 0.3f;   // Minimum curiosity for exploration
        float explorationRate_ = 0.3f;      // Max exploration probability
        
        // Neuromodulation parameters
        float rewardPredictionWeight_ = 0.05f; // How fast expected reward updates
        float plasticityFactorRange_ = 2.0f;   // Range of plasticity modulation
        float dopamineClamp_ = 1.0f;           // Dopamine signal clamping
        
        // Development parameters
        double developmentalStageInitial_ = 60.0;     // Age when development starts
        double developmentalStageCritical_ = 300.0;   // Age of critical period end
        double developmentalStageMaturation_ = 900.0; // Age of maturation end
        
        // Advanced configuration
        bool useAdvancedGrouping_ = true;     // Use intelligent neuron grouping
        bool multiModalNovelty_ = true;       // Use all sensory modalities for novelty
        bool adaptiveThresholds_ = false;     // Adapt thresholds based on context
        std::string neuronGroupingStrategy_ = "behavioral"; // Grouping strategy
        
        Configuration() = default;
        
        // Validate configuration
        void validate() const {
            // Ensure weights sum to 1.0
            float totalWeight = visionNoveltyWeight_ + touchNoveltyWeight_ + 
                               internalNoveltyWeight_ + proprioceptionNoveltyWeight_;
            if (std::abs(totalWeight - 1.0f) > 0.01f) {
                // Normalize weights
                float scale = 1.0f / totalWeight;
                visionNoveltyWeight_ *= scale;
                touchNoveltyWeight_ *= scale;
                internalNoveltyWeight_ *= scale;
                proprioceptionNoveltyWeight_ *= scale;
            }
        }
    };

public:
    AgentBrain(std::shared_ptr<Brain> brain, const Configuration& config = Configuration());
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
    
    // Get current configuration
    const Configuration& getConfiguration() const { return config_; }
    
    // Configure neuron grouping strategy
    void configureNeuronGrouping(const std::string& strategy);
    
    // Get comprehensive statistics
    std::map<std::string, float> getComprehensiveStats() const;
    
    // Analyze novelty across all sensory modalities
    float analyzeSensoryNovelty(const SensoryPercept& percept) const;
    
    // Plan action based on current state
    Action planAction(const SensoryPercept& percept);
    
    // Enable/disable advanced features
    void enableAdvancedFeatures(bool enable) { advancedFeaturesEnabled_ = enable; }
    bool areAdvancedFeaturesEnabled() const { return advancedFeaturesEnabled_; }
    
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
    
    // Get current configuration
    const Configuration& getConfig() const { return config_; }
    
    // Set custom configuration
    void setConfiguration(const Configuration& config) { 
        config_ = config; 
        config_.validate();
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Intelligent neuron grouping strategies
    void groupMotorNeuronsIntelligent();
    void groupSensoryNeuronsIntelligent();
    void distributeMotorNeuronsByRegion();
    void distributeSensoryNeuronsByRegion();
    
    // Advanced novelty calculation
    float calculateMultiModalNovelty(const SensoryPercept& percept) const;
    float calculateVisionNovelty(const std::vector<float>& current, const std::vector<float>& previous) const;
    float calculateTouchNovelty(const std::vector<float>& current) const;
    float calculateInternalNovelty(const std::vector<float>& current) const;
    float calculateProprioceptionNovelty(const std::vector<float>& current) const;
    
    // Adaptive threshold calculation
    float getAdaptiveActionThreshold() const;
    float getAdaptiveCuriosityThreshold() const;
    
    // Performance optimization
    void optimizeNeuronAccess();
    float calculateNeuralActivityOptimized(const std::vector<Neuron*>& neurons) const;
    
    // State management
    void updatePerformanceMetrics() const;
    void decayNoveltyLevels();
    
    std::shared_ptr<Brain> brain_;
    Configuration config_;
    
    // Motor neuron groups (organized by function)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (organized by modality)
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
    float novelAccumulation_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    float performanceScore_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool advancedFeaturesEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    std::vector<float> previousTouch_;
    std::vector<float> previousInternal_;
    std::vector<float> previousProprioception_;
    
    // Performance metrics
    mutable size_t neuronAccessCount_;
    mutable float lastActivityCalculationTime_;
    mutable float totalActivityScore_;
    
    // Random state for exploration
    mutable bool randomStateInitialized_;
    
    // Helper function for weighted random selection
    MotorCommand weightedRandomAction(const std::map<MotorCommand, float>& probabilities) const;
};

} // namespace nlm
