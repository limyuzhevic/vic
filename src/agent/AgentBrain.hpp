#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <array>

namespace nlm {

// Forward declarations for cleaner headers
class Neuron;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with improved separation of concerns
class AgentBrain {
public:
    // Constructor with brain dependency injection
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    // Destructor
    ~AgentBrain();
    
    // Delete copy operations (managed by unique_ptr)
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    // Initialize with world - configures sensory mapping
    void initialize(const SimpleWorld& world);
    
    // Configuration methods
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    // Get configuration status
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Main interface methods
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    
    // Query methods
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Access to underlying brain
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Advanced configuration presets
    void configureForGoalOrientedBehavior();
    void configureForExploration();
    void configureForExploitation();
    
    // Performance optimization
    void optimizeForSpeed();
    void optimizeForAccuracy();
    
    // Statistical methods
    float getAverageSensoryResponse() const;
    float getMotorConfidence(MotorCommand cmd) const;
    
private:
    // === Core Components ===
    std::shared_ptr<Brain> brain_;
    
    // === Neuron Group Management ===
    struct NeuronGroups {
        std::vector<Neuron*> vision;
        std::vector<Neuron*> touch;
        std::vector<Neuron*> internal;
        std::vector<Neuron*> proprioception;
        
        std::vector<Neuron*> forward;
        std::vector<Neuron*> backward;
        std::vector<Neuron*> turnLeft;
        std::vector<Neuron*> turnRight;
        std::vector<Neuron*> interact;
        std::vector<Neuron*> wait;
    };
    
    NeuronGroups neuronGroups_;
    
    // === State Management ===
    struct State {
        float dopamineLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float predictionError = 0.0f;
        float expectedReward = 0.0f;
        double developmentalAge = 0.0;
        float plasticityModifier = 1.0f;
        
        // Performance metrics
        size_t stepsProcessed = 0;
        size_t sensoryInputEvents = 0;
        size_t motorCommandsProduced = 0;
        
        // Previous state for novelty detection
        std::vector<float> previousVision;
    };
    
    State state_;
    
    // === Configuration ===
    struct Configuration {
        bool rewardModulationEnabled = true;
        bool structuralPlasticityEnabled = true;
        bool developmentEnabled = true;
        bool curiosityEnabled = true;
        
        // Performance settings
        bool optimizeForSpeed = false;
        bool optimizeForAccuracy = false;
        
        // Advanced behavior settings
        float curiosityThreshold = 0.3f;
        float explorationProbability = 0.2f;
    };
    
    Configuration config_;
    
    // === Novelty Detection ===
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    
    // === Performance Optimization ===
    static constexpr size_t VISION_NEURON_GROUP_SIZE = 256;
    static constexpr size_t TOUCH_NEURON_GROUP_SIZE = 8;
    static constexpr size_t INTERNAL_NEURON_GROUP_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_NEURON_GROUP_SIZE = 6;
    static constexpr size_t MOTOR_GROUP_SIZE = 6;
    
    // === Private Implementation Methods ===
    
    // Sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprioception);
    
    // Motor decoding
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Neuromodulation
    void updateNeuromodulationState();
    void applyDopamineEffects();
    void updateCuriosityBasedOnNovelty();
    
    // Development
    void updateDevelopmentalStage();
    void updateStructuralPlasticity();
    
    // State management
    void initializeNoveltyDetection(const SimpleWorld& world);
    float calculateNovelty(const std::vector<float>& currentVision) const;
    
    // Performance helpers
    float calculateAverageResponse(const std::vector<Neuron*>& neurons) const;
    float calculateMotorConfidence(const std::vector<Neuron*>& neurons) const;
    
    // Configuration helpers
    void applyPresetConfiguration(const std::string& preset);
    void validateConfiguration() const;
    
    // Utility methods
    template<typename T>
    float normalizeValue(T value, T min, T max) const {
        return (static_cast<float>(value) - static_cast<float>(min)) / 
               std::max(static_cast<float>(max - min), 1.0f);
    }
    
    // Debug and monitoring
    void logPerformanceMetrics() const;
    void resetPerformanceCounters();
};

} // namespace nlm
