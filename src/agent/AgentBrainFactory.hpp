#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class AgentBrain;

// Factory for creating neuron groups and distributions
class AgentBrainFactory {
public:
    AgentBrainFactory();
    ~AgentBrainFactory();
    
    // Create and initialize neuron groups based on brain configuration
    static void initializeNeuronGroups(
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait,
        std::vector<Neuron*>& sensoryVision,
        std::vector<Neuron*>& sensoryTouch,
        std::vector<Neuron*>& sensoryInternal,
        std::vector<Neuron*>& sensoryProprioception,
        const Brain& brain
    );
    
    // Get neuron distribution ratios based on configuration
    static NeuronDistribution getNeuronDistribution(const Brain& brain);
    
    // Calculate optimal neuron allocation for a given population size
    static std::vector<size_t> calculateOptimalAllocation(
        size_t totalNeurons,
        const std::vector<float>& weights
    );
    
    // Create sensory neuron groups from brain populations
    static void createSensoryGroups(
        std::vector<Neuron*>& visionGroup,
        std::vector<Neuron*>& touchGroup,
        std::vector<Neuron*>& internalGroup,
        std::vector<Neuron*>& proprioceptionGroup,
        const Brain& brain
    );
    
    // Create motor neuron groups from brain populations
    static void createMotorGroups(
        std::vector<Neuron*>& forwardGroup,
        std::vector<Neuron*>& backwardGroup,
        std::vector<Neuron*>& leftGroup,
        std::vector<Neuron*>& rightGroup,
        std::vector<Neuron*>& interactGroup,
        std::vector<Neuron*>& waitGroup,
        const Brain& brain
    );
    
    // Validate that groups are properly initialized
    static bool validateInitialization(
        const std::vector<Neuron*>& visionGroup,
        const std::vector<Neuron*>& touchGroup,
        const std::vector<Neuron*>& internalGroup,
        const std::vector<Neuron*>& proprioceptionGroup,
        const std::vector<std::vector<Neuron*>>& motorGroups
    );
    
    // Get configuration for neuron distribution
    static AgentBrainConfig getConfig() { return config_; }
    
    // Update configuration
    static void updateConfig(const AgentBrainConfig& newConfig);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    static AgentBrainConfig config_;
};

// Neuron distribution weights for balanced allocation
struct NeuronDistribution {
    float visionWeight = 0.4f;      // Vision neurons
    float touchWeight = 0.2f;       // Touch neurons
    float internalWeight = 0.1f;    // Internal state neurons
    float proprioceptionWeight = 0.3f;  // Proprioception neurons
    
    float motorForwardWeight = 0.25f;    // Move forward
    float motorBackwardWeight = 0.15f;   // Move backward
    float motorTurnLeftWeight = 0.15f;   // Turn left
    float motorTurnRightWeight = 0.15f;  // Turn right
    float motorInteractWeight = 0.15f;   // Interact
    float motorWaitWeight = 0.05f;       // Wait
    
    // Normalize weights
    void normalize();
    
    // Validate weights sum to approximately 1.0
    bool isValid() const;
};

// Configuration for AgentBrain factory
struct AgentBrainConfig {
    size_t maxVisionNeurons = 256;
    size_t maxTouchNeurons = 8;
    size_t maxInternalNeurons = 4;
    size_t maxProprioceptionNeurons = 6;
    
    size_t maxMotorForwardNeurons = 50;
    size_t maxMotorBackwardNeurons = 30;
    size_t maxMotorTurnLeftNeurons = 30;
    size_t maxMotorTurnRightNeurons = 30;
    size_t maxMotorInteractNeurons = 30;
    size_t maxMotorWaitNeurons = 10;
    
    // Scaling factor for configuration changes
    float scalingFactor = 1.0f;
    
    // Enable/disable specific neuron types
    bool enableVisionNeurons = true;
    bool enableTouchNeurons = true;
    bool enableInternalNeurons = true;
    bool enableProprioceptionNeurons = true;
    bool enableAllMotorNeurons = true;
    
    // Distribution weights (overrides defaults if specified)
    NeuronDistribution distributionWeights;
    
    // Update configuration based on brain size
    void updateFromBrain(const Brain& brain);
    
    // Scale configuration for different brain sizes
    void scaleForBrainSize(size_t neuronCount);
};

} // namespace nlm