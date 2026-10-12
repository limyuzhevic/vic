#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <array>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration constants for advanced users
    static constexpr size_t NUM_MOTOR_ACTIONS = 6;
    static constexpr size_t NUM_SENSORY_MODALITIES = 4;
    static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    static constexpr float CURIOUS_THRESHOLD = 0.3f;
    static constexpr float HIGH_CURIOUS_THRESHOLD = 0.5f;
    static constexpr float EXPLORATION_MAX_CHANCE = 0.3f;
    
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration methods
    void setMotorNeuronDistribution(const std::array<size_t, NUM_MOTOR_ACTIONS>& distribution);
    void setSensoryNeuronDistribution(const std::array<size_t, NUM_SENSORY_MODALITIES>& distribution);
    
    // Utility methods for advanced users
    const std::vector<Neuron*>& getMotorNeurons(MotorCommand action) const;
    const std::vector<Neuron*>& getSensoryNeurons(size_t modality) const;
    size_t getMotorNeuronCount() const;
    size_t getSensoryNeuronCount() const;
    
    // Get access to individual motor neuron groups (for advanced users)
    const std::vector<Neuron*>& getMotorGroup(MotorCommand action) const {
        size_t idx = 0;
        for (; idx < NUM_MOTOR_ACTIONS; ++idx) {
            if (MOTOR_ACTIONS[idx] == action) {
                break;
            }
        }
        return motorGroups_[idx];
    }
    
    // Get all sensory neuron groups (for advanced users)
    const std::array<std::vector<Neuron*>, NUM_SENSORY_MODALITIES>& getAllSensoryGroups() const {
        return sensoryGroups_;
    }
    
    // Get all motor neuron groups (for advanced users)
    const std::array<std::vector<Neuron*>, NUM_MOTOR_ACTIONS>& getAllMotorGroups() const {
        return motorGroups_;
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods for improved code organization
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    float calculateAverageActivity(const std::vector<Neuron*>& neurons) const;
    MotorCommand findCommandWithMaxActivity(const std::array<MotorCommand, NUM_MOTOR_ACTIONS>& commands, 
                                           const std::array<float, NUM_MOTOR_ACTIONS>& activities) const;
    float computeNovelty(const std::vector<float>& currentVision) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (using array for better organization)
    std::array<std::vector<Neuron*>, NUM_MOTOR_ACTIONS> motorGroups_;
    static constexpr std::array<MotorCommand, NUM_MOTOR_ACTIONS> MOTOR_ACTIONS = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward, MotorCommand::TurnLeft,
        MotorCommand::TurnRight, MotorCommand::Interact, MotorCommand::Wait
    };
    
    // Sensory neuron groups (using array for better organization)
    std::array<std::vector<Neuron*>, NUM_SENSORY_MODALITIES> sensoryGroups_;
    static constexpr std::array<size_t, NUM_SENSORY_MODALITIES> SENSORY_MODALITIES = {0, 1, 2, 3}; // Vision, Touch, Internal, Proprioception
    
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
};

} // namespace nlm
