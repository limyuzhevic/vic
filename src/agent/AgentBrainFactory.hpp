#pragma once

// AgentBrainFactory: Factory for creating configurable AgentBrain instances
// Provides pre-configured agent profiles and customization capabilities

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace nlm {

enum class AgentProfile {
    Default,           // Balanced behavior for general use
    Explorer,          // High curiosity, frequent exploration
    Worker,            // Focused on task completion, lower curiosity
    Social,            // Enhanced social interaction capabilities
    Conservative,      // Low risk, stable behavior
    Aggressive,        // High motivation, rapid learning
    Adaptive,          // Changes strategy based on environment
    Custom             // Fully customizable parameters
};

class AgentBrainFactory {
public:
    AgentBrainFactory() = default;
    ~AgentBrainFactory() = default;
    
    // Create an agent with default configuration
    static std::unique_ptr<AgentBrain> createAgent(std::shared_ptr<Brain> brain, 
                                                   const SimpleWorld& world,
                                                   AgentProfile profile = AgentProfile::Default);
    
    // Create an agent with custom parameters
    static std::unique_ptr<AgentBrain> createCustomAgent(std::shared_ptr<Brain> brain,
                                                         const SimpleWorld& world,
                                                         const AgentConfig& config);
    
    // Get agent configuration template for a profile
    static AgentConfig getProfileConfig(AgentProfile profile);
    
    // Get all available profile names
    static std::vector<std::string> getAvailableProfiles() {
        return {"default", "explorer", "worker", "social", "conservative", "aggressive", "adaptive", "custom"};
    }
    
    // Modify existing agent configuration
    static void applyConfiguration(std::unique_ptr<AgentBrain>& agent, 
                                   const AgentConfig& config);
    
    // Get recommended configuration for specific use case
    static AgentConfig getRecommendedConfig(const std::string& useCase);
    
    // Validate agent configuration
    static bool validateConfig(const AgentConfig& config, std::string& errorMessage);

private:
    // Predefined agent profiles
    static std::map<AgentProfile, AgentConfig> createProfileConfigs();
    
    // Default agent configuration
    struct AgentConfig {
        // Motor neuron distribution
        struct MotorDistribution {
            float forward = 0.25f;    // 25% to forward
            float backward = 0.25f;   // 25% to backward  
            float turnLeft = 0.125f;  // 12.5% to turn left
            float turnRight = 0.125f; // 12.5% to turn right
            float interact = 0.125f;  // 12.5% to interact
            float wait = 0.125f;      // 12.5% to wait
        } motorDistribution;
        
        // Sensory neuron distribution
        struct SensoryDistribution {
            float vision = 0.5f;      // 50% to vision
            float touch = 0.25f;      // 25% to touch
            float internal = 0.125f;  // 12.5% to internal
            float proprioception = 0.125f; // 12.5% to proprioception
        } sensoryDistribution;
        
        // Neuromodulation settings
        struct Neuromodulation {
            bool enableRewardModulation = true;
            bool enableStructuralPlasticity = true;
            bool enableDevelopment = true;
            bool enableCuriosity = true;
            
            float curiosityThreshold = 0.3f;     // Curiosity becomes active above this level
            float explorationProbability = 0.2f; // Chance of random exploration
            float dopamineLearningRate = 0.01f;
            float noveltyDecayRate = 0.99f;
        } neuromodulation;
        
        // Development settings
        struct Development {
            double developmentalStageDuration = 60.0; // Time per developmental stage in seconds
            float initialPlasticity = 1.0f;         // Starting plasticity level
            float finalPlasticity = 0.2f;           // Adult plasticity level
            bool enableMaturation = true;
            bool enablePruning = true;
        } development;
        
        // Learning and adaptation settings
        struct Learning {
            float learningRate = 0.01f;
            float memoryRetention = 0.8f;
            size_t workingMemoryCapacity = 1000;
            size_t episodicMemoryMaxEpisodes = 1000;
            float predictionAccuracyThreshold = 0.7f;
        } learning;
        
        // Behavior settings
        struct Behavior {
            MotorCommand defaultAction = MotorCommand::Wait;
            float activityThreshold = 0.5f;      // Minimum activity to trigger action
            float fatigueThreshold = 0.9f;       // High activity causes fatigue
            float recoveryRate = 0.1f;            // Recovery from fatigue
            bool enableSelfRegulation = true;
        } behavior;
        
        // Constructor
        AgentConfig() = default;
    };
};

} // namespace nlm

(End of file - total 202 lines)