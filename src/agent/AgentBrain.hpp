#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace nlm {

// Forward declarations for configuration
struct SensoryMapping {
    std::string name;
    std::vector<Neuron*> neurons;
    float scaleFactor;
    float minValue;
    float maxValue;
};

struct MotorMapping {
    MotorCommand command;
    std::vector<Neuron*> neurons;
    float activityThreshold;
    float decayRate;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with improved configurability
class AgentBrain {
public:
    // Configuration structure for agent behavior
    struct Config {
        // Sensory mapping configuration
        std::unordered_map<std::string, SensoryMapping> sensoryMappings;
        
        // Motor mapping configuration
        std::vector<MotorMapping> motorMappings;
        
        // Scaling factors for different sensory modalities
        struct ScaleFactors {
            float vision;      // Default: 5.0f
            float touch;       // Default: 8.0f
            float internal;    // Default: 2.0f
            float proprioception; // Default: 3.0f
        } scaleFactors;
        
        // Developmental parameters
        struct DevelopmentParams {
            double initialStageAge;    // Age when stage 1 ends (default: 60.0)
            double criticalPeriodEnd;   // Age when stage 2 ends (default: 300.0)
            double maturationEnd;       // Age when stage 3 ends (default: 900.0)
            float initialPlasticity;   // Plasticity modifier for stage 1 (default: 1.0f)
            float criticalPeriodPlasticity; // For stage 2 (default: 0.8f)
            float maturationPlasticity;    // For stage 3 (default: 0.5f)
            float adultPlasticity;     // For adult stage (default: 0.2f)
        } development;
        
        // Novelty detection parameters
        struct NoveltyParams {
            float decayRate;        // Exponential decay (default: 0.99f)
            float noveltyThreshold; // Threshold for curiosity activation (default: 0.3f)
            float curiosityWeight;  // Weight of novelty in curiosity (default: 2.0f)
            float errorWeight;      // Weight of prediction error in curiosity (default: 0.5f)
        } novelty;
        
        // Curiosity parameters
        struct CuriosityParams {
            float explorationThreshold; // Curiosity level that triggers exploration (default: 0.5f)
            float maxExplorationChance; // Max random exploration probability (default: 0.3f)
            float minExplorationChance; // Min random exploration probability (default: 0.05f)
        } curiosity;
        
        // Reward modulation parameters
        struct RewardParams {
            float eligibilityTraceDecay; // Decay rate for eligibility traces (default: 0.1f)
            float eligibilityThreshold;  // Minimum trace magnitude to apply (default: 0.001f)
            float plasticityModulation;  // Dopamine effect on plasticity (default: 0.5f)
        } reward;
        
        Config() {
            // Initialize with sensible defaults
            scaleFactors.vision = 5.0f;
            scaleFactors.touch = 8.0f;
            scaleFactors.internal = 2.0f;
            scaleFactors.proprioception = 3.0f;
            
            development.initialStageAge = 60.0;
            development.criticalPeriodEnd = 300.0;
            development.maturationEnd = 900.0;
            development.initialPlasticity = 1.0f;
            development.criticalPeriodPlasticity = 0.8f;
            development.maturationPlasticity = 0.5f;
            development.adultPlasticity = 0.2f;
            
            novelty.decayRate = 0.99f;
            novelty.noveltyThreshold = 0.3f;
            novelty.curiosityWeight = 2.0f;
            novelty.errorWeight = 0.5f;
            
            curiosity.explorationThreshold = 0.5f;
            curiosity.maxExplorationChance = 0.3f;
            curiosity.minExplorationChance = 0.05f;
            
            reward.eligibilityTraceDecay = 0.1f;
            reward.eligibilityThreshold = 0.001f;
            reward.plasticityModulation = 0.5f;
            
            // Create default sensory mappings
            SensoryMapping visionMapping;
            visionMapping.name = "vision";
            visionMapping.scaleFactor = 5.0f;
            visionMapping.minValue = 0.0f;
            visionMapping.maxValue = 10.0f;
            
            SensoryMapping touchMapping;
            touchMapping.name = "touch";
            touchMapping.scaleFactor = 8.0f;
            touchMapping.minValue = -10.0f;
            touchMapping.maxValue = 10.0f;
            
            SensoryMapping internalMapping;
            internalMapping.name = "internal";
            internalMapping.scaleFactor = 2.0f;
            internalMapping.minValue = -10.0f;
            internalMapping.maxValue = 10.0f;
            
            SensoryMapping proprioceptionMapping;
            proprioceptionMapping.name = "proprioception";
            proprioceptionMapping.scaleFactor = 3.0f;
            proprioceptionMapping.minValue = -5.0f;
            proprioceptionMapping.maxValue = 5.0f;
            
            sensoryMappings["vision"] = visionMapping;
            sensoryMappings["touch"] = touchMapping;
            sensoryMappings["internal"] = internalMapping;
            sensoryMappings["proprioception"] = proprioceptionMapping;
            
            // Create default motor mappings
            MotorMapping moveForward;
            moveForward.command = MotorCommand::MoveForward;
            moveForward.activityThreshold = 0.5f;
            moveForward.decayRate = 0.9f;
            
            MotorMapping moveBackward;
            moveBackward.command = MotorCommand::MoveBackward;
            moveBackward.activityThreshold = 0.5f;
            moveBackward.decayRate = 0.9f;
            
            MotorMapping turnLeft;
            turnLeft.command = MotorCommand::TurnLeft;
            turnLeft.activityThreshold = 0.5f;
            turnLeft.decayRate = 0.9f;
            
            MotorMapping turnRight;
            turnRight.command = MotorCommand::TurnRight;
            turnRight.activityThreshold = 0.5f;
            turnRight.decayRate = 0.9f;
            
            MotorMapping interact;
            interact.command = MotorCommand::Interact;
            interact.activityThreshold = 0.5f;
            interact.decayRate = 0.9f;
            
            MotorMapping wait;
            wait.command = MotorCommand::Wait;
            wait.activityThreshold = 0.5f;
            wait.decayRate = 0.9f;
            
            motorMappings = {moveForward, moveBackward, turnLeft, turnRight, interact, wait};
        }
    };
    
    AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config());
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
    
    // Get current configuration
    const Config& getConfig() const { return config_; }
    
    // Update configuration
    void updateConfig(const Config& newConfig);
    
    // Validate configuration
    bool validateConfig() const;
    
    // Get current developmental age
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    // Get current plasticity modifier
    float getPlasticityModifier() const { return plasticityModifier_; }
    
private:
    // Error handling helper methods
    void validateBrainPointer() const;
    void validatePerceptDimensions(const SensoryPercept& percept) const;
    
    // Helper methods for sensory processing
    void processVisionInput(const std::vector<float>& vision, size_t& processedCount);
    void processTouchInput(const std::vector<float>& touch, size_t& processedCount);
    void processInternalInput(const std::vector<float>& internal, size_t& processedCount);
    void processProprioceptionInput(const std::vector<float>& proprio, size_t& processedCount);
    
    // Motor decoding helper methods
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // State management
    void resetNeuromodulationState();
    void updateDevelopmentStage();
    void computeNovelty(const std::vector<float>& vision);
    void updateCuriosityBasedOnNovelty();
    
    // Configuration and mappings
    std::shared_ptr<Brain> brain_;
    Config config_;
    
    // Current sensory and motor mappings (derived from config)
    std::vector<MotorMapping*> activeMotorMappings_;
    std::vector<SensoryMapping*> activeSensoryMappings_;
    
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
    
    // Performance optimization
    mutable bool mappingsInitialized_;
};

} // namespace nlm
