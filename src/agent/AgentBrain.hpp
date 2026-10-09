#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
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
    
    // ========== AGENT UTILITIES ==========
    
    // Create default agent brain interface
    static std::shared_ptr<AgentBrain> create_default_agent_brain(std::shared_ptr<Brain> brain);
    
    // Process sensory input
    void process_sensory_input(const SensoryPercept& percept);
    
    // Decode motor command
    MotorCommand decode_motor_command();
    
    // Run single episode
    void run_episode(int steps);
    
    // Run complete agent simulation
    float run_agent_simulation(int steps);
    
    // Get agent performance metrics
    std::string get_agent_metrics() const;
    
    // Validate agent configuration
    bool validate_agent() const;
    
    // Enable all agent features
    void enable_all_features();
    
    // Disable all agent features
    void disable_all_features();
    
    // Check if agent is ready for simulation
    bool is_ready() const;
    
    // Get agent status string
    std::string get_status() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decode_from_motor_neurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand select_with_curiosity(MotorCommand default_cmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motor_forward_;
    std::vector<Neuron*> motor_backward_;
    std::vector<Neuron*> motor_turn_left_;
    std::vector<Neuron*> motor_turn_right_;
    std::vector<Neuron*> motor_interact_;
    std::vector<Neuron*> motor_wait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensory_vision_;
    std::vector<Neuron*> sensory_touch_;
    std::vector<Neuron*> sensory_internal_;
    std::vector<Neuron*> sensory_proprioception_;
    
    // Neuromodulation state
    float dopamine_level_;
    float novelty_level_;
    float curiosity_level_;
    float prediction_error_;
    float expected_reward_;
    
    // Development state
    double developmental_age_;
    float plasticity_modifier_;
    
    // Configuration flags
    bool reward_modulation_enabled_;
    bool structural_plasticity_enabled_;
    bool development_enabled_;
    bool curiosity_enabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previous_vision_;
    float sensory_novelty_decay_;
};

} // namespace nlm
