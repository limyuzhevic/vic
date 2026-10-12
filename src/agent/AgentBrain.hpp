#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction, motor decoding, and neuromodulation
// Provides advanced configuration and extensibility for complex agent behaviors
class AgentBrain {
public:
    // Configuration options for advanced agent behavior
    struct Config {
        bool rewardModulationEnabled = true;
        bool structuralPlasticityEnabled = true;
        bool developmentEnabled = true;
        bool curiosityEnabled = true;
        bool verboseLogging = false;
        
        // Curiosity parameters
        float curiosityThreshold = 0.3f;
        float explorationFactor = 0.3f;
        
        // Reward modulation parameters
        float rewardPredictionDecay = 0.95f;
        float dopamineSensitivity = 1.0f;
        
        // Development parameters
        double initialDevelopmentalAge = 0.0;
        float baselinePlasticityModifier = 1.0f;
        
        // Novelty detection
        float noveltySmoothingFactor = 0.99f;
        float noveltyThreshold = 0.1f;
        
        // Custom callbacks for advanced behavior
        std::function<void(float)> onCuriosityUpdate;
        std::function<void(float, float)> onRewardProcessed;
        std::function<void(const std::vector<float>&)> onSensoryProcessed;
    };

    // Specialized event types for agent behavior
    enum class EventType {
        RewardReceived,
        NoveltyDetected,
        CuriosityUpdate,
        DevelopmentalStageChange,
        LearningMilestone,
        BehaviorChange
    };
    
    // Event callback type
    using EventCallback = std::function<void(EventType, float, const std::string&)>;

    // Constructor with enhanced configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config{});
    ~AgentBrain();
    
    // Initialize with world specification
    // Requires: world must be properly configured before initialization
    // Effect: Sets up internal sensory buffers and neuron mappings
    void initialize(const SimpleWorld& world);
    
    // Process sensory input from environment
    // Validates input dimensions and handles sensor calibration
    // Returns: true if successfully processed, false on error
    bool processSensoryInput(const SensoryPercept& percept);
    
    // Decode motor command from neural activity
    // Applies safety checks and returns default command if no clear action
    MotorCommand decodeMotorCommand();
    
    // Apply reward-based neuromodulation with prediction error
    // Updates dopamine levels and eligibility traces for synaptic plasticity
    void applyRewardModulation(float reward, float predictedReward = 0.0f);
    
    // Advance development system based on simulated time
    // Updates plasticity modifiers and developmental stage
    void updateDevelopment(double timestep);
    
    // Get current developmental stage for behavioral adaptation
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get current neuromodulation level (dopamine equivalent)
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    
    // Get curiosity level driving exploration behavior
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Get novelty level indicating environmental change detection
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Get prediction error for reward-based learning
    float getPredictionError() const { return predictionError_; }
    
    // Get expected reward for reinforcement learning
    float getExpectedReward() const { return expectedReward_; }
    
    // Reset agent state for new episode or testing
    // Clears all internal state and prepares for fresh interaction
    void reset();
    
    // Get brain pointer for advanced operations
    // WARNING: Direct manipulation bypasses safety checks
    Brain* getBrainUnsafe() { return brain_.get(); }
    const Brain* getBrainUnsafe() const { return brain_.get(); }
    
    // Get brain pointer with null check
    Brain* getBrain() { return brain_ ? brain_.get() : nullptr; }
    const Brain* getBrain() const { return brain_ ? brain_.get() : nullptr; }
    
    // Configuration management methods
    void configure(const Config& config);
    const Config& getConfig() const { return config_; }
    
    // Enable/disable specific systems
    void enableSystem(const std::string& systemName, bool enable);
    bool isSystemEnabled(const std::string& systemName) const;
    
    // Advanced motor command options
    MotorCommand getEmergencyCommand() const; // Fallback for safety
    MotorCommand getDefaultCommand() const;   // Neutral behavior
    MotorCommand getExploreCommand() const;   // Maximize exploration
    
    // Developmental stage helpers
    bool isInCriticalPeriod() const;
    bool isMature() const;
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Novelty and curiosity management
    void setNoveltyThreshold(float threshold) { config_.noveltyThreshold = threshold; }
    void setCuriosityThreshold(float threshold) { config_.curiosityThreshold = threshold; }
    
    // Event system for external monitoring
    void addEventCallback(EventCallback callback);
    void removeEventCallback(EventCallback callback);
    void emitEvent(EventType type, float value, const std::string& message = "");
    
    // Statistics and diagnostics
    struct PerformanceStats {
        uint64_t totalSteps = 0;
        float totalRewards = 0.0f;
        float averageCuriosity = 0.0f;
        float averageNovelty = 0.0f;
        uint64_t motorCommandsIssued = 0;
        std::vector<MotorCommand> commandHistory;
    };
    
    PerformanceStats getStats() const;
    void resetStats();
    
    // Sensory statistics
    size_t getVisionNeurons() const { return sensoryVision_.size(); }
    size_t getTouchNeurons() const { return sensoryTouch_.size(); }
    size_t getInternalNeurons() const { return sensoryInternal_.size(); }
    size_t getProprioceptionNeurons() const { return sensoryProprioception_.size(); }
    size_t getTotalMotorNeurons() const {
        return motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size();
    }
    
    // Quality of life and convenience methods
    bool isHealthy() const; // Checks for critical internal state
    std::string getStatusSummary() const; // Human-readable status
    
private:
    // Internal implementation details
    void setupNeuronGroups();
    void setupSensoryBuffers(const SimpleWorld& world);
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    void updateEventSystem();
    void updateStats();
    
    std::shared_ptr<Brain> brain_;
    Config config_;
    
    // Motor neuron groups (organized by action type)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    
    // Sensory neuron groups (organized by modality)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> sensoryAudio_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags (legacy - use config_ for new code)
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Event system
    std::vector<EventCallback> eventCallbacks_;
    
    // Performance statistics
    mutable PerformanceStats stats_;
    uint64_t lastStepCount_;
};

} // namespace nlm
