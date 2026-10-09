#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for brain components
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

/**
 * @brief AgentBrain: Connects NLM brain to the world
 * 
 * Improved AgentBrain architecture with better encapsulation and reduced state complexity.
 * This class handles sensory transduction and motor decoding while maintaining clean separation of concerns.
 */
class AgentBrain {
public:
    // Constructor with simplified parameter passing
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // Public interface with controlled access
    void initialize(const SimpleWorld& world);
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Status checks
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // State access (read-only for external users)
    float getNeuromodulationLevel() const { return neuromodulationState_.dopamine; }
    float getCuriosityLevel() const { return neuromodulationState_.curiosity; }
    float getNoveltyLevel() const { return neuromodulationState_.novelty; }
    float getPredictionError() const { return neuromodulationState_.predictionError; }
    double getDevelopmentalAge() const { return neuromodulationState_.developmentalAge; }
    float getPlasticityModifier() const { return neuromodulationState_.plasticityModifier; }
    
    // Core functionality
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Reset and cleanup
    void reset();
    
    // Internal brain access (for advanced users)
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
private:
    // Simplified brain interface
    std::shared_ptr<Brain> brain_;
    
    // Structured neuromodulation state (encapsulated)
    struct NeuromodulationState {
        float dopamine{0.0f};
        float curiosity{0.0f};
        float novelty{0.0f};
        float predictionError{0.0f};
        float expectedReward{0.0f};
        double developmentalAge{0.0};
        float plasticityModifier{1.0f};
        
        NeuromodulationState() : dopamine(0.0f), curiosity(0.0f), novelty(0.0f),
                               predictionError(0.0f), expectedReward(0.0f),
                               developmentalAge(0.0), plasticityModifier(1.0f) {}
    };
    
    NeuromodulationState neuromodulationState_;
    
    // Motor neuron groups (structured)
    struct MotorGroups {
        std::vector<Neuron*> forward;
        std::vector<Neuron*> backward;
        std::vector<Neuron*> turnLeft;
        std::vector<Neuron*> turnRight;
        std::vector<Neuron*> interact;
        std::vector<Neuron*> wait;
        
        MotorGroups() : forward{}, backward{}, turnLeft{}, turnRight{}, interact{}, wait{} {}
        
        // Helper method to get all groups
        std::vector<std::vector<Neuron*>> getAllGroups() const {
            return {forward, backward, turnLeft, turnRight, interact, wait};
        }
    };
    
    MotorGroups motorGroups_;
    
    // Sensory neuron groups (structured)
    struct SensoryGroups {
        std::vector<Neuron*> vision;
        std::vector<Neuron*> touch;
        std::vector<Neuron*> internal;
        std::vector<Neuron*> proprioception;
        
        SensoryGroups() : vision{}, touch{}, internal{}, proprioception{} {}
        
        // Helper method to get all groups
        std::vector<std::vector<Neuron*> > getAllGroups() const {
            return {vision, touch, internal, proprioception};
        }
    };
    
    SensoryGroups sensoryGroups_;
    
    // Configuration flags
    bool rewardModulationEnabled_{true};
    bool structuralPlasticityEnabled_{true};
    bool developmentEnabled_{true};
    bool curiosityEnabled_{true};
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_{0.99f};
    
    // Private helper methods
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    void updateNovelty(const std::vector<float>& currentVision);
};

} // namespace nlm
