// Agent brain utility functions and helper classes
// Extracted from AgentBrain.cpp for better maintainability

#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Motor group classification and management
class AgentMotorGroups {
public:
    // Motor group types for agent action selection
    enum class MotorGroup {
        Forward,
        Backward,
        TurnLeft,
        TurnRight,
        Interact,
        Wait,
        Count
    };
    
    // Distribution strategy for motor neurons
    enum class DistributionStrategy {
        RoundRobin,
        Weighted,
        Balanced,
        Random
    };
    
    AgentMotorGroups() = default;
    
    // Distribute motor neurons into action groups
    void distributeMotorNeurons(const std::vector<Neuron*>& motorNeurons,
                               DistributionStrategy strategy = DistributionStrategy::RoundRobin);
    
    // Get neurons for a specific motor action
    const std::vector<Neuron*>& getMotorGroup(MotorGroup group) const;
    
    // Get all motor groups
    const std::vector<std::vector<Neuron*>>& getAllMotorGroups() const;
    
    // Get group index for action type
    static MotorGroup motorCommandToGroup(MotorCommand cmd);
    
    // Calculate group statistics
    struct GroupStats {
        size_t neuronCount;
        float averageFiringRate;
        float totalActivity;
        float dominanceScore;
    };
    
    GroupStats getGroupStats(MotorGroup group) const;
    
    // Get overall group distribution
    std::vector<GroupStats> getAllGroupStats() const;
    
    // Reset all groups
    void clearGroups();
    
    // Get total neuron count across all groups
    size_t getTotalNeuronCount() const;
    
    // Check if groups are properly initialized
    bool isInitialized() const;
    
private:
    std::vector<std::vector<Neuron*>> motorGroups_;
    std::unordered_map<MotorGroup, size_t> groupIndex_;
    bool initialized_ = false;
};

// Sensory neuron classification and management
class AgentSensoryGroups {
public:
    enum class SensoryGroup {
        Vision,
        Touch,
        Internal,
        Proprioception,
        Count
    };
    
    AgentSensoryGroups() = default;
    
    // Distribute sensory neurons into processing groups
    void distributeSensoryNeurons(const std::vector<Neuron*>& sensoryNeurons,
                                 const std::string& distribution = "balanced");
    
    // Get neurons for a specific sensory modality
    const std::vector<Neuron*>& getSensoryGroup(SensoryGroup group) const;
    
    // Get all sensory groups
    const std::vector<std::vector<Neuron*>>& getAllSensoryGroups() const;
    
    // Get group name for display
    static const char* sensoryGroupName(SensoryGroup group);
    
    // Calculate sensory group characteristics
    struct GroupCharacteristics {
        float sensitivity;
        float tuningWidth;
        float baselineActivity;
        float dynamicRange;
    };
    
    GroupCharacteristics getGroupCharacteristics(SensoryGroup group) const;
    
    // Get overall sensory profile
    std::vector<GroupCharacteristics> getAllCharacteristics() const;
    
    // Reset all groups
    void clearGroups();
    
    // Get total sensory neuron count
    size_t getTotalSensoryCount() const;
    
    // Check initialization status
    bool isInitialized() const;
    
private:
    std::vector<std::vector<Neuron*>> sensoryGroups_;
    std::unordered_map<SensoryGroup, size_t> groupIndex_;
    bool initialized_ = false;
};

// Lateral inhibition system for motor group competition
class AgentLateralInhibition {
public:
    struct InhibitionParams {
        float inhibitionStrength;      // Base inhibition pulse strength
        float recoveryTime;             // Time for inhibition to decay
        float threshold;                // Activity threshold for inhibition
        float noveltySensitivity;       // How strongly novelty affects inhibition
        float winnerLockout;            // How long winners are protected from inhibition
    };
    
    AgentLateralInhibition() = default;
    
    // Configure inhibition parameters
    void configure(const InhibitionParams& params);
    
    // Apply lateral inhibition based on current group activities
    void applyInhibition(const std::vector<float>& groupActivities,
                        const std::vector<MotorGroups::MotorGroup>& groupOrder);
    
    // Get current inhibition state
    const std::vector<float>& getInhibitionState() const;
    
    // Reset inhibition system
    void resetInhibition();
    
    // Check if inhibition is currently active
    bool isInhibitionActive() const;
    
    // Get inhibition strength for a specific group
    float getInhibitionStrength(size_t groupIndex) const;
    
private:
    std::vector<float> inhibitionState_;
    InhibitionParams params_;
    bool active_ = false;
    float decayTimer_ = 0.0f;
};

// Internal neuron group management
class AgentInternalNeurons {
public:
    enum class InternalNeuronType {
        Cognitive,
        Memory,
        Executive,
        Emotional,
        Count
    };
    
    AgentInternalNeurons() = default;
    
    // Classify internal neurons based on their properties
    void classifyInternalNeurons(const std::vector<Neuron*>& internalNeurons);
    
    // Get neurons for a specific internal type
    const std::vector<Neuron*>& getInternalGroup(InternalNeuronType type) const;
    
    // Get all internal groups
    const std::vector<std::vector<Neuron*>>& getAllInternalGroups() const;
    
    // Calculate internal group dynamics
    struct GroupDynamics {
        float excitationLevel;
        float adaptationRate;
        float refractoryPeriod;
        float plasticityRate;
    };
    
    GroupDynamics getGroupDynamics(InternalNeuronType type) const;
    
    // Get overall internal system state
    std::vector<GroupDynamics> getAllDynamics() const;
    
    // Reset all groups
    void clearGroups();
    
    // Get total internal neuron count
    size_t getTotalInternalCount() const;
    
    // Check initialization status
    bool isInitialized() const;
    
private:
    stdاصر
    std::vector<std::vector<Neuron*>> internalGroups_;
    std::unordered_map<InternalNeuronType, size_t> groupIndex_;
    bool initialized_ = false;
};

// Agent brain system coordinator
class AgentBrainSystems {
public:
    AgentBrainSystems() = default;
    
    // Initialize all agent systems
    void initialize(
        const std::vector<Neuron*>& motorNeurons,
        const std::vector<Neuron*>& sensoryNeurons,
        const std::vector<Neuron*>& internalNeurons
    );
    
    // Update all systems
    void update(float timestep);
    
    // Get system status
    struct SystemStatus {
        bool motorInitialized;
        bool sensoryInitialized;
        bool internalInitialized;
        bool lateralInhibitionActive;
        float overallActivity;
        float systemStability;
    };
    
    SystemStatus getStatus() const;
    
    // Get system parameters
    struct SystemParameters {
        AgentMotorGroups::DistributionStrategy motorDistribution;
        std::string sensoryDistribution;
        AgentLateralInhibition::InhibitionParams inhibitionParams;
        float updateInterval;
    };
    
    SystemParameters getParameters() const;
    
    // Configure system parameters
    void configure(const SystemParameters& params);
    
    // Reset all systems
    void reset();
    
    // Check if systems are ready for operation
    bool isReady() const;
    
private:
    AgentMotorGroups motorGroups_;
    AgentSensoryGroups sensoryGroups_;
    AgentInternalNeurons internalGroups_;
    AgentLateralInhibition lateralInhibition_;
    bool ready_ = false;
};

} // namespace nlm