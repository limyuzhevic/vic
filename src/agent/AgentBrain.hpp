// ================================================
// IMPROVED AGENT BRAIN WITH BETTER CODE ORGANIZATION
// ================================================

#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Constants.hpp"  // NEW: Named constants for improved configuration
#include <memory>
#include <vector>

namespace nlm {

// ================================================
// AGENT BRAIN CLASS - IMPROVED VERSION
// ================================================
//
// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with improved organization
//
// KEY IMPROVEMENTS:
// - Uses named constants for all magic numbers
// - Better neuron categorization with efficient helper functions
// - Improved motor command decoding with structured activity calculation
// - Enhanced neuromodulation with configurable parameters
// - Better developmental stages with clear timing constants
// - Improved curiosity and exploration system
//
// IMPROVEMENTS OVER ORIGINAL:
// 1. Eliminated magic numbers throughout the code
// 2. More efficient neuron distribution algorithms
// 3. Better separation of concerns with helper functions
// 4. Enhanced configurability through Constants namespace
// 5. Improved performance through pre-allocation and efficient algorithms
// 6. Better documentation and code organization
// 7. More maintainable and extensible design
//

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving for better performance
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world and configure sensory systems
    void initialize(const SimpleWorld& world);
    
    // Get sensory input sizes using named constants
    size_t getSensoryInputSize() const;
    
    // Get motor output size using named constant
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain using improved scaling
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command with curiosity
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward with enhanced configurability
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system with named developmental stages
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation levels
    float getNeuromodulationLevel() const;
    
    // Get cognitive state levels
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Reset agent for new episode with complete state reset
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods for advanced users
    // These allow fine-tuning of agent behavior
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Configuration status checks
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Enhanced motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Enhanced motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups organized by action type
    // Uses constants from Constants.hpp for better configurability
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups organized by modality
    // Uses constants from Constants.hpp for better configurability
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state with improved initialization
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state with named constants
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags for fine-tuning agent behavior
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection with named decay rate
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Helper function for efficient neuron categorization
    // Uses deterministic algorithms for better performance
    static void categorizeMotorNeurons(
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait,
        std::vector<Neuron*>& allMotorNeurons);
    
    // Helper function for sensory neuron categorization
    static void categorizeSensoryNeurons(
        std::vector<Neuron*>& visionNeurons,
        std::vector<Neuron*>& touchNeurons,
        std::vector<Neuron*>& internalNeurons,
        std::vector<Neuron*>& proprioceptionNeurons,
        std::vector<Neuron*>& allSensoryNeurons);
    
    // Helper function for motor activity calculation
    static float calculateMotorActivity(const std::vector<Neuron*>& neurons);
};

} // namespace nlm
