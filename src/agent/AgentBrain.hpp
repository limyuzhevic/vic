#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// =============================================================================
// AgentBrain: NLM Brain-to-World Interface
// =============================================================================
// 
// This class serves as the interface layer between the NLM brain and the external
// world, handling sensory transduction (world → brain) and motor decoding 
// (brain → world). It implements the complete agent control loop including
// perception, decision making, and action execution.
// 
// Key Responsibilities:
// - Sensory transduction: Convert world percepts to neural inputs
// - Motor decoding: Convert neural activity to world commands
// - Neuromodulation: Apply reward and error signals to plasticity
// - Development management: Track and update developmental state
// - Curiosity-driven exploration: Balance exploitation vs exploration
// - Novelty detection: Identify new experiences for learning
// - Action selection: Choose optimal behaviors based on context
// 
// The agent operates in closed-loop with the world:
// 1. World generates percepts (sensory input)
// 2. AgentBrain processes percepts and injects into brain
// 3. Brain processes inputs and generates motor commands
// 4. AgentBrain decodes commands and applies to world
// 5. World provides feedback (reward, novel events)
// 6. Neuromodulation updates learning based on feedback
// 7. Development progresses based on experience
// 
// Usage Example:
// ```
// #include "AgentBrain.hpp"
// 
// // Create brain
// auto config = std::make_shared<Config>();
// auto brain = std::make_shared<Brain>(config);
// 
// // Create agent interface
// AgentBrain agent(brain);
// 
// // Create and configure world
// SimpleWorld world;
// world.configure(20, 20, 8, 8);
// 
// // Initialize agent with world
// agent.initialize(world);
// 
// // Configure agent behavior
// agent.enableRewardModulation(true);
// agent.enableCuriosity(true);
// agent.enableDevelopment(true);
// agent.enableStructuralPlasticity(true);
// 
// // Run control loop
// for (int step = 0; step < 1000; ++step) {
//     // World updates
//     world.update(0.1);
//     
//     // Get sensory percept from world
//     SensoryPercept percept = world.getSensoryPercept();
//     
//     // Process sensory input
//     agent.processSensoryInput(percept);
//     
//     // Brain step (handled internally)
//     
//     // Get motor command
//     MotorCommand cmd = agent.decodeMotorCommand();
//     
//     // Apply action to world
//     world.applyMotorCommand(cmd, world.getSimulationTime());
//     
//     // Get reward and apply neuromodulation
//     float reward = 0.0f;  // Would extract from percept
//     agent.applyRewardModulation(reward, 0.0f);
//     
//     // Update development
//     agent.updateDevelopment(0.1);
//     
//     // Print debug info
//     if (step % 100 == 0) {
//         std::cout << "Step " << step << ": "
//                   << "Curiosity: " << agent.getCuriosityLevel()
//                   << " Novelty: " << agent.getNoveltyLevel()
//                   << " Dopamine: " << agent.getNeuromodulationLevel()
//                   << std::endl;
//     }
// }
// ```
// =============================================================================

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
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
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
